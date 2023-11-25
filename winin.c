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

PRIVATE int input_str_pos_(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, va_list ap);
PRIVATE int input_str_pos__(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg);
PRIVATE void disp_input_box(const char *buf, const char *input_buf, int x);
PRIVATE void blank_input_box(void);

// input string
int input_string(const char *default__, char *input_buf,
 int hist_type_idx, const char *msg, ...)
{
	va_list ap;
	int ret;

	va_start(ap, msg);
	ret = input_str_pos_(default__, input_buf, MAX_PATH_LEN, hist_type_idx, msg, ap);
	va_end(ap);
	return ret;
}

int input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...)
{
	va_list ap;
	int ret;

	va_start(ap, msg);
	ret = input_str_pos_(default__, input_buf, cursor_byte_idx, hist_type_idx, msg, ap);
	va_end(ap);
	return ret;
}

// return -1: recursively called
// return 0 : cancelled
// return 1 : input normally
PRIVATE int input_str_pos_(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, va_list ap)
{
	static int recursively_called = 0;
	char dir_save[MAX_PATH_LEN + 1];
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];
	int byte_idx;
	int ret;

	if (recursively_called) {
		return -1;				// -1: recursive called
	}
	recursively_called = 1;

	vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
	byte_idx = byte_idx_from_col_idx(msg_buf, main_win_get_columns(), CHAR_LEFT, NULL);
	msg_buf[byte_idx] = '\0';		// limit message length

	set_work_space_color_low();
	update_screen_app(1, 1, 1);
	set_work_space_color_normal();
	get_cur_dir(dir_save);

	tio_set_cursor_on(1);
	//---------------------------------------------------------------------------------
	ret = input_str_pos__(default__, input_buf, cursor_byte_idx, hist_type_idx, msg_buf);
	//---------------------------------------------------------------------------------
flf_d_printf("ret: %d\n", ret);
	// -1: cancelled
	//  0: quited
	//  1: string is normally input
	tio_set_cursor_on(0);

	change_cur_dir(dir_save);
	update_screen_app(1, 1, 1);
	recursively_called = 0;

	if (ret < 0) {
		disp_status_bar_done(_("Cancelled"));
		return ret;			// -1: cancelled
	}
	if (ret == 0) {
		return ret;			// 0: something done and quited input
	}
#ifdef ENABLE_HISTORY
	if (is_strlen_not_0(input_buf)) {
		update_history(hist_type_idx, input_buf, 0);
	}
#endif
	return 1;					// 1: input normally
}

