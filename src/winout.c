/**************************************************************************
 *   winout.c - windowed terminal output                                  *
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

win_rect_t *central_win;
win_rect_t sub_win_rects[SUB_WINS];
int cur_sub_win_idx;
win_rect_t *cur_sub_win;

PRIVATE int win_get_lines(win_rect_t *win);
PRIVATE int win_get_columns(win_rect_t *win);
PRIVATE void win_set_cursor_pos(win_rect_t *win, int yy, int xx);
PRIVATE void win_clear_screen(win_rect_t *win);
PRIVATE void win_clear_lines(win_rect_t *win, int line_1, int line_2);
PRIVATE int win_output_string(win_rect_t *win, int yy, int xx, const char *string, int bytes);

//------------------------------------------------------------------------------
PRIVATE int win_depth = 0;			// 0:root window, 1,2,...:child window

void win_init_win_size()
{
	win_depth = 0;
	win_reinit_win_size();
}
void set_win_depth(int nesting_depth)
{
	win_depth = nesting_depth;
	win_reinit_win_size();
}
int get_win_depth()
{
	return win_depth;
}
void win_reinit_win_size()
{
	win_setup_win_size(win_depth);
}

// editor window sectioning
//	[single pane]							[two panes]
//	+path---------(CENTRAL)-------------+	+---------------(CENTRAL)---------------+
//	|-----ruler--(SUB_WHOLE)------------|	|path--(SUB_LEFT)---+path--(SUB_RIGHT)--|
//	|  1-------------text---------------|	|---+----ruler------+---+----ruler------|
//	|  2|                               |	|  1+----text-------|  1+----text-------|
//	|  3|                               |	|  2|               |  2|               |
//	|  4|                               |	|  3|               |  3|               |
//	|  5|                               |	|  4|               |  4|               |
//	|  6|                               |	|  5|               |  5|               |
//	|  7|                               |	|  6|               |  6|               |
//	|  8|                               |	|  7|               |  7|               |
//	|---+-------------------------------|	|---+---------------+---+---------------|
//	|                                   |	|                                       |
//	+-----------------------------------+	+---------------------------------------+

// filer window sectioning
//	[single pane]							[two panes]
//	+path---------(CENTRAL)-------------+	+---------------(CENTRAL)---------------+
//	|------------(SUB_WHOLE)------------|	|path-(SUB_LEFT)----+path-(SUB_RIGHT)---|
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|-----------------------------------|	|-------------------+-------------------|
//	|                                   |	|                                       |
//	+-----------------------------------+	+---------------------------------------+
int win_size_shrink_lines()
{
	return (win_depth * 1 * 2);
}
int win_size_shrink_columns()
{
	return (win_depth * 2 * 2);
}
void win_setup_win_size(int win_depth)
{
	for (int sub_win_idx = WIN_IDX_CENTRAL; sub_win_idx < SUB_WINS; sub_win_idx++) {
		win_select_cur_sub_win(sub_win_idx);
		switch (sub_win_idx) {
		default:
		case WIN_IDX_CENTRAL:
			central_win->top = 0 + win_depth * 1;
			central_win->lines = LIM_MIN(0, tio_get_lines() - win_size_shrink_lines());
			central_win->left = 0 + win_depth * 2;
			central_win->columns = LIM_MIN(0, tio_get_columns() - win_size_shrink_columns());
			break;
		case WIN_IDX_SUB_WHOLE:
		case WIN_IDX_SUB_LEFT:
		case WIN_IDX_SUB_RIGHT:
			cur_sub_win->top = central_win->top + central_win_get_top_win_lines();
			cur_sub_win->lines = LIM_MIN(0, central_win_get_lines()
			 - central_win_get_top_win_lines() - central_win_get_bottom_win_lines());
			break;
		}
		switch (sub_win_idx) {
		default:
		case WIN_IDX_CENTRAL:
			break;
		case WIN_IDX_SUB_WHOLE:
			cur_sub_win->left = central_win->left;
			cur_sub_win->columns = central_win->columns;
			break;
		case WIN_IDX_SUB_LEFT:
			cur_sub_win->left = central_win->left;
			cur_sub_win->columns = central_win->columns / 2;
			break;
		case WIN_IDX_SUB_RIGHT:
			cur_sub_win->left = central_win->left + central_win->columns / 2;
			cur_sub_win->columns = central_win->columns - central_win->columns / 2;
			break;
		}
	}
}
void win_select_cur_sub_win(int sub_win_idx)
{
	cur_sub_win_idx = MIN_MAX_(0, sub_win_idx, SUB_WINS-1);
	cur_sub_win = &sub_win_rects[cur_sub_win_idx];
	if (sub_win_idx == WIN_IDX_CENTRAL) {
		central_win = cur_sub_win;
	}
}

#ifdef ENABLE_DEBUG
void dump_cur_central_win()
{
	flf_dprintf("central_win: top-left(%d, %d), lines-columns(%d, %d)\n",
	 central_win->top, central_win->left, central_win->lines, central_win->columns);
}
void dump_cur_sub_win()
{
	flf_dprintf("sub_win-%d: top-left(%d, %d), lines-columns(%d, %d)\n",
	 cur_sub_win_idx,
	 cur_sub_win->top, cur_sub_win->left, cur_sub_win->lines, cur_sub_win->columns);
}
#endif // ENABLE_DEBUG

//------------------------------------------------------------------------------
const char *shrink_str_to_scr_static(const char *str)
{
	return shrink_str_static(str, central_win_get_columns() / 3 * 2, 5);
}

//------------------------------------------------------------------------------
int central_win_get_lines()
{
	return central_win->lines;
}
int central_win_get_columns()
{
	return central_win->columns;
}
int central_win_get_top_win_lines()
{
	return TITLE_LINES;
}
int central_win_get_mid_win_lines()
{
	return LIM_MIN(0, central_win_get_lines()
	 - central_win_get_top_win_lines() - central_win_get_bottom_win_lines());
}
int central_win_get_bottom_win_lines()
{
	return STATUS_LINES + get_key_list_lines();
}

int central_win_get_top_win_y()
{
	return 0;
}
int central_win_get_mid_win_y()
{
	return central_win_get_top_win_y() + central_win_get_top_win_lines();
}
int central_win_get_bottom_win_y()
{
	return central_win_get_top_win_y() + central_win_get_top_win_lines()
	 + central_win_get_mid_win_lines();
}

int central_win_get_yes_no_line_y()
{
	// if there is key-list-line, input on KEY_LIST_LINE otherwise STATUS_LINE
	return (get_key_list_lines() == 0)
	 ? central_win_get_status_line_y()
	 : central_win_get_key_list_line_y();
}
int central_win_get_status_line_y()
{
	return central_win_get_bottom_win_y() + STATUS_LINE;
}
int central_win_get_key_list_line_y()
{
	return central_win_get_bottom_win_y() + KEY_LIST_LINE;
}
//------------------------------------------------------------------------------
void central_win_set_cursor_pos(int yy, int xx)
{
	win_set_cursor_pos(central_win, yy, xx);
}
void central_win_clear_screen()
{
	win_clear_screen(central_win);
}
void central_win_clear_lines(int line_1, int line_2)
{
	win_clear_lines(central_win, line_1, line_2);
}
int central_win_output_string(int yy, int xx, const char *string, int bytes)
{
	return win_output_string(central_win, yy, xx, string, bytes);
}
//------------------------------------------------------------------------------
int sub_win_get_lines()
{
	return win_get_lines(cur_sub_win);
}
int sub_win_get_columns()
{
	return win_get_columns(cur_sub_win);
}
void sub_win_set_cursor_pos(int yy, int xx)
{
	win_set_cursor_pos(cur_sub_win, yy, xx);
}
void sub_win_clear_screen()
{
	win_clear_screen(cur_sub_win);
}
void sub_win_clear_lines(int line_1, int line_2)
{
	win_clear_lines(cur_sub_win, line_1, line_2);
}
int sub_win_output_string(int yy, int xx, const char *string, int bytes)
{
	return win_output_string(cur_sub_win, yy, xx, string, bytes);
}
//------------------------------------------------------------------------------
PRIVATE int win_get_lines(win_rect_t *win)
{
	return win->lines;
}
PRIVATE int win_get_columns(win_rect_t *win)
{
	return win->columns;
}
PRIVATE void win_set_cursor_pos(win_rect_t *win, int yy, int xx)
{
	if (yy >= 0 && xx >= 0) {
		yy += win->top;
		xx += win->left;
	}
	tio_set_cursor_pos(yy, xx);
}

PRIVATE int win_cursor_yy = 0;
void win_save_cursor_y()
{
	tio_get_cursor_pos(&win_cursor_yy, NULL);
}
int win_get_saved_cursor_y()
{
	return win_cursor_yy;
}

PRIVATE void win_clear_screen(win_rect_t *win)
{
	win_clear_lines(win, 0, win_get_lines(win));
}
PRIVATE void win_clear_lines(win_rect_t *win, int line_1, int line_2)
{
	if (line_2 < 0) {
		line_2 = line_1 + (-line_2);
	}
	for (int yy = line_1; yy < line_2; yy++) {
		win_output_string(win, yy, 0, tio_blank_line(), win_get_columns(win));
	}
}
PRIVATE int win_output_string(win_rect_t *win, int yy, int xx, const char *string, int bytes)
{
	int space_columns = win->columns;
	if (xx >= 0) {
		space_columns = win->columns - xx;
	}
	if (yy >= 0 && xx >= 0) {
		yy += win->top;
		xx += win->left;
	}
	// limit output length
	if (bytes < 0) {
		bytes = byte_idx_from_col_idx(string, space_columns, CHAR_LEFT, NULL);
	}
	tio_output_string(yy, xx, string, bytes);
	return col_idx_from_byte_idx(string, bytes);
}

// End of winout.c
