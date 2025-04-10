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
PRIVATE int check_filer_cur_dir(void);
PRIVATE int update_file_infos_of_all_panes(int update_request);
PRIVATE int update_file_infos(filer_view_t *fv, int update_request);

PRIVATE void disp_title_bar_filer(const char* dir, const char* filter,
 int cur_idx, int files_selected, int files_total);
PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane);

PRIVATE void disp_key_list_filer(void);

void set_cur_filer_panes(filer_panes_t *fps)
{
	cur_filer_panes = fps;
}
filer_panes_t* get_cur_filer_panes()
{
	return cur_filer_panes;
}
void init_cur_filer_panes(filer_panes_t *fps, const char *cur_dir)
{
	filer_panes_t *prev_fps = get_cur_filer_panes();
	set_cur_filer_panes(fps);
	set_filer_cur_pane_idx(0);
	for (int filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		// set initial value
		init_filer_view(get_cur_filer_view(filer_pane_idx), cur_dir);
		if (prev_fps) {
			get_cur_filer_view(filer_pane_idx)->cur_file_idx =
			 prev_fps->filer_views[filer_pane_idx].cur_file_idx;
		}
	}
}
void destroy_filer_panes()
{
	for (int filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		free_file_infos(get_cur_filer_view(filer_pane_idx));
	}
}
void copy_filer_panes_cur_dir(filer_panes_t *dest, filer_panes_t *src)
{
	for (int filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		strlcpy__(dest->filer_views[filer_pane_idx].cur_dir,
		 src->filer_views[filer_pane_idx].cur_dir, MAX_PATH_LEN);
	}
}
PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir)
{
	memset(fv, 0x00, sizeof(*fv));
	strlcpy__(fv->org_cur_dir, cur_dir, MAX_PATH_LEN);
	strlcpy__(fv->cur_dir, cur_dir, MAX_PATH_LEN);
	strcpy__(fv->filter, "");
	strcpy__(fv->listed_dir, "");
	fv->file_infos_entries = 0;
	fv->file_infos = NULL;
	fv->prev_file_idx = 0;
	fv->cur_file_idx = -1;
	fv->top_file_idx = 0;
	strcpy__(fv->prev_dir, "");
	strcpy__(fv->next_file, "");
}
filer_view_t *get_cur_filer_view(int pane_idx)
{
	if (pane_idx < 0) {
		return get_cur_filer_pane_view();
	}
	return &(cur_filer_panes->filer_views[pane_idx]);
}
filer_view_t *get_cur_filer_pane_view(void)
{
	return get_cur_filer_view(get_filer_cur_pane_idx());
}
filer_view_t *get_another_filer_pane_view(void)
{
	return get_cur_filer_view(get_another_filer_pane_idx(get_filer_cur_pane_idx()));
}
PRIVATE int get_another_filer_pane_idx(int filer_pane_idx)
{
	return filer_pane_idx == 0 ? 1 : 0;
}

file_info_t *get_cur_fv_file_infos()
{
	return get_cur_filer_pane_view()->file_infos;
}
file_info_t *get_cur_fv_cur_file_ptr()
{
	return get_cur_fv_file_ptr(get_cur_fv_file_idx());
}
file_info_t *get_cur_fv_file_ptr(int file_idx)
{
	return &(get_cur_fv_file_infos()[file_idx]);
}
file_info_t *get_fv_file_ptr(int pane_idx)
{
	update_file_infos(get_cur_filer_view(pane_idx), 0);		// get if not yet gotten
	return &(get_cur_filer_view(pane_idx)->file_infos[
	 get_cur_filer_view(pane_idx)->cur_file_idx]);
}
int get_cur_fv_file_idx()
{
	return get_cur_filer_pane_view()->cur_file_idx;
}
void set_cur_fv_file_idx(int file_idx)
{
	get_cur_filer_pane_view()->cur_file_idx = file_idx;
}

//------------------------------------------------------------------------------

