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

PRIVATE int open_file_recursive(int flags);
int doe_open_file_recursive()
{
	return open_file_recursive(RECURS1 | RDOL0 | FOLF0 | LFH0);
}
int doe_open_file_ro()
{
	return open_file_recursive(RECURS1 | RDOL1 | FOLF0 | LFH0);
}
int doe_open_locked_file()
{
	return open_file_recursive(RECURS1 | RDOL0 | FOLF1 | LFH0);
}
PRIVATE int open_file_recursive(int flags)
{
	clear_files_loaded();
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_FILE,
	 _("Open existing file:")))) {
		return 0;
	}
	// CURDIR: changed in editor
	if (load_files_in_string(file_path,
	 TUL0 | OOE0 | MOE1 | RECURS1 | MFPL1 | (flags & (RDOL1 | FOLF1 | LFH0))) < 0) {
		return 0;
	}
	disp_files_loaded_if_ge_0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_open_new_file()
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 _("Open new file:")))) {
		return 0;
	}

	if (load_files_in_string(file_path,
	 TUL0 | OOE1 | MOE0 | RECURS0 | MFPL1 | RDOL0 | FOLF0 | LFH1) >= 0) {
		disp_files_loaded_if_ge_0();
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
		return 1;
	}
#ifdef ENABLE_FILER
	if (try_to_open_dir_in_str_with_filer(file_path)) {
		return 1;
	}
#endif // ENABLE_FILER
	return 0;
}

int doe_open_proj_file()
{
	do_open_proj_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int doe_open_exec_log_file()
{
	do_open_exec_log_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

int do_open_proj_file()
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
	if (load_file_name_upp_low(file_name,
	 TUL0 | OOE0 | MOE1 | RECURS1 | RDOL0 | FOLF0 | LFH0) < 0) {
		return 0;
	}
	disp_files_loaded_if_ge_0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int do_open_exec_log_file()
{
	// CURDIR: changed in editor
	if (load_file_name_upp_low(get_exec_log_file_path(),
	 TUL0 | OOE0 | MOE1 | RECURS1 | RDOL0 | FOLF0 | LFH0) < 0) {
		return 0;
	}
	disp_files_loaded_if_ge_0();
	doe_last_line();
	return 1;
}

int doe_reopen_file_last_line()
{
	if (doe_reopen_file() == 0) {
		return 0;
	}
	doe_last_line();
	return 1;
}
int doe_reopen_file()
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
	int view = is_epc_buf_mode_ro();
	// memorize current file path before closing
	memorize_cur_file_pos_null(file_pos_str);
	free_cur_edit_buf();
	// CURDIR: abs-path is specified
	get_file_line_col_from_str(file_pos_str, file_path, NULL, NULL);
	if (load_file_name_upp_low(file_path,
	 TUL0 | OOE0 | MOE1 | RECURS1 | (view ? RDOL1 : RDOL0) | FOLF0 | LFH0) < 0) {
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

//------------------------------------------------------------------------------
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

PRIVATE int input_new_file_name__ask(char *file_path);

int doe_write_file_to()
{
	char cur_file_path[MAX_PATH_LEN+1];
	char next_file_path[MAX_PATH_LEN+1];

	buf_get_file_path(get_epc_buf(), cur_file_path);
	buf_get_file_path(get_epc_buf(), next_file_path);
	for ( ; ; ) {
		if (input_new_file_name__ask(next_file_path) <= 0) {
			disp_status_bar_warn(_("Writing to specified file cancelled"));
			return -1;
		}
		break;
	}

	if (compare_file_path_in_abs_path(cur_file_path, next_file_path) == 0) {
		// writing to the same file
		if (is_epc_buf_file_locked()) {
			disp_status_bar_err(_("Buffer [%s] is locked"), cur_file_path);
			return -1;
		}
	} else {
		// writing to another file
		if (flock_is_locked(next_file_path)) {
			disp_status_bar_err(_("File [%s] is locked"), next_file_path);
			return -1;
		}
		buf_set_file_path(get_epc_buf(), next_file_path);	// set new file name
	}

	if (backup_and_save_cur_buf_ask() < 0) {
		// error then restore old file path
		buf_set_file_path(get_epc_buf(), cur_file_path);
		return -1;
	}

	if (compare_file_path_in_abs_path(cur_file_path, next_file_path) == 0) {
		// same file name
	} else {
		buf_set_file_path(get_epc_buf(), cur_file_path);
		unlock_epc_buf_if_file_had_locked_by_myself();	// unlock old file name
		buf_set_file_path(get_epc_buf(), next_file_path);
		lock_epc_buf_if_file_already_locked(1);			// lock new file name
	}

	char file_name[MAX_PATH_LEN+1];
	separate_path_to_dir_and_file(next_file_path, next_file_path, file_name);
#ifdef ENABLE_FILER
	// copy new file name to filer next_file
	strlcpy__(get_cur_filer_pane_view()->next_file, file_name, MAX_PATH_LEN);
#endif // ENABLE_FILER
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Written to the file: %s"), file_name);
#ifdef ENABLE_SYNTAX
	set_file_type_and_tab_size_by_cur_file_path();
#endif // ENABLE_SYNTAX
	return 1;
}

// all cases on write_file_to():
//  |writing to the|current buffer state|another file state   |what to do|
//  |--------------|--------------------|---------------------|----------|
//  |current file  |not locked          |--                   |write to it         |
//  |current file  |locked              |--                   |can NOT write to it |
//  |another file  |--                  |has not locked       |unlock prev., write and lock it|
//  |another file  |--                  |has locked by someone|can NOT write to it |
PRIVATE int input_new_file_name__ask(char *file_path)
{
	for ( ; ; ) {
		if (chk_inp_str_ret_val_editor(input_string_pos(file_path, file_path, MAX_PATH_LEN,
		 HISTORY_TYPE_IDX_DIR,
		 _("File Name to Write:")))) {
			return 0;
		}
#ifdef ENABLE_FILER
		if (is_path_wildcard(file_path)) {
			continue;
		}
#endif // ENABLE_FILER
		if (is_path_exist(file_path) == 0) {
			// file of the path is not exist
			break;
		}
		if (is_path_regular_file(file_path) <= 0) {
			// ask non regular file
			int ret = ask_yes_no(ASK_YES_NO,
			 _("Path is not file, can not WRITE it"));
			if (ret < 0) {
				return 0;		// cancelled
			}
			continue;
		}
		// ask overwrite
		int ret = ask_yes_no(ASK_YES_NO,
		 _("File exists, OVERWRITE it ?"));
		if (ret < 0) {
			return 0;		// cancelled
		}
		if (ret == 0) {
			continue;
		}
		break;
	}
	return 1;		// input
}

int doe_write_file_ask()
{
	return write_file_ask(ANSWER_NO, NO_CLOSE_AFTER_SAVE_0);
}
int doe_write_file_always()
{
	return write_file_ask(ANSWER_FORCE, NO_CLOSE_AFTER_SAVE_0);
}
int doe_write_all_ask()
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);
	write_all_ask(ANSWER_NO, NO_CLOSE_AFTER_SAVE_0);
	recall_file_pos_null(file_pos_str);
	return 1;
}
int doe_write_all_modified()
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);
	write_all_ask(ANSWER_ALL, NO_CLOSE_AFTER_SAVE_0);
	recall_file_pos_null(file_pos_str);
	return 1;
}

