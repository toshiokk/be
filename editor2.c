/**************************************************************************
 *   editor2.c                                                            *
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

PRIVATE void disp_edit_line_blank(int yy, const be_line_t *line);
PRIVATE void disp_edit_line__(int cur_pane, int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2);
#ifdef ENABLE_SYNTAX
PRIVATE void disp_edit_line_single_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax);
PRIVATE void disp_edit_line_multi_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax);
#endif // ENABLE_SYNTAX

PRIVATE int output_edit_line_text(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2);
PRIVATE int output_edit_line_num(int yy, const be_line_t *line);
PRIVATE int output_edit_line_text__(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2, const char *vis_code);

PRIVATE int get_edit_win_x_of_cursor_x(void);
PRIVATE int get_cursor_text_x(void);
PRIVATE int get_max_text_x_to_keep(void);
PRIVATE int get_edit_win_x_of_text_x(int text_x);
PRIVATE int get_edit_win_x_of_view_x(int view_x);

PRIVATE void clear_cur_buf_line_num_columns(void);
PRIVATE int get_cur_buf_line_num_columns(void);

//------------------------------------------------------------------------------
PRIVATE int edit_win_update_needed = UPDATE_SCRN_NONE;
void set_edit_win_update_needed(int update_needed)
{
	if (edit_win_update_needed & (UPDATE_SCRN_PREV | UPDATE_SCRN_NEXT))
		update_needed |= UPDATE_SCRN_ALL;
	edit_win_update_needed |= update_needed;
}
int get_edit_win_update_needed(void)
{
	return edit_win_update_needed;
}
void clear_edit_win_update_needed(void)
{
	edit_win_update_needed = UPDATE_SCRN_NONE;
}

#ifdef ENABLE_REGEX
PRIVATE void disp_edit_win_bracket_hl();
PRIVATE void disp_edit_win_bracket_hl_dir(int display_dir,
 char char_under_cursor, char *needle, char depth_increase);
#endif // ENABLE_REGEX

//-111111111111<   |
//22222222222222222|
//-2222#2222222<   |
//33333333333333333|
//-333333333333<   |
//44444444444444444|

// Refresh the editor screen.
void disp_edit_win(int cur_pane)
{
	int buf_idx;
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_line_num[MAX_LINE_NUM_STR_LEN+1];
	be_line_t *line;
	int byte_idx;
	int byte_idx_1;
	int byte_idx_2;
	int yy;
	int max_wl_idx;
	int wl_idx;
	int cursor_line_right_text_x = -1;

	if (cur_pane == 0) {
		set_work_space_color_dark();
	} else {
		clear_work_space_color_dark();
	}
#ifdef ENABLE_SYNTAX
	set_file_type_by_cur_file_path();
#endif // ENABLE_SYNTAX
	set_wrap_line_tab_size_from_cur_buf();
	clear_cur_buf_line_num_columns();

	// Don't make the cursor jump around the screen while updating
	tio_set_cursor_on(0);
	// clear whole of edit-win
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_lines(edit_win_get_text_y(), edit_win_get_text_y() + edit_win_get_text_lines());

	if (edit_win_get_path_lines()) {
		// on two pane mode, show each file path
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, ! cur_pane);
		sub_win_clear_lines(edit_win_get_path_y(), -1);
		buf_idx = buf_get_buf_idx(get_epc_buf());
		snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%d%c%s",
		 buf_idx, ':', buf_get_file_path(get_epc_buf(), NULL));
		shrink_str(buf_path, sub_win_get_columns(), 2);
		sub_win_output_string(edit_win_get_path_y(), 0, buf_path, -1);
	}

	// make cursor position within the screen after resizing screen
	EPCBVC_CURS_Y = MIN(edit_win_get_text_lines()-1, EPCBVC_CURS_Y);
	get_edit_win_screen_top(EPCBVC_CL, EPCBVC_CLBI, EPCBVC_CURS_Y, &line, &byte_idx);
flf_d_printf("cursor_y:%d\n", EPCBVC_CURS_Y);
	for (yy = 0; yy < edit_win_get_text_lines(); ) {
		disp_edit_line_blank(yy, line);
		if (IS_NODE_INT(line) == 0) {
			EPCBVC_CURS_Y = MIN_MAX_(0, EPCBVC_CURS_Y, LIM_MIN(0, yy-1));
			if (yy == 0) {
				output_edit_line_num(yy, NULL);	// "     "
				set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
				sub_win_output_string(edit_win_get_text_y(), get_cur_buf_line_num_columns(),
				 _("---- No lines in this buffer ----"), -1);
				yy++;
			}
			break;
		}
		max_wl_idx = te_tab_expand__max_wl_idx(line->data);
		wl_idx = start_wl_idx_of_wrap_line(te_concat_lf_buf, byte_idx, -1);
		for ( ; wl_idx <= max_wl_idx; wl_idx++) {
			byte_idx_1 = start_byte_idx_of_wrap_line(te_concat_lf_buf, wl_idx, 0, -1);
			byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_concat_lf_buf, wl_idx,
			 INT_MAX, -1);
			disp_edit_line__(cur_pane, yy, line, byte_idx_1, byte_idx_2);
			if (yy == EPCBVC_CURS_Y) {
				cursor_line_right_text_x = end_col_idx_of_wrap_line(
				 te_concat_lf_buf, wl_idx, byte_idx_2, -1);
			}
			yy++;
			if (yy >= edit_win_get_text_lines())
				break;
		}
		line = NODE_NEXT(line);
		byte_idx = 0;
	}

#ifdef ENABLE_REGEX
#define HL_SEARCH_CURSOR
#define HL_SEARCH_OTHER
#define HL_BRACKET_BW
#define HL_BRACKET_FW
#if defined(HL_BRACKET_BW) || defined(HL_BRACKET_FW)
	if ((search_is_needle_set(&search__) == 2) && cur_pane) {
		disp_edit_win_bracket_hl();
	}
#endif // defined(HL_BRACKET_BW) || defined(HL_BRACKET_FW)
#endif // ENABLE_REGEX

	if (GET_APPMD(ed_SHOW_RULER)) {
		if (GET_APPMD(ed_SHOW_LINE_NUMBER)) {
			// display buffer total lines ("999 ")
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
			sub_win_output_string(edit_win_get_ruler_y(), 0,
			 get_line_num_string(get_epc_buf(), CUR_EDIT_BUF_BOT_LINE, buf_line_num),
			 get_cur_buf_line_num_columns());
		}
		// display ruler("1---5----10---15---20---25---30---35---40---45---50---55---60---65")
		const char *ruler = get_ruler_text(EPCBVC_MIN_TEXT_X_TO_KEEP);
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
		sub_win_output_string(edit_win_get_ruler_y(), get_cur_buf_line_num_columns(), ruler, -1);
		// display cursor column indicator in reverse text on ruler
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
		sub_win_output_string(edit_win_get_ruler_y(), get_edit_win_x_of_cursor_x(),
		 &ruler[get_cursor_text_x() - EPCBVC_MIN_TEXT_X_TO_KEEP], 1);
		// display line tail column indicator in reverse text on ruler
		if (cursor_line_right_text_x >= 0) {
			int view_x = cursor_line_right_text_x-1 - EPCBVC_MIN_TEXT_X_TO_KEEP;
			if (view_x < get_edit_win_columns_for_text()) {
				sub_win_output_string(edit_win_get_ruler_y(), get_edit_win_x_of_view_x(view_x),
				 &ruler[view_x], 1);
			}
		}
	}

	tio_set_cursor_on(1);
}

//------------------------------------------------------------------------------

PRIVATE void disp_edit_line_blank(int yy, const be_line_t *line)
{
	// First, blank out the line (at a minimum)
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == EPCBVC_CL) {
		// highlight current line by painting background
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// First, blank out the line (at a minimum)
	sub_win_clear_lines(edit_win_get_text_y() + yy, -1);
}

// Just update one line in the edit buffer.
PRIVATE void disp_edit_line__(int cur_pane, int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2)
{
#ifdef ENABLE_SYNTAX
	int syntax_idx;
#endif // ENABLE_SYNTAX
	int left_byte_idx = 0, right_byte_idx = 0;
	int vis_idx;
#ifdef ENABLE_REGEX
	matches_t matches;
#endif // ENABLE_REGEX

	// output line number -----------------------------------------------------
	if (byte_idx_1 == 0) {
		// first line of line wrapping, display line number
		output_edit_line_num(yy, line);	// "9999 "
	} else {
		output_edit_line_num(yy, NULL);	// "     "
	}

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == EPCBVC_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// display text simply =====================================================
	output_edit_line_text(yy, line->data, byte_idx_1, byte_idx_2);

#ifdef ENABLE_SYNTAX
	// display syntax color highlighting =======================================
	if (GET_APPMD(ed_SYNTAX_HIGHLIGHT)) {
		// check for all of syntaxes
		for (syntax_idx = 0; syntax_idx < 2; syntax_idx++) {
			const color_syntax_t *clr_syntax = NULL;
			if (syntax_idx == 0) {
				// syntax_idx = 0: loop 0: user defined color syntax
				clr_syntax = get_color_syntax_head();
			} else {
				// syntax_idx = 1: loop 1: default color syntax
				// clr_syntax == NULL
				if (GET_APPMD(ed_TAB_EOL_NOTATION)) {
					clr_syntax = get_default_color_syntax_head();
				}
			}
			for ( ; clr_syntax; clr_syntax = clr_syntax->next) {
				if (clr_syntax->regexp_end == NULL) {
					// single-line regexp
					disp_edit_line_single_line_regexp(yy, line,
					 byte_idx_1, byte_idx_2, clr_syntax);
				} else {
					// multi-line regexp
					disp_edit_line_multi_line_regexp(yy, line,
					 byte_idx_1, byte_idx_2, clr_syntax);
				}
			}
		}
	}
#endif // ENABLE_SYNTAX

#if 1
	// highlight marked segment ================================================
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE))
	 && (mark_min_line->line_num <= line->line_num
	  && line->line_num <= mark_max_line->line_num)) {
		// this line is at least partially selected
		switch (CUR_EBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
			break;
		case CUT_MODE_N_LINE:
		case CUT_MODE_H_CHAR:
		case CUT_MODE_VH_CHAR:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:
			if (mark_min_line == line && mark_max_line == line) {
				// selection starts and ends within this line
				left_byte_idx = mark_min_byte_idx;
				right_byte_idx = mark_max_byte_idx;
			} else if (mark_min_line->line_num < line->line_num 
			 && line->line_num < mark_max_line->line_num) {
				// whole line is selected
				left_byte_idx = 0;
				right_byte_idx = INT_MAX;
			} else if (mark_min_line->line_num == line->line_num) {
				// selection starts from this line
				left_byte_idx = mark_min_byte_idx;
				right_byte_idx = INT_MAX;
			} else if (line->line_num == mark_max_line->line_num) {
				// selection ends at this line
				left_byte_idx = 0;
				right_byte_idx = mark_max_byte_idx;
			}
			break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:
			// this line is partially selected
			left_byte_idx = byte_idx_from_col_idx(line->data, mark_min_col_idx,
			 CHAR_RIGHT, NULL);
			right_byte_idx = byte_idx_from_col_idx(line->data, mark_max_col_idx,
			 CHAR_LEFT, NULL);
			break;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 left_byte_idx, right_byte_idx, &left_byte_idx, &right_byte_idx) > 0) {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED3, 0);
			output_edit_line_text(yy, te_concat_lf_buf, left_byte_idx, right_byte_idx);
		}
	}
#endif
#ifdef HL_SEARCH_OTHER
	if (search_is_needle_set(&search__) == 1) {
		// display all text matched in the screen =======================================
		for (int byte_idx = 0; byte_idx < byte_idx_2; ) {
			if (search_str_in_line(&search__, &matches, NULL,
			 FORWARD_SEARCH, CASE_SENSITIVE, line->data, byte_idx) == 0) {
				// not found
				break;
			}
			if (get_intersection(byte_idx_1, byte_idx_2,
			 matches_start_idx(&matches), matches_end_idx(&matches),
			 &left_byte_idx, &right_byte_idx) > 0) {
				// match is in the line
				set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
				output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
			}
			byte_idx = matches_end_idx(&matches);
		}
	}
#endif // HL_SEARCH_OTHER
	if (cur_pane) {
#ifdef HL_SEARCH_CURSOR
		if ((search_is_needle_set(&search__) == 1) && (line == EPCBVC_CL)) {
			// display matched text at cursor pos ===========================================
			if (search_str_in_line(&search__, &matches, NULL,
			 FORWARD_SEARCH, CASE_SENSITIVE, line->data, EPCBVC_CLBI)) {
				// found
				if (matches_start_idx(&matches) == EPCBVC_CLBI) {
					if (get_intersection(byte_idx_1, byte_idx_2,
					 EPCBVC_CLBI, EPCBVC_CLBI + matches_match_len(&matches),
					 &left_byte_idx, &right_byte_idx) > 0) {
						set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED1, 0);
						output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
					}
				}
			}
		}
#endif // HL_SEARCH_CURSOR
		// draw cursor myself ===========================================================
		if (GET_APPMD(app_DRAW_CURSOR) && (yy == EPCBVC_CURS_Y)) {
			set_color_by_idx(ITEM_COLOR_IDX_CURSOR_CHAR, 1);
			vis_idx = vis_idx_from_byte_idx(EPCBVC_CL->data, EPCBVC_CLBI);
			output_edit_line_text(EPCBVC_CURS_Y, te_vis_code_buf,
			 vis_idx, vis_idx+utf8c_bytes(&te_vis_code_buf[vis_idx]));
		}
	}
}

#ifdef ENABLE_SYNTAX
PRIVATE void disp_edit_line_single_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax)
{
	int byte_idx;
	regexp_matches_t regexp_matches;
	int min_byte_idx, max_byte_idx;

	for (byte_idx = 0; byte_idx < byte_idx_2; ) {
		if (regexp_search_compiled(clr_syntax->regexp_start, te_concat_lf_buf, byte_idx,
		 REG_NONE, &regexp_matches, 1) != 0) {
			// not found
			break;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&regexp_matches, 0),
		 regexp_matches_end_idx(&regexp_matches, 0),
		 &min_byte_idx, &max_byte_idx) > 0) {
			set_item_color(&clr_syntax->color, 0);
			output_edit_line_text(yy, te_concat_lf_buf, min_byte_idx, max_byte_idx);
		}
		byte_idx = regexp_matches_end_idx(&regexp_matches, 0);
	}
}

#ifdef NOT_DEFINED
// test string of multi region syntax highlighting
// e.g.1
	/* comment */ not-comment /* comment */ not-comment /* comment */ not-comment /* comment */
