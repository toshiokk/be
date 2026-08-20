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

const char *get_do_next_name(do_next_t do_next)
{
	switch (do_next) {
	default:							return "EF_UNKNOWN";
	case EF_NONE:						return "EF_NONE";
	case FL_UPDATE_AUTO:				return "FL_UPDATE_AUTO";
	case FL_UPDATE_FORCE:				return "FL_UPDATE_FORCE";
	case EF_CANCELLED:					return "EF_CANCELLED";
	case EF_TO_QUIT:					return "EF_TO_QUIT";
	case EF_LOADED_GO_TO_ROOT_EDITOR:	return "EF_LOADED_GO_TO_ROOT_EDITOR";
	case EF_EXECUTED_RET_TO_CALLER:		return "EF_EXECUTED_RET_TO_CALLER";
#ifdef ENABLE_FILER
	case EF_GO_TO_LEVEL_FILER:			return "EF_GO_TO_LEVEL_FILER";
#endif // ENABLE_FILER
	case EF_ENTER_STRING:				return "EF_ENTER_STRING";
	case EF_ENTER_STRING_ADD:			return "EF_ENTER_STRING_ADD";
	case EF_INPUT_PATH_TO_COPY:			return "EF_INPUT_PATH_TO_COPY";
	case EF_INPUT_PATH_TO_MOVE:			return "EF_INPUT_PATH_TO_MOVE";
	}
}

fname_0_fpath_1_dpath_2_t input_fname_0_fpath_1 = ENTER_FILE_NAME;

PRIVATE do_next_t input_str_pos_(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, va_list ap);
PRIVATE do_next_t input_str_main_loop(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg);
PRIVATE void disp_input_box(const char *buf, const char *input_buf, int x);
PRIVATE void blank_input_box();

PRIVATE int determine_input_box_y();
PRIVATE int get_input_box_y();

// input_string_pos() inputs:
//   - "file-name"
//   - "/absolute/path/to/directory"
//   - "command line to execute"
//   - "keyword"
//   - "keymacro-string"
// input_full_path() inputs "/absolute/path/to/file" being called from:
//   - _doe_open_file_recursive()
//   - doe_open_new_file()
//   - input_new_file_name__ask()
//   - _dof_open_new_file()

do_next_t input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...)
{
	disp_status_bar_cwd();
	va_list ap;
	va_start(ap, msg);
	input_str_pos_(default__, input_buf, cursor_byte_idx, hist_type_idx, msg, ap);
	va_end(ap);
	disp_status_bar_cwd();
	return app_do_next;
}
do_next_t input_full_path(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...)
{
	disp_status_bar_cwd();
	va_list ap;
	va_start(ap, msg);
	input_fname_0_fpath_1 = ENTER_FILE_PATH;
	input_str_pos_(default__, input_buf, cursor_byte_idx, hist_type_idx, msg, ap);
	input_fname_0_fpath_1 = ENTER_FILE_NAME;
	va_end(ap);
	disp_status_bar_cwd();
	return app_do_next;
}

// return -1: recursively called
// return 0 : cancelled
// return 1 : input normally
PRIVATE do_next_t input_str_pos_(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, va_list ap)
{
	static int recursively_called = 0;

	if (recursively_called >= 3) {
		return -1;				// -1: recursive called
	}
	recursively_called++;
	inc_sub_win_depth();

	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];
	vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);

	editor_panes_t next_eps;
	editor_panes_t *prev_eps;
	app_mode_t prev_app_mode;

#ifdef ENABLE_FILER
	filer_panes_t next_fps;
	filer_panes_t *prev_fps;
	push_app_stack(&next_eps, NULL, &next_fps, &prev_eps, &prev_fps, &prev_app_mode);
#else // ENABLE_FILER
	push_app_stack(&next_eps, NULL, &prev_eps, &prev_app_mode);
