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

PRIVATE void app_menu_loop(int *group_idx_, int *entry_idx_);

PRIVATE int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx);
PRIVATE void disp_drop_down_menu_line(int group_idx, int menu_y, int menu_lines,
 int entry_idx, int fx_idx, int entries, int yy, int xx);
PRIVATE int drop_down_menu_text(int group_idx, int menu_y, int menu_lines,
 int entry_idx, int fx_idx, int entries, char *buffer);
PRIVATE int get_groups_in_func_key_table();
PRIVATE int get_func_key_group_entries_executable(int group_idx);
PRIVATE func_key_t *get_func_key_executable_by_idx(int group_idx, int fx_idx);
PRIVATE key_code_t get_key_from_menu_entry(int group_idx, int entry_idx);
PRIVATE int exec_menu_func(int group_idx, int entry_idx);

int editor_menu_n(int grp_idx)
{
	// previous selection in editor
	static int prev_group_idx = 0;
	static int prev_entry_idx = 1;

	if (grp_idx >= 0) {
		prev_group_idx = grp_idx;
		prev_entry_idx = 1;
	}
	app_menu_loop(&prev_group_idx, &prev_entry_idx);
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
	app_menu_loop(&prev_group_idx, &prev_entry_idx);
	return 0;
}
#endif // ENABLE_FILER

// How to get rid of operability among key-macro and pull-down-menu
// - does not record during the menu opening
// - record only the keys on:
//   - a key which opens a menu
//   - a key which is triggered by menu

