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

PRIVATE void disp_edit_line(int cur_pane, int yy, const be_buf_t *buf, const be_line_t *line,
 int byte_idx_1, int byte_idx_2);
#ifdef ENABLE_SYNTAX
PRIVATE void disp_edit_line_single_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax);
PRIVATE void disp_edit_line_multi_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax);
#endif // ENABLE_SYNTAX

PRIVATE int output_edit_line_text(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2);
PRIVATE int output_edit_line_num(int yy, const be_buf_t *buf, const be_line_t *line);
PRIVATE int output_edit_line_text__(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2, const char *vis_code);

PRIVATE int get_cursor_x_in_edit_win(void);
PRIVATE int get_cursor_x_in_text(void);
PRIVATE int get_max_text_x_to_be_disp(void);
PRIVATE int get_edit_win_x_in_text(int text_x);
PRIVATE int get_line_num_columns(void);

PRIVATE const char *get_ruler_text(int col_idx);
PRIVATE const char *make_ruler_text__(int col_x, int columns);

#define MAX_LINE_NUM_STR_LEN		10	// 4000000000
PRIVATE const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line,
 char *buf_line_num);
PRIVATE int get_buf_line_num_columns(const be_buf_t *buf);
PRIVATE int get_buf_line_num_digits(const be_buf_t *buf);
PRIVATE int get_line_num_digits(int max_line_num);

								//  0123456789012345678901
#define BUF_BUF_LEN			22	// "Mc Cut00 Undo00 Redo00"
#define MEM_BUF_LEN			8	// "999000Mb" (999Gb)
#define HHCMMCSS_LEN		8	// "23:59:59"
//1:/home/...editor2.c[Modified] Mc Cut00 Undo00 Redo00 123456MB 11:55:04
void disp_editor_title_bar(void)
{
	int buf_idx;
	char *path;
	char separator_char;
	int space;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_status[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_num[2+1];
	int cut_bufs;
#ifdef ENABLE_UNDO
	int undo_bufs;
	int redo_bufs;
#endif // ENABLE_UNDO
	char buf_buf[BUF_BUF_LEN+1];
#ifdef ENABLE_DEBUG
	char buf_mem[MEM_BUF_LEN+1];
#endif // ENABLE_DEBUG
	char buf_time[HHCMMCSS_LEN+1];

	buf_idx = get_edit_buf_idx_from_buf(get_c_e_b());
	path = get_c_e_b()->abs_path;

	tio_set_cursor_on(0);

	set_title_bar_color_by_state(BUF_STATE(get_c_e_b(), buf_CUT_MODE));
	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0,
	 tio_blank_line(), main_win_get_columns());

	//-------------------------------------------------------------------------
	separator_char = ':';
	if (is_app_list_mode()) {
		separator_char = '.';
	}
#ifdef ENABLE_DEBUG
	if (GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF) {
		separator_char = ';';
		if (is_app_list_mode()) {
			separator_char = ',';
		}
	}
#endif // ENABLE_DEBUG
	snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%s%d%c%d:%s",
	 root_notation(),
	 editor_panes.view_idx+1, separator_char, buf_idx+1,
	 (path[0] == '\0') ? _("New File") : path);
	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[Modified] "));
	} else if (CUR_EBUF_STATE(buf_VIEW_MODE)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[VIEW mode] "));
	} else if (is_st_writable(&get_c_e_b()->orig_file_stat) == 0) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[WRITE-PROTECTED] "));
	}

	//-------------------------------------------------------------------------
	buf_buf[0] = '\0';
	// edit buffer cut mode
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, "%s", buf_cut_mode_str(get_c_e_b()));
#ifdef ENABLE_DEBUG
///	// cut buffer cut mode
///	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " %s", buf_cut_mode_str(CUR_CUT_BUF));
#endif // ENABLE_DEBUG
	// cut buffers
	cut_bufs = count_cut_bufs();
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " Ct%s", nn_from_num(cut_bufs, buf_num));
#ifdef ENABLE_UNDO
	// undo buffers
	undo_bufs = count_undo_bufs() / 2;
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " Ud%s", nn_from_num(undo_bufs, buf_num));
	// redo buffers
	redo_bufs = count_redo_bufs() / 2;
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " Rd%s", nn_from_num(redo_bufs, buf_num));
#endif // ENABLE_UNDO