#endif // ENABLE_FILER

	char dir_save[MAX_PATH_LEN + 1];
	get_full_path_of_cur_dir(dir_save);
	tio_set_cursor_on(1);
	//--------------------------------------------------------------------------
	input_str_main_loop(default__, input_buf, cursor_byte_idx, hist_type_idx, msg_buf);
	//--------------------------------------------------------------------------
	flf_dprintf("app_stk: %d, do_next__[%s]\n",
	 get_app_stack_depth(), get_do_next_name(app_do_next));

	// editor: refrect from the callee's cur_dir to the caller's cur_dir if file loaded
	// filer : propagate the current directory only when called as a normal-mode
flf_dprintf("is_epc_buf_modified(): %d\n", is_epc_buf_modified());
#ifdef ENABLE_FILER
	pop_app_stack(prev_eps, prev_fps, prev_app_mode,
	 app_do_next == EF_LOADED_GO_TO_ROOT_EDITOR, 0);
#else // ENABLE_FILER
	pop_app_stack(prev_eps, prev_app_mode,
	 app_do_next == EF_LOADED_GO_TO_ROOT_EDITOR);
#endif // ENABLE_FILER

	tio_set_cursor_on(0);
	chdir__(dir_save);

	update_screen_app(S_B_CURS, 1);

	if (app_do_next <= EF_TO_QUIT) {
		disp_status_bar_warn(_("Inputting a string cancelled"));
	}
#ifdef ENABLE_HISTORY
	if (IS_EF_ENTER_STRING(app_do_next)) {
		// input normally
		modify_history_w_reloading(hist_type_idx, input_buf);
	}
#endif

	dec_sub_win_depth();
	recursively_called--;
flf_dprintf("do_next__[%s]\n", get_do_next_name(app_do_next));
	return app_do_next;
}

