/**************************************************************************
 *   filerdir.c                                                           *
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

#ifdef ENABLE_HISTORY
PRIVATE int chdir_from_dir_history(const char *dir);
PRIVATE int chdir_from_file_history(const char *dir);
#endif // ENABLE_HISTORY

int try_to_chdir_in_string(const char *str, char *buf_dir)
{
	int changeable = 0;	// directory changeable
	for (int field_idx = 0; field_idx < MAX_FILES_TO_TRY_TO_LOAD_IN_A_LINE; field_idx++) {
		if (get_n_th_file_line_col_from_str(str, field_idx, buf_dir, NULL, NULL) > 0) {
			// directory gotten
			if (chdir_there_or_parent(buf_dir)) {
				// directory changeable
				changeable = 1;
				goto changeable;
			}
		}
	}
#ifdef ENABLE_HISTORY
	for (int field_idx = 0; field_idx < MAX_FILES_TO_TRY_TO_LOAD_IN_A_LINE; field_idx++) {
		if (get_n_th_file_line_col_from_str(str, field_idx, buf_dir, NULL, NULL) > 0) {
			// directory gotten
			if (chdir_from_dir_history(buf_dir) || chdir_from_file_history(buf_dir)) {
				// directory changeable
				changeable = 1;
				goto changeable;
			}
		}
	}
#endif // ENABLE_HISTORY

changeable:;
	return changeable;	// changeable
}

int chdir_there_or_parent(char *buf_dir)
{
	if (is_dir_readable(buf_dir)) {
		return 1;
	}
	strip_file_from_path(buf_dir, buf_dir);
flf_dprintf("buf_dir: [%s]\n", buf_dir);
	if (is_dir_readable(buf_dir)) {
		return 1;
	}
	return 0;
}

#ifdef ENABLE_HISTORY
PRIVATE int chdir_from_dir_history(const char *dir)
{
	set_history_newest(HISTORY_TYPE_IDX_DIR);
	for ( ; ; ) {
		const char *history = get_history_older(HISTORY_TYPE_IDX_DIR);
		if (is_strlen_0(history)) {
			break;
		}
		if (compare_file_path_from_tail(history, dir) == 0) {
			if (chdir_saving_prev_next(history)) {
				return 1;	// changed
			}
		}
	}
	return 0;				// not changed
}
PRIVATE int chdir_from_file_history(const char *dir)
{
	set_history_newest(HISTORY_TYPE_IDX_FILE);
	for ( ; ; ) {
		const char *history = get_history_older(HISTORY_TYPE_IDX_FILE);
		if (is_strlen_0(history)) {
			break;
		}
		char dir_in_list[MAX_PATH_LEN+1];
		get_file_line_col_from_str(history, dir_in_list, NULL, NULL);
		if (compare_file_path_from_tail(dir_in_list, dir) == 0) {
			if (chdir_saving_prev_next(dir_in_list)) {
				return 1;	// changed
			}
		}
	}
	return 0;		// no directory found nor changeable
}
#endif // ENABLE_HISTORY

int filer_chdir_to_cur_sel()
{
	if (S_ISDIR(get_cur_fv_cur_file_info()->st.st_mode)) {
		if (filer_chdir(get_cur_fv_cur_file_name())) {
			return 1;		// OK
		}
	}
	return 0;			// error
}
int filer_chdir_if_not_yet(const char *dir)
{
	if (compare_dir_path_w_or_wo_trailing_slash(get_fv_from_cur_pane()->cur_dir, dir) == 0) {
		return filer_chdir_to_prev_dir();
	} else {
		return filer_chdir(dir);
	}
}
int filer_chdir_to_prev_dir()
{
	if (is_strlen_not_0(get_fv_from_cur_pane()->prev_dir)) {
		return filer_chdir(get_fv_from_cur_pane()->prev_dir);
	}
	disp_status_bar_err(_("No previous directory [%s]"), get_fv_from_cur_pane()->prev_dir);
	return 0;		// error
}

// If can not change dir, try parent dir
int filer_chdir_parent(const char *path)
{
	char dir[MAX_PATH_LEN+1];
	strlcpy__(dir, path, MAX_PATH_LEN);
	for ( ; ; ) {
		if (is_strlen_0(dir) || (compare_dir_path_w_or_wo_trailing_slash(dir, "/") == 0)) {
			return 0;	// error
		}
		if (filer_chdir(dir)) {
			break;
		}
		// If can not change dir, try parent dir
		// /try/to/change/dir/file ==> /try/to/change/dir
		// /try/to/change/dir      ==> /try/to/change
		strip_file_from_path(dir, dir);
	}
	return 1;	// changed
}
int filer_chdir(const char *dir)
{
	if (chdir_saving_prev_next(dir) == 0) {
		// We can't open this dir for some reason. Complain.
		disp_status_bar_err(_("Can not change current to [%s]: %s"),
		 shrink_str_to_scr_static(dir), strerror(errno));
		return 0;	// error
	}
	disp_status_bar_done(_("Chdir [%s]"),
	 shrink_str_to_scr_static(get_real_path_of_cur_dir(NULL)));
	SET_filer_do_next(FL_UPDATE_FORCE);
	return 1;		// OK
}

int chdir_saving_prev_next(const char *dir)
{
	return chdir_saving_prev_next_dir(dir,
	 get_fv_from_cur_pane()->cur_dir,
	 get_fv_from_cur_pane()->prev_dir,
	 get_fv_from_cur_pane()->next_file);
}

#endif // ENABLE_FILER

// End of filerdir.c
