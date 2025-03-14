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
PRIVATE int change_cur_dir_from_history(const char *dir);
#endif // ENABLE_HISTORY

int goto_dir_in_string(const char *str)
{
	char buf_dir[MAX_PATH_LEN+1];
	if (check_to_change_dir_in_string(str, buf_dir)) {
		change_cur_dir_by_file_path_after_save(buf_dir, buf_get_file_path(get_epc_buf(), NULL));
#ifdef ENABLE_HISTORY
		update_history_dir_change(get_cur_filer_pane_view()->prev_dir,
		 get_cur_filer_pane_view()->cur_dir);
#endif // ENABLE_HISTORY
		return 1;
	}
	return 0;
}
int check_to_change_dir_in_string(const char *str, char* buf_dir)
{
	int changeable = 0;	// directory changeable
	char dir_save[MAX_PATH_LEN+1];
	get_full_path_of_cur_dir(dir_save);

	for (int field_idx = 0; field_idx < MAX_FILES_TO_TRY_TO_LOAD_IN_A_LINE; field_idx++) {
		if (get_n_th_file_line_col_from_str(str, field_idx, buf_dir, NULL, NULL) > 0) {
			// directory gotten
			if (try_to_chdir_parent(buf_dir)) {
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
			if (change_cur_dir_from_history(buf_dir)) {
				// directory changeable
				changeable = 1;
				goto changeable;
			}
		}
	}
#endif // ENABLE_HISTORY

changeable:;
	change_cur_dir(dir_save);
	return changeable;	// changeable
}

int try_to_chdir_parent(char* buf_dir)
{
	if (is_dir_readable(buf_dir)) {
		return 1;
	}
	strip_file_from_path(buf_dir, buf_dir);
	if (is_dir_readable(buf_dir)) {
		return 1;
	}
	return 0;
}

#ifdef ENABLE_HISTORY
PRIVATE int change_cur_dir_from_history(const char *dir)
{
	set_history_newest(HISTORY_TYPE_IDX_DIR);
	for ( ; ; ) {
		const char *history = get_history_older(HISTORY_TYPE_IDX_DIR);
		if (strlen_path(history) == 0) {
			break;
		}
		if (compare_file_path_from_tail(history, dir) == 0) {
			if (change_cur_dir_saving_prev_next(history)) {
				return 1;	// changeable
			}
		}
	}
	return 0;	// not changeable
}
#endif // ENABLE_HISTORY

int filer_change_dir_to_cur_sel(void)
{
	if (S_ISDIR(get_cur_fv_cur_file_ptr()->st.st_mode)) {
		if (filer_change_dir(get_cur_fv_cur_file_ptr()->file_name)) {
			return 1;		// OK
		}
	}
	return 0;			// error
}
int filer_change_dir_if_not_yet(char *dir)
{
	if (strcmp(get_cur_filer_pane_view()->cur_dir, dir) == 0) {
		return filer_change_dir_to_prev_dir();
	} else {
		return filer_change_dir(dir);
	}
}
int filer_change_dir_to_prev_dir(void)
{
	if (is_strlen_not_0(get_cur_filer_pane_view()->prev_dir)) {
		return filer_change_dir(get_cur_filer_pane_view()->prev_dir);
	}
	return 0;		// error
}

// If can not change dir, try parent dir
int filer_change_dir_parent(char *path)
{
	char dir[MAX_PATH_LEN+1];

	strlcpy__(dir, path, MAX_PATH_LEN);
	for ( ; ; ) {
		if (strcmp(dir, "/") == 0) {
			return 0;	// error
		}
		if (filer_change_dir(dir)) {
			break;
		}
		// If can not change dir, try parent dir
		// /try/to/change/dir/file ==> /try/to/change/dir
		// /try/to/change/dir      ==> /try/to/change
		strip_file_from_path(dir, dir);
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
	update_history_dir_change(get_cur_filer_pane_view()->prev_dir,
	 get_cur_filer_pane_view()->cur_dir);
#endif // ENABLE_HISTORY
	get_cur_filer_pane_view()->top_file_idx = 0;
	disp_status_bar_done(_("Changed current directory to [%s]"),
	 shrink_str_to_scr_static(get_cur_filer_pane_view()->cur_dir));
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 1;		// OK
}

int change_cur_dir_saving_prev_next(const char *dir)
{
	return change_cur_dir_saving_prev_next_dir(dir,
	 get_cur_filer_pane_view()->cur_dir,
	 get_cur_filer_pane_view()->prev_dir,
	 get_cur_filer_pane_view()->next_file);
}

#endif // ENABLE_FILER

// End of filerdir.c
