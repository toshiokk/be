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
int post_cmd_processing(be_line_t *renum_from, cursor_horiz_vert_move_t cursor_move,
 locate_cursor_to_t locate_cursor, int update_needed)
{
	switch (GET_APPMD(ed_CURS_POSITIONING)) {
	case CURS_POSITIONING_NONE:		break;
	case CURS_POSITIONING_TOP:		locate_cursor = LOCATE_CURS_TOP;		break;
	case CURS_POSITIONING_CENTER:	locate_cursor = LOCATE_CURS_CENTER;		break;
	case CURS_POSITIONING_BOTTOM:	locate_cursor = LOCATE_CURS_BOTTOM;		break;
	}
	if (renum_from) {
		buf_renumber_from_line(get_c_e_b(), renum_from);
	}
	fix_buf_state_after_cursor_move(cursor_move);
	setup_cut_region_after_cursor_move(cursor_move);
	locate_cursor_in_edit_win(locate_cursor);
	set_edit_win_update_needed(update_needed);
#ifdef ENABLE_UNDO
	undo_save_after_change();
#endif // ENABLE_UNDO
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
void locate_cursor_in_edit_win(locate_cursor_to_t locate_curs)
{
	int disp_y_preferred;

	switch(locate_curs) {
	default:
	case LOCATE_CURS_NONE:
		disp_y_preferred = CBV_CURSOR_Y;
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
			switch(locate_curs) {
			default:
			case LOCATE_CURS_JUMP_BACKWARD:	// 2 line upper than center
				disp_y_preferred = edit_win_get_text_lines() / 2 - 2;
				break;
			case LOCATE_CURS_JUMP_CENTER:	// center
				disp_y_preferred = edit_win_get_text_lines() / 2;
				break;
			case LOCATE_CURS_JUMP_FORWARD:	// 2 line lower than center
				disp_y_preferred = edit_win_get_text_lines() / 2 + 2;
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
	CBV_CURSOR_Y = disp_y_preferred;
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
	int disp_y_preferred;
	int lines_go_up;
	be_line_t *line;
	int byte_idx;

	// keep top/bottom scroll margin
	disp_y_preferred = MIN_MAX_(
	 TOP_SCROLL_MARGIN_IDX,
	 CBV_CURSOR_Y,
	 BOTTOM_SCROLL_MARGIN_IDX);

	line = CBV_CL;
	byte_idx = CBV_CLBI;
	for (lines_go_up = 0; lines_go_up < disp_y_preferred; lines_go_up++) {
		if (c_l_up(&line, &byte_idx) == 0) {
			break;
		}
	}
	CBV_CURSOR_Y = lines_go_up;
}

PRIVATE be_line_t *prev_cur_line = NULL;	// Previous CBV_CL
PRIVATE int prev_cur_line_byte_idx = 0;		// Previous CBV_CLBI
PRIVATE int prev_cursor_y = 0;				// Previous cursor_y
void memorize_cursor_pos_before_move(void)
{
	prev_cur_line = CBV_CL;
	prev_cur_line_byte_idx = CBV_CLBI;
	prev_cursor_y = CBV_CURSOR_Y;
}
int get_disp_y_after_cursor_move(void)
{
	int cur_wl_idx;
	be_line_t *line;
	int byte_idx;
	int wl_idx;
	int yy;

	///if (IS_PTR_NULL(prev_cur_line) || IS_PTR_NULL(prev_cur_line->data)) {
	///	// Avoid editor crash !!!!
	///	_PROGERR_
	///	return -1;
	///}
	cur_wl_idx = start_wl_idx_of_wrap_line(CBV_CL->data, CBV_CLBI, -1);

	// check if cur_line is in [0, prev_cursor_y]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (yy = prev_cursor_y; yy >= 0; yy--) {
		wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == CBV_CL && wl_idx == cur_wl_idx) {
			// found in screen
			return yy;
		}
		if (c_l_up(&line, &byte_idx) == 0) {
			break;
		}
	}

	// check if cur_line is in [prev_cursor_y, edit_win_get_text_lines()]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (yy = prev_cursor_y; yy < edit_win_get_text_lines(); yy++) {
		wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == CBV_CL && wl_idx == cur_wl_idx) {
			// found in screen
			return yy;	// current line is in previous screen
		}
		if (c_l_down(&line, &byte_idx) == 0) {
			break;
		}
	}

	return -1;	// current line is out of previous screen
}

