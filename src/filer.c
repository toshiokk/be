/**************************************************************************
 *   filer.c                                                              *
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

PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir);
PRIVATE int get_another_filer_pane_idx(int filer_pane_idx);

ef_do_next_t filer_do_next = EF_NONE;

PRIVATE int filer_main_loop(const char *dir, const char *filter, char *path_buf);
PRIVATE int check_filer_cur_dir();
PRIVATE void update_file_info_of_all_panes(int update_request);
PRIVATE void update_file_info(filer_view_t *fv, int update_request);

PRIVATE void disp_title_bar_filer(const char *dir, const char *filter,
 int cur_idx, int files_selected, int files_total);
PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane);

PRIVATE void disp_key_list_filer();

void set_cur_filer_panes(filer_panes_t *fps)
{
	cur_filer_panes = fps;
}
filer_panes_t* get_cur_filer_panes()
{
	return cur_filer_panes;
}
void init_filer_panes(filer_panes_t *fps, const char *cur_dir)
{
	set_cur_filer_panes(fps);
	set_filer_cur_pane_idx(0);
	for (int pane_idx = 0; pane_idx < FILER_PANES; pane_idx++) {
		// set initial value
		init_filer_view(get_fv_from_pane(pane_idx), cur_dir);
	}
}
void inherit_filer_panes(filer_panes_t *fps)
{
	filer_panes_t *prev_fps = get_cur_filer_panes();
	set_cur_filer_panes(fps);
	for (int pane_idx = 0; pane_idx < FILER_PANES; pane_idx++) {
		// set initial value
		init_filer_view(get_fv_from_pane(pane_idx),
		 prev_fps->filer_views[pane_idx].cur_dir);
		if (prev_fps) {
			FV_CUR_F_IDX(get_fv_from_pane(pane_idx)) =
			 prev_fps->filer_views[pane_idx].cur_file_idx;
			FV_CURS_Y(get_fv_from_pane(pane_idx)) =
			 prev_fps->filer_views[pane_idx].cursor_y;
		}
	}
}
void free_filer_panes()
{
	for (int pane_idx = 0; pane_idx < FILER_PANES; pane_idx++) {
		free_file_info_array(get_fv_from_pane(pane_idx));
	}
}
void copy_filer_panes_cur_dir(filer_panes_t *dest, filer_panes_t *src)
{
	for (int pane_idx = 0; pane_idx < FILER_PANES; pane_idx++) {
		strlcpy__(dest->filer_views[pane_idx].cur_dir,
		 src->filer_views[pane_idx].cur_dir, MAX_PATH_LEN);
	}
}
PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir)
{
	memset(fv, 0x00, sizeof(*fv));
	strlcpy__(fv->org_cur_dir, cur_dir, MAX_PATH_LEN);
	strlcpy__(fv->cur_dir, cur_dir, MAX_PATH_LEN);
	strcpy__(fv->filter, "");
	strcpy__(fv->listed_dir, "");
	fv->file_info_array = NULL;
	fv->file_info_entries = 0;
	FV_CUR_F_IDX(fv) = -1;
	FV_CURS_Y(fv) = 0;
	strcpy__(fv->prev_dir, "");
	strcpy__(fv->next_file, "");
}

#ifdef ENABLE_DEBUG
void dump_filer_view(filer_view_t *fv)
{
flf_dprintf("org_cur_dir: [%s]\n", fv->org_cur_dir);
flf_dprintf("cur_dir    : [%s]\n", fv->cur_dir);
flf_dprintf("filter     : [%s]\n", fv->filter);
flf_dprintf("listed_dir : [%s]\n", fv->listed_dir);
flf_dprintf("file_info_entries: %d\n", fv->file_info_entries);
flf_dprintf("FV_CUR_F_IDX: %d\n", FV_CUR_F_IDX(fv));
flf_dprintf("FV_CURS_Y   : %d\n", FV_CURS_Y(fv));
flf_dprintf("prev_dir : [%s]\n", fv->prev_dir);
flf_dprintf("next_file: [%s]\n", fv->next_file);
}
void dump_filer_view_cur_dir(filer_view_t *fv)
{
flf_dprintf("cur_dir    : [%s]\n", fv->cur_dir);
}
#endif // ENABLE_DEBUG

filer_view_t *get_fv_from_pane(int pane_idx)
{
	if (pane_idx < 0) {
		return get_fv_from_cur_pane();
	}
	return &(cur_filer_panes->filer_views[pane_idx]);
}
filer_view_t *get_fv_from_cur_pane()
{
	return get_fv_from_pane(get_filer_cur_pane_idx());
}
filer_view_t *get_fv_from_other_pane()
{
	return get_fv_from_pane(get_another_filer_pane_idx(get_filer_cur_pane_idx()));
}
PRIVATE int get_another_filer_pane_idx(int filer_pane_idx)
{
	return filer_pane_idx == 0 ? 1 : 0;
}

file_info_t *get_cur_fv_file_info_array()
{
	return get_fv_from_cur_pane()->file_info_array;
}
int get_cur_fv_file_info_entries()
{
	return get_fv_from_cur_pane()->file_info_entries;
}
const char *get_cur_fv_cur_file_name()
{
	return get_cur_fv_file_name(get_cur_fv_file_idx());
}
const char *get_cur_fv_file_name(int file_idx)
{
	return (&(get_cur_fv_file_info_array()[file_idx]))->file_name;
}
file_info_t *get_cur_fv_cur_file_info()
{
	return get_cur_fv_file_info(get_cur_fv_file_idx());
}
file_info_t *get_cur_fv_file_info(int file_idx)
{
	return &(get_cur_fv_file_info_array()[file_idx]);
}
file_info_t *get_cur_fv_file_ptr_from_pane(int pane_idx)
{
	update_file_info(get_fv_from_pane(pane_idx), 0);		// get if not yet gotten
	return &(get_fv_from_pane(pane_idx)->file_info_array[
	 FV_CUR_F_IDX(get_fv_from_pane(pane_idx))]);
}
int get_cur_fv_file_idx()
{
	return FV_CUR_F_IDX(get_fv_from_cur_pane());
}

//------------------------------------------------------------------------------

int do_call_filer(int push_win, int list_mode, const char *dir, const char *filter,
 char *path_buf)
{
flf_dprintf("push: %d, list: %d, dir: %s, filter: [%s]\n", push_win, list_mode, dir, filter);
#ifdef ENABLE_HISTORY
	save_histories_if_modified();
#endif // ENABLE_HISTORY
	strcpy__(path_buf, "");

	editor_panes_t next_eps;
	filer_panes_t next_fps;
	if (push_win) {
		push_app_stack(&next_eps, NULL, &next_fps);
	}

	SET_APPMD_VAL(app_EDITOR_FILER, EF_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);

	flf_dprintf("GET_APPMD(app_EDITOR_FILER): %d\n", GET_APPMD(app_EDITOR_FILER));
	flf_dprintf("[[[[ push_win:%d, list_mode:%d\n", push_win, list_mode);

	int ret = filer_main_loop(dir, filter, path_buf);

	flf_dprintf("]]]] push_win:%d, list_mode:%d --> app_stk: %d, ret__[%s]\n",
	 push_win, list_mode, get_app_stack_depth(), get_ef_name(ret));

	if (push_win) {
		// editor: refrect from the callee's cur-buf to the caller's cur-buf if file loaded
		// filer : propagate the current directory only when called as a normal-mode
		pop_app_stack(ret == EF_LOADED_RET_TO_EDITOR,
		 (list_mode == 0) || (ret == EF_CHDIR_RET_TO_FILER));
flf_dprintf("cur-dir: [%s]\n", get_full_path_of_cur_dir(NULL));
flf_dprintf("[0].cur_dir: [%s]\n", get_fv_from_pane(0)->cur_dir);
flf_dprintf("[1].cur_dir: [%s]\n", get_fv_from_pane(1)->cur_dir);
		update_screen_app(S_B_CURS, 1);
	}

	return ret;		// EF_...
}

//------------------------------------------------------------------------------

PRIVATE int filer_main_loop(const char *dir, const char *filter, char *path_buf)
{
flf_dprintf("dir: [%s], filter: [%s], path: [%s]\n", dir, filter, path_buf);
	if (filter) {
		strlcpy__(get_fv_from_cur_pane()->filter, filter, MAX_PATH_LEN);
	}
	if (path_buf) {
		strcpy__(path_buf, "");
	}

	if (is_strlen_not_0(dir)) {
		chdir_saving_prev_next(dir);
	}
#ifdef ENABLE_HISTORY
	dir_history_update(get_fv_from_cur_pane()->cur_dir);
	dir_history_fix();
#endif // ENABLE_HISTORY

	key_code_t key_input = K_VALID;
	SET_filer_do_next(FL_UPDATE_FORCE);

	// Main input loop
	for ( ; ; ) {
		check_filer_cur_dir();
#ifdef ENABLE_HISTORY
		dir_history_update(get_fv_from_cur_pane()->cur_dir);
#endif // ENABLE_HISTORY
		update_file_info_of_all_panes(filer_do_next);
_FLF_
		update_screen_app((IS_KEY_VALID(key_input) == 0) ? S_B_AUTO : S_B_CURS, 1);
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		SET_filer_do_next(FL_UPDATE_AUTO);
		strcpy__(get_fv_from_cur_pane()->next_file, "");
		if (IS_KEY_VALID(key_input)) {
			clear_status_bar_displayed();
			// some key input
			hmflf_dprintf("input%ckey:0x%04x([%s]|[%s])========================================\n",
			 '_', (UINT16)key_input,
			 long_key_name_from_key_code(key_input, NULL),
			 short_key_name_from_key_code(key_input, NULL));
			SET_filer_do_next(EF_NONE);
			func_key_t *func_key;
			if ((func_key = get_fkey_entry_from_key(filer_func_key_table, key_input, -1))
			 == NULL) {
				func_key = get_fkey_entry_from_key(filer_func_key_table, tolower__(key_input),
				 -1);
			}
			if (func_key == NULL) {
				disp_status_bar_warn(_("No command assigned for the key: 0x%04x(%s)"),
				 (UINT16)key_input, long_key_name_from_key_code(key_input, NULL));
			} else {
				strlcpy__(get_fv_from_cur_pane()->next_file,
				 get_cur_fv_cur_file_name(), MAX_PATH_LEN);
				if (is_app_chooser_viewer_mode()) {
					switch (func_key->list_mode) {
					case EFNM:		// not executable in List mode
						disp_status_bar_done(
						 _("Can not execute this function in filer List mode: [%s]"),
						 func_key->func_id);
						SET_filer_do_next(FL_UPDATE_FORCE);
						break;
					case EFLM:		// executable in editor List mode
					case EFAM:
					default:
						break;
					}
				}
				if (filer_do_next == EF_NONE) {
					flf_dprintf("[[ CALL_FUNC_FILER [%s]\n", func_key->func_id);
					//=========================
					(*func_key->func)();	// call function "dof_...()"
					//=========================
					flf_dprintf("]] filer_do_next_[%s]\n", get_ef_name(filer_do_next));
					unselect_all_files_auto(_FILE_SEL_AUTO_);
				}
			}
		}
		// check the conditions for exiting filer
		switch (filer_do_next) {
		case EF_CHDIR_RET_TO_FILER:
		case EF_EXECUTED_RET_TO_CALLER:
			// return to the root editor/caller
			if (get_app_stack_depth()) {
				break;						// exit from filer
			}
			SET_filer_do_next(EF_NONE);		// not exit
			break;
		case EF_LOADED_RET_TO_EDITOR:
			// always return to editor since it's filer
		default:
			break;							// exit from filer
		}
		if (filer_do_next >= EF_TO_QUIT) {
			break;
		}
		sync_cut_buffers_and_histories();
	}
#ifdef ENABLE_HISTORY
	dir_history_fix();
#endif // ENABLE_HISTORY

	switch (filer_do_next) {
	// only list mode
	case FL_ENTER_FILE_NAME:
	case FL_ENTER_FILE_NAME_ADD:
	case FL_ENTER_FILE_PATH:
	case FL_ENTER_FILE_PATH_ADD:
		strcpy__(path_buf, "");
		for (int file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if ((filer_do_next == FL_ENTER_FILE_NAME)
			 || (filer_do_next == FL_ENTER_FILE_NAME_ADD)) {
				// enter file names: file-1 "file 2" "file 3"
				concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN,
				 get_cur_fv_file_name(file_idx));
			} else /* if (filer_do_next == FL_ENTER_FILE_PATH) */ {
				// enter file paths: /path/to/file-1 "/path/to/file 2" "/path/to/file 3"
				char path[MAX_PATH_LEN];
				concat_dir_and_file(path,
				 get_fv_from_cur_pane()->cur_dir, get_cur_fv_file_name(file_idx));
				concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN, path);
			}
		}
		if ((filer_do_next == FL_ENTER_FILE_NAME)
		 || (filer_do_next == FL_ENTER_FILE_PATH)) {
			SET_filer_do_next(EF_ENTER_STRING);
		} else {
			SET_filer_do_next(EF_ENTER_STRING_ADD);
		}
		break;
	// only list mode
	case FL_ENTER_DIR_PATH:
	case FL_ENTER_DIR_PATH_ADD:
		strlcpy__(path_buf, get_fv_from_cur_pane()->cur_dir, MAX_PATH_LEN);
		add_trailing_slash_in_handling(path_buf);
		if (filer_do_next == FL_ENTER_DIR_PATH) {
			SET_filer_do_next(EF_ENTER_STRING);
		} else {
			SET_filer_do_next(EF_ENTER_STRING_ADD);
		}
		break;
	default:
		break;							// exit from filer
	}
	// | command modifier key | replace/add string            | return value        |
	// |----------------------|-------------------------------|---------------------|
	// | none                 | replacing input file/dir name | EF_ENTER_STRING     |
	// | ALT                  | adding    input file/dir name | EF_ENTER_STRING_ADD |
	if (filer_do_next == EF_ENTER_STRING) {
		SET_filer_do_next((IS_META_KEY(key_input) == 0)
		 ? EF_ENTER_STRING			// Replace input file/dir name
		 : EF_ENTER_STRING_ADD);	// Append input file/dir name
	}

	int ret = filer_do_next;
	if (filer_do_next == EF_EXECUTED_RET_TO_CALLER) {
		SET_filer_do_next(EF_NONE);
	}
	return ret;
} // filer_main_loop

