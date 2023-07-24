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

filer_views_t *cur_filer_views;		// Current Filer ViewS
filer_view_t *cur_fv;				// Current Filer View

PRIVATE char filer_cur_path[MAX_PATH_LEN+1];	// /directory/filter.*
int filer_do_next = FILER_DO_NOTHING;

PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir);
PRIVATE filer_views_t *inherit_filer_views(filer_views_t *new_fvs);
PRIVATE void discard_filer_views(filer_views_t *new_fvs, filer_views_t *prev_fvs);
PRIVATE int get_other_view_idx(int filer_view_idx);
#if 0
#ifdef ENABLE_DEBUG
PRIVATE void dump_filer_view(filer_view_t *fv);
PRIVATE void dump_filer_views(void);
#endif // ENABLE_DEBUG
#endif // 0

PRIVATE int filer_main_loop(const char *directory, const char *filter,
 char *file_path, int buf_len);
PRIVATE int check_filer_cur_dir(void);
PRIVATE int update_all_file_list(const char *filter, int force_update);
PRIVATE int update_file_list(filer_view_t *fv, const char *filter, int force_update);

PRIVATE void disp_filer_title_bar(const char *path,
 int cur_idx, int files_selected, int files_total);
PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane);

PRIVATE int filer_win_get_file_path_lines(void);
PRIVATE int filer_win_get_file_list_lines(void);
PRIVATE int filer_win_get_file_path_y(void);
PRIVATE int filer_win_get_file_list_y(void);

PRIVATE void disp_key_list_filer(void);

#define FILER_VERT_SCROLL_MARGIN_LINES	LIM_MAX(5, filer_win_get_file_list_lines()/3)
#define FILER_VERT_SCROLL_LINES			\
	LIM_MIN(1, filer_win_get_file_list_lines()-1 - FILER_VERT_SCROLL_MARGIN_LINES)

void init_filer_views(filer_views_t *fvs, const char *cur_dir)
{
	int filer_view_idx;

	cur_filer_views = fvs;
	strlcpy__(cur_filer_views->org_cur_dir, cur_dir, MAX_PATH_LEN);
	cur_filer_views->view_idx = 0;
	for (filer_view_idx = 0; filer_view_idx < FILER_VIEWS; filer_view_idx++) {
		// set initial value
		init_filer_view(&cur_filer_views->views[filer_view_idx], cur_dir);
	}
}
PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir)
{
	memset(fv, 0x00, sizeof(*fv));
	strlcpy__(fv->cur_dir, cur_dir, MAX_PATH_LEN);
	strcpy__(fv->cur_filter, "");
	fv->file_list_entries = 0;
	fv->file_list = NULL;
	fv->cur_sel_idx = -1;
	fv->top_idx = 0;
	strcpy__(fv->prev_dir, "");
	strcpy__(fv->next_file, "");
}
void set_cur_filer_view(void)
{
	cur_fv = &cur_filer_views->views[cur_filer_views->view_idx];
}

PRIVATE filer_views_t *inherit_filer_views(filer_views_t *new_fvs)
{
	filer_views_t *prev_fvs = cur_filer_views;

	init_filer_views(new_fvs, prev_fvs->views[prev_fvs->view_idx].cur_dir);
	return prev_fvs;
}
PRIVATE void discard_filer_views(filer_views_t *new_fvs, filer_views_t *prev_fvs)
{
	int filer_view_idx;

	for (filer_view_idx = 0; filer_view_idx < FILER_VIEWS; filer_view_idx++) {
		free_file_list(&new_fvs->views[filer_view_idx]);
	}
	cur_filer_views = prev_fvs;
}
filer_view_t *get_other_filer_view(void)
{
	return &cur_filer_views->views[get_other_view_idx(cur_filer_views->view_idx)];
}
PRIVATE int get_other_view_idx(int filer_view_idx)
{
	return filer_view_idx == 0 ? 1 : 0;
}

#if 0
#ifdef ENABLE_DEBUG
PRIVATE void dump_filer_view(filer_view_t *fv)
{
flf_d_printf("fv:%p\n", fv);
flf_d_printf("cur_dir:[%s]\n", fv->cur_dir);
flf_d_printf("file_list_entries: %d\n", fv->file_list_entries);
flf_d_printf("file_list: %p\n", fv->file_list);
flf_d_printf("top_idx:[%d]\n", fv->top_idx);
flf_d_printf("cur_sel_idx:[%d]\n", fv->cur_sel_idx);
flf_d_printf("next_file:[%s]\n", fv->next_file);
}
PRIVATE void dump_filer_views(void)
{
	flf_d_printf("cur_filer_views: %p\n", cur_filer_views);
	flf_d_printf("view_idx: %d\n", cur_filer_views->view_idx);
	flf_d_printf("cur_fv: %p\n", cur_fv);
}
#endif // ENABLE_DEBUG
#endif // 0

