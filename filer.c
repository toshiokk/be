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

filer_panes_t *cur_filer_panes;		// Current Filer Panes

PRIVATE char filer_cur_path[MAX_PATH_LEN+1];	// /directory/filter.*
filer_do_next_t filer_do_next = FILER_DO_NOTHING;

PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir);
PRIVATE filer_panes_t *inherit_filer_panes(filer_panes_t *next_fps);
PRIVATE int get_other_filer_pane_idx(int filer_pane_idx);

PRIVATE int filer_main_loop(const char *directory, const char *filter,
 char *file_path, int buf_len);
PRIVATE int check_filer_cur_dir(void);
PRIVATE int update_all_file_list(const char *filter, int force_update);
PRIVATE int update_file_list(filer_view_t *fv, const char *filter, int force_update);

PRIVATE void disp_filer_title_bar(const char *path,
 int cur_idx, int files_selected, int files_total);
PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane);

PRIVATE void disp_key_list_filer(void);

void init_filer_panes(filer_panes_t *fps, const char *cur_dir)
{
	cur_filer_panes = fps;
	strlcpy__(cur_filer_panes->org_cur_dir, cur_dir, MAX_PATH_LEN);
	set_filer_cur_pane_idx(0);
	for (int filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		// set initial value
		init_filer_view(&cur_filer_panes->filer_views[filer_pane_idx], cur_dir);
	}
}
PRIVATE filer_panes_t *inherit_filer_panes(filer_panes_t *next_fps)
{
	int cur_pane_idx = get_filer_cur_pane_idx();
	filer_panes_t *prev_fps = cur_filer_panes;	// previous filer panes
	init_filer_panes(next_fps, prev_fps->filer_views[get_filer_cur_pane_idx()].cur_dir);
	set_filer_cur_pane_idx(cur_pane_idx);
	for (int filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		// set initial value
		init_filer_view(&next_fps->filer_views[filer_pane_idx],
		 prev_fps->filer_views[filer_pane_idx].cur_dir);
		next_fps->filer_views[filer_pane_idx].cur_sel_idx = 
		 prev_fps->filer_views[filer_pane_idx].cur_sel_idx;
	}
	return prev_fps;
}
void free_filer_panes(filer_panes_t *fps, filer_panes_t *prev_fps)
{
	for (int filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		free_file_list(&fps->filer_views[filer_pane_idx]);
	}
	if (prev_fps) {
		cur_filer_panes = prev_fps;
	}
}
void set_filer_cur_pane_idx(int cur_pane_idx)
{
	cur_filer_panes->cur_pane_idx = cur_pane_idx;
}
int get_filer_cur_pane_idx()
{
	int cur_pane_idx = cur_filer_panes->cur_pane_idx;
	if (cur_pane_idx < 0) {
		cur_pane_idx = 0;
	}
	return cur_pane_idx;
}
PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir)
{
	memset(fv, 0x00, sizeof(*fv));
	strlcpy__(fv->cur_dir, cur_dir, MAX_PATH_LEN);
	strcpy__(fv->cur_filter, "");
	strcpy__(fv->listed_dir, "");
	fv->file_list_entries = 0;
	fv->file_list = NULL;
	fv->cur_sel_idx = -1;
	fv->top_idx = 0;
	strcpy__(fv->prev_dir, "");
	strcpy__(fv->next_file, "");
}
filer_view_t *get_filer_view(int pane_idx)
{
	if (pane_idx < 0) {
		return get_cur_filer_view();
	}
	return &cur_filer_panes->filer_views[pane_idx];
}
filer_view_t *get_cur_filer_view(void)
{
	return &cur_filer_panes->filer_views[get_filer_cur_pane_idx()];
}

filer_view_t *get_other_filer_view(void)
{
	return &cur_filer_panes->filer_views[get_other_filer_pane_idx(get_filer_cur_pane_idx())];
}
PRIVATE int get_other_filer_pane_idx(int filer_pane_idx)
{
	return filer_pane_idx == 0 ? 1 : 0;
}

//-----------------------------------------------------------------------------