PRIVATE int check_filer_cur_dir()
{
	// check if cur_dir is readable
	if (is_dir_readable(get_fv_from_cur_pane()->cur_dir) == 0) {
		// current directory is not readable or disappeared
		while (is_dir_readable(get_fv_from_cur_pane()->cur_dir) == 0) {
			// go up to the root dir
			dof_parent_directory();
		}
		tio_beep();
	}
	if (is_dir_readable(get_fv_from_cur_pane()->cur_dir) == 0) {
		get_full_path_of_cur_dir(get_fv_from_cur_pane()->cur_dir);
	}
	change_cur_dir(get_fv_from_cur_pane()->cur_dir);
	return 0;
}

PRIVATE void update_file_info_of_all_panes(int update_request)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		update_file_info(get_fv_from_cur_pane(), update_request);
	} else {
		update_file_info(get_fv_from_pane(0), update_request);
		update_file_info(get_fv_from_pane(1), update_request);
	}
}
// the occasions file list to be updated:
//  - not yet file list gotten
//  - current directory have changed
//  - command requests updating file list (force update)
//  - automatic periodic update of file list
//    the occasion file list update disabled:
//    - when some files are selected (for file processing e.g. copy, move, delete, ...)
PRIVATE void update_file_info(filer_view_t *fv, int update_request)
{
	int cur_dir_changed = compare_dir_path_w_or_wo_trailing_slash(fv->listed_dir, fv->cur_dir);
	if ((fv->file_info_array == NULL)
	 || cur_dir_changed
	 || ((update_request == FL_UPDATE_AUTO) && (get_files_selected(fv) == 0))
	 || (update_request == FL_UPDATE_FORCE)) {
		if ((update_request < FL_UPDATE_FORCE) && (fv->file_info_array != NULL)) {
			strlcpy__(fv->next_file, fv->file_info_array[FV_CUR_F_IDX(fv)].file_name,
			 MAX_PATH_LEN);
		} else {
			// 'fv->next_file' has set by the requesters
		}
		read_file_info_array(fv);
		sort_file_info_array(fv);
		research_file_name_in_file_info_array(fv, fv->next_file, cur_dir_changed);
	}
}