PRIVATE int close_file_ask(int yes_no);
int doe_close_file_ask()
{
	return close_file_ask(ANSWER_NO);
}
int doe_close_file_always()
{
	return close_file_ask(ANSWER_FORCE);
}
PRIVATE int close_file_ask(int yes_no)
{
	if (is_epc_buf_closeable() == 0) {
		switch_epc_buf_to_edit_buf();
		disp_status_bar_warn(_("Switched from not closeable buffer"));
		return ANSWER_NO;		// not saveable, not closeable
	}
	if (write_file_ask(yes_no, CLOSE_AFTER_SAVE_1) <= ANSWER_CANCEL) {
		// CANCEL/END
		return ANSWER_CANCEL;
	}
	// YES/NO

	free_cur_edit_buf();

	doe_refresh_editor();
	disp_status_bar_done(_("One buffer closed"));
	return ANSWER_YES;
}
PRIVATE int write_close_all(int yes_no);
int doe_close_all_ask()
{
	write_close_all(ANSWER_NO);
	return 0;
}
int doe_close_all_modified()
{
	write_close_all(ANSWER_ALL);
	return 0;
}
PRIVATE int write_close_all(int yes_no)
{
	close_all_not_modified();
	if (write_all_ask(yes_no, CLOSE_AFTER_SAVE_1) < 0)
		return -1;
	close_all_saved();
	return 0;
}

