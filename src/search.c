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

#ifdef ENABLE_HISTORY
// define this to share it among BE editor instances
#define GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
#endif // ENABLE_HISTORY

#ifndef GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
PRIVATE char last_searched_needle[MAX_PATH_LEN+1] = "";
#endif // GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY

void set_last_searched_needle(const char *needle)
{
#ifndef GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
	strlcpy__(last_searched_needle, needle, MAX_PATH_LEN);
#endif // GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
}
const char *get_last_searched_needle()
{
#ifndef GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
	return last_searched_needle;
#else // GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
	return history_last_line_str(HISTORY_TYPE_IDX_SEARCH);
#endif // GET_LAST_SEARCHED_NEEDLE_FROM_HISTORY
}

#define SEARCH0		0
#define REPLACE1	1

PRIVATE int found_in_prev_search = 1;
PRIVATE int direction_of_prev_search = 0;
PRIVATE be_line_t *line_of_prev_search = NULL;
PRIVATE int byte_idx_of_prev_search = 0;

PRIVATE int doe_search_first_();
PRIVATE int input_search_str(int search0_replace1, char *input_buf);
PRIVATE int input_replace_str(char *input_buf);

#define FIRST_ATTEMPT_0		0
#define LAST_ATTEMPT_1		1
PRIVATE int search_str_once(const char *needle, int attempt_count);
PRIVATE int replace_str_loop(const char *replace_from, const char *replace_to, int *num_replaced_);
PRIVATE int replace_str_in_buffer(search_t *search, matches_t *matches, const char *replace_to);
PRIVATE void disp_status_bar_not_found_msg(const char *str, int attempt_count);

void doe_search_first_backward()
{
	SET_APPMD(ed_REVERSE_SEARCH);
	doe_search_first_();
}
void doe_search_first_forward()
{
	CLR_APPMD(ed_REVERSE_SEARCH);
	doe_search_first_();
}
PRIVATE int doe_search_first_()
{
	char needle[MAX_PATH_LEN+1];
	if (input_search_str(SEARCH0, needle) <= 0) {
		return 0;
	}
	found_in_prev_search = 1;
	if (search_str_once(needle, FIRST_ATTEMPT_0))
		return 1;
	// not found in the first direction, search in the reverse direction
	TOGGLE_APPMD(ed_REVERSE_SEARCH);
	return search_str_once(needle, LAST_ATTEMPT_1);
}
void doe_search_next_backward()
{
	SET_APPMD(ed_REVERSE_SEARCH);
	search_str_once(get_last_searched_needle(), LAST_ATTEMPT_1);
}
void doe_search_next_forward()
{
	CLR_APPMD(ed_REVERSE_SEARCH);
	search_str_once(get_last_searched_needle(), LAST_ATTEMPT_1);
}

//------------------------------------------------------------------------------

// Replace a string
void doe_replace()
{
	char replace_from[MAX_PATH_LEN+1];
	char replace_to[MAX_PATH_LEN+1];
	char prev_file_pos[MAX_PATH_LEN+1];
	int num_replaced;

	if (is_editor_unmodifiable_then_warn_it()) {
		return;
	}

	CLR_APPMD(ed_REVERSE_SEARCH);

	if (input_search_str(REPLACE1, replace_from) <= 0) {
		return;
	}
	if (input_replace_str(replace_to) <= 0) {
		return;
	}
	conv_esc_str(replace_to);

	memorize_cur_file_pos_into_str(prev_file_pos);

	int ret = replace_str_loop(replace_from, replace_to, &num_replaced);

	if (ret == ANSWER_CANCEL) {
		// not return to original file pos
	} else
	if (ret == ANSWER_END) {
		// return to original file pos
		recall_file_pos_from_str(prev_file_pos);
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);

	if (num_replaced >= 0) {
		disp_status_bar_done(P_(_("Replaced %d occurrence"),
								_("Replaced %d occurrences"),
								_("Replaced %d occurrencess"),
								_("Replaced %d occurrencesss"),
		 num_replaced), num_replaced);
	} else {
		disp_status_bar_not_found_msg(replace_from, FIRST_ATTEMPT_0);
	}
}

//------------------------------------------------------------------------------