// Input string. This should only be called from input_string_xxx().
PRIVATE do_next_t input_str_main_loop(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *message)
{
	int key_input;
#if defined(ENABLE_HISTORY) || defined(ENABLE_FILER)
	char str_buf[MAX_PATH_LEN+1];
	char filer_dir[MAX_PATH_LEN+1];
#endif
	char local_cut_buf[MAX_PATH_LEN+1] = "";

	strlcpy__(input_buf, default__, MAX_PATH_LEN);
	cursor_byte_idx = MIN_MAX_(0, cursor_byte_idx, strlen_path(default__));

	// Main input loop
	for ( ; ; ) {
		SET_app_do_next(EF_NONE);
		disp_input_box(message, input_buf, cursor_byte_idx);
		//----------------------------------
		key_input = input_key_timeout();
		//----------------------------------
		hmflf_dprintf("input%ckey:0x%04x(%s)===================================================\n",
		 '_', (UINT16)key_input, get_key_name_from_key_code(key_input, NULL));

		if (is_key_print(key_input)) {
			// character key
			if (strlen_path(input_buf) < MAX_PATH_LEN) {
				insert_str_chr(input_buf, MAX_PATH_LEN, cursor_byte_idx, key_input);
				cursor_byte_idx++;
			}
			continue;
		}
		// function key
		const char *func_id = get_func_id_from_key(key_input);
		if ((key_input == K_ESC) || (key_input == K_M_ESC)
		 || (strcmp(func_id, "doe_close_file_ask") == 0)
		 || (strcmp(func_id, "doe_close_all_ask") == 0)
		 || (strcmp(func_id, "doe_close_all_always") == 0)) {
			strcpy__(input_buf, "");
			SET_app_do_next(EF_CANCELLED);				// cancelled, return
		} else if (strcmp(func_id, "doe_carriage_return") == 0) {
			SET_app_do_next(EF_ENTER_STRING);			// confirm a string input
		} else if (strcmp(func_id, "doe_carriage_return_indent") == 0) {
			SET_app_do_next(EF_ENTER_STRING_ADD);		// confirm a string input
		} else if ((strcmp(func_id, "dof_copy_file_update") == 0)
		 || (strcmp(func_id, "dof_drop_files_to_copy") == 0)) {
			SET_app_do_next(EF_INPUT_PATH_TO_COPY);
		} else if ((strcmp(func_id, "dof_move_file") == 0)
		 || (strcmp(func_id, "dof_drop_files_to_move") == 0)) {
			SET_app_do_next(EF_INPUT_PATH_TO_MOVE);
		} else if (strcmp(func_id, "doe_left") == 0) {
			// cursor left
			if (cursor_byte_idx > 0) {
				cursor_byte_idx -= utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
			}
		} else if (strcmp(func_id, "doe_right") == 0) {
			// cursor right
			if (cursor_byte_idx < strlen_path(input_buf)) {
				cursor_byte_idx += utf8c_bytes(&input_buf[cursor_byte_idx]);
			}
		} else if (strcmp(func_id, "doe_prev_word") == 0) {
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
		} else if (strcmp(func_id, "doe_next_word") == 0) {
			// goto a next word
			while (cursor_byte_idx < strlen_path(input_buf)) {
				cursor_byte_idx += utf8c_bytes(&input_buf[cursor_byte_idx]);
				if (input_buf[cursor_byte_idx] == ' ') {
					cursor_byte_idx++;	// skip a space
					break;
				}
			}
		} else if (strcmp(func_id, "doe_start_of_line") == 0) {
			// goto line head
			cursor_byte_idx = 0;
		} else if (strcmp(func_id, "doe_end_of_line") == 0) {
			// goto line tail
			cursor_byte_idx = strlen_path(input_buf);
		} else if ((strcmp(func_id, "doe_backspace") == 0)
		 || (key_input == AK_BS)) {
			// backspace
			if (cursor_byte_idx > 0) {
				int bytes = utf8c_prev_bytes(input_buf, &input_buf[cursor_byte_idx]);
				cursor_byte_idx -= bytes;
				delete_str(input_buf, cursor_byte_idx, bytes);
			}
		} else if ((strcmp(func_id, "doe_delete_char") == 0)
		 || (key_input == AK_DEL)) {
			// delete
			if (cursor_byte_idx < strlen_path(input_buf)) {
				int bytes = utf8c_bytes(&input_buf[cursor_byte_idx]);
				delete_str(input_buf, cursor_byte_idx, bytes);
			}
		} else if (strcmp(func_id, "doe_cut_to_head") == 0) {
			// cut to line head
			strcut__(local_cut_buf, MAX_PATH_LEN, input_buf, 0, cursor_byte_idx);
			delete_str(input_buf, 0, cursor_byte_idx);
			cursor_byte_idx = 0;
		} else if (strcmp(func_id, "doe_cut_text") == 0) {
			// cut line
			strlcpy__(local_cut_buf, input_buf, MAX_PATH_LEN);
			strcpy__(input_buf, "");
			cursor_byte_idx = 0;
		} else if (strcmp(func_id, "doe_cut_to_tail") == 0) {
			// cut to line tail
			strcut__(local_cut_buf, MAX_PATH_LEN,
			 input_buf, cursor_byte_idx, strlen_path(input_buf));
			delete_str(input_buf, cursor_byte_idx,
			 strlen_path(input_buf) - cursor_byte_idx);
		} else if (strcmp(func_id, "doe_copy_text") == 0) {
			// copy to the cut buffer
			strlcpy__(local_cut_buf, input_buf, MAX_PATH_LEN);
		} else if ((strcmp(func_id, "doe_paste_text_with_pop") == 0)
		 || (strcmp(func_id, "doe_paste_text_without_pop") == 0)) {
			// paste from the cut buffer
			if (strlen_path(local_cut_buf)) {
				insert_str(input_buf, MAX_PATH_LEN, cursor_byte_idx, local_cut_buf, -1);
			} else {
				const char *cut_buf;
				if (strcmp(func_id, "doe_paste_text_with_pop") == 0) {
					cut_buf = get_text_from_cut_buffer_with_pop();
				} else {
					cut_buf = get_text_from_cut_buffer_without_pop();
				}
				insert_str(input_buf, MAX_PATH_LEN, cursor_byte_idx, cut_buf, -1);
			}
			cursor_byte_idx = LIM_MAX(MAX_PATH_LEN, cursor_byte_idx + strlen_path(local_cut_buf));
		} else if ((strcmp(func_id, "doe_search_first_backward") == 0)
		 || (strcmp(func_id, "doe_search_first_forward") == 0)
		 || (strcmp(func_id, "doe_replace") == 0)) {
			// get string from edit buffer's current cursor position
_FLF_
			if (count_edit_bufs()) {
				char *line = EPCBVC_CL->data;
				cursor_byte_idx = strlen_path(input_buf);
flf_dprintf("line: [%s]\n", line + cursor_byte_idx);
				int start_byte_idx = byte_idx_from_byte_idx(line, EPCBVC_CLBI + cursor_byte_idx);
				int byte_idx = start_byte_idx;
				if (((strcmp(func_id, "doe_search_first_backward") == 0) && (SEARCH_DIR() > 0))
				 || ((strcmp(func_id, "doe_search_first_forward") == 0) && (SEARCH_DIR() < 0))) {
					// copy whole line
					byte_idx = byte_idx_from_byte_idx(line, MAX_PATH_LEN);
flf_dprintf("byte_idx: %d\n", byte_idx);
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
if (strlen(input_buf) == 0) {
progerr_printf("cutting text from editing text failed !!!!");
_WARNING_
set_application_warning_state();
flf_dprintf("input_buf: [%s]\n", input_buf);
}
			}
#ifdef ENABLE_HISTORY
		} else if (key_input == K_TAB) {
			// tab history completion
			strlcpy__(input_buf, get_history_completion(hist_type_idx, input_buf),
			 MAX_PATH_LEN);
			cursor_byte_idx = strlen_path(input_buf);
		} else if ((strcmp(func_id, "doe_up") == 0)
		 || (strcmp(func_id, "doe_first_line") == 0)) {
			load_histories_if_file_newer();
			load_cut_buffers_if_file_newer();
			//==================================================================
			do_call_editor(1, APP_MODE_VIEWER, get_history_buf(hist_type_idx), str_buf);
			//==================================================================
			hmflf_dprintf("app_stk: %d, do_next__[%s]\n",
			 get_app_stack_depth(), get_do_next_name(app_do_next));
			if (IS_EF_ENTER_STRING(app_do_next)) {
				if (app_do_next == EF_ENTER_STRING) {
					// clear input buffer
					strcpy__(input_buf, "");
					cursor_byte_idx = 0;
				} else {
					// insert string into buffer without clearing buffer
				}
				cursor_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 cursor_byte_idx, str_buf);
			}
#ifdef ENABLE_FILER
			if (app_do_next == EF_GO_TO_LEVEL_FILER) {
				strcpy__(filer_dir, "");
			} else
#endif // ENABLE_FILER
			if (! IS_EF_LOADED_OR_EXECUTED(app_do_next)) {
				SET_app_do_next(EF_NONE);
			}
#endif // ENABLE_HISTORY
#ifdef ENABLE_FILER
		} else if ((strcmp(func_id, "doe_down") == 0)
		 || (strcmp(func_id, "doe_last_line") == 0)) {
			SET_app_do_next(EF_GO_TO_LEVEL_FILER);
			strcpy__(filer_dir, "");
		} else if ((strcmp(func_id, "doe_page_up") == 0)
		 || (strcmp(func_id, "doe_page_down") == 0)) {
			SET_app_do_next(EF_GO_TO_LEVEL_FILER);
			strlcpy__(filer_dir, input_buf, MAX_PATH_LEN);
		}

		//---------------------------------------------------
		if (app_do_next == EF_GO_TO_LEVEL_FILER) {
			//==================================================================
			do_call_filer(1, APP_MODE_VIEWER, filer_dir, "", str_buf);
			//==================================================================
			hmflf_dprintf("app_stk: %d, do_next__[%s]\n",
			 get_app_stack_depth(), get_do_next_name(app_do_next));
			if (IS_EF_ENTER_STRING(app_do_next)) {
				if (app_do_next == EF_ENTER_STRING) {
					// clear input buffer
					strcpy__(input_buf, "");
					cursor_byte_idx = 0;
				} else {
					// insert string into buffer without clearing buffer
				}
				cursor_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 cursor_byte_idx, str_buf);
			}
			if (! IS_EF_LOADED_OR_EXECUTED(app_do_next)) {
				SET_app_do_next(EF_NONE);
			}
#endif // ENABLE_FILER
		}
hmflf_dprintf("0x%04x(%s)\n", (UINT16)key_input, get_key_name_from_key_code(key_input, NULL));
		if ((app_do_next == EF_CANCELLED)
		 || IS_EF_ENTER_STRING(app_do_next) || IS_EF_LOADED_OR_EXECUTED(app_do_next)) {
			break;
		}
		sync_cut_buffers_and_histories(0);
	}
