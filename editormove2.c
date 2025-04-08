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

int editor_vert_scroll_margin_lines()
{
	return LIM_MAX(EDITOR_VERT_SCROLL_MERGIN, edit_win_get_text_lines() / 3);		// [0, 3]
}
int top_scroll_margin_y()
{
	return editor_vert_scroll_margin_lines();
}
int bottom_scroll_margin_y()
{
	return edit_win_get_text_lines()-1 - editor_vert_scroll_margin_lines();
}
int editor_vert_scroll_lines()
{
	return MIN_MAX_(1, edit_win_get_text_lines()-1 - editor_vert_scroll_margin_lines(), 20);
}

// name								to does
// -------------------------------- ------------------------------------------
// update buffer size				after deletion and insertion of lines
// renumber lines					after deletion and insertion of lines
// fix buffer state					update cursor_x_to_keep, update cut region
// position cursor in edit_win		
// set edit_win_update_needed		update screen of cur-line or whole win

void post_cmd_processing(be_line_t *renum_from, cursor_horiz_vert_move_t cursor_move,
 locate_cursor_to_t locate_cursor, int update_needed)
{
	switch (GET_APPMD(ed_CURS_POSITIONING)) {
	default:
	case CURS_POSITIONING_NONE:		break;		// not change
	case CURS_POSITIONING_TOP:		locate_cursor = LOCATE_CURS_TOP;		break;
	case CURS_POSITIONING_CENTER:	locate_cursor = LOCATE_CURS_CENTER;		break;
	case CURS_POSITIONING_BOTTOM:	locate_cursor = LOCATE_CURS_BOTTOM;		break;
	}
	if (renum_from) {
		buf_renumber_from_line(get_epc_buf(), renum_from);
	}
	adjust_curs_pos_after_cursor_move(cursor_move);
	setup_cut_region_after_cursor_move(cursor_move);
	locate_curs_y_in_edit_win(locate_cursor);
	set_edit_win_update_needed(update_needed);
#ifdef ENABLE_UNDO
	undo_save_after_change();
#endif // ENABLE_UNDO
	return;
}

//------------------------------------------------------------------------------
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
void locate_curs_y_in_edit_win(locate_cursor_to_t locate_curs)
{
	int disp_y_preferred;

	switch (locate_curs) {
	default:
	case LOCATE_CURS_NONE:
		disp_y_preferred = EPCBVC_CURS_Y;
		break;
	case LOCATE_CURS_JUMP_BACKWARD:	// locate cursor keeping screen if possible.
	case LOCATE_CURS_JUMP_CENTER:	// locate cursor keeping screen if possible.
	case LOCATE_CURS_JUMP_FORWARD:	// locate cursor keeping screen if possible.
		// In search string,
		// Case-A: When the next cursor position is in screen,
		//         it does not move contents and locate cursor in it.
		// Case-B: When the next cursor position is out of screen,
		//         it does move contents and locate cursor at the center of screen.
		if ((disp_y_preferred = get_disp_y_after_cursor_move()) >= 0) {
			// Case-A: current line is in previous screen
		} else {
			// Case-B: current line is out of previous screen
			// LOCATE_CURS_CENTER
			switch (locate_curs) {
			default:
			case LOCATE_CURS_JUMP_BACKWARD:	// upper one third of the screen
				disp_y_preferred = edit_win_get_text_lines() * 1 / 3;
				break;
			case LOCATE_CURS_JUMP_CENTER:	// center
				disp_y_preferred = edit_win_get_text_lines() / 2;
				break;
			case LOCATE_CURS_JUMP_FORWARD:	// lower one third of the screen
				disp_y_preferred = edit_win_get_text_lines() * 2 / 3;
				break;
			}
			disp_y_preferred = MK_IN_RANGE(0, disp_y_preferred, edit_win_get_text_lines());
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
	EPCBVC_CURS_Y = disp_y_preferred;
	fix_cursor_y_keeping_vert_scroll_margin();
}

//	+---------------------------------+
//	| top-of-screen                   | ^
//	|                                 | |
//	|                                 | | cursor_y
//	|                                 | |
//	|                                 | v
//	|                                 | <= prev_cur_line, prev_cur_line_byte_idx
//	|                                 |
//	|                                 |
//	+---------------------------------+
void fix_cursor_y_keeping_vert_scroll_margin(void)
{
	// keep top/bottom scroll margin
	int disp_y_preferred = MIN_MAX_(
	 top_scroll_margin_y(),
	 EPCBVC_CURS_Y,
	 bottom_scroll_margin_y());

	be_line_t *line = EPCBVC_CL;
	int byte_idx = EPCBVC_CLBI;
	int lines_go_up;
	for (lines_go_up = 0; lines_go_up < disp_y_preferred; lines_go_up++) {
		if (cur_line_up(&line, &byte_idx) == 0) {
			break;
		}
	}
	EPCBVC_CURS_Y = lines_go_up;
}

PRIVATE be_line_t *prev_cur_line = NULL;	// Previous EPCBVC_CL
PRIVATE int prev_cur_line_byte_idx = 0;		// Previous EPCBVC_CLBI
PRIVATE int prev_cursor_y = 0;				// Previous cursor_y
void memorize_cursor_pos_before_move(void)
{
	prev_cur_line = EPCBVC_CL;
	prev_cur_line_byte_idx = EPCBVC_CLBI;
	prev_cursor_y = EPCBVC_CURS_Y;
}
int get_disp_y_after_cursor_move(void)
{
	int cur_wl_idx = start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);

	// check if cur_line is in [0, prev_cursor_y]
	be_line_t *line = prev_cur_line;
	int byte_idx = prev_cur_line_byte_idx;
	for (int yy = prev_cursor_y; yy >= 0; yy--) {
		int wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == EPCBVC_CL && wl_idx == cur_wl_idx) {
			// found in screen
			return yy;
		}
		if (cur_line_up(&line, &byte_idx) == 0) {
			break;
		}
	}

	// check if cur_line is in [prev_cursor_y, edit_win_get_text_lines()]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (int yy = prev_cursor_y; yy < edit_win_get_text_lines(); yy++) {
		int wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == EPCBVC_CL && wl_idx == cur_wl_idx) {
			// found in screen
			return yy;	// current line is in previous screen
		}
		if (cur_line_down(&line, &byte_idx) == 0) {
			break;
		}
	}

	return -1;	// current line is out of previous screen
}

