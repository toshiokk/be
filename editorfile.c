/**************************************************************************
 *   editorfile.c                                                         *
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

PRIVATE int doe_open_file_recursive(int recursive);
int doe_open_file(void)
{
	return doe_open_file_recursive(RECURSIVE1);
}
PRIVATE int doe_open_file_recursive(int recursive)
{
	char file_path[MAX_PATH_LEN+1];

	clear_files_loaded();
#ifndef ENABLE_FILER
	if (chk_inp_str_ret_val_editor(input_string_pos("", file_path,
	 MAX_PATH_LEN, HISTORY_TYPE_IDX_FILEPOS,
	 _("Open existing file:")))) {
		return 0;
	}
	// CURDIR: changed in editor
	if (load_file_name_upp_low(file_path, TUL0, OOE0, MOE1, LFH0, recursive) < 0) {
		tio_beep();
		return 0;
	}
#else // ENABLE_FILER
	call_filer(1, APP_MODE_NORMAL, "", file_path, file_path, MAX_PATH_LEN);
#endif // ENABLE_FILER
	disp_files_loaded_if_ge_0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_open_new_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", file_path,
	 MAX_PATH_LEN, HISTORY_TYPE_IDX_DIR,
	 _("Open new file:")))) {
		return 0;
	}

	// CURDIR: changed in editor
	if (load_files_in_string(file_path, TUL0, OOE1, MOE0, LFH1, RECURSIVE0) >= 0) {
		disp_files_loaded_if_ge_0();

		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
		return 1;
	}
	if (goto_dir_in_str__call_filer(file_path)) {
		return 1;
	}
	tio_beep();
	return 0;
}

int doe_open_proj_file(void)
{
	do_open_proj_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int doe_open_exec_log_file(void)
{
	do_open_exec_log_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

int do_open_proj_file(void)
{
	int loop;
	DIR *dir;
	struct dirent *dirent;
	struct stat st;
	struct stat lst;
	char file_name[MAX_PATH_LEN+1] = "";

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
	if (load_file_name_upp_low(file_name, TUL0, OOE0, MOE1, LFH0, RECURSIVE1) < 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded_if_ge_0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int do_open_exec_log_file(void)
{
	// CURDIR: changed in editor
	if (load_file_name_upp_low(get_exec_log_file_path(),
	 TUL0, OOE0, MOE1, LFH0, RECURSIVE1) < 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded_if_ge_0();
	doe_last_line();
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
	if (load_file_name_upp_low(file_path, TUL0, OOE0, MOE1, LFH0, RECURSIVE1) < 0) {
		tio_beep();
		return 0;
	}
	goto_str_line_col_in_cur_buf(file_pos_str);
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
	// file was reopened, re-memorize undo state.
	memorize_undo_state_before_change(NULL);
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("File re-loaded"));
	return 1;
}

//-----------------------------------------------------------------------------
//|file saving function    |files|close|unmod|ask  |file-name|Key|
//|                        |     |     |ified| Y/N |         |   |
//|------------------------|-----|-----|-----|-----|---------|---|
//|doe_write_file_to()     | one |no   | Yes |none |New-name |@s |write to new file
//|doe_write_file_ask()    | one |no   | no  |Ask  |cur-name |@w |
//|doe_write_file_always() | one |no   | no  |none |cur-name |@W |write soon if modified
//|doe_write_all_ask()     | All |no   | no  |Ask  |cur-name |@a |
//|doe_write_all_modified()| All |no   | no  |none |cur-name |@A |
//|doe_close_file_ask()    | one |Close| no  |Ask  |cur-name |^Q |
//|doe_close_file_always() | one |Close| no  |none |cur-name |@^Q|write soon if mod. and close
//|doe_close_all_ask()     | All |Close| no  |Ask  |cur-name |@q |
//|doe_close_all_modified()| All |Close| no  |none |cur-name |@Q |write soon if mod. and close

int doe_write_file_to(void)
{
	char file_path[MAX_PATH_LEN+1];

	strlcpy__(file_path, get_epc_buf()->file_path_, MAX_PATH_LEN);
	for ( ; ; ) {
		if (input_new_file_name__ask(file_path) <= 0) {
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
	return 1;
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

PRIVATE int close_file_ask(int yes_no);
int doe_close_file_ask(void)
{
	return close_file_ask(ANSWER_NO);
}
int doe_close_file_always(void)
{
	return close_file_ask(ANSWER_FORCE);
}
PRIVATE int close_file_ask(int yes_no)
{
	int ret = write_file_ask(yes_no, CLOSE_AFTER_SAVE_1);
	if (ret <= ANSWER_CANCEL) {
		// Cancel/Error
		return -1;
	}
	// Yes/No

	free_cur_edit_buf();

	doe_refresh_editor();
	disp_status_bar_done(_("One buffer closed"));
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
	write_close_all(ANSWER_ALL);
	return 0;
}
PRIVATE int write_close_all(int yes_no)
{
	close_all_not_modified();
	if (write_all_ask(yes_no, CLOSE_AFTER_SAVE_1) < 0)
		return -1;
	close_all();
	return 0;
}

//-----------------------------------------------------------------------------
int doe_read_file_into_cur_buf(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);
	doe_open_file_recursive(RECURSIVE0);
	if (get_files_loaded() < 0) {
		return 0;
	}
	doe_select_all_lines();
	doe_copy_text();
	doe_close_file_ask();
	recall_file_pos_null(file_pos_str);
	doe_paste_text_with_pop();
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
	disp_status_bar_done(_("All buffers are checked and saved if modified"));
	return 1;
}
int close_all_not_modified(void)
{
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
	switch_epc_buf_to_top();
	while (free_cur_edit_buf()) {
		// loop
		tio_refresh();
	}
	return 0;
}

// | "yes_no" value   | ask (YES / NO)  | save                            |
// |------------------|-----------------|---------------------------------|
// | ANSWER_FORCE     | not ask         | save soon event if not modified |
// | ANSWER_ALL       | not ask         | save soon if modified           |
// | x < ANSWER_ALL   | ask if modified | save if answered YES            |

int write_file_ask(int yes_no, close_after_save_t close)
{
	int ret = yes_no;

	if (check_cur_buf_modified() == 0) {
		disp_status_bar_done(_("Buffer is NOT modified"));
		return ANSWER_NO;
	}
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(1, 1, 1);
flf_d_printf("ret: %d\n", ret);
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

// End of editorfile.c