flf_dprintf("input_buf:[%s]\n", input_buf);
flf_dprintf("key_input: %04x, do_next__[%s]\n", key_input, get_do_next_name(app_do_next));
	return app_do_next;
} // input_str_main_loop

// display input box
// +Prompt message------------------------------------------------------+
// |input-string                                                        |
// |UP:history DOWN:browser---------------------------------------------|
// +CWD: /usr/local/bin-------------------------------------------------+
#define INPUT_BOX_LINES				4
#define INPUT_BOX_X_L_BORDER_WIDTH	1
#define INPUT_BOX_X_R_BORDER_WIDTH	1
#define INPUT_BOX_X_LR_BORDER_WIDTH	(INPUT_BOX_X_L_BORDER_WIDTH + INPUT_BOX_X_R_BORDER_WIDTH)
#define INPUT_BOX_Y_PROMPT			0
#define INPUT_BOX_Y_INPUT			1
#define INPUT_BOX_Y_HELP			2
#define INPUT_BOX_Y_CWD				3
PRIVATE void disp_input_box(const char *msg, const char *input_buf, int cursor_byte_idx)
{
	determine_input_box_y();
	blank_input_box();
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);

	char curs_pos[MAX_SCRN_LINE_BUF_LEN+1];		// " 99/999"
	snprintf_(curs_pos, MAX_SCRN_LINE_BUF_LEN, " %d/%d", cursor_byte_idx, strlen_path(input_buf));
	int msg_space = central_win_get_columns()-INPUT_BOX_X_LR_BORDER_WIDTH - strlen_path(curs_pos);
	int byte_idx = byte_idx_from_col_idx(msg, msg_space, CHAR_LEFT, NULL);
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];		// "Messages... 99/999"
	strlcpy__(msg_buf, msg, MAX_SCRN_LINE_BUF_LEN);
	msg_buf[byte_idx] = '\0';					// limit message length
	strcat_printf(msg_buf, MAX_SCRN_LINE_BUF_LEN, " %s", curs_pos);

	central_win_output_string(get_input_box_y()+INPUT_BOX_Y_PROMPT,
	 INPUT_BOX_X_L_BORDER_WIDTH, msg_buf, -1);

	central_win_output_string(get_input_box_y()+INPUT_BOX_Y_HELP,
	 INPUT_BOX_X_L_BORDER_WIDTH, _("UP/PGUP:history, DOWN/PGDN:filer"), -1);

	int input_area_width = central_win_get_columns()-INPUT_BOX_X_LR_BORDER_WIDTH;
	char buf_cwd[MAX_PATH_LEN+1];
	snprintf_(buf_cwd, MAX_PATH_LEN, _("CWD: %s"), get_full_path_of_cur_dir(NULL));
	snprintf_(buf_cwd, MAX_PATH_LEN, _("( %s )"), get_full_path_of_cur_dir(NULL));
	central_win_output_string(get_input_box_y()+INPUT_BOX_Y_CWD,
	 INPUT_BOX_X_L_BORDER_WIDTH, shrink_str(buf_cwd, input_area_width, 5), -1);
	set_item_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);

	int cursor_col_idx = col_idx_from_byte_idx(input_buf, cursor_byte_idx);
	if (cursor_col_idx < input_area_width) {
		int bytes = byte_idx_from_col_idx(input_buf, input_area_width, CHAR_LEFT, NULL);
		central_win_output_string(get_input_box_y()+INPUT_BOX_Y_INPUT,
		 INPUT_BOX_X_L_BORDER_WIDTH, input_buf, bytes);
		central_win_set_cursor_pos(get_input_box_y()+INPUT_BOX_Y_INPUT,
		 INPUT_BOX_X_L_BORDER_WIDTH + cursor_col_idx);
	} else {
#define TRUNCATION_MARK	"."
		//"abcdefghijklmnopqrstuvwxyz"
		// ==> |.ghijklmnopqrstuvwxyz |
		//                           ^cursor
		//    "abcdefghijklmnopqrstuvwxyz"
		// ==> |.cdefghijklmnopqrstuvw|
		//                           ^cursor
		//     "abcdefghijklmnopqrstuvwxyz"
		// ==> |abcdefghijklmnopqrstuv|
		//                           ^cursor
		int start_byte_idx = byte_idx_from_col_idx(input_buf,
		 cursor_col_idx - (input_area_width-2), CHAR_RIGHT, NULL);
		int bytes = byte_idx_from_col_idx(&input_buf[start_byte_idx],
		 input_area_width - utf8s_columns(TRUNCATION_MARK, MAX_SCRN_COLS), CHAR_LEFT, NULL);
		central_win_output_string(get_input_box_y()+INPUT_BOX_Y_INPUT,
		 INPUT_BOX_X_L_BORDER_WIDTH, TRUNCATION_MARK, -1);
		central_win_output_string(-1, -1, &input_buf[start_byte_idx], bytes);
		central_win_set_cursor_pos(get_input_box_y()+INPUT_BOX_Y_INPUT,
		 INPUT_BOX_X_L_BORDER_WIDTH + utf8s_columns(TRUNCATION_MARK, MAX_SCRN_COLS)
		 + col_idx_from_byte_idx(&input_buf[start_byte_idx], cursor_byte_idx - start_byte_idx));
	}
	tio_refresh();
}