//-----------------------------------------------------------------------------

int call_filer(int push_win, int list_mode,
char *dir, char *filter, char *file_path, int buf_len)
{
	filer_views_t *prev_fvs = NULL;
	filer_views_t new_filer_views;
	app_mode_t appmode_save;
	int ret;

	if (push_win) {
		win_push_win_size();

		prev_fvs = inherit_filer_views(&new_filer_views);
		set_cur_filer_view();
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
	filer_do_next = FILER_DO_NOTHING;

	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));
	SET_APPMD_VAL(app_LIST_MODE, GET_APPMD_PTR(&appmode_save, app_LIST_MODE));
	SET_APPMD_VAL(ed_EDITOR_PANES, GET_APPMD_PTR(&appmode_save, ed_EDITOR_PANES));
	set_app_func_key_table();

	if (push_win) {
		discard_filer_views(&new_filer_views, prev_fvs);
		set_cur_filer_view();
		change_cur_dir(cur_fv->cur_dir);

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
	get_cur_dir(prev_cur_dir);		// memorize prev. current dir
#endif // ENABLE_HISTORY
	if (is_strlen_not_0(directory)) {
		strlcpy__(cur_fv->cur_dir, directory, MAX_PATH_LEN);
	}

	filer_do_next = FILER_REFRESH_FORCED;

	while(1) {
		check_filer_cur_dir();
		cat_dir_and_file(filer_cur_path, MAX_PATH_LEN, cur_fv->cur_dir, filter);
#ifdef ENABLE_HISTORY
		if (strcmp(prev_cur_dir, cur_fv->cur_dir) != 0) {
			update_history(HISTORY_TYPE_IDX_DIR, cur_fv->cur_dir);
			strlcpy__(prev_cur_dir, cur_fv->cur_dir, MAX_PATH_LEN);
		}
#endif // ENABLE_HISTORY
		if (filer_do_next >= FILER_UPDATE_SCREEN) {
			update_all_file_list(filter, filer_do_next == FILER_REFRESH_FORCED ? 2
			 : (filer_do_next == FILER_REFRESH_AUTO ? 1 : 0));
		}
		update_screen_filer(1, filer_do_next >= FILER_UPDATE_SCREEN, 1);
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		if (key_input >= 0) {
			// some key input
mflf_d_printf("input%ckey:0x%04x===================================================\n",
 '_', key_input);
			clear_status_bar_displayed();
		}

		strcpy__(cur_fv->next_file, "");

		filer_do_next = FILER_UPDATE_SCREEN;
		switch (key_input) {
		case K_NONE:
			filer_do_next = FILER_REFRESH_AUTO;
			break;
		case K_UP:
			cur_fv->cur_sel_idx = MIN_MAX_(0, cur_fv->cur_sel_idx - 1,
			 cur_fv->file_list_entries-1);
			break;
		case K_DOWN:
			cur_fv->cur_sel_idx = MIN_MAX_(0, cur_fv->cur_sel_idx + 1,
			 cur_fv->file_list_entries-1);
			break;
		case K_PPAGE:
		case K_LEFT:
			cur_fv->cur_sel_idx = MIN_MAX_(0,
			 cur_fv->cur_sel_idx - FILER_VERT_SCROLL_LINES,
			 cur_fv->file_list_entries-1);
			break;
		case K_NPAGE:
		case K_RIGHT:
			cur_fv->cur_sel_idx = MIN_MAX_(0,
			 cur_fv->cur_sel_idx + FILER_VERT_SCROLL_LINES,
			 cur_fv->file_list_entries-1);
			break;
		case K_HOME:
			do_top_of_list();
			break;
		case K_END:
			do_bottom_of_list();
			break;
///
		case K_ESC:
///
			filer_do_next = FILER_ABORT;
///
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
				filer_do_next = FILER_DO_NOTHING;
			} else {
				strlcpy__(cur_fv->next_file,
				 cur_fv->file_list[cur_fv->cur_sel_idx].file_name, MAX_PATH_LEN);
				if (is_app_list_mode() == 0 || func_key_table->list_mode) {
flf_d_printf("CALL_FILER_FUNC [%s]\n", func_key_table->func_id);
					//=========================
					(*func_key_table->func)();			// call function
					//=========================
				}
				unselect_all_files_auto(_FILE_SEL_AUTO_);
			}
			break;
		}
		if (filer_do_next >= FILER_QUIT) {
			break;
		}
	}