// go backward to screen top and return line and byte_idx
int get_edit_win_screen_top(be_line_t *_cl_, int _clbi_, int yy, be_line_t **line, int *byte_idx)
{
	int line_cnt = 0;

	if (_cl_->data == NULL) {
		*line = _cl_;
		*byte_idx = _clbi_;
		return 0;
	}
	te_concat_linefeed(_cl_->data);
	int wl_idx = start_wl_idx_of_wrap_line(te_concat_lf_buf, _clbi_, -1);
	for ( ; ; ) {
		if (yy <= 0) {
			break;
		}
		if (wl_idx <= 0) {
			if (IS_NODE_TOP_MOST(_cl_) == 0) {
				_cl_ = NODE_PREV(_cl_);
			}
			te_concat_linefeed(_cl_->data);
			wl_idx = max_wrap_line_idx(te_concat_lf_buf, -1);
		} else {
			wl_idx--;
		}
		yy--;
		line_cnt++;
	}
	*line = _cl_;
	*byte_idx = start_byte_idx_of_wrap_line(te_concat_lf_buf, wl_idx, 0, -1);
	return line_cnt;
}
//------------------------------------------------------------------------------
// adjust pointers after moving horizontally or vertically
void adjust_curs_pos_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	int wl_idx;
	int cursor_x_in_text;

	if (is_disabled_update_min_text_x_to_keep() == 0) {
		if ((cursor_move == CURS_MOVE_HORIZ) || (cursor_move == CURS_MOVE_JUMP)) {
			// update `buf_view->cursor_x_to_keep`
			// and    `buf_view->min_text_x_to_keep`
			EPCBVC_CURS_X_TO_KEEP = start_col_idx_of_wrap_line(
			 EPCBVC_CL->data, EPCBVC_CLBI, -1);
			update_min_text_x_to_keep(EPCBVC_CURS_X_TO_KEEP);
		} else {
			// not upate  `buf_view->cursor_x_to_keep`
			// and update `buf_view->min_text_x_to_keep`
			wl_idx = start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
			EPCBVC_CLBI = end_byte_idx_of_wrap_line_le(EPCBVC_CL->data, wl_idx,
			 EPCBVC_CURS_X_TO_KEEP, -1);
			cursor_x_in_text = start_col_idx_of_wrap_line(
			 EPCBVC_CL->data, EPCBVC_CLBI, -1);
			update_min_text_x_to_keep(cursor_x_in_text);
		}
	}
}

