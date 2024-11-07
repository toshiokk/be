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

PRIVATE int win_depth = 0;			// 0:root window, 1,2,...:child window
win_rect_t sub_win_rects[SUB_WINS];
win_rect_t *main_win, *cur_win;
int cur_win_idx;

PRIVATE int win_get_lines(win_rect_t *win);
PRIVATE int win_get_columns(win_rect_t *win);
PRIVATE void win_set_cursor_pos(win_rect_t *win, int yy, int xx);
PRIVATE void win_clear_screen(win_rect_t *win);
PRIVATE void win_clear_lines(win_rect_t *win, int line_1, int line_2);
PRIVATE void win_output_string(win_rect_t *win, int yy, int xx, const char *string, int bytes);

//-----------------------------------------------------------------------------
int win_lines = -1;
int win_columns = -1;

void win_init_win_size(void)
{
	win_depth = 0;
	win_reinit_win_size();
}

//-----------------------------------------------------------------------------

void inc_win_depth(void)
{
	win_depth++;
}
void dec_win_depth(void)
{
	if (win_depth > 0) {
		win_depth--;
	}
}
const int get_win_depth(void)
{
	return win_depth;
}

// editor window sectioning
//	[single pane]							[two panes]
//	+path-----------(MAIN)--------------+	+----------------(MAIN)-----------------+
//	|---+--ruler--(SUB_WHOLE)-----------|	|path--(SUB_LEFT)---+path--(SUB_RIGHT)--|
//	|  1+------------text---------------|	|---+----ruler------+---+----ruler------|
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
//	+---------------(MAIN)--------------+	+----------------(MAIN)-----------------+
//	|------------(SUB_WHOLE)------------|	|----(SUB_LEFT)-----+----(SUB_RIGHT)----|
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
void win_reinit_win_size(void)
{
	int sub_win_idx;

	win_lines = tio_get_lines();
	win_columns = tio_get_columns();

	win_select_win(WIN_IDX_MAIN);
	main_win = cur_win;		// &sub_win_rects[WIN_IDX_MAIN];
	for (sub_win_idx = 0; sub_win_idx < SUB_WINS; sub_win_idx++) {
		win_select_win(sub_win_idx);
		switch (sub_win_idx) {
		default:
		case WIN_IDX_MAIN:
			cur_win->top = 0 + win_depth * 1;
			cur_win->left = 0 + win_depth * 2;
			cur_win->lines = LIM_MIN(0, tio_get_lines() - (win_depth * 1 * 2));
			cur_win->columns = LIM_MIN(0, tio_get_columns() - (win_depth * 2 * 2));
			break;
		case WIN_IDX_SUB_WHOLE:
			cur_win->top = main_win->top + main_win_get_top_win_lines();
			cur_win->left = main_win->left + 0;
			cur_win->lines = LIM_MIN(0, main_win_get_lines()
			 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
			cur_win->columns = main_win->columns;
			break;
		case WIN_IDX_SUB_LEFT:
			cur_win->top = main_win->top + main_win_get_top_win_lines();
			cur_win->left = main_win->left;
			cur_win->lines = LIM_MIN(0, main_win_get_lines()
			 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
			cur_win->columns = main_win->columns / 2;
			break;
		case WIN_IDX_SUB_RIGHT:
			cur_win->top = main_win->top + main_win_get_top_win_lines();
			cur_win->left = main_win->left + main_win->columns / 2;
			cur_win->lines = LIM_MIN(0, main_win_get_lines()
			 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
			cur_win->columns = main_win->columns - main_win->columns / 2;
			break;
		}
	}
}
void win_select_win(int sub_win_idx)
{
	cur_win_idx = MIN_MAX_(0, sub_win_idx, SUB_WINS-1);
	cur_win = &sub_win_rects[cur_win_idx];
}

void dump_cur_win(void)
{
	flf_d_printf("sub_win-%d: top-left(%d, %d), lines-columns(%d, %d)\n",
	 cur_win_idx, cur_win->top, cur_win->left, cur_win->lines, cur_win->columns);
}

//-----------------------------------------------------------------------------
int main_win_get_lines(void)
{
	return main_win->lines;
}
int main_win_get_columns(void)
{
	return main_win->columns;
}
int main_win_get_top_win_lines(void)
{
	return TITLE_LINES;
}
int main_win_get_mid_win_lines(void)
{
	return LIM_MIN(0, main_win_get_lines()
	 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
}
int main_win_get_bottom_win_lines(void)
{
	return STATUS_LINES + get_key_list_lines();
}

int main_win_get_top_win_y(void)
{
	return 0;
}
int main_win_get_mid_win_y(void)
{
	return main_win_get_top_win_y() + main_win_get_top_win_lines();
}
int main_win_get_bottom_win_y(void)
{
	return main_win_get_top_win_y() + main_win_get_top_win_lines() + main_win_get_mid_win_lines();
}
//-----------------------------------------------------------------------------
void main_win_set_cursor_pos(int yy, int xx)
{
	win_set_cursor_pos(main_win, yy, xx);
}
void main_win_clear_screen(void)
{
	win_clear_screen(main_win);
}
void main_win_clear_lines(int line_1, int line_2)
{
	win_clear_lines(main_win, line_1, line_2);
}
void main_win_output_string(int yy, int xx, const char *string, int bytes)
{
	win_output_string(main_win, yy, xx, string, bytes);
}
//-----------------------------------------------------------------------------
int sub_win_get_lines(void)
{
	return win_get_lines(cur_win);
}
int sub_win_get_columns(void)
{
	return win_get_columns(cur_win);
}
void sub_win_set_cursor_pos(int yy, int xx)
{
	win_set_cursor_pos(cur_win, yy, xx);
}
void sub_win_clear_screen(void)
{
	win_clear_screen(cur_win);
}
void sub_win_clear_lines(int line_1, int line_2)
{
	win_clear_lines(cur_win, line_1, line_2);
}
void sub_win_output_string(int yy, int xx, const char *string, int bytes)
{
	win_output_string(cur_win, yy, xx, string, bytes);
}
//-----------------------------------------------------------------------------
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
PRIVATE int win_cursor_xx = 0;
void win_save_cursor_pos()
{
	tio_get_cursor_pos(&win_cursor_yy, &win_cursor_xx);
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
	int yy;

	if (line_2 < 0) {
		line_2 = line_1 + 1;
	}
	for (yy = line_1; yy < line_2; yy++) {
		win_output_string(win, yy, 0, tio_blank_line(0), win_get_columns(win));
	}
}
PRIVATE void win_output_string(win_rect_t *win, int yy, int xx, const char *string, int bytes)
{
	int space_columns;

	space_columns = win->columns;
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
}

//-----------------------------------------------------------------------------

const char *shrink_str_to_scr_static(const char *str)
{
	return shrink_str_static(str, main_win_get_columns() / 2, 5);
}

// End of winout.c