int call_filer(int push_win, int list_mode,
 const char *dir, const char *filter, char *file_path, int buf_len)
{
	filer_panes_t *prev_fps = NULL;
	filer_panes_t next_filer_panes;
	app_mode_t appmode_save;
	int ret;

	if (push_win) {
		win_push_win_size();

		prev_fps = inherit_filer_panes(&next_filer_panes);
	}

	memcpy(&appmode_save, &app_mode__, sizeof(app_mode__));
	SET_APPMD(app_EDITOR_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);
	SET_APPMD_VAL(ed_EDITOR_PANES, 0);
	set_app_func_key_table();

flf_d_printf("push_win:%d, list_mode:%d\n", push_win, list_mode);
flf_d_printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	ret = filer_main_loop(dir, filter, file_path, buf_len);
flf_d_printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
flf_d_printf("ret: %d\n", ret);
	filer_do_next = FILER_DO_NOTHING;	// for caller of call_filer(), clear "editor_quit"

	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));
	SET_APPMD_VAL(app_LIST_MODE, GET_APPMD_PTR(&appmode_save, app_LIST_MODE));
	SET_APPMD_VAL(ed_EDITOR_PANES, GET_APPMD_PTR(&appmode_save, ed_EDITOR_PANES));
	set_app_func_key_table();

	if (push_win) {
		free_filer_panes(&next_filer_panes, prev_fps);
		change_cur_dir(get_cur_filer_view()->cur_dir);

		win_pop_win_size();
	}

	return ret;
}

//-----------------------------------------------------------------------------