// cursor_text_x                                          : X position in contents of cursor
// text_x, min_text_x, max_text_x,
//  left_text_x, right_text_x, text_left_x, text_right_x  : X position in contents
// disp_x, min_disp_x, max_disp_x,
//  left_disp_x, right_disp_x, disp_left_x, disp_right_x  : X position in screen

PRIVATE int calc_min_text_x_to_keep();
PRIVATE int recalc_min_text_x_to_keep(int disp_width, int text_width, int margin,
 int cursor_text_x, int min_text_x);

// sample long line:
// 00000000001111111111222222222233333333334444444444
// 000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888

void update_min_text_x_to_keep(int text_x)
{
	int min_text_x_to_keep = calc_min_text_x_to_keep(text_x);
	if (min_text_x_to_keep != EPCBVC_MIN_TEXT_X_TO_KEEP) {
		EPCBVC_MIN_TEXT_X_TO_KEEP = min_text_x_to_keep;
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
}
// This function does not need to consider line wrapping,
// because in the line wrapping mode, the contents width never beyond the display width.
PRIVATE int calc_min_text_x_to_keep()
{
	te_concat_linefeed(EPCBVC_CL->data);
	return recalc_min_text_x_to_keep(get_edit_win_columns_for_text(),
	 end_col_idx_of_wrap_line(te_concat_lf_buf, 0, INT_MAX, -1),
	 HORIZ_SCROLL_MARGIN,
	 start_col_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1),
	 EPCBVC_MIN_TEXT_X_TO_KEEP);
}

// Calculate the column number of the first character displayed left most
// in window when the cursor is at the given column 'cursor_text_x' in the text.
PRIVATE int recalc_min_text_x_to_keep(int disp_width, int text_width, int margin,
 int cursor_text_x, int min_text_x)
{
	min_text_x = MIN_MAX_(
	 // this value push 'min_text_x' to right.     ...==>|
	 cursor_text_x - (disp_width - margin) + 1,
	 min_text_x,
	 // this value push 'min_text_x' to left.  |<==...
	 cursor_text_x - margin);
	return MIN_MAX_(0, min_text_x, LIM_MIN(0, text_width - disp_width));

// Condition of the below example: disp_width=40, margin=5, text_width=50
//  cursor_text_x: [0--50], min_text_x: [0--10]

// |cursor_text_x|push_right |push_left  |
// |             |/ min_bound|/ max_bound|
// |-------------|-----------|-----------|
// |  0--5       |-35~-30 / 0|-5~0  /   0|
// |  6--10      |-31~-25 / 0| 1~5  / 1~5|
// | 11--15      |-24~-20 / 0| 6~10 /0~10|
// | 16--34      |-19~0   / 0|11~39 /  10|
// | 35--44      | 1~9  / 1~9|40~49 /  10|
// | 45--50      |10~15 /  10|50    /  10|

// C: cursor_text_x, <: left-most, >: right-most, <--->: horizontal-margin

// 012345678901234-text-to-be-shown-34567890123456789
// <--->----------disp_width----------<--->
// C

// 01234567890123456789012345678901234567890123456789
// <--->----------disp_width----------<--->
//                                 ->C

// 01234567890123456789012345678901234567890123456789
//  <--->----------disp_width----------<--->
//                                  ->C

// 01234567890123456789012345678901234567890123456789
//           <--->----------disp_width----------<--->
//                                           ->C

// 01234567890123456789012345678901234567890123456789
//            <--->----------disp_width----------<--->
//                                            ->C....C

// 01234567890123456789012345678901234567890123456789
//           <--->----------disp_width----------<--->
//                C<-

// 01234567890123456789012345678901234567890123456789
//          <--->----------disp_width----------<--->
//               C<-

// 01234567890123456789012345678901234567890123456789
// <--->----------disp_width----------<--->
//      C<-

// 01234567890123456789012345678901234567890123456789
// <--->----------disp_width----------<--->
// C...C<-

}

//          min_text_x_to_keep = 0
//          |<-- display width ------------------->|
//          01234567890123456789012345678901234567890123456789

//          min_text_x_to_keep = 10
//          |<-- display width ------------------->|
//01234567890123456789012345678901234567890123456789

//------------------------------------------------------------------------------
PRIVATE char disabled_update_min_text_x_to_keep = 0;
void set_disabled_update_min_text_x_to_keep()
{
	disabled_update_min_text_x_to_keep = 1;
}
void clear_disabled_update_min_text_x_to_keep()
{
	disabled_update_min_text_x_to_keep = 0;
}
char is_disabled_update_min_text_x_to_keep()
{
	return disabled_update_min_text_x_to_keep;
}

// End of editormove2.c
