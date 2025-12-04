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

PRIVATE void app_menu_n(int *group_idx_, int *entry_idx_);
PRIVATE void exec_menu_func(int group_idx, int entry_idx);

PRIVATE int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx);
PRIVATE void disp_drop_down_menu_line(func_key_t *func_key, int menu_y, int menu_lines,
 int entry_idx, int fx_idx, int entries, int yy, int xx);
PRIVATE int get_groups_in_func_key_table();
PRIVATE int get_func_key_group_entries(int group_idx);
PRIVATE func_key_t *get_func_key_executable_by_idx(func_key_t *func_key, int f_ex_idx);

int editor_menu_n(int grp_idx)
{
	// previous selection in editor
	static int prev_group_idx = 0;
	static int prev_entry_idx = 1;

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
	static int prev_group_idx = 0;
	static int prev_entry_idx = 1;

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
	int entry_idx = *entry_idx_;	// 1 -- "number of menu entries"

	key_code_t key_input = K_VALID;
	for ( ; ; ) {
		int quit_menu = 0;
		update_screen_app((key_input == K_NONE) ? S_B_AUTO : S_B_CURS, 0);
		tio_set_cursor_on(0);
		disp_drop_down_menu(group_idx, entry_idx, central_win_get_top_win_y(), group_idx * 1);
		tio_refresh();

		//---------------------------------------------
		key_input = input_key_wait_return();
		//---------------------------------------------
		hmflf_dprintf("input%ckey:0x%04x(%s)================\n",
		 '_', (UINT16)key_input, short_key_name_from_key_code(key_input, NULL));

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
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_group_entries(group_idx)-1);
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
				entry_idx = get_func_key_group_entries(group_idx)-1;
			}
			if (entry_idx >= get_func_key_group_entries(group_idx)) {
				entry_idx = 1;
			}
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_group_entries(group_idx)-1);
			break;
		case K_ENTER:
			exec_menu_func(group_idx, entry_idx);
#ifndef ENABLE_FILER
			if (count_edit_bufs()) {
				set_menu_key(get_key_for_func_id("doe_menu_0"));
			}
#else // ENABLE_FILER
			if ((GET_APPMD(app_EDITOR_FILER) == EF_FILER) || count_edit_bufs()) {
				set_menu_key(get_key_for_func_id("doe_menu_0"));
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
			if (IS_KEY_VALID(key_input)) {
				if (get_app_function_for_key(key_input) != NULL) {
					set_menu_key(key_input);
				}
				quit_menu = 1;
			}
			break;
		}
		if (quit_menu) {
			break;
		}
	}
#ifndef ENABLE_FILER
	if (count_edit_bufs()) {
		update_screen_app(S_B_CURS, 1);
	}
#else // ENABLE_FILER
	if ((GET_APPMD(app_EDITOR_FILER) == EF_FILER) || count_edit_bufs()) {
		update_screen_app(S_B_CURS, 1);
	}
#endif // ENABLE_FILER

	tio_set_cursor_on(1);
	*group_idx_ = group_idx;
	*entry_idx_ = entry_idx;
}

PRIVATE int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx)
{
	func_key_t *func_key;
	if ((func_key = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return 0;
	}
	// menu contents:
	//  "File Menu"    None None state  ^            0
	//  "Item-1"       @e   @E   --     |            1
	//  "Item-2"                        |            :
	//  "Item-3"                        |            :
	//  "Item-4"       ^Q   @Q          |            :
	//  "Item-5"       ^Q   @Q          |            :
	//  "Item-6"       ^Q   @Q          v entries  entries-1
	//  ""                                         entries
	//
	// menu display:
	// ##File#Menu#####Key1#Key1#state##  ^            : top bar
	// # Open file     @e   @E   --    #  |
	// #                               #  |
	// #                               #  |
	// # Close fil     ^Q   @Q         #  |
	// #################################  v menu_lines : bottom bar
	int entries = get_func_key_group_entries(group_idx);
	int menu_lines = MIN_(entries + 1, central_win_get_lines());
	int shift = MAX_(0, entry_idx - (menu_lines-2));
	for (int menu_y = 0; menu_y < menu_lines; menu_y++) {
		int fx_idx = 0;
		if (menu_y == 0) {
			fx_idx = 0;		// top bar
		} else {
			fx_idx = menu_y + shift;
		}
		disp_drop_down_menu_line(func_key, menu_y, menu_lines, entry_idx, fx_idx, entries, yy, xx);
	}
	return 0;
}

PRIVATE void disp_drop_down_menu_line(func_key_t *func_key, int menu_y, int menu_lines,
 int entry_idx, int fx_idx, int entries, int yy, int xx)
{
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char template_[] = " %-32s  %-*s %-*s  %-12s ";
	char buffer[MAX_PATH_LEN+1];

	func_key = get_func_key_executable_by_idx(func_key, fx_idx);
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(yy + menu_y, xx, " ", -1);
	if (menu_y == 0) {
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 func_key->explanation, MAX_KEY_NAME_LEN, "Key1", MAX_KEY_NAME_LEN, "Key2",
		 "state");
	} else
	if (menu_y < menu_lines-1) {
		set_item_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		if (fx_idx == entry_idx) {
			set_item_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
		}
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 func_key->explanation,
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(func_key->keys[0], buf1),
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(func_key->keys[1], buf2),
		 func_key->func_get());
	} else {
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 fx_idx < entries ? ":" : "",
		 MAX_KEY_NAME_LEN, "",
		 MAX_KEY_NAME_LEN, "",
		 "");
	}
	central_win_output_string(-1, -1, buffer, -1);
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(-1, -1, " ", -1);
}

