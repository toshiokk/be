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

ef_do_next_t editor_do_next = EF_NONE;
PRIVATE int editor_main_loop(char *str_buf, int buf_len);

int call_editor(int push_win, int list_mode, char *str_buf, int buf_len)
{
#ifdef ENABLE_HISTORY
	save_histories();
#endif // ENABLE_HISTORY

	editor_panes_t *prev_eps = NULL;
	editor_panes_t next_editor_panes;
	app_mode_t appmode_save;

	if (push_win) {
		win_push_win_size();
		prev_eps = push_editor_panes(&next_editor_panes);
	}

	memcpy(&appmode_save, &app_mode__, sizeof(app_mode__));
	CLR_APPMD(app_EDITOR_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);
	SET_APPMD_VAL(ed_EDITOR_PANES, 0);
	set_work_space_color_on_app_list_mode();

flf_d_printf("push_win:%d, list_mode:%d\n", push_win, list_mode);
flf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");

	int ret = editor_main_loop(str_buf, buf_len);

flf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
flf_d_printf("push_win:%d, list_mode:%d --> ret: %d\n", push_win, list_mode, ret);
	_mlc_check_count

	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));
	SET_APPMD_VAL(app_LIST_MODE, GET_APPMD_PTR(&appmode_save, app_LIST_MODE));
	SET_APPMD_VAL(ed_EDITOR_PANES, GET_APPMD_PTR(&appmode_save, ed_EDITOR_PANES));
	set_work_space_color_on_app_list_mode();

	if (push_win) {
		pop_editor_panes(prev_eps, &next_editor_panes, ret == EF_LOADED);
		win_pop_win_size();
	}

	return ret;		// EF_...
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_HISTORY
char last_touched_file_pos_str[MAX_PATH_LEN+1];
#endif // ENABLE_HISTORY

PRIVATE int editor_main_loop(char *str_buf, int buf_len)
{
	if (str_buf) {
		strcpy__(str_buf, "");
	}
	key_code_t key_input;
	func_key_table_t *func_key_table;

	search_clear(&search__);
#ifdef ENABLE_REGEX
	matches_clear(&matches__);
#endif // ENABLE_REGEX

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);

	// Main input loop
	key_input = K_C_AT;
	for ( ; ; ) {
		editor_do_next = EF_NONE;
		if (key_macro_is_playing_back()) {
			// During playing back key-macro, do not update screen for speed up.
		} else {
			set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
			update_screen_editor(1, IS_KEY_VALID(key_input), 1);
		}
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		if (IS_CHAR_KEY(key_input)) {
			doe_buffer_utf8c_bytes(key_input);	// put the first char
			for ( ; ; ) {
				key_input = input_key_macro();
				if (IS_CHAR_KEY(key_input) == 0)
					break;
				doe_buffer_utf8c_bytes(key_input);	// put trailing chars
			}
			doe_enter_buffered_utf8c_bytes();
		}
		if (IS_KEY_VALID(key_input)) {
mflf_d_printf("input%ckey:0x%04x(%s)=======================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));

#ifdef ENABLE_REGEX
			matches_clear(&matches__);
#endif // ENABLE_REGEX
			if ((func_key_table = get_func_key_table_from_key(editor_func_key_table, key_input))
			 == NULL) {
				disp_status_bar_err(_("No command assigned for the key: %04xh"), key_input);
			} else {
				if (is_app_list_help_mode()) {
					switch (func_key_table->list_mode) {
					case XL:	// not executable in editor List mode
						disp_status_bar_done(
						 _("Can not execute this function in editor List mode: [%s]"),
						 func_key_table->func_id);
						editor_do_next = EF_QUIT;
						break;
					case XI:	// not executable in editor List mode, get a text
						editor_do_next = EF_INPUT;
						break;
					default:
						break;
					}
				}
				if (editor_do_next == EF_NONE) {
#ifdef ENABLE_HISTORY
					memorize_cur_file_pos_null(last_touched_file_pos_str);
#endif // ENABLE_HISTORY
#if defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					memorize_undo_state_before_change(func_key_table->func_id);
#endif // defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					search_clear(&search__);
mflf_d_printf("<<<< CALL_EDITOR_FUNC [%s]\n", func_key_table->func_id);
					//=========================
					(*func_key_table->func)();	// call function "doe__...()"
					//=========================
mflf_d_printf(">>>> editor_do_next: EF__%d\n", editor_do_next);
					easy_buffer_switching_count();
#if defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					check_undo_state_after_change();
#endif // defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
				}
			}
		}
#ifdef ENABLE_HISTORY
		save_histories();
#endif // ENABLE_HISTORY

		if (is_app_list_help_mode() == 0) {
			if (epc_buf_count_bufs() == 0) {
flf_d_printf("all files closed\n");
#ifdef ENABLE_HISTORY
				update_history(HISTORY_TYPE_IDX_FILEPOS, last_touched_file_pos_str);
#endif // ENABLE_HISTORY
				// If all files closed on editor, exit editor.
				break;
			}
		} else /* if (is_app_list_help_mode()) */ {
			if (editor_do_next) {
				break;
			}
		}
	}
#ifdef ENABLE_HISTORY
	key_macro_cancel_recording();
#endif // ENABLE_HISTORY
	if (editor_do_next == EF_INPUT) {
		if (str_buf && epc_buf_count_bufs()) {
			// get a text from editor current line
			strlcpy__(str_buf, EPCBVC_CL->data, buf_len);
		}
		editor_do_next = (IS_META_KEY(key_input) == 0)
		 ? EF_INPUT_TO_REPLACE		// Replace input file/dir name
		 : EF_INPUT_TO_APPEND;		// Append input file/dir name
	}
	return editor_do_next;
}

