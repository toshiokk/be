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
	dir_history_fix();
#endif // ENABLE_HISTORY
	disp_status_bar_done(_("File view refreshed"));
	SET_filer_do_next(FL_UPDATE_FORCE);
}
//------------------------------------------------------------------------------
void dof_enter_file_name()     { SET_filer_do_next(((input_string_full_path == 0)
 ? FL_ENTER_FILE_NAME : FL_ENTER_FILE_PATH)); }
void dof_enter_file_name_add() { SET_filer_do_next(((input_string_full_path == 0)
 ? FL_ENTER_FILE_NAME_ADD : FL_ENTER_FILE_PATH_ADD)); }
void dof_enter_file_path()     { SET_filer_do_next(FL_ENTER_FILE_PATH); }
void dof_enter_file_path_add() { SET_filer_do_next(FL_ENTER_FILE_PATH_ADD); }
void dof_enter_dir_path()      { SET_filer_do_next(FL_ENTER_DIR_PATH); }
void dof_enter_dir_path_add()  { SET_filer_do_next(FL_ENTER_DIR_PATH_ADD); }
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
	get_cur_fv_cur_file_info()->selected = get_cur_fv_cur_file_info()->selected ^ _FILE_SEL_MAN_;
	dof_down();
	disp_files_selected();
}

#define MY_ISREG		0x01
#define MY_ISDIR		0x02
#define MY_ISLNK		0x04
PRIVATE void _dof_select_all_files(mode_t type);
void dof_select_no_file()
{
	_dof_select_all_files(0);
}
void dof_select_all_files()
{
	_dof_select_all_files(MY_ISREG | MY_ISDIR | MY_ISLNK);
}
void dof_select_all_regular_files()
{
	_dof_select_all_files(MY_ISREG);
}
PRIVATE void _dof_select_all_files(mode_t type)
{
	for (int file_idx = 0 ; file_idx < get_cur_fv_file_info_entries(); file_idx++) {
		if ((type == 0)
		 || (strcmp(get_cur_fv_file_name(file_idx), ".") == 0)
		 || (strcmp(get_cur_fv_file_name(file_idx), "..") == 0)) {
			get_cur_fv_file_info(file_idx)->selected = 0;
		} else {
			mode_t ftype = 0;	// (S_IFREG or S_IFDIR) and S_IFLNK are exclusive
			mode_t mode = get_cur_fv_file_info(file_idx)->lst.st_mode;
			if (S_ISLNK(mode))		ftype |= MY_ISLNK;
			if (S_ISREG(mode))		ftype |= MY_ISREG;
			if (S_ISDIR(mode))		ftype |= MY_ISDIR;
			if (ftype & type) {
				get_cur_fv_file_info(file_idx)->selected
				 = get_cur_fv_file_info(file_idx)->selected ^ _FILE_SEL_MAN_;
			}
		}
	}
	if (type == 0) {
		disp_status_bar_done(_("File selection cleared"));
	} else {
		disp_files_selected();
	}
}

void disp_files_selected()
{
#ifdef ENABLE_HISTORY
	dir_history_fix();
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

void dof_find_file()
{
	char file_path[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", file_path, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_DIR,
	 _("Find file:")))) {
		return;
	}
	strlcpy__(get_fv_from_cur_pane()->next_file, file_path, MAX_PATH_LEN);
	SET_filer_do_next(FL_UPDATE_FORCE);
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
void dof_tog_show_zebra_striping()
{
	_dof_tog_show_zebra_striping();
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
#ifdef ENABLE_HISTORY
	dir_history_fix();
#endif // ENABLE_HISTORY
	tog_filer_panex();
	SHOW_MODE("Filer pane index", get_str_filer_pane_num());
	SET_filer_do_next(FL_UPDATE_FORCE);
}
void dof_inc_key_list_lines()
{
	_dof_inc_key_list_lines();
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
