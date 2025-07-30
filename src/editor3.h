/**************************************************************************
 *   editor3.h                                                            *
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

#ifndef editor3_h
#define editor3_h

#define MAX_LINE_NUM_STR_LEN		10	// 4000000000
const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line, char *buf_line_num);
int get_buf_line_num_columns(const be_buf_t *buf);

#ifdef START_UP_TEST
void test_make_ruler_text();
#endif // START_UP_TEST

const char *get_ruler_text(int col_idx);
const char *make_ruler_text(int col_x, int columns);

int edit_win_get_path_lines();
int edit_win_get_path_columns();
int edit_win_get_text_lines();

int edit_win_get_path_y();
int edit_win_get_ruler_y();
int edit_win_get_text_y();

int te_tab_expand__max_wl_idx(const char *original);
extern int te_concat_lf_bytes;
extern char te_concat_lf_buf[MAX_EDIT_LINE_LEN * 2 +1];
extern int te_vis_code_columns;
extern char te_vis_code_buf[MAX_EDIT_LINE_LEN * MAX_TAB_SIZE +1];
const char *te_tab_expand(const char *original);
const char *te_concat_linefeed(const char *original);

#endif // editor3_h

// End of editor3.h