// e.g.2
	not-comment /* comment */ not-comment /* comment */ not-comment /* comment */
// e.g.3
	/* comment
	   comment */ not-comment /* comment */ not-comment /* comment */ not-comment /* comment
	   comment */
// e.g.4
	/* comment
	   comment
	*/
// e.g.5
	/* comment
	/ * comment (Compiler outputs warning error to this NESTED comment. DO NOT care !)
	*/
#endif

PRIVATE void disp_edit_line_multi_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax)
{
	// This is a multi-line regexp. There are two steps.

	// [First step] We have to see if the beginning is there
	// on an earlier line, and an end on this line or later.
	// We find the first line before 'line' matching the
	// start. If every match on that line is followed by an
	// end, then go to step two.  Otherwise, find the next line
	// after start_line matching the end.  If that line is not
	// before 'line', then paint the beginning of this line.
	regexp_matches_t matches_begin;	// match position for regexp_start
	regexp_matches_t matches_end;	// match position for regexp_end
	const be_line_t *start_line;	// the beginning line at which start regexp matched
	int byte_idx;					// where it starts in that line
	int line_cnt;
	int min_byte_idx, max_byte_idx;

	set_item_color(&clr_syntax->color, 0);
	// First, search start or end syntax on earlier lines.
	for (line_cnt = 0, start_line = NODE_PREV(line); ;
	 line_cnt++, start_line = NODE_PREV(start_line)) {
#define MAX_SYNTAX_SEARCH_LINES		LIM_MIN(25, edit_win_get_text_lines() * 2)
		if (! IS_NODE_INT(start_line) || line_cnt >= MAX_SYNTAX_SEARCH_LINES) {
			// No syntax found, so skip to the next step.
			goto step_two;
		}
		if (regexp_search_compiled(clr_syntax->regexp_start, start_line->data, 0,
		 REG_NONE, &matches_begin, 1) == 0) {
			// A start found before current line
			break;
		}
		if (regexp_search_compiled(clr_syntax->regexp_end, start_line->data, 0,
		 REG_NONE, &matches_end, 1) == 0) {
			// No start but there is an end before current line
			goto step_two;
		}
	}
	// Now start_line is the first line containing a start match.
	// Is there a start on this line not followed by an end on this line?
	byte_idx = regexp_matches_end_idx(&matches_begin, 0);
	for ( ; ; ) {
		if (regexp_search_compiled(clr_syntax->regexp_end, start_line->data, byte_idx,
		 REG_NONE, &matches_end, 1) != 0)
			// No end found after start on this line,
			// region is continuing to the current line
			break;
		byte_idx = regexp_matches_end_idx(&matches_end, 0);
		if (regexp_search_compiled(clr_syntax->regexp_start, start_line->data, byte_idx,
		 REG_NONE, &matches_begin, 1) != 0)
			// No more start on this line.
			goto step_two;
		byte_idx = regexp_matches_end_idx(&matches_begin, 0);
	}
	// Indeed, there is a start not followed by an end.
	// Region is continuing to the current line.
	if (regexp_search_compiled(clr_syntax->regexp_end, line->data, 0,
	 REG_NONE, &matches_end, 1) == 0) {
		// region is closed in the current line
		//		/*
		//		  comment */				<== current line
		byte_idx = regexp_matches_end_idx(&matches_end, 0);
	} else {
		// region is not closed in the current line
		//		/*
		//		  comment .....				<== current line
		//		*/
		byte_idx = te_concat_lf_bytes;
	}
	if (get_intersection(byte_idx_1, byte_idx_2, 0, byte_idx,
	 &min_byte_idx, &max_byte_idx) > 0) {
		// Now paint the region.
		output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
	}

step_two:
	// [Second step] We look for start in the current line.
	for (byte_idx = 0; byte_idx < te_concat_lf_bytes; ) {
		if (regexp_search_compiled(clr_syntax->regexp_start, line->data, byte_idx,
		 REG_NONE, &matches_begin, 1) != 0)
			// No more start in the current line.
			break;
		if (regexp_search_compiled(clr_syntax->regexp_end,
		 line->data, regexp_matches_end_idx(&matches_begin, 0),
		 REG_NONE, &matches_end, 1) == 0) {
			// There is an end on this line. Paint the region.
			//		... /* comment */				<== current line
			byte_idx = regexp_matches_end_idx(&matches_end, 0);
		} else {
			//		... /* comment ...				<== current line
			//		*/
			byte_idx = te_concat_lf_bytes;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&matches_begin, 0), byte_idx,
		 &min_byte_idx, &max_byte_idx) > 0) {
			output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
		}
		if (byte_idx == te_concat_lf_bytes) {
			// We painted to the end of the line, so
			// don't bother checking any more starts.
			break;
		}
	}
}
#endif // ENABLE_SYNTAX

