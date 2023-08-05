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

PRIVATE int do_edit_file_(int recursive);

PRIVATE int if_dir_change_dir(void);
PRIVATE int filer_change_dir_if_not_yet(const char *dir);
PRIVATE int filer_change_prev_dir(void);

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"

#define BECMD		"becmd"		// becmd?

int do_top_of_list(void)
{
	cur_fv->cur_sel_idx = 0;
	return 1;
}
int do_bottom_of_list(void)
{
	cur_fv->cur_sel_idx = cur_fv->file_list_entries-1;
	return 1;
}

int do_switch_filer_pane(void)
{
	if (++(cur_filer_views->view_idx) >= FILER_VIEWS) {
		cur_filer_views->view_idx = 0;
	}
	set_cur_filer_view();
	filer_do_next = FILER_REFRESH_FORCED;
	return 1;
}
int do_refresh_filer(void)
{
	disp_status_bar_done(_("File view refreshed"));
	filer_do_next = FILER_REFRESH_FORCED;
	return 1;
}
int do_enter_file(void)
{
	if (if_dir_change_dir()) {
		return 0;
	}
///_FLF_
	if (S_ISREG(cur_fv->file_list[cur_fv->cur_sel_idx].st.st_mode)) {
		if (count_edit_bufs() == 0) {
			return do_view_file();
		} else {
			return do_edit_file_non_recursive();
		}
	}
	return 0;
}
int do_edit_file(void)
{
	do_edit_file_(1);
	return 1;
}
int do_edit_file_non_recursive(void)
{
///_FLF_
	do_edit_file_(0);
	return 1;
}
int do_edit_new_file(void)
{
	char file_name[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_name, HISTORY_TYPE_IDX_DIR, _("Edit new file:"));

	if (ret < 0) {
		// do_edit_new_file -> FILER_ENTERED_DIR_PATH
		filer_do_next = FILER_ENTERED_DIR_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	clear_files_loaded();
	if (load_file_name_upp_low(file_name, TUL0, OOE1, WOE0, RECURSIVE0) <= 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded_ifnon0();
	filer_do_next = FILER_LOADED_FILE;
	return 1;
}
int do_view_file(void)
{
	int file_idx;
	char *file_name;

///_FLF_
	if (if_dir_change_dir()) {
		return 0;
	}
	if (is_app_list_mode()) {
		// do_view_file -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	file_idx = cur_fv->cur_sel_idx;
	file_name = cur_fv->file_list[file_idx].file_name;
	if (S_ISREG(cur_fv->file_list[file_idx].st.st_mode)) {
		if (fork_exec_once(SEPARATE1, PAUSE0, BEPAGER, file_name, 0))
			if (fork_exec_once(SEPARATE1, PAUSE0, "less", file_name, 0))
				fork_exec_once(SEPARATE1, PAUSE0, "more", file_name, 0);
	}
	return 0;
}
int do_tail_file(void)	// view file with "tail" command
{
	int file_idx;
	char *file_name;

	if (if_dir_change_dir()) {
		return 0;
	}
	if (is_app_list_mode()) {
		// do_tail_file -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	file_idx = cur_fv->cur_sel_idx;
	file_name = cur_fv->file_list[file_idx].file_name;
	if (S_ISREG(cur_fv->file_list[file_idx].st.st_mode)) {
		fork_exec_once(SEPARATE1, PAUSE0, BETAIL, file_name, 0);
	}
	return 0;
}
int do_copy_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	file_idx = get_first_file_idx_selected();

	ret = input_string(get_other_filer_view()->cur_dir, file_path,
	 HISTORY_TYPE_IDX_DIR, _("Copy to:"));

	if (ret < 0) {
		// do_copy_file -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_handler_sigint_called())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_repeat(SEPARATE1, "cp", "-afv",
		 cur_fv->file_list[file_idx].file_name,
		 file_path, 0);
#else
		fork_exec_repeat(SEPARATE1, "cp", "-a",
		 cur_fv->file_list[file_idx].file_name,
		 file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_copy_file_update(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	ret = input_string(get_other_filer_view()->cur_dir, file_path,
	 HISTORY_TYPE_IDX_DIR, _("Copy to (Update):"));

	if (ret < 0) {
		// do_copy_file_update -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_handler_sigint_called())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_repeat(SEPARATE1, "cp", "-aufv",
		 cur_fv->file_list[file_idx].file_name, file_path, 0);
#else
		fork_exec_repeat(SEPARATE1, "cp", "-a",
		 cur_fv->file_list[file_idx].file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_rename_file(void)
{
	char file_name[MAX_PATH_LEN+1];
	int ret;

	strlcpy__(file_name, cur_fv->file_list[cur_fv->cur_sel_idx].file_name, MAX_PATH_LEN);

	ret = input_string(file_name, file_name, HISTORY_TYPE_IDX_EXEC, _("Rename to:"));

	if (ret < 0) {
		// do_rename_file -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (fork_exec_once(SEPARATE1, PAUSE1, "mv", "-i",
	 cur_fv->file_list[cur_fv->cur_sel_idx].file_name, file_name, 0) == 0) {
		strlcpy__(cur_fv->next_file, file_name, MAX_PATH_LEN);
		filer_do_next = FILER_REFRESH_FORCED;
	}
	return 0;
}
int do_move_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	ret = input_string(get_other_filer_view()->cur_dir, file_path,
	 HISTORY_TYPE_IDX_DIR, _("Move to:"));

	if (ret < 0) {
		// do_move_file -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_handler_sigint_called())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_repeat(SEPARATE1, "mv", "-ufv",
		 cur_fv->file_list[file_idx].file_name, file_path, 0);
#else
		fork_exec_repeat(SEPARATE1, "mv",
		 cur_fv->file_list[file_idx].file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_trash_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Trash file %s ?"),
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Trash %d files ?"), files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			if (fork_exec_repeat(SEPARATE1, BETRASH,
			 cur_fv->file_list[file_idx].file_name, 0)) {
#ifndef	USE_BUSYBOX
				fork_exec_repeat(SEPARATE1, "rm", "-rv",
				 cur_fv->file_list[file_idx].file_name, 0);
#else
				fork_exec_repeat(SEPARATE1, "rm", "-r",
				 cur_fv->file_list[file_idx].file_name, 0);
#endif
			}
		}
		end_fork_exec_repeat();
	}
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_delete_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Delete file %s ?"),
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Delete %d files ?"), files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
#ifndef	USE_BUSYBOX
			fork_exec_repeat(SEPARATE1, "rm", "-rv",
			 cur_fv->file_list[file_idx].file_name, 0);
#else
			fork_exec_repeat(SEPARATE1, "rm", "-r",
			 cur_fv->file_list[file_idx].file_name, 0);
#endif
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_REFRESH_FORCED;
	}
	return 0;
}
int do_mark_to_delete_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Mark file %s to be Deleted later ?"),
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Mark %d files to be Deleted later ?"),
		 files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			if (fork_exec_repeat(SEPARATE1, BEMARKDEL,
			 cur_fv->file_list[file_idx].file_name, 0)) {
#ifndef	USE_BUSYBOX
				fork_exec_repeat(SEPARATE1, "chmod", "-v", "606",
				 cur_fv->file_list[file_idx].file_name, 0);
#else
				fork_exec_repeat(SEPARATE1, "chmod", "606",
				 cur_fv->file_list[file_idx].file_name, 0);
#endif
			}
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_REFRESH_FORCED;
	}
	return 0;
}
int do_size_zero_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Make size of file %s 0 ?"),
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Make size of %d files 0 ?"),
		 files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			if (fork_exec_repeat(SEPARATE1, BESIZE0,
			 cur_fv->file_list[file_idx].file_name, 0)) {
#ifndef	USE_BUSYBOX
				fork_exec_repeat(SEPARATE1, "chmod", "-v", "000",
				 cur_fv->file_list[file_idx].file_name, 0);
#else
				fork_exec_repeat(SEPARATE1, "chmod", "000",
				 cur_fv->file_list[file_idx].file_name, 0);
#endif
			}
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_REFRESH_FORCED;
	}
	return 0;
}
int do_find_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_path, HISTORY_TYPE_IDX_DIR, _("Find:"));

	if (ret < 0) {
		// do_find_file -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	strlcpy__(cur_fv->next_file, file_path, MAX_PATH_LEN);
	cur_fv->top_idx = 0;
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_make_directory(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_path, HISTORY_TYPE_IDX_DIR, _("Mkdir:"));

	if (ret < 0) {
		// do_make_directory -> FILER_ENTERED_DIR_PATH
		filer_do_next = FILER_ENTERED_DIR_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	fork_exec_once(SEPARATE1, PAUSE1, "mkdir", "-p", file_path, 0);
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_change_directory(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_path, HISTORY_TYPE_IDX_DIR, _("Chdir to:"));

	if (ret < 0) {
		// do_change_directory -> FILER_ENTERED_DIR_PATH
		filer_do_next = FILER_ENTERED_DIR_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	return filer_change_dir_parent(file_path);
}
int do_parent_directory(void)
{
	char chg_dir[MAX_PATH_LEN+1];

	if (filer_change_dir("..") == 0)
		return 0;
	strlcpy__(cur_fv->next_file, strip_one_dir(cur_fv->cur_dir, chg_dir), MAX_PATH_LEN);
	strlcpy__(cur_fv->cur_dir, chg_dir, MAX_PATH_LEN);
	return 1;
}
int do_beginning_directory(void)
{
	return filer_change_dir_if_not_yet(cur_filer_views->org_cur_dir);
}
int do_home_directory(void)
{
	return filer_change_dir_if_not_yet("~");
}
int do_root_directory(void)
{
	return filer_change_dir_if_not_yet("/");
}
int do_prev_directory(void)
{
	return filer_change_prev_dir();
}
int do_real_path(void)
{
	char chg_dir[MAX_PATH_LEN+1];

	return filer_change_dir_if_not_yet(get_cwd(chg_dir));
}
//-----------------------------------------------------------------------------
int do_select_file(void)
{
	int files_selected;

	cur_fv->file_list[cur_fv->cur_sel_idx].selected
	 = cur_fv->file_list[cur_fv->cur_sel_idx].selected ^ _FILE_SEL_MAN_;
	files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected"),
							_("%d files selected"),
							_("%d filess selected"),
							_("%d filesss selected"),
	 files_selected), files_selected);
	cur_fv->cur_sel_idx = MIN_MAX_(0, cur_fv->cur_sel_idx + 1, cur_fv->file_list_entries-1);
//	filer_do_next = FILER_UPDATE_SCREEN;
	return 0;
}
int do_select_no_file(void)
{
	int file_idx;

	for (file_idx = 0 ; file_idx < cur_fv->file_list_entries; file_idx++) {
		cur_fv->file_list[file_idx].selected = 0;
	}
	disp_status_bar_done(_("File selection cleared"));
//	filer_do_next = FILER_UPDATE_SCREEN;
	return 0;
}
int do_select_all_files(void)
{
	int file_idx;
	int files_selected;

	for (file_idx = 0 ; file_idx < cur_fv->file_list_entries; file_idx++) {
		if (strcmp(cur_fv->file_list[file_idx].file_name, ".") == 0
		 || strcmp(cur_fv->file_list[file_idx].file_name, "..") == 0)
			cur_fv->file_list[file_idx].selected = 0;
		else
			cur_fv->file_list[file_idx].selected
			 = cur_fv->file_list[file_idx].selected ^ _FILE_SEL_MAN_;
	}
	files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected"),
							_("%d files selected"),
							_("%d filess selected"),
							_("%d filesss selected"),
	 files_selected), files_selected);