PRIVATE void app_menu_loop(int *group_idx_, int *entry_idx_)
{
	int group_idx = *group_idx_;
	int entry_idx = *entry_idx_;	// 1 -- "number of menu entries"

	key_macro_delete_last_key();

	key_code_t key_input = K_VALID;
	for ( ; ; ) {
		int quit_menu = 0;
		update_screen_app(IS_KEY_INVALID(key_input) ? S_B_AUTO : S_B_CURS, 0);
		tio_set_cursor_on(0);
		disp_drop_down_menu(group_idx, entry_idx, central_win_get_top_win_y(), group_idx * 1);
		tio_refresh();

		//---------------------------------------------
		key_input = input_key_timeout_no_rec();
		//---------------------------------------------
		hmflf_dprintf("input%ckey:0x%04x(%s)================\n",
		 '_', (UINT16)key_input, get_key_name_from_key_code(key_input, NULL));

		switch (key_input) {
		case K_LEFT:
			group_idx = group_idx - 1;
			goto app_menu_loop_left_right;
		case K_RIGHT:
			group_idx = group_idx + 1;
			goto app_menu_loop_left_right;
app_menu_loop_left_right:;
			if (group_idx < 0) {
				group_idx = get_groups_in_func_key_table() - 1;
			}
			if (group_idx > get_groups_in_func_key_table() - 1) {
				group_idx = 0;
			}
			entry_idx = MIN_MAX_(1, entry_idx,
			 get_func_key_group_entries_executable(group_idx)-1);
			break;
		case K_UP:
			entry_idx = entry_idx - 1;
			goto app_menu_loop_up_down;
		case K_DOWN:
			entry_idx = entry_idx + 1;
			goto app_menu_loop_up_down;
		case K_PPAGE:
			entry_idx = entry_idx - 5;
			goto app_menu_loop_up_down;
		case K_NPAGE:	
			entry_idx = entry_idx + 5;
			goto app_menu_loop_up_down;
app_menu_loop_up_down:;
			if (entry_idx < 1) {
				entry_idx = get_func_key_group_entries_executable(group_idx)-1;
			}
			if (entry_idx >= get_func_key_group_entries_executable(group_idx)) {
				entry_idx = 1;
			}
			entry_idx = MIN_MAX_(1, entry_idx,
			 get_func_key_group_entries_executable(group_idx)-1);
			break;
		case K_ESC:
		case K_M_ESC:
			key_input = KEY_NONE;
			quit_menu = 1;
			break;
		default:
			const char *func_id = get_func_id_from_key(key_input);
			if (strcmp(func_id, "doe_carriage_return") == 0) {
				key_input = get_key_from_menu_entry(group_idx, entry_idx);
				if (IS_KEY_VALID(key_input)) {
					// execute the function by the key after returning to editor or filer
					set_menu_key_to_executed(key_input);
					quit_menu = 1;
				} else {
					// execute the function in editor or filer directly
					exec_menu_func(group_idx, entry_idx);
				}
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
	if (count_edit_bufs() || (GET_APPMD(app_EDITOR_FILER) == EF_FILER)) {
		update_screen_app(S_B_CURS, 1);
	}
#endif // ENABLE_FILER

	tio_set_cursor_on(1);
	*group_idx_ = group_idx;
	*entry_idx_ = entry_idx;
}

PRIVATE int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx)
{
	// menu contents:
	//  "File Menu"    None None state  ^ 0        
	//  "Item-1"       @e   @E   --     | 1        
	//  "Item-2"                        | :        
	//  "Item-3"                        | :        
	//  "Item-4"       ^Q   @Q          | :        
	//  "Item-5"       ^Q   @Q          | :        
	//  "Item-6"       ^Q   @Q          v entries-1
	//  ""                                entries  
	//
	// menu display:
	// +-File-Menu-----Key1-Key1-state-+  ^ 1           : top bar  
	// | Open file     @e   @E   --    |  |                        
	// |                               |  |                        
	// |                               |  |                        
	// | Close fil     ^Q   @Q         |  |                        
	// +-------------------------------+  v menu_lines : bottom bar
	int entries = get_func_key_group_entries_executable(group_idx);
	int menu_lines = MIN_(entries + 1, central_win_get_lines());
	int shift = MAX_(0, entry_idx - (menu_lines-2));
	for (int menu_y = 0; menu_y < menu_lines; menu_y++) {
		int fx_idx = 0;
		if (menu_y == 0) {
			fx_idx = 0;		// top bar
		} else {
			fx_idx = menu_y + shift;
		}
		disp_drop_down_menu_line(group_idx, menu_y, menu_lines,
		 entry_idx, fx_idx, entries, yy, xx);
	}
	return 0;
}

PRIVATE void disp_drop_down_menu_line(int group_idx, int menu_y, int menu_lines,
 int entry_idx, int fx_idx, int entries, int yy, int xx)
{
	char buffer[MAX_PATH_LEN+1];

	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(yy + menu_y, xx, " ", -1);	// left border

	int selected = drop_down_menu_text(group_idx, menu_y, menu_lines,
	 entry_idx, fx_idx, entries, buffer);
	switch (selected) {
	case 4:
		set_item_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
		break;
	case 3:
		set_item_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED_DISABLED, 0);
		break;
	case 2:
		set_item_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		break;
	case 1:
		set_item_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM_DISABLED, 0);
		break;
	case 0:
	default:
		set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		break;
	}
	central_win_output_string(-1, -1, buffer, -1);

	set_item_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	central_win_output_string(-1, -1, " ", -1);				// right border
}
PRIVATE int drop_down_menu_text(int group_idx, int menu_y, int menu_lines,
 int entry_idx, int fx_idx, int entries, char *buffer)
{
	char template_[] = " %-32s  %-*s %-*s  %-12s ";
	int selected = 0;
	func_key_t *func_key = get_func_key_executable_by_idx(group_idx, fx_idx);

	if (menu_y == 0) {
		// +-File-Menu-----Key1-Key1-state-+
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 func_key->explanation, MAX_KEY_NAME_LEN, "Key1", MAX_KEY_NAME_LEN, "Key2",
		 "state");
	} else
	if (menu_y < menu_lines-1) {
		if (fx_idx == entry_idx) {
			if (is_fkey_entry_executable(func_key, EF0M)) {
				selected = 4;
			} else {
				selected = 3;
			}
		} else {
			if (is_fkey_entry_executable(func_key, EF0M)) {
				selected = 2;
			} else {
				selected = 1;
			}
		}
		char buf1[MAX_KEY_NAME_LEN+1];
		char buf2[MAX_KEY_NAME_LEN+1];
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 func_key->explanation,
		 MAX_KEY_NAME_LEN, get_key_name_from_key_code(func_key->keys[0], buf1),
		 MAX_KEY_NAME_LEN, get_key_name_from_key_code(func_key->keys[1], buf2),
		 func_key->func_get());
	} else {
		// +-------------------------------+
		snprintf(buffer, MAX_PATH_LEN+1, template_,
		 fx_idx < entries ? ":" : "",
		 MAX_KEY_NAME_LEN, "",
		 MAX_KEY_NAME_LEN, "",
		 "");
	}
	return selected;
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

