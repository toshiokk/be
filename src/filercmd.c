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
	return !IS_EF_STRING_ENTERED(ret);
}

void dof_up()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv)--;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv)--;
	normalize_filer_cursor_y(fv);
}
void dof_down()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv)++;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv)++;
	normalize_filer_cursor_y(fv);
}
void dof_page_up()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) -= filer_vert_scroll_lines(),
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv) -= filer_vert_scroll_lines(),
	normalize_filer_cursor_y(fv);
}
void dof_page_down()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) += filer_vert_scroll_lines(),
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv) += filer_vert_scroll_lines(),
	normalize_filer_cursor_y(fv);
}
void dof_top_of_list()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) = 0;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv) = 0;
	normalize_filer_cursor_y(fv);
}
void dof_bottom_of_list()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) = get_cur_fv_file_info_entries()-1;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv) = filer_win_get_file_list_lines()-1;
	normalize_filer_cursor_y(fv);
}

void dof_refresh_filer()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	disp_status_bar_done(_("File view refreshed"));
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_tap_file()
{
	if (filer_change_dir_to_cur_sel()) {
		return;
	}
	dof_view_file();
}
void dof_tap_file_to_enter()
{
	if (filer_change_dir_to_cur_sel()) {
		return;
	}
	dof_enter_file_name();
}
void dof_tap_file_to_enter_add()
{
	if (filer_change_dir_to_cur_sel()) {
		return;
	}
	dof_enter_file_name_add();
}
void dof_view_file()
{
	if (filer_change_dir_to_cur_sel()) {
		return;
	}
	if (S_ISREG(get_cur_fv_cur_file_info()->st.st_mode)) {
		const char *file_name = get_cur_fv_cur_file_name();
		if (fork_exec_args_once(EX_FLAGS_0, BEPAGER, file_name, 0)) {
			if (fork_exec_args_once(EX_FLAGS_0, "less", file_name, 0)) {
				fork_exec_args_once(EX_FLAGS_0, "more", file_name, 0);
			}
		}
	}
}
void dof_tail_file()	// view file with "tail" command
{
	if (filer_change_dir_to_cur_sel()) {
		return;
	}
	if (S_ISREG(get_cur_fv_cur_file_info()->st.st_mode)) {
		const char *file_name = get_cur_fv_cur_file_name();
		fork_exec_args_once(EX_FLAGS_0, BETAIL, file_name, 0);
	}
}

PRIVATE int dof_open_file_(int flags);

