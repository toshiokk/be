/**************************************************************************
 *   linewrap.h                                                           *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
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

#ifndef linewrap_h
#define linewrap_h

extern int priv_tab_size;

void set_wrap_line_tab_size(int tab_size);

// line wrapping related functions
#ifdef START_UP_TEST
void test_wrap_line(void);
#endif // START_UP_TEST
int max_wrap_line_idx(const char *line, int width);
int start_wl_idx_of_wrap_line(const char *line, int byte_idx, int width);
int end_wl_idx_of_wrap_line(const char *line, int byte_idx, int width);
int start_col_idx_of_wrap_line(const char *line, int byte_idx, int width);
int end_col_idx_of_wrap_line(const char *line, int wl_idx, int byte_idx, int width);
int start_byte_idx_of_wrap_line(const char *line, int wl_idx, int column, int width);
int end_byte_idx_of_wrap_line_le(const char *line, int wl_idx, int column, int width);
int end_byte_idx_of_wrap_line_ge(const char *line, int wl_idx, int column, int width);
int wrap_line_length(int width);

typedef enum {
	CHAR_LEFT = -1,
	CHAR_RIGHT = +1,
} char_left_right_t;
int byte_idx_from_byte_idx(const char *utf8s, int bytes);
int byte_idx_from_col_idx(const char *utf8s, int columns, char_left_right_t left_right,
 int *col_idx__);
int col_idx_from_byte_idx(const char *utf8s, int col_idx, int bytes);
int vis_idx_from_byte_idx(const char *utf8s, int bytes);

#ifdef START_UP_TEST
void test_get_intersection(void);
#endif // START_UP_TEST
int get_intersection(int min1, int max1, int min2, int max2, int *min, int *max);

#endif // linewrap_h

// End of linewrap.h