PRIVATE void disp_status_bar_filer(s_b_d_t status_bar);

int update_screen_filer(s_b_d_t status_bar, int refresh)
{
	win_select_cur_sub_win(WIN_IDX_CENTRAL);
	tio_set_cursor_on(0);

	// title bar
	disp_title_bar_filer(get_fv_from_cur_pane()->cur_dir, get_fv_from_cur_pane()->filter,
	 get_cur_fv_file_idx(), get_files_selected_cfv(), get_cur_fv_file_info_entries());

	// status bar
	if (status_bar) {
		disp_status_bar_filer(status_bar);
	} else {
		redisp_status_bar();
	}
	// key list
	disp_key_list_filer();

	if (GET_APPMD(fl_FILER_PANES) == 0) {		// 1 pane
		win_select_cur_sub_win(WIN_IDX_SUB_WHOLE);
		disp_file_list(get_fv_from_cur_pane(), 1);
	} else {									// 2 panes
		for (int pane_sel_idx = 0; pane_sel_idx < FILER_PANES; pane_sel_idx++) {
			// pane_sel_idx=0: update not current pane
			// pane_sel_idx=1: update current pane
			int pane_idx = (pane_sel_idx == 0)
			 ? get_filer_another_pane_idx()		// not current pane
			 : get_filer_cur_pane_idx();		// current pane
			win_select_cur_sub_win(WIN_IDX_SUB_LEFT + pane_idx);
			disp_file_list(get_fv_from_pane(pane_idx), pane_sel_idx);
		}
	}

	tio_set_cursor_on(1);
	if (refresh) {
		tio_refresh();
	}

	return 0;
}

