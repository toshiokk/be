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

#define SEARCH0		0
#define REPLACE1	1

PRIVATE int input_search_str(int search0_replace1, char *input_buf);
PRIVATE int input_replace_str(char *input_buf);

char last_searched_needle[MAX_PATH_LEN+1] = "";	// Last search string

int do_search_backward(void)
{
	char needle[MAX_PATH_LEN+1];

	SET_APPMD(ed_REVERSE_SEARCH);
	if (input_search_str(SEARCH0, needle) <= 0) {
		return 0;
	}
	search_string_once(needle);
	return 0;
}
int do_search_forward(void)
{
	char needle[MAX_PATH_LEN+1];

	CLR_APPMD(ed_REVERSE_SEARCH);
	if (input_search_str(SEARCH0, needle) <= 0) {
		return 0;
	}
	search_string_once(needle);
	return 0;
}
int do_search_next_backward(void)
{
	SET_APPMD(ed_REVERSE_SEARCH);
	search_string_once(last_searched_needle);
	return 0;
}
int do_search_next_forward(void)
{
	CLR_APPMD(ed_REVERSE_SEARCH);
	search_string_once(last_searched_needle);
	return 0;
}

//------------------------------------------------------------------------------

// TODO: search wrap around
// on forward search, end of the last file ==> top of the first file
// on reverse search, top of the first file ==> end of the last file
// Replace a string
int do_replace(void)
{
	char replace_from[MAX_PATH_LEN+1];
	char replace_to[MAX_PATH_LEN+1];
	int ret;
	char prev_file_pos[MAX_PATH_LEN+1];
	int num_replaced;

	if (is_view_mode_then_warn_it())
		return 0;

	CLR_APPMD(ed_REVERSE_SEARCH);

	ret = input_search_str(REPLACE1, replace_from);
	if (ret <= 0) {
		return 0;
	}

	ret = input_replace_str(replace_to);
	if (ret <= 0) {
		return 0;
	}
	conv_esc_str(replace_to);

	memorize_cur_file_pos_null(prev_file_pos);

	ret = replace_string_loop(replace_from, replace_to, &num_replaced);

	if (ret == ANSWER_CANCEL) {
		// not return to original file pos
	} else {
		// return to original file pos
		recall_cur_file_pos_null(prev_file_pos);
	}
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);

	if (num_replaced >= 0)
		disp_status_bar_done(P_(_("Replaced %d occurrence"),
								_("Replaced %d occurrences"),
								_("Replaced %d occurrencess"),
								_("Replaced %d occurrencesss"),
		 num_replaced), num_replaced);
	else
		not_found_msg(replace_from);

	return 0;
}

//------------------------------------------------------------------------------

// search0_replace1 = 0: search, 1: replace
int input_search_str(int search0_replace1, char *input_buf)
{
	char default_needle[MAX_PATH_LEN+1];
	int ret = 0;

	if (strlen(last_searched_needle)) {
		// We use main_win_get_columns() / 3 here because we need to see more on the line
		snprintf(default_needle, MAX_PATH_LEN+1, "(%.*s%s)",
		 main_win_get_columns() / 3, last_searched_needle,
		 strlen(last_searched_needle) > main_win_get_columns() / 3 ? "..." : "");
	} else {
		strcpy__(default_needle, "");
	}

	ret = input_string("", input_buf, HISTORY_TYPE_IDX_SEARCH, "%s%s%s%s%s:",
	 search0_replace1 == 0 ? _("Search") : _("Replace"),
	 GET_APPMD(ed_IGNORE_CASE) ? _("[Ignore-case]") : "",
#ifdef ENABLE_REGEX
	 GET_APPMD(ed_USE_REGEXP) ? _("[Regexp]") : "",
#else // ENABLE_REGEX
	 "",
#endif // ENABLE_REGEX
	 GET_APPMD(ed_REVERSE_SEARCH) ? _("[Backward]") : _("[Forward]"), default_needle);

	if (ret <= 0) {
		// cancelled
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
		return ret;						// cancelled
	}
	if (strlen(input_buf) == 0) {
		// nothing input, get last searched string
		strlcpy__(input_buf, last_searched_needle, MAX_PATH_LEN);
	}
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP))
		if (regexp_compile(&search__.regexp, input_buf,
		 GET_APPMD(ed_IGNORE_CASE) ? REG_ICASE : 0)) {
			disp_status_bar_err(_("Invalid regexp: [%s]:%s"),
			 search__.regexp.needle_compiled, search__.regexp.regex_err_msg);
			set_edit_win_update_needed(UPDATE_SCRN_ALL);
			return -1;			// regexp error
		}