PRIVATE int filer_main_loop(const char *directory, const char *filter,
 char *file_path, int buf_len)
{
#ifdef ENABLE_HISTORY
	char prev_cur_dir[MAX_PATH_LEN+1];
#endif // ENABLE_HISTORY
	key_code_t key_input;
	func_key_table_t *func_key_table;
	int file_idx;

#ifdef ENABLE_HISTORY
	get_cur_dir(prev_cur_dir);		// memorize current dir
#endif // ENABLE_HISTORY
	if (is_strlen_not_0(directory)) {
		strlcpy__(get_cur_filer_view()->cur_dir, directory, MAX_PATH_LEN);
	}

	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;

	while (1) {
		check_filer_cur_dir();
		cat_dir_and_file(filer_cur_path, MAX_PATH_LEN, get_cur_filer_view()->cur_dir, filter);
#ifdef ENABLE_HISTORY
		if (strcmp(prev_cur_dir, get_cur_filer_view()->cur_dir) != 0) {
			update_history(HISTORY_TYPE_IDX_DIR, get_cur_filer_view()->cur_dir, 0);
			strlcpy__(prev_cur_dir, get_cur_filer_view()->cur_dir, MAX_PATH_LEN);
		}
#endif // ENABLE_HISTORY
		if (filer_do_next >= FILER_DO_UPDATE_FILE_LIST_AUTO) {
			update_all_file_list(filter, filer_do_next == FILER_DO_UPDATE_FILE_LIST_FORCE
			 ? 2
			 : (filer_do_next == FILER_DO_UPDATE_FILE_LIST_AUTO ? 1 : 0));
		}
		update_screen_filer(1, 1, 1);
		//----------------------------------
		key_input = input_key_wait_return(500);
		//----------------------------------
		if (key_input >= 0) {
			// some key input
mflf_d_printf("input%ckey:0x%04x(%s)=======================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));
			clear_status_bar_displayed();
		}

		strcpy__(get_cur_filer_view()->next_file, "");

		filer_do_next = FILER_DO_UPDATE_FILE_LIST_AUTO;
		switch (key_input) {
		case K_NONE:
			filer_do_next = FILER_DO_UPDATE_FILE_LIST_AUTO;
			break;
		case K_UP:
			dof_up();
			break;
		case K_DOWN:
			dof_down();
			break;
		case K_PPAGE:
		case K_LEFT:
			dof_page_up();
			break;
		case K_NPAGE:
		case K_RIGHT:
			dof_page_down();
			break;
		case K_HOME:
			dof_top_of_list();
			break;
		case K_END:
			dof_bottom_of_list();
			break;
		case K_ESC:
			filer_do_next = FILER_DO_ABORT;
			break;
		default:
			filer_do_next = FILER_DO_NOTHING;
			if ((func_key_table = get_func_key_table_from_key(filer_func_key_table,
			 key_input)) == NULL) {
				func_key_table = get_func_key_table_from_key(filer_func_key_table,
				 tolower_if_alpha(key_input));
			}
			if (func_key_table == NULL) {
				disp_status_bar_err(_("Key command not assigned: %04xh"), key_input);
				filer_do_next = FILER_DO_UPDATE_FILE_LIST_AUTO;
			} else {
				strlcpy__(get_cur_filer_view()->next_file,
				 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name,
				  MAX_PATH_LEN);
				if (is_app_list_mode()) {
					switch (func_key_table->list_mode) {
					case XA:		// executable all Normal/List mode
						break;
					case XL:		// not executable in List mode
						disp_status_bar_done(
						 _("Can not execute this function: [%s]"), func_key_table->func_id);
						filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
						break;
					case XF:		// not executable in List mode and return FILE_NAME
						filer_do_next = FILER_DO_ENTER_FILE_NAME;
						break;
					case XP:		// not executable in List mode and return FILE_PATH
						filer_do_next = FILER_DO_ENTER_FILE_PATH;
						break;
					case XC:		// not executable in List mode and return CUR_DIR_PATH
						filer_do_next = FILER_DO_ENTER_CUR_DIR_PATH;
						break;
					}
				}
				if (filer_do_next == FILER_DO_NOTHING) {
flf_d_printf("CALL_FILER_FUNC [%s] <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n",
 func_key_table->func_id);
					//=========================
					int ret = (*func_key_table->func)();	// call function "dof_...()"
					//=========================
flf_d_printf("ret_val: %d    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n", ret);
					unselect_all_files_auto(_FILE_SEL_AUTO_);
				}
flf_d_printf("filer_do_next: %d\n", filer_do_next);
			}
			break;
		}
		if (filer_do_next >= FILER_DO_QUIT) {
			break;
		}
	}

flf_d_printf("filer_do_next: %d\n", filer_do_next);
	strcpy__(file_path, "");
	if (filer_do_next == FILER_DO_ABORT) {
		return -1;		// abort
	}
	if (filer_do_next == FILER_DO_QUIT) {
		return 0;		// quit
	}
	if (filer_do_next == FILER_DO_ENTER_FILE_NAME
	 || filer_do_next == FILER_DO_ENTER_FILE_PATH) {
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (filer_do_next == FILER_DO_ENTER_FILE_NAME) {
				// file-1 "file name 2" "file name 3"
				concat_file_name_separating_by_space(file_path, buf_len,
				 get_cur_filer_view()->file_list[file_idx].file_name);
			} else {
				// /dir/to/file-path-1 "/dir/to/file path 2" "/dir/to/file path 3"
				char path[MAX_PATH_LEN];
				cat_dir_and_file(path, MAX_PATH_LEN,
				 get_cur_filer_view()->cur_dir,
				 get_cur_filer_view()->file_list[file_idx].file_name);
				concat_file_name_separating_by_space(file_path, buf_len, path);
			}
		}
	}
	if (filer_do_next == FILER_DO_ENTER_CUR_DIR_PATH) {
		strlcpy__(file_path, get_cur_filer_view()->cur_dir, MAX_PATH_LEN);
	}
flf_d_printf("[%s]\n", file_path);
	return IS_META_KEY(key_input)
	 ? 2	// Append input file/dir name
	 : 1;	// input file/dir name
}

PRIVATE int check_filer_cur_dir(void)
{
	// check if cur_dir is readable
	if (is_dir_readable(get_cur_filer_view()->cur_dir) == 0) {
		// current directory is not readable or disappeared
		while (is_dir_readable(get_cur_filer_view()->cur_dir) == 0) {
			// go up to the root dir
			dof_parent_directory();
		}
		tio_beep();
	}
	if (is_dir_readable(get_cur_filer_view()->cur_dir) == 0) {
		get_cur_dir(get_cur_filer_view()->cur_dir);
	}
	change_cur_dir(get_cur_filer_view()->cur_dir);
	return 0;
}

