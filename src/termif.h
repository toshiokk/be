/**************************************************************************
 *   termif.h - Linux terminal interface                                  *
 *                                                                        *
 *   Copyright (C) 2020-2020 Toshio Koike                                 *
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

#ifndef termif_h
#define termif_h

#define COLOR_BLACK		0
#define COLOR_RED		1
#define COLOR_GREEN		2
#define COLOR_YELLOW	3	// brown
#define COLOR_BLUE		4
#define COLOR_MAGENTA	5
#define COLOR_CYAN		6
#define COLOR_WHITE		7	// white (light gray)

#define COLORS			8
#define COLOR_PAIRS		((COLORS) * (COLORS))

int termif_init();
int termif_begin();
int termif_end();

char investigate_wcwidth(wchar_t wc);
char investigate_utf8c_width(const char *utf8c);

int termif_get_screen_size_from_term();
void termif_set_screen_size(int lines, int columns);
int termif_get_lines();
int termif_get_columns();

void termif_clear_screen();
void termif_clear_vscreen_to_paint();
void termif_clear_vscreen_painted();
void termif_set_cursor_pos(short yy, short xx);
void termif_get_cursor_pos(int *yy, int *xx);
void termif_set_cursor_on(char on_off);

void termif_set_attrs(int bgc, int fgc, int rev);
void termif_output_string(short yy, short xx, const char *string, int bytes);
void termif_beep();
void termif_refresh();

#endif // termif_h

// End of termif.h