// Input string. This should only be called from input_string().
// return 0 : cancelled
// return 1 : input normally
PRIVATE int input_str_pos__(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg)
{
	int key_input;
	const char *func_id;
	int bytes;
#if defined(ENABLE_HISTORY) || defined(ENABLE_FILER)
	char buffer[MAX_PATH_LEN+1];
	int ret;
#endif
	char cut_buf[MAX_PATH_LEN+1] = "";

	strlcpy__(input_buf, default__, MAX_PATH_LEN);
	cursor_byte_idx = MIN_MAX_(0, cursor_byte_idx, strnlen(default__, MAX_PATH_LEN));

	blank_key_list_lines();
	for ( ; ; ) {
		disp_input_box(msg, input_buf, cursor_byte_idx);
		tio_refresh();
		//---------------------------
		key_input = input_key_loop();
		//---------------------------
mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));

		if (IS_CHAR_KEY(key_input)) {
			// character key
			if (strnlen(input_buf, MAX_PATH_LEN) < MAX_PATH_LEN) {
				memmove(input_buf + cursor_byte_idx + 1, input_buf + cursor_byte_idx,
				 strnlen(input_buf, MAX_PATH_LEN) - cursor_byte_idx + 1);
				input_buf[cursor_byte_idx] = key_input;
				cursor_byte_idx++;
			}
			func_id = "";
		} else {
			// function key
			func_id = get_func_id_from_key(key_input);
		}
		if (key_input == K_ESC || key_input == K_M_ESC
		 || cmp_func_id(func_id, "doe_close_file_ask")
		 || cmp_func_id(func_id, "doe_close_all_ask")) {
			strcpy__(input_buf, "");
			key_input = K_ESC;
		} else
		if (key_input == K_C_M || key_input == K_ENTER) {
			key_input = K_C_M;
		} else
		if (cmp_func_id(func_id, "doe_left")) {
			// cursor left
			if (cursor_byte_idx > 0) {
				bytes = utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
				cursor_byte_idx -= bytes;
			}
		} else
		if (cmp_func_id(func_id, "doe_right")) {
			// cursor right
			if (cursor_byte_idx < strnlen(input_buf, MAX_PATH_LEN)) {
				bytes = utf8c_bytes(&input_buf[cursor_byte_idx]);
				cursor_byte_idx += bytes;
			}
		} else
		if (cmp_func_id(func_id, "doe_start_of_line")
		 || cmp_func_id(func_id, "doe_prev_word")) {
			// goto line head
			cursor_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "doe_end_of_line")
		 || cmp_func_id(func_id, "doe_next_word")) {
			// goto line tail
			cursor_byte_idx = strnlen(input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "doe_backspace")
		 || (key_input == K_BS)) {
			// backspace
			if (cursor_byte_idx > 0) {
				bytes = utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
				cursor_byte_idx -= bytes;
				delete_str(input_buf, cursor_byte_idx, bytes);
			}
		} else
		if (cmp_func_id(func_id, "doe_delete_char")
		 || (key_input == K_DEL)) {
			// delete
			if (cursor_byte_idx < strnlen(input_buf, MAX_PATH_LEN)) {
				bytes = utf8c_bytes(&input_buf[cursor_byte_idx]);
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
			strcpy(input_buf, "");
			cursor_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "doe_cut_to_tail")) {
			// cut to line tail
			strcut__(cut_buf, MAX_PATH_LEN,
			 input_buf, cursor_byte_idx, strnlen(input_buf, MAX_PATH_LEN));
			delete_str(input_buf, cursor_byte_idx, strnlen(input_buf, MAX_PATH_LEN) - cursor_byte_idx);
		} else
		if (cmp_func_id(func_id, "doe_copy_text")) {
			// copy to the cut buffer
			strlcpy__(cut_buf, input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "doe_paste_text_with_pop")) {
			// paste from the cut buffer
			insert_str(input_buf, MAX_PATH_LEN, cursor_byte_idx, cut_buf, -1);
#ifdef ENABLE_HISTORY
		} else
		if (key_input == K_TAB) {
			// tab history completion
			strlcpy__(input_buf, get_history_completion(hist_type_idx, input_buf),
			 MAX_PATH_LEN);
			cursor_byte_idx = strnlen(input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "doe_up")
		 || cmp_func_id(func_id, "doe_page_up")
		 || cmp_func_id(func_id, "doe_first_line")) {
			//----------------------------------------------------
			ret = select_from_history_list(hist_type_idx, buffer);
			//----------------------------------------------------
			if (ret > 0) {
				if (cmp_func_id(func_id, "doe_up")) {
					// clear input buffer
					strcpy__(input_buf, "");
					cursor_byte_idx = 0;
				}
				cursor_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 cursor_byte_idx, buffer);
			} else
			if (ret == 0) {
				key_input = KNA;	// quit
			}
#endif // ENABLE_HISTORY
#ifdef ENABLE_FILER
		} else
		if (cmp_func_id(func_id, "doe_down")
		 || cmp_func_id(func_id, "doe_page_down")
		 || cmp_func_id(func_id, "doe_last_line")) {
			//---------------------------------------------------
			ret = call_filer(1, 1, "", "", buffer, MAX_PATH_LEN);
			//---------------------------------------------------
flf_d_printf("ret: %d\n", ret);
			if (ret > 0) {
				if (cmp_func_id(func_id, "doe_down")) {
					// clear input buffer
					strcpy__(input_buf, "");
					cursor_byte_idx = 0;
				}
				cursor_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 cursor_byte_idx, buffer);
			}
#endif // ENABLE_FILER
		} else
		if (cmp_func_id(func_id, "doe_search_backward_first")
		 || cmp_func_id(func_id, "doe_search_forward_first")
		 || cmp_func_id(func_id, "doe_replace")) {
			// get string from edit buffer's current cursor position
			if (count_edit_bufs()) {
				char *line = CEPBV_CL->data;
				int byte_idx = byte_idx_from_byte_idx(line,
				 CEPBV_CLBI + strnlen(input_buf, MAX_PATH_LEN));
				// copy one token (at least copy one character)
				cursor_byte_idx = 0;
				for ( ;
				 (strnlen(input_buf, MAX_PATH_LEN) < MAX_PATH_LEN - MAX_UTF8C_BYTES)
				  && (byte_idx < strnlen(line, MAX_PATH_LEN))
				  && (cursor_byte_idx == 0
				   || (isalnum(line[byte_idx]) || line[byte_idx] == '_')); ) {
					strlncat__(input_buf, MAX_PATH_LEN, &line[byte_idx],
					 utf8c_bytes(&line[byte_idx]));
					cursor_byte_idx++;
					byte_idx += utf8c_bytes(&line[byte_idx]);
				}
				cursor_byte_idx = strnlen(input_buf, MAX_PATH_LEN);
			}
		}
		if (key_input == K_ESC || key_input == KNA || key_input == K_C_M)
			break;
	}

	if (key_input == K_ESC)
		return -1;						// cancelled, no input
	if (key_input == KNA)
		return 0;						// done in editor
	return 1;							// input
}

