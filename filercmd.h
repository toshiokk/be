/**************************************************************************
 *   filercmd.h                                                           *
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

#ifndef filercmd_h
#define filercmd_h

#ifdef ENABLE_FILER

int do_top_of_list(void);
int do_bottom_of_list(void);

int do_switch_filer_pane(void);
int do_refresh_filer(void);
int do_enter_file(void);
int do_edit_file(void);
int do_edit_file_non_recursive(void);
int do_edit_new_file(void);
int do_view_file(void);
int do_copy_file(void);
int do_copy_file_update(void);
int do_rename_file(void);
int do_move_file(void);
int do_trash_file(void);
int do_delete_file(void);
int do_mark_to_delete_file(void);
int do_size_zero_file(void);
int do_find_file(void);
int do_tail_file(void);
int do_make_directory(void);
int do_change_directory(void);
int do_parent_directory(void);
int do_beginning_directory(void);
int do_home_directory(void);
int do_root_directory(void);
int do_prev_directory(void);

int do_select_file(void);
int do_select_no_file(void);
int do_select_all_files(void);
int do_real_path(void);
int do_quit_filer(void);
int do_quit_home_dir(void);

int do_filer_tog_show_dot_file(void);
int do_filer_inc_show_file_info(void);
int do_filer_inc_sort_by(void);
int do_tog_filer_panes(void);
int do_filer_inc_key_list_lines(void);
int do_filer_display_color_pairs(void);
#ifdef ENABLE_HELP
int do_filer_splash(void);
#endif // ENABLE_HELP

int do_filer_menu_0(void);
int do_filer_menu_1(void);
int do_filer_menu_2(void);
int do_filer_menu_3(void);
int do_filer_menu_4(void);
int do_filer_menu_5(void);

int goto_last_file_line_col_in_loaded();
int goto_file_line_col_in_loaded(const char *file_path, int line_num, int col_num);

int filer_change_dir_parent(const char *dir);
int filer_change_dir(const char *dir);

#endif // ENABLE_FILER

#endif // filercmd_h

// End of filercmd.h