void dof_open_file_recursive()
{
	dof_open_file_(RDOL0 | FOLF0 | LFH0 | RECURS1 | MFPL0);
}
void dof_open_file_ro()
{
	dof_open_file_(RDOL1 | FOLF0 | LFH0 | RECURS1 | MFPL0);
}
void dof_open_file_locked()
{
	delete_all_lock_files();
	dof_open_file_(RDOL0 | FOLF1 | LFH0 | RECURS1 | MFPL0);
}
void dof_open_file_non_recursive()
{
	dof_open_file_(RDOL0 | FOLF0 | LFH0 | RECURS0 | MFPL0);
}
void dof_open_file_from_history()
{
	dof_open_file_(RDOL0 | FOLF0 | LFH1 | RECURS1 | MFPL0);
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
		if (S_ISREG(get_cur_fv_file_info(file_idx)->st.st_mode)) {
			if (load_file_name_upp_low_(get_cur_fv_file_name(file_idx),
			 TUL0 | OOE0 | MOE1 | (flags & (RDOL1 | FOLF1 | LFH1 | RECURS1 | MFPL0))) <= 0) {
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

void dof_open_proj_file()
{
	_doe_open_proj_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	if (get_files_loaded() >= 0) {
		filer_do_next = EF_LOADED;
	}
	clear_files_loaded();
}
void dof_open_exec_log_file()
{
	_doe_open_exec_log_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	if (get_files_loaded() >= 0) {
		filer_do_next = EF_LOADED;
	}
	clear_files_loaded();
}

PRIVATE int _dof_open_new_file(const char *str);
void dof_open_new_file()
{
	char file_path[MAX_PATH_LEN+1] = "";
	concat_file_path_separating_by_space(file_path, MAX_PATH_LEN,
	 get_cur_fv_cur_file_name());
	_dof_open_new_file(file_path);
}
PRIVATE int _dof_open_new_file(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_full_path(str, file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_FILE,
	 _("Open new file:")))) {
		return 0;
	}

	if (load_files_in_string(file_path,
	 TUL0 | OOE1 | MOE0 | RDOL0 | FOLF0 | LFH1 | RECURS0 | MFPL0) >= 0) {
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

#define ACTION_SEL		0	// choose from 'open', 'copy' or 'move'
#define ACTION_INPUT	1	// input file path to open
#define ACTION_OPEN		2
#define ACTION_COPY		3
#define ACTION_MOVE		4
PRIVATE int _dof_drop_files_to_do_action(int action);
void dof_input_files_to_open()
{
	_dof_drop_files_to_do_action(ACTION_INPUT);
}
void dof_drop_files_to_open()
{
	_dof_drop_files_to_do_action(ACTION_SEL);
}

PRIVATE int dof_copy_file_(int update1);
void dof_copy_file_update()
{
	dof_copy_file_(0);
}
void dof_copy_file()
{
	dof_copy_file_(1);
}
void dof_copy_file_update_touch()
{
	dof_copy_file_(2);
}
void dof_copy_file_force_touch()
{
	dof_copy_file_(3);
}
PRIVATE int dof_copy_file_(int update1)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos(get_fv_from_other_pane()->cur_dir,
	 file_path, MAX_PATH_LEN, HISTORY_TYPE_IDX_DIR,
	 (update1 == 0) ? _("Copy to:") : _("Copy to (Update):")))) {
		return 0;
	}
	int use_target_dir = 0;		// "-t target-dir"
	if (get_number_of_selected_files() >= 2) {
		use_target_dir = 1;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

#ifndef	USE_BUSYBOX
		if (use_target_dir == 0) {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp",
			 (update1 == 0) ? "-auv"
			  : ((update1 == 1) ? "-afv"
			   : ((update1 == 2) ? "-uv" : "-fv")),
			 get_cur_fv_file_name(file_idx), file_path, 0);
		} else {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp",
			 (update1 == 0) ? "-auv"
			  : ((update1 == 1) ? "-afv"
			   : ((update1 == 2) ? "-uv" : "-fv")),
			 get_cur_fv_file_name(file_idx), "-t", file_path, 0);
		}
#else
		if (use_target_dir == 0) {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp", "-a",
			 get_cur_fv_file_name(file_idx), file_path, 0);
		} else {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "cp", "-a",
			 get_cur_fv_file_name(file_idx), "-t", file_path, 0);
		}
#endif

	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
void dof_drop_files_to_copy()
{
	_dof_drop_files_to_do_action(ACTION_COPY);
}

PRIVATE int dof_move_file_(int update1);
void dof_move_file()
{
	dof_move_file_(0);
}
void dof_move_file_update()
{
	dof_move_file_(1);
}
PRIVATE int dof_move_file_(int update1)
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos(get_fv_from_other_pane()->cur_dir,
	 file_path, MAX_PATH_LEN, HISTORY_TYPE_IDX_DIR,
	 (update1 == 0) ? _("Move to:") : _("Move to (Update):")))) {
		return 0;
	}
	int use_target_dir = 0;		// "-t target-dir"
	if (get_number_of_selected_files() >= 2) {
		use_target_dir = 1;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

#ifndef	USE_BUSYBOX
		if (use_target_dir == 0) {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv", (update1 == 0) ? "-fv" : "-uv",
			 get_cur_fv_file_name(file_idx), file_path, 0);
		} else {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv", (update1 == 0) ? "-fv" : "-uv",
			 get_cur_fv_file_name(file_idx), "-t", file_path, 0);
		}
#else
		if (use_target_dir == 0) {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv",
			 get_cur_fv_file_name(file_idx), file_path, 0);
		} else {
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "mv",
			 get_cur_fv_file_name(file_idx), "-t", file_path, 0);
		}