int do_call_filer(int push_win, int list_mode, const char *dir, const char *filter,
 char *path_buf)
{
flf_d_printf("push: %d, list: %d, dir: %s, filter: [%s]\n", push_win, list_mode, dir, filter);
#ifdef ENABLE_HISTORY
	save_histories();
#endif // ENABLE_HISTORY
	strcpy__(path_buf, "");

	editor_panes_t next_eps;
	filer_panes_t next_fps;
	if (push_win) {
		push_app_stack(&next_eps, NULL, &next_fps);
	}

	SET_APPMD_VAL(app_EDITOR_FILER, EF_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);

flf_d_printf("GET_APPMD(app_EDITOR_FILER): %d\n", GET_APPMD(app_EDITOR_FILER));
flf_d_printf("push_win:%d, list_mode:%d\n", push_win, list_mode);
flf_d_printf("<<<<<<<<<<<<<<<<<<<<<<<<<\n");

	int ret = filer_main_loop(dir, filter, path_buf);

flf_d_printf(">>>>>>>>>>>>>>>>>>>>>>>>>\n");
flf_d_printf("push_win:%d, list_mode:%d --> ret: %d\n", push_win, list_mode, ret);

	if (push_win) {
		// editor: refrect the callee's cur-buf to the caller's cur-buf if file loaded additionaly
		// filer : propagate the current directory only when called as a normal-mode
		pop_app_stack(ret == EF_LOADED, list_mode == 0);
		disp_status_bar_done(_("Cur dir changed: %s"),
		 get_cur_filer_view(get_filer_cur_pane_idx())->cur_dir);

flf_d_printf("cur-dir: [%s]\n", full_path_of_cur_dir_s());
flf_d_printf("[0].cur_dir: [%s]\n", get_cur_filer_view(0)->cur_dir);
flf_d_printf("[1].cur_dir: [%s]\n", get_cur_filer_view(1)->cur_dir);
		update_screen_app(1, 1);
	}

	filer_do_next = EF_NONE;	// for caller of do_call_filer(), clear "filer_do_next"

///	_D(disp_status_bar_cwd())
	return ret;
}

//------------------------------------------------------------------------------

PRIVATE int filer_main_loop(const char *dir, const char *filter, char *path_buf)
{
flf_d_printf("dir: [%s], filter: [%s], path: [%s], len: %d\n", dir, filter, path_buf);
	if (filter) {
		strlcpy__(get_cur_filer_pane_view()->filter, filter, MAX_PATH_LEN);
	}
	if (path_buf) {
		strcpy__(path_buf, "");
	}
#ifdef ENABLE_HISTORY
	char prev_cur_dir[MAX_PATH_LEN+1];
#endif // ENABLE_HISTORY

#ifdef ENABLE_HISTORY
	get_full_path_of_cur_dir(prev_cur_dir);		// memorize prev. current dir
#endif // ENABLE_HISTORY
	if (is_strlen_not_0(dir)) {
		change_cur_dir_saving_prev_next(dir);
	}

	key_code_t key_input = K_VALID;		// show status bar at the first loop
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;

	// Main input loop
	for ( ; ; ) {
		check_filer_cur_dir();
#ifdef ENABLE_HISTORY
		if (strcmp(prev_cur_dir, get_cur_filer_pane_view()->cur_dir) != 0) {
			dir_history_append_temporarily(get_cur_filer_pane_view()->cur_dir);
			strlcpy__(prev_cur_dir, get_cur_filer_pane_view()->cur_dir, MAX_PATH_LEN);
		}
#endif // ENABLE_HISTORY
		update_file_infos_of_all_panes(filer_do_next);
		update_screen_app(IS_KEY_VALID(key_input), 1);
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		strcpy__(get_cur_filer_pane_view()->next_file, "");
		filer_do_next = FL_UPDATE_FILE_LIST_AUTO;
		if (IS_KEY_VALID(key_input)) {
			// some key input
			mflf_d_printf("input%ckey:0x%04x(%s|%s)=======================\n",
			 '_', (UINT16)key_input,
			 long_key_name_from_key_code(key_input, NULL),
			 short_key_name_from_key_code(key_input, NULL));
			filer_do_next = EF_NONE;
			func_key_list_t *fkey_list;
			if ((fkey_list = get_fkey_entry_table_from_key(
			 filer_func_key_table, key_input, -1, 1)) == NULL) {
				fkey_list = get_fkey_entry_table_from_key(
				 filer_func_key_table, tolower_if_alpha(key_input), -1, 1);
			}
			if (fkey_list == NULL) {
				disp_status_bar_warn(_("No command assigned for the key: 0x%04x"),
				 (UINT16)key_input);
			} else {
				strlcpy__(get_cur_filer_pane_view()->next_file,
				 get_cur_fv_cur_file_ptr()->file_name,
				  MAX_PATH_LEN);
				if (is_app_chooser_viewer_mode()) {
					switch (fkey_list->list_mode) {
					case EFNM_EXEC:		// not executable in List mode
						disp_status_bar_done(
						 _("Can not execute this function in filer List mode: [%s]"),
						 fkey_list->func_id);
						filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
						break;
					case F_LM_FLNM:		// no exec in filer List mode and return FILE_NAME
						filer_do_next = FL_ENTER_FILE_NAME_OR_PATH;
						break;
					case F_LM_CUDI:		// no exec in filer List mode and return CUR_DIR_PATH
						filer_do_next = FL_ENTER_CUR_DIR_PATH;
						break;
					case EFLM_EXEC:	// executable in editor List mode
					case EFAM_EXEC:
					default:
						break;
					}
				}
				if (filer_do_next == EF_NONE) {
					flf_d_printf("<<<< CALL_FUNC_FILER [%s]\n", fkey_list->func_id);
					//=========================
					(*fkey_list->func)();	// call function "dof__...()"
					//=========================
					flf_d_printf(">>>> filer_do_next: EF__%d\n", filer_do_next);
					unselect_all_files_auto(_FILE_SEL_AUTO_);
				}
flf_d_printf("filer_do_next: %d\n", filer_do_next);
			}
		}
		if (is_app_normal_mode()) {
			// normal mode
			switch(filer_do_next) {
			default:
				break;
			case EF_CANCELLED:
			case EF_EXECUTED:
				filer_do_next = EF_NONE;	// not exit from application
				break;
			case EF_QUIT:
			case EF_LOADED:
				// quit from filer
				break;
			}
		} else {
			// chooser mode
			switch(filer_do_next) {
			default:
				break;
			case EF_CANCELLED:
			case EF_QUIT:
			case EF_LOADED:
			case EF_EXECUTED:
				break;
			case FL_ENTER_FILE_NAME_OR_PATH:
				strcpy__(path_buf, "");
				for (int file_idx = select_and_get_first_file_idx_selected();
				 file_idx >= 0;
				 file_idx = get_next_file_idx_selected(file_idx)) {
					if (IS_UPPER_KEY(key_input) == 0) {
						// enter file paths: /path/to/file-1 "/path/to/file 2" "/path/to/file 3"
						char path[MAX_PATH_LEN];
						cat_dir_and_file(path,
						 get_cur_filer_pane_view()->cur_dir,
						 get_cur_fv_file_ptr(file_idx)->file_name);
						concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN, path);
					} else /* if (IS_UPPER_KEY(key_input)) */ {
						// enter file names: file-1 "file 2" "file 3"
						concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN,
						 get_cur_fv_file_ptr(file_idx)->file_name);
					}
				}
				filer_do_next = EF_INPUT_W_ENTER;
				break;
			case FL_ENTER_CUR_DIR_PATH:
				strlcpy__(path_buf, get_cur_filer_pane_view()->cur_dir, MAX_PATH_LEN);
				filer_do_next = EF_INPUT_W_ENTER;
				break;
			}
		}