#ifdef ENABLE_DEBUG
	// free memory in MB
	snprintf_(buf_mem, MEM_BUF_LEN+1, "%dMb", get_mem_free_in_kb(1)/1000);
#endif // ENABLE_DEBUG

	// current time
	strlcpy__(buf_time, cur_ctime(), HHCMMCSS_LEN);

	//-------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
	snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s %s %s", buf_buf, buf_mem, buf_time);
#else // ENABLE_DEBUG
	snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s %s", buf_buf, buf_time);
#endif // ENABLE_DEBUG
	space = LIM_MIN(0, main_win_get_columns() - strnlen(buf_status, MAX_SCRN_LINE_BUF_LEN) - 1);
	if (space < main_win_get_columns() / 2) {
#ifdef ENABLE_DEBUG
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s %s", buf_mem, buf_time);
#else // ENABLE_DEBUG
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s", buf_time);
#endif // ENABLE_DEBUG
		space = LIM_MIN(0,
		 main_win_get_columns() - strnlen(buf_status, MAX_SCRN_LINE_BUF_LEN) - 1);
	}
	shrink_str(buf_path, space);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%-*s %s", space, buf_path, buf_status);

	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);

	tio_set_cursor_on(1);
///_FLF_
}
//-----------------------------------------------------------------------------
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
	int cursor_line_text_right_x = -1;

///_FLF_
	// Don't make the cursor jump around the screen while updating
	tio_set_cursor_on(0);

///_FLF_
	if (edit_win_get_path_lines()) {
		// file path per pane
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, cur_pane);
		sub_win_clear_lines(edit_win_get_path_y(), -1);
		buf_idx = get_edit_buf_idx_from_buf(get_c_e_b());
		snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%d%c%s",
		 buf_idx+1, ':', get_c_e_b()->abs_path);
		shrink_str(buf_path, sub_win_get_columns());
		sub_win_output_string(edit_win_get_path_y(), 0, buf_path, -1);
	}

///_FLF_
	get_cur_screen_top(&line, &byte_idx);
	for (yy = 0; yy < edit_win_get_text_lines(); ) {
		if (IS_NODE_BOT_ANCH(line))
			break;
		te_tab_expand_line(line->data);
		max_wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
///flf_d_printf("[%s]\n", te_line_concat_linefeed);
///flf_d_printf("max_wl_idx: %d\n", max_wl_idx);
		wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, byte_idx, -1);
		for ( ; wl_idx <= max_wl_idx; wl_idx++) {
///_FLF_
			byte_idx_1 = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, 0, -1);
			byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_line_concat_linefeed, wl_idx,
			 INT_MAX, -1);
			disp_edit_line(cur_pane, yy, get_c_e_b(), line, byte_idx_1, byte_idx_2);
			if (yy == CBV_CURSOR_Y) {
				cursor_line_text_right_x = LIM_MAX(get_edit_win_columns_for_text(),
				 end_col_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, byte_idx_2, -1));
			}
			yy++;
			if (yy >= edit_win_get_text_lines())
				break;
		}
///_FLF_
		line = NEXT_NODE(line);
		byte_idx = 0;
	}
	// clear remaining edit-win lines
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_lines(edit_win_get_text_y() + yy,
	 edit_win_get_text_y() + edit_win_get_text_lines());

	if (GET_APPMD(ed_SHOW_RULER)) {
		int edit_win_text_x = get_line_num_columns();
		if (GET_APPMD(ed_SHOW_LINE_NUMBER)) {
			// display buffer total lines ("999 ")
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
			sub_win_output_string(edit_win_get_ruler_y(), 0,
			 get_line_num_string(get_c_e_b(), CUR_EDIT_BUF_BOT_LINE, buf_line_num),
			 edit_win_text_x);
		}
		// display ruler("1---5----10---15---20---25---30---35---40---45---50---55---60---65")
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
		sub_win_output_string(edit_win_get_ruler_y(), edit_win_text_x,
		 get_ruler_text(0), -1);
		// display cursor column indicator in reverse text on ruler
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
		sub_win_output_string(edit_win_get_ruler_y(), get_cursor_x_in_edit_win(),
		 get_ruler_text(get_cursor_x_in_text() - get_c_b_v_min_text_x_to_keep()), 1);
		// display line tail column indicator in reverse text on ruler
		if (cursor_line_text_right_x >= 0) {
			sub_win_output_string(edit_win_get_ruler_y(),
			 edit_win_text_x + (cursor_line_text_right_x-1 - get_c_b_v_min_text_x_to_keep()),
			 get_ruler_text(cursor_line_text_right_x-1 - get_c_b_v_min_text_x_to_keep()), 1);
		}
	}

