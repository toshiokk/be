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

PRIVATE int dof_open_file_(int load_from_history, int recursive);

PRIVATE int filer_change_dir_to_cur_sel(void);
PRIVATE int filer_change_dir_if_not_yet(char *dir);
PRIVATE int filer_change_dir_to_prev_dir(void);

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"

#define BECMD		"becmd"		// becmd?

PRIVATE int is_reg_file_and_app_list_mode_then_enter_file_name()
{
	if (S_ISREG(get_cur_fv_file_ptr(get_cur_fv_file_idx())->st.st_mode)
	 && is_app_list_mode()) {
		filer_do_next = FILER_DO_ENTER_FILE_NAME;
		return -1;
	}
	return 0;
}
BOOL set_filer_do_next(int ret)
{
	if (ret == INPUT_LOADED) {
		filer_do_next = FILER_LOADED;
	}
	return ret <= INPUT_LOADED;
}

int dof_up(void)
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() - 1,
	 get_cur_filer_view()->file_list_entries-1));
	return 1;
}
int dof_down(void)
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() + 1,
	 get_cur_filer_view()->file_list_entries-1));
	return 1;
}
int dof_page_up(void)
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() - FILER_VERT_SCROLL_LINES,
	 get_cur_filer_view()->file_list_entries-1));
	return 1;
}
int dof_page_down(void)
{
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() + FILER_VERT_SCROLL_LINES,
	 get_cur_filer_view()->file_list_entries-1));
	return 1;
}
int dof_top_of_list(void)
{
	set_cur_fv_file_idx(0);
	return 1;
}
int dof_bottom_of_list(void)
{
	set_cur_fv_file_idx(get_cur_filer_view()->file_list_entries-1);
	return 1;
}

int dof_switch_filer_pane(void)
{
	set_filer_cur_pane_idx((get_filer_cur_pane_idx() + 1) % FILER_PANES);
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 1;
}
int dof_refresh_filer(void)
{
	disp_status_bar_done(_("File view refreshed"));
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 1;
}
int dof_tap_file(void)
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	if (is_reg_file_and_app_list_mode_then_enter_file_name()) {
		return -1;
	}

	if (S_ISREG(get_cur_fv_file_ptr(get_cur_fv_file_idx())->st.st_mode)) {
		if (count_edit_bufs() == 0) {
			return dof_view_file();
		} else {
			return dof_open_file_non_recursive();
		}
	}
	return 0;
}

int dof_open_file(void)
{
	dof_open_file_(LFH0, RECURSIVE1);
	return 1;
}
int dof_open_file_non_recursive(void)
{
	dof_open_file_(LFH0, RECURSIVE0);
	return 1;
}
int dof_open_file_from_history(void)
{
	dof_open_file_(LFH1, RECURSIVE1);
	return 1;
}

int dof_open_proj_file(void)
{
	do_open_proj_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	if (get_files_loaded() >= 0) {
		filer_do_next = FILER_LOADED;
	}
	clear_files_loaded();
	return 1;
}
int dof_open_exec_log_file(void)
{
	do_open_exec_log_file();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	if (get_files_loaded() >= 0) {
		filer_do_next = FILER_LOADED;
	}
	clear_files_loaded();
	return 1;
}