//------------------------------------------------------------------------------
int doe_read_file_into_cur_buf()
{
	char file_pos_str[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str);

	open_file_recursive(RECURS0);
	if (get_files_loaded() < 0) {
		return 0;
	}
	doe_select_all_lines();
	doe_copy_text();
	if (check_cur_buf_modified() == 0) {
		// close a buffer only when the buffer is not modified
		doe_close_file_ask();
	}

	recall_file_pos_null(file_pos_str);
	doe_paste_text_with_pop();
	return 0;
}
//------------------------------------------------------------------------------
int write_all_ask(int yes_no, close_after_save_t close)
{
	switch_epc_buf_to_top_of_edit_buf();
	while (IS_NODE_INT(get_epc_buf())) {
		if (write_file_ask(yes_no, close) <= ANSWER_CANCEL) {
			// CANCEL/END
			return -1;
		}
		// YES/NO
		if (switch_epc_buf_to_next_buf(0, 0) == 0)
			break;
	}
	disp_status_bar_done(_("All modified buffers are saved"));
	return 1;
}
int close_all_not_modified()
{
	switch_epc_buf_to_top_of_edit_buf();
	while (IS_NODE_INT(get_epc_buf())) {
		if (check_cur_buf_modified()) {
			if (switch_epc_buf_to_next_buf(0, 0) == 0) {
				break;
			}
		} else {
			free_cur_edit_buf();
		}
		tio_refresh();
	}
	return 0;
}
int close_all_saved()
{
	switch_epc_buf_to_top_of_edit_buf();
	while (IS_NODE_INT(get_epc_buf())) {
		if (is_epc_buf_closeable() == 0) {
			disp_status_bar_err(_("This buffer is NOT closeable"));
			if (switch_epc_buf_to_next_buf(0, 0) == 0) {
				break;
			}
		} else {
			free_cur_edit_buf();
		}
		tio_refresh();
	}
	return 0;
}

// | "yes_no" value   | ask (YES / NO)  | save                           |
// |------------------|-----------------|--------------------------------|
// | ANSWER_FORCE     | not ask         | save soon even if not modified |
// | ANSWER_ALL       | not ask         | save soon if modified          |
// | x < ANSWER_ALL   | ask if modified | save if answered YES           |

int write_file_ask(int yes_no, close_after_save_t close)
{
	int ret = yes_no;
	if (check_cur_buf_modified() == 0) {
		disp_status_bar_done(_("This buffer is NOT modified"));
		return ANSWER_NONE;		// saveable but not modified
	}
	if (is_epc_buf_saveable() == 0) {
		disp_status_bar_err(_("This buffer is NOT saveable"));
		return ANSWER_NONE;		// not saveable
	}

	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(1, 1);
	if (ret < ANSWER_ALL) {
		ret = ask_yes_no(ASK_YES_NO | ASK_ALL,
		 close == 0
		  ? _("Save modified buffer ?")
		  : _("Save modified buffer (ANSWERING \"No\" WILL DISCARD CHANGES) ?"));
		if (ret < 0) {
			disp_status_bar_warn(_("Saving modified buffer cancelled"));
			return ANSWER_CANCEL;
		}
	}
	if (ret >= ANSWER_YES) {	// Yes
		if (backup_and_save_cur_buf_ask() < 0) {
			return ANSWER_CANCEL;
		}
		disp_status_bar_done(_("Buffer saved to file"));
	}
	return ret;		// ANSWER_ALL/ANSWER_YES/ANSWER_NO
}
//------------------------------------------------------------------------------
// The word 'lock' has two meanings:
// - file lock:   file has been locked by someone
// - buffer lock: buffer contents was loaded from a locked file
//                and the buffer was locked from modification
void lock_epc_buf_if_file_already_locked(BOOL lock_buffer_if_already_locked)
{
	SET_CUR_EBUF_STATE(buf_LOCKED, 0);
	if (flock_lock(buf_get_abs_path(get_epc_buf(), NULL)) == 0) {
		// file has successfully locked: this is the 1st load
	} else {
		// already file has been locked: lock this buffer
		if (lock_buffer_if_already_locked) {
			SET_CUR_EBUF_STATE(buf_LOCKED, 1);
		}
	}
}
void unlock_epc_buf_if_file_had_locked_by_myself()
{
	if (is_epc_buf_file_locked() == 0) {
		// this buffer has NOT been locked:
		// - this file must had been locked by myself
		// - unlock by myself
		if (flock_unlock(buf_get_abs_path(get_epc_buf(), NULL)) == 0) {
			// successfully unlocked
		} else {
			// already unlocked by another instance: ERROR
			progerr_printf("already unlocked [%s]\n", buf_get_abs_path(get_epc_buf(), NULL));
		}
	} else {
		// this file has been locked by another instance:
		// - nothing to do
	}
}

// End of editorfile.c