#ifdef ENABLE_REGEX
PRIVATE void disp_edit_win_bracket_hl()
{
	char char_under_cursor;
	char needle[BRACKET_SEARCH_REGEXP_STR_LEN+1];
	int display_direction;

	prepare_colors_for_bracket_hl();
	char_under_cursor = *EPCBVC_CL_EPCBVC_CLBI;

	display_direction = setup_bracket_search(char_under_cursor, search__.direction, needle);

	disp_edit_win_bracket_hl_dir(- display_direction, char_under_cursor, needle, -1);
	disp_edit_win_bracket_hl_dir(+ display_direction, char_under_cursor, needle, +1);
}

PRIVATE void disp_edit_win_bracket_hl_dir(int display_dir,
 char char_under_cursor, char *needle, char depth_increase)
{
	be_line_t *match_line;
	int match_byte_idx;
	be_line_t *line;
	int byte_idx;
	int yy;
	int depth;
	int prev_depth;
	int skip_here;
	int byte_idx_1;
	int byte_idx_2;
	int max_wl_idx;
	int wl_idx;
	int left_byte_idx;
	int right_byte_idx;
#define MAX_BRACKET_HL		100	// for avoiding infinite loop
	int safe_cnt = 0;
	UINT8 zero_occurances = 0;

flf_d_printf("display_dir: %d, char_under_cursor: [%c], depth_increase: %d\n",
 display_dir, char_under_cursor, depth_increase);
	if (display_dir < 0) {
#ifdef HL_BRACKET_BW
		// draw backward [0, yy] from cursor pos
		match_line = EPCBVC_CL;
		match_byte_idx = EPCBVC_CLBI;
		line = EPCBVC_CL;
		byte_idx = EPCBVC_CLBI;
		yy = EPCBVC_CURS_Y;
		skip_here = 0;
		for (depth = 0, safe_cnt = 0;
		 ((-MAX_BRACKET_NESTINGS < depth) && (depth < MAX_BRACKET_NESTINGS))
		  && (safe_cnt < MAX_BRACKET_HL);
		 safe_cnt++) {
			int match_len = search_bracket_in_buffer(&match_line, &match_byte_idx,
			 char_under_cursor, needle, BACKWARD_SEARCH, skip_here, depth_increase,
			 &depth, &prev_depth);
			skip_here = 1;
			for ( ; yy >= 0; ) {
				if (match_len == 0)
					break;
				max_wl_idx = te_tab_expand__max_wl_idx(line->data);
				wl_idx = start_wl_idx_of_wrap_line(te_concat_lf_buf, byte_idx, -1);
				for ( ; wl_idx >= 0; wl_idx--) {
					byte_idx_1 = start_byte_idx_of_wrap_line(te_concat_lf_buf, wl_idx,
					 0, -1);
					byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_concat_lf_buf, wl_idx,
					 INT_MAX, -1);
					if (match_line == line && get_intersection(byte_idx_1, byte_idx_2,
					 match_byte_idx, match_byte_idx + match_len,
					 &left_byte_idx, &right_byte_idx) > 0) {
						// select color by depth
						set_color_for_bracket_hl(depth_increase, &zero_occurances, prev_depth);
						output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
						match_len = 0;	// clear match_len so that go to next bracket
						break;
					}
					yy--;
					if (yy < 0)
						break;
				}
				if (match_len == 0)
					break;
				line = NODE_PREV(line);
				if (IS_NODE_INT(line) == 0)
					break;
				byte_idx = line_strlen(line);	// goto the previous line's tail
			}
		}
#endif // HL_BRACKET_BW
	} else {
#ifdef HL_BRACKET_FW
		// draw forward [yy, edit_win_get_text_lines()-1] from cursor pos
		match_line = EPCBVC_CL;
		match_byte_idx = EPCBVC_CLBI;
		line = EPCBVC_CL;
		byte_idx = EPCBVC_CLBI;
		yy = EPCBVC_CURS_Y;
		skip_here = 0;
		for (depth = 0, safe_cnt = 0;
		 ((-MAX_BRACKET_NESTINGS < depth) && (depth < MAX_BRACKET_NESTINGS))
		  && (safe_cnt < MAX_BRACKET_HL);
		 safe_cnt++) {
			int match_len = search_bracket_in_buffer(&match_line, &match_byte_idx,
			 char_under_cursor, needle, FORWARD_SEARCH, skip_here, depth_increase,
			 &depth, &prev_depth);
			skip_here = 1;
			for ( ; yy < edit_win_get_text_lines(); ) {
				if (match_len == 0)
					break;
				max_wl_idx = te_tab_expand__max_wl_idx(line->data);
				wl_idx = start_wl_idx_of_wrap_line(te_concat_lf_buf, byte_idx, -1);
				for ( ; wl_idx <= max_wl_idx; wl_idx++) {
					byte_idx_1 = start_byte_idx_of_wrap_line(te_concat_lf_buf, wl_idx,
					 0, -1);
					byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_concat_lf_buf, wl_idx,
					 INT_MAX, -1);
					if (match_line == line && get_intersection(byte_idx_1, byte_idx_2,
					 match_byte_idx, match_byte_idx + match_len,
					 &left_byte_idx, &right_byte_idx) > 0) {
						// select color by depth
						set_color_for_bracket_hl(depth_increase, &zero_occurances, prev_depth);
						output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
						match_len = 0;	// clear match_len so that go to next bracket
						break;
					}
					yy++;
					if (yy >= edit_win_get_text_lines())
						break;
				}
				if (match_len == 0)
					break;
				line = NODE_NEXT(line);
				if (IS_NODE_INT(line) == 0)
					break;
				byte_idx = 0;	// goto the next line's head
			}
		}
