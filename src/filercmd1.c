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

PRIVATE void select_all_files(mode_t type);
PRIVATE void start_selecting_files();
PRIVATE void select_from_start_to_cur();
PRIVATE void disp_files_selected();

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"
#define BEUNZIP		"beunzip"
#define BEZIP		"bezip"

int chk_inp_str_ret_val_filer(int ret)
{
flf_dprintf("ret__[%s]\n", get_ef_name(ret));
	if (ret <= EF_TO_QUIT) {
		// EF_CANCELLED/EF_TO_QUIT ==> EF_NONE so that filer does not quit
		SET_filer_do_next(EF_NONE);
		return 1;
	} else {
		SET_filer_do_next(ret);
	}
	return IS_EF_ENTER_STRING(ret) ? 0 : 1;	// 0: something input
}

void dof_up()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv)--;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv)--;
	normalize_filer_cursor_y(fv);
	select_from_start_to_cur();
}
void dof_down()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv)++;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv)++;
	normalize_filer_cursor_y(fv);
	select_from_start_to_cur();
}
void dof_page_up()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) -= filer_vert_scroll_lines(),
	normalize_filer_cur_file_idx(fv);
///	FV_CURS_Y(fv) -= filer_vert_scroll_lines(),
	normalize_filer_cursor_y(fv);
	select_from_start_to_cur();
}
void dof_page_down()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) += filer_vert_scroll_lines(),
	normalize_filer_cur_file_idx(fv);
///	FV_CURS_Y(fv) += filer_vert_scroll_lines(),
	normalize_filer_cursor_y(fv);
	select_from_start_to_cur();
}
void dof_top_of_list()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) = 0;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv) = 0;
	normalize_filer_cursor_y(fv);
	select_from_start_to_cur();
}
void dof_bottom_of_list()
{
	filer_view_t *fv = get_fv_from_cur_pane();
	FV_CUR_F_IDX(fv) = get_cfv_file_info_entries()-1;
	normalize_filer_cur_file_idx(fv);
	FV_CURS_Y(fv) = filer_win_get_file_list_lines()-1;
	normalize_filer_cursor_y(fv);
	select_from_start_to_cur();
}

void dof_refresh_filer()
{
	disp_status_bar_done(_("File view refreshed"));
	SET_filer_do_next(FL_UPDATE_FORCE);
}
//------------------------------------------------------------------------------
PRIVATE void set_files_to_output_buf_filer(int file0_path1_dir2);
void dof_enter_file_name()
{
	set_files_to_output_buf_filer(input_file_name_0_file_path_1);
	SET_filer_do_next(EF_ENTER_STRING);
}
void dof_enter_file_name_add()
{
	set_files_to_output_buf_filer(input_file_name_0_file_path_1);
	SET_filer_do_next(EF_ENTER_STRING_ADD);
}
void dof_enter_file_path()
{
	set_files_to_output_buf_filer(1);
	SET_filer_do_next(EF_ENTER_STRING);
}
void dof_enter_file_path_add()
{
	set_files_to_output_buf_filer(1);
	SET_filer_do_next(EF_ENTER_STRING_ADD);
}
void dof_enter_dir_path()
{
	set_files_to_output_buf_filer(2);
	SET_filer_do_next(EF_ENTER_STRING);
}
void dof_enter_dir_path_add()
{
	set_files_to_output_buf_filer(2);
	SET_filer_do_next(EF_ENTER_STRING_ADD);
}
PRIVATE void set_files_to_output_buf_filer(int file0_path1_dir2)
{
	char path_buf[MAX_PATH_LEN+1] = "";
	switch (file0_path1_dir2) {
	default:
	case 0:
	case 1:
		for (int file_idx = get_first_file_idx_selected(); file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (file0_path1_dir2 == 0) {
				// enter file names: file-1 "file 2" "file 3"
				concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN,
				 get_cfv_file_name(file_idx));
				if (get_cfv_symlink(file_idx)) {
					concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN,
					 get_cfv_symlink(file_idx));
				}
			} else {
				// enter file paths: /path/to/file-1 "/path/to/file 2" "/path/to/file 3"
				concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN,
				 concat_dir_and_file(NULL,
				 get_fv_from_cur_pane()->cur_dir, get_cfv_file_name(file_idx)));
				if (get_cfv_symlink(file_idx)) {
					concat_file_path_separating_by_space(path_buf, MAX_PATH_LEN,
					 get_cfv_symlink(file_idx));
				}
			}
		}
		set_text_to_output_buf_filer(path_buf);
		break;
	case 2:
		// enter dir-path: /path/to/directory
		set_text_to_output_buf_filer(add_trailing_slash_for_handling(
		 get_fv_from_cur_pane()->cur_dir));
		break;
	}
}
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
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_select_file()
{
flf_dprintf("get_cfv_files_selected() %d\n", get_cfv_files_selected());
	set_cfv_file_selected(-1, get_cfv_file_selected(-1) ^ _FILE_SEL_MAN_);
flf_dprintf("get_cfv_files_selected() %d\n", get_cfv_files_selected());
	dof_down();
	disp_files_selected();
flf_dprintf("get_cfv_files_selected() %d\n", get_cfv_files_selected());
}
void dof_select_no_file()
{
	select_all_files(0);
}
void dof_select_all_files()
{
	select_all_files(MY_ISREG | MY_ISDIR | MY_ISLNK);
}
void dof_select_all_regular_files()
{
	select_all_files(MY_ISREG);
}
PRIVATE void select_all_files(mode_t type)
{
	for (int file_idx = 0 ; file_idx < get_cfv_file_info_entries(); file_idx++) {
		if (is_cfv_file_selectable(file_idx, type) == 0) {
			set_cfv_file_selected(file_idx, _FILE_SEL_NONE_);
		} else {
			set_cfv_file_selected(file_idx,
			 get_cfv_file_selected(file_idx) ^ _FILE_SEL_MAN_);
		}
	}
	if (type == 0) {
		disp_status_bar_done(_("File selection cleared"));
	} else {
		disp_files_selected();
	}
}
void dof_toggle_selecting_files()
{
	if (is_selecting_files() == 0) {
		start_selecting_files();
	} else {
		stop_selecting_files();
		SET_filer_do_next(FL_UPDATE_FORCE);
	}
}
int is_selecting_files()
{
	return FV_SEL_F_IDX(get_fv_from_cur_pane()) >= 0;
}
PRIVATE void start_selecting_files()
{
	FV_SEL_F_IDX(get_fv_from_cur_pane()) = FV_CUR_F_IDX(get_fv_from_cur_pane());
	select_from_start_to_cur();
}
void stop_selecting_files()
{
	FV_SEL_F_IDX(get_fv_from_cur_pane()) = -1;
	select_all_files(0);
}
PRIVATE void select_from_start_to_cur()
{
	if (is_selecting_files() == 0)
		return;
	for (int file_idx = 0; file_idx < get_cfv_file_info_entries(); file_idx++) {
		set_cfv_file_selected(file_idx,
		 (is_cfv_file_selectable(file_idx, MY_ISREG | MY_ISDIR | MY_ISLNK)
		   && IS_BETWEEN_TWO(
			   FV_SEL_F_IDX(get_fv_from_cur_pane()),
			   file_idx,
			   FV_CUR_F_IDX(get_fv_from_cur_pane()))
		 ) ? _FILE_SEL_MAN_ : _FILE_SEL_NONE_);
	}
	disp_files_selected();
}