//-----------------------------------------------------------------------------
int chk_inp_str_ret_val_editor(int ret)
{
	return ret <= EF_EXECUTED;
}

//-----------------------------------------------------------------------------
char *get_app_dir(void)
{
	static char dir[MAX_PATH_LEN+1];

#if defined(APP_DIR)
	snprintf_(dir, MAX_PATH_LEN+1, "%s/%s", get_home_dir(), APP_DIR);
#else
	snprintf_(dir, MAX_PATH_LEN+1, "%s", get_home_dir());
#endif
	return dir;
}

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

	snprintf_(file_path, MAX_PATH_LEN+1, "%s/%s", get_app_dir(), CLIPBOARD_FILE_NAME);
	return file_path;
}
int save_cut_buf_to_clipboard_file()
{
	return save_buf_to_file(CUT_BUFS_TOP_BUF, get_clipboard_file_path());
}

int load_clipboard_into_cut_buf()
{
	return load_file_into_buf(CUT_BUFS_TOP_BUF, get_clipboard_file_path());
}

int doe_read_clipboard_into_cur_char()
{
	return doe_read_clipboard_into_cur_pos_(0);
}
int doe_read_clipboard_into_cur_line()
{
	return doe_read_clipboard_into_cur_pos_(1);
}
int doe_read_clipboard_into_cur_pos_(int char0_line1)
{
	push_cut_buf();
	if (load_clipboard_into_cut_buf() < 0) {
		pop__free_from_cut_buf();
		return 0;
	}
	if (char0_line1 == 0) {
		// set character cut ==> character paste
		SET_CUR_CBUF_STATE(buf_CUT_MODE, CUT_MODE_H_CHAR);
	} else {
		// set line cut ==> line paste
		SET_CUR_CBUF_STATE(buf_CUT_MODE, CUT_MODE_N_LINE);
	}
	doe_paste_text_with_pop();
	return 0;
}

//-----------------------------------------------------------------------------
int doe_run_line_soon(void)
{
	return doe_run_line_soon_(LOGGING0);
}
int doe_run_line_soon_w_log(void)
{
	return doe_run_line_soon_(LOGGING2);
}
int doe_run_line_soon_(int logging)
{
	char buffer[MAX_PATH_LEN+1];

	// NOTE: EPCBVC_CL->data may be in history buffer and freed in calling update_history().
	// So copy to local buffer.
	strlcpy__(buffer, EPCBVC_CL->data, MAX_PATH_LEN);

	clear_fork_exec_counter();
	fork_exec_sh_c(SETTERM1, SEPARATE1, logging, PAUSE1, buffer);

	editor_do_next = EF_EXECUTED;
	doe_refresh_editor();
	return 0;
}

#ifdef ENABLE_FILER
int doe_call_filer(void)
{
	char file_path[MAX_PATH_LEN+1];
	call_filer(1, APP_MODE_NORMAL, "", "", file_path, MAX_PATH_LEN);
	return 0;
}
#endif // ENABLE_FILER