PRIVATE int update_all_file_list(const char *filter, int force_update)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
///_FLF_
		update_file_list(get_cur_filer_view(), filter, force_update);
	} else {
///_FLF_
		update_file_list(get_filer_view(0), filter, force_update);
///_FLF_
		update_file_list(get_filer_view(1), filter, force_update);
	}
	return 0;
}
PRIVATE int update_file_list(filer_view_t *fv, const char *filter, int force_update)
{
	int files = 0;

	if (get_cur_filer_view()->file_list == NULL
	 || strcmp(fv->listed_dir, fv->cur_dir) != 0
	 || (force_update == 1 && get_files_selected(fv) == 0)
	 || force_update >= 2) {
		files = make_file_list(fv, filter);
		strcpy__(fv->listed_dir, fv->cur_dir);
		sort_file_list(fv);
		research_file_name_in_file_list(fv);
	}
	return files;
}

int update_screen_filer(int title_bar, int status_bar, int refresh)
{
	int files_selected;
	int pane_sel_idx;		// 0: not current pane, 1: current pane
	int pane_idx;			// pane index

	tio_set_cursor_on(0);

	files_selected = get_files_selected_cfv();

	// title bar
	disp_filer_title_bar(filer_cur_path,
	 get_cur_filer_view()->cur_sel_idx, files_selected, get_cur_filer_view()->file_list_entries);

	if (GET_APPMD(fl_FILER_PANES) == 0) {		// 1 pane
		win_select_win(WIN_IDX_SUB_WHOLE);
		disp_file_list(get_cur_filer_view(), 1);
	} else {									// 2 panes
		for (pane_sel_idx = 0; pane_sel_idx < FILER_PANES; pane_sel_idx++) {
			// 1st, update not current pane.
			// 2nd, update current pane.
			if (pane_sel_idx == 0) {
				pane_idx = get_filer_cur_pane_idx() == 0 ? 1 : 0;	// not current pane
			} else {
				pane_idx = get_filer_cur_pane_idx();	// current pane
			}
			win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
			if (pane_sel_idx == 0) {
				// not current pane
				set_work_space_color_low();
			} else {
				// current pane
				set_work_space_color_normal();
			}
			disp_file_list(&cur_filer_panes->filer_views[pane_idx], pane_sel_idx);
		}
	}
	// status bar
	disp_status_bar_percent_filer(
	 "%s", file_info_str(&(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx]),
	 0, 1, 0));
	// key list
	disp_key_list_filer();

	// Set cursor position
	sub_win_set_cursor_pos(filer_win_get_file_list_y()
	 + get_cur_filer_view()->cur_sel_idx
	 - get_cur_filer_view()->top_idx, 0);

	set_work_space_color_normal();

	if (refresh) {
		tio_refresh();
	}

	tio_set_cursor_on(1);
	return 0;
}

#define HHCMMCSS_LEN		8	// "23:59:59"
PRIVATE void disp_filer_title_bar(const char *path,
 int cur_idx, int files_selected, int files_total)
{
	int space;
	char separator_char;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_dir[MAX_PATH_LEN+1];
	char buf_files[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_time[1+HHCMMCSS_LEN+1];

	set_title_bar_color_by_state(0, 0, 0);
	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0,
	 tio_blank_line(), main_win_get_columns());

	//-------------------------------------------------------------------------
	separator_char = ':';
	if (is_app_list_mode()) {
		separator_char = '.';
	}
#ifdef ENABLE_DEBUG
	if (GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF) {
		separator_char = ';';
		if (is_app_list_mode()) {
			separator_char = ',';
		}
	}
#endif // ENABLE_DEBUG
	snprintf_(buf_dir, MAX_SCRN_LINE_BUF_LEN, "%s%d%c%s",
	 root_notation(), get_filer_cur_pane_idx()+1, separator_char, path);

	// current time
	snprintf_(buf_time, 1+HHCMMCSS_YY_MM_DD_LEN+1, " %s",
	 cur_ctime_cdate(just_has_been_input_key()));

	//-------------------------------------------------------------------------
	snprintf_(buf_files, MAX_SCRN_LINE_BUF_LEN+1, "%d/%d:%d %c%s",
	 cur_idx, files_total, files_selected, *get_str_sort_by(), buf_time);
	space = LIM_MIN(0, main_win_get_columns() - strnlen(buf_files, MAX_SCRN_LINE_BUF_LEN) - 1);
	shrink_str(buf_dir, space, 2);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%-*s %s", space, buf_dir, buf_files);

	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);
}

PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane)
{
	int line_idx;
	int cur_sel_idx;
	int bottom_idx;
	int file_idx;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char *ptr;

	cur_sel_idx = fv->cur_sel_idx;
	if (cur_sel_idx < fv->top_idx + FILER_VERT_SCROLL_MARGIN_LINES) {
		fv->top_idx = LIM_MIN(0, cur_sel_idx - FILER_VERT_SCROLL_MARGIN_LINES);
	}
	bottom_idx = fv->top_idx + (filer_win_get_file_list_lines()-1);
	if (bottom_idx - FILER_VERT_SCROLL_MARGIN_LINES < cur_sel_idx) {
		bottom_idx = LIM_MAX(fv->file_list_entries-1,
		 cur_sel_idx) + FILER_VERT_SCROLL_MARGIN_LINES;
		fv->top_idx = LIM_MIN(0, bottom_idx - (filer_win_get_file_list_lines()-1));
	}

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_screen();

	if (filer_win_get_file_path_lines()) {
		// on two pane mode, show each path
		strlcpy__(buffer, fv->cur_dir, MAX_SCRN_LINE_BUF_LEN);
		shrink_str(buffer, main_win_get_columns(), 2);
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, cur_pane);
		sub_win_output_string(filer_win_get_file_path_y(), 0, buffer, -1);
	}

	for (line_idx = 0, file_idx = fv->top_idx;
	 line_idx < filer_win_get_file_list_lines() && file_idx < fv->file_list_entries;
	 line_idx++, file_idx++) {
		// Highlight the currently selected file/dir.
		ptr = file_info_str(&(fv->file_list[file_idx]),
		 1, file_idx != cur_sel_idx, fv->file_list[file_idx].selected);
		adjust_utf8s_columns(ptr, main_win_get_columns());
		if (fv->file_list[file_idx].selected) {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
		} else {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
		}
		if (cur_pane && file_idx == cur_sel_idx) {
			tio_set_attr_rev(1);		// display current line inverted
		}
		sub_win_output_string(filer_win_get_file_list_y() + line_idx, 0, ptr, -1);
		if (cur_pane && file_idx == cur_sel_idx) {
			tio_set_attr_rev(0);
		}
	}
	if (cur_pane) {
		// Set cursor position
		sub_win_set_cursor_pos(filer_win_get_file_list_y() + cur_sel_idx - fv->top_idx, 0);
	}
	return 0;
}
//-------------------------------------
int filer_win_get_file_path_lines(void)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		return 0;
	} else {
		return 1;
	}
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
	char *filer_key_lists[] = {
 "{Menu}"
 "  {Home } {Copy } {CpyUd} {Renam}"
 "  {Move } {Delet} {MkDel} {MkDir}"
 "  {ChDir} {Exec } {Edit } {Edit }",

 "<dof_quit_filer>Quit "
 "<dof_edit_file>Edit "
 "<dof_edit_new_file>EditNewFile "
 "<dof_copy_file>Copy "
 "<dof_copy_file_update>UpdateCopy "
 "<dof_move_file>Move "
 "<dof_trash_file>Trash "
 "<dof_delete_file>Delete "
 "<dof_rename_file>Rename "
 "<dof_mark_to_delete_file>MarkToDelete "
 "<dof_exec_command_with_file>Exec "
 "<dof_run_command_rel>Run "
 "<dof_select_file>SelectFile "
 "<dof_select_no_file>SelNoFile "
 "<dof_select_all_files>SelAllFiles ",

 "<dof_home_directory>HomeDir "
 "<dof_root_directory>RootDir "
 "<dof_change_directory>ChgDir "
 "<dof_parent_directory>ParentDir "
 "<dof_tap_file>Pager/EnterDir "
 "<dof_make_directory>MkDir "
 "<dof_tog_panes>TwoPane "
 "<dof_switch_filer_pane>SwPane "
 "<dof_inc_sort_by>Sort "
 "<dof_inc_show_file_info>Info "
 "<dof_refresh_filer>Refresh "
 "<dof_tog_show_dot_file>ShowDotFile "
 "<dof_find_file>FindFile ",
	};
	disp_key_list(filer_key_lists);
}

#endif // ENABLE_FILER

// End of filer.c