///_FLF_
	tio_set_cursor_on(1);
}

//-----------------------------------------------------------------------------

// Just update one line in the edit buffer.
PRIVATE void disp_edit_line(int cur_pane, int yy, const be_buf_t *buf, const be_line_t *line,
 int byte_idx_1, int byte_idx_2)
{
#ifdef ENABLE_SYNTAX
	int syntax_idx;
	const color_syntax_t *clr_syntax = NULL;
#endif // ENABLE_SYNTAX
	int left_byte_idx = 0, right_byte_idx = 0;
	int vis_idx;
	int byte_idx;
	matches_t matches;

///flf_d_printf("%d, [%d, %d]\n", yy, byte_idx_1, byte_idx_2);
///flf_d_printf("[%s]\n", te_line_concat_linefeed);
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == CBV_CL) {
		// highlight current line by painting background
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// First, blank out the line (at a minimum)
	sub_win_clear_lines(edit_win_get_text_y() + yy, -1);

	// output line number -----------------------------------------------------
	if (byte_idx_1 == 0) {
		// first line of line wrapping, display line number
		output_edit_line_num(yy, buf, line);	// "9999 "
///_FLF_
	} else {
		output_edit_line_num(yy, buf, NULL);	// "     "
	}

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == CBV_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// display text simply =====================================================
	output_edit_line_text(yy, line->data, byte_idx_1, byte_idx_2);
///_FLF_

#ifdef ENABLE_SYNTAX
	// display syntax color highlighting =======================================
	if (GET_APPMD(ed_SYNTAX_HIGHLIGHT)) {
		// check for all of syntaxes
		for (syntax_idx = 0; syntax_idx < 2; syntax_idx++) {
			if (syntax_idx == 0) {
				// syntax_idx = 0: loop 0
				clr_syntax = get_color_syntax_head();
			} else {
				// clr_syntax == NULL
				// syntax_idx = 1: loop 1
				if (GET_APPMD(ed_TAB_EOL_NOTATION)) {
					clr_syntax = get_default_color_syntax_head();
				}
			}
			for ( ; clr_syntax; clr_syntax = clr_syntax->next) {
///dump_color_syntax(clr_syntax);
				if (clr_syntax->regexp_end == NULL) {
///_FLF_
					// single-line regexp
					disp_edit_line_single_line_regexp(yy, line,
					 byte_idx_1, byte_idx_2, clr_syntax);
				} else {
///_FLF_
					// multi-line regexp
					disp_edit_line_multi_line_regexp(yy, line,
					 byte_idx_1, byte_idx_2, clr_syntax);
				}
			}
		}
	}
#endif // ENABLE_SYNTAX
///_FLF_

#if 1
	// highlight marked segment ================================================
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE))
	 && (mark_min_line->line_num <= line->line_num
	  && line->line_num <= mark_max_line->line_num)) {
		// this line is at least partially selected
		switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
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
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, 0);
			output_edit_line_text(yy, te_line_concat_linefeed, left_byte_idx, right_byte_idx);
		}
	}