#ifdef ENABLE_HISTORY
		save_histories();
#endif // ENABLE_HISTORY

		// | command modifier key | replace/append string         | return value        |
		// |----------------------|-------------------------------|---------------------|
		// | none                 | replacing input file/dir name | EF_INPUT_TO_REPLACE |
		// | ALT                  | appending input file/dir name | EF_INPUT_TO_APPEND  |
		if (filer_do_next == EF_INPUT_W_ENTER) {
			filer_do_next = (IS_META_KEY(key_input) == 0)
			 ? EF_INPUT_TO_REPLACE		// Replace input file/dir name
			 : EF_INPUT_TO_APPEND;		// Append input file/dir name
		}
		if (filer_do_next >= EF_QUIT) {
			break;
		}
	}
flf_d_printf("filer_do_next: %d, [%s]\n", filer_do_next, path_buf);
	return filer_do_next;
}

PRIVATE int check_filer_cur_dir(void)
{
	// check if cur_dir is readable
	if (is_dir_readable(get_cur_filer_pane_view()->cur_dir) == 0) {
		// current directory is not readable or disappeared
		while (is_dir_readable(get_cur_filer_pane_view()->cur_dir) == 0) {
			// go up to the root dir
			dof_parent_directory();
		}
		tio_beep();
	}
	if (is_dir_readable(get_cur_filer_pane_view()->cur_dir) == 0) {
		get_full_path_of_cur_dir(get_cur_filer_pane_view()->cur_dir);
	}
	change_cur_dir(get_cur_filer_pane_view()->cur_dir);
	return 0;
}

