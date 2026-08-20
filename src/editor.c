/**************************************************************************
 *   editor.c                                                             *
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

#include "headers.h"

do_next_t app_do_next = EF_NONE;

PRIVATE do_next_t editor_main_loop(char *str_buf);

do_next_t do_call_editor(int push_win, int list_mode, be_buf_t *buffer, char *str_buf)
{
#ifdef ENABLE_HISTORY
	save_histories_if_modified_newer();
#endif // ENABLE_HISTORY

	SET_APPMD_VAL(app_EDITOR_FILER, EF_EDITOR);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);

	if (buffer) {
		set_epx_buf(-1, buffer);	// set a buffer only to the current pane
	}

	flf_dprintf("GET_APPMD(app_EDITOR_FILER): %d\n", GET_APPMD(app_EDITOR_FILER));
	hmflf_dprintf("{{{{ CALL_EDITOR_MAIN_LOOP:%d push_win:%d, list_mode:%d\n",
	 get_sub_win_depth(), push_win, list_mode);

	editor_main_loop(str_buf);

	hmflf_dprintf("}}}} CALL_EDITOR_MAIN_LOOP:%d push_win:%d, list_mode:%d\n",
	 get_sub_win_depth(), push_win, list_mode);
	hmflf_dprintf(" --> app_stk: %d, do_next__[%s]\n",
	 get_app_stack_depth(), get_do_next_name(app_do_next));
	_mlc_check_count

	return app_do_next;		// EF_...
}

//------------------------------------------------------------------------------

PRIVATE char *output_buf_editor = NULL;
PRIVATE void set_output_buf_editor(char *output_buf)
{
	if (output_buf) {
		strcpy__(output_buf, "");
	}
	output_buf_editor = output_buf;
}
void set_text_to_output_buf_editor(char *text)
{
	if (output_buf_editor) {
		strlcpy__(output_buf_editor, text, MAX_PATH_LEN);
	}
}

PRIVATE do_next_t editor_main_loop(char *output_buf)
{
	set_output_buf_editor(output_buf);
#ifdef ENABLE_HISTORY
	char last_viewed_file_pos_str[MAX_PATH_LEN+1] = "";
#endif // ENABLE_HISTORY

	search_clear(&search__);
#ifdef ENABLE_REGEX
	matches_clear(&matches__);
#endif // ENABLE_REGEX

	key_code_t key_input = K_NONE;

	// Main input loop
	for ( ; ; ) {
		set_editor_app_mode_based_on_buf_mode();
		if (key_macro_is_playing_back()) {
			// During playing back key-macro, do not update screen for speed up.
		} else {
			update_screen_app(IS_KEY_INPUT(key_input) ? S_B_AUTO : S_B_NONE, 1);
		}
		//----------------------------------
		key_input = input_key_timeout();
		//----------------------------------
		SET_app_do_next(EF_NONE);
		if (IS_KEY_VALID(key_input)) {
			clear_status_bar_displayed();
			hmflf_dprintf("input%ckey:0x%04x([%s])========================================\n",
			 '_', (UINT16)key_input,
			 get_key_name_from_key_code(key_input, NULL));
			while (is_key_print(key_input)) {
				doe_put_utf8c_byte(key_input);	// put char
				key_input = input_key_with_key_macro();
			}
			doe_enter_buffered_utf8c_bytes();
			// The last key is in 'key_input'.
		}
		if (IS_KEY_VALID(key_input)) {
#ifdef ENABLE_REGEX
			matches_clear(&matches__);
#endif // ENABLE_REGEX
			func_key_t *func_key;
			if ((func_key = get_fkey_entry_from_key(editor_func_key_table, key_input, EF0M))
			 == NULL) {
				if (key_input != K_RESIZE) {
					disp_status_bar_warn(_("No command assigned for the key: 0x%04x([%s])"),
					 (UINT16)key_input,
					 get_key_name_from_key_code(key_input, NULL));
				}
			} else {
				if (is_fkey_entry_executable(func_key, EF0M) == 0) {
					disp_status_bar_err(
					 _("Can not execute this function in editor List mode: [%s]"),
					 func_key->func_id);
					SET_app_do_next(EF_CANCELLED);	// execution cancelled
				}
				if (app_do_next == EF_NONE) {
#ifdef ENABLE_HISTORY
					memorize_cur_file_pos_into_str(last_viewed_file_pos_str);
#endif // ENABLE_HISTORY
#if defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					memorize_undo_state_before_change(func_key->func_id);
#endif // defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					search_clear(&search__);
					hmflf_dprintf("{{ CALL_FUNC_EDITOR:%d [%s]\n",
					 get_sub_win_depth(), func_key->func_id);
					//=========================
					call_func_key_func(func_key);	// call function "doe_...()"
					//=========================
					hmflf_dprintf("}} CALL_FUNC_EDITOR:%d [%s]\n",
					 get_sub_win_depth(), func_key->func_id);
					hmflf_dprintf("app_do_next_[%s]\n", get_do_next_name(app_do_next));
					easy_buffer_switching_count();
#if defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					if (check_undo_state_after_change()) { progerr_printf("\n"); }
#endif // defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
				}
			}
		}
hmflf_dprintf("GET_APPMD(app_LIST_MODE): %d\n", GET_APPMD(app_LIST_MODE));
		// check the conditions for exiting editor
		if (is_app_normal_mode() && (has_bufs_to_edit() == 0)) {
#ifdef ENABLE_HISTORY
			if (is_internal_buf_file_path(last_viewed_file_pos_str) == 0) {
				modify_history_w_reloading(HISTORY_TYPE_IDX_FILE, last_viewed_file_pos_str);
			}
#endif // ENABLE_HISTORY
			// all files closed and no file to edit, exit editor.
			break;
		}
hmflf_dprintf("app_do_next: %s\n", get_do_next_name(app_do_next));
		switch (app_do_next) {
		case EF_LOADED_GO_TO_ROOT_EDITOR:
		case EF_EXECUTED_RET_TO_CALLER:
			// return to the root editor/caller
			if (get_app_stack_depth()) {
				break;						// exit from editor
			}
			SET_app_do_next(EF_NONE);		// not exit
			break;
#ifdef ENABLE_FILER
		case EF_GO_TO_LEVEL_FILER:
#ifdef ENABLE_DEBUG
dump_filer_view(get_fv_from_cur_pane());
#endif // ENABLE_DEBUG
			// always return to filer since it's editor
			// FALLTHROUGH
#endif // ENABLE_FILER
		default:
			break;							// exit from editor
		}
		if (app_do_next >= EF_TO_QUIT) {
			break;
		}
		sync_cut_buffers_and_histories(0);
	}
#ifdef ENABLE_HISTORY
	key_macro_cancel_recording();
#endif // ENABLE_HISTORY
	set_output_buf_editor(NULL);
	return app_do_next;
} // editor_main_loop

//------------------------------------------------------------------------------
int chk_inp_str_ret_val_editor(int ret)
{
	return ret <= EF_EXECUTED_RET_TO_CALLER;	// 1: nothing input
}

//------------------------------------------------------------------------------

#define _CLIPBOARD_FILE_NAME	"clipboard"		// default clipboard file name
#if defined(APP_DIR)
#define CLIPBOARD_FILE_NAME		_CLIPBOARD_FILE_NAME
#else // APP_DIR
#define CLIPBOARD_FILE_NAME		"." _CLIPBOARD_FILE_NAME
#endif // APP_DIR

// clipboard file is common to all be-editor instances in one user
const char *get_clipboard_file_path()
{
	static char file_path[MAX_PATH_LEN+1];
	return concat_dir_and_file(file_path, get_app_dir(), CLIPBOARD_FILE_NAME);
}
int save_newest_cut_buf_to_clipboard_file()
{
	return save_buf_to_file(CUT_BUFS_NEWEST_BUF, get_clipboard_file_path());
}

//------------------------------------------------------------------------------
PRIVATE int _doe_run_line(int flags, int clbi, int input);
void doe_run_line_immediate()
{
	_doe_run_line(EX_FLAGS_0, 0, 0);
}
void doe_run_line_input()
{
	_doe_run_line(EX_FLAGS_0, EPCBVC_CLBI, 1);
}
PRIVATE int _doe_run_line(int flags, int clbi, int input)
{
	char buffer[MAX_PATH_LEN+1];
	// NOTE: EPCBVC_CL->data may be in history buffer
	//       and possibly be freed in the call of modify_history_w_reloading().
	//       So copy here to local buffer.
	strlcpy__(buffer, &(EPCBVC_CL->data[clbi]), MAX_PATH_LEN);
	if (input) {
		if (chk_inp_str_ret_val_editor(input_string_pos(buffer, buffer, MAX_PATH_LEN,
		 HISTORY_TYPE_IDX_EXEC,
		 _("Edit command line:")))) {
			return 0;
		}
	}

	clear_fork_exec_counter();
	fork_exec_sh_c(EX_FLAGS_0 | EX_SETTERM | EX_SEPARATE | EX_PAUSE | flags, buffer);

	SET_app_do_next(EF_EXECUTED_RET_TO_CALLER);
	doe_refresh_editor();
	return 0;
}

//------------------------------------------------------------------------------
#ifdef ENABLE_HELP
void doe_view_file_list()
{
	view_list(HELP_BUF_IDX_EDITOR_FILE_LIST);
}
void doe_view_func_list()
{
	view_list(HELP_BUF_IDX_EDITOR_FUNC_LIST);
}
void doe_splash()
{
	do_splash();

	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
}

PRIVATE void examine_key_code_show(key_code_t key);
void do_splash()
{
	key_code_t key = K_NONE;
	for ( ; ; ) {
		disp_splash(100);
		examine_key_code_show(key);
		if (IS_KEY_INPUT(key = input_unmapped_key_loop()))
			break;
	}
	if (key == K_ESC) {	return;	}
	display_color_settings(key);
}

void display_color_settings(key_code_t key)
{
	for ( ; ; ) {
		display_color_pairs(0, 0);
		examine_key_code_show(key);
		if (IS_KEY_INPUT(key = input_unmapped_key_loop()))
			break;
	}
	if (key == K_ESC) {	return;	}
#ifdef ENABLE_DEBUG
	for ( ; ; ) {
		display_item_colors(0, 0);
		examine_key_code_show(key);
		if (IS_KEY_INPUT(key = input_unmapped_key_loop()))
			break;
	}
	if (key == K_ESC) {	return;	}
#ifdef ENABLE_REGEX
	for ( ; ; ) {
		display_bracket_hl_colors(0, 0);
		examine_key_code_show(key);
		key = input_unmapped_key_loop();
		if ((key == K_ESC) || (key == K_M_q) || (key == K_MC_q))
			break;
	}
flf_dprintf("key:%04x, K_MC_q:%04x\n", (UINT16)key, K_MC_q);
#endif // ENABLE_REGEX
#endif // ENABLE_DEBUG
#ifdef ON_DEMAND_WCWIDTH
flf_dprintf("key:%04x, K_MC_q:%04x\n", (UINT16)key, K_MC_q);
	if (key == K_MC_q) {
		investigate_all_wcwidth();
	}
#endif // ON_DEMAND_WCWIDTH
	return;
}

PRIVATE void examine_key_code_show(key_code_t key)
{
	if (IS_KEY_INVALID(key)) {
		disp_status_bar_warn(_("Input key to show key code"));
	} else {
		key_code_t key_mapped = map_key_code(key);
		char buf[MAX_KEY_NAME_LEN+1];
		disp_status_bar_done(_("Key code input: %04x:[%s], mapped: %04x:[%s]"),
		 (UINT16)key,
		 get_key_name_from_key_code(key, NULL),
		 (UINT16)key_mapped,
		 get_key_name_from_key_code(key_mapped, buf));
	}
	tio_refresh();
}
#endif // ENABLE_HELP

//------------------------------------------------------------------------------
void doe_quit()
{
	SET_app_do_next(EF_TO_QUIT);
}

void doe_menu_0()
{
	editor_menu_n(-1);
}

void doe_inc_key_list_lines()
{
	_dof_inc_key_list_lines();
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
//------------------------------------------------------------------------------
#define MAX_APP_STACK_DEPTH		(1+3)				// 1 root + 3 sub
app_stack_entry app_stack[MAX_APP_STACK_DEPTH+1];	// 1 root + 3 sub + 1 current-state

int cur_app_stack_depth = 0;
void clear_app_stack_depth()
{
	cur_app_stack_depth = 0;
	clear_app_stack_entry(cur_app_stack_depth);
}
int get_app_stack_depth()
{
	return cur_app_stack_depth;
}
int inc_app_stack_depth()
{
	if (cur_app_stack_depth < (MAX_APP_STACK_DEPTH - 1)) {
		cur_app_stack_depth++;
	}
	return cur_app_stack_depth;
}
int dec_app_stack_depth()
{
	if (cur_app_stack_depth > 0) {
		cur_app_stack_depth--;
	}
	return cur_app_stack_depth;
}
app_stack_entry *get_app_stack_ptr(int depth)
{
	if (depth < 0) {
		depth = cur_app_stack_depth;
	}
	return &(app_stack[depth]);
}
void clear_app_stack_entry(int depth)
{
	app_stack_entry *app_stk_ptr = get_app_stack_ptr(depth);
	memset(app_stk_ptr, 0x00, sizeof(*app_stk_ptr));
	app_stk_ptr->status_bar_color_idx = ITEM_COLOR_IDX_STATUS;
}

#ifdef ENABLE_FILER
void push_app_stack(editor_panes_t *next_eps, be_buf_t *buf, filer_panes_t *next_fps,
 editor_panes_t **prev_editor_panes, filer_panes_t **prev_filer_panes, app_mode_t *prev_app_mode)
#else // ENABLE_FILER
void push_app_stack(editor_panes_t *next_eps, be_buf_t *buf,
 editor_panes_t **prev_editor_panes, app_mode_t *prev_app_mode)
#endif // ENABLE_FILER
{
	*prev_editor_panes = get_cur_editor_panes();
	*prev_app_mode = app_mode__;

#ifdef ENABLE_FILER
	*prev_filer_panes = get_cur_filer_panes();
#endif // ENABLE_FILER
	inherit_editor_panes(next_eps, buf);
#ifdef ENABLE_FILER
	inherit_filer_panes(next_fps);
#endif // ENABLE_FILER

	set_win_depth(inc_app_stack_depth() * 2);
	// clear previous message displayed on the status bar
	clear_app_stack_entry(-1);
}
#ifdef ENABLE_FILER
void pop_app_stack(editor_panes_t* prev_editor_panes, filer_panes_t* prev_filer_panes,
 app_mode_t prev_app_mode, BOOL change_parent_editor, BOOL change_parent_filer)
#else // ENABLE_FILER
void pop_app_stack(editor_panes_t* prev_editor_panes,
 app_mode_t prev_app_mode, BOOL change_parent_editor)
#endif // ENABLE_FILER
{
	set_win_depth(dec_app_stack_depth() * 2);

	app_mode__ = prev_app_mode;
	if (change_parent_editor) {
		// change caller's current file
		copy_editor_panes(prev_editor_panes, get_cur_editor_panes());
	}
	free_editor_panes();
	set_cur_editor_panes(prev_editor_panes);
#ifdef ENABLE_FILER
	if (change_parent_filer) {
		// change caller's current directory
		copy_filer_panes_cur_dir(prev_filer_panes, get_cur_filer_panes());
	}
	free_filer_panes();
	set_cur_filer_panes(prev_filer_panes);
#endif // ENABLE_FILER
}

PRIVATE void update_screen_app__(s_b_d_t status_bar, int refresh);
void update_screen_app(s_b_d_t status_bar, int refresh)
{
	int app_stk_depth = 0;
	if (cur_app_stack_depth > 0) {
		app_stk_depth = cur_app_stack_depth - 1;
		set_win_depth(app_stk_depth * 2 + 1);
		set_item_color_by_idx(ITEM_COLOR_IDX_PARENT, 0);
		central_win_clear_screen();		// draw dark frame
	}
	app_stk_depth = cur_app_stack_depth;
	set_win_depth(app_stk_depth * 2);
	update_screen_app__(status_bar, refresh);
}
PRIVATE void update_screen_app__(s_b_d_t status_bar, int refresh)
{
#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == EF_EDITOR) {
#endif // ENABLE_FILER
		set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
		update_screen_editor(status_bar, refresh);
#ifdef ENABLE_FILER
	} else {
		update_screen_filer(status_bar, refresh);
	}
#endif // ENABLE_FILER
}

//------------------------------------------------------------------------------

PRIVATE void disp_status_bar_editor(s_b_d_t status_bar);
PRIVATE void disp_key_list_editor();

void update_screen_editor(s_b_d_t status_bar, int refresh)
{
	win_select_cur_sub_win(WIN_IDX_CENTRAL);

	// title bar
	disp_title_bar_editor();

	// status bar
	if (status_bar == S_B_NONE) {
		redisp_status_bar();				// display the previous message
	} else {
		disp_status_bar_editor(status_bar);
	}
	// key list
	disp_key_list_editor();

	if (get_edit_win_update_needed()) {
		if (GET_APPMD(ed_EDITOR_PANES) == 0) {		// 1 pane
			win_select_cur_sub_win(WIN_IDX_SUB_WHOLE);
			disp_edit_win(1);
		} else {									// 2 panes
			for (int pane_sel_idx = 0; pane_sel_idx < EDITOR_PANES; pane_sel_idx++) {
				// pane_sel_idx=0: update not current pane
				// pane_sel_idx=1: update current pane
				int pane_idx = get_editor_another_pane_idx();
				set_editor_cur_pane_idx(pane_idx);
				win_select_cur_sub_win(WIN_IDX_SUB_LEFT + pane_idx);
				disp_edit_win(pane_sel_idx);
			}
		}
	}

	set_edit_cursor_pos();
	if (refresh) {
		tio_refresh();
	}
	clear_edit_win_update_needed();
	win_select_cur_sub_win(WIN_IDX_CENTRAL);
}

//------------------------------------------------------------------------------

PRIVATE char blink_counter = 0;
void start_title_bar_blinking()
{
	blink_counter = 1;
}
void stop_title_bar_blinking()
{
	blink_counter = 0;
}
PRIVATE int get_title_bar_inversion()
{
	// blink_counter:
	//   0 ==>  0 ==>  0 ...
	//  +1 ==> -1 ==> +1 ...
	blink_counter *= -1;
	// return value:
	//   0 ==>  1 ==>  0 ...
	return blink_counter < 0;
}

#ifdef ENABLE_UNDO
#define SHOW_UNDO_BUFS
#endif // ENABLE_UNDO
#define SHOW_MEM_FREE_ON_EBUFS_CHG	// show memory free on edit buffers change
											//  0123456789012345
#define BUF_BUF_LEN				(4 + 12)	// " Ml e99c99u99r99"
#define HHCMMCSS_BUF_LEN		8			// "23:59:59" / "24/10/09"
#define MEM_BUF_LEN				8			// "9999000M" (9999G)
#define BUF_STATUS_LEN			((BUF_BUF_LEN) + 1 + (HHCMMCSS_BUF_LEN))
//1:/home/...editor2.c[MOD]    Mc e99c0u0r0 1234M 11:55:04
void disp_title_bar_editor()
{
	static int prev_edit_bufs = 0;

	int bufs_idx = bufs_get_bufs_idx_in_bufss(NODES_TOP_ANCH(&all_bufferss), get_epc_buf());
	const char *bufs_name = get_bufs_name_contains_buf(get_epc_buf());
	int buf_idx = buf_get_buf_idx(get_epc_buf());
	const char *path = buf_get_file_path(get_epc_buf(), NULL);

	tio_set_cursor_on(0);

	//-------------------------------------------------------------------------
	char separator_char = indication_of_app_mode();
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%s%d%c%d%s%c%d/%d:%s",
	 root_notation(), get_editor_cur_pane_idx()+1, separator_char, bufs_idx, bufs_name,
	 separator_char, buf_idx, epc_buf_count_bufs(), (path[0] == '\0') ? _("New File") : path);
	strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, get_all_buf_state_str());

	//-------------------------------------------------------------------------
	// edit buffer cut mode
	char buf_bufs[BUF_BUF_LEN+1] = "";
	if (GET_BUF_STATE(get_epc_buf(), buf_CUT_MODE) != CUT_MODE_0_NONE) {
		strcat_printf(buf_bufs, BUF_BUF_LEN, " %s", buf_cut_mode_str(get_epc_buf()));
	}
	// edit buffers
	int edit_bufs = count_edit_bufs();
	strcat_printf(buf_bufs, BUF_BUF_LEN, " e%s", zz_from_num(edit_bufs, NULL));
	// cut buffers
	int cut_bufs = count_cut_bufs();
	strcat_printf(buf_bufs, BUF_BUF_LEN, "c%s", zz_from_num(cut_bufs, NULL));
#ifdef SHOW_UNDO_BUFS
	// undo buffers
	int undo_bufs = count_undo_bufs() / 2;
	strcat_printf(buf_bufs, BUF_BUF_LEN, "u%s", zz_from_num(undo_bufs, NULL));
	// redo buffers
	int redo_bufs = count_redo_bufs() / 2;
	strcat_printf(buf_bufs, BUF_BUF_LEN, "r%s", zz_from_num(redo_bufs, NULL));
#endif // SHOW_UNDO_BUFS

	// current date / time
	char buf_time[HHCMMCSS_BUF_LEN+1];
	snprintf_(buf_time, HHCMMCSS_BUF_LEN+1, "%s",
	 cur_ctime_cdate(get_msec_past_after_key_input() < 1000));

	//-------------------------------------------------------------------------
	char buf_status[BUF_STATUS_LEN+1];
#ifdef SHOW_MEM_FREE_ON_EBUFS_CHG
	if (edit_bufs != prev_edit_bufs) {
		prev_edit_bufs = edit_bufs;
		// free memory in MB
		char buf_mem[MEM_BUF_LEN+1];
		snprintf_(buf_mem, MEM_BUF_LEN+1, "%7dM", get_mem_free_in_kb(1) / 1000);
		snprintf_(buf_status, BUF_STATUS_LEN+1, "%s %s", buf_bufs, buf_mem);
	} else {
		snprintf_(buf_status, BUF_STATUS_LEN+1, "%s %s", buf_bufs, buf_time);
	}
#else // SHOW_MEM_FREE_ON_EBUFS_CHG
	snprintf_(buf_status, BUF_STATUS_LEN+1, "%s %s", buf_bufs, buf_time);
#endif // SHOW_MEM_FREE_ON_EBUFS_CHG

	int path_cols = LIM_MIN(0, central_win_get_columns() - strlen_path(buf_status));
	shrink_str__adjust_col(buf_path, path_cols, 4);
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_path, buf_status);

	set_title_bar_color_by_state(
	 (is_epc_buf_modifiable() == 0) ? ITEM_COLOR_IDX_ERROR
	  : (is_epc_buf_modified() ? ITEM_COLOR_IDX_WARNING1
	   : (is_any_ebuf_modified() ? ITEM_COLOR_IDX_WARNING2
	    : (GET_CUR_EBUF_STATE(buf_CUT_MODE) ? ITEM_COLOR_IDX_TEXT_SELECTED1
	     : ITEM_COLOR_IDX_TITLE))),
	 get_title_bar_inversion());
	central_win_output_string(central_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);

	tio_set_cursor_on(1);
}
//------------------------------------------------------------------------------
PRIVATE void disp_status_bar_editor(s_b_d_t status_bar)
{
	int bytes, byte_idx;
#define UTF32_CODE_LEN		(1+2+8+1)				// "(U+xxxx)" ~ "(U+xxxxxxxx)"
#define UTF8_CODE_LEN		(17+(UTF32_CODE_LEN))	// "00-00-00-00-00-00(U+xxxxxxxx)"
	char buf_char_code[UTF8_CODE_LEN+1] = "";		// "00-00-00-00-00-00(U+xxxxxxxx)"
	unsigned long xx;
	unsigned long disp_len;
#define SEL_LINES_COLS_LEN		(1+1+4+10+1+5+4+1)		// " (LNS:9999999999 COLS:9999)"
	char buf_lines_sel[SEL_LINES_COLS_LEN+1] = "";
#define ENC_EOL_LEN		(3+2+6+1+5+5)					// "ENC: EUC-JP EOL: CR+LF"
	char buf_enc_eol[ENC_EOL_LEN+1] = "";
	char buf_line_col_size_char[MAX_EDIT_LINE_LEN+1] = "";
	char buffer[MAX_EDIT_LINE_LEN+1] = "";

	xx = col_idx_from_byte_idx(EPCBVC_CL->data, EPCBVC_CLBI) + 1;
	disp_len = col_idx_from_byte_idx(EPCBVC_CL->data, MAX_EDIT_LINE_LEN) + 1;

	bytes = utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
	for (byte_idx = 0; byte_idx < bytes; byte_idx++) {
		strcat_printf(buf_char_code, UTF8_CODE_LEN, byte_idx == 0 ? "%02x" : "-%02x",
		 (UCHAR)EPCBVC_CL->data[EPCBVC_CLBI + byte_idx]);
	}
	// show Unicode
	if (bytes >= 2) {
		strcat_printf(buf_char_code, UTF8_CODE_LEN,
		 "(U+%04x)", (unsigned int)utf8c_decode(&EPCBVC_CL->data[EPCBVC_CLBI]));
	}
	strcat_printf(buf_char_code, UTF8_CODE_LEN,
	 "(%d)", utf8c_columns(&EPCBVC_CL->data[EPCBVC_CLBI]));

	if (IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE))) {
		snprintf(buf_lines_sel, SEL_LINES_COLS_LEN+1, " (LNS:%2d COLS:%2d)",
		 get_lines_selected_in_cut_region(), get_columns_selected_in_cut_region());
	}

	snprintf(buf_line_col_size_char, MAX_EDIT_LINE_LEN,
	 _("LINE:%4u/%-4u COLUMN:%3lu/%-3lu SIZE:%6lu%s CHR:%s"),
	 EPCBVC_CL->line_num, get_epc_buf()->buf_lines, xx, disp_len,
	 get_epc_buf()->buf_size, buf_lines_sel, buf_char_code);

	snprintf(buf_enc_eol, ENC_EOL_LEN+1, _("ENC:%s EOL:%s"),
	 buf_enc_str(get_epc_buf()), buf_eol_str(get_epc_buf()));

	int cols_line_col_size_char = col_idx_from_byte_idx(buf_line_col_size_char, INT_MAX);
	int cols_enc_eol = col_idx_from_byte_idx(buf_enc_eol, INT_MAX);

	int spaces = LIM_MIN(1, sub_win_get_columns() - (cols_line_col_size_char + cols_enc_eol));
	snprintf(buffer, MAX_EDIT_LINE_LEN, "%s%*s%s",
	 buf_line_col_size_char, spaces, "", buf_enc_eol);
	adjust_str_columns(buffer, sub_win_get_columns());

	disp_status_bar_type(status_bar, buffer);
}

PRIVATE void disp_key_list_editor()
{
	disp_fkey_list();

	const char *editor_keys_in_normal_mode[] = {
	 "<doe_close_file_ask>Quit "
	 "<doe_first_line>TopOfFile "
	 "<doe_last_line>BotOfFile "
	 "<doe_prev_word>PrevWord "
	 "<doe_next_word>NextWord "
	 "<doe_cut_to_head>CutToHead "
	 "<doe_cut_to_tail>CutToTail ",
	 "<doe_tog_mark>Mark "
	 "<doe_cut_text>CutLine "
	 "<doe_copy_text>CopyLine "
	 "<doe_paste_text_with_pop>PasteWPop "
	 "<doe_paste_text_without_pop>PasteWoPop "
	 "<doe_duplicate_text>DupLine "
	 "<doe_close_all_ask>CloseAll "
	 "<doe_open_file_recursive>OpenFile "
	 "<doe_write_file_ask>WriteFile "
	 "<doe_search_first_backward>Search BW "
	 "<doe_search_first_forward>Search FW "
	 "<doe_replace>Replace "
	 "<doe_view_file_list>FileList "
#ifdef ENABLE_HELP
	 "<doe_view_func_list>KeyList "
#endif // ENABLE_HELP
	 "<doe_switch_to_prev_buffer>PrevBuf "
	 "<doe_switch_to_next_buffer>NextBuf "
	};
	const char *editor_keys_in_list_mode[] = {
	 "<doe_enter_text>Enter text "
	 "<doe_enter_text_add>Enter text (add) ",
	 ""
	};
	disp_key_list_lines(is_app_viewer_mode() == 0
	 ? editor_keys_in_normal_mode : editor_keys_in_list_mode);
}
//------------------------------------------------------------------------------
int is_editor_unmodifiable_then_warn_it()
{
	// in Application mode
	if (is_app_viewer_mode()) {
		disp_status_bar_err(_("Modification not allowed in VIEW mode application"));
		return 1;
	}
	// in Buffer state
	if (is_epc_buf_modifiable() == 0) {
		disp_status_bar_err(_("Modification not allowed to %s file"),
		 get_all_buf_unmodifiable_str());
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_cur_pointers()
{
	flf_dprintf("epc_buf:[%s]\n", buf_get_file_path(get_epc_buf(), NULL));
	flf_dprintf("%d:[%s]\n", EPCBVC_CL->line_num, EPCBVC_CL->data);
	flf_dprintf("EPCBVC_CLBI:%d\n", EPCBVC_CLBI);
	flf_dprintf("cursor_y:%d\n", EPCBVC_CURS_Y);
	flf_dprintf("cursor_x_to_keep:%d\n", EPCBVC_CURS_X_TO_KEEP);
	flf_dprintf("min_text_x_to_keep:%d\n", EPCBVC_MIN_TEXT_X_TO_KEEP);
}
#endif // ENABLE_DEBUG

// End of editor.c