#endif
///_FLF_
#if 1
	if (search_is_needle_set(&search__)) {
		// display all text matched in the screen =======================================
		for (byte_idx = 0; byte_idx < byte_idx_2; ) {
			if (search_str_in_line(&search__, &matches, NULL,
			 FORWARD_SEARCH, DISTINGUISH_CASE, line->data, byte_idx) == 0) {
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
#endif
	if (cur_pane && line == CBV_CL && search_is_needle_set(&search__)) {
		// display matched text at cursor pos ===========================================
		if (search_str_in_line(&search__, &matches, NULL,
		 FORWARD_SEARCH, DISTINGUISH_CASE, line->data, CBV_CLBI)) {
			// found
			if (matches_start_idx(&matches) == CBV_CLBI) {
				if (get_intersection(byte_idx_1, byte_idx_2,
				 CBV_CLBI, CBV_CLBI + matches_match_len(&matches),
				 &left_byte_idx, &right_byte_idx) > 0) {
					set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, 0);
					output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
				}
			}
		}
		// draw cursor myself ===========================================================
		if (GET_APPMD(app_DRAW_CURSOR)) {
			set_color_by_idx(ITEM_COLOR_IDX_CURSOR_CHAR, 0);
			vis_idx = vis_idx_from_byte_idx(CBV_CL->data, CBV_CLBI);
			output_edit_line_text(CBV_CURSOR_Y, te_line_visible_code,
			 vis_idx, vis_idx+utf8c_bytes(&te_line_visible_code[vis_idx]));
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

////flf_d_printf("[%d, %d]\n", byte_idx_1, byte_idx_2);
	for (byte_idx = 0; byte_idx < byte_idx_2; ) {
		if (regexp_search_compiled(clr_syntax->regexp_start, te_line_concat_linefeed, byte_idx,
		 REG_NONE, &regexp_matches, 1) != 0) {
			// not found
			break;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&regexp_matches, 0),
		 regexp_matches_end_idx(&regexp_matches, 0),
		 &min_byte_idx, &max_byte_idx) > 0) {
			set_color_attrs_ptr(&clr_syntax->color);
			output_edit_line_text(yy, te_line_concat_linefeed, min_byte_idx, max_byte_idx);
		}
		byte_idx = regexp_matches_end_idx(&regexp_matches, 0);
	}
}

#ifdef NOT_DEFINED
// test string of multi region syntax highlighting
// Ex.1
	/* comment */ not-comment /* comment */ not-comment /* comment */ not-comment /* comment */
// Ex.2
	not-comment /* comment */ not-comment /* comment */ not-comment /* comment */
// Ex.3
	/* comment
	   comment */ not-comment /* comment */ not-comment /* comment */ not-comment /* comment
	   comment */
// Ex.4
	/* comment
	   comment
	*/
// Ex.5
	/* comment
	/* comment (Compiler outputs warning error to this NESTED comment. DO NOT care !)
	*/
#endif

PRIVATE void disp_edit_line_multi_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax)
{
	// This is a multi-line regexp. There are two steps.
	// First step, we have to see if the beginning is there
	// on an earlier line, and an end on this line or later.
	// We find the first line before line_ptr matching the
	// start. If every match on that line is followed by an
	// end, then go to step two.  Otherwise, find the next line
	// after start_line matching the end.  If that line is not
	// before line_ptr, then paint the beginning of this line.
	regexp_matches_t matches_begin;	// match position for regexp_start
	regexp_matches_t matches_end;	// match position for regexp_end
	const be_line_t *start_line;	// the beginning line at which start regexp matched
	int byte_idx;					// where it starts in that line
	int line_cnt;
	int min_byte_idx, max_byte_idx;

	set_color_attrs_ptr(&clr_syntax->color);
	// First, search start or end syntax on earlier lines.
	for (line_cnt = 0, start_line = line->prev; ;
	 line_cnt++, start_line = start_line->prev) {
#define MAX_SYNTAX_SEARCH_LINES		LIM_MIN(25, edit_win_get_text_lines() * 2)
		if (line_cnt >= MAX_SYNTAX_SEARCH_LINES || IS_NODE_TOP_ANCH(start_line)) {
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
		byte_idx = te_line_concat_linefeed_bytes;
	}
	if (get_intersection(byte_idx_1, byte_idx_2, 0, byte_idx,
	 &min_byte_idx, &max_byte_idx) > 0) {
		// Now paint the region.
		output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
	}

step_two:
	// Second step, we look for start in the current line.
	for (byte_idx = 0; byte_idx < te_line_concat_linefeed_bytes; ) {
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
			byte_idx = te_line_concat_linefeed_bytes;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&matches_begin, 0),
		 byte_idx,
		 &min_byte_idx, &max_byte_idx) > 0) {
			output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
		}
		if (byte_idx == te_line_concat_linefeed_bytes) {
			// We painted to the end of the line, so
			// don't bother checking any more starts.
			break;
		}
	}
}
#endif // ENABLE_SYNTAX

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

PRIVATE int output_edit_line_num(int yy, const be_buf_t *buf, const be_line_t *line)
{
	char buf_line_num[MAX_LINE_NUM_STR_LEN+1];

	if (GET_APPMD(ed_SHOW_LINE_NUMBER) == 0) {
		return 0;
	}
	if (line == CBV_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
	} else {
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
	}
	sub_win_output_string(edit_win_get_text_y() + yy, 0,
	 get_line_num_string(buf, line, buf_line_num), -1);
	return 1;
}