PRIVATE void blank_input_box()
{
	// display frame
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	//+--------------------------------------------------------------------+
	//|                                                                    |
	//+--------------------------------------------------------------------+
	//+--------------------------------------------------------------------+
	central_win_clear_lines(get_input_box_y(), -INPUT_BOX_LINES);
	// clear input area
	set_item_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);
	//+--------------------------------------------------------------------+
	//|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
	//+--------------------------------------------------------------------+
	//+--------------------------------------------------------------------+
	int input_area_width = central_win_get_columns()-INPUT_BOX_X_LR_BORDER_WIDTH;
	central_win_output_string(get_input_box_y()+INPUT_BOX_Y_INPUT,
	 INPUT_BOX_X_L_BORDER_WIDTH, tio_blank_line(), input_area_width);
}

PRIVATE int input_box_y = 2;
// determines Y position of input-box avoiding the current cursor line
PRIVATE int determine_input_box_y()
{
	int cursor_y = win_get_saved_cursor_y();
	input_box_y = central_win_get_mid_win_y() + central_win_get_mid_win_lines() / 2;
	if (IS_IN_RANGE(input_box_y-1, cursor_y, input_box_y + INPUT_BOX_LINES+1)) {
		// avoid showing input box over the current cursor line
		if (input_box_y <= cursor_y) {
			// avoid above
			input_box_y = cursor_y - (INPUT_BOX_LINES+1);
		} else {
			// avoid below
			input_box_y = cursor_y + 1+1;
		}
	}
	return input_box_y = MIN_MAX_(TITLE_LINES, input_box_y, central_win_get_status_line_y());
}
PRIVATE int get_input_box_y()
{
	return input_box_y;
}

