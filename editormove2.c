/**************************************************************************
 *   editormove2.c                                                        *
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

#include "headers.h"

// name								to does
// -------------------------------- ------------------------------------------
// update buffer size				after deletion and insertion of lines
// renumber lines					after deletion and insertion of lines
// fix buffer state					update cursor_x_to_keep, update cut region
//
// position cursor in edit_win		
//
// set edit_win_update_needed		update screen of cur-line or whole win
//
// memorize prev positions			for the next update
//
int post_cmd_processing(be_line_t *renum_from, cursor_horiz_vert_move_t cursor_move,
 locate_cursor_to_t locate_cursor, int update_needed)
{
///_D_(dump_cur_editor_views());
	switch (GET_APPMD(ed_CURS_POSITIONING)) {
	case CURS_POSITIONING_NONE:		break;
	case CURS_POSITIONING_TOP:		locate_cursor = LOCATE_CURS_TOP;		break;
	case CURS_POSITIONING_CENTER:	locate_cursor = LOCATE_CURS_CENTER;		break;
	case CURS_POSITIONING_BOTTOM:	locate_cursor = LOCATE_CURS_BOTTOM;		break;
	}
	if (renum_from) {
///_FLF_
		buf_renumber_from_line(get_c_e_b(), renum_from);
	}
///_FLF_
	fix_buf_state_after_cursor_horiz_vert_move(cursor_move);
///_FLF_
	locate_cursor_in_edit_win(locate_cursor);
	set_edit_win_update_needed(update_needed);
#ifdef ENABLE_UNDO
	undo_save_after_change();
#endif // ENABLE_UNDO
///_FLF_
	return 0;
}

//-----------------------------------------------------------------------------

//	+---------------------------------+
//	|        LOCATE_CURS_TOP          |
//	|                                 |
//	|                                 |
//	|                                 |
//	|                                 |
//	|       LOCATE_CURS_CENTER        |
//	|                                 |
//	|                                 |
//	|                                 |
//	|                                 |
//	|       LOCATE_CURS_BOTTOM        |
//	+---------------------------------+
void locate_cursor_in_edit_win(locate_cursor_to_t location)
{
	int disp_y_preferred;

	switch(location) {
	default:
	case LOCATE_CURS_NONE:
		disp_y_preferred = CEBV_CURSOR_Y;
		break;
	case LOCATE_CURS_KEEP:
		if ((disp_y_preferred = get_disp_y_after_cursor_move()) < 0) {
			disp_y_preferred = edit_win_get_text_lines() / 2;
		}
		break;
	case LOCATE_CURS_TOP:
		disp_y_preferred = 0;
		break;
	case LOCATE_CURS_CENTER:
		disp_y_preferred = edit_win_get_text_lines() / 2;
		break;
	case LOCATE_CURS_BOTTOM:
		disp_y_preferred = edit_win_get_text_lines() - 1;
		break;
	}
	CEBV_CURSOR_Y = disp_y_preferred;
	fix_cursor_y_keeping_vert_scroll_margin();
}

void fix_cursor_y_keeping_vert_scroll_margin(void)
{
	int disp_y_preferred;
	int lines_go_up;
	be_line_t *line;
	int byte_idx;

	// keep top/bottom scroll margin
	disp_y_preferred = MIN_MAX_(
	 TOP_SCROLL_MARGIN_IDX,
	 CEBV_CURSOR_Y,
	 BOTTOM_SCROLL_MARGIN_IDX);

	line = CEBV_CL;
	byte_idx = CEBV_CLBI;
	for (lines_go_up = 0; lines_go_up < disp_y_preferred; lines_go_up++) {
		if (c_l_up(&line, &byte_idx) == 0) {
			break;
		}
	}
	CEBV_CURSOR_Y = lines_go_up;
}

PRIVATE be_line_t *prev_cur_line;		// Previous CEBV_CL
PRIVATE int prev_cur_line_byte_idx;		// Previous CEBV_CLBI
PRIVATE int prev_cursor_y;				// Previous cursor_y
void memorize_cur_cursor_pos_before_cursor_move(void)
{
///_FLF_
	prev_cur_line = CEBV_CL;
	prev_cur_line_byte_idx = CEBV_CLBI;
	prev_cursor_y = CEBV_CURSOR_Y;
}
int get_disp_y_after_cursor_move(void)
{
	int _cl_wl_idx;
	be_line_t *line;
	int byte_idx;
	int wl_idx;
	int yy;

	_cl_wl_idx = start_wl_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1);

	// check [0 -- prev_cursor_y]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (yy = prev_cursor_y; yy >= 0; yy--) {
		wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == CEBV_CL && wl_idx == _cl_wl_idx) {
			// found in screen
			return yy;
		}
		if (c_l_up(&line, &byte_idx) == 0) {
			break;
		}
	}
	// check [prev_cursor_y -- edit_win_get_text_lines()]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (yy = prev_cursor_y; yy < edit_win_get_text_lines(); yy++) {
		wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == CEBV_CL && wl_idx == _cl_wl_idx) {
			// found in screen
			return yy;
		}
		if (c_l_down(&line, &byte_idx) == 0) {
			break;
		}
	}
	return -1;
}

int get_cur_screen_top(be_line_t **line, int *byte_idx)
{
	return get_screen_top(CEBV_CL, CEBV_CLBI, CEBV_CURSOR_Y, line, byte_idx);
}
// go backward to screen top and return line and byte_idx
int get_screen_top(be_line_t *_cl_, int _clbi_, int yy,
 be_line_t **line, int *byte_idx)
{
	int line_cnt = 0;
	int wl_idx;

	if (_cl_->data == NULL) {
		*line = _cl_;
		*byte_idx = _clbi_;
		return 0;
	}
	te_concat_linefeed(_cl_->data);
	wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, _clbi_, -1);
	for ( ; ; ) {
		if (yy <= 0)
			break;
		if (wl_idx <= 0) {
			if (IS_NODE_TOP(_cl_) == 0)
				_cl_ = _cl_->prev;
			te_concat_linefeed(_cl_->data);
			wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
		} else {
			wl_idx--;
		}
		yy--;
		line_cnt++;
	}
	*line = _cl_;
	*byte_idx = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, 0, -1);
	return line_cnt;
}

//-----------------------------------------------------------------------------

// adjust pointers after moving horizontally or vertically
void fix_buf_state_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t move)
{
	int wl_idx;
	int cursor_x_in_text;

////_D_(line_dump_byte_idx(CEBV_CL, CEBV_CLBI));
	if (move == HORIZ_MOVE) {
		CEBV_CURSOR_X_TO_KEEP = start_col_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1);
		update_min_text_x_to_be_disp(CEBV_CURSOR_X_TO_KEEP);
	} else {
		wl_idx = start_wl_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1);
		CEBV_CLBI = end_byte_idx_of_wrap_line_le(CEBV_CL->data, wl_idx,
		 CEBV_CURSOR_X_TO_KEEP, -1);
		cursor_x_in_text = start_col_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1);
		update_min_text_x_to_be_disp(cursor_x_in_text);
	}
	setup_cut_region_after_cursor_horiz_vert_move(move);
}

void update_min_text_x_to_be_disp(int text_x)
{
	int min_text_x_to_be_disp = calc_min_text_x_to_be_disp(text_x);
	if (min_text_x_to_be_disp != CEBV_MIN_TEXT_X_TO_BE_DISP) {
		CEBV_MIN_TEXT_X_TO_BE_DISP = min_text_x_to_be_disp;
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
}
int get_cebv_min_text_x_to_be_disp(void)
{
	return CEBV_MIN_TEXT_X_TO_BE_DISP;
}

// sample very long line
// 00000000001111111111222222222233333333334444444444
// 000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888

// Scrolls horizontally within a line in chunks.  This function
// returns the column number of the first character displayed left most
// in window when the cursor is at the given column.
int calc_min_text_x_to_be_disp(int text_x)
{
	int end_col_idx;

	te_concat_linefeed(CEBV_CL->data);
	end_col_idx = end_col_idx_of_wrap_line(te_line_concat_linefeed, 0, INT_MAX, -1);
///flf_d_printf("%d <== [%s]\n", end_col_idx, te_line_concat_linefeed);
flf_d_printf("%d\n", get_edit_win_columns_for_text());
	if (end_col_idx <= get_edit_win_columns_for_text())
		return 0;
	if (text_x < get_edit_win_columns_for_text() - HORIZ_SCROLL_MARGIN)
		return 0;
	if (end_col_idx - text_x <= HORIZ_SCROLL_MARGIN)
		return end_col_idx - get_edit_win_columns_for_text();
	return text_x - (get_edit_win_columns_for_text() - 1 - HORIZ_SCROLL_MARGIN);
}

// End of editormove2.c
