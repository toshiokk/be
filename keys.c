/**************************************************************************
 *   keys.c                                                               *
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

//-----------------------------------------------------------------------------

func_key_table_t *app_func_key_table = editor_func_key_table;
PRIVATE void app_menu_n(int *group_idx_, int *entry_idx_);

void set_app_func_key_table(void)
{
#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
#endif // ENABLE_FILER
		app_func_key_table = editor_func_key_table;
#ifdef ENABLE_FILER
	} else {
		app_func_key_table = filer_func_key_table;
	}
#endif // ENABLE_FILER
}

int editor_menu_n(int grp_idx)
{
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
	int again_ret;
	int group_idx = *group_idx_;
	int entry_idx = *entry_idx_;

app_menu_n_again:;
	for ( ; ; ) {
		again_ret = 0;
		update_screen_app(1, 1, 0);
		disp_drop_down_menu(group_idx, entry_idx, main_win_get_top_win_y(), group_idx * 2);
		tio_refresh();

		tio_set_cursor_on(0);
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
			if (group_idx < 0)
				group_idx = get_func_key_table_from_key_groups() - 1;
			if (group_idx > get_func_key_table_from_key_groups() - 1)
				group_idx = 0;
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_table_from_key_entries(group_idx));
			again_ret = 1;
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
			if (entry_idx < 1)
				entry_idx = get_func_key_table_from_key_entries(group_idx);
			if (entry_idx > get_func_key_table_from_key_entries(group_idx))
				entry_idx = 1;
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_table_from_key_entries(group_idx));
			break;
		case K_ENTER:
			exec_func(group_idx, entry_idx);
#ifndef ENABLE_FILER
			if (edit_bufs_count_bufs())
#else // ENABLE_FILER
			if (edit_bufs_count_bufs() || GET_APPMD(app_EDITOR_FILER))
#endif // ENABLE_FILER
				set_menu_key_for_do_app_menu_0();
			again_ret = 2;
			break;
		case K_ESC:
		case K_M_ESC:
			key_input = KEY_NONE;
			again_ret = 2;
			break;
		default:
			if (get_app_function_for_key(key_input) != NULL)
				set_menu_key(key_input);
			again_ret = 2;
			break;
		}
		if (again_ret)
			break;
	}
	if (again_ret == 1)
		goto app_menu_n_again;

#ifndef ENABLE_FILER
	if (edit_bufs_count_bufs()) {
		update_screen_app(1, 1, 1);
	}
#else // ENABLE_FILER
	if (edit_bufs_count_bufs() || GET_APPMD(app_EDITOR_FILER)) {
		update_screen_app(1, 1, 1);
	}
#endif // ENABLE_FILER

	tio_set_cursor_on(1);
	*group_idx_ = group_idx;
	*entry_idx_ = entry_idx;
}

int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx)
{
	func_key_table_t *table;
	int idx;
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char template[] = "%-32s  %-5s %-5s  %-12s";
	char buffer[MAX_PATH_LEN+1];

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return 0;
	for (idx = 0; ; idx++) {
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(yy + idx, xx, " ", -1);
		if (table[idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		if (table[idx].desc[0]) {
			if (idx == entry_idx) {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
			} else {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
			}
		}
		if (idx != 0 && table[idx].desc[0] == 0)
			snprintf(buffer, MAX_PATH_LEN+1, template, "", "", "", "");
		else
			snprintf(buffer, MAX_PATH_LEN+1, template,
			 table[idx].help,
			 short_key_name_from_key_code(table[idx].key1, buf1),
			 short_key_name_from_key_code(table[idx].key2, buf2),
			 table[idx].func_get ? (*table[idx].func_get)() : "" );
		main_win_output_string(-1, -1, buffer, -1);
		if (table[idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(-1, -1, " ", -1);
		if (idx != 0 && table[idx].desc[0] == 0)
			break;
	}
	return 0;
}

int get_func_key_table_from_key_groups(void)
{
	int idx;
	int group_idx;

	group_idx = 0;
	for (idx = 0; app_func_key_table[idx].help[0]; idx++) {
		if (app_func_key_table[idx].desc[0] == 0) {
			group_idx++;
		}
	}
	return group_idx;
}

int get_func_key_table_from_key_entries(int group_idx)
{
	int idx;
	func_key_table_t *table;

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return 0;
	for (idx = 1; table[idx].desc[0]; idx++) {
		// loop
	}
	return idx - 1;
}

key_code_t get_func_key_code(int group_idx, int entry_idx)
{
	func_key_table_t *table;

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return -1;
	return table[entry_idx].key1;
}

void exec_func(int group_idx, int entry_idx)
{
	func_key_table_t *table;

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return;
	table[entry_idx].func();
}

func_key_table_t *get_func_key_table_from_key_group(int group_idx)
{
	int idx;

	for (idx = 0; app_func_key_table[idx].help[0]; idx++) {
		if (app_func_key_table[idx].desc[0] == 0) {
			if (group_idx == 0)
				return &app_func_key_table[idx];
			group_idx--;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
int cmp_func_id(const char *func_id_1, const char *func_id_2)
{
	return strcmp(func_id_1, func_id_2) == 0;
}
void *get_app_function_for_key(key_code_t key)
{
	func_key_table_t *func_key_table;

	func_key_table = get_func_key_table_from_key(app_func_key_table, key);
	if (func_key_table)
		return (void *)func_key_table->func;
	return NULL;
}
const char *get_func_id_from_key(key_code_t key)
{
	func_key_table_t *func_key_table;

	func_key_table = get_func_key_table_from_key(editor_func_key_table, key);
	if (func_key_table)
		return func_key_table->func_id;
#ifdef ENABLE_FILER
	func_key_table = get_func_key_table_from_key(filer_func_key_table, key);
	if (func_key_table)
		return func_key_table->func_id;
#endif // ENABLE_FILER
	return "";
}
func_key_table_t *get_func_key_table_from_key(func_key_table_t *key_table, key_code_t key)
{
	int idx;

	for (idx = 0; key_table[idx].help[0]; idx++) {
		if (is_key_bound_to_func(key, &key_table[idx])) {
			return &key_table[idx];
		}
	}
	return NULL;
}
key_code_t get_key_for_func_id(char *func_id)
{
	func_key_table_t *func_key_table = get_func_table_from_func_id(func_id);
	if (func_key_table == NULL)
		return 0;
	return func_key_table->key1;
}
PRIVATE func_key_table_t *get_func_table_from_func_id__(func_key_table_t *func_key_table,
 const char *func_id);
func_key_table_t *get_func_table_from_func_id(const char *func_id)
{
	func_key_table_t *func_key_table
	 = get_func_table_from_func_id__(editor_func_key_table, func_id);
	if (func_key_table)
		return func_key_table;
#ifdef ENABLE_FILER
	func_key_table = get_func_table_from_func_id__(filer_func_key_table, func_id);
	if (func_key_table)
		return func_key_table;
#endif // ENABLE_FILER
	return NULL;
}
PRIVATE func_key_table_t *get_func_table_from_func_id__(func_key_table_t *func_key_table,
 const char *func_id)
{
	int idx;

	for (idx = 0; func_key_table[idx].help[0]; idx++) {
		if (strcmp(func_key_table[idx].func_id, func_id) == 0) {
			return &func_key_table[idx];
		}
	}
	return NULL;
}

int is_key_bound_to_func(key_code_t key, func_key_table_t *func_key_table)
{
	return key != KNA
	 && (key == func_key_table->key1
	  || key == func_key_table->key2
	  || key == func_key_table->key3);
}
void clear_keys_if_bound(key_code_t *keys)
{
	func_key_table_t *func_key_table = editor_func_key_table;
	int idx;

	for (idx = 0; func_key_table[idx].func != NULL; idx++) {
		clear_key_if_bound_to_func(keys[0], &func_key_table[idx]);
		clear_key_if_bound_to_func(keys[1], &func_key_table[idx]);
	}
}
void clear_key_if_bound_to_func(key_code_t key, func_key_table_t *func_key_table)
{
	if (func_key_table->key1 == key) {
		func_key_table->key1 = KNA;
	}
	if (func_key_table->key2 == key) {
		func_key_table->key2 = KNA;
	}
	if (func_key_table->key3 == key) {
		func_key_table->key3 = KNA;
	}
}
void clear_keys_bound_to_func(func_key_table_t *func_key_table)
{
	func_key_table->key1 = KNA;
	func_key_table->key2 = KNA;
}

void bind_key_to_func(func_key_table_t *func_key_table, key_code_t *keys)
{
	if (keys[0] >= 0)
		func_key_table->key1 = keys[0];
	if (keys[1] >= 0)
		func_key_table->key2 = keys[1];
	if (keys[2] >= 0)
		func_key_table->key3 = keys[2];
}

// 0x01 ==> "^A"
const char *short_key_name_from_func_id(char *buf)
{
	return short_key_name_from_key_code(get_key_for_func_id(buf), buf);
}

//-----------------------------------------------------------------------------
PRIVATE key_code_t menu_key = -1;
void set_menu_key(key_code_t key)
{
flf_d_printf("set_menu_key(%04x)\n", key);
	menu_key = key;
}
key_code_t get_menu_key(void)
{
	key_code_t key = -1;

	if (menu_key >= 0) {
		key = menu_key;
		menu_key = -1;
flf_d_printf("get_menu_key(%04x)\n", key);
	}
	return key;
}

void set_menu_key_for_do_app_menu_0(void)
{
#ifndef ENABLE_FILER
	set_menu_key(get_key_for_func_id("doe_editor_menu_0"));
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0)
		set_menu_key(get_key_for_func_id("doe_editor_menu_0"));
	else
		set_menu_key(get_key_for_func_id("dof_filer_menu_0"));
#endif // ENABLE_FILER
}

//-----------------------------------------------------------------------------
PRIVATE unsigned long msec_when_input_key = 0;
unsigned long msec_past_input_key()
{
	return get_msec() - msec_when_input_key;
}

PRIVATE key_code_t input_key_timeout(void);
PRIVATE key_code_t input_key_check_break_key(void);
PRIVATE key_code_t map_key_code(key_code_t key);

key_code_t input_key_loop(void)
{
	key_code_t key;

	while ((key = input_key_wait_return()) < 0) {
	}
	return key;
}
key_code_t input_key_wait_return(void)
{
	static key_code_t prev_key = KEY_NONE;
	key_code_t key = input_key_timeout();
	if (key < 0 && prev_key >= 0) {
		tio_repaint_all();
	}
	prev_key = key;
	if (key >= 0) {
		msec_when_input_key = get_msec();
	}
	return key;
}

// Display update durations:
// | type                              | duration [mSec] |
// |-----------------------------------|-----------------|
// | current time update in title bar  | 1000            |
// | recording key macro               |  200            |
// | file list update                  | 1000            |
// | splash screen when screen resized | 1000            |
// | splash screen by key              | infinite        |
//
#define DEFAULT_KEY_WAIT_MSEC		1000	// return every 1[Sec]
#define KEY_MACRO_KEY_WAIT_MSEC		200
PRIVATE key_code_t input_key_timeout(void)
{
	long key_wait_time_msec = DEFAULT_KEY_WAIT_MSEC;
	if (key_macro_is_recording()) {
		key_wait_time_msec = KEY_MACRO_KEY_WAIT_MSEC;
	}
	long msec_enter = get_msec();
	key_code_t key;
	while ((key = input_key_macro()) < 0) {
		if (tio_check_update_terminal_size()) {
			win_reinit_win_size();
			update_screen_app(1, 1, 0);
			disp_status_bar_ing(_("Window resized to (%d, %d)"),
			 tio_get_columns(), tio_get_lines());
			msec_enter = get_msec();	// restart time monitoring
		}
		if ((long)(get_msec() - msec_enter) >= key_wait_time_msec)
			break;
		MSLEEP(10);		// wait 10[mS]
	}
	return key;
}

key_code_t input_key_macro(void)
{
	key_code_t key = -1;

	if ((key = get_menu_key()) >= 0) {
		return key;
	}
#ifndef ENABLE_HISTORY
	key = input_key_check_break_key();
#else
	key = key_macro_get_key();
	if (key < 0) {
		key = input_key_check_break_key();
	}
	key_macro_put_key(key);
#endif // ENABLE_HISTORY
	return key;
}

// Record key strokes pushed while loading large or many files
// and Restore it after finished loading files.
PRIVATE int key_codes_saved = -1;			// >=0: saving,    -1: not recording
PRIVATE int key_codes_restored = -1;		// >=0: restoring, -1: not restoring
#define MAX_KEY_STROKES_CHECK_BREAK_KEY		40
PRIVATE key_code_t key_codes_check_break_key[MAX_KEY_STROKES_CHECK_BREAK_KEY+1];
PRIVATE int is_saving_check_break_key(void);
PRIVATE int is_restoring_check_break_key(void);

PRIVATE int is_saving_check_break_key(void)
{
	return (0 <= key_codes_saved && key_codes_saved < MAX_KEY_STROKES_CHECK_BREAK_KEY)
	 && (is_restoring_check_break_key() == 0);
}
PRIVATE int is_restoring_check_break_key(void)
{
	return (0 <= key_codes_restored && key_codes_restored < key_codes_saved);
}
void begin_check_break_key(void)
{
	key_codes_saved = 0;		// start saving
	key_codes_restored = -1;	// stop restoration
	clear_sigint_signaled();
}
void end_check_break_key(void)
{
	if (key_codes_saved > 0) {
		key_codes_restored = 0;		// start restoring
	} else {
		key_codes_saved = -1;		// clear saved keys
		key_codes_restored = -1;	// stop restoration
	}
}
int check_break_key(void)
{
	if (is_saving_check_break_key()) {
		if (input_key_check_break_key() == K_C_C) {	// Ctrl-C key is break key
			set_sigint_signaled();
flf_d_printf("sigint_signaled\n");
		}
	}
	return is_sigint_signaled();
}
PRIVATE key_code_t input_key_check_break_key(void)
{
	key_code_t key;

	if (is_restoring_check_break_key()) {
		// restoring key strokes
		key = key_codes_check_break_key[key_codes_restored];
		key_codes_restored++;
		if (key_codes_restored >= key_codes_saved) {
			key_codes_saved = -1;		// clear saved keys
			key_codes_restored = -1;	// stop restoration
		}
flf_d_printf("<[%04x]\n", key);
		return key;
	}
	key = tio_input_key();
	key = map_key_code(key);
	if (is_saving_check_break_key()) {
		// saving key strokes
		if (key >= 0) {
			key_codes_check_break_key[key_codes_saved] = key;
			key_codes_saved++;
flf_d_printf(">[%04x]\n", key);
		}
	}
	return key;
}

PRIVATE key_code_t map_key_code(key_code_t key)
{
	switch (key) {
	case CHAR_DEL:
#define MAP_KEY_7F_BS
#ifdef MAP_KEY_7F_BS
		if (GET_APPMD(app_MAP_KEY_7F_BS)) {
flf_d_printf("KEY_7F ==> BS\n");
			key = K_BS;		// CHAR_DEL ==> BS
		} else {
#endif // MAP_KEY_7F_BS
flf_d_printf("KEY_7F ==> DEL\n");
			key = K_DEL;	// CHAR_DEL ==> Delete
#ifdef MAP_KEY_7F_BS
		}
#endif // MAP_KEY_7F_BS
		break;
	case KEY_BACKSPACE:		// 0x0107
#ifdef MAP_KEY_7F_BS
		if (GET_APPMD(app_MAP_KEY_7F_BS) == 0) {
flf_d_printf("KEY_BACKSPACE ==> BS\n");
			key = K_BS;		// KEY_BACKSPACE ==> BS
		} else {
#endif // MAP_KEY_7F_BS
flf_d_printf("KEY_BACKSPACE ==> DEL\n");
			key = K_DEL;	// KEY_BACKSPACE ==> DEL
#ifdef MAP_KEY_7F_BS
		}
#endif // MAP_KEY_7F_BS
		break;
	case KEY_DC:
flf_d_printf("KEY_DC ==> DEL\n");
		// Delete(0x0113) key code was seen.
		SET_APPMD(app_MAP_KEY_7F_BS);	// set conversion of CHAR_DEL ==> BS
		key = K_DEL;
		break;
	case KEY_ENTER:		// 0x0157
		key = K_ENTER;
	default:
		break;
	}
	return key;
}

//-----------------------------------------------------------------------------
key_name_table_t key_name_table[] = {
//					   12345
	{ ' '			, "SP", },
	{ K_BS			, "BS", },
	{ K_TAB			, "TAB", },
	{ K_ENTER		, "ENTER", },
	{ K_ESC			, "ESC", },
	{ K_C_AT		, "C-@", },
	{ K_C_A			, "C-A", },
	{ K_C_B			, "C-B", },
	{ K_C_C			, "C-C", },
	{ K_C_D			, "C-D", },
	{ K_C_E			, "C-E", },
	{ K_C_F			, "C-F", },
	{ K_C_G			, "C-G", },
	{ K_C_H			, "C-H", },
	{ K_C_J			, "C-J", },
	{ K_C_K			, "C-K", },
	{ K_C_L			, "C-L", },
	{ K_C_M			, "C-M", },
	{ K_C_N			, "C-N", },
	{ K_C_O			, "C-O", },
	{ K_C_P			, "C-P", },
	{ K_C_Q			, "C-Q", },
	{ K_C_R			, "C-R", },
	{ K_C_S			, "C-S", },
	{ K_C_T			, "C-T", },
	{ K_C_U			, "C-U", },
	{ K_C_V			, "C-V", },
	{ K_C_W			, "C-W", },
	{ K_C_X			, "C-X", },
	{ K_C_Y			, "C-Y", },
	{ K_C_Z			, "C-Z", },
	{ K_C_LBRACKET	, "C-[", },
	{ K_C_BACKSLASH	, "C-\\", },
	{ K_C_RBRACKET	, "C-]", },
	{ K_C_CARET		, "C-^", },
	{ K_C_UNDERLINE	, "C-_", },
	{ K_M_ESC		, "M-ESC", },
	{ K_M_0			, "M-0", },
	{ K_M_1			, "M-1", },
	{ K_M_2			, "M-2", },
	{ K_M_3			, "M-3", },
	{ K_M_4			, "M-4", },
	{ K_M_5			, "M-5", },
	{ K_M_6			, "M-6", },
	{ K_M_7			, "M-7", },
	{ K_M_8			, "M-8", },
	{ K_M_9			, "M-9", },
	{ K_M_a			, "M-a", },
	{ K_M_b			, "M-b", },
	{ K_M_c			, "M-c", },
	{ K_M_d			, "M-d", },
	{ K_M_e			, "M-e", },
	{ K_M_f			, "M-f", },
	{ K_M_g			, "M-g", },
	{ K_M_h			, "M-h", },
	{ K_M_i			, "M-i", },
	{ K_M_j			, "M-j", },
	{ K_M_k			, "M-k", },
	{ K_M_l			, "M-l", },
	{ K_M_m			, "M-m", },
	{ K_M_n			, "M-n", },
	{ K_M_o			, "M-o", },
	{ K_M_p			, "M-p", },
	{ K_M_q			, "M-q", },
	{ K_M_r			, "M-r", },
	{ K_M_s			, "M-s", },
	{ K_M_t			, "M-t", },
	{ K_M_u			, "M-u", },
	{ K_M_v			, "M-v", },
	{ K_M_w			, "M-w", },
	{ K_M_x			, "M-x", },
	{ K_M_y			, "M-y", },
	{ K_M_z			, "M-z", },
	{ K_M_A			, "M-A", },
	{ K_M_B			, "M-B", },
	{ K_M_C			, "M-C", },
	{ K_M_D			, "M-D", },
	{ K_M_E			, "M-E", },
	{ K_M_F			, "M-F", },
	{ K_M_G			, "M-G", },
	{ K_M_H			, "M-H", },
	{ K_M_I			, "M-I", },
	{ K_M_J			, "M-J", },
	{ K_M_K			, "M-K", },
	{ K_M_L			, "M-L", },
	{ K_M_M			, "M-M", },
	{ K_M_N			, "M-N", },
	{ K_M_O			, "M-O", },
	{ K_M_P			, "M-P", },

	{ K_M_Q			, "M-Q", },
	{ K_M_R			, "M-R", },
	{ K_M_S			, "M-S", },
	{ K_M_T			, "M-T", },
	{ K_M_U			, "M-U", },
	{ K_M_V			, "M-V", },
	{ K_M_W			, "M-W", },
	{ K_M_X			, "M-X", },
	{ K_M_Y			, "M-Y", },
	{ K_M_Z			, "M-Z", },
	{ K_MC_A		, "MC-A", },
	{ K_MC_B		, "MC-B", },
	{ K_MC_C		, "MC-C", },
	{ K_MC_D		, "MC-D", },
	{ K_MC_E		, "MC-E", },
	{ K_MC_F		, "MC-F", },
	{ K_MC_G		, "MC-G", },
	{ K_MC_H		, "MC-H", },
	{ K_MC_I		, "MC-I", },
	{ K_MC_J		, "MC-J", },
	{ K_MC_K		, "MC-K", },
	{ K_MC_L		, "MC-L", },
	{ K_MC_M		, "MC-M", },
	{ K_MC_N		, "MC-N", },
	{ K_MC_O		, "MC-O", },
	{ K_MC_P		, "MC-P", },
	{ K_MC_Q		, "MC-Q", },
	{ K_MC_R		, "MC-R", },
	{ K_MC_S		, "MC-S", },
	{ K_MC_T		, "MC-T", },
	{ K_MC_U		, "MC-U", },
	{ K_MC_V		, "MC-V", },
	{ K_MC_W		, "MC-W", },
	{ K_MC_X		, "MC-X", },
	{ K_MC_Y		, "MC-Y", },
	{ K_MC_Z		, "MC-Z", },
	{ K_F01			, "F01", },
	{ K_F02			, "F02", },
	{ K_F03			, "F03", },
	{ K_F04			, "F04", },
	{ K_F05			, "F05", },
	{ K_F06			, "F06", },
	{ K_F07			, "F07", },
	{ K_F08			, "F08", },
	{ K_F09			, "F09", },
	{ K_F10			, "F10", },
	{ K_F11			, "F11", },
	{ K_F12			, "F12", },
	{ K_S_F01		, "S-F01", },
	{ K_S_F02		, "S-F02", },
	{ K_S_F03		, "S-F03", },
	{ K_S_F04		, "S-F04", },
	{ K_S_F05		, "S-F05", },
	{ K_S_F06		, "S-F06", },
	{ K_S_F07		, "S-F07", },
	{ K_S_F08		, "S-F08", },
	{ K_S_F09		, "S-F09", },
	{ K_S_F10		, "S-F10", },
	{ K_S_F11		, "S-F11", },
	{ K_S_F12		, "S-F12", },
	{ K_INS			, "INS", },
	{ K_DEL			, "DEL", },
	{ K_HOME		, "HOME", },
	{ K_END			, "END", },
	{ K_PPAGE		, "PGUP", },
	{ K_NPAGE		, "PGDN", },
	{ K_UP			, "UP", },
	{ K_DOWN		, "DOWN", },
	{ K_RIGHT		, "RIGHT", },
	{ K_LEFT		, "LEFT", },
};

const char *short_key_name_from_key_code(key_code_t key_code, char *buf)
{
	static char buf_s_[MAX_KEY_NAME_LEN+1];
	if (buf == NULL) {
		buf = buf_s_;
	}
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"
	return short_key_name_from_key_name(key_name_from_key_code(key_code, buf_key_name), buf);
}
const char *key_name_from_key_code(key_code_t key_code, char *buf)
{
	static char buf_s_[MAX_KEY_NAME_LEN+1];
	if (buf == NULL) {
		buf = buf_s_;
	}
	for (int idx = 0; idx < ARRAY_SIZE_OF(key_name_table); idx++) {
		if (key_name_table[idx].key_code == key_code)
			return key_name_table[idx].key_name;
	}
///flf_d_printf("key_code:[%d]\n", key_code);
	if (isgraph(key_code)) {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%c", key_code);
	} else if ((key_code & 0xff00) == K_M(0)) {	// 0x1bxx
		if (isgraph(key_code & 0x00ff)) {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "M-%c", key_code & 0x00ff);
		} else if (0 <= (key_code & 0x00ff) && (key_code & 0x00ff) < 0x20) {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "MC-%c", '@' + (key_code & 0xff));
		} else {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "%04x", key_code);
		}
	} else if (key_code == KEY_NONE) {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "---");
	} else {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%04x", key_code);
	}
	return buf;
}
const char *short_key_name_from_key_name(const char *key_name, char *buf)
{
	if (strlcmp__(key_name, "C-") == 0) {
		// "C-x" ==> "^x"
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%s%s", "^", &key_name[2]);
	} else
	if (strlcmp__(key_name, "M-") == 0) {
		// "M-x" ==> "@x"
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%s%s", "@", &key_name[2]);
	} else
	if (strlcmp__(key_name, "MC-") == 0) {
		// "MC-x" ==> "@^x"
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%s%s", "@^", &key_name[3]);
	} else {
		strlcpy__(buf, key_name, MAX_KEY_NAME_LEN);
	}
	return buf;
}

key_code_t key_code_from_key_name(char *key_name)
{
	int idx;

	for (idx = 0; idx < ARRAY_SIZE_OF(key_name_table); idx++) {
		if (strcmp(key_name_table[idx].key_name, key_name) == 0)
			return key_name_table[idx].key_code;
	}
	return -1;
}
key_code_t key_code_from_short_key_name(char *short_key_name)
{
	int idx;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	for (idx = 0; idx < ARRAY_SIZE_OF(key_name_table); idx++) {
		if (strcmp(short_key_name_from_key_name(key_name_table[idx].key_name, buf_key_name),
		 short_key_name) == 0)
			return key_name_table[idx].key_code;
	}
	return -1;
}

int get_key_name_table_entries(void)
{
	return ARRAY_SIZE_OF(key_name_table);
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_DEBUG

#ifdef START_UP_TEST
PRIVATE int check_all_functions_accessible_without_function_key_(func_key_table_t *key_table);
int check_all_functions_accessible_without_function_key()
{
flf_d_printf("-------------------------\n");
	int err = check_all_functions_accessible_without_function_key_(editor_func_key_table);
#ifdef ENABLE_FILER
	err += check_all_functions_accessible_without_function_key_(filer_func_key_table);
#endif // ENABLE_FILER
	return err;
}
PRIVATE int check_all_functions_accessible_without_function_key_(func_key_table_t *key_table)
{
	for (int func_idx = 0; key_table[func_idx].help[0]; func_idx++) {
		int accessible = 0;
		int accessible_without_fkey = 0;
		for (int key_idx = 0; key_idx < MAX_KEYS_BOUND; key_idx++) {
			key_code_t key;
			switch (key_idx) {
			case 0:
				key = key_table[func_idx].key1;	break;
			case 1:
				key = key_table[func_idx].key2;	break;
			case 2:
				key = key_table[func_idx].key3;	break;
			}
			if (IS_KEY_VALID(key)) {
				accessible++;
				if (IS_BYTE_KEY(key) || IS_META_KEY(key)) {
					accessible_without_fkey++;
				}
			}
		}
		if (accessible && (accessible_without_fkey == 0)) {
			warning_printf("func:[%s] is not accessible without Func-key\n",
			 key_table[func_idx].desc);
		}
	}
	return 0;
}

PRIVATE int check_multiple_assignment_of_key_(func_key_table_t *key_table);
int check_multiple_assignment_of_key()
{
flf_d_printf("-------------------------\n");
	int err = check_multiple_assignment_of_key_(editor_func_key_table);
#ifdef ENABLE_FILER
	err += check_multiple_assignment_of_key_(filer_func_key_table);
#endif // ENABLE_FILER
	return err;
}

PRIVATE int check_multiple_assignment_of_key_(func_key_table_t *key_table)
{
	for (int func_idx = 0; key_table[func_idx].help[0]; func_idx++) {
		for (int key_idx = 0; key_idx < MAX_KEYS_BOUND; key_idx++) {
			key_code_t key;
			switch (key_idx) {
			case 0:		key = key_table[func_idx].key1;	break;
			case 1:		key = key_table[func_idx].key2;	break;
			case 2:		key = key_table[func_idx].key3;	break;
			}
			if (key == KEY_NONE) {
				continue;
			}
			for (int func_idx2 = func_idx + 1; key_table[func_idx2].help[0] ; func_idx2++) {
				for (int key_idx2 = 0; key_idx2 < MAX_KEYS_BOUND; key_idx2++) {
					key_code_t key2;
					switch (key_idx2) {
					case 0:		key2 = key_table[func_idx2].key1;	break;
					case 1:		key2 = key_table[func_idx2].key2;	break;
					case 2:		key2 = key_table[func_idx2].key3;	break;
					}
					if (key2 == key) {
						warning_printf("key: %04x assigned multiple to func:[%s] and [%s]\n",
						 key, key_table[func_idx].desc, key_table[func_idx2].desc);
					}
				}
			}
		}
	}
	return 0;
}
#endif // START_UP_TEST

#endif // ENABLE_DEBUG

// End of keys.c