//-----------------------------------------------------------------------------
#ifdef ENABLE_HELP
int doe_splash(void)
{
	disp_splash(100);

	examine_key_code();

	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_view_key_list(void)
{
	view_help(HELP_BUF_IDX_EDITOR_KEY_LIST);
	return 1;
}
#endif // ENABLE_HELP

int doe_display_color_settings(void)
{
	display_color_settings();
	return 0;
}
void display_color_settings(void)
{
	tio_clear_flash_screen(1);
	display_color_pairs(0, 0);
	examine_key_code();

#ifdef ENABLE_DEBUG
	tio_clear_flash_screen(1);
	display_item_colors(0, 0);
	examine_key_code();
#ifdef ENABLE_REGEX
	tio_clear_flash_screen(1);
	display_bracket_hl_colors(0, 0);
	examine_key_code();
#endif // ENABLE_REGEX
#endif // ENABLE_DEBUG
}

int doe_editor_menu_0(void)
{
	return editor_menu_n(-1);
}
int doe_editor_menu_1(void)
{
	return editor_menu_n(0);
}
int doe_editor_menu_2(void)
{
	return editor_menu_n(1);
}
int doe_editor_menu_3(void)
{
	return editor_menu_n(2);
}
int doe_editor_menu_4(void)
{
	return editor_menu_n(3);
}
int doe_editor_menu_5(void)
{
	return editor_menu_n(4);
}
int doe_editor_menu_6(void)
{
	return editor_menu_n(5);
}
int doe_editor_menu_7(void)
{
	return editor_menu_n(6);
}
int doe_editor_menu_8(void)
{
	return editor_menu_n(7);
}

//-----------------------------------------------------------------------------

int doe_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
void win_push_win_size(void)
{
	// draw parent screen reversed
	update_screen_app(1, 1, 1);

	inc_win_depth();
	win_reinit_win_size();
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_screen();		// draw dark frame
	inc_win_depth();
	win_reinit_win_size();
}
void win_pop_win_size(void)
{
	dec_win_depth();
	dec_win_depth();
	win_reinit_win_size();

	// draw parent screen
	update_screen_app(1, 1, 1);
}

int update_screen_app(int title_bar, int status_bar, int refresh)
{
#ifndef ENABLE_FILER
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(title_bar, status_bar, refresh);
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
		update_screen_editor(title_bar, status_bar, refresh);
	} else {
		update_screen_filer(title_bar, status_bar, refresh);
	}
#endif // ENABLE_FILER
	return 0;
}

//-----------------------------------------------------------------------------
int update_screen_editor(int title_bar, int status_bar, int refresh)
{
	int pane_sel_idx;		// 0: not current pane, 1: current pane
	int cur_pane_idx;		// pane index
	int pane_idx;			// pane index

	EPCBVC_CURS_Y = MIN(edit_win_get_text_lines()-1, EPCBVC_CURS_Y);
////mflf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{\n");
	// title bar
	if (title_bar) {
		editor_disp_title_bar();
	}

	if (get_edit_win_update_needed()) {
		if (GET_APPMD(ed_EDITOR_PANES) == 0) {		// 1 pane
			win_select_win(WIN_IDX_SUB_WHOLE);
			disp_edit_win(1);
		} else {									// 2 panes
			cur_pane_idx = get_editor_cur_pane_idx();
			for (pane_sel_idx = 0; pane_sel_idx < EDITOR_PANES; pane_sel_idx++) {
				// 1st, update not current pane.
				// 2nd, update current pane.
				if (pane_sel_idx == 0) {
					// not current pane
					pane_idx = 1 - cur_pane_idx;	// 0 ==> 1, 1 ==> 0
				} else {
					// current pane
					pane_idx = cur_pane_idx;
				}
				win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
				if (pane_sel_idx == 0) {
					set_work_space_color_dark();
				}
////flf_d_printf("pane_sel_idx: %d, pane_idx: %d\n", pane_sel_idx, pane_idx);
				set_editor_cur_pane_idx(pane_idx);
				disp_edit_win(pane_sel_idx);
				if (pane_sel_idx == 0) {
					clear_work_space_color_dark();
				}
			}
		}
	}

	// status bar
	if (status_bar) {
		disp_status_bar_editor();
		// key list
		disp_key_list_editor();
	}

	set_edit_cursor_pos();

	if (refresh) {
		tio_refresh();
	}
	clear_edit_win_update_needed();
////mflf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}\n");
	return 0;
}

