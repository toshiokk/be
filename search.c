/**************************************************************************
 *   search.c                                                             *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#ifndef ENABLE_HISTORY
PRIVATE char last_searched_needle[MAX_PATH_LEN+1] = "";
#endif // ENABLE_HISTORY

void set_last_searched_needle(const char* needle)
{
#ifndef ENABLE_HISTORY
	strlcpy__(last_searched_needle, needle, MAX_PATH_LEN);
#endif // ENABLE_HISTORY
}
const char* get_last_searched_needle()
{
#ifdef ENABLE_HISTORY
	return history_last_line_str(HISTORY_TYPE_IDX_SEARCH);
#else // ENABLE_HISTORY
	return last_searched_needle;
#endif // ENABLE_HISTORY
}

#define SEARCH0		0
#define REPLACE1	1

PRIVATE int found_in_prev_search = 1;
PRIVATE int direction_of_prev_search = 0;
PRIVATE be_line_t *line_of_prev_search = NULL;
PRIVATE int byte_idx_of_prev_search = 0;

PRIVATE int doe_search_first_(void);
PRIVATE int input_search_str(int search0_replace1, char *input_buf);
PRIVATE int input_replace_str(char *input_buf);

int doe_search_backward_first(void)
{
	SET_APPMD(ed_REVERSE_SEARCH);
	return doe_search_first_();
}
int doe_search_forward_first(void)
{
	CLR_APPMD(ed_REVERSE_SEARCH);
	return doe_search_first_();
}
PRIVATE int doe_search_first_(void)
{
	char needle[MAX_PATH_LEN+1];
	if (input_search_str(SEARCH0, needle) <= 0) {
		return 0;
	}
	memorize_cur_file_pos_before_jump();
	found_in_prev_search = 1;
	if (search_string_once(needle, 0))
		return 1;
	// not found in the first direction, search in the reverse direction
	TOGGLE_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(needle, 1);
}
int doe_search_backward_next(void)
{
	SET_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(get_last_searched_needle(HISTORY_TYPE_IDX_SEARCH), 0);
}
int doe_search_forward_next(void)
{
	CLR_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(get_last_searched_needle(HISTORY_TYPE_IDX_SEARCH), 0);
}

//------------------------------------------------------------------------------

// TODO: search wrap around of files
//  on forward search, end of the last file ==> top of the first file
//  on reverse search, top of the first file ==> end of the last file

// Replace a string
int doe_replace(void)
{
	char replace_from[MAX_PATH_LEN+1];
	char replace_to[MAX_PATH_LEN+1];
	char prev_file_pos[MAX_PATH_LEN+1];
	int num_replaced;

	if (is_editor_unmodifiable_then_warn_it()) {
		return 0;
	}

	CLR_APPMD(ed_REVERSE_SEARCH);

	if (input_search_str(REPLACE1, replace_from) <= 0) {
		return 0;
	}
	if (input_replace_str(replace_to) <= 0) {
		return 0;
	}
	conv_esc_str(replace_to);

	memorize_cur_file_pos_null(prev_file_pos);

	int ret = replace_string_loop(replace_from, replace_to, &num_replaced);

	if (ret == ANSWER_CANCEL) {
		// not return to original file pos
	} else
	if (ret == ANSWER_END) {
		// return to original file pos
		recall_file_pos_null(prev_file_pos);
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);

	if (num_replaced >= 0)
		disp_status_bar_done(P_(_("Replaced %d occurrence"),
								_("Replaced %d occurrences"),
								_("Replaced %d occurrencess"),
								_("Replaced %d occurrencesss"),
		 num_replaced), num_replaced);
	else
		disp_status_bar_not_found_msg(replace_from, 0);

	return 0;
}

//------------------------------------------------------------------------------

// search0_replace1 = 0: search, 1: replace
int input_search_str(int search0_replace1, char *input_buf)
{
	const char* last_needle = get_last_searched_needle(HISTORY_TYPE_IDX_SEARCH);
	char default_needle[MAX_PATH_LEN+1];

	if (strlen(last_needle)) {
		// We use central_win_get_columns() / 3 here because we need to see more on the line
		snprintf(default_needle, MAX_PATH_LEN+1, "(%.*s%s)",
		 central_win_get_columns() / 3, last_needle,
		 strlen(last_needle) > central_win_get_columns() / 3 ? "..." : "");
	} else {
		strcpy__(default_needle, "");
	}

	if (chk_inp_str_ret_val_editor(input_string_pos("", input_buf, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 "%s%s%s%s%s:",
	 search0_replace1 == 0 ? _("Search") : _("Replace"),
	 GET_APPMD(ed_IGNORE_CASE) ? _("[Ignore-case]") : _("[Case-sensitive]"),
#ifdef ENABLE_REGEX
	 GET_APPMD(ed_USE_REGEXP) ? _("[Regexp]") : "",
#else // ENABLE_REGEX
	 "",
#endif // ENABLE_REGEX
	 GET_APPMD(ed_REVERSE_SEARCH) ? _("[Backward]") : _("[Forward]"),
	 default_needle))) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
		return 0;						// cancelled
	}
	if (strlen(input_buf) == 0) {
		// nothing input, get last searched string
		strlcpy__(input_buf, last_needle, MAX_PATH_LEN);
	}
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP)) {
		if (regexp_compile(&search__.regexp, input_buf,
		 GET_APPMD(ed_IGNORE_CASE) ? REG_ICASE : 0)) {
			disp_status_bar_err(_("Invalid regexp: [%s]:%s"),
			 search__.regexp.needle_compiled, search__.regexp.regex_err_msg);
			set_edit_win_update_needed(UPDATE_SCRN_ALL);
			return -1;			// regexp error
		}
	}
#endif // ENABLE_REGEX
	set_last_searched_needle(input_buf);
	return 1;							// input normally
}

int input_replace_str(char *input_buf)
{
	if (chk_inp_str_ret_val_editor(input_string_pos("", input_buf, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 "%s:", _("Replace with:")))) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
		return 0;
	}
	set_last_searched_needle(input_buf);
	return 1;
}

//=============================================================================

search_t search__;
matches_t matches__;

//------------------------------------------------------------------------------

#define INNER_BUFFER_SEARCH		0
#define INTER_BUFFER_SEARCH		1
PRIVATE int search_needle_in_buffers(
 const char *needle, int search_dir, int ignore_case, int skip_here, int search_count);
PRIVATE int search_needle_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 const char *needle, int search_dir, int ignore_case, int skip_here, int global_search);

// Search for a string
int search_string_once(const char *needle, int search_count)
{
	const char* last_needle = get_last_searched_needle(HISTORY_TYPE_IDX_SEARCH);

	if (found_in_prev_search == 0 && direction_of_prev_search == SEARCH_DIR()
	 && line_of_prev_search == EPCBVC_CL && byte_idx_of_prev_search == EPCBVC_CLBI
	 && strcmp(last_needle, needle) == 0) {
		disp_status_bar_not_found_msg(needle, search_count);
		return 0;
	}

	int match_len = search_needle_in_buffers(needle,
	 SEARCH_DIR(), GET_APPMD(ed_IGNORE_CASE), SKIP_HERE, search_count);

	if (match_len > 0) {
		if (search_count == 0) {
			// first time
			if (GET_APPMD(ed_REVERSE_SEARCH)) {
				post_cmd_processing(NULL, CURS_MOVE_JUMP, LOCATE_CURS_JUMP_BACKWARD,
				 UPDATE_SCRN_ALL);
			} else {
				post_cmd_processing(NULL, CURS_MOVE_JUMP, LOCATE_CURS_JUMP_FORWARD,
				 UPDATE_SCRN_ALL);
			}
		} else {
			// next time
			post_cmd_processing(NULL, CURS_MOVE_JUMP, LOCATE_CURS_JUMP_CENTER,
			 UPDATE_SCRN_ALL);
		}
		disp_status_bar_done(_("[%s] found in %s search"), needle,
		 GET_APPMD(ed_REVERSE_SEARCH) ? _("Backward") : _("Forward"));
	}

	found_in_prev_search = match_len;
	direction_of_prev_search = SEARCH_DIR();
	line_of_prev_search = EPCBVC_CL;
	byte_idx_of_prev_search = EPCBVC_CLBI;
	return match_len;
}

//------------------------------------------------------------------------------
int replace_string_loop(const char *needle, const char *replace_to, int *num_replaced_)
{
	int num_replaced = 0;
#ifdef ENABLE_UNDO
	int num_undone = 0;
#endif // ENABLE_UNDO
	int skip_here;
	int ret = 0;
	long length_change;
	int prev_ed_REVERSE_SEARCH;
	key_code_t key;

	prev_ed_REVERSE_SEARCH = GET_APPMD(ed_REVERSE_SEARCH);
	for (skip_here = NO_SKIP_HERE; ; ) {
		if (prev_ed_REVERSE_SEARCH != GET_APPMD(ed_REVERSE_SEARCH)) {
			prev_ed_REVERSE_SEARCH = GET_APPMD(ed_REVERSE_SEARCH);
			// search direction changed, not skip here on the next search
			skip_here = NO_SKIP_HERE;
		}

		int match_len = search_needle_in_buffers(needle,
		 SEARCH_DIR(), GET_APPMD(ed_IGNORE_CASE), skip_here, 0);

		if (match_len) {
			// found
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_CENTER,
			 UPDATE_SCRN_ALL);
			update_screen_editor(1, 1);
			if (ret < ANSWER_ALL) {
#ifdef ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO | ASK_ALL | ASK_BACKWARD | ASK_FORWARD | ASK_END
				 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0),
#else // ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO | ASK_ALL | ASK_BACKWARD | ASK_FORWARD | ASK_END,
#endif // ENABLE_UNDO
				 _("Replace from [%s] to [%s] ?"), needle, replace_to);
			} else {
				// break ALL-replacing loop
				if ((key = tio_input_key()) >= 0) {
					if (key == K_C_C) {
						ret = ANSWER_CANCEL;
					} else
					if (key == K_ESC) {
						ret = ANSWER_END;
					}
				}
			}
		} else {
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_CENTER,
			 UPDATE_SCRN_ALL);
			update_screen_editor(1, 1);
			// not found message has been displayed
#ifdef ENABLE_UNDO
			ret = ask_yes_no(ASK_NO | ASK_BACKWARD | ASK_FORWARD | ASK_END
			 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0),
#else // ENABLE_UNDO
			ret = ask_yes_no(ASK_NO | ASK_BACKWARD | ASK_FORWARD | ASK_END,
#endif // ENABLE_UNDO
			 _("Keyword [%s] not found"), needle);
		}
		if (ret == ANSWER_NO) {
			// Not replace and search next
			skip_here = SKIP_HERE;
			continue;
		} else if (ret == ANSWER_FORWARD) {
			// forward search
			CLR_APPMD(ed_REVERSE_SEARCH);
			skip_here = SKIP_HERE;
			continue;
		} else if (ret == ANSWER_BACKWARD) {
			// backward search
			SET_APPMD(ed_REVERSE_SEARCH);
			skip_here = SKIP_HERE;
			continue;
		} else if (ret == ANSWER_YES || ret == ANSWER_ALL) {
			if (match_len) {
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
				memorize_undo_state_before_change(NULL);
#endif // ENABLE_DEBUG
				undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO
				// ----------------------------------------------------------------------
				length_change = replace_str_in_buffer(&search__, &matches__, replace_to);
				// ----------------------------------------------------------------------
#ifdef ENABLE_UNDO
				undo_save_after_change();
#ifdef ENABLE_DEBUG
				if (check_undo_state_after_change()) { _WARNING_ }
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
				// Set the cursor at the last character of the replacement
				// text, so searching will resume after the replacement text. */
				if (GET_APPMD(ed_REVERSE_SEARCH) == 0) {
					// forward search
					EPCBVC_CLBI += matches_match_len(&matches__) + length_change;
					skip_here = NO_SKIP_HERE;	// EPCBVC_CLBI already forwarded to skip word
				} else {
					// backward search
					skip_here = SKIP_HERE;		// skip
				}
				get_epc_buf()->buf_size += length_change;
				set_cur_buf_modified();
				num_replaced++;
			}
