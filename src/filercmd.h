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

int chk_inp_str_ret_val_filer(int ret);

// "dof_..."  DO Editor function
void dof_up();
void dof_down();
void dof_page_up();
void dof_page_down();
void dof_top_of_list();
void dof_bottom_of_list();

void dof_refresh_filer();

// current : subject currently selected file to command
// marked  : subject marked files to command
//|function                   | current | marked | logging |
//|---------------------------|---------|--------|---------|
//|dof_tap_file               | v       | -      | --  |
//|dof_view_file              | v       | -      | --  |
//|dof_tail_file              | v       | -      | --  |
//|dof_open_file              | -       | v      | --  |
//|dof_open_file_ro           | -       | v      | --  |
//|dof_open_file_non_recursive| -       | v      | --  |
//|dof_open_file_from_history | -       | v      | --  |
//|dof_open_proj_file         | -       | -      | --  |
//|dof_open_exec_log_file     | -       | -      | --  |
//|dof_open_new_file          | v       | -      | --  |
//|dof_drop_files_to_open     | -       | -      | --  |
//|dof_copy_file              | -       | v      | --  |
//|dof_copy_file_update       | -       | v      | --  |
//|dof_move_file              | -       | v      | --  |
//|dof_move_file_update       | -       | v      | --  |
//|dof_rename_file            | v       | -      | --  |
//|dof_trash_file             | -       | v      | --  |
//|dof_delete_file            | -       | v      | --  |
//|dof_mark_to_delete_file    | -       | v      | --  |
//|dof_size_zero_file         | -       | v      | --  |
//|dof_exec_command_with_file | -       | v      | no  |
//|dof_exec_command_with_files| -       | v      | no  |
//|dof_run_command_rel        | v       | -      | --  |
//|dof_run_command_abs        | v       | -      | --  |
//|dof_run_command_src_dst    | v       | -      | --  |
//|dof_run_command_symlink    | v       | -      | --  |
//|dof_run_command_soon       | --      | --     | yes |
//|doe_run_line_soon          | --      | --     | yes |

void dof_tap_file();
void dof_tap_file_to_enter();
void dof_tap_file_to_enter_add();
void dof_view_file();
void dof_tail_file();

void dof_open_file_recursive();
void dof_open_file_non_recursive();
void dof_open_file_ro();
void dof_open_file_locked();
void dof_open_file_from_history();
void dof_open_proj_file();
void dof_open_exec_log_file();
void dof_open_new_file();
void dof_input_files_to_open();
void dof_drop_files_to_open();

void dof_copy_file_update();
void dof_copy_file();
void dof_copy_file_force_touch();
void dof_copy_file_update_touch();
void dof_drop_files_to_copy();

void dof_move_file();
void dof_move_file_update();
void dof_drop_files_to_move();

void dof_rename_file();
void dof_trash_file();
void dof_delete_file();
void dof_mark_to_delete_file();
void dof_size_zero_file();

void dof_unzip_file();
void dof_zip_file();

void dof_find_file();
void dof_make_directory();
void dof_change_directory();
void dof_parent_directory();
void dof_beginning_directory();
void dof_home_directory();
void dof_root_directory();
void dof_prev_directory();

void dof_enter_file_name();
void dof_enter_file_name_add();
void dof_enter_file_path();
void dof_enter_file_path_add();
void dof_enter_dir_path();
void dof_enter_dir_path_add();

void dof_set_filter();
void dof_select_file();
void dof_select_no_file();
void dof_select_all_files();
void disp_files_selected();

void dof_real_path();
void dof_quit_filer();
void dof_quit_home_dir();
void dof_restart_filer();

void dof_tog_show_dot_file();
void dof_inc_file_view_mode();
void dof_clear_file_sort_mode();
void dof_inc_file_sort_mode();
void dof_tog_show_zebra_striping();
void dof_tog_panes();
void dof_tog_panex();
void dof_inc_key_list_lines();
#ifdef ENABLE_HELP
void dof_splash();
void dof_view_file_list();
void dof_view_func_list();
#endif // ENABLE_HELP

void dof_menu_0();

int goto_dir_in_string(const char *str);
int check_to_change_dir_in_string(const char *str, char* buf_dir);
int try_to_chdir_parent(char* buf_dir);

int filer_change_dir_parent(char *path);
int filer_change_dir(const char *dir);

int change_cur_dir_saving_prev_next(const char *dir);

#endif // ENABLE_FILER

#endif // filercmd_h

// End of filercmd.h