//------------------------------------------------------------------------------
PRIVATE int check_ask_yes_no_key(int flags, key_code_t key_input, const char *func_id);
PRIVATE void disp_ask_yes_no_msg(int flags);
PRIVATE void list_one_key(key_code_t key, const char *desc);

PRIVATE const char *chars_yes = "Yy";				// Yes(replace)
PRIVATE const char *chars_no = "Nn";				// No
PRIVATE const char *chars_all = "Aa";				// All
PRIVATE const char *chars_backward = "Bb";			// Backward search
PRIVATE const char *chars_forward = "Ff ";			// Forward search
PRIVATE const char *chars_cancel = "Cc" S_ESC;		// Cancel/ESC
PRIVATE const char *chars_stop = "Ss";				// Stop
PRIVATE const char *chars_end = "EeRrQq";			// End/Return
PRIVATE const char *chars_quit = "Qq" S_C_Q;		// Quit/Ctrl-Q
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

	int answer = ANSWER_NONE;
	for ( ; answer == ANSWER_NONE; ) {
		// display key list
		disp_ask_yes_no_msg(flags);

		set_item_color_by_idx(ITEM_COLOR_IDX_WARNING1, 0);
		blank_status_bar();
		central_win_output_string(central_win_get_status_line_y(), 0, msg_buf, -1);
		tio_refresh();

		// "Save modified buffer ?"[] <== cursor
		tio_set_cursor_on(1);
		//---------------------------------------------
		key_code_t key_input = input_key_timeout();
		//---------------------------------------------
		if (IS_KEY_VALID(key_input)) {
			hmflf_dprintf("input%ckey:0x%04x(%s)================\n",
			 '_', (UINT16)key_input, get_key_name_from_key_code(key_input, NULL));
			answer = check_ask_yes_no_key(flags, key_input, get_func_id_from_key(key_input));
		}
		sync_cut_buffers_and_histories(0);
	}

	// Then blank the status_bar.
	set_item_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();

	SET_APPMD_VAL(app_KEY_LINES, key_lines_save);	// restore KEY_LINES
	update_screen_app(S_B_CURS, 1);

	return answer;	// x > 0: yes, x = 0: no, x < 0: cancel
}
PRIVATE int check_ask_yes_no_key(int flags, key_code_t key_input, const char *func_id)
{
	int answer = ANSWER_NONE;
	// Look for the key_input in yes/no/all ...
	if ((flags & ASK_YES) && (strchr__(chars_yes, key_input) != NULL))
		answer = ANSWER_YES;
	else if ((flags & ASK_NO) && (strchr__(chars_no, key_input) != NULL))
		answer = ANSWER_NO;
	else if ((flags & ASK_ALL_YES) && (strchr__(chars_all, key_input) != NULL))
		answer = ANSWER_ALL;
	else if ((flags & ASK_BACKWARD) && ((strchr__(chars_backward, key_input) != NULL)
	  || (strcmp(func_id, "doe_up") == 0) || (strcmp(func_id, "doe_page_up") == 0)
	  || (strcmp(func_id, "doe_search_backward_next") == 0)))
		answer = ANSWER_BACKWARD;
	else if ((flags & ASK_FORWARD) && ((strchr__(chars_forward, key_input) != NULL)
	  || (strcmp(func_id, "doe_down") == 0) || (strcmp(func_id, "doe_page_down") == 0)
	  || (strcmp(func_id, "doe_search_forward_next") == 0)))
		answer = ANSWER_FORWARD;
	else if (strchr__(chars_cancel, key_input) != NULL)
		answer = ANSWER_CANCEL;
	else if ((flags & ASK_STOP) && (strchr__(chars_stop, key_input) != NULL))
		answer = ANSWER_CANCEL;
	else if ((flags & ASK_END) && (strchr__(chars_end, key_input) != NULL))
		answer = ANSWER_END;
	else if ((flags & ASK_QUIT) && (strchr__(chars_quit, key_input) != NULL))
		answer = ANSWER_QUIT;
	else if ((flags & ASK_UNDO) && (strchr__(chars_undo, key_input) != NULL))
		answer = ANSWER_UNDO;
	else if ((flags & ASK_REDO) && (strchr__(chars_redo, key_input) != NULL))
		answer = ANSWER_REDO;
	else if (flags & ASK_OTHERS) {
		answer = ANSWER_CANCEL;
	}
	return answer;
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
		if (flags & ASK_ALL_YES) {
			list_one_key(chars_all[0], _("All"));
		}
		if (flags & ASK_BACKWARD) {
			list_one_key(chars_backward[0], _("Backward"));
		}
		if (flags & ASK_FORWARD) {
			list_one_key(chars_forward[0], _("Forward"));
		}
		list_one_key(chars_cancel[0], _("Cancel"));
		if (flags & ASK_STOP) {
			list_one_key(chars_stop[0], _("Stop"));
		}
		if (flags & ASK_END) {
			list_one_key(chars_end[0], _("End"));
		}
		if (flags & ASK_QUIT) {
			list_one_key(chars_quit[0], _("Quit"));
		}
		if (flags & ASK_UNDO) {
			list_one_key(chars_undo[0], _("Undo"));
		}
		if (flags & ASK_REDO) {
			list_one_key(chars_redo[0], _("Redo"));
		}
	}
}
PRIVATE void list_one_key(key_code_t key, const char *desc)
{
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s", get_key_name_from_key_code(key, key_name));
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
	central_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s ", desc);
	central_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
}