// Set cursor at (cursor_y, cur_line_byte_idx).
void set_edit_cursor_pos(void)
{
	sub_win_set_cursor_pos(edit_win_get_text_y() + CBV_CURSOR_Y, get_cursor_x_in_edit_win());
}

PRIVATE int output_edit_line_text(int yy, const char *raw_code, int byte_idx_1, int byte_idx_2)
{
	return output_edit_line_text__(yy, raw_code, byte_idx_1, byte_idx_2, te_line_visible_code);
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
	left_x = LIM_MIN(left_x, get_c_b_v_min_text_x_to_keep());
	right_x = LIM_MAX(right_x, get_max_text_x_to_be_disp());
	left_byte_idx = end_byte_idx_of_wrap_line_ge(raw_code, wl_idx, left_x, -1);
	right_byte_idx = end_byte_idx_of_wrap_line_le(raw_code, wl_idx, right_x, -1);

	left_x = start_col_idx_of_wrap_line(raw_code, left_byte_idx, -1);
	right_x = end_col_idx_of_wrap_line(raw_code, wl_idx, right_byte_idx, -1);
	left_vis_idx = vis_idx_from_byte_idx(raw_code, left_byte_idx);
	right_vis_idx = vis_idx_from_byte_idx(raw_code, right_byte_idx);
	bytes = right_vis_idx - left_vis_idx;
///flf_d_printf("left_x:%d,right_x:%d,left_vis_idx:%d,right_vis_idx:%d,bytes:%d,[%s]\n",
/// left_x,right_x,left_vis_idx,right_vis_idx,bytes, &vis_code[left_vis_idx]);
	if (bytes > 0) {	// if (bytes <= 0), no output neccesary
///_FLF_
		sub_win_output_string(edit_win_get_text_y() + yy,
		 get_edit_win_x_in_text(left_x), &vis_code[left_vis_idx], bytes);
	}
///_FLF_
	return bytes;
}

PRIVATE int get_cursor_x_in_edit_win(void)
{
	return get_edit_win_x_in_text(get_cursor_x_in_text());
}
PRIVATE int get_cursor_x_in_text(void)
{
	return start_col_idx_of_wrap_line(CBV_CL->data, CBV_CLBI, -1);
}
PRIVATE int get_max_text_x_to_be_disp(void)
{
	return get_c_b_v_min_text_x_to_keep() + get_edit_win_columns_for_text();
}
PRIVATE int get_edit_win_x_in_text(int text_x)
{
	return get_line_num_columns() + (text_x - get_c_b_v_min_text_x_to_keep());
}
// width of text view area (window-width - line-number-width)
int get_edit_win_columns_for_text(void)
{
	// <text-to-edit................................>
	// 999 <text-to-edit............................>
	return sub_win_get_columns() - get_line_num_columns();
}
// text x position (line-number-width)
PRIVATE int get_line_num_columns(void)
{
	return GET_APPMD(ed_SHOW_LINE_NUMBER) == 0
	// ^text-to-edit
	 ? 0
	// 999 ^text-to-edit
	 : get_buf_line_num_columns(get_c_e_b());
}

// ruler ======================================================================

#ifdef START_UP_TEST
void test_make_ruler_text(void)
{
flf_d_printf("[%s]\n", make_ruler_text__(0, 40));
flf_d_printf("[%s]\n", make_ruler_text__(97, 40));
flf_d_printf("[%s]\n", make_ruler_text__(98, 40));
flf_d_printf("[%s]\n", make_ruler_text__(99, 40));
flf_d_printf("[%s]\n", make_ruler_text__(100, 40));
}
#endif // START_UP_TEST