#ifdef ENABLE_UNDO
		} else if (ret == ANSWER_UNDO) {
			// undo replace
			doe_undo();
			num_replaced--;
			num_undone++;
			skip_here = NO_SKIP_HERE;
			continue;
		} else if (ret == ANSWER_REDO) {
			// redo replace
			doe_redo();
			num_undone--;
			num_replaced++;
			skip_here = NO_SKIP_HERE;
			continue;
#endif // ENABLE_UNDO
		} else {			// Cancel
			break;
		}
	}
	*num_replaced_ = num_replaced;
	if (num_replaced && strlen(replace_to)) {
		// copy [string replace_to] ==> [next string to search]
		set_last_searched_needle(replace_to);
	}
	return ret;
}

int replace_str_in_buffer(search_t *search, matches_t *matches, const char *replace_to)
{
	char before[MAX_EDIT_LINE_LEN+1];
	char after[MAX_EDIT_LINE_LEN+1];

	strlcpy__(before, EPCBVC_CL->data, MAX_EDIT_LINE_LEN);
	strlcpy__(after, EPCBVC_CL->data, MAX_EDIT_LINE_LEN);
	// replace in buffer
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP) == 0) {
#endif // ENABLE_REGEX
		replace_str(after, MAX_EDIT_LINE_LEN,
		 matches_start_idx(matches), matches_match_len(matches),
		 replace_to, -1);
#ifdef ENABLE_REGEX
	} else {
		regexp_replace(&search->regexp, &matches->regexp_matches,
		 after, MAX_EDIT_LINE_LEN, replace_to);
	}
