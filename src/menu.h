/**************************************************************************
 *   menu.h                                                               *
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

#ifndef menu_h
#define menu_h

#define FUNC_ID(func)	func, #func
#define F_I(func)		FUNC_ID(func)

typedef enum {
	NORMAL_MODE = 0,	// Normal-mode
	LIST_MODE = 1,		// List-mode
} list_mode_t;

typedef enum {				// | Normal-mode  | List-mode    |
							// |--------------|--------------|
	EF0M = 0x00,			// | --           | --           |
	EFNM = 0x01,			// | executable   |not executable| executable only in Normal mode
	EFLM = 0x02,			// |not executable| executable   | executable only in List mode
	EFAM = (EFNM | EFLM),	// | executable   | executable   | executable in All mode
} list_mode_mask_t;
#define MASK_FROM_LIST_MODE(list_mode)	(((list_mode) == NORMAL_MODE) ? EFNM : EFLM)

typedef struct {
	list_mode_mask_t list_mode;		// executable in list mode
	char *desc;						// short description
	char *explanation;				// explanation
#define MAX_KEYS_BIND	4
	key_code_t keys[MAX_KEYS_BIND];
	void (*func)();
	char *func_id;
	const char *(*func_get)();	// function to get assigned value
} func_key_t;

#define MAX_KEY_NAME_LEN	8		// "@^$RIGHT"
typedef struct /*key_name_table_t*/ {
	key_code_t key_code;
	char *key_name;
} key_name_table_t;
extern key_name_table_t key_name_table[];

const char *key_name_from_key_code(key_code_t key_code);
key_code_t key_code_from_key_name(const char *key_name);

#define MAX_KEY_STR_LEN	(1+MAX_KEY_NAME_LEN+1)	// "(@^$RIGHT)"

func_key_t *get_app_func_key_table();

void conv_func_id_to_key_names(char *func_id, int max_keys);

void *get_app_function_for_key(key_code_t key);
const char *get_func_id_from_key(key_code_t key);
func_key_t *get_fkey_entry_from_key(func_key_t *func_key, key_code_t key,
 list_mode_mask_t list_mode);
int is_key_assigned_to_func(key_code_t key, func_key_t *func_key);
key_code_t get_key_for_func_id(const char *func_id);
func_key_t *get_fkey_entry_from_func_id(const char *func_id, list_mode_mask_t list_mode);

int is_fkey_entry_executable(func_key_t *func_key, list_mode_mask_t list_mode);

void clear_fkey_tbl_using_these_keys(key_code_t *keys);
void clear_key_if_bound_to_func(key_code_t key, func_key_t *func_key);
void clear_fkey_tbl_keys(func_key_t *func_key);
void bind_key_to_func(func_key_t *func_key, key_code_t *keys);

int editor_menu_n(int group_idx);
int filer_menu_n(int group_idx);

func_key_t *get_func_key_group_from_group_idx(int group_idx);
int call_func_key_func(func_key_t *func_key);

void set_menu_key_to_executed(key_code_t key);
key_code_t get_menu_key_to_execute();

#ifdef ENABLE_DEBUG
#ifdef START_UP_TEST
int check_multiple_assignment_of_key();
int check_all_functions_accessible_without_function_key();
#endif // START_UP_TEST
#endif // ENABLE_DEBUG

#endif // menu_h

// End of menu.h
