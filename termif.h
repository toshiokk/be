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

// These keywords are defined in curses.h

#define KEY_RESIZE		0x019a
#define KEY_HOME		0x0106
#define KEY_END			0x0168
#define KEY_UP			0x0103
#define KEY_DOWN		0x0102
#define KEY_LEFT		0x0104
#define KEY_RIGHT		0x0105
#define KEY_PPAGE		0x0153
#define KEY_NPAGE		0x0152
#define KEY_IC			0x014b
#define KEY_DC			0x014a
#define KEY_BACKSPACE	0x0107
#define KEY_ENTER		0x0157
#define KEY_F(fknum)	(0x0108 + fknum)

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

int termif_init(void);
int termif_begin(void);
int termif_end(void);

int termif_get_screen_size_from_term(void);
void termif_set_screen_size(int lines, int columns);
int termif_get_lines(void);
int termif_get_columns(void);

void termif_clear_screen(void);
void termif_set_cursor_on(int on_off);
void termif_set_cursor_pos(int yy, int xx);
int termif_get_cursor_pos(int *yy, int *xx);
void termif_set_attrs(int bgc, int fgc, int rev);

void termif_output_string(int yy, int xx, const char *string, int bytes);
void termif_beep(void);
void termif_refresh(void);

key_code_t termif_input_key(void);

void investigate_key_sequence(void);

#endif // termif_h

// End of termif.h