#endif // ENABLE_REGEX
	line_set_string(EPCBVC_CL, after);		// replace whole of the line
	return strlen(after) - strlen(before);
}

//------------------------------------------------------------------------------

#ifdef ENABLE_REGEX
PRIVATE int do_find_bracket_(int search1_hilight0, int reverse_pair);

// [test string for bracket search]--------------------------------------------
// 12345678901234567890123456789012345678901234567890
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
//  }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
//  {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
// }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// (((((((((((((((((((((((((((((((((((((((((((((((((((
//  ))))))))))))))))))))))))))))))))))))))))))))))))))
//  ((((((((((((((((((((((((((((((((((((((((((((((((((
// )))))))))))))))))))))))))))))))))))))))))))))))))))
// {{}}} <<>>> {{{}} <<<>>
// }}{{{ }}}{{ >><<< >>><<
int doe_find_bracket(void)
{
	return do_find_bracket_(1, FORWARD_SEARCH);
}
int doe_find_bracket_reverse(void)
{
	return do_find_bracket_(1, BACKWARD_SEARCH);
}
int doe_highlight_bracket(void)
{
	return do_find_bracket_(0, FORWARD_DIR);
}
int doe_highlight_bracket_reverse(void)
{
	return do_find_bracket_(0, BACKWARD_DIR);
}
PRIVATE int do_find_bracket_(int search1_hilight0, int reverse_pair)
{
	char char_under_cursor;
	char needle[BRACKET_SEARCH_REGEXP_STR_LEN+1];
	int search_dir;			// search direction (FORWARD_SEARCH / BACKWARD_SEARCH)
	be_line_t *line;
	int byte_idx;
	int depth;
	int skip_here;
	int safe_cnt = 0;

	char_under_cursor = *EPCBVC_CL_EPCBVC_CLBI;
	search_dir = setup_bracket_search(char_under_cursor, reverse_pair, needle);
	if (search_dir == 0) {
		disp_status_bar_err(_("Not a bracket"));
		return 1;
	}
	search__.direction = reverse_pair;
	SET_APPMD(ed_USE_REGEXP);

	set_item_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();

	line = EPCBVC_CL;
	byte_idx = EPCBVC_CLBI;

	memorize_cursor_pos_before_move();

	skip_here = 0;
	int match_len;
	for (match_len = 0, depth = 0, safe_cnt = 0; safe_cnt < MAX_BRACKETS_SEARCH; safe_cnt++) {
		match_len = search_bracket_in_buffer(&line, &byte_idx,
		 char_under_cursor, needle, search_dir, skip_here, FORWARD_DIR, &depth, NULL);
		// repeat until the counterpart bracket found
		if ((depth <= 0) || (MAX_BRACKET_NESTINGS <= depth) || (match_len == 0)) {
			// counterpart bracket found, nesting too deep or no bracket found
			break;
		}
		skip_here = 1;
	}
	if ((match_len > 0) && (depth == 0)) {
		// found peer bracket
		disp_status_bar_done(_("Counterpart bracket found"));
	} else if (depth < MAX_BRACKET_NESTINGS) {
		// didn't find peer bracket
		if (safe_cnt < MAX_BRACKETS_SEARCH) {
			disp_status_bar_warn(_("No peer bracket found"));
		} else {
			disp_status_bar_warn(_("Too many bracket pairs (%d)"), MAX_BRACKETS_SEARCH);
		}
	} else {
		// didn't find peer bracket
		disp_status_bar_warn(_("Bracket nesting too deep (%d)"), MAX_BRACKET_NESTINGS);
	}
	if (depth == 0) {
		// found peer bracket
		if (search1_hilight0) {
			EPCBVC_CL = line;
			EPCBVC_CLBI = byte_idx;
		}
		if (search_dir < 0) {
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_BACKWARD,
			 UPDATE_SCRN_ALL);
		} else {
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_FORWARD,
			 UPDATE_SCRN_ALL);
		}
	}
	return 0;
}

