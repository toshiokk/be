/**************************************************************************
 *   cursesif.c - CURSES interface                                        *
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

#ifdef ENABLE_NCURSES

PRIVATE WINDOW *curses_win;

PRIVATE short get_color_pair(char bgc, char fgc);
PRIVATE void init_color_pairs();
PRIVATE key_code_t getch_();

//------------------------------------------------------------------------------
PRIVATE char curses_enabled = 0;
PRIVATE struct termios term_settings_saved;	// The user's original term settings
PRIVATE char curses_bgc = 0;
PRIVATE char curses_fgc = 7;
PRIVATE char curses_rev = 0;
PRIVATE short curses_color_pair = -1;

int curses_init()
{
	// ncurses shows only 16 grayscale monochrome colors if "TERM=xterm-256color".
	// so change it to "TERM=linux" or "TERM=xterm"
///	setenv("TERM", "linux", 1);
	setenv("TERM", "xterm", 1);
	curses_bgc = 0;
	curses_fgc = 7;
	curses_rev = 0;
	curses_color_pair = -1;
	save_term_settings(&term_settings_saved);
	return 0;
}
int curses_begin()
{
	set_term_no_intr();
	set_term_raw();
	curses_win = initscr();
	nonl();
	cbreak();
	noecho();
	raw();
	keypad(curses_win, TRUE);
	nodelay(curses_win, TRUE);
	ESCDELAY = 1;
	init_color_pairs();
	curses_enabled = 1;
	return 0;
}
int curses_end()
{
	keypad(curses_win, TRUE);
	nocbreak();
	echo();
	endwin();
	restore_term_settings(&term_settings_saved);
	curses_enabled = 0;
	return 0;
}
//------------------------------------------------------------------------------
void curses_set_screen_size(int lines, int columns)
{
	LINES = lines;
	COLS = columns;
}
int curses_get_lines()
{
	return LINES;
}
int curses_get_columns()
{
	return LIM_MAX(MAX_SCRN_COLS, COLS);
}
//------------------------------------------------------------------------------
PRIVATE short get_color_pair(char bgc, char fgc)
{
	if ((GET_BASE_COLOR(bgc) == CL_BK) && (GET_BASE_COLOR(fgc) == CL_BK)) {
		fgc = CL_GY;	// (0, 0) ==> (0, 7)
	}
	return CP_FROM_CBF(bgc, fgc);	// [1, COLOR_PAIRS-1]
}

PRIVATE void init_color_pairs()
{
	start_color();
	for (int bgc = 0; bgc < COLORS; bgc++) {
		for (int fgc = 0; fgc < COLORS; fgc++) {
			init_pair(get_color_pair(bgc, fgc), fgc, bgc);
		}
	}
}

void curses_set_attrs(int bgc, int fgc, int rev)
{
	if (! curses_enabled)
		return;
	if (bgc >= 0) {
		curses_bgc = bgc;
	}
	if (fgc >= 0) {
		curses_fgc = fgc;
	}
	if (rev >= 0) {
		curses_rev = rev;
	}
	if (curses_rev == 0) {
		bgc = curses_bgc;
		fgc = curses_fgc;
	} else {
		// reverse ==> swap foreground and background color
		bgc = curses_fgc;
		fgc = curses_bgc;
	}
	if (fgc >= CL_HI) {
		wattron(curses_win, A_BOLD);	// only foreground have lighter color
	} else {
		wattroff(curses_win, A_BOLD);
	}
	short color_pair = get_color_pair(bgc, fgc);
	if (curses_color_pair != color_pair) {
		wattron(curses_win, COLOR_PAIR(color_pair));
	}
	curses_color_pair = color_pair;
}
void curses_set_cursor_on(int on_off)
{
	if (! curses_enabled)
		return;
	curs_set(on_off);
}
PRIVATE int curses_cursor_yy = 0;
PRIVATE int curses_cursor_xx = 0;
void curses_set_cursor_pos(int yy, int xx)
{
	if (! curses_enabled)
		return;
	curses_cursor_yy = yy;
	curses_cursor_xx = xx;
	wmove(curses_win, yy, xx);
}
void curses_get_cursor_pos(int *yy, int *xx)
{
	if (yy) {
		*yy = curses_cursor_yy;
	}
	if (xx) {
		*xx = curses_cursor_xx;
	}
}

void curses_clear_screen()
{
	if (! curses_enabled)
		return;
	clear();
}
void curses_output_string(int yy, int xx, const char *string, int bytes)
{
	if (! curses_enabled)
		return;
	if (yy >= 0 && xx >= 0)
		curses_set_cursor_pos(yy, xx);
	waddnstr(curses_win, (char *)string, bytes);
}
void curses_beep()
{
	if (! curses_enabled)
		return;
	beep();
}
void curses_refresh()
{
	if (! curses_enabled)
		return;
	refresh();
}
//------------------------------------------------------------------------------
key_code_t curses_input_key()
{
	key_code_t key1;
	key_code_t key2;

	if ((key1 = getch_()) < 0) {
		return key1;
	}
	if (key1 == CHAR_ESC) {
		if ((key2 = getch_()) >= 0) {
			key1 = KEY_META(key2);	// 0x1b, 0xXX ==> 0x1bXX
		}
	}
	return key1;
}

PRIVATE key_code_t getch_()
{
	int key = getch();
	return key;
}

#endif // ENABLE_NCURSES

// End of cursesif.c