PRIVATE int get_groups_in_func_key_table()
{
	func_key_t *app_func_key_table = get_app_func_key_table();
	int group_idx = 0;
	for (int f_idx = 0; app_func_key_table[f_idx].explanation[0]; f_idx++) {
		if (app_func_key_table[f_idx].desc[0] == 0) {
			group_idx++;
		}
	}
	return group_idx;
}

PRIVATE int get_func_key_group_entries(int group_idx)
{
	func_key_t *func_key;
	if ((func_key = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return 0;
	}
	int entries = 0;
	for (int f_idx = 0; func_key[f_idx].desc[0] || (f_idx == 0); f_idx++) {
		if (is_fkey_entry_executable(&func_key[f_idx], -1)) {
			entries++;
		}
	}
	return entries;		// number of lines executable in normal mode
}
PRIVATE func_key_t *get_func_key_executable_by_idx(func_key_t *func_key, int fx_idx)
{
	int fx_cnt = 0;		// count of entries executable
	int f_idx;
	for (f_idx = 0; (func_key[f_idx].desc[0] || (f_idx == 0)) && (fx_cnt < fx_idx); f_idx++) {
		if (is_fkey_entry_executable(&func_key[f_idx], -1)) {
			fx_cnt++;
		}
	}
	return &func_key[f_idx];
}


PRIVATE void exec_menu_func(int group_idx, int entry_idx)
{
	func_key_t *func_key;
	if ((func_key = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return;
	}
	flf_dprintf("[[[[ CALL_FUNC_MENU [%s]\n", func_key[entry_idx].func_id);
	func_key[entry_idx].func();
	flf_dprintf("]]]]\n");
}

func_key_t *get_func_key_group_from_group_idx(int group_idx)
{
	func_key_t *app_func_key_table = get_app_func_key_table();
	for (int f_idx = 0; app_func_key_table[f_idx].explanation[0]; f_idx++) {
		if (app_func_key_table[f_idx].desc[0] == '\0') {
			if (group_idx == 0) {
				return &app_func_key_table[f_idx];
			}
			group_idx--;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
#ifdef START_UP_TEST

PRIVATE int check_multiple_assignment_of_key_(func_key_t *func_key);

int check_multiple_assignment_of_key()
{
flf_dprintf("-------------------------\n");
	int err = check_multiple_assignment_of_key_(editor_func_key_table);
#ifdef ENABLE_FILER
	err += check_multiple_assignment_of_key_(filer_func_key_table);
#endif // ENABLE_FILER
	return err;
}
PRIVATE int check_multiple_assignment_of_key_(func_key_t *func_key_list)
{
  for (int list_mode = 0; list_mode < 2; list_mode++) {
	for (int f_idx = 0; func_key_list[f_idx].explanation[0]; f_idx++) {
		func_key_t *func_key = &func_key_list[f_idx];
		if (is_fkey_entry_executable(func_key, list_mode) == 0)
			continue;
		for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
			key_code_t key = func_key->keys[key_idx];
			if (key == KEY_NONE) {
				continue;
			}
			for (int f_idx2 = f_idx + 1; func_key_list[f_idx2].explanation[0] ; f_idx2++) {
				func_key_t *func_key2 = &func_key_list[f_idx2];
				if (is_fkey_entry_executable(func_key2, list_mode) == 0)
					continue;
				for (int key_idx2 = 0; key_idx2 < MAX_KEYS_BIND; key_idx2++) {
					key_code_t key2 = func_key2->keys[key_idx2];
					if (key2 == key) {
						warning_printf("key: %04x assigned multiple to func:[%s] and [%s]\n",
						 key, func_key2->desc, func_key2->desc);
					}
				}
			}
		}
	}
  }
	return 0;
}

PRIVATE int check_all_functions_accessible_without_function_key_(func_key_t *func_key);

int check_all_functions_accessible_without_function_key()
{
flf_dprintf("-------------------------\n");
	int err = check_all_functions_accessible_without_function_key_(editor_func_key_table);
#ifdef ENABLE_FILER
	err += check_all_functions_accessible_without_function_key_(filer_func_key_table);
#endif // ENABLE_FILER
	return err;
}
PRIVATE int check_all_functions_accessible_without_function_key_(func_key_t *func_key)
{
	for (int f_idx = 0; func_key[f_idx].explanation[0]; f_idx++) {
		int accessible = 0;
		int accessible_without_fkey = 0;
		for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
			key_code_t key = func_key[f_idx].keys[key_idx];
			if (IS_KEY_VALID(key)) {
				accessible++;
				if (IS_BYTE_KEY(key) || IS_META_KEY(key)) {
					accessible_without_fkey++;
				}
			}
		}
		if (accessible && (accessible_without_fkey == 0)) {
			warning_printf("func:[%s] is not accessible without Func-key\n",
			 func_key[f_idx].desc);
		}
	}
	return 0;
}
#endif // START_UP_TEST
#endif // ENABLE_DEBUG

// End of menu.c
