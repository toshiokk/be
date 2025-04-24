/**************************************************************************
 *   winin.c                                                              *
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

PRIVATE int input_string_pos_(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, va_list ap);
PRIVATE int input_string_pos__(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg);
PRIVATE void disp_input_box(const char *buf, const char *input_buf, int x);
PRIVATE void blank_input_box(void);

// input string
int input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...)
{
	_D(disp_status_bar_cwd())
	va_list ap;
	va_start(ap, msg);
	int ret = input_string_pos_(default__, input_buf, cursor_byte_idx, hist_type_idx, msg, ap);
	va_end(ap);
	_D(disp_status_bar_cwd())
	return ret;
}

// return -1: recursively called
// return 0 : cancelled
// return 1 : input normally
PRIVATE int input_string_pos_(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, va_list ap)
{
	static int recursively_called = 0;
	char dir_save[MAX_PATH_LEN + 1];
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];

	if (recursively_called >= 3) {
		return -1;				// -1: recursive called
	}

	vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
	int byte_idx = byte_idx_from_col_idx(msg_buf, central_win_get_columns(), CHAR_LEFT, NULL);
	msg_buf[byte_idx] = '\0';		// limit message length

	update_screen_app(1, 1);

	get_full_path_of_cur_dir(dir_save);
	tio_set_cursor_on(1);
	recursively_called++;
	//--------------------------------------------------------------------------
	int ret = input_string_pos__(default__, input_buf, cursor_byte_idx, hist_type_idx, msg_buf);
	//--------------------------------------------------------------------------
	recursively_called--;
flf_d_printf("ret: %d\n", ret);
	tio_set_cursor_on(0);
	change_cur_dir(dir_save);

	update_screen_app(1, 1);

	if (ret <= EF_QUIT) {
		disp_status_bar_warn(_("Input string cancelled"));
	}
#ifdef ENABLE_HISTORY
	if (IS_EF_INPUT_XX(ret)) {
		// input normally
		update_history(hist_type_idx, input_buf);
	}
#endif
	return ret;
}

// Input string. This should only be called from input_string_xxx().
PRIVATE int input_string_pos__(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *message)
{
	int key_input;
#if defined(ENABLE_HISTORY) || defined(ENABLE_FILER)
	char buffer[MAX_PATH_LEN+1];
#endif
	char cut_buf[MAX_PATH_LEN+1] = "";
	int ret = EF_NONE;

	strlcpy__(input_buf, default__, MAX_PATH_LEN);
	cursor_byte_idx = MIN_MAX_(0, cursor_byte_idx, strlen_path(default__));

	blank_key_list_lines();
	// Main input loop
	for ( ; ; ) {
		ret = EF_NONE;

		disp_input_box(message, input_buf, cursor_byte_idx);
		//---------------------------
		key_input = input_key_wait_return();
		//---------------------------
		mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
		 '_', (UINT16)key_input, short_key_name_from_key_code(key_input, NULL));

		if (is_key_char(key_input)) {
			// character key
			if (strlen_path(input_buf) < MAX_PATH_LEN) {
				insert_str_chr(input_buf, MAX_PATH_LEN, cursor_byte_idx, key_input);
				cursor_byte_idx++;
			}
			continue;
		}
		// function key
		const char *func_id = get_func_id_from_key(key_input);
flf_d_printf("func_id: [%s]\n", func_id);
		if ((key_input == K_ESC) || (key_input == K_M_ESC)
		 || cmp_func_id(func_id, "doe_close_file_ask")
		 || cmp_func_id(func_id, "doe_close_all_ask")
		 || cmp_func_id(func_id, "doe_close_all_modified")) {
			strcpy__(input_buf, "");
			ret = EF_CANCELLED;				// cancelled, return
		} else
		if (key_input == K_ENTER) {
			ret = EF_INPUT_W_ENTER;			// confirm a string input
		} else
		if (key_input == K_M_ENTER) {
			ret = EF_INPUT_W_ALT_ENTER;		// confirm a string input
		} else
		if (cmp_func_id(func_id, "dof_copy_file")
		 || cmp_func_id(func_id, "dof_drop_files_to_copy")) {
			ret = EF_INPUT_W_ALT_C;			// Alt-c
		} else
		if (cmp_func_id(func_id, "dof_move_file")
		 || cmp_func_id(func_id, "dof_drop_files_to_move")) {
			ret = EF_INPUT_W_ALT_M;			// Alt-m
		} else
		if (cmp_func_id(func_id, "doe_left")) {
			// cursor left
			if (cursor_byte_idx > 0) {
				cursor_byte_idx -= utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
			}
		} else
		if (cmp_func_id(func_id, "doe_right")) {
			// cursor right
			if (cursor_byte_idx < strlen_path(input_buf)) {
				cursor_byte_idx += utf8c_bytes(&input_buf[cursor_byte_idx]);
			}
		} else
		if (cmp_func_id(func_id, "doe_prev_word")) {
			// goto a prev word
			if (cursor_byte_idx > 0) {
				cursor_byte_idx -= utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
			}
			while (cursor_byte_idx > 0) {
				cursor_byte_idx -= utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
				if (input_buf[cursor_byte_idx] == ' ') {
					cursor_byte_idx++;	// skip a space
					break;
				}
			}
		} else
		if (cmp_func_id(func_id, "doe_next_word")) {
			// goto a next word
			while (cursor_byte_idx < strlen_path(input_buf)) {
				cursor_byte_idx += utf8c_bytes(&input_buf[cursor_byte_idx]);
				if (input_buf[cursor_byte_idx] == ' ') {
					cursor_byte_idx++;	// skip a space
					break;
				}
			}
		} else
		if (cmp_func_id(func_id, "doe_start_of_line")) {
			// goto line head
			cursor_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "doe_end_of_line")) {
			// goto line tail
			cursor_byte_idx = strlen_path(input_buf);
		} else
		if (cmp_func_id(func_id, "doe_backspace")
		 || (key_input == K_BS)) {
			// backspace
			if (cursor_byte_idx > 0) {
				int bytes = utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
				cursor_byte_idx -= bytes;
				delete_str(input_buf, cursor_byte_idx, bytes);
			}
		} else
		if (cmp_func_id(func_id, "doe_delete_char")
		 || (key_input == K_DEL)) {
			// delete
			if (cursor_byte_idx < strlen_path(input_buf)) {
				int bytes = utf8c_bytes(&input_buf[cursor_byte_idx]);
				delete_str(input_buf, cursor_byte_idx, bytes);
			}
		} else
		if (cmp_func_id(func_id, "doe_cut_to_head")) {
			// cut to line head
			strcut__(cut_buf, MAX_PATH_LEN, input_buf, 0, cursor_byte_idx);
			delete_str(input_buf, 0, cursor_byte_idx);
			cursor_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "doe_cut_text")) {
			// cut line
			strlcpy__(cut_buf, input_buf, MAX_PATH_LEN);
			strcpy__(input_buf, "");
			cursor_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "doe_cut_to_tail")) {
			// cut to line tail
			strcut__(cut_buf, MAX_PATH_LEN,
			 input_buf, cursor_byte_idx, strlen_path(input_buf));
			delete_str(input_buf, cursor_byte_idx,
			 strlen_path(input_buf) - cursor_byte_idx);
		} else
		if (cmp_func_id(func_id, "doe_copy_text")) {
			// copy to the cut buffer
			strlcpy__(cut_buf, input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "doe_paste_text_with_pop")) {
			// paste from the cut buffer
			insert_str(input_buf, MAX_PATH_LEN, cursor_byte_idx, cut_buf, -1);
		} else
		if (cmp_func_id(func_id, "doe_search_backward_first")
		 || cmp_func_id(func_id, "doe_search_forward_first")
		 || cmp_func_id(func_id, "doe_replace")) {
			// get string from edit buffer's current cursor position
			if (count_edit_bufs()) {
				char *line = EPCBVC_CL->data;
				cursor_byte_idx = strlen_path(input_buf);
				int start_byte_idx = byte_idx_from_byte_idx(line, EPCBVC_CLBI + cursor_byte_idx);
				int byte_idx = start_byte_idx;
				if ((cmp_func_id(func_id, "doe_search_backward_first") && (SEARCH_DIR() > 0))
				 || (cmp_func_id(func_id, "doe_search_forward_first") && (SEARCH_DIR() < 0))) {
					// copy whole line
					byte_idx = byte_idx_from_byte_idx(line, MAX_PATH_LEN);
				} else {
					if (cursor_byte_idx == 0) {
						// copy one token
						for ( ; (byte_idx - start_byte_idx) < (MAX_PATH_LEN - MAX_UTF8C_BYTES); ) {
							if (is_char_id(line[byte_idx]) == 0) {
								break;
							}
							byte_idx++;
						}
					}
					if ((byte_idx - start_byte_idx) == 0) {
						// copy one character
						byte_idx += utf8c_bytes(&line[byte_idx]);
					}
				}
				strlncat__(input_buf, MAX_PATH_LEN, &line[start_byte_idx],
				 byte_idx - start_byte_idx);
				cursor_byte_idx = strlen_path(input_buf);
			}
#ifdef ENABLE_HISTORY
		} else
		if (key_input == K_TAB) {
			// tab history completion
			strlcpy__(input_buf, get_history_completion(hist_type_idx, input_buf),
			 MAX_PATH_LEN);
			cursor_byte_idx = strlen_path(input_buf);
		} else
		if (cmp_func_id(func_id, "doe_up")
		 || cmp_func_id(func_id, "doe_page_up")
		 || cmp_func_id(func_id, "doe_first_line")) {
			//----------------------------------------------------
			ret = select_from_history_list(hist_type_idx, buffer);
			//----------------------------------------------------
			ret = editor_do_next;
			if ((ret == EF_INPUT_TO_REPLACE) || (ret == EF_INPUT_TO_APPEND)) {
				if ((ret == EF_INPUT_TO_REPLACE) || cmp_func_id(func_id, "doe_page_up")) {
					// clear input buffer
					strcpy__(input_buf, "");
					cursor_byte_idx = 0;
				} else {
					// insert string into buffer without clearing buffer
				}
				cursor_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 cursor_byte_idx, buffer);
			}
#endif // ENABLE_HISTORY
#ifdef ENABLE_FILER
		} else
		if (cmp_func_id(func_id, "doe_down")
		 || cmp_func_id(func_id, "doe_page_down")
		 || cmp_func_id(func_id, "doe_last_line")) {
			//---------------------------------------------------
			ret = do_call_filer(1, APP_MODE_CHOOSER, "", "", buffer);
			//---------------------------------------------------
			ret = filer_do_next;
			if ((ret == EF_INPUT_TO_REPLACE) || (ret == EF_INPUT_TO_APPEND)) {
				if ((ret == EF_INPUT_TO_REPLACE) || cmp_func_id(func_id, "doe_page_down")) {
					// clear input buffer
					strcpy__(input_buf, "");
					cursor_byte_idx = 0;
				} else {
					// insert string into buffer without clearing buffer
				}
				cursor_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 cursor_byte_idx, buffer);
			}
#endif // ENABLE_FILER
		}
		// EF_QUIT: stay in this loop
		if ((ret == EF_CANCELLED) || IS_EF_INPUT_XX(ret)
		 || (ret == EF_LOADED) || (ret == EF_EXECUTED)) {
			break;
		}
	}
	return ret;
} // input_string_pos__

/* display input box
#Prompt message#######################################################
#input-string                                                        #
###UP:history##DOWN:browser###########################################
+Prompt message------------------------------------------------------+
|input-string                                                        |
+--------------------------------------------------------------------+
*/
PRIVATE void disp_input_box(const char *msg, const char *input_buf, int cursor_byte_idx)
{
	int cursor_col_idx = col_idx_from_byte_idx(input_buf, 0, cursor_byte_idx);
	int input_area_width;
	int start_byte_idx;

	determine_input_line_y();
	blank_input_box();
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(get_input_line_y(), 1, msg, -1);
	central_win_output_string(get_input_line_y()+2, 1,
	 _("UP/PGUP:history, DOWN/PGDN:filer"), -1);
	set_item_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);

	input_area_width = central_win_get_columns()-2;
	if (cursor_col_idx < input_area_width) {
		int bytes = byte_idx_from_col_idx(input_buf, input_area_width, CHAR_LEFT, NULL);
		central_win_output_string(get_input_line_y()+1, 1, input_buf, bytes);
		central_win_set_cursor_pos(get_input_line_y()+1, 1 + cursor_col_idx);
	} else {
#define TRUNCATION_MARK	"."
		//"abcdefghijklmnopqrstuvwxyz"
		// ==> |.ghijklmnopqrstuvwxyz |
		//                           ^cursor
		//
		//    "abcdefghijklmnopqrstuvwxyz"
		// ==> |.cdefghijklmnopqrstuvw|
		//                           ^cursor
		//
		//     "abcdefghijklmnopqrstuvwxyz"
		// ==> |abcdefghijklmnopqrstuv|
		//                           ^cursor
		start_byte_idx = byte_idx_from_col_idx(input_buf,
		 cursor_col_idx - (input_area_width-2), CHAR_RIGHT, NULL);
		int bytes = byte_idx_from_col_idx(&input_buf[start_byte_idx],
		 input_area_width - utf8s_columns(TRUNCATION_MARK, MAX_SCRN_COLS), CHAR_LEFT, NULL);
		central_win_output_string(get_input_line_y()+1, 1, TRUNCATION_MARK, -1);
		central_win_output_string(-1, -1, &input_buf[start_byte_idx], bytes);
		central_win_set_cursor_pos(get_input_line_y()+1,
		 1 + utf8s_columns(TRUNCATION_MARK, MAX_SCRN_COLS)
		 + col_idx_from_byte_idx(&input_buf[start_byte_idx],
		 0, cursor_byte_idx - start_byte_idx));
	}
	tio_refresh();
}