PRIVATE int dof_open_new_file_(const char *str);
int dof_open_new_file(void)
{
	return dof_open_new_file_("");
}
// open files dropped by Window manager 'file-1' 'file-2' ...
int dof_open_new_file_dropped(void)
{
	return dof_open_new_file_("'");
}
PRIVATE int dof_open_new_file_(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = input_string_tail(str, file_path, HISTORY_TYPE_IDX_FILEPOS,
	 _("Open new file:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}

	if (load_files_in_string(file_path, TUL0, OOE1, MOE0, LFH1, RECURSIVE0) >= 0) {
		disp_files_loaded_if_ge_0();
		filer_do_next = FILER_LOADED;
		return 1;
	}
	if (goto_dir_in_string(file_path)) {
		return 1;
	}
	tio_beep();
	return 0;
}
int dof_view_file(void)
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	int file_idx = get_cur_fv_file_idx();
	char *file_name = get_cur_fv_file_ptr(file_idx)->file_name;
	if (S_ISREG(get_cur_fv_file_ptr(file_idx)->st.st_mode)) {
		if (fork_exec_args_once(PAUSE0, BEPAGER, file_name, 0))
			if (fork_exec_args_once(PAUSE0, "less", file_name, 0))
				fork_exec_args_once(PAUSE0, "more", file_name, 0);
	}
	return 0;
}
int dof_tail_file(void)	// view file with "tail" command
{
	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	int file_idx = get_cur_fv_file_idx();
	char *file_name = get_cur_fv_file_ptr(file_idx)->file_name;
	if (S_ISREG(get_cur_fv_file_ptr(file_idx)->st.st_mode)) {
		fork_exec_args_once(PAUSE0, BETAIL, file_name, 0);
	}
	return 0;
}
int dof_copy_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = input_string_tail(get_other_filer_view()->cur_dir, file_path, HISTORY_TYPE_IDX_DIR,
	 _("Copy to:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_args_repeat(SEPARATE1, "cp", "-afv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#else
		fork_exec_args_repeat(SEPARATE1, "cp", "-a",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_copy_file_update(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = input_string_tail(get_other_filer_view()->cur_dir, file_path, HISTORY_TYPE_IDX_DIR,
	 _("Copy to (Update):"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_args_repeat(SEPARATE1, "cp", "-aufv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#else
		fork_exec_args_repeat(SEPARATE1, "cp", "-a",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_move_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = input_string_tail(get_other_filer_view()->cur_dir, file_path, HISTORY_TYPE_IDX_DIR,
	 _("Move to:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_args_repeat(SEPARATE1, "mv", "-ufv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#else
		fork_exec_args_repeat(SEPARATE1, "mv",
		 get_cur_fv_file_ptr(file_idx)->file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_trash_file(void)
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Trash file %s ?"),
		 get_cur_fv_file_ptr(get_cur_fv_file_idx())->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Trash %d files ?"), files_selected);
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		if (fork_exec_args_repeat(SEPARATE1, BETRASH,
		 get_cur_fv_file_ptr(file_idx)->file_name, 0)) {
#ifndef	USE_BUSYBOX
			fork_exec_args_repeat(SEPARATE1, "rm", "-rv",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
			fork_exec_args_repeat(SEPARATE1, "rm", "-r",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
		}
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_delete_file(void)
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Delete file %s ?"),
		 get_cur_fv_file_ptr(get_cur_fv_file_idx())->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Delete %d files ?"), files_selected);
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_args_repeat(SEPARATE1, "rm", "-rv",
		 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
		fork_exec_args_repeat(SEPARATE1, "rm", "-r",
		 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_mark_to_delete_file(void)
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Mark file %s to be Deleted later ?"),
		 get_cur_fv_file_ptr(get_cur_fv_file_idx())->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Mark %d files to be Deleted later ?"),
		 files_selected);
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		if (fork_exec_args_repeat(SEPARATE0, BEMARKDEL,
		 get_cur_fv_file_ptr(file_idx)->file_name, 0)) {
#ifndef	USE_BUSYBOX
			fork_exec_args_repeat(SEPARATE0, "chmod", "-v", "606",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
			fork_exec_args_repeat(SEPARATE0, "chmod", "606",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
		}
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_size_zero_file(void)
{
	int files_selected;
	int ret;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Make size of file %s 0 ?"),
		 get_cur_fv_file_ptr(get_cur_fv_file_idx())->file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Make size of %d files 0 ?"),
		 files_selected);
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		if (fork_exec_args_repeat(SEPARATE0, BESIZE0,
		 get_cur_fv_file_ptr(file_idx)->file_name, 0)) {
#ifndef	USE_BUSYBOX
			fork_exec_args_repeat(SEPARATE0, "chmod", "-v", "000",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#else
			fork_exec_args_repeat(SEPARATE0, "chmod", "000",
			 get_cur_fv_file_ptr(file_idx)->file_name, 0);
#endif
		}
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_rename_file(void)
{
	char file_name[MAX_PATH_LEN+1];

	int file_idx = select_and_get_first_file_idx_selected();
	strlcpy__(file_name, get_cur_fv_file_ptr(file_idx)->file_name,
	 MAX_PATH_LEN);
	int ret = input_string_tail(file_name, file_name, HISTORY_TYPE_IDX_EXEC,
	 _("Rename to:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	if (fork_exec_args_once(PAUSE1, "mv", "-i",
	 get_cur_fv_file_ptr(file_idx)->file_name,
	 file_name, 0) == 0) {
		strlcpy__(get_cur_filer_view()->next_file, file_name, MAX_PATH_LEN);
		filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	}
	return 0;
}
int dof_find_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = input_string_tail("", file_path, HISTORY_TYPE_IDX_DIR,
	 _("Find:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	strlcpy__(get_cur_filer_view()->next_file, file_path, MAX_PATH_LEN);
	get_cur_filer_view()->top_file_idx = 0;
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_make_directory(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = input_string_tail("", file_path, HISTORY_TYPE_IDX_DIR,
	 _("Mkdir:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	fork_exec_args_once(PAUSE1, "mkdir", "-p", file_path, 0);
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_change_directory(void)
{
	char string[MAX_PATH_LEN+1];
	int ret = input_string_tail("", string, HISTORY_TYPE_IDX_DIR,
	 _("Chdir to:"));
	if (set_filer_do_next(ret)) {
		return 0;
	}
	char file_path[MAX_PATH_LEN+1];
	get_file_line_col_from_str_null(string, file_path, NULL, NULL);
	return filer_change_dir_parent(file_path);
}

int dof_parent_directory(void)
{
	if (filer_change_dir("..")) {
		return 1;   // OK
	}
	separate_path_to_dir_and_file(
	 get_cur_filer_view()->cur_dir,
	 get_cur_filer_view()->cur_dir,
	 get_cur_filer_view()->next_file);
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_beginning_directory(void)
{
	return filer_change_dir_if_not_yet(cur_filer_panes->org_cur_dir);
}
int dof_home_directory(void)
{
	return filer_change_dir_if_not_yet("~");
}
int dof_root_directory(void)
{
	return filer_change_dir_if_not_yet("/");
}
int dof_prev_directory(void)
{
	return filer_change_dir_to_prev_dir();
}
int dof_real_path(void)
{
	char dir[MAX_PATH_LEN+1];
	file_info_t *f_info = get_cur_fv_file_ptr(get_cur_fv_file_idx());
	if (S_ISLNK(f_info->lst.st_mode)) {
		if (is_abs_path(f_info->symlink)) {
			strlcpy__(dir, f_info->symlink, MAX_PATH_LEN);
		} else {
			cat_dir_and_file(dir, get_cur_filer_view()->cur_dir, f_info->symlink);
		}
		if (S_ISREG(f_info->st.st_mode)) {
			char file[MAX_PATH_LEN+1];
			separate_path_to_dir_and_file(dir, dir, file);
		}
	} else {
		get_real_path_of_cur_dir(dir);
	}
	return filer_change_dir_if_not_yet(dir);
}
//-----------------------------------------------------------------------------
int dof_select_file(void)
{
	get_cur_fv_file_ptr(get_cur_fv_file_idx())->selected
	 = get_cur_fv_file_ptr(get_cur_fv_file_idx())->selected ^ _FILE_SEL_MAN_;
	set_cur_fv_file_idx(MIN_MAX_(0,
	 get_cur_fv_file_idx() + 1, get_cur_filer_view()->file_list_entries-1));
	disp_files_selected();
	return 0;
}
int dof_select_no_file(void)
{
	for (int file_idx = 0 ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		get_cur_fv_file_ptr(file_idx)->selected = _FILE_SEL_NONE_;
	}
	disp_status_bar_done(_("File selection cleared"));
	return 0;
}
int dof_select_all_files(void)
{
	for (int file_idx = 0 ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
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
	int files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected"),
							_("%d files selected"),
							_("%d filess selected"),
							_("%d filesss selected"),
	 files_selected), files_selected);
}
int dof_quit_filer(void)
{
	filer_do_next = FILER_DO_QUIT;
	return 0;
}
int dof_quit_home_dir(void)
{
	dof_home_directory();
	dof_quit_filer();
	return 0;
}
int dof_tog_show_dot_file(void)
{
	do_tog_show_dot_file();
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_inc_show_file_info(void)
{
	inc_show_file_info();
	SHOW_MODE("File information", get_str_show_file_info());
	return 0;
}
int dof_clear_sort_by(void)
{
	clear_sort_by();
	SHOW_MODE("Clear File sort mode", get_str_sort_by());
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_inc_sort_by(void)
{
	inc_sort_by();
	SHOW_MODE("File sort mode", get_str_sort_by());
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_tog_panes(void)
{
	tog_filer_panes();
	SHOW_MODE("Filer panes", get_str_filer_panes());
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_AUTO;
	return 0;
}
int dof_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	return 0;
}

int dof_display_color_settings(void)
{
	display_color_settings();
	return 0;
}

#ifdef ENABLE_HELP
int dof_filer_splash(void)
{
	disp_splash(100);

	examine_key_code();

	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
#endif // ENABLE_HELP

int dof_filer_menu_0(void)
{
	filer_menu_n(-1);
	return 0;
}
int dof_filer_menu_1(void)
{
	if (is_app_list_mode()) {
		filer_do_next = FILER_DO_QUIT;
		return -1;
	}

	filer_menu_n(0);
	return 0;
}
int dof_filer_menu_2(void)
{
	filer_menu_n(1);
	return 0;
}
int dof_filer_menu_3(void)
{
	filer_menu_n(2);
	return 0;
}
int dof_filer_menu_4(void)
{
	filer_menu_n(2);
	return 0;
}
int dof_filer_menu_5(void)
{
	filer_menu_n(2);
	return 0;
}

//-----------------------------------------------------------------------------
PRIVATE int dof_open_file_(int load_from_history, int recursive)
{
#ifdef ENABLE_HISTORY
	int prev_count_edit_bufs = count_edit_bufs();
#endif // ENABLE_HISTORY

	if (filer_change_dir_to_cur_sel()) {
		return 0;
	}
	if (is_reg_file_and_app_list_mode_then_enter_file_name()) {
		return -1;
	}

	clear_files_loaded();
	begin_check_break_key();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (S_ISREG(get_cur_fv_file_ptr(file_idx)->st.st_mode)) {
			if (load_file_name_upp_low_(get_cur_fv_file_ptr(file_idx)->file_name,
			 TUL0, OOE0, MOE1, load_from_history, recursive) <= 0) {
				tio_beep();
			}
		}
		if (check_break_key()) {
			break;
		}
	}
	end_check_break_key();

	if (get_files_loaded() >= 0) {
#ifdef ENABLE_HISTORY
		if (prev_count_edit_bufs == 0) {
			goto_last_file_line_col_in_history();
		}
#endif // ENABLE_HISTORY
	}
	unselect_all_files_auto(_FILE_SEL_MAN_ | _FILE_SEL_AUTO_);
	disp_files_loaded_if_ge_0();
	if (get_files_loaded() < 0) {
		filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	} else {
		filer_do_next = FILER_LOADED;
	}
	return 0;
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_FILER
#ifdef ENABLE_HISTORY
PRIVATE int change_cur_dir_from_history(const char *dir);
#endif // ENABLE_HISTORY
#endif // ENABLE_FILER

int goto_dir_in_string(const char *str)
{
	char dir_save[MAX_PATH_LEN+1];
	change_cur_dir_by_file_path_after_save(dir_save, get_epc_buf()->file_path);

	char dir[MAX_PATH_LEN+1];
	for (int field_idx = 0; field_idx < 10; field_idx++) {
		if (get_n_th_file_line_col_from_str_null(str, field_idx, dir, NULL, NULL) > 0) {
			// directory gotten
			if (change_cur_dir_saving_prev_next(dir)) {
				// directory changed
				goto changed;
			}
#ifdef ENABLE_HISTORY
			if (change_cur_dir_from_history(dir)) {
				// directory changed
				goto changed;
			}
#endif // ENABLE_HISTORY
		}
	}
	change_cur_dir(dir_save);
	disp_status_bar_err(_("Can not change directory to [%s]"), str);
_FLF_
	return 0;

changed:;
#ifdef ENABLE_HISTORY
	update_dir_history(get_cur_filer_view()->prev_dir, get_cur_filer_view()->cur_dir);
#endif // ENABLE_HISTORY
_FLF_
	return 1;
}

#ifdef ENABLE_FILER
#ifdef ENABLE_HISTORY
PRIVATE int change_cur_dir_from_history(const char *dir)
{
	set_history_newest(HISTORY_TYPE_IDX_DIR);
	for ( ; ; ) {
		char *history = get_history_older(HISTORY_TYPE_IDX_DIR);
		if (strlen_path(history) == 0) {
			break;
		}
/////flf_d_printf("history: [%s]\n", history);
		if (compare_file_path_from_tail(history, dir) == 0) {
			if (change_cur_dir_saving_prev_next(history)) {
				return 1;
			}
		}
	}
	return 0;
}
#endif // ENABLE_HISTORY
#endif // ENABLE_FILER

PRIVATE int filer_change_dir_to_cur_sel(void)
{
	int file_idx = select_and_get_first_file_idx_selected();
	if (S_ISDIR(get_cur_fv_file_ptr(file_idx)->st.st_mode)) {
		filer_change_dir(get_cur_fv_file_ptr(file_idx)->file_name);
		return 1;		// OK
	}
	return 0;			// error
}
PRIVATE int filer_change_dir_if_not_yet(char *dir)
{
	if (strcmp(get_cur_filer_view()->cur_dir, dir) == 0) {
		return filer_change_dir_to_prev_dir();
	} else {
		return filer_change_dir(dir);
	}
}
PRIVATE int filer_change_dir_to_prev_dir(void)
{
	if (strlen(get_cur_filer_view()->prev_dir)) {
		return filer_change_dir(get_cur_filer_view()->prev_dir);
	}
	return 0;		// error
}

// If can not change dir, try parent dir
int filer_change_dir_parent(char *path)
{
	char dir[MAX_PATH_LEN+1];

	strlcpy__(dir, path, MAX_PATH_LEN);
	for ( ; ; ) {
flf_d_printf("try to dir[%s]\n", dir);
		if (strcmp(dir, "/") == 0) {
			return 0;	// error
		}
		if (filer_change_dir(dir)) {
			break;
		}
		// If can not change dir, try parent dir
		// /try/to/change/dir/file ==> /try/to/change/dir
		strip_file_from_path(dir, NULL);
	}
	return 1;	// changed
}
int filer_change_dir(char *dir)
{
	if (change_cur_dir_saving_prev_next(dir) == 0) {
		// We can't open this dir for some reason. Complain.
		disp_status_bar_err(_("Can not change current to [%s]: %s"),
		 shrink_str_to_scr_static(dir), strerror(errno));
		return 0;	// error
	}
#ifdef ENABLE_HISTORY
	// previous dir, next dir
	update_dir_history(get_cur_filer_view()->prev_dir, get_cur_filer_view()->cur_dir);
#endif // ENABLE_HISTORY
	get_cur_filer_view()->top_file_idx = 0;
	disp_status_bar_done(_("Changed current directory to [%s]"),
	 shrink_str_to_scr_static(get_cur_filer_view()->cur_dir));
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 1;		// OK
}

int change_cur_dir_saving_prev_next(char *dir)
{
	return change_cur_dir_saving_prev_next_dir(dir,
	 get_cur_filer_view()->cur_dir,
	 get_cur_filer_view()->prev_dir,
	 get_cur_filer_view()->next_file);
}

#endif // ENABLE_FILER

// End of filercmd.c