/* display input box
#Prompt message#######################################################
#input-string                                                        #
###UP:history##DOWN:browser###########################################

-Prompt message-------------------------------------------------------
-input-string                                                        -
----------------------------------------------------------------------

-Prompt message-------------------------------------------------------
|input-string                                                        |
----------------------------------------------------------------------

+Prompt message------------------------------------------------------+
|input-string                                                        |
+--------------------------------------------------------------------+
+Prompt message------------------------------------------------------+
+input-string                                                        +
+--------------------------------------------------------------------+
*/
PRIVATE void disp_input_box(const char *msg, const char *input_buf, int cursor_byte_idx)
{
	int cursor_col_idx = col_idx_from_byte_idx(input_buf, 0, cursor_byte_idx);
	int input_area_width;
	int start_byte_idx;
	int bytes;

	blank_input_box();
	set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	main_win_output_string(get_input_line_y(), 1, msg, -1);
	main_win_output_string(get_input_line_y()+2, 1,
	 _("UP:history, DOWN:browser, PgUp:insert from history, PgDn:insert from browser"), -1);
	set_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);

	input_area_width = main_win_get_columns()-2;
	if (cursor_col_idx < input_area_width) {
		bytes = byte_idx_from_col_idx(input_buf, input_area_width, CHAR_LEFT, NULL);
		main_win_output_string(get_input_line_y()+1, 1, input_buf, bytes);
		main_win_set_cursor_pos(get_input_line_y()+1, 1 + cursor_col_idx);
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
		bytes = byte_idx_from_col_idx(&input_buf[start_byte_idx],
		 input_area_width-utf8s_columns(TRUNCATION_MARK, INT_MAX), CHAR_LEFT, NULL);
		main_win_output_string(get_input_line_y()+1, 1, TRUNCATION_MARK, -1);
		main_win_output_string(-1, -1, &input_buf[start_byte_idx], bytes);
		main_win_set_cursor_pos(get_input_line_y()+1,
		 1 + utf8s_columns(TRUNCATION_MARK, INT_MAX)
		 + col_idx_from_byte_idx(&input_buf[start_byte_idx],
		 0, cursor_byte_idx - start_byte_idx));
	}
}
PRIVATE void blank_input_box(void)
{
	// display frame
	set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	//+--------------------------------------------------------------------+
	//|                                                                    |
	//+--------------------------------------------------------------------+
	main_win_clear_lines(get_input_line_y(), get_input_line_y() + 3);
	// clear input area
	set_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);
	//+--------------------------------------------------------------------+
	//|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
	//+--------------------------------------------------------------------+
	main_win_output_string(get_input_line_y()+1, 1, tio_blank_line(), main_win_get_columns()-2);
}

//-----------------------------------------------------------------------------

PRIVATE void list_one_key(char key, const char *desc);