//------------------------------------------------------------------------------
PRIVATE void display_reverse_text(int yy, const char *text);
void disp_fkey_list()
{
	key_code_t func_keys[] = { K_ESC,
	 K_SP, K_F01, K_F02, K_F03, K_F04,
	 K_SP, K_F05, K_F06, K_F07, K_F08,
	 K_SP, K_F09, K_F10, K_F11, K_F12, K_NONE };
	if (get_key_list_lines() <= 0) {
		return;
	}
	char buf[MAX_SCRN_LINE_BUF_LEN+1] = "";
	for (int key_idx = 0; func_keys[key_idx] != K_NONE; key_idx++) {
		if (func_keys[key_idx] == K_SP) {
			strcat_printf(buf, MAX_SCRN_LINE_BUF_LEN+1, " ");	// separator
		} else {
			func_key_t *func_key = get_fkey_entry_from_key(NULL, func_keys[key_idx], EF0M);
			if (func_key) {
				strcat_printf(buf, MAX_SCRN_LINE_BUF_LEN+1, "{%s} ", func_key->desc);
			} else {
				strcat_printf(buf, MAX_SCRN_LINE_BUF_LEN+1, "{-----} ");
			}
		}
	}
	display_reverse_text(central_win_get_key_list_line_y() + 0, buf);
}
// display text parenthesized by {} in reverse
// "{Menu}  {RecKM} ..."
PRIVATE void display_reverse_text(int yy, const char *text)
{
	int xx = 0;
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	central_win_clear_lines(yy, -1);
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
			xx += central_win_output_string(yy, xx, buf, -1);
			if (delimiter == '{') {
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
			}
		}
	}
}

