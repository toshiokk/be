/**************************************************************************
 *   editor2.h                                                            *
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

#ifndef editor2_h
#define editor2_h

void disp_editor_title_bar(void);

#define UPDATE_SCRN_NONE		0x00
#define UPDATE_SCRN_BACKWARD	0x01	// 0000 0001
#define UPDATE_SCRN_PREV		0x02	// 0000 0010
#define UPDATE_SCRN_CUR			0x04	// 0000 0100
#define UPDATE_SCRN_NEXT		0x08	// 0000 1000
#define UPDATE_SCRN_FORWARD		0x10	// 0001 0000
#define UPDATE_SCRN_ALL			0x1f	// 0001 1111
#define UPDATE_SCRN_SOON		0x80	// 1000 0000
#define UPDATE_SCRN_CUR_PREV	(UPDATE_SCRN_CUR | UPDATE_SCRN_PREV)
#define UPDATE_SCRN_CUR_NEXT	(UPDATE_SCRN_CUR | UPDATE_SCRN_NEXT)
///#define UPDATE_SCRN_ALL_SOON		(UPDATE_SCRN_ALL | UPDATE_SCRN_SOON)
#define UPDATE_SCRN_ALL_SOON		UPDATE_SCRN_ALL
void set_edit_win_update_needed(int update_needed);
int get_edit_win_update_needed(void);
void clear_edit_win_update_needed(void);

void disp_edit_win(int cur_pane);
void set_edit_cursor_pos(void);

int get_edit_win_columns_for_text(void);

#ifdef START_UP_TEST
void test_make_ruler_text(void);
#endif // START_UP_TEST

int edit_win_get_path_lines(void);
int edit_win_get_path_columns(void);
int edit_win_get_text_lines(void);

int edit_win_get_path_y(void);
int edit_win_get_ruler_y(void);
int edit_win_get_text_y(void);

extern int te_line_concat_linefeed_bytes;
extern char te_line_concat_linefeed[MAX_EDIT_LINE_LEN * 2 +1];
extern int te_line_visible_code_columns;
extern char te_line_visible_code[MAX_EDIT_LINE_LEN * MAX_TAB_SIZE +1];
const char *te_tab_expand_line(const char *original);
const char *te_concat_linefeed(const char *original);
int te_tab_expand_line__max_wl_idx(const char *original);

#endif // editor2_h

// End of editor2.h