// Ask a simple yes/no question on the status_bar.
int ask_yes_no(int flags, const char *msg, ...)
{
	int key_lines_save;
	va_list ap;
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];
	const char *chars_yes = "Yy";				// Yes(replace)
	const char *chars_no = "Nn";				// No
	const char *chars_all = "Aa";				// All
	const char *chars_backward = "Bb";			// Backward search
	const char *chars_forward = "Ff ";			// Forward search
	const char *chars_cancel = "EeSsXx";		// End/Stop/eXit
	const char *chars_end = "QqRr\x1b" S_C_Q;	// Quit/Return/cancel
	const char *chars_undo = "Uu";				// Undo
	const char *chars_redo = "Oo";				// redO
	key_code_t key_input;
	int answer;
	int byte_idx;
	const char *func_id;

	key_lines_save = GET_APPMD(app_KEY_LINES);		// save KEY_LINES
	SET_APPMD_VAL(app_KEY_LINES, LIM_MIN(1, key_lines_save));	// set lines more than 1

	blank_key_list_lines();
	// First, display key list
	if (get_key_list_lines()) {
		blank_yes_no_line();
		main_win_set_cursor_pos(get_yes_no_line_y(), 0);
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

	// Second, display prompt message
	if (*msg) {
		va_start(ap, msg);
		vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
		va_end(ap);
		strlcat__(msg_buf, MAX_SCRN_LINE_BUF_LEN, " ");
		byte_idx = byte_idx_from_col_idx(msg_buf, MAX_SCRN_LINE_BUF_LEN, CHAR_LEFT, NULL);
		msg_buf[byte_idx] = '\0';
		set_color_by_idx(ITEM_COLOR_IDX_WARNING, 0);
		blank_status_bar();
		main_win_output_string(main_win_get_bottom_win_y() + STATUS_LINE, 0, msg_buf, -1);
	}

	tio_refresh();

	for (answer = ANSWER_NONE; answer == ANSWER_NONE; ) {
		tio_set_cursor_on(1);
		//---------------------------
		key_input = input_key_loop();
		func_id = get_func_id_from_key(key_input);
		//---------------------------
		// Look for the key_input in yes/no/all
		if (strchr__(chars_yes, key_input) != NULL)
			answer = ANSWER_YES;
		else if (strchr__(chars_no, key_input) != NULL)
			answer = ANSWER_NO;
		else if ((flags & ASK_ALL) && strchr__(chars_all, key_input) != NULL)
			answer = ANSWER_ALL;
		else if ((flags & ASK_BACKWARD) && (strchr__(chars_backward, key_input) != NULL)
		 || cmp_func_id(func_id, "doe_search_backward_next"))
			answer = ANSWER_BACKWARD;
		else if ((flags & ASK_FORWARD) && (strchr__(chars_forward, key_input) != NULL)
		 || cmp_func_id(func_id, "doe_search_forward_next"))
			answer = ANSWER_FORWARD;
		else if (strchr__(chars_cancel, key_input) != NULL)
			answer = ANSWER_CANCEL;
		else if ((flags & ASK_END || flags & ASK_NO) && strchr__(chars_end, key_input) != NULL)
			answer = ANSWER_END;
		else if ((flags & ASK_UNDO) && strchr__(chars_undo, key_input) != NULL)
			answer = ANSWER_UNDO;
		else if ((flags & ASK_REDO) && strchr__(chars_redo, key_input) != NULL)
			answer = ANSWER_REDO;
	}

	// Then blank the status_bar.
	set_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();
	disp_key_list_editor();

	SET_APPMD_VAL(app_KEY_LINES, key_lines_save);	// recover KEY_LINES
	update_screen_app(1, 1, 1);

	return answer;
}

PRIVATE void list_one_key(char key, const char *desc)
{
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s", short_key_name_from_key_code(key, key_name));
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
	main_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s ", desc);
	main_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
}

//-----------------------------------------------------------------------------

void disp_key_list(char *key_lists[])
{
	int idx;

	for (idx = 0; idx < get_key_list_lines(); idx++) {
		display_reverse_text(main_win_get_bottom_win_y() + KEY_LIST_LINE + idx,
		 key_lists[idx]);
	}
}
// text parenthesized by {} are displayed in reversed
void display_reverse_text(int yy, char *text)
{
	char *ptr;
	char *begin;
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	int columns = 1;
	int xx;
	char delimiter = 0;

	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_lines(yy, -1);
	xx = 0;
	for (ptr = text; *ptr && xx < main_win_get_columns(); xx += columns) {
		if (*ptr == '{' || *ptr == '<' || *ptr == '}' || *ptr == '>')
			delimiter = *ptr++;
		for (begin = ptr; *ptr; ptr++) {
			if (*ptr == '{' || *ptr == '<' || *ptr == '}' || *ptr == '>')
				break;
		}
		if (ptr > begin) {
			strlcpy__(buf, begin, ptr - begin);
			if (delimiter == '<') {
				strlcpy__(buf, short_key_name_from_func_id(buf), MAX_SCRN_LINE_BUF_LEN);
			}
			columns = LIM_MAX(main_win_get_columns() - xx, utf8s_columns(buf, INT_MAX));
			truncate_tail_utf8s_columns(buf, columns);
			expand_utf8s_columns(buf, columns);
			if (delimiter == '{' || delimiter == '<') {
				set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
			}
			main_win_output_string(yy, xx, buf, -1);
			if (delimiter == '{' || delimiter == '<') {
				set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
			}
		}
	}
}

// End of winin.c