PRIVATE int update_file_infos_of_all_panes(int update_request)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		update_file_infos(get_cur_filer_pane_view(), update_request);
	} else {
		update_file_infos(get_cur_filer_view(0), update_request);
		update_file_infos(get_cur_filer_view(1), update_request);
	}
	return 0;
}
// the occasions file list to be updated:
//  - the first time a filer displays file list
//  - command requests updating file list (force update or after changing current directory)
//  - periodic update of file list
// the occasion file list update stopped:
//  - when some files are selected (for file processing e.g. copy, move, delete, ...)
PRIVATE int update_file_infos(filer_view_t *fv, int update_request)
{
	int files = 0;
	fv->prev_file_idx = fv->cur_file_idx;
	if ((fv->file_infos == NULL)
	 || (strcmp(fv->listed_dir, fv->cur_dir) != 0)
	 || ((update_request == FL_UPDATE_FILE_LIST_AUTO) && (get_files_selected(fv) == 0))
	 || (update_request == FL_UPDATE_FILE_LIST_FORCE)) {
		if ((update_request < FL_UPDATE_FILE_LIST_FORCE) && (fv->file_infos != NULL)) {
			strlcpy__(fv->next_file, fv->file_infos[fv->cur_file_idx].file_name, MAX_PATH_LEN);
		} else {
			// 'fv->next_file' has set by the requester
		}
		files = make_file_infos(fv);
		sort_file_infos(fv);
		research_file_name_in_file_infos(fv, fv->next_file);
	}
	return files;
}

PRIVATE void disp_status_bar_filer();

int update_screen_filer(int status_bar, int refresh)
{
	win_select_cur_sub_win(WIN_IDX_CENTRAL);
	tio_set_cursor_on(0);

	// title bar
	int files_selected = get_files_selected_cfv();
	disp_title_bar_filer(get_cur_filer_pane_view()->cur_dir, get_cur_filer_pane_view()->filter,
	 get_cur_fv_file_idx(), files_selected, get_cur_filer_pane_view()->file_infos_entries);

	// status bar
	if (status_bar) {
		disp_status_bar_filer();
	} else {
		redisp_status_bar();
	}
	// key list
	disp_key_list_filer();

	if (GET_APPMD(fl_FILER_PANES) == 0) {		// 1 pane
		win_select_cur_sub_win(WIN_IDX_SUB_WHOLE);
		disp_file_list(get_cur_filer_pane_view(), 1);
	} else {									// 2 panes
		for (int pane_sel_idx = 0; pane_sel_idx < FILER_PANES; pane_sel_idx++) {
			// pane_sel_idx=0: update not current pane
			// pane_sel_idx=1: update current pane
			int pane_idx = (pane_sel_idx == 0)
			 ? get_filer_another_pane_idx()		// not current pane
			 : get_filer_cur_pane_idx();		// current pane
			win_select_cur_sub_win(WIN_IDX_SUB_LEFT + pane_idx);
			disp_file_list(get_cur_filer_view(pane_idx), pane_sel_idx);
		}
	}

	tio_set_cursor_on(1);
	if (refresh) {
		tio_refresh();
	}

	return 0;
}

#define HHCMMCSS_BUF_LEN		(1+8)	// " 23:59:59"
PRIVATE void disp_title_bar_filer(const char* dir, const char* filter,
 int cur_idx, int files_selected, int files_total)
{
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_path[MAX_PATH_LEN+1];
	char buf_files[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_time[HHCMMCSS_BUF_LEN+1];

	set_title_bar_color_by_state(ITEM_COLOR_IDX_TITLE, 0);
	central_win_output_string(central_win_get_top_win_y() + TITLE_LINE, 0,
	 tio_blank_line(), central_win_get_columns());

	//-------------------------------------------------------------------------
	strlcpy__(buffer, dir, MAX_PATH_LEN);
	add_last_slash_to_dir(buffer);		// /home/user/bin ==> /home/user/bin/, / ==> //
	remove_redundant_slash(buffer);		// /home/user/bin/ ==> no change, // ==> /
	char separator_char = indication_of_app_mode();
	snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN, "%s%d%c%s%s",
	 root_notation(), get_filer_cur_pane_idx()+1, separator_char, buffer, filter);
	if ((get_win_depth() == 0) && (strcmp(dir, get_home_dir()) == 0)) {
		strcat_printf(buf_path, MAX_SCRN_LINE_BUF_LEN, "  [%s|%s]",
		 get_at_host_name(), get_tty_name());
	}

	//-------------------------------------------------------------------------
	// current date / time
	snprintf_(buf_time, HHCMMCSS_BUF_LEN+1, " %s",
	 cur_ctime_cdate(msec_past_input_key() < 1000));
	if (files_selected == 0) {
		snprintf_(buf_files, MAX_SCRN_LINE_BUF_LEN+1, " %d/%d %c%s",
		 cur_idx, files_total, *get_str_file_sort_mode(), buf_time);
	} else {
		snprintf_(buf_files, MAX_SCRN_LINE_BUF_LEN+1, " %d/%d %c%s",
		 files_selected, files_total, *get_str_file_sort_mode(), buf_time);
	}

	int path_cols = LIM_MIN(0, central_win_get_columns() - strlen_path(buf_files));
	shrink_str__adjust_col(buf_path, path_cols, 2);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_path, buf_files);

	central_win_output_string(central_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);
}