PRIVATE void blank_input_box(void)
{
	// display frame
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	//+--------------------------------------------------------------------+
	//|                                                                    |
	//+--------------------------------------------------------------------+
	central_win_clear_lines(get_input_line_y(), -3);
	// clear input area
	set_item_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);
	//+--------------------------------------------------------------------+
	//|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
	//+--------------------------------------------------------------------+
	central_win_output_string(get_input_line_y()+1, 1, tio_blank_line(),
	 central_win_get_columns()-2);
}

//------------------------------------------------------------------------------

PRIVATE void disp_ask_yes_no_msg(int flags);
PRIVATE void list_one_key(char key, const char *desc);

PRIVATE const char *chars_yes = "Yy";				// Yes(replace)
PRIVATE const char *chars_no = "Nn";				// No
PRIVATE const char *chars_all = "Aa";				// All
PRIVATE const char *chars_backward = "Bb";			// Backward search
PRIVATE const char *chars_forward = "Ff ";			// Forward search
PRIVATE const char *chars_cancel = "CcSs" S_ESC;	// Cancel/Stop/ESC
PRIVATE const char *chars_end = "EeQqRr" S_C_Q;		// End/Quit/Return/Ctrl-Q
PRIVATE const char *chars_undo = "Uu";				// Undo
PRIVATE const char *chars_redo = "Oo";				// redO

