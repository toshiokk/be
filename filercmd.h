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
int dof_up(void);
int dof_down(void);
int dof_page_up(void);
int dof_page_down(void);
int dof_top_of_list(void);
int dof_bottom_of_list(void);

int dof_refresh_filer(void);

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
//|dof_run_command_soon       | --      | --     | --  |
//|dof_run_command_soon_w_log | --      | --     | yes |
//|doe_run_line_soon          | --      | --     | --  |
//|doe_run_line_soon_w_log    | --      | --     | yes |

int dof_tap_file(void);
int dof_view_file(void);
int dof_tail_file(void);

int dof_open_file_recursive(void);
int dof_open_file_non_recursive(void);
int dof_open_file_ro(void);
int dof_open_locked_file(void);
int dof_open_file_from_history(void);
int dof_open_proj_file(void);
int dof_open_exec_log_file(void);
int dof_open_new_file(void);
int dof_input_files_to_open(void);
int dof_drop_files_to_open(void);

int dof_copy_file(void);
int dof_copy_file_update(void);
int dof_drop_files_to_copy(void);

int dof_move_file(void);
int dof_move_file_update(void);
int dof_drop_files_to_move();

#define ACTION_SEL		0	// choose from 'open', 'copy' or 'move'
#define ACTION_INPUT	1	// input file path to open
#define ACTION_OPEN		2
#define ACTION_COPY		3
#define ACTION_MOVE		4
int dof_drop_files_to_do_action_(int action);

int dof_rename_file(void);
int dof_trash_file(void);
int dof_delete_file(void);
int dof_mark_to_delete_file(void);
int dof_size_zero_file(void);

int dof_unzip_file(void);
int dof_zip_file(void);

int dof_find_file(void);
int dof_make_directory(void);
int dof_change_directory(void);
int dof_parent_directory(void);
int dof_beginning_directory(void);
int dof_home_directory(void);
int dof_root_directory(void);
int dof_prev_directory(void);

int dof_set_filter(void);
int dof_select_file(void);
int dof_select_no_file(void);
int dof_select_all_files(void);
void disp_files_selected();

int dof_real_path(void);
int dof_quit_filer(void);
int dof_quit_home_dir(void);

int dof_tog_show_dot_file(void);
int dof_inc_file_view_mode(void);
int dof_clear_file_sort_mode(void);
int dof_inc_file_sort_mode(void);
int dof_tog_show_zebra_striping(void);
int dof_tog_panes(void);
int dof_tog_panex(void);
int dof_inc_key_list_lines(void);
#ifdef ENABLE_HELP
int dof_splash(void);
int dof_view_file_list(void);
int dof_view_func_list(void);
#endif // ENABLE_HELP

int dof_menu_0(void);

int goto_dir_in_string(const char *str);
int check_to_change_dir_in_string(const char *str, char* buf_dir);
int try_to_chdir_parent(char* buf_dir);

int filer_change_dir_parent(char *path);
int filer_change_dir(char *dir);

int change_cur_dir_saving_prev_next(const char *dir);

#endif // ENABLE_FILER

#endif // filercmd_h

// End of filercmd.h
