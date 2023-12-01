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

PRIVATE int get_edit_win_x_for_cursor_x(void);
PRIVATE int get_cursor_text_x(void);
PRIVATE int get_max_text_x_to_keep(void);
PRIVATE int get_edit_win_x_for_text_x(int text_x);
PRIVATE int get_edit_win_x_for_view_x(int view_x);
PRIVATE int get_cur_buf_line_num_columns(void);

PRIVATE const char *get_ruler_text(int col_idx);
PRIVATE const char *make_ruler_text(int col_x, int columns);

#define MAX_LINE_NUM_STR_LEN		10	// 4000000000
PRIVATE const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line,
 char *buf_line_num);
PRIVATE int get_buf_line_num_columns(const be_buf_t *buf);
PRIVATE int get_buf_line_num_digits(const be_buf_t *buf);
PRIVATE int get_line_num_digits(int max_line_num);

								//  0123456789012345678901
#define BUF_BUF_LEN			15	// "E99 C99 U99 R99"
#define MEM_BUF_LEN			7	// "999000M" (999G)
#define HHCMMCSS_LEN		8	// "23:59:59"
//1:/home/...editor2.c[Modified] Mc Cut00 Undo00 Redo00 123456MB 11:55:04
void disp_editor_title_bar(void)
{
	int buf_idx;
	char *path;
	char separator_char;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_status[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_num[2+1];
	int edit_bufs;
	int cut_bufs;
#ifdef ENABLE_UNDO
	int undo_bufs;
	int redo_bufs;
#endif // ENABLE_UNDO
	char buf_buf[BUF_BUF_LEN+1];
#ifdef ENABLE_DEBUG
	char buf_mem[MEM_BUF_LEN+1];
#endif // ENABLE_DEBUG
	char buf_time[1+HHCMMCSS_LEN+1];

	buf_idx = get_edit_buf_idx_from_buf(get_cep_buf());
	path = get_cep_buf()->abs_path;

	tio_set_cursor_on(0);

	set_title_bar_color_by_state(BUF_STATE(get_cep_buf(), buf_CUT_MODE));
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
	 get_editor_cur_pane_idx()+1, separator_char, buf_idx+1,
	 (path[0] == '\0') ? _("New File") : path);
	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[Mod] "));
	} else if (CUR_EBUF_STATE(buf_VIEW_MODE)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[VM] "));
	} else if (is_st_writable(&get_cep_buf()->orig_file_stat) == 0) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[WP] "));
	}

	//-------------------------------------------------------------------------
	buf_buf[0] = '\0';
	// edit buffer cut mode
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, "%s", buf_cut_mode_str(get_cep_buf()));
#ifdef ENABLE_DEBUG
///	// cut buffer cut mode
///	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " %s", buf_cut_mode_str(TOP_BUF_OF_CUT_BUFS));
#endif // ENABLE_DEBUG
	// edit buffers
	edit_bufs = count_edit_bufs();
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " E%s", nn_from_num(edit_bufs, buf_num));
	// cut buffers
	cut_bufs = count_cut_bufs();
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " C%s", nn_from_num(cut_bufs, buf_num));
#ifdef ENABLE_UNDO
	// undo buffers
	undo_bufs = count_undo_bufs() / 2;
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " U%s", nn_from_num(undo_bufs, buf_num));
	// redo buffers
	redo_bufs = count_redo_bufs() / 2;
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " R%s", nn_from_num(redo_bufs, buf_num));
#endif // ENABLE_UNDO

#ifdef ENABLE_DEBUG
	// free memory in MB
	snprintf_(buf_mem, MEM_BUF_LEN+1, " %dM", get_mem_free_in_kb(1)/1000);
#endif // ENABLE_DEBUG

	// current time
	snprintf_(buf_time, 1+HHCMMCSS_YY_MM_DD_LEN+1, " %s",
	 cur_ctime_cdate(just_has_been_input_key()));

	//-------------------------------------------------------------------------
	int max_status_cols = main_win_get_columns() / 2;
	///int max_status_cols = main_win_get_columns() / 3;