#define RULER_NUM_INTERVAL	5
#define R_N_I				RULER_NUM_INTERVAL
#define MAX_RULER_STR_LEN	(MAX_SCRN_LINE_BUF_LEN + R_N_I)
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
//1---5----10---15---20---25---30---35---40---45---50---55---60---65---70---75---80
//9910-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//----9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//---9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//--9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
PRIVATE const char *get_ruler_text(int col_idx)
{
	const char *str;

	str = make_ruler_text__(get_c_b_v_min_text_x_to_keep(), get_edit_win_columns_for_text());
	col_idx = MK_IN_RANGE(0, col_idx, strnlen(str, MAX_RULER_STR_LEN));
	return &str[col_idx];
}
PRIVATE const char *make_ruler_text__(int start_col_idx, int columns)
{
	static int start_col_idx__ = -1;
	static int columns__ = -1;
	static char ruler_line_buf[MAX_RULER_STR_LEN+1] = "";	// ruler line
	int col_num;
	char col_num_str[4+1];	// "9999"

	if (start_col_idx__ != start_col_idx || columns__ != columns) {
		start_col_idx__ = start_col_idx;
		columns__ = columns;

		strcpy__(ruler_line_buf, "");
		// 0, 5, 10, 15, 20, ...
		for (col_num = (start_col_idx / R_N_I) * R_N_I;
		 col_num <= MAX_EDIT_LINE_LEN * get_cur_buf_tab_size();
		 col_num = ((col_num / R_N_I) + 1) * R_N_I) {
			snprintf_(col_num_str, 4+1, "%d", col_num);
			if (col_num - 1 - start_col_idx < 0) {
				// line head
				if (start_col_idx == 0) {
					snprintf_(ruler_line_buf, R_N_I+1, "1----");
				} else {
					snprintf_(ruler_line_buf, R_N_I+1, "-----");
				}
			} else {
				snprintf_(&ruler_line_buf[col_num - 1 - start_col_idx], R_N_I+1,
				 "%s----", col_num_str);
			}
			if (strnlen(ruler_line_buf, MAX_RULER_STR_LEN) >= columns)
				break;
		}
///flf_d_printf("[%s]\n", ruler_line_buf);
		ruler_line_buf[columns] = '\0';
///flf_d_printf("[%s]\n", ruler_line_buf);
	}
	return ruler_line_buf;
}

// line number ================================================================

//   1 #include <stdio.h>
//   2 int main(int argc, char *argv[])
// 998 #include <stdio.h>
// 999 int main(int argc, char *argv[])
//    1 #include <stdio.h>
//    2 int main(int argc, char *argv[])
// 9998 #include <stdio.h>
// 9999 int main(int argc, char *argv[])
//          1 #include <stdio.h>
//          2 int main(int argc, char *argv[])
// 9999999998 #include <stdio.h>
// 9999999999 int main(int argc, char *argv[])

#define MIN_LINE_NUM_DIGITS			3		// 999(1000-1)
#define MIN_LINE_NUM_DIGITS_NUM		1000	// 10 ^ MIN_LINE_NUM_DIGITS
#define MAX_LINE_NUM_DIGITS			10		// 9999999999(10000000000-1)
#define LINE_NUM_SEPARATOR			" "		// "999 "
#define LINE_NUM_SEPARATOR_COLS		1		// strlen(LINE_NUM_SEPARATOR);

PRIVATE const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line,
 char *buf_line_num)
{
	if (line) {
		// "9999 "
		snprintf_(buf_line_num, MAX_LINE_NUM_STR_LEN+1,
		 "%*d" LINE_NUM_SEPARATOR, get_buf_line_num_digits(buf), line->line_num);
	} else {
		// "     "
		snprintf_(buf_line_num, MAX_LINE_NUM_STR_LEN+1,
		 "%*s" LINE_NUM_SEPARATOR, get_buf_line_num_digits(buf), "");
	}
	return buf_line_num;
}
PRIVATE int get_buf_line_num_columns(const be_buf_t *buf)
{
	return get_buf_line_num_digits(buf) + LINE_NUM_SEPARATOR_COLS;		// "123" + " "
}
PRIVATE int get_buf_line_num_digits(const be_buf_t *buf)
{
	return get_line_num_digits(BUF_BOT_LINE(buf)->line_num);
}
PRIVATE int get_line_num_digits(int max_line_num)
{
	int digits;		// 3,    4,     5, ...     10
	long long num;	// 1000, 10000, 100000, ...10000000000

	// 999, 9999, 99999, ... 999999999
	for (digits = MIN_LINE_NUM_DIGITS, num = MIN_LINE_NUM_DIGITS_NUM;
	 digits <= MAX_LINE_NUM_DIGITS;
	 digits++, num *= 10) {
		if (max_line_num < num)
			break;
	}
	return digits;
}