// Ask a simple yes/no question on the status_bar.
int ask_yes_no(int flags, const char *msg, ...)
{
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1] = "";

	int key_lines_save = GET_APPMD(app_KEY_LINES);		// save KEY_LINES
	SET_APPMD_VAL(app_KEY_LINES, LIM_MIN(1, key_lines_save));	// set lines more than 1

	// display prompt message
	if (*msg) {
		va_list ap;
		va_start(ap, msg);
		vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
		va_end(ap);
		strlcat__(msg_buf, MAX_SCRN_LINE_BUF_LEN, " ");
		int byte_idx = byte_idx_from_col_idx(msg_buf, MAX_SCRN_LINE_BUF_LEN, CHAR_LEFT, NULL);
		msg_buf[byte_idx] = '\0';
	}

	int answer;
	for (answer = ANSWER_NONE; answer == ANSWER_NONE; ) {
		// display key list
		disp_ask_yes_no_msg(flags);

		set_item_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
		set_item_color_by_idx(ITEM_COLOR_IDX_WARNING1, 1);
		blank_status_bar();
		central_win_output_string(central_win_get_status_line_y(), 0, msg_buf, -1);
		tio_refresh();

		// "Save modified buffer ?"[] <== cursor
		tio_set_cursor_on(1);
		//---------------------------
		key_code_t key_input = input_key_wait_return();
		//---------------------------
		const char *func_id = get_func_id_from_key(key_input);
		// Look for the key_input in yes/no/all
		if (strchr__(chars_yes, key_input) != NULL)
			answer = ANSWER_YES;
		else if (strchr__(chars_no, key_input) != NULL)
			answer = ANSWER_NO;
		else if ((flags & ASK_ALL) && (strchr__(chars_all, key_input) != NULL))
			answer = ANSWER_ALL;
		else if ((flags & ASK_BACKWARD)
		 && ((strchr__(chars_backward, key_input) != NULL)
		  || cmp_func_id(func_id, "doe_search_backward_next")))
			answer = ANSWER_BACKWARD;
		else if ((flags & ASK_FORWARD)
		 && ((strchr__(chars_forward, key_input) != NULL)
		  || cmp_func_id(func_id, "doe_search_forward_next")))
			answer = ANSWER_FORWARD;
		else if (strchr__(chars_cancel, key_input) != NULL)
			answer = ANSWER_CANCEL;
		else if ((flags & ASK_END || flags & ASK_NO) && (strchr__(chars_end, key_input) != NULL))
			answer = ANSWER_END;
		else if ((flags & ASK_UNDO) && (strchr__(chars_undo, key_input) != NULL))
			answer = ANSWER_UNDO;
		else if ((flags & ASK_REDO) && (strchr__(chars_redo, key_input) != NULL))
			answer = ANSWER_REDO;
	}

	// Then blank the status_bar.
	set_item_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();

	SET_APPMD_VAL(app_KEY_LINES, key_lines_save);	// restore KEY_LINES
	update_screen_app(1, 1);

	return answer;	// x > 0: yes, x = 0: no, x < 0: cancel
}
PRIVATE void disp_ask_yes_no_msg(int flags)
{
	blank_key_list_lines();
	if (get_key_list_lines()) {
		central_win_set_cursor_pos(central_win_get_yes_no_line_y(), 0);
		if (flags & ASK_YES) {
			list_one_key(chars_yes[0], _("Yes"));
		}
		if (flags & ASK_NO) {
			list_one_key(chars_no[0], _("No"));
		}
		if (flags & ASK_ALL) {
			list_one_key(chars_all[0], _("All"));
		}
		if (flags & ASK_BACKWARD) {
			list_one_key(chars_backward[0], _("Backward"));
		}
		if (flags & ASK_FORWARD) {
			list_one_key(chars_forward[0], _("Forward"));
		}
		list_one_key(chars_cancel[0], _("Cancel"));
		if (flags & ASK_END || flags & ASK_NO) {
			list_one_key(chars_end[0], _("End"));
		}
		if (flags & ASK_UNDO) {
			list_one_key(chars_undo[0], _("Undo"));
		}
		if (flags & ASK_REDO) {
			list_one_key(chars_redo[0], _("Redo"));
		}
	}
}
PRIVATE void list_one_key(char key, const char *desc)
{
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s", short_key_name_from_key_code(key, key_name));
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
	central_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s ", desc);
	central_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
}