int setup_bracket_search(char char_under_cursor, int reverse_pair, char *needle)
{
	const char *brackets = "([{<>}])";
	const char *ptr;
	char char_counterpart;
	char regexp_str[] = "[xy]";
	int offset;
	int search_dir;			// search direction (FORWARD_SEARCH / BACKWARD_SEARCH)

	if ((ptr = strchr__(brackets, char_under_cursor)) == NULL) {
		return 0;
	}
	offset = ptr - brackets;
	char_counterpart = brackets[strlen(brackets) - (offset + 1)];

	// apparent near redundancy with regexp_str[] here is needed,
	// "[][]" works, "[[]]" doesn't
	if (offset < (strlen(brackets) / 2)) {	// on a left bracket
		regexp_str[1] = char_counterpart;	// ']'
		regexp_str[2] = char_under_cursor;	// '['
		if (reverse_pair >= 0) {
			// (0)
			// ^
			search_dir = FORWARD_SEARCH;	// forward
		} else {
			// )0(
			//   ^
			search_dir = BACKWARD_SEARCH;	// backward
		}
	} else {								// on a right bracket
		regexp_str[1] = char_under_cursor;	// ']'
		regexp_str[2] = char_counterpart;	// '['
		if (reverse_pair >= 0) {
			// (0)
			//   ^
			search_dir = BACKWARD_SEARCH;	// backward
		} else {
			// )0(
			// ^
			search_dir = FORWARD_SEARCH;	// forward
		}
	}

	if (needle) {
		strlcpy__(needle, regexp_str, BRACKET_SEARCH_REGEXP_STR_LEN);
	}
flf_d_printf("search_dir/needle: [%d][%s]\n", search_dir, needle);
	return search_dir;
}