#endif

	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
void dof_drop_files_to_move()
{
	_dof_drop_files_to_do_action(ACTION_MOVE);
}

// files dropped by Window manager "'file-1' 'file-2' ..."
PRIVATE int _dof_drop_files_to_do_action(int action)
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
	if (filer_do_next == EF_INPUT_PATH_TO_COPY) {
		action = ACTION_COPY;
	} else
	if (filer_do_next == EF_INPUT_PATH_TO_MOVE) {
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
			 TUL0 | OOE1 | MOE0 | RDOL0 | FOLF0 | LFH1 | RECURS0 | MFPL0) >= 0) {
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

void dof_rename_file()
{
	char file_name[MAX_PATH_LEN+1];
	strlcpy__(file_name, get_cur_fv_cur_file_name(), MAX_PATH_LEN);
	if (chk_inp_str_ret_val_filer(input_string_pos(file_name, file_name, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Rename to:")))) {
		return;
	}
	if (fork_exec_args_once(EX_PAUSE, "mv", "-iv",
	 get_cur_fv_cur_file_name(), file_name, 0) == 0) {
		strlcpy__(get_fv_from_cur_pane()->next_file, file_name, MAX_PATH_LEN);
		filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	}
}
void dof_trash_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Trash file %s ?"),
		 get_cur_fv_cur_file_name());
	else
		ret = ask_yes_no(ASK_YES_NO, _("Trash %d files ?"), files_selected);
	if (ret <= 0) {
		return;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0; file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

		if (fork_exec_args_repeat(EX_SEPARATE, BETRASH, get_cur_fv_file_name(file_idx), 0)) {
#ifndef	USE_BUSYBOX
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-rv",
			 get_cur_fv_file_name(file_idx), 0);
#else
			exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-r",
			 get_cur_fv_file_name(file_idx), 0);
#endif
		}

	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_delete_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Delete file %s ?"),
		 get_cur_fv_cur_file_name());
	else
		ret = ask_yes_no(ASK_YES_NO, _("Delete %d files ?"), files_selected);
	if (ret <= 0) {
		return;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0; file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

#ifndef	USE_BUSYBOX
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-rvf",
		 get_cur_fv_file_name(file_idx), 0);
#else
		exit_status = fork_exec_args_repeat(EX_SEPARATE, "rm", "-rf",
		 get_cur_fv_file_name(file_idx), 0);
#endif

	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_mark_to_delete_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Mark file %s to be Deleted later ?"),
		 get_cur_fv_cur_file_name());
	else
		ret = ask_yes_no(ASK_YES_NO, _("Mark %d files to be Deleted later ?"),
		 files_selected);
	if (ret <= 0) {
		return;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0; file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

		if (fork_exec_args_repeat(EX_FLAGS_0, BEMARKDEL, get_cur_fv_file_name(file_idx), 0)) {
#ifndef	USE_BUSYBOX
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "-v", "606",
			 get_cur_fv_file_name(file_idx), 0);
#else
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "606",
			 get_cur_fv_file_name(file_idx), 0);
#endif
		}

	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_size_zero_file()
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0) {
		ret = ask_yes_no(ASK_YES_NO, _("Make size of file %s 0 ?"),
		 get_cur_fv_cur_file_name());
	} else {
		ret = ask_yes_no(ASK_YES_NO, _("Make size of %d files 0 ?"),
		 files_selected);
	}
	if (ret <= 0) {
		return;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0; file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

		if (fork_exec_args_repeat(EX_FLAGS_0, BESIZE0, get_cur_fv_file_name(file_idx), 0)) {
#ifndef	USE_BUSYBOX
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "-v", "000",
			 get_cur_fv_file_name(file_idx), 0);
#else
			exit_status = fork_exec_args_repeat(EX_FLAGS_0, "chmod", "000",
			 get_cur_fv_file_name(file_idx), 0);
#endif
		}

	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}