#endif // HL_BRACKET_FW
	}
}
#endif // ENABLE_REGEX

// display output routines ----------------------------------------------------
/*
          |min_text_x_to_keep
qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
rrrrrrrrrr+----------------------------------------------------+rrrrrrrrrrrrrrr
ssOUTPUTss+ssssssssssssssssssssssssssssssssssssssssssssssssssss+sssssssssssssss
tttttttttt|tttttttttttttttttttttttttttttttttttttttttttttttttttt|ttttttttttttttt
uuuuuuuOUTPUT-STRINGuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu|uuuuuuuuuuuuuuu
vvvvvvvvvv|vvvvvvvvvvvOUTPUT_STRINGvvvvvvvvvvvvvvvvvvvvvvvvvvvv|vvvvvvvvvvvvvvv
wwwwwwwwww|wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwOUTPUT_STRINGwwwwwww
xxxxxxxxxx|xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxOUTPUTxxxxxxx
yyyyyyyyyy+----------------------------------------------------+yyyyyyyyyyyyyyy
zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
*/

PRIVATE int output_edit_line_num(int yy, const be_line_t *line)
{
	char buf_line_num[MAX_LINE_NUM_STR_LEN+1];

	if (GET_APPMD(ed_SHOW_LINE_NUMBER) == 0) {
		return 0;
	}
	if (line == EPCBVC_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
	} else {
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
	}
	sub_win_output_string(edit_win_get_text_y() + yy, 0,
	 get_line_num_string(get_epc_buf(), line, buf_line_num), -1);
	return 1;
}