int get_cur_screen_top(be_line_t **line, int *byte_idx)
{
	return get_screen_top(CBV_CL, CBV_CLBI, CBV_CURSOR_Y, line, byte_idx);
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
				_cl_ = NODE_PREV(_cl_);
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
void fix_buf_state_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	int wl_idx;
	int cursor_x_in_text;

	if (is_disable_update_min_x_to_keep() == 0) {
		if (cursor_move == CURS_MOVE_HORIZ) {
			CBV_CURSOR_X_TO_KEEP = start_col_idx_of_wrap_line(CBV_CL->data, CBV_CLBI, -1);
			update_min_text_x_to_keep(CBV_CURSOR_X_TO_KEEP);
		} else {
			wl_idx = start_wl_idx_of_wrap_line(CBV_CL->data, CBV_CLBI, -1);
			CBV_CLBI = end_byte_idx_of_wrap_line_le(CBV_CL->data, wl_idx,
			 CBV_CURSOR_X_TO_KEEP, -1);
			cursor_x_in_text = start_col_idx_of_wrap_line(CBV_CL->data, CBV_CLBI, -1);
			update_min_text_x_to_keep(cursor_x_in_text);
		}
	}
}

PRIVATE int calc_min_text_x_to_keep();
PRIVATE int recalc_min_text_x_to_keep(int disp_width, int text_width, int horiz_margin,
 int cursor_x, int cur_min_text_x);

// sample long line:
// 00000000001111111111222222222233333333334444444444
// 000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888

void update_min_text_x_to_keep(int text_x)
{
	int min_text_x_to_keep = calc_min_text_x_to_keep(text_x);
	if (min_text_x_to_keep != CBV_MIN_TEXT_X_TO_KEEP) {
		CBV_MIN_TEXT_X_TO_KEEP = min_text_x_to_keep;
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
}
PRIVATE int calc_min_text_x_to_keep()
{
	te_concat_linefeed(CBV_CL->data);
	return recalc_min_text_x_to_keep(get_edit_win_columns_for_text(),
	 end_col_idx_of_wrap_line(te_line_concat_linefeed, 0, INT_MAX, -1),
	 HORIZ_SCROLL_MARGIN,
	 start_col_idx_of_wrap_line(CBV_CL->data, CBV_CLBI, -1),
	 CBV_MIN_TEXT_X_TO_KEEP);
}

// Calculate the column number of the first character displayed left most
// in window when the cursor is at the given column.
// c: cursor-pos, <: left-most, >: right-most
PRIVATE int recalc_min_text_x_to_keep(int disp_width, int text_width, int margin,
 int cursor_text_x, int cur_min_text_x)
{
	return MIN_MAX_(
	 LIM_MAX(text_width - disp_width, cursor_text_x - (disp_width - margin)),
	 cur_min_text_x,
	 LIM_MIN(0, cursor_text_x - margin));
//      cccccccccccccccccccccccccccccccccccccccc
//      0000000000111111111122222222223333333333
//      <--disp_width-------------------------->
//      <--->                              <---> horizontal-margin

//      ccccc
//      000000000011111111112222222222333333333344444444445555555555
//      <--disp_width-------------------------->
//      <--->                              <---> margin

//           cccccccccccccccccccccccccccccc
//      000000000011111111112222222222333333333344444444445555555555
//      <--disp_width-------------------------->
//      <--->                              <---> margin

//                     cccccccccccccccccccccccccccccccccccccccc
//      000000000011111111112222222222333333333344444444445555555555
//                          <--disp_width-------------------------->
//                                                             <---> margin

//                                                             ccccc
//      000000000011111111112222222222333333333344444444445555555555
//                          <--disp_width-------------------------->
//                          <--->                              <---> margin
}

// min_text_x_to_keep = 0
// |<-- display width ----------------------------->|
// 00000000001111111111222222222233333333334444444444555555555566666666667777777777
//
//           min_text_x_to_keep = 10
//           |<-- display width ----------------------------->|
// 00000000001111111111222222222233333333334444444444555555555566666666667777777777
int get_c_b_v_min_text_x_to_keep(void)
{
	return CBV_MIN_TEXT_X_TO_KEEP;
}

//-----------------------------------------------------------------------------

PRIVATE char disable_update_min_x_to_keep = 0;
void set_disable_update_min_x_to_keep()
{
	disable_update_min_x_to_keep = 1;
}
void clear_disable_update_min_x_to_keep()
{
	disable_update_min_x_to_keep = 0;
}
char is_disable_update_min_x_to_keep()
{
	return disable_update_min_x_to_keep;
}

// End of editormove2.c