#ifdef ENABLE_DEBUG
	snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, " %s%s%s", buf_buf, buf_mem, buf_time);
	int status_cols = str_path_len(buf_status);
	if (status_cols > max_status_cols) {
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, " %s%s", buf_mem, buf_time);
		status_cols = str_path_len(buf_status);
	}
#endif // ENABLE_DEBUG
	if (status_cols > max_status_cols) {
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, " %s%s", buf_buf, buf_time);
		status_cols = str_path_len(buf_status);
		if (status_cols > max_status_cols) {
			snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, " %s", buf_time);
			status_cols = str_path_len(buf_status);
		}
	}
	int path_cols = LIM_MIN(0, main_win_get_columns() - status_cols);
	shrink_str(buf_path, path_cols, 2);
	adjust_utf8s_columns(buf_path, path_cols);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_path, buf_status);

	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);

	tio_set_cursor_on(1);
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

#ifdef ENABLE_REGEX
PRIVATE void disp_edit_win_bracket_hl();
PRIVATE void disp_edit_win_bracket_hl_dir(int display_dir,
 char char_under_cursor, char *needle, int depth_increase);
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

	// Don't make the cursor jump around the screen while updating
	tio_set_cursor_on(0);

	if (edit_win_get_path_lines()) {
		// file path per pane
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, cur_pane);
		sub_win_clear_lines(edit_win_get_path_y(), -1);
		buf_idx = get_edit_buf_idx_from_buf(get_cep_buf());
		snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%d%c%s",
		 buf_idx+1, ':', get_cep_buf()->abs_path);
		shrink_str(buf_path, sub_win_get_columns(), 2);
		sub_win_output_string(edit_win_get_path_y(), 0, buf_path, -1);
	}

	get_cur_screen_top(&line, &byte_idx);
	for (yy = 0; yy < edit_win_get_text_lines(); ) {
		if (IS_NODE_BOT_ANCH(line))
			break;
		max_wl_idx = te_tab_expand__max_wl_idx(line->data);
///flf_d_printf("[%s]\n", te_concat_linefeed_buf);
///flf_d_printf("max_wl_idx: %d\n", max_wl_idx);
		wl_idx = start_wl_idx_of_wrap_line(te_concat_linefeed_buf, byte_idx, -1);
		for ( ; wl_idx <= max_wl_idx; wl_idx++) {
			byte_idx_1 = start_byte_idx_of_wrap_line(te_concat_linefeed_buf, wl_idx, 0, -1);
			byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_concat_linefeed_buf, wl_idx,
			 INT_MAX, -1);
			disp_edit_line(cur_pane, yy, get_cep_buf(), line, byte_idx_1, byte_idx_2);
			if (yy == CEPBV_CURSOR_Y) {
				cursor_line_right_text_x = end_col_idx_of_wrap_line(
				 te_concat_linefeed_buf, wl_idx, byte_idx_2, -1);
			}
			yy++;
			if (yy >= edit_win_get_text_lines())
				break;
		}
		line = NODE_NEXT(line);
		byte_idx = 0;
	}
	// clear remaining edit-win lines
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_lines(edit_win_get_text_y() + yy,
	 edit_win_get_text_y() + edit_win_get_text_lines());

#ifdef ENABLE_REGEX
#define HL_SEARCH_CURSOR
///
#define HL_SEARCH_OTHER
///
#define HL_BRACKET_BW
///
#define HL_BRACKET_FW
#if defined(HL_BRACKET_BW) || defined(HL_BRACKET_FW)
	if ((search_is_needle_set(&search__) == 2) && cur_pane) {
		disp_edit_win_bracket_hl();
	}