// Set cursor at (cursor_y, cur_line_byte_idx).
void set_edit_cursor_pos(void)
{
	sub_win_set_cursor_pos(edit_win_get_text_y() + EPCBVC_CURS_Y, get_edit_win_x_of_cursor_x());
	win_save_cursor_pos();
}

PRIVATE int output_edit_line_text(int yy, const char *raw_code, int byte_idx_1, int byte_idx_2)
{
	return output_edit_line_text__(yy, raw_code, byte_idx_1, byte_idx_2, te_vis_code_buf);
}
PRIVATE int output_edit_line_text__(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2, const char *vis_code)
{
	int wl_idx;
	int left_x;
	int right_x;
	int left_byte_idx;
	int right_byte_idx;
	int left_vis_idx;
	int right_vis_idx;
	int bytes;

	wl_idx = start_wl_idx_of_wrap_line(raw_code, byte_idx_1, -1);
	left_x = start_col_idx_of_wrap_line(raw_code, byte_idx_1, -1);
	right_x = end_col_idx_of_wrap_line(raw_code, wl_idx, byte_idx_2, -1);
	left_x = LIM_MIN(left_x, EPCBVC_MIN_TEXT_X_TO_KEEP);
	right_x = LIM_MAX(right_x, get_max_text_x_to_keep());
	left_byte_idx = end_byte_idx_of_wrap_line_ge(raw_code, wl_idx, left_x, -1);
	right_byte_idx = end_byte_idx_of_wrap_line_le(raw_code, wl_idx, right_x, -1);

	left_x = start_col_idx_of_wrap_line(raw_code, left_byte_idx, -1);
	right_x = end_col_idx_of_wrap_line(raw_code, wl_idx, right_byte_idx, -1);
	left_vis_idx = vis_idx_from_byte_idx(raw_code, left_byte_idx);
	right_vis_idx = vis_idx_from_byte_idx(raw_code, right_byte_idx);
	bytes = right_vis_idx - left_vis_idx;
	if (bytes > 0) {	// if (bytes <= 0), no output neccesary
		sub_win_output_string(edit_win_get_text_y() + yy,
		 get_edit_win_x_of_text_x(left_x), &vis_code[left_vis_idx], bytes);
	}
	return bytes;
}