flf_d_printf("filer_do_next: %d\n", filer_do_next);
	strcpy__(file_path, "");
	if (filer_do_next == FILER_ABORT) {
		return -1;		// aborted
	}
	if (filer_do_next == FILER_QUIT) {
		return 0;		// quitted
	}
	if (filer_do_next == FILER_ENTERED_FILE
	 || filer_do_next == FILER_ENTERED_FILE_PATH) {
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (filer_do_next == FILER_ENTERED_FILE) {
				// file-1 "file name 2" "file name 3"
				concat_file_name_separating_by_space(file_path, buf_len,
				 cur_fv->file_list[file_idx].file_name);
			} else {
				// file-path-1 "file path 2" "file path 3"
				char path[MAX_PATH_LEN];

				cat_dir_and_file(path, MAX_PATH_LEN,
				 cur_fv->cur_dir, cur_fv->file_list[file_idx].file_name);
				concat_file_name_separating_by_space(file_path, buf_len, path);
			}
		}
	}
	if (filer_do_next == FILER_ENTERED_DIR_PATH) {
		strlcpy__(file_path, cur_fv->cur_dir, MAX_PATH_LEN);
	}
flf_d_printf("[%s]\n", file_path);
	return 1;		// file or dir is entered
}

PRIVATE int check_filer_cur_dir(void)
{
	// check if cur_dir is readable
	if (is_dir_readable(cur_fv->cur_dir) == 0) {
		// current directory is not readable or disappeared
		while (is_dir_readable(cur_fv->cur_dir) == 0) {
			// go up to the root dir
			do_parent_directory();
////			if (strcmp(cur_fv->cur_dir, cur_fv->next_file) == 0)
////				break;		// the same dir
		}
		tio_beep();
	}
	if (is_dir_readable(cur_fv->cur_dir) == 0) {
		get_cur_dir(cur_fv->cur_dir);
	}
	change_cur_dir(cur_fv->cur_dir);
	return 0;
}