//------------------------------------------------------------------------------

PRIVATE void display_reverse_text(int yy, const char *text);
PRIVATE void display_func_id_key(int yy, const char *text);

void disp_fkey_list()
{
	key_code_t func_keys[] = { K_ESC,
	 K_SP, K_F01, K_F02, K_F03, K_F04,
	 K_SP, K_F05, K_F06, K_F07, K_F08,
	 K_SP, K_F09, K_F10, K_F11, K_F12, K_NONE };
	if (get_key_list_lines() > 0) {
		char buf[MAX_SCRN_LINE_BUF_LEN+1] = "";
		for (int key_idx = 0; func_keys[key_idx] != K_NONE; key_idx++) {
			if (func_keys[key_idx] == K_SP) {
				strcat_printf(buf, MAX_SCRN_LINE_BUF_LEN+1, " ");	// separator
			} else {
				func_key_list_t *fkey_list = get_fkey_entry_table_from_key(
				 NULL, func_keys[key_idx], -1, 1);
				if (fkey_list) {
					strcat_printf(buf, MAX_SCRN_LINE_BUF_LEN+1, "{%s} ", fkey_list->desc);
				} else {
					strcat_printf(buf, MAX_SCRN_LINE_BUF_LEN+1, "{-----} ");
				}
			}
		}
		display_reverse_text(central_win_get_key_list_line_y() + 0, buf);
	}
}
void disp_key_list(const char *key_lists[])
{
	for (int line_idx = 1; line_idx < get_key_list_lines(); line_idx++) {
		display_func_id_key(central_win_get_key_list_line_y() + line_idx, key_lists[line_idx - 1]);
	}
}
// display text parenthesized by {} in reverse
// "{Menu}  {RecKM} ..."
PRIVATE void display_reverse_text(int yy, const char *text)
{
	int xx = 0;
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	central_win_clear_lines(yy, -1);
	// get default fkey_list
	for (const char *ptr = text; *ptr && xx < central_win_get_columns(); ) {
		char delimiter = 0;
		const char *begin;
		if (*ptr == '{' || *ptr == '}') {
			delimiter = *ptr++;
		}
		for (begin = ptr; *ptr; ptr++) {
			if (*ptr == '{' || *ptr == '}')
				break;
		}
		if (ptr > begin) {
			char buf[MAX_SCRN_LINE_BUF_LEN+1];
			strlcpy__(buf, begin, ptr - begin);
			if (delimiter == '{') {
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
			}
			int columns = LIM_MAX(central_win_get_columns() - xx,
			 utf8s_columns(buf, MAX_SCRN_COLS));
			adjust_str_columns(buf, columns);
			central_win_output_string(yy, xx, buf, -1);
			xx += columns;
			if (delimiter == '{') {
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
			}
		}
	}
}
// display text parenthesized by <> in reverse
// "<doe_first_line>TopOfFile ..."
PRIVATE void display_func_id_key(int yy, const char *text)
{
	int xx = 0;
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	central_win_clear_lines(yy, -1);
	func_key_list_t *fkey_list = NULL;
	for (const char *ptr = text; *ptr && xx < central_win_get_columns(); ) {
		char delimiter = 0;
		const char *begin;
		if (*ptr == '<' || *ptr == '>') {
			delimiter = *ptr++;
		}
		for (begin = ptr; *ptr; ptr++) {
			if (*ptr == '<' || *ptr == '>')
				break;
		}
		if (ptr > begin) {
			char buf[MAX_SCRN_LINE_BUF_LEN+1];
			strlcpy__(buf, begin, ptr - begin);
			if (delimiter == '<') {
				fkey_list = get_fkey_entry_from_func_id(buf);
				if (fkey_list) {
					strlcpy__(buf, short_key_name_from_func_id(buf), MAX_SCRN_LINE_BUF_LEN);
					if (is_fkey_entry_executable(fkey_list, -1) < 2) {
						fkey_list = NULL;
					}
				}
			}
			if (delimiter == '<') {
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
			}
			if (fkey_list != NULL) {
				int columns = LIM_MAX(central_win_get_columns() - xx,
				 utf8s_columns(buf, MAX_SCRN_COLS));
				adjust_str_columns(buf, columns);
				central_win_output_string(yy, xx, buf, -1);
				xx += columns;
			}
			if (delimiter == '<') {
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
			}
		}
	}
}

// End of winin.c