PRIVATE void disp_status_bar_filer()
{
	disp_status_bar_cursor("%s", file_info_str(get_cur_fv_cur_file_ptr(), 0, 1, 0));
}

PRIVATE void adjust_top_file_idx(filer_view_t *fv)
{
	// keep row position of the current file as same as possible
	int disp_line_idx_to_keep = MIN_MAX_(filer_vert_scroll_margin_lines(),
	 fv->prev_file_idx - fv->top_file_idx,
	 filer_win_get_file_list_lines() - filer_vert_scroll_margin_lines() - 1);
	fv->top_file_idx = MIN_MAX_(0, fv->cur_file_idx - disp_line_idx_to_keep,
	 fv->file_infos_entries-1);
}
PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane)
{
	if (cur_pane == 0) {
		set_work_space_color_dark();
	} else {
		clear_work_space_color_dark();
	}
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_screen();

	if (filer_win_get_file_path_lines()) {
		char buffer[MAX_SCRN_LINE_BUF_LEN+1];
		// on two pane mode, show each directory path
		strlcpy__(buffer, fv->cur_dir, MAX_SCRN_LINE_BUF_LEN);
		shrink_str__adjust_col(buffer, central_win_get_columns(), 2);
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, ! cur_pane);
		sub_win_output_string(filer_win_get_file_path_y(), 0, buffer, -1);
	}

	adjust_top_file_idx(fv);
	int file_idx = fv->top_file_idx;
	int cur_file_idx = fv->cur_file_idx;

	for (int line_idx = 0;
	 line_idx < filer_win_get_file_list_lines() && file_idx < fv->file_infos_entries;
	 line_idx++, file_idx++) {
		// Highlight the currently selected file/dir.
		char *ptr = file_info_str(&(fv->file_infos[file_idx]),
		 1, file_idx != cur_file_idx, fv->file_infos[file_idx].selected);
		if (cur_pane && (file_idx == cur_file_idx)) {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
			tio_set_attr_rev(1);		// display current line inverted
		} else {
			if (fv->file_infos[file_idx].selected) {
				set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
			} else {
#define STRIPE_LINES	4
				if (GET_APPMD(fl_SHOW_ZEBRA_STRIPE)
				 && (file_idx % (STRIPE_LINES*2)) >= STRIPE_LINES) {
					set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL2, 0);
				} else {
					set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
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
		sub_win_set_cursor_pos(filer_win_get_file_list_y() + cur_file_idx - fv->top_file_idx, 1);
		win_save_cursor_pos();
	}
	return 0;
}
//-------------------------------------
#define FILER_VERT_SCROLL_MERGIN	VERT_SCROLL_MERGIN
int filer_vert_scroll_margin_lines()
{
	return LIM_MAX(FILER_VERT_SCROLL_MERGIN, filer_win_get_file_list_lines() / 3);	// [0, 3]
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
int filer_win_get_file_path_lines(void)
{
	return GET_APPMD(fl_FILER_PANES);
}
int filer_win_get_file_list_lines(void)
{
	return LIM_MIN(0, sub_win_get_lines() - filer_win_get_file_path_lines());
}
//-------------------------------------
int filer_win_get_file_path_y(void)
{
	return 0;
}
int filer_win_get_file_list_y(void)
{
	return filer_win_get_file_path_y() + filer_win_get_file_path_lines();
}
//-------------------------------------
PRIVATE void disp_key_list_filer(void)
{
	disp_fkey_list();

	const char *filer_key_lists[] = {
 "<dof_quit_filer>Quit "
 "<dof_open_file_recursive>Edit "
 "<dof_open_new_file>EditNewFile "
 "<dof_copy_file>Copy "
 "<dof_copy_file_update>UpdateCopy "
 "<dof_move_file>Move "
 "<dof_rename_file>Rename "
 "<dof_trash_file>Trash "
 "<dof_delete_file>Delete "
 "<dof_mark_to_delete_file>MarkToDelete "
 "<dof_exec_command_with_file>Exec "
 "<dof_run_command_rel>Run "
 "<dof_select_file>SelectFile "
 "<dof_select_all_files>SelAllFiles ",
 "<dof_select_no_file>SelNoFile "

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
 "<dof_find_file>FindFile ",
	};
	disp_key_list(filer_key_lists);
}

#endif // ENABLE_FILER

// End of filer.c
