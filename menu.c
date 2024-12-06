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
	key_code_t key_input;
	int group_idx = *group_idx_;
	int entry_idx = *entry_idx_;

	for ( ; ; ) {
		int quit_menu = 0;
		update_screen_app(1, 0);
		tio_set_cursor_on(0);
		disp_drop_down_menu(group_idx, entry_idx, main_win_get_top_win_y(), group_idx * 1);
		tio_refresh();

		//---------------------------
		key_input = input_key_loop();
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
				group_idx = get_func_key_table_from_key_groups() - 1;
			}
			if (group_idx > get_func_key_table_from_key_groups() - 1) {
				group_idx = 0;
			}
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_table_from_key_entries(group_idx));
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
				entry_idx = get_func_key_table_from_key_entries(group_idx);
			}
			if (entry_idx > get_func_key_table_from_key_entries(group_idx)) {
				entry_idx = 1;
			}
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_table_from_key_entries(group_idx));
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
	func_key_table_t *fkey_table;
	int idx;
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char menu_templ[] = "%-32s  %-8s %-8s  %-12s";
	char buffer[MAX_PATH_LEN+1];

	if ((fkey_table = get_func_key_table_from_key_group(group_idx)) == NULL) {
		return 0;
	}
	for (idx = 0; ; idx++) {
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(yy + idx, xx, " ", -1);
		if (fkey_table[idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		if (fkey_table[idx].desc[0]) {
			if (idx == entry_idx) {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
			} else {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
			}
		}
		if (idx != 0 && fkey_table[idx].desc[0] == 0) {
			snprintf(buffer, MAX_PATH_LEN+1, menu_templ, "", "", "", "");
		} else {
			snprintf(buffer, MAX_PATH_LEN+1, menu_templ,
			 fkey_table[idx].help,
			 short_key_name_from_key_code(fkey_table[idx].key1, buf1),
			 short_key_name_from_key_code(fkey_table[idx].key2, buf2),
			 fkey_table[idx].func_get ? (*fkey_table[idx].func_get)() : "");
		}
		main_win_output_string(-1, -1, buffer, -1);
		if (fkey_table[idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(-1, -1, " ", -1);
		if (idx != 0 && fkey_table[idx].desc[0] == 0) {
			break;
		}
	}
	return 0;
}

int get_func_key_table_from_key_groups(void)
{
	func_key_table_t *app_func_key_table = get_app_func_key_table();
	int group_idx = 0;
	for (int idx = 0; app_func_key_table[idx].help[0]; idx++) {
		if (app_func_key_table[idx].desc[0] == 0) {
			group_idx++;
		}
	}
	return group_idx;
}

int get_func_key_table_from_key_entries(int group_idx)
{
	func_key_table_t *fkey_table;
	if ((fkey_table = get_func_key_table_from_key_group(group_idx)) == NULL) {
		return 0;
	}
	int idx;
	for (idx = 1; fkey_table[idx].desc[0]; idx++) {
		// loop
	}
	return idx - 1;
}

key_code_t get_func_key_code(int group_idx, int entry_idx)
{
	func_key_table_t *fkey_table;

	if ((fkey_table = get_func_key_table_from_key_group(group_idx)) == NULL) {
		return -1;
	}
	return fkey_table[entry_idx].key1;
}

PRIVATE void exec_menu_func(int group_idx, int entry_idx)
{
	func_key_table_t *fkey_table;
	if ((fkey_table = get_func_key_table_from_key_group(group_idx)) == NULL) {
		return;
	}
	flf_d_printf("[[[[ CALL_FUNC_MENU [%s]\n", fkey_table[entry_idx].func_id);
	fkey_table[entry_idx].func();
	flf_d_printf("]]]]\n");
}

func_key_table_t *get_func_key_table_from_key_group(int group_idx)
{
	func_key_table_t *app_func_key_table = get_app_func_key_table();
	for (int idx = 0; app_func_key_table[idx].help[0]; idx++) {
		if (app_func_key_table[idx].desc[0] == 0) {
			if (group_idx == 0) {
				return &app_func_key_table[idx];
			}
			group_idx--;
		}
	}
	return NULL;
}

// End of menu.c
