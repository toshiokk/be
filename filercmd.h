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

// "dof_..."  DO Editor function
int dof_top_of_list(void);
int dof_bottom_of_list(void);

int dof_switch_filer_pane(void);
int dof_refresh_filer(void);
int dof_tap_file(void);
int dof_edit_file(void);
int dof_edit_file_non_recursive(void);
int dof_edit_new_file(void);
int dof_view_file(void);
int dof_copy_file(void);
int dof_copy_file_update(void);
int dof_rename_file(void);
int dof_move_file(void);
int dof_trash_file(void);
int dof_delete_file(void);
int dof_mark_to_delete_file(void);
int dof_size_zero_file(void);
int dof_find_file(void);
int dof_tail_file(void);
int dof_make_directory(void);
int dof_change_directory(void);
int dof_parent_directory(void);
int dof_beginning_directory(void);
int dof_home_directory(void);
int dof_root_directory(void);
int dof_prev_directory(void);

int dof_select_file(void);
int dof_select_no_file(void);
int dof_select_all_files(void);
int dof_real_path(void);
int dof_quit_filer(void);
int dof_quit_home_dir(void);

int dof_tog_show_dot_file(void);
int dof_inc_show_file_info(void);
int dof_clr_sort_by(void);
int dof_inc_sort_by(void);
int dof_tog_panes(void);
int dof_inc_key_list_lines(void);
int dof_display_color_pairs(void);
#ifdef ENABLE_HELP
int dof_filer_splash(void);
#endif // ENABLE_HELP

int dof_filer_menu_0(void);
int dof_filer_menu_1(void);
int dof_filer_menu_2(void);
int dof_filer_menu_3(void);
int dof_filer_menu_4(void);
int dof_filer_menu_5(void);

int goto_last_file_line_col_in_loaded();
int goto_file_line_col_in_loaded(const char *file_path, int line_num, int col_num);

int filer_change_dir_parent(const char *dir);
int filer_change_dir(const char *dir);

#endif // ENABLE_FILER

#endif // filercmd_h

// End of filercmd.h
