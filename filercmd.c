/**************************************************************************
 *   filercmd.c                                                           *
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

#ifdef ENABLE_FILER

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"
#define BEUNZIP		"beunzip"
#define BEZIP		"bezip"

int chk_inp_str_ret_val_filer(int ret)
{
	filer_do_next = ret;
	if (filer_do_next <= EF_QUIT) {
		// EF_CANCELLED, EF_QUIT ==> EF_NONE so that filer does not quit
		filer_do_next = EF_NONE;
	}
	return !IS_EF_INPUT_XX(ret);
}

int dof_up()
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() - 1,
	 get_cur_filer_pane_view()->file_infos_entries-1));
	return 1;
}
int dof_down()
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() + 1,
	 get_cur_filer_pane_view()->file_infos_entries-1));
	return 1;
}
int dof_page_up()
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() - filer_vert_scroll_lines(),
	 get_cur_filer_pane_view()->file_infos_entries-1));
	return 1;
}
int dof_page_down()
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() + filer_vert_scroll_lines(),
	 get_cur_filer_pane_view()->file_infos_entries-1));
	return 1;
}
int dof_top_of_list()
{
	set_cur_fv_file_idx(0);
	return 1;
}
int dof_bottom_of_list()
{
	set_cur_fv_file_idx(get_cur_filer_pane_view()->file_infos_entries-1);
	return 1;
}

int dof_refresh_filer()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	disp_status_bar_done(_("File view refreshed"));
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 1;
}
int dof_tap_file()
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	if (is_app_chooser_mode()) {
		if (get_file_type_num(get_cur_fv_cur_file_ptr()) >= 80) {
			filer_do_next = FL_ENTER_FILE_NAME_OR_PATH;
			return 1;
		}
	}
	return dof_view_file();
}
int dof_view_file()
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	if (S_ISREG(get_cur_fv_cur_file_ptr()->st.st_mode)) {
		const char *file_name = get_cur_fv_cur_file_ptr()->file_name;
		if (fork_exec_args_once(EX_FLAGS_0, BEPAGER, file_name, 0)) {
			if (fork_exec_args_once(EX_FLAGS_0, "less", file_name, 0)) {
				fork_exec_args_once(EX_FLAGS_0, "more", file_name, 0);
			}
		}
	}
	return 0;
}
int dof_tail_file()	// view file with "tail" command
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	if (S_ISREG(get_cur_fv_cur_file_ptr()->st.st_mode)) {
		const char *file_name = get_cur_fv_cur_file_ptr()->file_name;
		fork_exec_args_once(EX_FLAGS_0, BETAIL, file_name, 0);
	}
	return 0;
}

PRIVATE int dof_open_file_(int flags);

int dof_open_file_recursive()
{
	dof_open_file_(RECURS1 | RDOL0 | FOLF0 | LFH0);
	return 1;
}
int dof_open_file_ro()
{
	dof_open_file_(RECURS1 | RDOL1 | FOLF0 | LFH0);
	return 1;
}
int dof_open_locked_file()
{
	dof_open_file_(RECURS1 | RDOL0 | FOLF1 | LFH0);
	return 1;
}
int dof_open_file_non_recursive()
{
	dof_open_file_(RECURS0 | RDOL0 | FOLF0 | LFH0);
	return 1;
}
int dof_open_file_from_history()
{
	dof_open_file_(RECURS1 | RDOL0 | FOLF0 | LFH1);
	return 1;
}

PRIVATE int dof_open_file_(int flags)
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}

#ifdef ENABLE_HISTORY
	int prev_count_edit_bufs = count_edit_bufs();
#endif // ENABLE_HISTORY

	clear_files_loaded();
	begin_check_break_key();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (S_ISREG(get_cur_fv_file_ptr(file_idx)->st.st_mode)) {
			if (load_file_name_upp_low_(get_cur_fv_file_ptr(file_idx)->file_name,
			 TUL0 | OOE0 | MOE1 | (flags & (RECURS1 | RDOL1 | FOLF1 | LFH1))) <= 0) {
				tio_beep();
			}
		}
		if (check_break_key()) {
			break;
		}
	}
	end_check_break_key();

#ifdef ENABLE_HISTORY
	if ((prev_count_edit_bufs == 0) && (get_files_loaded() >= 0)) {
		goto_last_file_line_col_in_history();
	}
#endif // ENABLE_HISTORY

	disp_files_loaded_if_ge_0();

	unselect_all_files_auto(_FILE_SEL_MAN_ | _FILE_SEL_AUTO_);
	if (get_files_loaded() < 0) {
		filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	} else {
		filer_do_next = EF_LOADED;
	}
	return 0;
}

//------------------------------------------------------------------------------

int dof_open_proj_file()
{
	do_open_proj_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	if (get_files_loaded() >= 0) {
		filer_do_next = EF_LOADED;
	}
	clear_files_loaded();
	return 1;
}
int dof_open_exec_log_file()
{
	do_open_exec_log_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	if (get_files_loaded() >= 0) {
		filer_do_next = EF_LOADED;
	}
	clear_files_loaded();
	return 1;
}

PRIVATE int dof_open_new_file_(const char *str);
int dof_open_new_file()
{
	char file_path[MAX_PATH_LEN+1] = "";
	concat_file_path_separating_by_space(file_path, MAX_PATH_LEN,
	 get_cur_fv_cur_file_ptr()->file_name);
	return dof_open_new_file_(file_path);
}
PRIVATE int dof_open_new_file_(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos(str, file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_FILE,
	 _("Open new file:")))) {
		return 0;
	}

	if (load_files_in_string(file_path,
	 TUL0 | OOE1 | MOE0 | RECURS0 | RDOL0 | FOLF0 | LFH1) >= 0) {
		disp_files_loaded_if_ge_0();
		filer_do_next = EF_LOADED;
		return 1;
	}
	if (goto_dir_in_string(file_path)) {
		return 1;
	}
	tio_beep();
	return 0;
}
int dof_input_files_to_open()
{
	return dof_drop_files_to_do_action_(ACTION_INPUT);
}
int dof_drop_files_to_open()
{
	return dof_drop_files_to_do_action_(ACTION_SEL);
}

PRIVATE int dof_copy_file_(int update1);
int dof_copy_file()
{
	return dof_copy_file_(0);
}
int dof_copy_file_update()
{
	return dof_copy_file_(1);
}
PRIVATE int dof_copy_file_(int update1)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos(get_another_filer_pane_view()->cur_dir,
	 file_path, MAX_PATH_LEN, HISTORY_TYPE_IDX_DIR,
	 (update1 == 0) ? _("Copy to:") : _("Copy to (Update):")))) {
		return 0;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp", (update1 == 0) ? "-afv" : "-auv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#else
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp", "-a",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_drop_files_to_copy()
{
	return dof_drop_files_to_do_action_(ACTION_COPY);
}

PRIVATE int dof_move_file_(int update1);
int dof_move_file()
{
	return dof_move_file_(0);
}
int dof_move_file_update()
{
	return dof_move_file_(1);
}
PRIVATE int dof_move_file_(int update1)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos(get_another_filer_pane_view()->cur_dir,
	 file_path, MAX_PATH_LEN, HISTORY_TYPE_IDX_DIR,
	 (update1 == 0) ? _("Move to:") : _("Move to (Update):")))) {
		return 0;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv", (update1 == 0) ? "-fv" : "-uv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#else
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_drop_files_to_move()
{
	return dof_drop_files_to_do_action_(ACTION_MOVE);
}

// files dropped by Window manager "'file-1' 'file-2' ..."
int dof_drop_files_to_do_action_(int action)
{
	const char *request = "";
	const char *initial_str = "";
	switch(action) {
	default:
	case ACTION_SEL:
		request = _("Drop files to Open(Enter)/Copy(M-c)/Move(M-m):");
		initial_str = "'";
		action = ACTION_OPEN;	// default action of ACTION_SEL is ACTION_OPEN
		break;
	case ACTION_INPUT:
		request = _("Input files to Open(Enter)/Copy(M-c)/Move(M-m):");
		initial_str = "";
		action = ACTION_OPEN;	// default action of ACTION_SEL is ACTION_OPEN
		break;
	case ACTION_OPEN:
		request = _("Drop files to Open:");
		initial_str = "'";
		break;
	case ACTION_COPY:
		request = _("Drop files to Copy here (current directory):");
		break;
	case ACTION_MOVE:
		request = _("Drop files to Move here (current directory):");
		break;
	}

	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos(initial_str, file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 request))) {
		return 0;
	}
	if (filer_do_next == EF_INPUT_W_ALT_C) {
		action = ACTION_COPY;
	} else
	if (filer_do_next == EF_INPUT_W_ALT_M) {
		action = ACTION_MOVE;
	}

	switch(action) {
	default:
	case ACTION_SEL:
		break;
	case ACTION_INPUT:
	case ACTION_OPEN:
		clear_files_loaded();
		break;
	case ACTION_COPY:
	case ACTION_MOVE:
		begin_fork_exec_repeat();
		break;
	}
	int exit_status = 0;
	for (const char *ptr = file_path; ; ) {
		char path[MAX_PATH_LEN+1];
		ptr = get_one_file_path(ptr, path);
		if (is_sigint_signaled())
			break;
		if (is_strlen_0(path)) {
			break;
		}
		switch(action) {
		case ACTION_OPEN:
			if (load_files_in_string(path,
			 TUL0 | OOE1 | MOE0 | RECURS0 | RDOL0 | FOLF0 | LFH1) >= 0) {
				disp_files_loaded_if_ge_0();
				filer_do_next = EF_LOADED;
			} else {
				filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
			}
			break;
		case ACTION_COPY:
#ifndef	USE_BUSYBOX
			// "cp -afv <file/path/be/copied> ."
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp", "-afv", path, ".", 0);
#else
			// "cp -af <file/path/be/copied> ."
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp", "-af", path, ".", 0);
#endif
			break;
		case ACTION_MOVE:
#ifndef	USE_BUSYBOX
			// "mv -fv <file/path/be/copied> ."
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv", "-fv", path, ".", 0);
#else
			// "mv -f <file/path/be/copied> ."
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv", "-f", path, ".", 0);
#endif
			break;
		}
	}
	switch(action) {
	default:
	case ACTION_SEL:
	case ACTION_INPUT:
	case ACTION_OPEN:
		break;
	case ACTION_COPY:
	case ACTION_MOVE:
		end_fork_exec_repeat(exit_status);
		filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
		break;
	}
	return 0;
}

int dof_rename_file()
{
	char file_name[MAX_PATH_LEN+1];
	strlcpy__(file_name, get_cur_fv_cur_file_ptr()->file_name, MAX_PATH_LEN);
	if (chk_inp_str_ret_val_filer(input_string_pos(file_name, file_name, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Rename to:")))) {
		return 0;
	}
	if (fork_exec_args_once(EX_PAUSE, "mv", "-iv",
	 get_cur_fv_cur_file_ptr()->file_name, file_name, 0) == 0) {
		strlcpy__(get_cur_filer_pane_view()->next_file, file_name, MAX_PATH_LEN);
		filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	}
	return 0;
}
int dof_trash_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Trash file %s ?"),
		 get_cur_fv_cur_file_ptr()->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Trash %d files ?"), files_selected);
	if (ret <= 0) {
		return 0;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		if (fork_exec_args_repeat(EX_SEPARATE, BETRASH,
		 get_cur_fv_file_ptr(file_idx)->file_name, 0)) {
#ifndef	USE_BUSYBOX
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-rv",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-r",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
		}
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_delete_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Delete file %s ?"),
		 get_cur_fv_cur_file_ptr()->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Delete %d files ?"), files_selected);
	if (ret <= 0) {
		return 0;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-rvf",
		 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-rf",
		 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_mark_to_delete_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Mark file %s to be Deleted later ?"),
		 get_cur_fv_cur_file_ptr()->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Mark %d files to be Deleted later ?"),
		 files_selected);
	if (ret <= 0) {
		return 0;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		if (fork_exec_args_repeat(EX_FLAGS_0, BEMARKDEL,
		 get_cur_fv_file_ptr(file_idx)->file_name, 0)) {
#ifndef	USE_BUSYBOX
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "-v", "606",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "606",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
		}
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_size_zero_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0) {
		ret = ask_yes_no(ASK_YES_NO, _("Make size of file %s 0 ?"),
		 get_cur_fv_cur_file_ptr()->file_name);
	} else {
		ret = ask_yes_no(ASK_YES_NO, _("Make size of %d files 0 ?"),
		 files_selected);
	}
	if (ret <= 0) {
		return 0;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		if (fork_exec_args_repeat(EX_FLAGS_0, BESIZE0,
		 get_cur_fv_file_ptr(file_idx)->file_name, 0)) {
#ifndef	USE_BUSYBOX
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "-v", "000",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "000",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
		}
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}

// BEUNZIP file1.tgz file2.tgz ...
int dof_unzip_file()
{
	char command_str[MAX_PATH_LEN+1] = "";
	snprintf_(command_str, MAX_PATH_LEN, "%s", BEUNZIP);
	// "BEZIP file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_fv_file_ptr(file_idx)->file_name);
	}

	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Unzip files:")))) {
		return 0;
	}

	fork_exec_sh_c_once(EX_PAUSE, command_str);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
// BEZIP file.tgz file1 file2 file3 ...
int dof_zip_file()
{
	char command_str[MAX_PATH_LEN+1] = "";
	snprintf_(command_str, MAX_PATH_LEN, "%s {}", BEZIP);
	// "BEZIP file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_fv_file_ptr(file_idx)->file_name);
	}

	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Zip files:")))) {
		return 0;
	}

	fork_exec_sh_c_once(EX_PAUSE, command_str);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}

int dof_find_file()
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 _("Find file:")))) {
		return 0;
	}
	strlcpy__(get_cur_filer_pane_view()->next_file, file_path, MAX_PATH_LEN);
	get_cur_filer_pane_view()->top_file_idx = 0;
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_make_directory()
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Mkdir:")))) {
		return 0;
	}
	fork_exec_args_once(EX_PAUSE, "mkdir", "-p", file_path, 0);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_change_directory()
{
	char string[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", string, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 _("Chdir to:")))) {
		return 0;
	}
	char file_path[MAX_PATH_LEN+1];
	get_file_line_col_from_str(string, file_path, NULL, NULL);
	return filer_change_dir_parent(file_path);
}

int dof_parent_directory()
{
	if (filer_change_dir("..")) {
		return 1;   // OK
	}
	separate_path_to_dir_and_file(
	 get_cur_filer_pane_view()->cur_dir,
	 get_cur_filer_pane_view()->cur_dir,
	 get_cur_filer_pane_view()->next_file);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_beginning_directory()
{
	return filer_change_dir_if_not_yet(get_cur_filer_pane_view()->org_cur_dir);
}
int dof_home_directory()
{
	return filer_change_dir_if_not_yet("~");
}
int dof_root_directory()
{
	return filer_change_dir_if_not_yet("/");
}
int dof_prev_directory()
{
	return filer_change_dir_to_prev_dir();
}
int dof_real_path()
{
	char dir[MAX_PATH_LEN+1];
	file_info_t *f_info = get_cur_fv_cur_file_ptr();
	if (S_ISLNK(f_info->lst.st_mode)) {
		// get real path of selected symlink
		if (is_abs_path(f_info->symlink)) {
			strlcpy__(dir, f_info->symlink, MAX_PATH_LEN);
		} else {
			cat_dir_and_file(dir, get_cur_filer_pane_view()->cur_dir, f_info->symlink);
		}
		if (S_ISREG(f_info->st.st_mode)) {
			char file[MAX_PATH_LEN+1];
			separate_path_to_dir_and_file(dir, dir, file);
		}
	} else {
		// get real path of current directory
		get_real_path_of_cur_dir(dir);
	}
	return filer_change_dir_if_not_yet(dir);
}
//------------------------------------------------------------------------------
int dof_set_filter()
{
	char filter[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", filter, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 _("Set file name filter:")))) {
		return 0;
	}
	strlcpy__(get_cur_filer_pane_view()->filter, filter, MAX_PATH_LEN);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_select_file()
{
	get_cur_fv_cur_file_ptr()->selected = get_cur_fv_cur_file_ptr()->selected ^ _FILE_SEL_MAN_;
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() + 1, get_cur_filer_pane_view()->file_infos_entries-1));
	disp_files_selected();
	return 0;
}
int dof_select_no_file()
{
	for (int file_idx = 0 ; file_idx < get_cur_filer_pane_view()->file_infos_entries;
	 file_idx++) {
		get_cur_fv_file_ptr(file_idx)->selected = _FILE_SEL_NONE_;
	}
	disp_status_bar_done(_("File selection cleared"));
	return 0;
}
int dof_select_all_files()
{
	for (int file_idx = 0 ; file_idx < get_cur_filer_pane_view()->file_infos_entries;
	 file_idx++) {
		if (strcmp(get_cur_fv_file_ptr(file_idx)->file_name, ".") == 0
		 || strcmp(get_cur_fv_file_ptr(file_idx)->file_name, "..") == 0)
			get_cur_fv_file_ptr(file_idx)->selected = 0;
		else
			get_cur_fv_file_ptr(file_idx)->selected
			 = get_cur_fv_file_ptr(file_idx)->selected ^ _FILE_SEL_MAN_;
	}
	disp_files_selected();
	return 0;
}
void disp_files_selected()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	int files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected"),
							_("%d files selected"),
							_("%d filess selected"),
							_("%d filesss selected"),
	 files_selected), files_selected);
}

PRIVATE int dof_quit_filer_();
int dof_quit_filer()
{
////#define ASK_ON_EXIT
#ifdef ASK_ON_EXIT
	if ((get_app_stack_depth() == 0) && count_cut_bufs()) {
		int ret = ask_yes_no(ASK_YES_NO | ASK_END,
		 _("Are you OK to quit %s ?"), APP_LONG_NAME);
		if ((ret != ANSWER_YES) && (ret != ANSWER_END)) {
			disp_status_bar_warn(_("Quiting program cancelled"));
			return 0;
		}
	}
#endif // ASK_ON_EXIT
	return dof_quit_filer_();
}
int dof_quit_home_dir()
{
	if (get_app_stack_depth() == 0) {
		dof_home_directory();
	}
	return dof_quit_filer_();
}
int dof_restart_filer()
{
	restart_be = 1;
	return dof_quit_filer_();
}

PRIVATE int dof_quit_filer_()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	disp_status_bar_done(_("Quit filer"));
	filer_do_next = EF_QUIT;
	return 0;
}
int dof_tog_show_dot_file()
{
	do_tog_show_dot_file();
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_inc_file_view_mode()
{
	inc_file_view_mode();
	SHOW_MODE("File information", get_str_file_view_mode());
	return 0;
}
int dof_clear_file_sort_mode()
{
	clear_file_sort_mode();
	SHOW_MODE("Clear File sort mode", get_str_file_sort_mode());
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_inc_file_sort_mode()
{
	inc_file_sort_mode();
	SHOW_MODE("File sort mode", get_str_file_sort_mode());
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_tog_show_zebra_striping()
{
	do_tog_show_zebra_striping();
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_tog_panes()
{
	tog_filer_panes();
	SHOW_MODE("Filer panes", get_str_filer_panes());
	filer_do_next = FL_UPDATE_FILE_LIST_AUTO;
	return 0;
}
int dof_tog_panex()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	tog_filer_panex();
	SHOW_MODE("Filer pane index", get_str_filer_panex());
	filer_do_next = FL_UPDATE_FILE_LIST_AUTO;
	return 0;
}
int dof_inc_key_list_lines()
{
	return do_inc_key_list_lines_();
}

#ifdef ENABLE_HELP
int dof_splash()
{
	do_splash();

	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_view_file_list()
{
	return view_list(HELP_BUF_IDX_EDITOR_FILE_LIST);
}
int dof_view_func_list()
{
	return view_list(HELP_BUF_IDX_FILER_FUNC_LIST);
}
#endif // ENABLE_HELP

int dof_menu_0()
{
	return filer_menu_n(-1);
}

#endif // ENABLE_FILER

// End of filercmd.c