//	filer_do_next = FILER_UPDATE_SCREEN;
	return 0;
}
int do_quit_filer(void)
{
	filer_do_next = FILER_QUIT;
	return 0;
}
int do_quit_home_dir(void)
{
	do_home_directory();
	do_quit_filer();
	return 0;
}
int do_filer_tog_show_dot_file(void)
{
	do_tog_show_dot_file();
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_filer_inc_show_file_info(void)
{
	inc_show_file_info();
	SHOW_MODE("File information", get_str_show_file_info());
//	filer_do_next = FILER_UPDATE_SCREEN;
	return 0;
}
int do_filer_inc_sort_by(void)
{
	inc_sort_by();
	SHOW_MODE("File sort", get_str_sort_by());
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_tog_filer_panes(void)
{
	tog_filer_panes();
	SHOW_MODE("Filer panes", get_str_filer_panes());
	filer_do_next = FILER_REFRESH_AUTO;
	return 0;
}
int do_filer_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	return 0;
}

int do_filer_display_color_pairs(void)
{
	display_color_pairs(0, 0);
	input_key_loop();
#ifdef ENABLE_DEBUG
	display_item_colors(0, 0);
	input_key_loop();
#endif // ENABLE_DEBUG
	filer_do_next = FILER_DO_NOTHING;
	return 0;
}

#ifdef ENABLE_HELP
int do_filer_splash(void)
{
	disp_splash(200);
	input_key_loop();
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
#endif // ENABLE_HELP

int do_filer_menu_0(void)
{
	filer_menu_n(-1);
	return 0;
}
int do_filer_menu_1(void)
{
	if (is_app_list_mode()) {
		filer_do_next = FILER_QUIT;
		return 0;
	}
	filer_menu_n(0);
	return 0;
}
int do_filer_menu_2(void)
{
	filer_menu_n(1);
	return 0;
}
int do_filer_menu_3(void)
{
	filer_menu_n(2);
	return 0;
}
int do_filer_menu_4(void)
{
	filer_menu_n(2);
	return 0;
}
int do_filer_menu_5(void)
{
	filer_menu_n(2);
	return 0;
}

//-----------------------------------------------------------------------------

PRIVATE int do_edit_file_(int recursive)
{
	int file_idx;

	if (if_dir_change_dir()) {
		return 0;
	}
	if (is_app_list_mode()) {
		// do_edit_file_ -> FILER_ENTERED_FILE
		filer_do_next = FILER_ENTERED_FILE;
		return 0;
	}
	clear_files_loaded();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (cur_fv->file_list[file_idx].selected
		 && S_ISREG(cur_fv->file_list[file_idx].st.st_mode)) {
			if (load_file_name_upp_low(cur_fv->file_list[file_idx].file_name,
			 TUL0, OOE0, WOE1, recursive) <= 0) {
				tio_beep();
			}
		}
	}
	if (get_files_loaded() == 0) {
		filer_do_next = FILER_REFRESH_FORCED;
	} else {
		disp_files_loaded();
		filer_do_next = FILER_LOADED_FILE;
	}
	unselect_all_files_auto(_FILE_SEL_MAN_ | _FILE_SEL_AUTO_);
	return 0;
}

PRIVATE int if_dir_change_dir(void)
{
	if (S_ISDIR(cur_fv->file_list[cur_fv->cur_sel_idx].st.st_mode)) {
		filer_change_dir(cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
		return 1;
	}
	return 0;
}
PRIVATE int filer_change_dir_if_not_yet(const char *dir)
{
	char buf[MAX_PATH_LEN+1];

	if (strcmp(cur_fv->cur_dir, get_abs_path(dir, buf)) == 0) {
		return filer_change_prev_dir();
	} else {
		return filer_change_dir(dir);
	}
}
PRIVATE int filer_change_prev_dir(void)
{
	if (strlen(cur_fv->prev_dir)) {
		return filer_change_dir(cur_fv->prev_dir);
	}
	return 0;
}
int filer_change_dir_parent(const char *dir)
{
	char chg_dir[MAX_PATH_LEN+1];

	while(filer_change_dir(dir)) {
		if (strcmp(dir, "/") == 0) {
			return -1;	// error
		}
		// If can not change to dir, try parent dir
		// /try/to/change/dir/file ==> /try/to/change/dir
		strip_one_dir(dir, chg_dir);
		dir = chg_dir;
	}
	return 0;	// changed
}
int filer_change_dir(const char *dir)
{
	char buf[MAX_PATH_LEN+1];
	char chg_dir[MAX_PATH_LEN+1];

	if (strcmp(dir, ".") == 0) {
		return 0;
	} else if (strcmp(dir, "..") == 0) {
		strlcpy__(cur_fv->next_file, strip_one_dir(cur_fv->cur_dir, chg_dir), MAX_PATH_LEN);
	} else if (strcmp(dir, "~") == 0) {
		strcpy__(cur_fv->next_file, "..");
		strlcpy__(chg_dir, get_abs_path("~", buf), MAX_PATH_LEN);
	} else if (dir[0] == '/') {
		// absolute path
		strcpy__(cur_fv->next_file, "..");
		strlcpy__(chg_dir, dir, MAX_PATH_LEN);
	} else {
		// relative path
		strcpy__(cur_fv->next_file, "..");
		cat_dir_and_file(chg_dir, MAX_PATH_LEN, cur_fv->cur_dir, dir);	// /dir1 ==> /dir1/dir2
	}
	normalize_full_path(chg_dir);
	if (is_dir_readable(chg_dir) == 0) {
		// We can't open this dir for some reason. Complain.
		disp_status_bar_err(_("Can not change current directory to [%s]: %s"),
		 shrink_str_scr_static(chg_dir), strerror(errno));
		filer_do_next = FILER_DO_NOTHING;
		return 1;
	}
	strlcpy__(cur_fv->prev_dir, cur_fv->cur_dir, MAX_PATH_LEN);
	change_cur_dir(chg_dir);
	strlcpy__(cur_fv->cur_dir, chg_dir, MAX_PATH_LEN);
	cur_fv->top_idx = 0;
///#ifdef ENABLE_HISTORY
///	update_history(HISTORY_TYPE_IDX_DIR, chg_dir);
///#endif // ENABLE_HISTORY
	filer_do_next = FILER_REFRESH_FORCED;
	disp_status_bar_done("Changed to directory [%s]", chg_dir);
	return 0;
}

#endif // ENABLE_FILER

// End of filercmd.c