#define HHCMMCSS_BUF_LEN		8	// "23:59:59" / "24/10/09"
PRIVATE void disp_title_bar_filer(const char *dir, const char *filter,
 int cur_idx, int files_selected, int files_total)
{
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_files[SHORT_STR_BUF_LEN+1];
	char buf_bufs[SHORT_STR_BUF_LEN+1];
	char buf_time[HHCMMCSS_BUF_LEN+1];
	char buf_files_bufs_time[SHORT_STR_BUF_LEN+1];
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	// directory path
	char separator_char = indication_of_app_mode();
	snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN, "%s%d%c%s%s",
	 root_notation(), get_filer_cur_pane_idx()+1, separator_char, dir, filter);
	if ((get_win_depth() == 0)
	 && (compare_dir_path_w_or_wo_trailing_slash(dir, get_home_dir()) == 0)) {
		// when in home dir, show host and tty name
		strcat_printf(buf_path, MAX_SCRN_LINE_BUF_LEN, "  [%s|%s]",
		 get_at_host_name(), get_tty_name());
	}

	// current-index/files or files-selected/files
	snprintf_(buf_files, SHORT_STR_BUF_LEN+1, "%d/%d",
	 (files_selected == 0) ? cur_idx : files_selected, files_total);

	// edit buffers
	int edit_bufs = count_edit_bufs();
	snprintf_(buf_bufs, SHORT_STR_BUF_LEN, "e%s", zz_from_num(edit_bufs, NULL));

	// current date / time
	snprintf_(buf_time, HHCMMCSS_BUF_LEN+1, "%s", cur_ctime_cdate(msec_past_input_key() < 1000));

	if (edit_bufs == 0) {
		snprintf_(buf_files_bufs_time, SHORT_STR_BUF_LEN+1, " %s %c %s",
		 buf_files, *get_str_file_sort_mode(), buf_time);
	} else {
		snprintf_(buf_files_bufs_time, SHORT_STR_BUF_LEN+1, " %s %c %s %s",
		 buf_files, *get_str_file_sort_mode(), buf_bufs, buf_time);
	}

	int path_cols = LIM_MIN(0, central_win_get_columns() - strlen_path(buf_files_bufs_time));
	shrink_str__adjust_col(buf_path, path_cols, 4);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_path, buf_files_bufs_time);

	set_title_bar_color_by_state(
	 is_app_chooser_viewer_mode() ? ITEM_COLOR_IDX_ERROR : ITEM_COLOR_IDX_TITLE, 0);
	central_win_output_string(central_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);
}