//-----------------------------------------------------------------------------
int edit_win_get_path_lines(void)
{
	if (GET_APPMD(ed_EDITOR_PANES) == 0) {
		return 0;
	} else {
		return 1;
	}
}
int edit_win_get_text_lines(void)
{
	return sub_win_get_lines() - edit_win_get_path_lines() - get_ruler_lines();
}

int edit_win_get_path_y(void)
{
	return 0;
}
int edit_win_get_ruler_y(void)
{
	return edit_win_get_path_y() + edit_win_get_path_lines();
}
int edit_win_get_text_y(void)
{
	return edit_win_get_path_y() + edit_win_get_path_lines() + get_ruler_lines();
}

#define TAB_NOTATION	'>'
#define EOL_NOTATION	'<'
int te_line_concat_linefeed_bytes;							// bytes of (raw_byte + line-feed)
char te_line_concat_linefeed[MAX_EDIT_LINE_LEN * 2 +1];		// raw_byte + line-feed
int te_line_visible_code_columns;							// length of tab-expanded line
char te_line_visible_code[MAX_EDIT_LINE_LEN * MAX_TAB_SIZE +1];	// tab-expanded-visible-code
// tab-expansion
/* T:TAB, C:control-code ZZ:Zenkaku-space, L:'\n' */
/* original:     "TabcdCefghZZijkl" */
/* control_code: "TabcdCefghZZijklL" */
/* visible_code: ">   abcd^?efgh[]ijkl<" */
const char *te_tab_expand_line(const char *original)
{
	int col_idx;
	int bytes;
	const char *orig_ptr;
	char *vis_ptr;
#ifdef ENABLE_SYNTAX
	int notation = 0;		// is visible Tab/EOL/Zenkaku-space notation

	notation = (GET_APPMD(ed_SYNTAX_HIGHLIGHT) && GET_APPMD(ed_TAB_EOL_NOTATION));
#endif // ENABLE_SYNTAX

	te_concat_linefeed(original);
///flf_d_printf("original:%s\n", original);
	orig_ptr = original;
	vis_ptr = te_line_visible_code;
	for (col_idx = 0; *orig_ptr; ) {
		if (*orig_ptr == '\t') {
#ifdef ENABLE_SYNTAX
			*vis_ptr++ = notation ? TAB_NOTATION : ' ';
#else // ENABLE_SYNTAX
			*vis_ptr++ = ' ';
#endif // ENABLE_SYNTAX
			while (++col_idx % priv_tab_size) {
				*vis_ptr++ = ' ';
			}
			orig_ptr++;
		} else if (is_ctrl_char((unsigned char)*orig_ptr)) {
			*vis_ptr++ = '^';
			*vis_ptr++ = (*orig_ptr == 0x7f) ? '?' : *orig_ptr + '@';
			col_idx += 2;
			orig_ptr++;
		} else if ((unsigned char)*orig_ptr < 0x80) {	// ASCII
			*vis_ptr++ = *orig_ptr++;
			col_idx++;
		} else {
#ifdef ENABLE_SYNTAX
			if (notation && strlcmp__(orig_ptr, UTF8_ZEN_SPACE) == 0)
				strcpy__(vis_ptr, UTF8_ZEN_SPACE_NOTATION);
			else
				strlcpy__(vis_ptr, orig_ptr, utf8c_bytes(orig_ptr));
#else // ENABLE_SYNTAX
			strlcpy__(vis_ptr, orig_ptr, utf8c_bytes(orig_ptr));
#endif // ENABLE_SYNTAX
			col_idx += utf8c_columns(orig_ptr);
			bytes = utf8c_bytes(orig_ptr);
			vis_ptr += bytes;
			orig_ptr += bytes;
		}
	}
#ifdef ENABLE_SYNTAX
	*vis_ptr++ = notation ? EOL_NOTATION : ' ';
#else // ENABLE_SYNTAX
	*vis_ptr++ = ' ';
#endif // ENABLE_SYNTAX
	*vis_ptr = '\0';
	te_line_visible_code_columns = col_idx + 1;
	te_line_concat_linefeed_bytes = orig_ptr - original + 1;
	return te_line_visible_code;
}
const char *te_concat_linefeed(const char *original)
{
	strlcpy__(te_line_concat_linefeed, original, MAX_EDIT_LINE_LEN * 2);
	strlcat__(te_line_concat_linefeed, MAX_EDIT_LINE_LEN * 2, "\n");
	return te_line_concat_linefeed;
}

// End of editor2.c
