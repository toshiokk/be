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

	for ( ; ; ) {
		int quit_menu = 0;
		update_screen_app(1, 0);
		tio_set_cursor_on(0);
		disp_drop_down_menu(group_idx, entry_idx, central_win_get_top_win_y(), group_idx * 1);
		tio_refresh();

		//---------------------------
		key_code_t key_input = input_key_wait_return();
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
				set_menu_key_for_do_app_menu_0();
			}
#else // ENABLE_FILER
			if (GET_APPMD(app_EDITOR_FILER) || count_edit_bufs()) {
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
		update_screen_app(1, 1);
	}
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) || count_edit_bufs()) {
		update_screen_app(1, 1);
	}
#endif // ENABLE_FILER

	tio_set_cursor_on(1);
	*group_idx_ = group_idx;
	*entry_idx_ = entry_idx;
}

PRIVATE void disp_drop_down_menu_line(func_key_list_t *fkey_list,
 int menu_y, int f_idx, int entry_idx, int yy, int xx);

int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx)
{
	func_key_list_t *fkey_list;
	if ((fkey_list = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return 0;
	}
	// menu contents:
	//  "File Menu"    None None state  ^
	//  "Item-1"       @e   @E   --     |
	//  "Item-2"                        |
	//  "Item-3"                        |
	//  "Item-4"       ^Q   @Q          |
	//  "Item-5"       ^Q   @Q          |
	//  "Item-6"       ^Q   @Q          v items
	//  ""                             
	// menu display:
	// ##File#Menu#####Key1#Key1#state##  ^            : top bar
	// # Open file     @e   @E   --    #  |
	// #                               #  |
	// #                               #  |
	// # Close fil     ^Q   @Q         #  |
	// #################################  v menu_lines : bottom bar
	int items = get_func_key_group_entries(group_idx);
	int menu_lines = MIN_(items + 1, central_win_get_lines());
	int shift = MAX_(0, entry_idx - (menu_lines-2));
flf_d_printf("items:%d menu_lines:%d entry_idx:%d shift:%d\n",
 items, menu_lines, entry_idx, shift);
	for (int menu_y = 0; menu_y < menu_lines; menu_y++) {
		int f_idx = 0;
		if (menu_y == 0) {
			f_idx = 0;		// top bar
		} else if (menu_y < menu_lines - 1) {
			f_idx = menu_y + shift;
		} else {
			f_idx = items;	// bottom bar
		}
flf_d_printf("menu_y:%d menu_lines:%d\n", menu_y, menu_lines);
		disp_drop_down_menu_line(fkey_list, menu_y, f_idx,
		 (menu_y < menu_lines-1) ? entry_idx : (((shift + menu_lines) <= items) ? -1 : 0),
		 yy, xx);
	}
	return 0;
}

PRIVATE void disp_drop_down_menu_line(func_key_list_t *fkey_list,
 int menu_y, int f_idx, int entry_idx, int yy, int xx)
{
flf_d_printf("menu_y:%d f_idx:%d entry_idx:%d, desc:[%s]\n",
 menu_y, f_idx, entry_idx, fkey_list[f_idx].desc);
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char template_[] = " %-32s  %-*s %-*s  %-12s ";
	char buffer[MAX_PATH_LEN+1];

	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(yy + menu_y, xx, " ", -1);
	if ((menu_y == 0) || fkey_list[f_idx].desc[0]) {
		if (fkey_list[f_idx].desc[0]) {
			set_item_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
			if (f_idx == entry_idx) {
				set_item_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
			}
		}
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 fkey_list[f_idx].explanation,
		 MAX_KEY_NAME_LEN, (fkey_list[f_idx].desc[0] == 0) ? "Key1"
		  : short_key_name_from_key_code(fkey_list[f_idx].keys[0], buf1),
		 MAX_KEY_NAME_LEN, (fkey_list[f_idx].desc[0] == 0) ? "Key2"
		  : short_key_name_from_key_code(fkey_list[f_idx].keys[1], buf2),
		 (fkey_list[f_idx].desc[0] == 0) ? "state"
		  : fkey_list[f_idx].func_get());
	} else {
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 entry_idx < 0 ? ":" : "",
		 MAX_KEY_NAME_LEN, "",
		 MAX_KEY_NAME_LEN, "",
		 "");
	}
flf_d_printf("[%s]\n", buffer);
	central_win_output_string(-1, -1, buffer, -1);
	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(-1, -1, " ", -1);
}

int get_groups_in_func_key_table()
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
	return f_idx;		// number of lines including menu group title such as "File Menu"
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
		if (app_func_key_table[f_idx].desc[0] == '\0') {
			if (group_idx == 0) {
				return &app_func_key_table[f_idx];
			}
			group_idx--;
		}
	}
	return NULL;
}

// End of menu.c