#endif // defined(HL_BRACKET_BW) || defined(HL_BRACKET_FW)
#endif // ENABLE_REGEX

	if (GET_APPMD(ed_SHOW_RULER)) {
		int edit_win_text_x = get_cur_buf_line_num_columns();
		if (GET_APPMD(ed_SHOW_LINE_NUMBER)) {
			// display buffer total lines ("999 ")
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
			sub_win_output_string(edit_win_get_ruler_y(), 0,
			 get_line_num_string(get_cep_buf(), CUR_EDIT_BUF_BOT_LINE, buf_line_num),
			 edit_win_text_x);
		}
		// display ruler("1---5----10---15---20---25---30---35---40---45---50---55---60---65")
		const char *ruler = get_ruler_text(CEPBV_MIN_TEXT_X_TO_KEEP);
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
		sub_win_output_string(edit_win_get_ruler_y(), edit_win_text_x, ruler, -1);
		// display cursor column indicator in reverse text on ruler
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
		sub_win_output_string(edit_win_get_ruler_y(), get_edit_win_x_for_cursor_x(),
		 &ruler[get_cursor_text_x() - CEPBV_MIN_TEXT_X_TO_KEEP], 1);
		// display line tail column indicator in reverse text on ruler
		if (cursor_line_right_text_x >= 0) {
			int view_x = cursor_line_right_text_x-1 - CEPBV_MIN_TEXT_X_TO_KEEP;
			if (view_x < get_edit_win_columns_for_text()) {
				sub_win_output_string(edit_win_get_ruler_y(), get_edit_win_x_for_view_x(view_x),
				 &ruler[view_x], 1);
			}
		}
	}

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
///flf_d_printf("[%s]\n", te_concat_linefeed_buf);
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == CEPBV_CL) {
		// highlight current line by painting background
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// First, blank out the line (at a minimum)
	sub_win_clear_lines(edit_win_get_text_y() + yy, -1);

	// output line number -----------------------------------------------------
	if (byte_idx_1 == 0) {
		// first line of line wrapping, display line number
		output_edit_line_num(yy, buf, line);	// "9999 "
	} else {
		output_edit_line_num(yy, buf, NULL);	// "     "
	}

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == CEPBV_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// display text simply =====================================================
	output_edit_line_text(yy, line->data, byte_idx_1, byte_idx_2);

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
			// FALLTHROUGH
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
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
			output_edit_line_text(yy, te_concat_linefeed_buf, left_byte_idx, right_byte_idx);
		}
	}
#endif
#ifdef HL_SEARCH_OTHER
	if (search_is_needle_set(&search__) == 1) {
		// display all text matched in the screen =======================================
		for (byte_idx = 0; byte_idx < byte_idx_2; ) {
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
		if ((search_is_needle_set(&search__) == 1) && (line == CEPBV_CL)) {
			// display matched text at cursor pos ===========================================
			if (search_str_in_line(&search__, &matches, NULL,
			 FORWARD_SEARCH, CASE_SENSITIVE, line->data, CEPBV_CLBI)) {
				// found
				if (matches_start_idx(&matches) == CEPBV_CLBI) {
					if (get_intersection(byte_idx_1, byte_idx_2,
					 CEPBV_CLBI, CEPBV_CLBI + matches_match_len(&matches),
					 &left_byte_idx, &right_byte_idx) > 0) {
						set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, 0);
						output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
					}
				}
			}
		}
#endif // HL_SEARCH_CURSOR
		// draw cursor myself ===========================================================
		if (GET_APPMD(app_DRAW_CURSOR)) {
			set_color_by_idx(ITEM_COLOR_IDX_CURSOR_CHAR, 1);
			vis_idx = vis_idx_from_byte_idx(CEPBV_CL->data, CEPBV_CLBI);
			output_edit_line_text(CEPBV_CURSOR_Y, te_visible_code_buf,
			 vis_idx, vis_idx+utf8c_bytes(&te_visible_code_buf[vis_idx]));
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
		if (regexp_search_compiled(clr_syntax->regexp_start, te_concat_linefeed_buf, byte_idx,
		 REG_NONE, &regexp_matches, 1) != 0) {
			// not found
			break;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&regexp_matches, 0),
		 regexp_matches_end_idx(&regexp_matches, 0),
		 &min_byte_idx, &max_byte_idx) > 0) {
			set_item_color(&clr_syntax->color);
			output_edit_line_text(yy, te_concat_linefeed_buf, min_byte_idx, max_byte_idx);
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

	set_item_color(&clr_syntax->color);
	// First, search start or end syntax on earlier lines.
	for (line_cnt = 0, start_line = NODE_PREV(line); ;
	 line_cnt++, start_line = NODE_PREV(start_line)) {
#define MAX_SYNTAX_SEARCH_LINES		LIM_MIN(25, edit_win_get_text_lines() * 2)
		if (IS_NODE_TOP_ANCH(start_line) || line_cnt >= MAX_SYNTAX_SEARCH_LINES) {
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
		byte_idx = te_concat_linefeed_bytes;
	}
	if (get_intersection(byte_idx_1, byte_idx_2, 0, byte_idx,
	 &min_byte_idx, &max_byte_idx) > 0) {
		// Now paint the region.
		output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
	}

step_two:
	// Second step, we look for start in the current line.
	for (byte_idx = 0; byte_idx < te_concat_linefeed_bytes; ) {
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
			byte_idx = te_concat_linefeed_bytes;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&matches_begin, 0), byte_idx,
		 &min_byte_idx, &max_byte_idx) > 0) {
			output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
		}
		if (byte_idx == te_concat_linefeed_bytes) {
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
	int search_dir;
	int display_direction;

	prepare_colors_for_bracket_hl();
	char_under_cursor = *CEPBV_CL_CEPBV_CLBI;

	display_direction = setup_bracket_search(char_under_cursor, search__.direction, needle);

	disp_edit_win_bracket_hl_dir(- display_direction, char_under_cursor, needle, -1);
	disp_edit_win_bracket_hl_dir(+ display_direction, char_under_cursor, needle, +1);
}