PRIVATE int update_all_file_list(const char *filter, int force_update)
{
////	disp_status_bar_ing(_("Reading directory..."));
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		update_file_list(cur_fv, filter, force_update);
	} else {
		update_file_list(&cur_filer_views->views[0], filter, force_update);
		update_file_list(&cur_filer_views->views[1], filter, force_update);
	}
	return 0;
}
PRIVATE int update_file_list(filer_view_t *fv, const char *filter, int force_update)
{
	int files = 0;

	if (cur_fv->file_list == NULL
	 || strcmp(fv->listed_dir, fv->cur_dir) != 0
	 || (force_update == 1 && get_files_selected(fv) == 0)
	 || force_update >= 2) {
		files = make_file_list(fv, filter);
		strcpy__(fv->listed_dir, fv->cur_dir);
		force_update = 1;
	}
	if (force_update) {
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
	 cur_fv->cur_sel_idx, files_selected, cur_fv->file_list_entries);

	if (GET_APPMD(fl_FILER_PANES) == 0) {		// 1 pane
		win_select_win(WIN_IDX_SUB_WHOLE);
		disp_file_list(cur_fv, 1);
	} else {									// 2 panes
		for (pane_sel_idx = 0; pane_sel_idx < 2; pane_sel_idx++) {
			// 1st, update not current pane.
			// 2nd, update current pane.
			if (pane_sel_idx == 0) {
				pane_idx = cur_filer_views->view_idx == 0 ? 1 : 0;	// not current pane
			} else {
				pane_idx = cur_filer_views->view_idx;				// current pane
			}
			win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
			if (pane_sel_idx == 0) {
				// not current pane
				set_work_space_color_low();
			} else {
				// current pane
				set_work_space_color_normal();
			}
			disp_file_list(&cur_filer_views->views[pane_idx], pane_sel_idx);
		}
	}
	// status bar
	if (status_bar) {
		disp_status_bar_percent_filer(cur_fv->cur_sel_idx, cur_fv->file_list_entries - 1,
		 "%s", file_info_str(&(cur_fv->file_list[cur_fv->cur_sel_idx]), 0, 1, 0));
	}
	// key list
	disp_key_list_filer();

	// Set cursor position
	sub_win_set_cursor_pos(filer_win_get_file_list_y() + cur_fv->cur_sel_idx - cur_fv->top_idx, 0);

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
	char buf_time[HHCMMCSS_LEN+1];

	set_title_bar_color_by_state(0);
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
	snprintf_(buf_dir, MAX_SCRN_LINE_BUF_LEN, "%d%c%s",
	 cur_filer_views->view_idx+1, separator_char, path);

	// current time
	strlcpy__(buf_time, cur_ctime(), HHCMMCSS_LEN);

	//-------------------------------------------------------------------------
	snprintf_(buf_files, MAX_SCRN_LINE_BUF_LEN+1, "%d/%d:%d %c %s",
	 cur_idx, files_total, files_selected, *get_str_sort_by(), buf_time);
	space = LIM_MIN(0, main_win_get_columns() - strnlen(buf_files, MAX_SCRN_LINE_BUF_LEN) - 1);
	shrink_str(buf_dir, space);
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
///flf_d_printf("%d ==> %d\n", cur_sel_idx, fv->top_idx);

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_screen();

	if (filer_win_get_file_path_lines()) {
		// on two pane mode, show each path
		strlcpy__(buffer, fv->cur_dir, MAX_SCRN_LINE_BUF_LEN);
		shrink_str(buffer, main_win_get_columns());
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
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 1);
		} else {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
		}
		if (cur_pane && file_idx == cur_sel_idx) {
			set_color_attrs_rev(1);		// display current line inverted
		}
		sub_win_output_string(filer_win_get_file_list_y() + line_idx, 0, ptr, -1);
		if (cur_pane && file_idx == cur_sel_idx) {
			set_color_attrs_rev(0);
		}
	}
	if (cur_pane) {
		// Set cursor position
		sub_win_set_cursor_pos(filer_win_get_file_list_y() + cur_sel_idx - fv->top_idx, 0);
	}
	return 0;
}
//-------------------------------------
PRIVATE int filer_win_get_file_path_lines(void)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		return 0;
	} else {
		return 1;
	}
}
PRIVATE int filer_win_get_file_list_lines(void)
{
	return LIM_MIN(0, sub_win_get_lines() - filer_win_get_file_path_lines());
}
//-------------------------------------
PRIVATE int filer_win_get_file_path_y(void)
{
	return 0;
}
PRIVATE int filer_win_get_file_list_y(void)
{
	return filer_win_get_file_path_y() + filer_win_get_file_path_lines();
}
//-------------------------------------
PRIVATE void disp_key_list_filer(void)
{
	char *filer_key_lists[] = {
 "{Menu}  {Edit } {Copy } {CpyUd} {Renam}  {Move } {Delet} {MkDel} {MkDir} "
 " {ChDir} {Exec } {Run  } {Home }",

 "<do_quit_filer>Quit "
 "<do_edit_file>Edit "
 "<do_edit_new_file>EditNewFile "
 "<do_copy_file>Copy "
 "<do_copy_file_update>UpdateCopy "
 "<do_rename_file>Rename "
 "<do_move_file>Move "
 "<do_trash_file>Trash "
 "<do_delete_file>Delete "
 "<do_mark_to_delete_file>MarkToDelete "
 "<do_exec_command_with_file>Exec "
 "<do_run_command_rel>Run "
 "<do_select_file>SelectFile "
 "<do_select_no_file>SelNoFile "
 "<do_select_all_files>SelAllFiles ",

 "<do_home_directory>HomeDir "
 "<do_root_directory>RootDir "
 "<do_change_directory>ChgDir "
 "<do_parent_directory>ParentDir "
 "<do_enter_file>Pager/EnterDir "
 "<do_make_directory>MkDir "
 "<do_tog_filer_panes>TwoPane "
 "<do_switch_filer_pane>SwPane "
 "<do_filer_inc_sort_by>Sort "
 "<do_filer_inc_show_file_info>Info "
 "<do_refresh_filer>Refresh "
 "<do_filer_tog_show_dot_file>ShowDotFile "
 "<do_find_file>FindFile ",
	};
	disp_key_list(filer_key_lists);
}

#endif // ENABLE_FILER

// End of filer.c