int search_bracket_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 char char_under_cursor, const char *needle, int search_dir, int skip_here, char depth_increase,
 int *ptr_depth, int *prev_depth)
{
	int match_len = search_needle_in_buffer(ptr_line, ptr_byte_idx,
	 needle, search_dir, CASE_SENSITIVE, skip_here, INNER_BUFFER_SEARCH);
	if (match_len > 0) {
		// found bracket
		if ((*ptr_line)->data[*ptr_byte_idx] == char_under_cursor) {
			// found original bracket ==> increase depth
			if (prev_depth) {
				*prev_depth = *ptr_depth;
			}
			(*ptr_depth) += depth_increase;	// post increment/decrement
		} else {
			// found counterpart bracket ==> decrease depth
			(*ptr_depth) -= depth_increase;	// pre increment/decrement
			if (prev_depth) {
				*prev_depth = *ptr_depth;
			}
		}
	}
	return match_len;
}

// color management for bracket highlighting
#define COLORS_FOR_BRACKET_HL	((COLORS8)+1)	// color pairs for bracket highlighting
PRIVATE int num_colors_for_bracket_hl = 0;	// [0, COLORS_FOR_BRACKET_HL]
PRIVATE item_color_t colors_for_bracket_hl[COLORS_FOR_BRACKET_HL];