// search0_replace1 = 0: search, 1: replace
int input_search_str(int search0_replace1, char *input_buf)
{
	const char *last_needle = get_last_searched_needle();
	char default_needle[MAX_PATH_LEN+1];

	if (strlen(last_needle)) {
		// We use central_win_get_columns() / 3 here because we need to see more on the line
		snprintf(default_needle, MAX_PATH_LEN+1, "(%.*s%s)",
		 central_win_get_columns() / 3, last_needle,
		 (strlen(last_needle) > (central_win_get_columns() / 3)) ? "..." : "");
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
	SET_app_do_next(EF_NONE);
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
	SET_app_do_next(EF_NONE);
	return 1;
}

//=============================================================================

search_t search__;
matches_t matches__;

//------------------------------------------------------------------------------

#define INNER_BUFFER_SEARCH		0
#define INTER_BUFFER_SEARCH		1
PRIVATE int search_needle_in_bufs_not_found_msg(
 const char *needle, int search_dir, int ignore_case, int skip_here, int attempt_count);
PRIVATE int search_needle_in_bufs(be_line_t **ptr_line, int *ptr_byte_idx,
 const char *needle, int search_dir, int ignore_case, int skip_here, int global_search);

// Search for a string
int search_str_once(const char *needle, int attempt_count)
{
	const char *last_needle = get_last_searched_needle();

	memorize_cur_file_pos_before_jump();
	if (found_in_prev_search == 0 && direction_of_prev_search == SEARCH_DIR()
	 && line_of_prev_search == EPCBVC_CL && byte_idx_of_prev_search == EPCBVC_CLBI
	 && (strcmp(last_needle, needle) == 0)) {
		disp_status_bar_not_found_msg(needle, attempt_count);
		return 0;
	}

	int match_len = search_needle_in_bufs_not_found_msg(needle,
	 SEARCH_DIR(), GET_APPMD(ed_IGNORE_CASE), SKIP_HERE, attempt_count);

	if (match_len > 0) {
		if (attempt_count == 0) {
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
		memorize_prev_file_pos_if_changed();
		disp_status_bar_done(_("Keyword [%s] found in %s search"), needle,
		 (SEARCH_DIR() < 0) ? _("Backward") : _("Forward"));
	}

	found_in_prev_search = match_len;
	direction_of_prev_search = SEARCH_DIR();
	line_of_prev_search = EPCBVC_CL;
	byte_idx_of_prev_search = EPCBVC_CLBI;
	return match_len;
}

//------------------------------------------------------------------------------
int replace_str_loop(const char *needle, const char *replace_to, int *num_replaced_)
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

		int match_len = search_needle_in_bufs_not_found_msg(needle,
		 SEARCH_DIR(), GET_APPMD(ed_IGNORE_CASE), skip_here, LAST_ATTEMPT_1);

		if (match_len) {
			// found
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_CENTER,
			 UPDATE_SCRN_ALL);
			update_screen_editor(S_B_CURS, 1);
			if (ret < ANSWER_ALL) {
#ifdef ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO_QUIT | ASK_ALL_YES | ASK_END
				 | ASK_BACKWARD | ASK_FORWARD
				 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0),
#else // ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO_QUIT | ASK_ALL_YES | ASK_END
				 | ASK_BACKWARD | ASK_FORWARD,
#endif // ENABLE_UNDO
				 _("Replace from [%s] to [%s] ?"), needle, replace_to);
			} else {
				// break ALL-replacing loop
				if ((key = tio_input_key()) >= 0) {
					if (key == K_C_c) {
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
			update_screen_editor(S_B_CURS, 1);
			// not found message has been displayed
#ifdef ENABLE_UNDO
			ret = ask_yes_no(ASK_NO_QUIT | ASK_END | ASK_BACKWARD | ASK_FORWARD
			 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0),
#else // ENABLE_UNDO
			ret = ask_yes_no(ASK_NO_QUIT | ASK_END | ASK_BACKWARD | ASK_FORWARD,
#endif // ENABLE_UNDO
			 _("Keyword [%s] NOT found in %s search"), needle,
			 (SEARCH_DIR() < 0) ? _("Backward") : _("Forward"));
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
				memorize_undo_state_before_change(__FUNCTION__);
#endif // ENABLE_DEBUG
				undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO
				// ----------------------------------------------------------------------
				length_change = replace_str_in_buffer(&search__, &matches__, replace_to);
				// ----------------------------------------------------------------------
#ifdef ENABLE_UNDO
				undo_save_after_change();
#ifdef ENABLE_DEBUG
				if (check_undo_state_after_change()) { progerr_printf("\n"); }
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
				// Set the cursor at the last character of the replacement
				// text, so searching will resume after the replacement text. */
				if (GET_APPMD(ed_REVERSE_SEARCH) == 0) {
					// forward search
					set_EPCBVC_CLBI(EPCBVC_CLBI + matches_match_len(&matches__) + length_change);
					skip_here = NO_SKIP_HERE;	// EPCBVC_CLBI already forwarded to skip word
				} else {
					// backward search
					skip_here = SKIP_HERE;		// skip
				}
				get_epc_buf()->buf_size += length_change;
				set_cur_ebuf_modified();
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
PRIVATE int _doe_find_bracket(int single1_multi2, int rev_pairing, int jump);
PRIVATE void get_color_for_bracket_hl_by_idx(int color_idx, char *bgc, char *fgc);
#define NORMAL_PAIRING		FORWARD_DIR		// { }
#define REVERSE_PAIRING		BACKWARD_DIR	// } {

void doe_find_bracket()
{
	_doe_find_bracket(SRCH_MODE_SINGLE_BRAC_1, NORMAL_PAIRING, 1);
}
void doe_find_bracket_reverse()
{
	_doe_find_bracket(SRCH_MODE_SINGLE_BRAC_1, REVERSE_PAIRING, 1);
}
void doe_find_brackets()
{
	_doe_find_bracket(SRCH_MODE_MULTI_BRAC_2, NORMAL_PAIRING, 1);
}
void doe_find_brackets_reverse()
{
	_doe_find_bracket(SRCH_MODE_MULTI_BRAC_2, REVERSE_PAIRING, 1);
}
PRIVATE int _doe_find_bracket(int single1_multi2, int rev_pairing, int jump)
{
	search_clear(&search__);

	// 1. setup bracket search =========
	char needle[BRACKET_SEARCH_REGEXP_STR_LEN+1];
	// search direction (FORWARD_SEARCH / BACKWARD_SEARCH)
	int search_dir = setup_bracket_search(single1_multi2, EPCBVC_CL_DATA, EPCBVC_CLBI,
	 rev_pairing, needle);
	if (search_dir == 0) {
		disp_status_bar_err(_("Not a bracket"));
		return 1;
	}
	search_set_mode(&search__, single1_multi2, search_dir, 0);
	search_set_needle(&search__, needle);

	// 2. jump to the peer bracket =========
	SET_APPMD(ed_USE_REGEXP);

	set_item_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();

	be_line_t *line = EPCBVC_CL;
	int byte_idx = EPCBVC_CLBI;

	memorize_cursor_pos_before_move();

	int skip_chars = 0;
	int match_len = 0;
	int depth = 0;
	int safe_cnt = 0;
	char char_under_cursor = *EPCBVC_CL_EPCBVC_CLBI;
	for ( ; safe_cnt < MAX_BRACKETS_SEARCH; safe_cnt++) {
		match_len = search_bracket_within_buffer(&line, &byte_idx, char_under_cursor,
		 needle, search_dir, skip_chars, FORWARD_DIR, &depth, NULL);
		// repeat until the counterpart bracket found
		if ((depth <= 0) || (MAX_BRACKET_NESTINGS <= depth) || (match_len == 0)) {
			// counterpart bracket found, nesting too deep or no bracket found
			break;
		}
		skip_chars = match_len;
	}
	if ((match_len > 0) && (depth == 0)) {
		// found peer bracket
		disp_status_bar_done(_("Counterpart bracket found"));
#ifdef ENABLE_HISTORY
		modify_history_w_reloading(HISTORY_TYPE_IDX_SEARCH, needle);
#endif // ENABLE_HISTORY
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
		if (jump) {
			set_EPCBVC_CL(line);
			set_EPCBVC_CLBI(byte_idx);
			// cursor jumped to the peer bracket and the search direction needs reversed
			search_set_mode(&search__, single1_multi2, - search_dir, 0);
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

// 1 character            : '{' or '}"   ==> "[\{\}]"
// more than 2 characters : '{{' or '}}' ==> "(\{\{|\}\})"

// [test string for bracket search]
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
// {{{{ {{{ {{ { } << > { } }} }}} }}}} < >>
// }}{{{ }}}{{ >><<< >>><<
// ＜＞｛｝（）［］    「」
// (([[{{<< >>}}]]))

int setup_bracket_search(int single1_multi2, char *str, int off, int rev_pairing, char *needle)
{
	const char *counterparts = "([{<>}])";
	const char *ptr;
	const char regexp_templ_str[] = "(%s|%s)";	// "(\{\{|\}\})" or "(<<|>>)"

	char char_under_cursor = str[off];
	if ((ptr = strchr__(counterparts, char_under_cursor)) == NULL) {
		return 0;			// not a bracket
	}
	int offset = ptr - counterparts;
	char char_counterpart = counterparts[strlen(counterparts) - (offset + 1)];

	char buf1[BRACKET_SEARCH_REGEXP_STR_LEN + 1] = "";
	char buf2[BRACKET_SEARCH_REGEXP_STR_LEN + 1] = "";
	int max_chars = 1;
	if (single1_multi2 >= 2) {
		max_chars = MAX_NUM_OF_BRACKET_CHAR;
	}
	int chars = 0;
	for ( ; chars < max_chars; chars++) {
		if (str[off + chars] != char_under_cursor) {
			break;
		}
	}
	for (int chrs = 0; chrs < chars; chrs++) {
		strlcat__(buf1, BRACKET_SEARCH_REGEXP_STR_LEN,
		 regexp_escape_special_char_s(char_under_cursor));
	}
	for (int chrs = 0; chrs < chars; chrs++) {
		strlcat__(buf2, BRACKET_SEARCH_REGEXP_STR_LEN,
		 regexp_escape_special_char_s(char_counterpart));
	}
	snprintf_(needle, BRACKET_SEARCH_REGEXP_STR_LEN+1, regexp_templ_str, buf1, buf2);
	int search_dir;			// search direction (FORWARD_SEARCH / BACKWARD_SEARCH)
	if (offset < (strlen(counterparts) / 2)) {	// char_under_cursor is one of left brackets "([<{"
		if (rev_pairing > 0) {
			search_dir = FORWARD_SEARCH;		// forward
		} else {								// on a right bracket
			search_dir = BACKWARD_SEARCH;		// backward
		}
	} else {									// on a right bracket
		if (rev_pairing > 0) {
			search_dir = BACKWARD_SEARCH;		// backward
		} else {								// on a right bracket
			search_dir = FORWARD_SEARCH;		// forward
		}
	}
	return search_dir;
}

int search_bracket_within_buffer(be_line_t **ptr_line, int *ptr_byte_idx, char char_under_cursor,
 const char *needle, int search_dir, int skip_chars, char depth_increase,
 int *ptr_depth, int *prev_depth)
{
	int match_len = search_needle_in_bufs(ptr_line, ptr_byte_idx,
	 needle, search_dir, CASE_SENSITIVE, skip_chars, INNER_BUFFER_SEARCH);
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
PRIVATE int colors_for_bracket_hl_prepared = 0;		// COLORS_FOR_BRACKET_HL
PRIVATE item_color_t colors_for_bracket_hl[COLORS_FOR_BRACKET_HL];

void prepare_colors_for_bracket_hl()
{
	if (colors_for_bracket_hl_prepared) {
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
		fgc = CL_HI | (fgc % COLORS8);		// use hi color for foreground
		if (fgc != (bgc_sel2 % COLORS8)) {	// Because there is no light color in BGC
			colors_for_bracket_hl[color_idx].bgc = bgc_sel2;
			colors_for_bracket_hl[color_idx].fgc = fgc;
			color_idx++;
		}
	}
	colors_for_bracket_hl_prepared = 1;
}
int get_colors_for_bracket_hl()
{
	return COLORS_FOR_BRACKET_HL;
}

//             color index ------------------------
// Example-1: searching to forward direction
//  depth      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 ...
//  color_idx  0  1  2  3  4  5  6  7  0  1  2  3  4  5  6  7  8  1  2  3  4
// Example-2:
//  depth      0 -1 -2 -3 -4 -5 -6 -7 -8 -9-10 ...
//  color_idx  0  7  6  5  4  3  2  1  8  7  6 ...
// Example-3: searching to backward direction
//  depth      0 -1  0  1  2  3 ...
//  color_idx  0  7  8  1  2  3 ...

int get_color_idx_for_bracket_hl(char depth_increase, UINT8 *depth_0_occurances, int depth)
{
	prepare_colors_for_bracket_hl();

	int num_colors_m1 = get_colors_for_bracket_hl() - 1;	// 8 = 9 - 1
	int color_idx = depth % num_colors_m1;	// [-(8-1), 8-1]
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

PRIVATE int search_needle_in_bufs_not_found_msg(
 const char *needle, int search_dir, int ignore_case, int skip_here, int attempt_count)
{
	disp_status_bar_ing(_("Searching word: [%s]..."), needle);

	search_set_mode(&search__, SRCH_MODE_STRING_0, search_dir, ignore_case);
	search_set_needle(&search__, needle);

	int match_len = search_needle_in_bufs(&(EPCBVC_CL), &(EPCBVC_CLBI),
	 needle, search_dir, ignore_case, skip_here, INTER_BUFFER_SEARCH);

	if (match_len > 0) {
		// found
		return match_len;
	}
	tio_beep();
	disp_status_bar_not_found_msg(needle, attempt_count);
	return 0;
}

// search wrap around of files
//  on forward search, end of the last file ==> top of the first file
//  on reverse search, top of the first file ==> end of the last file
// search keyword in buffer (when global_search is false) or buffers (when true).
// when keyword not found, return to original cursor position.
// WARNING: When "global_search" is true, epc_buf may be changed.
PRIVATE int search_needle_in_bufs(be_line_t **ptr_line, int *ptr_byte_idx,
 const char *needle, int search_dir, int ignore_case, int skip_chars, int global_search)
{
	be_line_t *line;
	int byte_idx;
	int match_len;

	line = *ptr_line;
	byte_idx = *ptr_byte_idx;
	match_len = 0;

	memorize_cur_file_pos_into_str(NULL);
	memorize_cursor_pos_before_move();

	if (search_dir <= BACKWARD_SEARCH) {
		// search backward -----------------------------------------------------
		int end_of_buf = 0;
		for ( ; ; ) {
			for (int skipped_chars = 0 ; skipped_chars < skip_chars; skipped_chars++) {
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
					break;
				} else {
					end_of_buf = 1;
					break;
				}
			}
			if (end_of_buf) {
				break;
			}
			// look for needle backward in haystack
			if ((match_len = search_str_in_line(&search__, &matches__,
			 needle, search_dir, ignore_case, line->data, byte_idx)) > 0) {
				// found
				break;
			}
			byte_idx = 0;
			skip_chars = 1;
		}
	} else {
		// search forward ------------------------------------------------------
		int end_of_buf = 0;
		for ( ; ; ) {
			for (int skipped_chars = 0 ; skipped_chars < skip_chars; skipped_chars++) {
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
					break;
				} else {
					end_of_buf = 1;
					break;
				}
			}
			if (end_of_buf) {
				break;
			}
			// look for needle forward in haystack
			if ((match_len = search_str_in_line(&search__, &matches__,
			 needle, search_dir, ignore_case, line->data, byte_idx)) > 0) {
				// found
				break;
			}
			byte_idx = line_strlen(line);
			skip_chars = 1;
		}
	}

	if (match_len > 0) {
		// found and update current line pointer
		*ptr_line = line;
		*ptr_byte_idx = matches_start_idx(&matches__);
		return match_len;
	}
	// not found then return to beginning position
	recall_file_pos_from_str(NULL);
	return match_len;
}

// attempt_count = 0: first uni-directional search
// attempt_count = 1: second uni-directional search (bi-directional search done)
void disp_status_bar_not_found_msg(const char *str, int attempt_count)
{
	disp_status_bar_type((attempt_count == FIRST_ATTEMPT_0) ? S_B_WARN : S_B_ERR,
	 _("Keyword [%s] NOT found in %s search"),
	 shrink_str_to_scr_static(str), (SEARCH_DIR() < 0) ? _("Backward") : _("Forward"));
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}

// End of search.c
