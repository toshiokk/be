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
PRIVATE char tio_blank_line_2_buf[MAX_SCRN_LINE_BUF_LEN+1] = "";	// A blank line " "

#ifdef START_UP_TEST
void tio_test(void)
{
	int yy = 0;
	int xx = 0;
	int bgc_fgc = 0;
	key_code_t key;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"
#define BUF_LEN		100
	char buf[BUF_LEN+1];

	tio_init();

	tio_fill_screen(0);
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
			tio_fill_screen(0);
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

//-----------------------------------------------------------------------------
PRIVATE int tio_initialized = FALSE;
//-----------------------------------------------------------------------------
int tio_is_initialized(void)
{
	return tio_initialized;
}
int tio_init(void)
{
	strnset__(tio_blank_line_buf, ' ', MAX_SCRN_LINE_BUF_LEN);
	///strnset__(tio_blank_line_2_buf, '_', MAX_SCRN_LINE_BUF_LEN);
	utf8s_strnset__(tio_blank_line_2_buf, " ", MAX_SCRN_LINE_BUF_LEN);
#ifdef ENABLE_NCURSES
	flf_d_printf("Using curses (cursesif)\n");
	curses_init();
#else // ENABLE_NCURSES
	flf_d_printf("Direct terminal control (termif)\n");
	termif_init();
#endif // ENABLE_NCURSES
	tio_begin();
	return 0;
}
int tio_destroy(void)
{
	tio_end();
	return 0;
}

int tio_begin(void)
{
#ifdef ENABLE_NCURSES
	curses_begin();
#else // ENABLE_NCURSES
	termif_begin();
#endif // ENABLE_NCURSES
	tio_initialized = TRUE;
	return 0;
}
int tio_end(void)
{
#ifdef ENABLE_NCURSES
	curses_end();
#else // ENABLE_NCURSES
	termif_end();
#endif // ENABLE_NCURSES
	tio_initialized = FALSE;
	return 0;
}

int tio_resize(void)
{
	// update screen size
	// re-initialize terminal interface to reflect new terminal size
	tio_end();
	tio_begin();
	return 0;
}
int tio_suspend(void)
{
	tio_end();
	return 0;
}
int tio_resume(void)
{
	tio_begin();
	return 0;
}

//-----------------------------------------------------------------------------

// How to know win size changed:
//  1. signal sigwinch()
// How to get win size:
//  1. ioctl TIOCGWINSZ
//  2. terminal set ("0x1b[999;999R") and get("\x1b[6n") cursor pos.
int tio_check_update_terminal_size(void)
{
	if (is_sigwinch_signaled()) {
mflf_d_printf("sigwinch_signaled\n");
		clear_sigwinch_signaled();
		tio_resize();
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------

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
int tio_get_lines(void)
{
#ifdef ENABLE_NCURSES
	return curses_get_lines();
#else // ENABLE_NCURSES
	return termif_get_lines();
#endif // ENABLE_NCURSES
}
int tio_get_columns(void)
{
#ifdef ENABLE_NCURSES
	return curses_get_columns();
#else // ENABLE_NCURSES
	return termif_get_columns();
#endif // ENABLE_NCURSES
}

//-----------------------------------------------------------------------------

int tio_differ_fgc_from_bgc(int bgc, int fgc)
{
	if (fgc == bgc) {
		// select different color as foreground color with background color
		// so that you can recognize character
		switch (fgc) {
		default:
		case CL_BK:		fgc = CL_DG;	break;
		case CL_RD:		fgc = CL_LR;	break;
		case CL_GR:		fgc = CL_LG;	break;
		case CL_BR:		fgc = CL_YL;	break;
		case CL_BL:		fgc = CL_LB;	break;
		case CL_MG:		fgc = CL_LM;	break;
		case CL_CY:		fgc = CL_LC;	break;
		case CL_GY:		fgc = CL_DG;	break;
		case CL_DG:		fgc = CL_BK;	break;
		case CL_LR:		fgc = CL_RD;	break;
		case CL_LG:		fgc = CL_GR;	break;
		case CL_YL:		fgc = CL_BR;	break;
		case CL_LB:		fgc = CL_BL;	break;
		case CL_LM:		fgc = CL_MG;	break;
		case CL_LC:		fgc = CL_CY;	break;
		case CL_WH:		fgc = CL_DG;	break;
		}
	}
#if 1 // Avoid similar fgc/bgc
	if ((bgc == CL_CY) && (fgc == CL_GR)) {
		fgc = CL_LG;
	}
	if ((bgc == CL_GR) && (fgc == CL_CY)) {
		fgc = CL_LC;
	}
	if ((bgc == CL_BR) && (fgc == CL_DG)) {
		fgc = CL_GY;
	}
#endif
	return fgc;
}

void tio_set_attrs(int bgc, int fgc, int rev)
{
#ifdef ENABLE_NCURSES
	curses_set_attrs(bgc, fgc, rev);
#else // ENABLE_NCURSES
	termif_set_attrs(bgc, fgc, rev);
#endif // ENABLE_NCURSES
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

void tio_clear_screen(void)
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
///	tio_set_attrs(CL_WH, CL_BK, 1);
///	tio_fill_screen(1);
///	MSLEEP(delay);
///	tio_set_attrs(CL_WH, CL_BK, 0);
///	tio_fill_screen(0);
}
void tio_fill_screen(int type)
{
	tio_fill_lines(0, tio_get_lines(), type);
	tio_refresh();
}
void tio_fill_lines(int line_1, int line_2, int type)
{
	for (int yy = line_1; yy < line_2; yy++) {
		tio_output_string(yy, 0, tio_blank_line(type), tio_get_columns());
	}
}
const char *tio_blank_line(int type)
{
	return (type == 0) ? tio_blank_line_buf : tio_blank_line_2_buf;
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

void tio_beep(void)
{
#ifdef ENABLE_NCURSES
	curses_beep();
#else // ENABLE_NCURSES
	termif_beep();
#endif // ENABLE_NCURSES
}

void tio_refresh(void)
{
#ifdef ENABLE_NCURSES
	curses_refresh();
#else // ENABLE_NCURSES
	termif_refresh();
#endif // ENABLE_NCURSES
}
void tio_repaint_all(void)
{
#ifdef ENABLE_NCURSES
	// no function
#else // ENABLE_NCURSES
	termif_clear_vscreen_painted();
#endif // ENABLE_NCURSES
}

key_code_t tio_input_key(void)
{
#ifdef ENABLE_NCURSES
	return curses_input_key();
#else // ENABLE_NCURSES
	return termif_input_key();
#endif // ENABLE_NCURSES
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_term_settings(char *msg, struct termios *term)
{
	char buf[NCCS*3+1];
	int ncc;

	for (ncc = 0; ncc < NCCS; ncc++) {
		snprintf(&buf[ncc * 3], 3+1, " %02x", term->c_cc[ncc]);
	}
	flf_d_printf("term_settings-%s i: %08lx o: %08lx c: %08lx l: %08lx\ncc:%s\n",
	 msg, term->c_iflag, term->c_oflag, term->c_cflag, term->c_lflag, buf);
}
#endif // ENABLE_DEBUG

void save_term_settings(struct termios *term_settings)
{
	/* First back up the old settings so they can be restored, duh */
	tcgetattr(STDIN_FILENO, term_settings);
}
void restore_term_settings(struct termios *term_settings)
{
	/* Restore the old term settings */
	tcsetattr(STDIN_FILENO, TCSANOW, term_settings);
}

// disable program termination by ^C
void set_term_no_intr(void)
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
void set_term_raw(void)
{
	struct termios term;
	struct termios term_raw;

	tcgetattr(STDIN_FILENO, &term);
	cfmakeraw(&term_raw);
	term.c_iflag = term_raw.c_iflag;
///	term.c_oflag = term_raw.c_oflag;
	term.c_cflag = term_raw.c_cflag;
	term.c_lflag = term_raw.c_lflag;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int init_stderr(void)
{
	return dup2(STDOUT_FILENO, STDERR_FILENO);
}

// End of tio.c