PRIVATE void disp_status_bar_filer(s_b_d_t status_bar)
{
	disp_status_bar_type(status_bar, "%s", file_info_str(get_cur_fv_cur_file_info(), 0, 1, 0));
}

PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane)
{
	if (cur_pane == 0) {
		set_work_space_color_dark();
	} else {
		clear_work_space_color_dark();
	}
	set_item_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_screen();

	if (filer_win_get_file_path_lines()) {
		char buffer[MAX_SCRN_LINE_BUF_LEN+1];
		// on two pane mode, show each directory path
		strlcpy__(buffer, fv->cur_dir, MAX_SCRN_LINE_BUF_LEN);
		shrink_str__adjust_col(buffer, central_win_get_columns(), 2);
		// show non current pane in reverse video
		if (cur_pane) {
			set_item_color_by_idx(ITEM_COLOR_IDX_TITLE, 0);
		}
		sub_win_output_string(filer_win_get_file_path_y(), 0, buffer, -1);
	}

	int cur_file_idx = FV_CUR_F_IDX(fv);
	int file_idx = LIM_MIN(0, cur_file_idx - FV_CURS_Y(fv));
	for (int line_idx = 0;
	 line_idx < filer_win_get_file_list_lines() && file_idx < fv->file_info_entries;
	 line_idx++, file_idx++) {
		// Highlight the currently selected file/dir.
		const char *ptr = file_info_str(&(fv->file_info_array[file_idx]),
		 1, file_idx != cur_file_idx, fv->file_info_array[file_idx].selected);
		if (cur_pane && (file_idx == cur_file_idx)) {
			// show current line in reverse video
			set_item_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 1);
		} else {
			if (fv->file_info_array[file_idx].selected) {
				set_item_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
			} else {
#define STRIPE_LINES	4
				if (GET_APPMD(fl_SHOW_ZEBRA_STRIPE)
				 && (file_idx % (STRIPE_LINES*2)) >= STRIPE_LINES) {
					set_item_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL2, 0);
				} else {
					set_item_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
				}
			}
		}
		sub_win_output_string(filer_win_get_file_list_y() + line_idx, 0, ptr, -1);
		if (cur_pane && (file_idx == cur_file_idx)) {
			tio_set_attr_rev(0);
		}
	}
	if (cur_pane) {
		// Set cursor position
		sub_win_set_cursor_pos(filer_win_get_file_list_y() + FV_CURS_Y(fv), 1);
		win_save_cursor_y();
	}
	return 0;
}
//-------------------------------------
#define FILER_VERT_SCROLL_MARGIN	VERT_SCROLL_MARGIN
int filer_vert_scroll_margin_lines()
{
	return LIM_MAX(FILER_VERT_SCROLL_MARGIN, filer_win_get_file_list_lines() / 3);	// [0, 3]
}
int filer_vert_scroll_lines()
{
#ifdef LIMIT_MAX_VERT_SCROLL_LINES
	return MIN_MAX_(1, filer_win_get_file_list_lines()-1 - filer_vert_scroll_margin_lines() * 2,
	 LIMIT_MAX_VERT_SCROLL_LINES);
#else // LIMIT_MAX_VERT_SCROLL_LINES
	return LIM_MIN(1, filer_win_get_file_list_lines()-1 - filer_vert_scroll_margin_lines() * 2);
#endif // LIMIT_MAX_VERT_SCROLL_LINES
}
//-------------------------------------
int filer_win_get_file_path_lines()
{
	return GET_APPMD(fl_FILER_PANES);
}
int filer_win_get_file_list_lines()
{
	return LIM_MIN(0, sub_win_get_lines() - filer_win_get_file_path_lines());
}
//-------------------------------------
int filer_win_get_file_path_y()
{
	return 0;
}
int filer_win_get_file_list_y()
{
	return filer_win_get_file_path_y() + filer_win_get_file_path_lines();
}
//-------------------------------------
PRIVATE void disp_key_list_filer()
{
	disp_fkey_list();

	const char *filer_keys_normal_mode[] = {
	 "<dof_quit>Quit "
	 "<dof_open_file_recursive>Edit "
	 "<dof_open_new_file>NewFile "
	 "<dof_copy_file>Copy "
	 "<dof_copy_file_update>CopyUpdate "
	 "<dof_move_file>Move "
	 "<dof_move_file_update>MoveUpdate "
	 "<dof_rename_file>Rename "
	 "<dof_trash_file>Trash "
	 "<dof_delete_file>Delete "
	 "<dof_mark_to_delete_file>MarkToDel "
	 "<dof_exec_command_with_file>Exec "
	 "<dof_run_command_rel>Run ",
	 "<dof_select_file>SelFile "
	 "<dof_select_all_files>SelAll "
	 "<dof_select_no_file>SelNone "
	 "<dof_home_directory>HomeDir "
	 "<dof_root_directory>RootDir "
	 "<dof_change_directory>ChgDir "
	 "<dof_parent_directory>ParentDir "
	 "<dof_tap_file>Pager/EnterDir "
	 "<dof_make_directory>MkDir "
	 "<dof_tog_panes>TwoPane "
	 "<dof_tog_panex>SwPane "
	 "<dof_inc_file_sort_mode>Sort "
	 "<dof_inc_file_view_mode>Info "
	 "<dof_refresh_filer>Refresh "
	 "<dof_tog_show_dot_file>ShowDotFile "
	 "<dof_find_file>FindFile "
	};
	const char *filer_keys_list_mode[] = {
	 "<dof_enter_file_name>Enter FN "
	 "<dof_enter_file_name_add>Enter FN(add) "
	 "<dof_enter_file_path>Enter file path "
	 "<dof_enter_file_path_add>Enter file path(add) ",
	 "<dof_enter_dir_path>Enter DIR "
	 "<dof_enter_dir_path_add>Enter DIR(add) "
	 "<dof_tap_file_to_enter>Enter FN "
	 "<dof_tap_file_to_enter_add>Enter FN(add) "
	};
	disp_key_list_lines(is_app_chooser_viewer_mode() == 0
	 ? filer_keys_normal_mode : filer_keys_list_mode);
}

#endif // ENABLE_FILER

// End of filer.c
