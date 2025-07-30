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

PRIVATE int get_color_pair(int bgc, int fgc);
PRIVATE int init_color_pairs();
PRIVATE key_code_t getch_();

//------------------------------------------------------------------------------
PRIVATE struct termios term_settings_save;	/* The user's original term settings */
PRIVATE char curses_bgc = 0;
PRIVATE char curses_fgc = 7;
PRIVATE char curses_rev = 0;
PRIVATE char curses_color_pair = -1;
PRIVATE char curses_cur_color_pair = -1;

int curses_init()
{
	curses_bgc = 0;
	curses_fgc = 7;
	curses_rev = 0;
	curses_color_pair = -1;
	curses_cur_color_pair = -1;
	save_term_settings(&term_settings_save);
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
	return 0;
}
int curses_end()
{
	keypad(curses_win, TRUE);
	nocbreak();
	echo();
	endwin();
	restore_term_settings(&term_settings_save);
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
PRIVATE int get_color_pair(int bgc, int fgc)
{
	if ((GET_BASE_COLOR(bgc) == CL_BK) && (GET_BASE_COLOR(fgc) == CL_BK)) {
		fgc = CL_GY;	// (0, 0) ==> (0, 7)
	}
	return CP_FROM_CBF(bgc, fgc);	// [1, COLOR_PAIRS-1]
}

PRIVATE int init_color_pairs()
{
	int fgc, bgc;

	start_color();
	for (bgc = 0; bgc < COLORS; bgc++) {
		for (fgc = 0; fgc < COLORS; fgc++) {
			init_pair(get_color_pair(bgc, fgc), fgc, bgc);
		}
	}
	return 0;
}

void curses_set_attrs(int bgc, int fgc, int rev)
{
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
		wattron(curses_win, A_BOLD);	// only foreground(character) color will be lighted
	} else {
		wattroff(curses_win, A_BOLD);
	}
	curses_color_pair = get_color_pair(bgc, fgc);
	if (curses_cur_color_pair != curses_color_pair) {
		wattron(curses_win, COLOR_PAIR(curses_color_pair));
	}
	curses_cur_color_pair = curses_color_pair;
}
void curses_set_cursor_on(int on_off)
{
	curs_set(on_off);
}
PRIVATE int curses_cursor_yy = 0;
PRIVATE int curses_cursor_xx = 0;
void curses_set_cursor_pos(int yy, int xx)
{
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
	clear();
}
void curses_output_string(int yy, int xx, const char *string, int bytes)
{
	if (yy >= 0 && xx >= 0)
		curses_set_cursor_pos(yy, xx);
	waddnstr(curses_win, (char *)string, bytes);
}
void curses_beep()
{
	beep();
}
void curses_refresh()
{
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
