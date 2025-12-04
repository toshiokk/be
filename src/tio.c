/**************************************************************************
 *   tio.c - terminal I/O                                                 *
 *                                                                        *
 *   Copyright (C) 2005-2018 PCR                                          *
 *                                                                        *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2, or (at your option)  *
 *   any later version.                                                   *
 *                                                                        *
 *   This program is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 *   GNU General Public License for more details.                         *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program; if not, write to the Free Software          *
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            *
 *                                                                        *
 **************************************************************************/

#include "headers.h"

PRIVATE char tio_blank_line_buf[MAX_SCRN_LINE_BUF_LEN+1] = "";		// A blank line ' '

#ifdef START_UP_TEST
void tio_test()
{
	int yy = 0;
	int xx = 0;
	int bgc_fgc = 0;
	key_code_t key;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"
#define BUF_LEN		100
	char buf[BUF_LEN+1];

	tio_init();

	tio_fill_screen();
	tio_set_cursor_on(1);
	tio_set_attrs(-1, -1, 0);
#ifdef ENABLE_NCURSES
	tio_output_string(0, 0, "          curses", -1);
#else // ENABLE_NCURSES
	tio_output_string(0, 0, "          termif", -1);
#endif // ENABLE_NCURSES
	tio_refresh();
	for ( ; ; ) {
		tio_set_cursor_pos(yy, xx);
		for ( ; ; ) {
			key = tio_input_key();
			if (key >= 0)
				break;
			MSLEEP(10);
		}
		switch (key) {
		case 'c':
			tio_fill_screen();
			break;
		case 'o':
			tio_set_cursor_on(1);
			break;
		case 'p':
			tio_set_cursor_on(0);
			break;
		case 'r':
			tio_set_attrs(-1, -1, 1);
			break;
		case 't':
			tio_set_attrs(-1, -1, 0);
			break;
		case 'h':
			tio_set_attrs(-1, -1, -1);
			break;
		case 'j':
			tio_set_attrs(-1, -1, -1);
			break;
		case 'f':
			bgc_fgc = 1;
			break;
		case 'g':
			bgc_fgc = 0;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			if (bgc_fgc == 0) {
				tio_set_attrs(key - '0', -1, -1);
			} else {
				tio_set_attrs(-1, key - '0', -1);
			}
			break;
		case 'b':
			tio_beep();
			break;
		case 's':
			xx--;
			break;
		case 'd':
			xx++;
			break;
		case 'e':
			yy--;
			break;
		case 'x':
			yy++;
			break;
		case 'q':
			break;
		default:
			break;
		}
		if (key == 'q')
			break;
		snprintf(buf, BUF_LEN+1, "%04x{%s}",
		 key, short_key_name_from_key_code(key, buf_key_name));
		tio_output_string(yy, xx, buf, -1);
	}

	tio_destroy();
	exit(0);
}
#endif // START_UP_TEST

//------------------------------------------------------------------------------
PRIVATE int tio_initialized = FALSE;
PRIVATE int high_bgc_enabled = FALSE;
//------------------------------------------------------------------------------
int tio_is_initialized()
{
	return tio_initialized;
}
int tio_init()
{
	strnset__(tio_blank_line_buf, ' ', MAX_SCRN_LINE_BUF_LEN);
#ifdef ENABLE_NCURSES
	flf_dprintf("Using curses (cursesif)\n");
	curses_init();
#else // ENABLE_NCURSES
	flf_dprintf("Direct terminal control (termif)\n");
	termif_init();
#endif // ENABLE_NCURSES
	o_printf("\r\nBE_EDITOR_A{{\r\n");
	MSLEEP(200);
	tio_begin();
	return 0;
}
int tio_destroy()
{
	tio_end();
	o_printf("\r\nBE_EDITOR_Z}}\r\n");
	return 0;
}