// BEUNZIP file1.tgz file2.tgz ...
void dof_unzip_file()
{
	char command_str[MAX_PATH_LEN+1] = "";
	snprintf_(command_str, MAX_PATH_LEN, "%s", BEUNZIP);
	// "BEZIP file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_fv_file_name(file_idx));
	}

	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Unzip files:")))) {
		return;
	}

	fork_exec_sh_c_once(EX_PAUSE, command_str);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
// BEZIP file.tgz file1 file2 file3 ...
void dof_zip_file()
{
	char command_str[MAX_PATH_LEN+1] = "";
	snprintf_(command_str, MAX_PATH_LEN, "%s {}", BEZIP);
	// "BEZIP file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_fv_file_name(file_idx));
	}

	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Zip files:")))) {
		return;
	}

	fork_exec_sh_c_once(EX_PAUSE, command_str);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}

void dof_find_file()
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 _("Find file:")))) {
		return;
	}
	strlcpy__(get_fv_from_cur_pane()->next_file, file_path, MAX_PATH_LEN);
///	FV_CURS_Y(get_fv_from_cur_pane()) = 0;
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_make_directory()
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Mkdir:")))) {
		return;
	}
	fork_exec_args_once(EX_PAUSE, "mkdir", "-p", file_path, 0);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_change_directory()
{
	char string[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", string, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 _("Chdir to:")))) {
		return;
	}
	char file_path[MAX_PATH_LEN+1];
	get_file_line_col_from_str(string, file_path, NULL, NULL);
	filer_change_dir_parent(file_path);
}

void dof_parent_directory()
{
	if (filer_change_dir("..")) {
		return;
	}
	separate_path_to_dir_and_file(
	 get_fv_from_cur_pane()->cur_dir,
	 get_fv_from_cur_pane()->cur_dir,
	 get_fv_from_cur_pane()->next_file);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_beginning_directory()
{
	filer_change_dir_if_not_yet(get_fv_from_cur_pane()->org_cur_dir);
}
void dof_home_directory()
{
	filer_change_dir_if_not_yet("~");
}
void dof_root_directory()
{
	filer_change_dir_if_not_yet("/");
}
void dof_prev_directory()
{
	filer_change_dir_to_prev_dir();
}
void dof_real_path()
{
	char dir[MAX_PATH_LEN+1];
	file_info_t *f_info = get_cur_fv_cur_file_info();
	if (S_ISLNK(f_info->lst.st_mode)) {
		// get real path of selected symlink
		if (is_abs_path(f_info->symlink)) {
			strlcpy__(dir, f_info->symlink, MAX_PATH_LEN);
		} else {
			cat_dir_and_file(dir, get_fv_from_cur_pane()->cur_dir, f_info->symlink);
		}
		if (S_ISREG(f_info->st.st_mode)) {
			char file[MAX_PATH_LEN+1];
			separate_path_to_dir_and_file(dir, dir, file);
		}
	} else {
		// get real path of current directory
		get_real_path_of_cur_dir(dir);
	}
	filer_change_dir_if_not_yet(dir);
}
//------------------------------------------------------------------------------
void dof_enter_file_name()     { filer_do_next = ((input_string_full_path == 0)
 ? FL_ENTER_FILE_NAME : FL_ENTER_FILE_PATH); }
void dof_enter_file_name_add() { filer_do_next = ((input_string_full_path == 0)
 ? FL_ENTER_FILE_NAME_ADD : FL_ENTER_FILE_PATH_ADD); }
void dof_enter_file_path()     { filer_do_next = FL_ENTER_FILE_PATH; }
void dof_enter_file_path_add() { filer_do_next = FL_ENTER_FILE_PATH_ADD; }
void dof_enter_dir_path()      { filer_do_next = FL_ENTER_DIR_PATH; }
void dof_enter_dir_path_add()  { filer_do_next = FL_ENTER_DIR_PATH_ADD; }
//------------------------------------------------------------------------------
void dof_set_filter()
{
	char filter[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", filter, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 _("Set file name filter:")))) {
		return;
	}
	strlcpy__(get_fv_from_cur_pane()->filter, filter, MAX_PATH_LEN);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_select_file()
{
	get_cur_fv_cur_file_info()->selected = get_cur_fv_cur_file_info()->selected ^ _FILE_SEL_MAN_;
	dof_down();
	disp_files_selected();
}
void dof_select_no_file()
{
	for (int file_idx = 0 ; file_idx < get_cur_fv_file_info_entries(); file_idx++) {
		get_cur_fv_file_info(file_idx)->selected = _FILE_SEL_NONE_;
	}
	disp_status_bar_done(_("File selection cleared"));
}
void dof_select_all_files()
{
	for (int file_idx = 0 ; file_idx < get_cur_fv_file_info_entries(); file_idx++) {
		if ((strcmp(get_cur_fv_file_name(file_idx), ".") == 0)
		 || (strcmp(get_cur_fv_file_name(file_idx), "..") == 0))
			get_cur_fv_file_info(file_idx)->selected = 0;
		else
			get_cur_fv_file_info(file_idx)->selected
			 = get_cur_fv_file_info(file_idx)->selected ^ _FILE_SEL_MAN_;
	}
	disp_files_selected();
}
void disp_files_selected()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	off_t size = 0;
	for (int file_idx = 0 ; file_idx < get_cur_fv_file_info_entries(); file_idx++) {
		if (get_cur_fv_file_info(file_idx)->selected) {
			size += get_cur_fv_file_info(file_idx)->lst.st_size;
		}
	}
	char buf_size[6+1] = "";
	get_file_size_str(buf_size, size);
	int files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected, total size: %s"),
							_("%d files selected, total size: %s"),
							_("%d filess selected, total size: %s"),
							_("%d filesss selected, total size: %s"),
	 files_selected), files_selected, buf_size);
}