#endif // ENABLE_REGEX
	return 1;							// input normally
}

int input_replace_str(char *input_buf)
{
	int ret;

	ret = input_string("", input_buf, HISTORY_TYPE_IDX_SEARCH, "%s:", _("Replace with"));

	if (ret <= 0) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
	return ret;
}

//==============================================================================

search_t search__;
matches_t matches__;

//------------------------------------------------------------------------------

PRIVATE int found_in_prev_search = 1;

// Search for a string
int search_string_once(const char *needle)
{
	int match_len;

////flf_d_printf("last_searched_needle[%s], needle[%s]\n", last_searched_needle, needle);
////flf_d_printf("%04x %04x %d\n", key_executed, prev_key_executed, found_in_prev_search);
	if (key_executed == prev_key_executed && found_in_prev_search == 0
	 && strcmp(last_searched_needle, needle) == 0) {
		not_found_msg(needle);
		return 0;
	}
	if (strlen(needle)) {
		strlcpy__(last_searched_needle, needle, MAX_PATH_LEN);
	}

	memorize_cur_file_pos_null(NULL);

	match_len = search_str_in_buffer(needle,
	 GET_APPMD(ed_REVERSE_SEARCH) ? BACKWARD_SEARCH : FORWARD_SEARCH,
	 GET_APPMD(ed_IGNORE_CASE),
	 SKIP,
	 INTER_FILE_SEARCH);
_FLF_

	if (match_len == 0) {
_FLF_
		recall_cur_file_pos_null(NULL);
////		post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_KEEP, UPDATE_SCRN_ALL);
	} else {
_FLF_
		if (GET_APPMD(ed_CURS_POSITIONING) == 0)
			post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_KEEP, UPDATE_SCRN_ALL);
		else
			post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	}
_FLF_

	found_in_prev_search = match_len;
	return match_len;
}

//------------------------------------------------------------------------------
// Step through each replace word and prompt user before replacing
// it.  Return -1 if the string to replace isn't found at all.
// Otherwise, return the number of replacements done.
// [test string]
//
// StringStringStringString
//
int replace_string_loop(const char *needle, const char *replace_to, int *num_replaced_)
{
	int num_replaced = 0;
#ifdef ENABLE_UNDO
	int num_undone = 0;
#endif // ENABLE_UNDO
	int skip_here;
	int match_len;
	int ret = 0;
	long length_change;
	int prev_ed_REVERSE_SEARCH;
	key_code_t key;

	if (strlen(needle)) {
		strlcpy__(last_searched_needle, needle, MAX_PATH_LEN);
	}

	prev_ed_REVERSE_SEARCH = GET_APPMD(ed_REVERSE_SEARCH);
	for (skip_here = NO_SKIP; ; ) {
		if (prev_ed_REVERSE_SEARCH != GET_APPMD(ed_REVERSE_SEARCH)) {
			prev_ed_REVERSE_SEARCH = GET_APPMD(ed_REVERSE_SEARCH);
			// search direction changed, not skip here on the next search
			skip_here = NO_SKIP;
		}
		memorize_cur_file_pos_null(NULL);
		memorize_cur_cursor_pos_before_cursor_move();

		match_len = search_str_in_buffer(needle,
		 GET_APPMD(ed_REVERSE_SEARCH) ? BACKWARD_SEARCH : FORWARD_SEARCH,
		 GET_APPMD(ed_IGNORE_CASE),
		 skip_here,
		 INTER_FILE_SEARCH);

		if (match_len == 0) {
			// not found
			recall_cur_file_pos_null(NULL);
		}
		post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_KEEP, UPDATE_SCRN_ALL);
		update_screen_editor(1, 1, 1);
		if (match_len) {
			// found
			if (ret < ANSWER_ALL) {
#ifdef ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO | ASK_ALL | ASK_BACKWARD | ASK_END
				 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0),
#else // ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO | ASK_ALL | ASK_BACKWARD | ASK_END,
#endif // ENABLE_UNDO
				 _("Replace this instance ?"));
			} else {
				if ((key = tio_input_key()) >= 0) {
					if (key == K_M_ESC) {
						ret = ANSWER_END;
					} else if (key == K_C_C) {
						ret = ANSWER_CANCEL;
					}
				}
			}
		} else {
			// not found message has been displayed
#ifdef ENABLE_UNDO
			ret = ask_yes_no(ASK_NO | ASK_BACKWARD | ASK_END
			 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0), "");