int disp_status_bar_editor(void)
{
	int bytes, byte_idx;
#define UTF8_CODE_LEN		(17+1+8+1)			// "00-00-00-00-00-00(U+xxxxxx)"
	char buf_char_code[UTF8_CODE_LEN+1] = "";	// "00-00-00-00-00-00(U+xxxxxx)"
	unsigned long xx;
	unsigned long disp_len;
#define SEL_LINES_LEN		(1+4+10+1)		// " LNS:9999999999"
	char buf_lines_sel[SEL_LINES_LEN] = "";
	char buffer[MAX_EDIT_LINE_LEN+1] = "";

	xx = col_idx_from_byte_idx(EPCBVC_CL->data, 0, EPCBVC_CLBI) + 1;
	disp_len = col_idx_from_byte_idx(EPCBVC_CL->data, 0, MAX_EDIT_LINE_LEN) + 1;

	bytes = utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
	for (byte_idx = 0; byte_idx < bytes; byte_idx++) {
		snprintf(&buf_char_code[strnlen(buf_char_code, UTF8_CODE_LEN)], 3+1,
		 byte_idx == 0 ? "%02x" : "-%02x",
		 (unsigned char)EPCBVC_CL->data[EPCBVC_CLBI + byte_idx]);
	}
	// show Unicode
	if (bytes >= 2) {
		snprintf(&buf_char_code[strnlen(buf_char_code, UTF8_CODE_LEN)], 8+1, "(U+%04x)",
		 (unsigned int)utf8c_decode(&EPCBVC_CL->data[EPCBVC_CLBI]));
	}

	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE))) {
		snprintf(buf_lines_sel, SEL_LINES_LEN, " LNS:%2d", lines_selected());
	}

	strlcat__(buffer, MAX_EDIT_LINE_LEN,
	 _("LINE:%4lu/%-4lu COLUMN:%3lu/%-3lu SIZE:%6lu%s CODE:%s ENC:%s EOL:%s"));
	disp_status_bar_percent_editor(
	 buffer, EPCBVC_CL->line_num, get_epc_buf()->buf_lines, xx, disp_len,
	 get_epc_buf()->buf_size, buf_lines_sel, buf_char_code,
	 buf_encode_str(get_epc_buf()), buf_eol_str(get_epc_buf()));
	return 1;
}

void disp_key_list_editor(void)
{
	char *editor_key_lists[] = {
 "{Menu} "
 " {Rec  } {Play } {SchBW} {SchFW} "
 " {Mark } {Cut  } {Copy } {Pop  } "
 " {Paste} {Dupli} {Files} {TagJp}",
 "<doe_close_file_ask>Quit "
 "<doe_cut_to_head>CutToHead "
 "<doe_cut_text>CutLine "
 "<doe_cut_to_tail>CutToTail "
 "<doe_copy_text>CopyLine "
 "<doe_paste_text_with_pop>PasteWPop "
 "<doe_paste_text_without_pop>PasteWoPop "
 "<doe_duplicate_text>DupLine "
 "<doe_first_line>TopOfFile "
 "<doe_last_line>BotOfFile "
 "<doe_prev_word>PrevWord "
 "<doe_next_word>NextWord ",

 "<doe_close_all_ask>CloseAll "
 "<doe_open_file>OpenFile "
 "<doe_write_file_ask>WriteFile "
 "<doe_search_backward_first>Search BW "
 "<doe_search_forward_first>Search FW "
 "<doe_replace>Replace "
 "<doe_view_file_list>FileList "
#ifdef ENABLE_HELP
 "<doe_view_key_list>KeyList "
#endif // ENABLE_HELP
 "<doe_switch_to_prev_file>PrevFile "
 "<doe_switch_to_next_file>NextFile "
#if APP_REL_LVL == APP_REL_LVL_EXPERIMENTAL
 "<doe_switch_to_prev_buffers>PrevBufs "
 "<doe_switch_to_next_buffers>NextBufs "
#endif // APP_REL_LVL
	};
	disp_key_list(editor_key_lists);
}

//-----------------------------------------------------------------------------

int is_editor_view_mode_then_warn_it(void)
{
	if (is_app_list_mode()) {
		disp_status_bar_done(_("Modification not allowed in LIST mode"));
		return 1;
	}
	if (IS_NODE_ANCH(get_epc_buf())) {
		disp_status_bar_done(_("Modification not allowed in Anchor buffer"));
		return 1;
	}
	if (CUR_EBUF_STATE(buf_VIEW_MODE)) {
		disp_status_bar_done(_("Modification not allowed in VIEW mode buffer"));
		return 1;
	}
	return 0;
}
int is_editor_view_mode(void)
{
	return is_app_list_mode() || is_epc_view_mode();
}
int is_epc_view_mode(void)
{
	return IS_NODE_ANCH(get_epc_buf()) || CUR_EBUF_STATE(buf_VIEW_MODE);
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_cur_pointers(void)
{
	flf_d_printf("epc_buf:[%s]\n", get_epc_buf()->file_path_);
	flf_d_printf("%d:[%s]\n", EPCBVC_CL->line_num, EPCBVC_CL->data);
	flf_d_printf("EPCBVC_CLBI:%d\n", EPCBVC_CLBI);
	flf_d_printf("cursor_y:%d\n", EPCBVC_CURS_Y);
	flf_d_printf("cursor_x_to_keep:%d\n", EPCBVC_CURS_X_TO_KEEP);
	flf_d_printf("min_text_x_to_keep:%d\n", EPCBVC_MIN_TEXT_X_TO_KEEP);
}
#endif // ENABLE_DEBUG

// End of editor.c