int tio_begin()
{
#ifdef ENABLE_NCURSES
	curses_begin();
#else // ENABLE_NCURSES
	termif_begin();
#endif // ENABLE_NCURSES
	tio_initialized = TRUE;
	return 0;
}
int tio_end()
{
#ifdef ENABLE_NCURSES
	curses_end();
#else // ENABLE_NCURSES
	termif_end();
#endif // ENABLE_NCURSES
	tio_initialized = FALSE;
	return 0;
}

void tio_enable_high_bgc(int enable)
{
	high_bgc_enabled = enable;
}
int tio_resize()
{
	// update screen size
	// re-initialize terminal interface to reflect new terminal size
#ifdef ENABLE_NCURSES
	curses_end();
	curses_begin();
#else // ENABLE_NCURSES
	termif_get_screen_size_from_term();
	termif_clear_screen();
#endif // ENABLE_NCURSES
	return 0;
}
int tio_suspend()
{
	tio_end();
	return 0;
}
int tio_resume()
{
	tio_begin();
	return 0;
}

//------------------------------------------------------------------------------

// How to know win size changed:
//  1. signal sigwinch()
// How to get win size:
//  1. ioctl TIOCGWINSZ
//  2. set cursor pos ("0x1b[999;999R") and get cursor pos ("\x1b[6n")
int tio_check_update_terminal_size()
{
	if (is_sigwinch_signaled()) {
hmflf_dprintf("sigwinch_signaled\n");
		clear_sigwinch_signaled();
		tio_resize();
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
void tio_set_screen_size(int lines, int columns)
{
#ifdef ENABLE_NCURSES
#define UPDATE_CURSES_COLS_LINES
#ifdef UPDATE_CURSES_COLS_LINES
	curses_set_screen_size(lines, columns);
#endif // UPDATE_CURSES_COLS_LINES
#else // ENABLE_NCURSES
	termif_set_screen_size(lines, columns);
#endif // ENABLE_NCURSES
}
int tio_get_lines()
{
#ifdef ENABLE_NCURSES
	return curses_get_lines();
#else // ENABLE_NCURSES
	return termif_get_lines();
#endif // ENABLE_NCURSES
}
int tio_get_columns()
{
#ifdef ENABLE_NCURSES
	return curses_get_columns();
#else // ENABLE_NCURSES
	return termif_get_columns();
#endif // ENABLE_NCURSES
}

//------------------------------------------------------------------------------
int cur_bgc = CL_BK;
int cur_fgc = CL_WH;
int cur_rev = 0;

void tio_set_attrs(int bgc, int fgc, int rev)
{
	if (bgc < 0) {
		bgc = cur_bgc;
	}
	if (fgc < 0) {
		fgc = cur_fgc;
	}
	if (rev < 0) {
		rev = cur_rev;
	}
	cur_bgc = bgc;
	cur_fgc = fgc;
	cur_rev = rev;
	tio_differentiate_fgc_from_bgc_rev(&bgc, &fgc, rev);
#ifdef ENABLE_NCURSES
	curses_set_attrs(bgc, fgc, rev);
#else // ENABLE_NCURSES
	termif_set_attrs(bgc, fgc, rev);
#endif // ENABLE_NCURSES
}
void tio_differentiate_fgc_from_bgc_rev(int *bgc, int *fgc, int rev)
{
	if (rev == 0) {
#ifndef ENABLE_HIGH_BGC
		*bgc = LIMIT_BGC8(*bgc);
#else // ENABLE_HIGH_BGC
		*bgc = (high_bgc_enabled == 0) ? LIMIT_BGC8(*bgc) : *bgc;
#endif // ENABLE_HIGH_BGC
		*fgc = LIMIT_FGC(*fgc);
		*fgc = tio_differentiate_fgc_from_bgc(*bgc, *fgc);
	} else {
		// apply differentiation to the swapped bgc and fgc
#ifndef ENABLE_HIGH_BGC
		*fgc = LIMIT_BGC8(*fgc);
#else // ENABLE_HIGH_BGC
		*fgc = (high_bgc_enabled == 0) ? LIMIT_BGC8(*fgc) : *fgc;
#endif // ENABLE_HIGH_BGC
		*bgc = LIMIT_FGC(*bgc);
		*bgc = tio_differentiate_fgc_from_bgc(*fgc, *bgc);
	}
}
int tio_differentiate_fgc_from_bgc(int bgc, int fgc)
{
	if (fgc == bgc) {
		// select different foreground color with background color
		// so that you can recognize character
		switch (bgc) {
		default:
		case CL_BK:		fgc = CL_DG;	break;
		case CL_RD:		fgc = CL_LR;	break;
		case CL_GR:		fgc = CL_LG;	break;
		case CL_BR:		fgc = CL_YL;	break;
		case CL_BL:		fgc = CL_LB;	break;
		case CL_MG:		fgc = CL_LM;	break;
		case CL_CY:		fgc = CL_LC;	break;
		case CL_GY:		fgc = CL_DG;	break;
#ifdef ENABLE_HIGH_BGC
		case CL_DG:		fgc = CL_GY;	break;
		case CL_LR:		fgc = CL_RD;	break;
		case CL_LG:		fgc = CL_GR;	break;
		case CL_YL:		fgc = CL_BR;	break;
		case CL_LB:		fgc = CL_BL;	break;
		case CL_LM:		fgc = CL_MG;	break;
		case CL_LC:		fgc = CL_CY;	break;
		case CL_WH:		fgc = CL_GY;	break;
#endif // ENABLE_HIGH_BGC
		}
	}
#if 1 // Avoid similar fgc/bgc
	if (((bgc == CL_RD) && (fgc == CL_MG))
	 || ((bgc == CL_MG) && (fgc == CL_RD))
	 || ((bgc == CL_CY) && (fgc == CL_GR))
	 || ((bgc == CL_GR) && (fgc == CL_CY))
	 || ((bgc == CL_BR) && (fgc == CL_DG))
	 || ((bgc == CL_DG) && (fgc == CL_BR))
#ifdef ENABLE_HIGH_BGC
	 || ((bgc == CL_LR) && (fgc == CL_LM))
	 || ((bgc == CL_LM) && (fgc == CL_LR))
	 || ((bgc == CL_LG) && (fgc == CL_LC))
	 || ((bgc == CL_LC) && (fgc == CL_LG))
	 || ((bgc == CL_WH) && (fgc == CL_YL))
	 || ((bgc == CL_YL) && (fgc == CL_WH))
#endif // ENABLE_HIGH_BGC
	) {
		// invert highlightness (dark color ==> high color, high color ==> dark color)
		fgc ^= CL_HI;
	}
#endif
	return fgc;
}
void tio_set_attr_rev(int rev)
{
	tio_set_attrs(-1, -1, rev);
}

void tio_set_cursor_on(int on_off)
{
#ifdef ENABLE_NCURSES
	curses_set_cursor_on(on_off);
#else // ENABLE_NCURSES
	termif_set_cursor_on(on_off);
#endif // ENABLE_NCURSES
}

void tio_set_cursor_pos(int yy, int xx)
{
#ifdef ENABLE_NCURSES
	curses_set_cursor_pos(yy, xx);
#else // ENABLE_NCURSES
	termif_set_cursor_pos(yy, xx);
#endif // ENABLE_NCURSES
}
void tio_get_cursor_pos(int *yy, int *xx)
{
#ifdef ENABLE_NCURSES
	curses_get_cursor_pos(yy, xx);
#else // ENABLE_NCURSES
	termif_get_cursor_pos(yy, xx);
#endif // ENABLE_NCURSES
}

void tio_clear_screen()
{
#ifdef ENABLE_NCURSES
	curses_clear_screen();
#else // ENABLE_NCURSES
	termif_clear_screen();
#endif // ENABLE_NCURSES
	tio_refresh();
}
void tio_flash_screen(int delay)
{
	tio_clear_screen();
	MSLEEP(delay);
}
void tio_fill_screen()
{
	tio_fill_lines(0, tio_get_lines());
	tio_refresh();
}
void tio_fill_lines(int line_1, int line_2)
{
	for (int yy = line_1; yy < line_2; yy++) {
		tio_output_string(yy, 0, tio_blank_line(), tio_get_columns());
	}
}
const char *tio_blank_line(int type)
{
	return tio_blank_line_buf;
}

void tio_output_string(int yy, int xx, const char *string, int bytes)
{
	if (bytes < 0) {
		bytes = byte_idx_from_col_idx(string, tio_get_columns() - xx, CHAR_LEFT, NULL);
	}
#ifdef ENABLE_NCURSES
	curses_output_string(yy, xx, string, bytes);
#else // ENABLE_NCURSES
	termif_output_string(yy, xx, string, bytes);
#endif // ENABLE_NCURSES
}

void tio_beep()
{
#ifdef ENABLE_NCURSES
	curses_beep();
#else // ENABLE_NCURSES
//PPP	termif_beep();
#endif // ENABLE_NCURSES
}

void tio_refresh()
{
_FLF_
#ifdef ENABLE_NCURSES
	curses_refresh();
#else // ENABLE_NCURSES
	termif_refresh();
#endif // ENABLE_NCURSES
}
void tio_repaint_all()
{
#ifdef ENABLE_NCURSES
	// no function
#else // ENABLE_NCURSES
	termif_clear_vscreen_painted();
#endif // ENABLE_NCURSES
}

key_code_t tio_input_key()
{
#ifdef ENABLE_NCURSES
	return curses_input_key();
#else // ENABLE_NCURSES
	return termif_input_key();
#endif // ENABLE_NCURSES
}

//------------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_term_settings(char *msg, struct termios *term)
{
	char buf[NCCS*3+1];
	int ncc;

	for (ncc = 0; ncc < NCCS; ncc++) {
		snprintf(&buf[ncc * 3], 3+1, " %02x", term->c_cc[ncc]);
	}
	flf_dprintf("term_settings-%s i: %08lx o: %08lx c: %08lx l: %08lx\ncc:%s\n",
	 msg, term->c_iflag, term->c_oflag, term->c_cflag, term->c_lflag, buf);
}
#endif // ENABLE_DEBUG

void save_term_settings(struct termios *term_settings)
{
	// First back up the old settings so that they can be restored later
	tcgetattr(STDIN_FILENO, term_settings);
}
void restore_term_settings(struct termios *term_settings)
{
	// Restore the old term settings
	tcsetattr(STDIN_FILENO, TCSANOW, term_settings);
}

// disable program termination by ^C
void set_term_no_intr()
{
#ifdef _POSIX_VDISABLE
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_cc[VINTR] = _POSIX_VDISABLE;
	term.c_cc[VQUIT] = _POSIX_VDISABLE;
	term.c_lflag &= ~IEXTEN;
	term.c_cc[VSTOP] = _POSIX_VDISABLE;
	term.c_cc[VSTART] = _POSIX_VDISABLE;
#ifdef VDSUSP
	term.c_cc[VDSUSP] = _POSIX_VDISABLE;
#endif // VDSUSP
	term.c_cc[VSUSP] = _POSIX_VDISABLE;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif // _POSIX_VDISABLE
}

// raw terminal mode (disable local echo, turn off cooked mode)
void set_term_raw()
{
	struct termios term;
	struct termios term_raw;

	tcgetattr(STDIN_FILENO, &term);
	cfmakeraw(&term_raw);
	term.c_iflag = term_raw.c_iflag;
	term.c_oflag = term_raw.c_oflag;
	term.c_cflag = term_raw.c_cflag;
	term.c_lflag = term_raw.c_lflag;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int init_stderr()
{
	return dup2(STDOUT_FILENO, STDERR_FILENO);
}

// End of tio.c