#else // ENABLE_UNDO
			ret = ask_yes_no(ASK_NO | ASK_BACKWARD | ASK_END, "");
#endif // ENABLE_UNDO
		}
		if (ret == ANSWER_NO) {
			// forward search
			CLR_APPMD(ed_REVERSE_SEARCH);
			skip_here = SKIP;
			continue;
		} else if (ret == ANSWER_BACKWARD) {
			// backward search
			SET_APPMD(ed_REVERSE_SEARCH);
			skip_here = SKIP;
			continue;
		} else if (ret == ANSWER_YES || ret == ANSWER_ALL) {
			if (match_len) {
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
				memorize_undo_state_before_change();
#endif // ENABLE_DEBUG
				undo_set_region_save_before_change(CBV_CL, CBV_CL, 1);
#endif // ENABLE_UNDO
				length_change = replace_str_in_buffer(&search__, &matches__, replace_to);
#ifdef ENABLE_UNDO
				undo_save_after_change();
#ifdef ENABLE_DEBUG
				check_undo_state_after_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
				// Set the cursor at the last character of the replacement
				// text, so searching will resume after the replacement text. */
				if (GET_APPMD(ed_REVERSE_SEARCH) == 0) {
					// forward search
					CBV_CLBI += matches_match_len(&matches__) + length_change;
					skip_here = NO_SKIP;	// CBV_CLBI already forwarded to skip word
				} else {
					// backward search
					skip_here = SKIP;		// skip
				}
				get_c_e_b()->buf_size += length_change;
				set_cur_buf_modified();
				num_replaced++;
			}
#ifdef ENABLE_UNDO
		} else if (ret == ANSWER_UNDO) {
			// undo replace
			do_undo();
			num_replaced--;
			num_undone++;
			skip_here = NO_SKIP;
			continue;
		} else if (ret == ANSWER_REDO) {
			// redo replace
			do_redo();
			num_undone--;
			num_replaced++;
			skip_here = NO_SKIP;
			continue;
#endif // ENABLE_UNDO
		} else {			// Cancel
			break;
		}
	}
	*num_replaced_ = num_replaced;
	if (num_replaced && strlen(replace_to)) {
		strlcpy__(last_searched_needle, replace_to, MAX_PATH_LEN);
	}
	return ret;
}

int replace_str_in_buffer(search_t *search, matches_t *matches, const char *replace_to)
{
	char before[MAX_EDIT_LINE_LEN+1];
	char after[MAX_EDIT_LINE_LEN+1];

	strlcpy__(before, CBV_CL->data, MAX_EDIT_LINE_LEN);
	strlcpy__(after, CBV_CL->data, MAX_EDIT_LINE_LEN);
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
	line_set_string(CBV_CL, after);		// replace whole of the line
	return strlen(after) - strlen(before);
}

//------------------------------------------------------------------------------