PRIVATE void disp_edit_win_bracket_hl_dir(int display_dir,
 char char_under_cursor, char *needle, int depth_increase)
{
	be_line_t *match_line;
	int match_byte_idx;
	be_line_t *line;
	int byte_idx;
	int yy;
	int depth;
	int prev_depth;
	int match_len;
	int skip_here;
	int byte_idx_1;
	int byte_idx_2;
	int max_wl_idx;
	int wl_idx;
	int left_byte_idx;
	int right_byte_idx;
#define MAX_BRACKET_HL		100	// for avoiding infinite loop
	int safe_cnt = 0;

	if (display_dir < 0) {
#ifdef HL_BRACKET_BW
		// draw backward [0, yy] from cursor pos
		match_line = CEPBV_CL;
		match_byte_idx = CEPBV_CLBI;
		line = CEPBV_CL;
		byte_idx = CEPBV_CLBI;
		yy = CEPBV_CURSOR_Y;
		skip_here = 0;
		for (depth = 0, safe_cnt = 0;
		 ((-MAX_BRACKET_NESTINGS < depth) && (depth < MAX_BRACKET_NESTINGS))
		  && (safe_cnt < MAX_BRACKET_HL);
		 safe_cnt++) {
			match_len = search_bracket_in_buffer(&match_line, &match_byte_idx,
			 char_under_cursor, needle, BACKWARD_SEARCH, skip_here, depth_increase,
			 &depth, &prev_depth);
			skip_here = 1;
			for ( ; yy >= 0; ) {
				if (match_len == 0)
					break;
				max_wl_idx = te_tab_expand__max_wl_idx(line->data);
				wl_idx = start_wl_idx_of_wrap_line(te_concat_linefeed_buf, byte_idx, -1);
				for ( ; wl_idx >= 0; wl_idx--) {
					byte_idx_1 = start_byte_idx_of_wrap_line(te_concat_linefeed_buf, wl_idx, 0, -1);
					byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_concat_linefeed_buf, wl_idx,
					 INT_MAX, -1);
					if (match_line == line && get_intersection(byte_idx_1, byte_idx_2,
					 match_byte_idx, match_byte_idx + match_len,
					 &left_byte_idx, &right_byte_idx) > 0) {
						set_color_for_bracket_hl(depth_increase, prev_depth); // select color by depth
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
				byte_idx = line_data_strlen(line->data);	// goto the previous line's tail
			}
		}
#endif // HL_BRACKET_BW
	} else {
#ifdef HL_BRACKET_FW
		// draw forward [yy, edit_win_get_text_lines()-1] from cursor pos
		match_line = CEPBV_CL;
		match_byte_idx = CEPBV_CLBI;
		line = CEPBV_CL;
		byte_idx = CEPBV_CLBI;
		yy = CEPBV_CURSOR_Y;
		skip_here = 0;
		for (depth = 0, safe_cnt = 0;
		 ((-MAX_BRACKET_NESTINGS < depth) && (depth < MAX_BRACKET_NESTINGS))
		  && (safe_cnt < MAX_BRACKET_HL);
		 safe_cnt++) {
			match_len = search_bracket_in_buffer(&match_line, &match_byte_idx,
			 char_under_cursor, needle, FORWARD_SEARCH, skip_here, depth_increase,
			 &depth, &prev_depth);
			skip_here = 1;
			for ( ; yy < edit_win_get_text_lines(); ) {
				if (match_len == 0)
					break;
				max_wl_idx = te_tab_expand__max_wl_idx(line->data);
				wl_idx = start_wl_idx_of_wrap_line(te_concat_linefeed_buf, byte_idx, -1);
				for ( ; wl_idx <= max_wl_idx; wl_idx++) {
					byte_idx_1 = start_byte_idx_of_wrap_line(te_concat_linefeed_buf, wl_idx, 0, -1);
					byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_concat_linefeed_buf, wl_idx,
					 INT_MAX, -1);
					if (match_line == line && get_intersection(byte_idx_1, byte_idx_2,
					 match_byte_idx, match_byte_idx + match_len,
					 &left_byte_idx, &right_byte_idx) > 0) {
						set_color_for_bracket_hl(depth_increase, prev_depth); // select color by depth
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

PRIVATE int output_edit_line_num(int yy, const be_buf_t *buf, const be_line_t *line)
{
	char buf_line_num[MAX_LINE_NUM_STR_LEN+1];

	if (GET_APPMD(ed_SHOW_LINE_NUMBER) == 0) {
		return 0;
	}
	if (line == CEPBV_CL) {
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
	sub_win_set_cursor_pos(edit_win_get_text_y() + CEPBV_CURSOR_Y, get_edit_win_x_for_cursor_x());
}

PRIVATE int output_edit_line_text(int yy, const char *raw_code, int byte_idx_1, int byte_idx_2)
{
	return output_edit_line_text__(yy, raw_code, byte_idx_1, byte_idx_2, te_visible_code_buf);
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
	left_x = LIM_MIN(left_x, CEPBV_MIN_TEXT_X_TO_KEEP);
	right_x = LIM_MAX(right_x, get_max_text_x_to_keep());
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
		sub_win_output_string(edit_win_get_text_y() + yy,
		 get_edit_win_x_for_text_x(left_x), &vis_code[left_vis_idx], bytes);
	}
	return bytes;
}

PRIVATE int get_edit_win_x_for_cursor_x(void)
{
	return get_edit_win_x_for_text_x(get_cursor_text_x());
}
PRIVATE int get_cursor_text_x(void)
{
	return start_col_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
}
PRIVATE int get_max_text_x_to_keep(void)
{
	return CEPBV_MIN_TEXT_X_TO_KEEP + get_edit_win_columns_for_text();
}
PRIVATE int get_edit_win_x_for_text_x(int text_x)
{
	return get_edit_win_x_for_view_x(text_x - CEPBV_MIN_TEXT_X_TO_KEEP);
}
PRIVATE int get_edit_win_x_for_view_x(int view_x)
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
// text x position (line-number-width)
PRIVATE int get_cur_buf_line_num_columns(void)
{
	return GET_APPMD(ed_SHOW_LINE_NUMBER) == 0
	// ^text-to-edit
	 ? 0
	// 999 ^text-to-edit
	 : get_buf_line_num_columns(get_cep_buf());
}

// ruler ======================================================================

#ifdef START_UP_TEST
void test_make_ruler_text(void)
{
flf_d_printf("[%s]\n", make_ruler_text(0, 40));
flf_d_printf("[%s]\n", make_ruler_text(97, 40));
flf_d_printf("[%s]\n", make_ruler_text(98, 40));
flf_d_printf("[%s]\n", make_ruler_text(99, 40));
flf_d_printf("[%s]\n", make_ruler_text(100, 40));
}
#endif // START_UP_TEST

PRIVATE const char *get_ruler_text(int col_idx)
{
	return make_ruler_text(col_idx, get_edit_win_columns_for_text());
}
#define RULER_NUM_INTERVAL	5
#define R_N_I				RULER_NUM_INTERVAL
#define MAX_RULER_STR_LEN	(MAX_SCRN_LINE_BUF_LEN + R_N_I)
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
//1---5----10---15---20---25---30---35---40---45---50---55---60---65---70---75---80
//9910-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//----9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//---9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//--9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
PRIVATE const char *make_ruler_text(int start_col_idx, int length)
{
	static int start_col_idx_cached = -1;
	static int length_cached = -1;
	static char ruler_line_buf[MAX_RULER_STR_LEN+1] = "";	// ruler line
	int col_num;
	char col_num_str[4+1];	// "9999"

	length = MIN(MAX_RULER_STR_LEN, length);
	if (start_col_idx_cached == start_col_idx && length_cached == length) {
		// return cached string
	} else {
		start_col_idx_cached = start_col_idx;
		length_cached = length;

		strcpy__(ruler_line_buf, "");
		// 0, 5, 10, 15, 20, ...
		for (col_num = (start_col_idx / R_N_I) * R_N_I;
		 col_num <= MAX_EDIT_LINE_LEN * get_cur_buf_tab_size();
		 col_num = ((col_num / R_N_I) + 1) * R_N_I) {
			snprintf_(col_num_str, 4+1, "%d", col_num);
			if (col_num <= start_col_idx) {
				// line head
				if (start_col_idx == 0) {
					snprintf_(ruler_line_buf, R_N_I+1, "1----");
				} else {
					snprintf_(ruler_line_buf, R_N_I+1, "-----");
				}
			} else {
										//4,9,14,19, ...
				snprintf_(&ruler_line_buf[col_num - 1 - start_col_idx], R_N_I+1,
				 "%s----", col_num_str);
			}
			if (strnlen(ruler_line_buf, MAX_RULER_STR_LEN) >= length)
				break;
		}
///flf_d_printf("[%s]\n", ruler_line_buf);
		ruler_line_buf[length] = '\0';	// limit length
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
// string linefeed('\n') concatenated
int te_concat_linefeed_bytes = 0;						// bytes of (raw_byte + line-feed)
char te_concat_linefeed_buf[MAX_EDIT_LINE_LEN * 2 +1];		// raw_byte + line-feed
// string invisible code(TAB, Zenkaku-space, EOL) converted to charactor('>', '__', '<')
int te_visible_code_columns;							// length of tab-expanded line
char te_visible_code_buf[MAX_EDIT_LINE_LEN * MAX_TAB_SIZE +1];	// tab-expanded-visible-code
// tab-expansion
/* T:TAB, C:control-code ZZ:Zenkaku-space, L:'\n' */
/* original:     "TabcdCefghZZijkl" */
/* control_code: "TabcdCefghZZijklL" */
/* visible_code: ">   abcd^?efgh[]ijkl<" */
const char *te_tab_expand(const char *original)
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
	vis_ptr = te_visible_code_buf;
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
	te_visible_code_columns = col_idx + 1;
	return te_visible_code_buf;
}
const char *te_concat_linefeed(const char *original)
{
///flf_d_printf("[%s]\n", original);
	te_visible_code_buf[0] = '\0';	// clear te_visible_code_buf
	strlcpy__(te_concat_linefeed_buf, original, MAX_EDIT_LINE_LEN * 2);
	strlcat__(te_concat_linefeed_buf, MAX_EDIT_LINE_LEN * 2, "\n");
	te_concat_linefeed_bytes = strnlen(te_concat_linefeed_buf, MAX_EDIT_LINE_LEN * 2);
	return te_concat_linefeed_buf;
}

int te_tab_expand__max_wl_idx(const char *original)
{
	te_tab_expand(original);
	return max_wrap_line_idx(te_concat_linefeed_buf, -1);
}

// End of editor2.c