PRIVATE void display_key_list_one_line(int yy, const char *text);

void disp_key_list_lines(const char *key_lists[])
{
	for (int line_idx = 1; line_idx < get_key_list_lines(); line_idx++) {
		display_key_list_one_line(central_win_get_key_list_line_y() + line_idx,
		 key_lists[line_idx - 1]);
	}
}
// display text parenthesized by <> in reverse
// "<doe_first_line>TopOfFile ..."
PRIVATE void display_key_list_one_line(int yy, const char *text)
{
	int xx = 0;
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	central_win_clear_lines(yy, -1);
	for (const char *ptr = text; *ptr && xx < central_win_get_columns(); ) {
		char start_chr = 0;
		char end_chr = 0;
		if (*ptr == '<' || *ptr == '>') {
			start_chr = *ptr++;
			end_chr = (start_chr == '<') ? '>' : '<';
		}
		const char *begin;
		for (begin = ptr; *ptr; ptr++) {
			if (*ptr == end_chr)
				break;
		}
		if (ptr > begin) {
			char func_id[MAX_SCRN_LINE_BUF_LEN+1];
			strlcpy__(func_id, begin, ptr - begin);
			if (start_chr == '<') {
				conv_func_id_to_key_names(func_id, 2);
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
			}
			xx += central_win_output_string(yy, xx, func_id, -1);
			if (start_chr == '<') {
				set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
			}
		}
	}
}

void sync_cut_buffers_and_histories(char soon)
{
	sync_cut_buffers_if_necessary();
#ifdef ENABLE_HISTORY
	sync_histories_if_necessary(soon);
#endif // ENABLE_HISTORY
}

// End of winin.c