#ifdef ENABLE_REGEX
PRIVATE int do_find_bracket_(int reverse);
// [test string]
// 12345678901234567890123456789012345678901234567890
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
// }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// (((((((((((((((((((((((((((((((((((((((((((((((((((
// ((((((((((((((((((((((((((((((((((((((((((((((((((
// ))))))))))))))))))))))))))))))))))))))))))))))))))
// )))))))))))))))))))))))))))))))))))))))))))))))))))
// {{}}}
// {{{}}
int do_find_bracket(void)
{
	return do_find_bracket_(1);
}
// [test string]
// 12345678901234567890123456789012345678901234567890
// }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
// )))))))))))))))))))))))))))))))))))))))))))))))))))
// ))))))))))))))))))))))))))))))))))))))))))))))))))
// ((((((((((((((((((((((((((((((((((((((((((((((((((
// (((((((((((((((((((((((((((((((((((((((((((((((((((
// }}}{{
// }}{{{
int do_find_bracket_reverse(void)
{
	return do_find_bracket_(-1);
}
PRIVATE int do_find_bracket_(int reverse)
{
	char char_under_cursor;
	char char_wanted;
	const char *brackets = "([{<>}])";
	const char *ptr;
	char regexp_str[] = "[  ]";
	int offset;
	int search_dir;			// search direction (FORWARD_SEARCH/BACKWARD_SEARCH)
	be_line_t *cur_line_save;
	int cur_line_byte_idx_save;
#define MAX_NESTING	100
	int depth;
	int match_len;

	char_under_cursor = CBV_CL->data[CBV_CLBI];
	if ((ptr = strchr__(brackets, char_under_cursor)) == NULL) {
		disp_status_bar_done(_("Not a bracket"));
		return 1;
	}
	offset = ptr - brackets;
	char_wanted = brackets[strlen(brackets) - (offset + 1)];

	set_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();

	cur_line_save = CBV_CL;
	cur_line_byte_idx_save = CBV_CLBI;
	SET_APPMD(ed_USE_REGEXP);

	// apparent near redundancy with regexp_str[] here is needed,
	// [][] works, [[]] doesn't
	if (offset < (strlen(brackets) / 2)) {	// on a left bracket
		regexp_str[1] = char_wanted;
		regexp_str[2] = char_under_cursor;
		if (reverse > 0) {
			search_dir = FORWARD_SEARCH;		// forward
		} else {
			search_dir = BACKWARD_SEARCH;		// backward
		}
	} else {								// on a right bracket
		regexp_str[1] = char_under_cursor;
		regexp_str[2] = char_wanted;
		if (reverse > 0) {
			search_dir = BACKWARD_SEARCH;		// backward
		} else {
			search_dir = FORWARD_SEARCH;		// forward
		}
	}

	for (depth = 1; depth > 0; ) {
		match_len = search_str_in_buffer(regexp_str,
		 search_dir, DISTINGUISH_CASE, SKIP, INNER_FILE_SEARCH);

		if (match_len) {
			// found bracket
			if (CBV_CL->data[CBV_CLBI] == char_under_cursor) {
				depth++;
				if (depth > MAX_NESTING) {
					break;
				}
			} else {
				// found complementary bracket
				depth--;
			}
		} else {
			break;
		}
	}
	if (depth == 0) {
		post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
		disp_status_bar_done(_("Peer bracket found"));
	} else if (depth < MAX_NESTING) {
		// didn't find peer bracket
		CBV_CL = cur_line_save;
		CBV_CLBI = cur_line_byte_idx_save;
		disp_status_bar_err(_("No peer bracket found"));
	} else {
		// didn't find peer bracket
		CBV_CL = cur_line_save;
		CBV_CLBI = cur_line_byte_idx_save;
		disp_status_bar_err(_("Bracket nesting too deep"));
	}
	regexp_free_regex_compiled(&search__.regexp);
	return 0;
}
#endif // ENABLE_REGEX

//------------------------------------------------------------------------------

int search_str_in_buffer(const char *needle,
 int search_dir, int ignore_case, int skip_here, int global_search)
{
	be_line_t *line;
	int byte_idx;
	int match_len;

	disp_status_bar_ing(_("Searching word: [%s]..."), needle);
_FLF_

	line = CBV_CL;
	byte_idx = CBV_CLBI;
	match_len = 0;
	if (search_dir <= BACKWARD_SEARCH) {
		// search backward -----------------------------------------------------
		while (1) {
			if (skip_here) {
				// move cur-pos left at least one char
				// if cur-pos is left most, move cur-pos up at least one line
				if (byte_idx > 0) {
					byte_idx--;
				} else if (IS_NODE_TOP(line) == 0) {
					line = line->prev;
					byte_idx = line_data_len(line);
				} else if (global_search && switch_c_e_b_to_prev(0, 0)) {
					line = CUR_EDIT_BUF_BOT_LINE;
					byte_idx = line_data_len(line);
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
_FLF_
	} else {
		// search forward ------------------------------------------------------
		while (1) {
			if (skip_here) {
				// move cur-pos right at least one char
				// if cur-pos is right most, move cur-pos down at least one line
				if (byte_idx < line_data_len(line)) {
					byte_idx++;
				} else if (IS_NODE_BOT(line) == 0) {
					line = NEXT_NODE(line);
					byte_idx = 0;
				} else if (global_search && switch_c_e_b_to_next(0, 0)) {
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
			byte_idx = line_data_len(line);
			skip_here = 1;
		}
_FLF_
	}
_FLF_
	if (match_len) {
		// found and update current line pointer
		CBV_CL = line;
		CBV_CLBI = matches_start_idx(&matches__);
_FLF_
		return match_len;
	}
	tio_beep();
	not_found_msg(needle);
_FLF_
	return 0;
}

void not_found_msg(const char *str)
{
	disp_status_bar_err(_("\"%s\" not found"), shrink_str_scr_static(str));
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}

// End of search.c
