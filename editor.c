/**************************************************************************
 *   editor.c                                                             *
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

editor_quit_t editor_quit = EDITOR_NONE;
PRIVATE int editor_main_loop(void);

int call_editor(int push_win, int list_mode)
{
#ifdef ENABLE_FILER
	filer_panes_t *prev_fps = NULL;
	filer_panes_t next_filer_panes;
#endif // ENABLE_FILER
	app_mode_t appmode_save;

	if (push_win) {
		win_push_win_size();
	}
#ifdef ENABLE_FILER
	prev_fps = inherit_filer_panes(&next_filer_panes);
#endif // ENABLE_FILER

	memcpy(&appmode_save, &app_mode__, sizeof(app_mode__));
	CLR_APPMD(app_EDITOR_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);
	SET_APPMD_VAL(ed_EDITOR_PANES, 0);
	set_app_func_key_table();
	set_work_space_color_on_app_list_mode();

////_D_(dump_cur_pointers())
flf_d_printf("push_win:%d, list_mode:%d\n", push_win, list_mode);
flf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");
	int ret = editor_main_loop();
flf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
flf_d_printf("ret: %d\n", ret);
	editor_quit = EDITOR_NONE;	// for caller of call_editor(), clear "editor_quit"
	_mlc_check_count
////_D_(dump_cur_pointers())

	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));
	SET_APPMD_VAL(app_LIST_MODE, GET_APPMD_PTR(&appmode_save, app_LIST_MODE));
	SET_APPMD_VAL(ed_EDITOR_PANES, GET_APPMD_PTR(&appmode_save, ed_EDITOR_PANES));
	set_app_func_key_table();
	set_work_space_color_on_app_list_mode();

#ifdef ENABLE_FILER
	free_filer_panes(&next_filer_panes, prev_fps);
#endif // ENABLE_FILER
	if (push_win) {
		win_pop_win_size();
	}

	return ret;		// 1: input, 0: cancelled
}

//-----------------------------------------------------------------------------
int prev_key_executed;
int key_executed;
char prev_func_id[MAX_PATH_LEN+1];
PRIVATE int editor_main_loop(void)
{
	key_code_t key_input;
	func_key_table_t *func_key_table;

	prev_key_executed = 0;
	key_executed = 0;

	search_clear(&search__);
#ifdef ENABLE_REGEX
	matches_clear(&matches__);
#endif // ENABLE_REGEX

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL_SOON);

	// Main input loop
	key_input = 0;
	while (1) {
		editor_quit = EDITOR_NONE;
		if (key_macro_is_playing_back()) {
			// When playing back key-macro, do not update screen for speed up.
		} else {
			set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
			update_screen_editor(1, key_input >= 0, 1);
		}
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		if (key_input < 0) {
			// no key input
		} else {
mflf_d_printf("input%ckey:0x%04x(%s)=======================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));

#ifdef ENABLE_REGEX
			matches_clear(&matches__);
#endif // ENABLE_REGEX
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
			memorize_undo_state_before_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
			if ((func_key_table = get_func_key_table_from_key(editor_func_key_table, key_input))
			 == NULL) {
				if (IS_CHAR_KEY(key_input) == 0) {
					disp_status_bar_err(_("No command assigned for the key: %04xh"), key_input);
				} else {
					doe_enter_char(key_input);
				}
			} else {
				doe_enter_char_send();
				switch (func_key_table->list_mode) {
				default:
				case XL:		// not executable in List mode
					if (is_app_list_mode()) {
						disp_status_bar_done(
						 _("Can not execute this function: [%s]"), func_key_table->func_id);
						break;
					}
					// FALLTHROUGH
				case XA:		// executable all Normal/List mode
					search_clear(&search__);
					key_executed = key_input;
flf_d_printf("CALL_EDITOR_FUNC [%s]\n", func_key_table->func_id);
					//=========================
					int ret = (*func_key_table->func)();	// call function "doe_...()"
					//=========================
					count_easy_buffer_switching();
flf_d_printf("ret_val: %d, editor_quit: %d\n", ret, editor_quit);
					prev_key_executed = key_executed;
					strlcpy__(prev_func_id, func_key_table->func_id, MAX_PATH_LEN);
					break;
				}
			}
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
			check_undo_state_after_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
		}
		if (is_app_list_mode() == 0 && count_edit_bufs() == 0) {
			// If all files closed on edit mode, exit editor.
			break;
		}
////_D_(dump_cur_pointers())
		if (editor_quit) {
			break;
		}
	}
#ifdef ENABLE_HISTORY
	key_macro_cancel_recording();
#endif // ENABLE_HISTORY
////_D_(dump_cur_pointers())
	if (editor_quit == EDITOR_CANCELLED) {
		return -1;	// cancelled
	} else
	if (editor_quit == EDITOR_DONE) {
		return 0;	// done
	}
	return IS_META_KEY(key_input) ? 2 : 1;	// selected
}

//-----------------------------------------------------------------------------
PRIVATE int open_file_recursive(int recursive);
int doe_open_file(void)
{
	return open_file_recursive(RECURSIVE1);
}
PRIVATE int open_file_recursive(int recursive)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	clear_files_loaded();
	while (1) {
#ifdef ENABLE_FILER
		ret = call_filer(1, 0, "", file_path, file_path, MAX_PATH_LEN);
		if (ret > 0)
			break;
#endif // ENABLE_FILER

		ret = input_string_tail("", file_path, HISTORY_TYPE_IDX_CURSPOS, _("Open existing file:"));

		if (ret <= 0) {
			break;
		}
#ifdef ENABLE_FILER
		if (is_path_wildcard(file_path))
			continue;
#endif // ENABLE_FILER
		// CURDIR: changed in editor
		if (load_file_name_upp_low(file_path, TUL0, OOE0, MOE1, LFH0, recursive) <= 0) {
			tio_beep();
		}
		break;
	}
	disp_files_loaded_ifnon0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_open_new_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string_tail("", file_path, HISTORY_TYPE_IDX_DIR, _("Open new file:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	clear_files_loaded();
	// CURDIR: changed in editor
	if (load_file_name_upp_low(file_path, TUL0, OOE1, MOE0, LFH0, RECURSIVE0) <= 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded_ifnon0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

int doe_open_proj_file(void)
{
	char filter[MAX_PATH_LEN+1];
	int loop;
	DIR *dir;
	struct dirent *dirent;
	struct stat st;
	struct stat lst;
	char file_name[MAX_PATH_LEN+1];

	strcpy__(file_name, "");
	dir = opendir(".");
	for (loop = 0; loop < 2; loop++) {
		rewinddir(dir);
		while ((dirent = readdir(dir)) != NULL) {
			lstat(dirent->d_name, &lst);
			if (S_ISLNK(lst.st_mode))
				stat(dirent->d_name, &st);
			else
				memcpy__(&st, &lst, sizeof(struct stat));
			if (S_ISREG(st.st_mode) && is_file_name_proj_file(dirent->d_name, 1+loop)) {
				strlcpy__(file_name, dirent->d_name, MAX_PATH_LEN);
				break;
			}
		}
		if (is_strlen_not_0(file_name)) {
			// file matched
			break;
		}
	}
	closedir(dir);

	if (is_strlen_0(file_name)) {
		disp_status_bar_err(_("No project file found"));
		return 0;
	}

	// CURDIR: changed in editor
	if (load_file_name_upp_low(file_name, TUL0, OOE0, MOE1, LFH0, RECURSIVE1) <= 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

int doe_reopen_file_last_line(void)
{
	if (doe_reopen_file() == 0) {
		return 0;
	}
	doe_last_line();
	return 1;
}
int doe_reopen_file(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	char file_path[MAX_PATH_LEN+1];
	int ret;

	if (check_cur_buf_modified()) {
		ret = ask_yes_no(ASK_YES_NO,
		 _("Buffer is modified, Are you OK to reopen file ?"));
		if (ret <= 0)
			return 0;
	}
	// memorize current file path before closing
	memorize_cur_file_pos_null(file_pos_str);
	free_cur_edit_buf();
	// CURDIR: abs-path is specified
	get_file_line_col_from_str_null(file_pos_str, file_path, NULL, NULL);
	if (load_file_name_upp_low(file_path, TUL0, OOE0, MOE1, LFH0, RECURSIVE1) <= 0) {
		tio_beep();
		return 0;
	}
	goto_str_line_col_in_cur_buf(NULL);
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
	// file was reopened, re-memorize undo state.
	memorize_undo_state_before_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("File re-loaded"));
	return 1;
}

//-----------------------------------------------------------------------------
//|Func name               |files|close|un-mod|ask Y/N|file-name|Key  |
//|                        |     |     | ified|       |         |     |
//|------------------------|-----|-----|------|-------|---------|-----|
//|doe_write_file_to()     | one |no   | Yes  | none  |New-name |@s   |write to new file
//|doe_write_file_ask()    | one |no   | no   | Ask   |cur-name |@w   |
//|doe_write_file_always() | one |no   | no   | none  |cur-name |@W   |
//|doe_write_all_ask()     | All |no   | no   | Ask   |cur-name |@a   |
//|doe_write_all_modified()| All |no   | no   | none  |cur-name |@A   |
//|doe_close_file_ask()    | one |Close| no   | Ask   |cur-name |^Q   |
//|doe_close_file_always() | one |Close| no   | none  |cur-name |(@^Q)|
//|doe_close_all_ask()     | All |Close| no   | Ask   |cur-name |@q   |
//|doe_close_all_modified()| All |Close| no   | none  |cur-name |@Q   |

int doe_write_file_to(void)
{
	char file_path[MAX_PATH_LEN+1];

	strlcpy__(file_path, get_epc_buf()->file_path, MAX_PATH_LEN);
	while (1) {
		if (input_new_file_name_n_ask(file_path) <= 0) {
			return -1;
		}
#ifdef ENABLE_FILER
		if (is_path_wildcard(file_path))
			continue;
#endif // ENABLE_FILER
		char org_file_path[MAX_PATH_LEN+1];
		buf_get_file_path(get_epc_buf(), org_file_path);
		buf_set_file_abs_path(get_epc_buf(), file_path);	// set new file name
		if (backup_and_save_cur_buf(file_path) < 0) {
			buf_set_file_abs_path(get_epc_buf(), org_file_path);
			return -1;
		}
		break;
	}
	char file_name[MAX_PATH_LEN+1];
	separate_path_to_dir_and_file(file_path, file_path, file_name);
#ifdef ENABLE_FILER
	// copy new file name to filer next_file
	strlcpy__(get_cur_filer_view()->next_file, file_name, MAX_PATH_LEN);
#endif // ENABLE_FILER
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Written to the file: %s"), file_name);
	return 0;
}
int doe_write_file_ask(void)
{
	return write_file_ask(ANSWER_NO, NO_CLOSE_AFTER_SAVE_0);
}
int doe_write_file_always(void)
{
	return write_file_ask(ANSWER_FORCE, NO_CLOSE_AFTER_SAVE_0);
}
int doe_write_all_ask(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);
	write_all_ask(ANSWER_NO, NO_CLOSE_AFTER_SAVE_0);
	recall_file_pos_null(file_pos_str);
	return 1;
}
int doe_write_all_modified(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);
	write_all_ask(ANSWER_ALL, NO_CLOSE_AFTER_SAVE_0);
	recall_file_pos_null(file_pos_str);
	return 1;
}

PRIVATE int close_file(int yes_no);
int doe_close_file_ask(void)
{
	return close_file(ANSWER_NO);
}
int doe_close_file_always(void)
{
	return close_file(ANSWER_FORCE);
}
PRIVATE int close_file(int yes_no)
{
	char file_pos_str[MAX_PATH_LEN+1];

	if (is_app_list_mode()) {
		editor_quit = EDITOR_CANCELLED;
		return 0;
	}
	int ret = write_file_ask(yes_no, CLOSE_AFTER_SAVE_1);
	if (ret <= ANSWER_CANCEL) {
		// Cancel/Error
		return -1;
	}
	// Yes/No

	// If the current file has been the last open file, memorize it later.
	memorize_cur_file_pos_null(file_pos_str);
	free_cur_edit_buf();

	doe_refresh_editor();
	disp_status_bar_done(_("One buffer closed"));
#ifdef ENABLE_HISTORY
	if (count_edit_bufs() == 0) {
		// This file is the last open file.
		// Memorize the last open file's cursor pos.
		update_history(HISTORY_TYPE_IDX_CURSPOS, file_pos_str, 1);
	}
#endif // ENABLE_HISTORY
	return 2;
}
PRIVATE int write_close_all(int yes_no);
int doe_close_all_ask(void)
{
	write_close_all(ANSWER_NO);
	return 0;
}
int doe_close_all_modified(void)
{
	write_close_all(ANSWER_YES);
	return 0;
}
PRIVATE int write_close_all(int yes_no)
{
	char file_pos_str[MAX_PATH_LEN+1];
	if (is_app_list_mode()) {
		editor_quit = EDITOR_CANCELLED;
		return 0;
	}
	// memorize the last current file.
	memorize_cur_file_pos_null(file_pos_str);

	close_all_not_modified();
	if (write_all_ask(yes_no, CLOSE_AFTER_SAVE_1) < 0)
		return -1;
	close_all();

#ifdef ENABLE_HISTORY
	// memorize the last current file's cursor pos.
	update_history(HISTORY_TYPE_IDX_CURSPOS, file_pos_str, 1);
#endif // ENABLE_HISTORY
	return 0;
}

//-----------------------------------------------------------------------------
int doe_read_file_into_cur_pos(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);
	open_file_recursive(RECURSIVE0);
	if (get_files_loaded() <= 0) {
		return 0;
	}
	doe_select_all_lines();
	doe_copy_text();
	doe_close_file_ask();
	recall_file_pos_null(file_pos_str);
	doe_paste_text_with_pop();
	return 0;
}

#define _CLIPBOARD_FILE_NAME	"clipboard"		// default clipboard file name
#if defined(APP_DIR)
#define CLIPBOARD_FILE_NAME		_CLIPBOARD_FILE_NAME
#else // APP_DIR
#define CLIPBOARD_FILE_NAME		"." _CLIPBOARD_FILE_NAME
#endif // APP_DIR

// clipboard file is common to all be-editor instances in one user
const char *get_clipboard_file_path()
{
	static char file_path[MAX_PATH_LEN+1];

	snprintf_(file_path, MAX_PATH_LEN+1, "%s/%s", get_app_dir(), CLIPBOARD_FILE_NAME);
	return file_path;
}
int save_cut_buf_to_clipboard_file()
{
	return save_buf_to_file(CUT_BUFS_TOP_BUF, get_clipboard_file_path());
}
int load_clipboard_into_cut_buf()
{
	return load_file_into_buf(CUT_BUFS_TOP_BUF, get_clipboard_file_path());
}

int doe_read_clipboard_into_cur_char()
{
	return do_read_clipboard_into_cur_pos(0);
}
int doe_read_clipboard_into_cur_line()
{
	return do_read_clipboard_into_cur_pos(1);
}
int do_read_clipboard_into_cur_pos(int char0_line1)
{
	push_cut_buf();
	if (load_clipboard_into_cut_buf() <= 0) {
		pop_n_free_from_cut_buf();
		return 0;
	}
	if (char0_line1 == 0) {
		// set character cut ==> character paste
		SET_CUR_CBUF_STATE(buf_CUT_MODE, CUT_MODE_H_CHAR);
	} else {
		// set line cut ==> line paste
		SET_CUR_CBUF_STATE(buf_CUT_MODE, CUT_MODE_N_LINE);
	}
	doe_paste_text_with_pop();
	return 0;
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_FILER
int doe_run_line_soon(void)
{
	char buffer[MAX_PATH_LEN+1];

	// EPCBVC_CL->data may be in history buffer and freed in calling update_history().
	// So copy to local buffer.
	strlcpy__(buffer, EPCBVC_CL->data, MAX_PATH_LEN);

	fork_exec_once_sh_c(SEPARATE1, PAUSE1, buffer);

	if (is_app_list_mode()) {
		editor_quit = EDITOR_DONE;
		return 0;
	}
	doe_refresh_editor();
	return 0;
}
int doe_call_filer(void)
{
	char file_path[MAX_PATH_LEN+1];
	call_filer(1, 0, "", "", file_path, MAX_PATH_LEN);
	return 0;
}
#endif // ENABLE_FILER

//-----------------------------------------------------------------------------
#ifdef ENABLE_HELP
int doe_editor_splash(void)
{
	disp_splash(100);
	input_key_loop();
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	return 0;
}
#endif // ENABLE_HELP

int doe_display_color_pairs(void)
{
	display_color_pairs(0, 0);
	input_key_loop();
#ifdef ENABLE_DEBUG
	display_item_colors(0, 0);
#ifdef ENABLE_REGEX
	display_bracket_hl_colors(0, 40);
#endif // ENABLE_REGEX
	input_key_loop();
#endif // ENABLE_DEBUG
	return 0;
}

int doe_editor_menu_0(void)
{
	editor_menu_n(-1);
	return 0;
}
int doe_editor_menu_1(void)
{
	editor_menu_n(0);
	return 0;
}
int doe_editor_menu_2(void)
{
	editor_menu_n(1);
	return 0;
}
int doe_editor_menu_3(void)
{
	editor_menu_n(2);
	return 0;
}
int doe_editor_menu_4(void)
{
	editor_menu_n(3);
	return 0;
}
int doe_editor_menu_5(void)
{
	editor_menu_n(4);
	return 0;
}
int doe_editor_menu_6(void)
{
	editor_menu_n(5);
	return 0;
}
int doe_editor_menu_7(void)
{
	editor_menu_n(6);
	return 0;
}
int doe_editor_menu_8(void)
{
	editor_menu_n(7);
	return 0;
}
int doe_editor_menu_9(void)
{
	editor_menu_n(8);
	return 0;
}

//-----------------------------------------------------------------------------
int write_all_ask(int yes_no, close_after_save_t close)
{
	switch_epc_buf_to_top();
	while (is_epc_buf_valid()) {
		int ret = write_file_ask(yes_no, close);
		if (ret <= ANSWER_CANCEL) {
			disp_status_bar_done(_("Cancelled"));
			return -1;
		}
		if (switch_epc_buf_to_next(0, 0) == 0)
			break;
	}
	disp_status_bar_done(_("All buffers are checked(saved if modified)"));
	return 1;
}
int close_all_not_modified(void)
{
	disp_status_bar_ing(_("Freeing buffers..."));
	switch_epc_buf_to_top();
	while (is_epc_buf_valid()) {
		if (check_cur_buf_modified()) {
			if (switch_epc_buf_to_next(0, 0) == 0)
				break;
		} else {
			free_cur_edit_buf();
		}
		tio_refresh();
	}
	return 0;
}
int close_all(void)
{
	disp_status_bar_ing(_("Freeing all buffers..."));
	switch_epc_buf_to_top();
	while (free_cur_edit_buf()) {
		// loop
		tio_refresh();
	}
	return 0;
}

// | "yes_no" value | ask yes / no          | save or not           |
// |----------------|-----------------------|-----------------------|
// | ANSWER_FORCE   | save soon if modified | save soon if modified |
// | < ANSWER_FORCE | ask if not modified   | save if answered YES  |
// | ANSWER_ALL     | save soon if modified | save soon if modified |
// | < ANSWER_ALL   | ask if modified       | save if answered YES  |

int write_file_ask(int yes_no, close_after_save_t close)
{
	int ret = yes_no;

	switch_epc_buf_to_valid_buf();

	/////if (yes_no < ANSWER_FORCE && check_cur_buf_modified() == 0) {
	if (check_cur_buf_modified() == 0) {
		disp_status_bar_done(_("Buffer is NOT modified"));
		return ANSWER_NO;
	}
	/////if (yes_no == ANSWER_FORCE && check_cur_buf_modified() == 0) {
	/////	ret = ask_yes_no(ASK_YES_NO | ASK_ALL, _("Save unmodified buffer ?"));
	/////	if (ret <= 0) {
	/////		disp_status_bar_done(_("Cancelled"));
	/////		return ANSWER_CANCEL;
	/////	}
	/////	ret = ANSWER_FORCE;
	/////}
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(1, 1, 1);
	if (ret < ANSWER_ALL) {
		ret = ask_yes_no(ASK_YES_NO | ASK_ALL,
		 close == 0
		  ? _("Save modified buffer ?")
		  : _("Save modified buffer (ANSWERING \"No\" WILL DISCARD CHANGES) ?"));
		if (ret < 0) {
			disp_status_bar_done(_("Cancelled"));
			return ANSWER_CANCEL;
		}
	}
	if (ret >= ANSWER_YES) {	// Yes
		if (backup_and_save_cur_buf_ask() < 0) {
			return ANSWER_CANCEL;
		}
		disp_status_bar_done(_("Buffer saved to file"));
	}
	return ret;		// all:2, yes:1
}

int doe_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
void win_push_win_size(void)
{
	// draw parent screen reversed
	update_screen_app(1, 1, 1);

	inc_win_depth();
	win_reinit_win_size();
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_screen();		// draw dark frame
	inc_win_depth();
	win_reinit_win_size();
}
void win_pop_win_size(void)
{
	dec_win_depth();
	dec_win_depth();
	win_reinit_win_size();

	// draw parent screen
	update_screen_app(1, 1, 1);
}

int update_screen_app(int title_bar, int status_bar, int refresh)
{
#ifndef ENABLE_FILER
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(title_bar, status_bar, refresh);
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
		update_screen_editor(title_bar, status_bar, refresh);
	} else {
		update_screen_filer(title_bar, status_bar, refresh);
	}
#endif // ENABLE_FILER
	return 0;
}

//-----------------------------------------------------------------------------
int update_screen_editor(int title_bar, int status_bar, int refresh)
{
	int pane_sel_idx;		// 0: not current pane, 1: current pane
	int cur_pane_idx;		// pane index
	int pane_idx;			// pane index

	EPCBVC_CURSOR_Y = MIN(edit_win_get_text_lines()-1, EPCBVC_CURSOR_Y);
////mflf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{\n");
	// title bar
	if (title_bar) {
		editor_disp_title_bar();
	}

	if (get_edit_win_update_needed()) {
		if (GET_APPMD(ed_EDITOR_PANES) == 0) {		// 1 pane
			win_select_win(WIN_IDX_SUB_WHOLE);
			disp_edit_win(1);
		} else {									// 2 panes
			cur_pane_idx = get_editor_cur_pane_idx();
			for (pane_sel_idx = 0; pane_sel_idx < EDITOR_PANES; pane_sel_idx++) {
				// 1st, update not current pane.
				// 2nd, update current pane.
				if (pane_sel_idx == 0) {
					// not current pane
					pane_idx = 1 - cur_pane_idx;	// 0 ==> 1, 1 ==> 0
				} else {
					// current pane
					pane_idx = cur_pane_idx;
				}
				win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
				if (pane_sel_idx == 0) {
					set_work_space_color_dark();
				}
////flf_d_printf("pane_sel_idx: %d, pane_idx: %d\n", pane_sel_idx, pane_idx);
				set_editor_cur_pane_idx(pane_idx);
				disp_edit_win(pane_sel_idx);
				if (pane_sel_idx == 0) {
					set_work_space_color_normal();
				}
			}
		}
	}

	// status bar
	if (status_bar) {
		disp_status_bar_editor();
		// key list
		disp_key_list_editor();
	}

	set_edit_cursor_pos();

	if (refresh) {
		tio_refresh();
	}
	clear_edit_win_update_needed();
////mflf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}\n");
	return 0;
}

int disp_status_bar_editor(void)
{
	int bytes, byte_idx;
#define UTF8_CODE_LEN		(17+1+8+1)		// "00-00-00-00-00-00(U+xxxxxx)"
	char buf_char_code[UTF8_CODE_LEN+1];	// "00-00-00-00-00-00(U+xxxxxx)"
	unsigned long xx;
	unsigned long disp_len;
#define SEL_LINES_LEN		(1+4+10+1)		// " LNS:9999999999"
	char buf_lines_sel[SEL_LINES_LEN] = "";
	char buffer[MAX_EDIT_LINE_LEN+1];

	xx = col_idx_from_byte_idx(EPCBVC_CL->data, 0, EPCBVC_CLBI) + 1;
	disp_len = col_idx_from_byte_idx(EPCBVC_CL->data, 0, MAX_EDIT_LINE_LEN) + 1;

	strcpy__(buf_char_code, "");
	bytes = utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
	for (byte_idx = 0; byte_idx < bytes; byte_idx++) {
		snprintf(&buf_char_code[strnlen(buf_char_code, UTF8_CODE_LEN)], 3+1,
		 byte_idx == 0 ? "%02x" : "-%02x",
		 (unsigned char)EPCBVC_CL->data[EPCBVC_CLBI + byte_idx]);
	}
#ifdef ENABLE_UTF8
	// show Unicode
	if (bytes >= 2) {
		snprintf(&buf_char_code[strnlen(buf_char_code, UTF8_CODE_LEN)], 8+1, "(U+%04x)",
		 (unsigned int)utf8c_decode(&EPCBVC_CL->data[EPCBVC_CLBI]));
	}
#endif // ENABLE_UTF8

	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE))) {
		snprintf(buf_lines_sel, SEL_LINES_LEN, " LNS:%2d", lines_selected());
	}

	strcpy__(buffer, "");
	strlcat__(buffer, MAX_EDIT_LINE_LEN,
	 _("LINE:%4lu/%-4lu COLUMN:%3lu/%-3lu SIZE:%6lu%s CODE:%s ENC:%s EOL:%s"));
	disp_status_bar_percent_editor(
	 buffer, EPCBVC_CL->line_num, get_epc_buf()->buf_lines, xx, disp_len,
	 get_epc_buf()->buf_size, buf_lines_sel, buf_char_code,
	 buf_encode_str(get_epc_buf()), buf_eol_str(get_epc_buf()));
	return 1;
}

void disp_key_list_editor(void)
{
	char *editor_key_lists[] = {
 "{Menu} "
 " {Rec  } {Play } {SchBW} {SchFW} "
 " {Mark } {Cut  } {Copy } {Pop  } "
 " {Paste} {Dupli} {Files} {TagJp}",
 "<doe_close_file_ask>Quit "
 "<doe_cut_to_head>CutToHead "
 "<doe_cut_text>CutLine "
 "<doe_cut_to_tail>CutToTail "
 "<doe_copy_text>CopyLine "
 "<doe_paste_text_with_pop>PasteWPop "
 "<doe_paste_text_without_pop>PasteWoPop "
 "<doe_duplicate_text>DupLine "
 "<doe_first_line>TopOfFile "
 "<doe_last_line>BotOfFile "
 "<doe_prev_word>PrevWord "
 "<doe_next_word>NextWord ",

 "<doe_close_all_ask>CloseAll "
 "<doe_open_file>OpenFile "
 "<doe_write_file_ask>WriteFile "
 "<doe_search_backward_first>Search BW "
 "<doe_search_forward_first>Search FW "
 "<doe_replace>Replace "
 "<doe_switch_to_file_list>FileList "
 "<doe_switch_to_prev_file>PrevFile "
 "<doe_switch_to_next_file>NextFile "
#if APP_REL_LVL == APP_REL_LVL_EXPERIMENTAL
 "<doe_switch_to_prev_buffers>PrevBufs "
 "<doe_switch_to_next_buffers>NextBufs "
#endif // APP_REL_LVL
 "<doe_switch_to_key_list>KeyList "
 "<doe_switch_to_func_list>FuncList ",
	};
	disp_key_list(editor_key_lists);
}

//-----------------------------------------------------------------------------

int is_view_mode_then_warn_it(void)
{
	if (is_app_list_mode()) {
		disp_status_bar_done(_("Modification not allowed in LIST mode"));
		return 1;
	}
	if (IS_NODE_ANCH(get_epc_buf())) {
		disp_status_bar_done(_("Modification not allowed in Anchor buffer"));
		return 1;
	}
	if (CUR_EBUF_STATE(buf_VIEW_MODE)) {
		disp_status_bar_done(_("Modification not allowed in VIEW mode"));
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_cur_pointers(void)
{
	flf_d_printf("epc_buf:[%s]\n", get_epc_buf()->file_path);
	flf_d_printf("%d:[%s]\n", EPCBVC_CL->line_num, EPCBVC_CL->data);
	flf_d_printf("EPCBVC_CLBI:%d\n", EPCBVC_CLBI);
	flf_d_printf("cursor_y:%d\n", EPCBVC_CURSOR_Y);
	flf_d_printf("cursor_x_to_keep:%d\n", EPCBVC_CURSOR_X_TO_KEEP);
	flf_d_printf("min_text_x_to_keep:%d\n", EPCBVC_MIN_TEXT_X_TO_KEEP);
}
#endif // ENABLE_DEBUG

// End of editor.c