void prepare_colors_for_bracket_hl()
{
	if (num_colors_for_bracket_hl) {
		// already prepared
		return;
	}
	char bgc_sel;
	char fgc_sel;
	char bgc_sel2;
	char fgc;
	get_item_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED1, &bgc_sel, &fgc_sel);
	get_item_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, &bgc_sel2, &fgc);
	int color_idx = 0;
	colors_for_bracket_hl[color_idx].bgc = bgc_sel;
	colors_for_bracket_hl[color_idx].fgc = fgc_sel;
	color_idx++;
	for (fgc = fgc_sel+1; color_idx < COLORS_FOR_BRACKET_HL; fgc++) {
		fgc = CL_HI | (fgc % COLORS8);
		if (fgc != (bgc_sel2 % COLORS8)) {	// Because there is no light color in BGC
			colors_for_bracket_hl[color_idx].bgc = bgc_sel2;
			colors_for_bracket_hl[color_idx].fgc = fgc;
			color_idx++;
		}
	}
	num_colors_for_bracket_hl = color_idx;
#ifdef ENABLE_DEBUG
	for (color_idx = 0; color_idx < num_colors_for_bracket_hl; color_idx++) {
		flf_d_printf("color_idx: %d, bgc: %2d, fgc: %2d\n", color_idx,
		 colors_for_bracket_hl[color_idx].bgc,
		 colors_for_bracket_hl[color_idx].fgc);
	}
#endif // ENABLE_DEBUG
}
int get_colors_for_bracket_hl()
{
	prepare_colors_for_bracket_hl();
	return num_colors_for_bracket_hl;
}
// depth_increase > 0, depth_0_occurances < 2
//   *               *
//   0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 ...
//   <-------------> <-------------> <------------->
//
// depth_increase > 0, depth_0_occurances >= 2
//   8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 ...
//   <-------------> <-------------> <------------->
//
// depth_increase > 0
//   8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1 8 7 6 5 4 3 2 1 ...
//   <-------------> <-------------> <------------->
int get_color_idx_for_bracket_hl(char depth_increase, UINT8 *depth_0_occurances, int depth)
{
	int num_colors_m1 = get_colors_for_bracket_hl() - 1;	// 8
	int color_idx = depth % num_colors_m1;	// [-8, 8-1]
	color_idx += num_colors_m1;				// [0, 8*2-1]
	color_idx %= num_colors_m1;				// [0, 8-1]
	if ((depth_increase < 0) || (*depth_0_occurances >= 2)) {
		if (color_idx == 0) {
			color_idx = num_colors_m1;		// 0 ==> 8
		}
	}
	if (color_idx == 0) {
		(*depth_0_occurances)++;
	}
	return color_idx;
}
void set_color_for_bracket_hl(char depth_increase, UINT8 *depth_0_occurances, int depth)
{
	set_color_for_bracket_hl_by_idx(
	 get_color_idx_for_bracket_hl(depth_increase, depth_0_occurances, depth));
}
void set_color_for_bracket_hl_by_idx(int color_idx)
{
	char fgc, bgc;
	get_color_for_bracket_hl_by_idx(color_idx, &bgc, &fgc);
	tio_set_attrs(bgc, fgc, 0);
}
void get_color_for_bracket_hl_by_idx(int color_idx, char *bgc, char *fgc)
{
	if (get_colors_for_bracket_hl() == 0) {
		get_item_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED1, bgc, fgc);
	} else {
		color_idx = MK_IN_RANGE(0, color_idx, COLORS_FOR_BRACKET_HL);
		*bgc = colors_for_bracket_hl[color_idx].bgc;
		*fgc = colors_for_bracket_hl[color_idx].fgc;
	}
}

#endif // ENABLE_REGEX