PRIVATE int get_edit_win_x_of_cursor_x(void)
{
	return get_edit_win_x_of_text_x(get_cursor_text_x());
}
PRIVATE int get_cursor_text_x(void)
{
	return start_col_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
}
PRIVATE int get_max_text_x_to_keep(void)
{
	return EPCBVC_MIN_TEXT_X_TO_KEEP + get_edit_win_columns_for_text();
}
PRIVATE int get_edit_win_x_of_text_x(int text_x)
{
	return get_edit_win_x_of_view_x(text_x - EPCBVC_MIN_TEXT_X_TO_KEEP);
}
PRIVATE int get_edit_win_x_of_view_x(int view_x)
{
	return get_cur_buf_line_num_columns() + view_x;
}
// width of text view area (window-width - line-number-width)
int get_edit_win_columns_for_text(void)
{
	// <text-to-edit................................>
	// 999 <text-to-edit............................>
	return sub_win_get_columns() - get_cur_buf_line_num_columns();
}

PRIVATE int cur_buf_line_num_columns = -1;
PRIVATE void clear_cur_buf_line_num_columns(void)
{
	cur_buf_line_num_columns = -1;
}
// text x position (line-number-width)
PRIVATE int get_cur_buf_line_num_columns(void)
{
	if (cur_buf_line_num_columns < 0) {
		// value not set, calculate now
		cur_buf_line_num_columns = GET_APPMD(ed_SHOW_LINE_NUMBER) == 0
		 // ^text-to-edit
		 ? 0
		 // 999 ^text-to-edit
		 : get_buf_line_num_columns(get_epc_buf());
	}
	return cur_buf_line_num_columns;
}

// End of editor2.c
