/**************************************************************************
 *   menu.c                                                               *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
 *   Copyright (C) 2010-2018 PCR                                          *
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

// Editing functions:
// | key    | filer function              | editor function           |
// |--------|-----------------------------|---------------------------|
// | K_e_   | dof_open_file               | --                        |
// | K_E_   | dof_open_file_ro            | --                        |
// | K_M_e  | dof_open_file_non_recursive | doe_open_file             |
// | K_M_E  | dof_open_file_from_history  | doe_open_file_ro          |
// | K_MC_E | dof_open_locked_file        | doe_open_locked_file      |
// | K_M_n  | dof_open_new_file           | doe_open_new_file         |
// | K_M_N  | dof_open_new_file           | doe_open_new_file         |
// | K_MC_N | --                          | doe_tog_show_line_num     |

// Execution functions:
// | key    | filer function              | editor function           |
// |--------|-----------------------------|---------------------------|
// | K_x_   | dof_exec_command_with_file  | --                        |
// | K_X_   | dof_exec_command_with_files | --                        |
// | K_M_x  | dof_run_command_soon        | doe_run_line_soon         |
// | K_M_X  | dof_run_command_soon_w_log  | doe_run_line_soon_w_log   |
// | K_MC_X | --                          | doe_tog_regex             |
// | K_r_   | dof_run_command_rel         | --                        |
// | K_R_   | dof_run_command_abs         | --                        |
// | K_M_r  | --                          | doe_start_rec__cancel_rec |
// | K_M_R  | --                          | --                        |
// | K_MC_R | --                          | doe_tog_show_ruler        |
// | K_M_p  | doe_open_proj_file          | doe_end_rec__playback     |
// | K_M_P  | doe_open_proj_file          | doe_open_proj_file        |
// | K_t_   | dof_run_command_src_dst     | --                        |
// | K_T_   | dof_run_command_symlink     | --                        |

PRIVATE void app_menu_n(int *group_idx_, int *entry_idx_);
PRIVATE void exec_menu_func(int group_idx, int entry_idx);

int editor_menu_n(int grp_idx)
{
	// previous selection in editor
	static int prev_group_idx;
	static int prev_entry_idx;

	if (grp_idx >= 0) {
		prev_group_idx = grp_idx;
		prev_entry_idx = 1;
	}
	app_menu_n(&prev_group_idx, &prev_entry_idx);
	return 0;
}

#ifdef ENABLE_FILER
int filer_menu_n(int grp_idx)
{
	// previous selection in filer
	static int prev_group_idx;
	static int prev_entry_idx;

	if (grp_idx >= 0) {
		prev_group_idx = grp_idx;
		prev_entry_idx = 1;
	}
	app_menu_n(&prev_group_idx, &prev_entry_idx);
	return 0;
}
#endif // ENABLE_FILER

PRIVATE void app_menu_n(int *group_idx_, int *entry_idx_)
{
	int group_idx = *group_idx_;
	int entry_idx = *entry_idx_;

	for ( ; ; ) {
		int quit_menu = 0;
		update_screen_app(1, 0);
		tio_set_cursor_on(0);
		disp_drop_down_menu(group_idx, entry_idx, main_win_get_top_win_y(), group_idx * 1);
		tio_refresh();

		//---------------------------
		key_code_t key_input = input_key_loop();
		//---------------------------
		mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
		 '_', key_input, short_key_name_from_key_code(key_input, NULL));

		switch (key_input) {
		case K_LEFT:
			group_idx = group_idx - 1;
			goto app_menu_n_left_right;
		case K_RIGHT:
			group_idx = group_idx + 1;
			goto app_menu_n_left_right;
app_menu_n_left_right:;
			if (group_idx < 0) {
				group_idx = get_groups_in_func_key_table() - 1;
			}
			if (group_idx > get_groups_in_func_key_table() - 1) {
				group_idx = 0;
			}
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_group_entries(group_idx));
			break;
		case K_UP:
			entry_idx = entry_idx - 1;
			goto app_menu_n_up_down;
		case K_DOWN:
			entry_idx = entry_idx + 1;
			goto app_menu_n_up_down;
		case K_PPAGE:
			entry_idx = entry_idx - 5;
			goto app_menu_n_up_down;
		case K_NPAGE:	
			entry_idx = entry_idx + 5;
			goto app_menu_n_up_down;
app_menu_n_up_down:;
			if (entry_idx < 1) {
				entry_idx = get_func_key_group_entries(group_idx);
			}
			if (entry_idx > get_func_key_group_entries(group_idx)) {
				entry_idx = 1;
			}
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_group_entries(group_idx));
			break;
		case K_ENTER:
			exec_menu_func(group_idx, entry_idx);
#ifndef ENABLE_FILER
			if (edit_bufs_count_bufs()) {
				set_menu_key_for_do_app_menu_0();
			}
#else // ENABLE_FILER
			if (GET_APPMD(app_EDITOR_FILER) || edit_bufs_count_bufs()) {
				set_menu_key_for_do_app_menu_0();
			}
#endif // ENABLE_FILER
			quit_menu = 1;
			break;
		case K_ESC:
		case K_M_ESC:
			key_input = KEY_NONE;
			quit_menu = 1;
			break;
		default:
			if (get_app_function_for_key(key_input) != NULL) {
				set_menu_key(key_input);
			}
			quit_menu = 1;
			break;
		}
		if (quit_menu) {
			break;
		}
	}
#ifndef ENABLE_FILER
	if (edit_bufs_count_bufs()) {
		update_screen_app(1, 1);
	}
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) || edit_bufs_count_bufs()) {
		update_screen_app(1, 1);
	}
#endif // ENABLE_FILER

	tio_set_cursor_on(1);
	*group_idx_ = group_idx;
	*entry_idx_ = entry_idx;
}

int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx)
{
	func_key_list_t *fkey_list;
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char template_[] = "%-32s  %-*s %-*s  %-12s";
	char buffer[MAX_PATH_LEN+1];

	if ((fkey_list = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return 0;
	}
	for (int f_idx = 0; ; f_idx++) {
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(yy + f_idx, xx, " ", -1);
		if (fkey_list[f_idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		if (fkey_list[f_idx].desc[0]) {
			if (f_idx == entry_idx) {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
			} else {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
			}
		}
		if (! (f_idx > 0 && fkey_list[f_idx].desc[0] == 0)) {
			snprintf(buffer, MAX_PATH_LEN+1, template_,
			 fkey_list[f_idx].explanation,
			 MAX_KEY_NAME_LEN, (fkey_list[f_idx].desc[0] == 0) ? "Key1"
			  : short_key_name_from_key_code(fkey_list[f_idx].keys[0], buf1),
			 MAX_KEY_NAME_LEN, (fkey_list[f_idx].desc[0] == 0) ? "Key2"
			  : short_key_name_from_key_code(fkey_list[f_idx].keys[1], buf2),
			 (fkey_list[f_idx].desc[0] == 0) ? "state"
			  : fkey_list[f_idx].func_get());
flf_d_printf("[%s]\n", buffer);
		} else {
			snprintf(buffer, MAX_PATH_LEN+1, template_,
			 "",
			 MAX_KEY_NAME_LEN, "",
			 MAX_KEY_NAME_LEN, "",
			 "");
		}
		main_win_output_string(-1, -1, buffer, -1);
		if (fkey_list[f_idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(-1, -1, " ", -1);
		if (f_idx > 0 && fkey_list[f_idx].desc[0] == 0) {
			break;
		}
	}
	return 0;
}

int get_groups_in_func_key_table(void)
{
	func_key_list_t *app_func_key_table = get_app_func_key_table();
	int group_idx = 0;
	for (int f_idx = 0; app_func_key_table[f_idx].explanation[0]; f_idx++) {
		if (app_func_key_table[f_idx].desc[0] == 0) {
			group_idx++;
		}
	}
	return group_idx;
}

int get_func_key_group_entries(int group_idx)
{
	func_key_list_t *fkey_list;
	if ((fkey_list = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return 0;
	}
	int f_idx;
	for (f_idx = 1; fkey_list[f_idx].desc[0]; f_idx++) {
		// loop
	}
	return f_idx - 1;
}

key_code_t get_func_key_code(int group_idx, int entry_idx)
{
	func_key_list_t *fkey_list;
	if ((fkey_list = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return -1;
	}
	return fkey_list[entry_idx].keys[0];
}

PRIVATE void exec_menu_func(int group_idx, int entry_idx)
{
	func_key_list_t *fkey_list;
	if ((fkey_list = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return;
	}
	flf_d_printf("[[[[ CALL_FUNC_MENU [%s]\n", fkey_list[entry_idx].func_id);
	fkey_list[entry_idx].func();
	flf_d_printf("]]]]\n");
}

func_key_list_t *get_func_key_group_from_group_idx(int group_idx)
{
	func_key_list_t *app_func_key_table = get_app_func_key_table();
	for (int f_idx = 0; app_func_key_table[f_idx].explanation[0]; f_idx++) {
		if (app_func_key_table[f_idx].desc[0] == 0) {
			if (group_idx == 0) {
				return &app_func_key_table[f_idx];
			}
			group_idx--;
		}
	}
	return NULL;
}

// End of menu.c