PRIVATE void disp_files_selected()
{
	size_t size_selected, size_total;
	int files_selected = get_cfv_files_selected_size(&size_selected, &size_total);
	char buf_size_selected[6+1] = "";
	char buf_size_total[6+1] = "";
	get_file_size_str(buf_size_selected, size_selected);
	get_file_size_str(buf_size_total, size_total);
	disp_status_bar_done(P_(_("%d file selected, size: %s, total size: %s"),
							_("%d files selected, size: %s, total size: %s"),
							_("%d filess selected, size: %s, total size: %s"),
							_("%d filesss selected, size: %s, total size: %s"),
	 files_selected), files_selected, buf_size_selected, buf_size_total);
}

PRIVATE void _dof_find_file(int first0_next1, int dir);
void dof_find_file_first_backw()
{
	_dof_find_file(0, -1);
}
void dof_find_file_first_forw()
{
	_dof_find_file(0, +1);
}
void dof_find_file_next_backw()
{
	_dof_find_file(1, -1);
}
void dof_find_file_next_forw()
{
	_dof_find_file(1, +1);
}
PRIVATE void _dof_find_file(int first0_next1, int dir)
{
	if (first0_next1 == 0) {
		char file_name[MAX_PATH_LEN+1];
		if (chk_inp_str_ret_val_filer(input_string_pos("", file_name, MAX_PATH_LEN,
		 HISTORY_TYPE_IDX_SEARCH,
		 (dir < 0) ? _("Find file backward:") : _("Find file forward:")))) {
			return;
		}
		set_last_searched_needle(file_name);
	}

	disp_status_bar_ing((dir < 0)
	  ? _("Searching file name [%s] forward")
	  : _("Searching file name [%s] backward"),
	 get_last_searched_needle());

	filer_view_t *fv = get_fv_from_cur_pane();
	int prev_file_idx = FV_CUR_F_IDX(fv);
	if (search_file_from_list(get_last_searched_needle(), first0_next1, dir) < 0) {
		disp_status_bar_err(_("No such file name:[%s] found"), get_last_searched_needle());
	} else {
		disp_status_bar_done(_("Such file name:[%s] found"), get_last_searched_needle());
	}
	FV_CURS_Y(fv) = FV_CURS_Y(fv) + (FV_CUR_F_IDX(fv) - prev_file_idx);
	normalize_filer_cursor_y(fv);
	SET_filer_do_next(FL_UPDATE_AUTO);
}

void dof_tog_show_dot_file()
{
	_dof_tog_show_dot_file();
	SET_filer_do_next(FL_UPDATE_FORCE);
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
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_inc_file_sort_mode()
{
	inc_file_sort_mode();
	SHOW_MODE("File sort mode", get_str_file_sort_mode());
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_tog_zebra_striping()
{
	_dof_tog_zebra_striping();
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_tog_panes()
{
	tog_filer_panes();
	SHOW_MODE("Filer panes", get_str_filer_panes());
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_tog_panex()
{
	tog_filer_panex();
	SHOW_MODE("Filer pane index", get_str_filer_panex());
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_inc_key_list_lines()
{
	_dof_inc_key_list_lines();
}
void dof_tog_map_key_7f_bs()
{
	tog_map_key_7f_bs();
	SHOW_MODE("MAP_KEY_7F_BS", get_str_map_key_7f_bs());
}

#ifdef ENABLE_HELP
void dof_splash()
{
	do_splash();

	SET_filer_do_next(FL_UPDATE_FORCE);
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