//------------------------------------------------------------------------------

PRIVATE int search_needle_in_buffers(
 const char *needle, int search_dir, int ignore_case, int skip_here, int search_count)
{
	disp_status_bar_ing(_("Searching word: [%s]..."), needle);

	int match_len = search_needle_in_buffer(&(EPCBVC_CL), &(EPCBVC_CLBI),
	 needle, search_dir, ignore_case, skip_here, INTER_BUFFER_SEARCH);

	if (match_len > 0) {
		// found
		return match_len;
	}
	tio_beep();
	disp_status_bar_not_found_msg(needle, search_count);
	return 0;
}

// search keyword in buffer (when global_search is false) or buffers (when true).
// when keyword not found, return to original cursor position.
// WARNING: When "global_search" is true, epc_buf may be changed.
PRIVATE int search_needle_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 const char *needle, int search_dir, int ignore_case, int skip_here, int global_search)
{
	be_line_t *line;
	int byte_idx;
	int match_len;

	line = *ptr_line;
	byte_idx = *ptr_byte_idx;
	match_len = 0;

	memorize_cur_file_pos_null(NULL);
	memorize_cursor_pos_before_move();

	if (search_dir <= BACKWARD_SEARCH) {
		// search backward -----------------------------------------------------
		for ( ; ; ) {
			if (skip_here) {
				// move cur-pos left at least one char
				// if cur-pos is left most, move cur-pos up at least one line
				if (byte_idx > 0) {
					byte_idx--;
				} else if (IS_NODE_TOP_MOST(line) == 0) {
					line = NODE_PREV(line);
					byte_idx = line_strlen(line);
				} else if (global_search && switch_epc_buf_to_prev_buf(0, 0)) {
					// update local pointers after switching buffer
					// but not update pointers in buffer
					ptr_line = &(EPCBVC_CL);
					ptr_byte_idx = &(EPCBVC_CLBI);
					line = CUR_EDIT_BUF_BOT_LINE;
					byte_idx = line_strlen(line);
				} else {
					break;
				}
			}
			// look for needle backward in haystack
			if ((match_len = search_str_in_line(&search__, &matches__,
			 needle, search_dir, ignore_case, line->data, byte_idx)) > 0) {
				// found
				break;
			}
			byte_idx = 0;
			skip_here = 1;
		}
	} else {
		// search forward ------------------------------------------------------
		for ( ; ; ) {
			if (skip_here) {
				// move cur-pos right at least one char
				// if cur-pos is right most, move cur-pos down at least one line
				if (byte_idx < line_strlen(line)) {
					byte_idx++;
				} else if (IS_NODE_BOT_MOST(line) == 0) {
					line = NODE_NEXT(line);
					byte_idx = 0;
				} else if (global_search && switch_epc_buf_to_next_buf(0, 0)) {
					// update local pointers after switching buffer
					// but not update pointers in buffer
					ptr_line = &(EPCBVC_CL);
					ptr_byte_idx = &(EPCBVC_CLBI);
					line = CUR_EDIT_BUF_TOP_LINE;
					byte_idx = 0;
				} else {
					break;
				}
			}
			// look for needle forward in haystack
			if ((match_len = search_str_in_line(&search__, &matches__,
			 needle, search_dir, ignore_case, line->data, byte_idx)) > 0) {
				// found
				break;
			}
			byte_idx = line_strlen(line);
			skip_here = 1;
		}
	}

	if (match_len > 0) {
		// found and update current line pointer
		*ptr_line = line;
		*ptr_byte_idx = matches_start_idx(&matches__);
		return match_len;
	}
	// not found then return to beginning position
	recall_file_pos_null(NULL);
	return match_len;
}

// search_count = 0: first uni-directional search
// search_count = 1: second uni-directional search (bi-directional search done)
void disp_status_bar_not_found_msg(const char *str, int search_count)
{
	disp_status_bar_warn(_("Keyword [%s] not found in %s search"),
	 shrink_str_to_scr_static(str), (search_count == 0)
	  ? ((SEARCH_DIR() < 0) ? _("Backward") : _("Forward")) : _("Bi-directional"));
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}

// End of search.c