PRIVATE int get_func_key_group_entries_executable(int group_idx)
{
	func_key_t *func_key;
	if ((func_key = get_func_key_group_from_group_idx(group_idx)) == NULL) {
		return 0;
	}
	int entries = 0;
	for (int f_idx = 0; (f_idx == 0) || func_key[f_idx].desc[0]; f_idx++) {
		entries++;
	}
	return entries;		// number of lines executable in normal mode
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

PRIVATE func_key_t *get_func_key_executable_by_idx(int group_idx, int fx_idx)
{
	func_key_t *func_key = get_func_key_group_from_group_idx(group_idx);
	int fx_cnt = 0;		// count of entries executable
	int f_idx;
	for (f_idx = 0; (f_idx == 0) || func_key[f_idx].desc[0]; f_idx++) {
		if (fx_cnt >= fx_idx) {
			break;
		}
		fx_cnt++;
	}
	return &func_key[f_idx];
}
PRIVATE key_code_t get_key_from_menu_entry(int group_idx, int entry_idx)
{
	func_key_t *func_key;
	if ((func_key = get_func_key_executable_by_idx(group_idx, entry_idx)) == NULL) {
		return KEY_NONE;
	}
	if (is_fkey_entry_executable(func_key, EF0M) == 0) {
		disp_status_bar_err(_("Can not execute this function in the current mode: [%s]"),
		 func_key->func_id);
		return KEY_NONE;
	}
	return func_key->keys[0];
}

PRIVATE int exec_menu_func(int group_idx, int entry_idx)
{
	func_key_t *func_key;
	if ((func_key = get_func_key_executable_by_idx(group_idx, entry_idx)) == NULL) {
		return 0;
	}
	if (is_fkey_entry_executable(func_key, EF0M) == 0) {
		disp_status_bar_err(_("Can not execute this function in List mode: [%s]"),
		 func_key->func_id);
		return 0;
	} else {
		flf_dprintf("<< CALL_FUNC_MENU [%s]\n", func_key->func_id);
		//=========================
		call_func_key_func(func_key);	// call function "do?_...()"
		//=========================
		flf_dprintf(">> CALL_FUNC_MENU [%s]\n", func_key->func_id);
	}
	return 1;
}
int call_func_key_func(func_key_t *func_key)
{
	if (func_key->func == NULL) {
		return -1;
	}
	(*func_key->func)();	// call function "do?_...()"
	return 1;
}

//------------------------------------------------------------------------------
PRIVATE key_code_t menu_key = -1;
void set_menu_key_to_executed(key_code_t key)
{
	menu_key = key;
}
key_code_t get_menu_key_to_execute()
{
	key_code_t key = K_NONE;
	if (menu_key >= 0) {
		key = menu_key;
		menu_key = K_NONE;
	}
	return key;
}

//==============================================================================
func_key_t *get_app_func_key_table()
{
#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == EF_EDITOR) {
#endif // ENABLE_FILER
		return editor_func_key_table;
#ifdef ENABLE_FILER
	} else {
		return filer_func_key_table;
	}
#endif // ENABLE_FILER
}

//------------------------------------------------------------------------------
// "dof_quit" ==> "q/^q/@q"
void conv_func_id_to_key_names(char *func_id, int max_keys)
{
#define MAX_KEY_NAMES_LEN	(MAX_KEY_NAME_LEN * MAX_KEYS_BIND + 1 * (MAX_KEYS_BIND-1))
	char key_names[MAX_KEY_NAMES_LEN + 1] = "";			// "KEY1|KEY2|KEY3"
	func_key_t *func_key = get_fkey_entry_from_func_id(func_id, EF0M);
	if (func_key == NULL) {
		return;
	}
	for (int key_idx = 0; key_idx < max_keys; key_idx++) {
		if (func_key->keys[key_idx] != KNA) {
			const char *key_name = get_key_name_from_key_code(func_key->keys[key_idx], NULL);
			if (is_strlen_not_0(key_names)) {
#define KEY_SEPERATOR	"/"
				strlcat__(key_names, MAX_KEY_NAMES_LEN, KEY_SEPERATOR);
			}
			strlcat__(key_names, MAX_KEY_NAMES_LEN, key_name);
		}
	}
	if (is_strlen_not_0(key_names)) {
		// copy into func_id and return it to the caller
		strlcpy__(func_id, key_names, MAX_KEY_NAMES_LEN);
	}
}

void *get_app_function_for_key(key_code_t key)
{
	func_key_t *func_key = get_fkey_entry_from_key(NULL, key, EFNM);
	if (func_key) {
		return (void *)func_key->func;
	}
	return NULL;
}
const char *get_func_id_from_key(key_code_t key)
{
	// First, search from editor function keys
	func_key_t *func_key = get_fkey_entry_from_key(editor_func_key_table, key, EFNM);
	if (func_key) {
		return func_key->func_id;
	}
#ifdef ENABLE_FILER
	// Second, search from filer function keys
	func_key = get_fkey_entry_from_key(filer_func_key_table, key, EFNM);
	if (func_key) {
		return func_key->func_id;
	}
#endif // ENABLE_FILER
	return "";
}
func_key_t *get_fkey_entry_from_key(func_key_t *func_key, key_code_t key,
 list_mode_mask_t list_mode)
{
	if (func_key == NULL) {
		func_key = get_app_func_key_table();
	}
	for (int f_idx = 0; func_key[f_idx].explanation[0]; f_idx++) {
		if (is_key_assigned_to_func(key, &func_key[f_idx])
		 && is_fkey_entry_executable(&func_key[f_idx], list_mode)) {
			return &func_key[f_idx];
		}
	}
	return NULL;
}

