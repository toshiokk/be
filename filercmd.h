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
int dof_up();
int dof_down();
int dof_page_up();
int dof_page_down();
int dof_top_of_list();
int dof_bottom_of_list();

int dof_refresh_filer();

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

int dof_tap_file();
int dof_tap_file_to_enter();
int dof_tap_file_to_enter_add();
int dof_view_file();
int dof_tail_file();

int dof_open_file_recursive();
int dof_open_file_non_recursive();
int dof_open_file_ro();
int dof_open_locked_file();
int dof_open_file_from_history();
int dof_open_proj_file();
int dof_open_exec_log_file();
int dof_open_new_file();
int dof_input_files_to_open();
int dof_drop_files_to_open();

int dof_copy_file();
int dof_copy_file_update();
int dof_drop_files_to_copy();

int dof_move_file();
int dof_move_file_update();
int dof_drop_files_to_move();

#define ACTION_SEL		0	// choose from 'open', 'copy' or 'move'
#define ACTION_INPUT	1	// input file path to open
#define ACTION_OPEN		2
#define ACTION_COPY		3
#define ACTION_MOVE		4
int dof_drop_files_to_do_action_(int action);

int dof_rename_file();
int dof_trash_file();
int dof_delete_file();
int dof_mark_to_delete_file();
int dof_size_zero_file();

int dof_unzip_file();
int dof_zip_file();

int dof_find_file();
int dof_make_directory();
int dof_change_directory();
int dof_parent_directory();
int dof_beginning_directory();
int dof_home_directory();
int dof_root_directory();
int dof_prev_directory();

int dof_enter_file_name();
int dof_enter_file_name_add();
int dof_enter_file_path();
int dof_enter_file_path_add();
int dof_enter_dir_path();
int dof_enter_dir_path_add();

int dof_set_filter();
int dof_select_file();
int dof_select_no_file();
int dof_select_all_files();
void disp_files_selected();

int dof_real_path();
int dof_quit_filer();
int dof_quit_home_dir();
int dof_restart_filer();

int dof_tog_show_dot_file();
int dof_inc_file_view_mode();
int dof_clear_file_sort_mode();
int dof_inc_file_sort_mode();
int dof_tog_show_zebra_striping();
int dof_tog_panes();
int dof_tog_panex();
int dof_inc_key_list_lines();
#ifdef ENABLE_HELP
int dof_splash();
int dof_view_file_list();
int dof_view_func_list();
#endif // ENABLE_HELP

int dof_menu_0();

int goto_dir_in_string(const char *str);
int check_to_change_dir_in_string(const char *str, char* buf_dir);
int try_to_chdir_parent(char* buf_dir);

int filer_change_dir_parent(char *path);
int filer_change_dir(const char *dir);

int change_cur_dir_saving_prev_next(const char *dir);

#endif // ENABLE_FILER

#endif // filercmd_h

// End of filercmd.h
