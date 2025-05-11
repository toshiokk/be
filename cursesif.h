/**************************************************************************
 *   cursesif.h - CURSES interface                                        *
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

#ifndef cursesif_h
#define cursesif_h

#ifdef HAVE_CURSES_H
#include <curses.h> 
#endif
#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#endif
#ifdef HAVE_NCURSES_CURSES_H
#include <ncurses/curses.h> 
#endif
#ifdef HAVE_NCURSESW_CURSES_H
#include <ncursesw/curses.h> 
#endif

#define CP_FROM_CBF(cb, cf)		(((cb & 0x07) << 3) | (cf & 0x07))	// color pair from bgc and fgc

//------------------------------------------------------------------------------

int curses_init();
int curses_begin();
int curses_end();

void curses_set_screen_size(int lines, int columns);
int curses_get_lines();
int curses_get_columns();

void curses_clear_screen();
void curses_set_cursor_on(int on_off);
void curses_set_cursor_pos(int yy, int xx);
void curses_get_cursor_pos(int *yy, int *xx);

void curses_set_attrs(int bgc, int fgc, int rev);

void curses_output_string(int yy, int xx, const char *string, int bytes);
void curses_beep();
void curses_refresh();

key_code_t curses_input_key();

#endif // cursesif_h

// End of cursesif.h