PRIVATE int dof_quit_filer_();
void dof_quit_filer()
{
////#define ASK_ON_EXIT
#ifdef ASK_ON_EXIT
	if ((get_app_stack_depth() == 0) && count_cut_bufs()) {
		int ret = ask_yes_no(ASK_YES_NO | ASK_END,
		 _("Are you OK to quit %s ?"), APP_LONG_NAME);
		if ((ret != ANSWER_YES) && (ret != ANSWER_END)) {
			disp_status_bar_warn(_("Quiting program cancelled"));
			return;
		}
	}
#endif // ASK_ON_EXIT
	dof_quit_filer_();
}
void dof_quit_home_dir()
{
	if (get_app_stack_depth() == 0) {
		dof_home_directory();
	}
	dof_quit_filer_();
}
void dof_restart_filer()
{
	restart_be = 1;
	dof_quit_filer_();
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
void dof_tog_show_dot_file()
{
	_dof_tog_show_dot_file();
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_inc_file_view_mode()
{
	inc_file_view_mode();
	SHOW_MODE("File information", get_str_file_view_mode());
}
void dof_clear_file_sort_mode()
{
	clear_file_sort_mode();
	SHOW_MODE("Clear File sort mode", get_str_file_sort_mode());
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_inc_file_sort_mode()
{
	inc_file_sort_mode();
	SHOW_MODE("File sort mode", get_str_file_sort_mode());
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_tog_show_zebra_striping()
{
	_dof_tog_show_zebra_striping();
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_tog_panes()
{
	tog_filer_panes();
	SHOW_MODE("Filer panes", get_str_filer_panes());
	filer_do_next = FL_UPDATE_FILE_LIST_AUTO;
}
void dof_tog_panex()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	tog_filer_panex();
	SHOW_MODE("Filer pane index", get_str_filer_panex());
	filer_do_next = FL_UPDATE_FILE_LIST_AUTO;
}
void dof_inc_key_list_lines()
{
	_dof_inc_key_list_lines();
}

#ifdef ENABLE_HELP
void dof_splash()
{
	do_splash();

	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
}
void dof_view_file_list()
{
	view_list(HELP_BUF_IDX_EDITOR_FILE_LIST);
}
void dof_view_func_list()
{
	view_list(HELP_BUF_IDX_FILER_FUNC_LIST);
}
#endif // ENABLE_HELP

void dof_menu_0()
{
	filer_menu_n(-1);
}

#endif // ENABLE_FILER

// End of filercmd.c