int is_key_assigned_to_func(key_code_t key, func_key_t *func_key)
{
	if (key != KNA) {
		for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
			if (key == func_key->keys[key_idx])
				return 1;
		}
	}
	return 0;
}

key_code_t get_key_for_func_id(const char *func_id)
{
	func_key_t *func_key = get_fkey_entry_from_func_id(func_id, EF0M);
	if (func_key == NULL) {
		return K_NONE;
	}
	// return a key which is not K_Fxx
	return (IS_BETWEEN(K_F01, func_key->keys[0], K_F12) == 0)
	 ? func_key->keys[0] : func_key->keys[1];
}

PRIVATE func_key_t *get_fkey_entry_from_func_id__(func_key_t *func_key,
 const char *func_id, list_mode_mask_t list_mode);

func_key_t *get_fkey_entry_from_func_id(const char *func_id, list_mode_mask_t list_mode)
{
	func_key_t *func_key = get_fkey_entry_from_func_id__(editor_func_key_table,
	 func_id, list_mode);
	if (func_key) {
		return func_key;
	}
#ifdef ENABLE_FILER
	func_key = get_fkey_entry_from_func_id__(filer_func_key_table, func_id, list_mode);
	if (func_key) {
		return func_key;
	}
#endif // ENABLE_FILER
	return NULL;
}
PRIVATE func_key_t *get_fkey_entry_from_func_id__(func_key_t *func_key,
 const char *func_id, list_mode_mask_t list_mode)
{
	for (int f_idx = 0; func_key[f_idx].explanation[0]; f_idx++) {
		if ((strcmp(func_key[f_idx].func_id, func_id) == 0)
		 && is_fkey_entry_executable(&func_key[f_idx], list_mode)) {
			return &func_key[f_idx];
		}
	}
	return NULL;
}
int is_fkey_entry_executable(func_key_t *func_key, list_mode_mask_t list_mode)
{
	// list_mode:
	//   0: normal_mode
	//   1: list mode
	//  -1: depend on is_app_viewer_mode()
	if (list_mode == EF0M) {
		list_mode = MASK_FROM_LIST_MODE(is_app_viewer_mode());
	}
	if (list_mode == EFNM) {
		// normal mode
		switch (func_key->list_mode) {
		default:
		case EFAM:		return 1;		// executable
		case EFNM:		return 1;		// executable
		case EFLM:		return 0;		// not executable
		}
	} else {
		// list mode
		switch (func_key->list_mode) {
		default:
		case EFAM:		return 1;		// executable
		case EFNM:		return 0;		// not executable
		case EFLM:		return 1;		// executable
		}
	}
}

void clear_fkey_tbl_using_these_keys(key_code_t *keys)
{
	func_key_t *func_key = editor_func_key_table;
	for (int f_idx = 0; func_key[f_idx].func != NULL; f_idx++) {
		clear_key_if_bound_to_func(keys[0], &func_key[f_idx]);
		clear_key_if_bound_to_func(keys[1], &func_key[f_idx]);
	}
}
void clear_key_if_bound_to_func(key_code_t key, func_key_t *func_key)
{
	for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
		if (func_key->keys[key_idx] == key) {
			func_key->keys[0] = KNA;
		}
	}
}
void clear_fkey_tbl_keys(func_key_t *func_key)
{
	func_key->keys[0] = KNA;
	func_key->keys[1] = KNA;
}

void bind_key_to_func(func_key_t *func_key, key_code_t *keys)
{
	clear_fkey_tbl_keys(func_key);	// clear keys before assigning keys
	for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
		if (IS_KEY_VALID(keys[key_idx])) {
			func_key->keys[key_idx] = keys[key_idx];
		}
	}
}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
#ifdef START_UP_TEST

PRIVATE int check_multiple_assignment_of_key_(const char *editor_filer, func_key_t *func_key_list);

int check_multiple_assignment_of_key()
{
flf_dprintf("-------------------------\n");
	int err = check_multiple_assignment_of_key_("editor", editor_func_key_table);
#ifdef ENABLE_FILER
	err += check_multiple_assignment_of_key_("filer", filer_func_key_table);
#endif // ENABLE_FILER
	return err;
}
PRIVATE int check_multiple_assignment_of_key_(const char *editor_filer, func_key_t *func_key_list)
{
	for (list_mode_mask_t list_mode = EFNM; list_mode <= EFLM; list_mode++) {
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
					warning_printf("%s: key: [%s] assigned multiple to func:[%s] and [%s]\n",
					 editor_filer, get_key_name_from_key_code(key, NULL),
					 func_key->desc, func_key2->desc);
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
