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

int call_editor(int push_win, int list_mode, be_buf_t *buf, char *str_buf, int buf_len)
{
#ifdef ENABLE_HISTORY
	save_histories();
#endif // ENABLE_HISTORY

	editor_panes_t next_eps;

	if (push_win) {
#ifdef ENABLE_FILER
		push_app_win(&next_eps, buf, NULL);
#else // ENABLE_FILER
		push_app_win(&next_eps, buf);
#endif // ENABLE_FILER
	}

	set_editor_app_mode_on_cur_buf_mode();
	SET_APPMD_VAL(app_EDITOR_FILER, EF_EDITOR);

flf_d_printf("GET_APPMD(app_EDITOR_FILER): %d\n", GET_APPMD(app_EDITOR_FILER));
flf_d_printf("push_win:%d\n", push_win);
flf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{\n");

	int ret = editor_main_loop(str_buf, buf_len);

flf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}\n");
flf_d_printf("push_win:%d --> ret: %d\n", push_win, ret);
	_mlc_check_count

	if (push_win) {
		// return setting newly loaded buffer as a current buffer
		pop_app_win(ret == EF_LOADED);
		update_screen_app(1, 1);
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
	search_clear(&search__);
#ifdef ENABLE_REGEX
	matches_clear(&matches__);
#endif // ENABLE_REGEX

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);

	key_code_t key_input = K_VALID;		// show status bar at the first loop

	// Main input loop
	for ( ; ; ) {
///		mflf_d_printf("key_input: (%s):%d\n",
///		 short_key_name_from_key_code(key_input, NULL), IS_KEY_VALID(key_input));
		editor_do_next = EF_NONE;
		if (key_macro_is_playing_back()) {
			// During playing back key-macro, do not update screen for speed up.
		} else {
			set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
			update_screen_app(IS_KEY_VALID(key_input), 1);
		}
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		if (IS_KEY_VALID(key_input)) {
			mflf_d_printf("input%ckey:0x%04x(%s|%s)=======================\n",
			 '_', key_input, short_key_name_from_key_code(key_input, NULL),
			 key_name_from_key_code(key_input, NULL));
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
			// The last key is still there in 'key_input'.
		}
		if (IS_KEY_VALID(key_input)) {
#ifdef ENABLE_REGEX
			matches_clear(&matches__);
#endif // ENABLE_REGEX
			func_key_table_t *fkey_table;
			if ((fkey_table = get_func_key_table_from_key(editor_func_key_table, key_input))
			 == NULL) {
				disp_status_bar_err(_("No command assigned for the key: %04xh"), key_input);
			} else {
				if (is_app_view_list_mode()) {
					switch (fkey_table->list_mode) {
					case EFLM_NO_EXEC:	// not executable in editor List mode
						disp_status_bar_done(
						 _("Can not execute this function in editor List mode: [%s]"),
						 fkey_table->func_id);
						editor_do_next = EF_QUIT;
						break;
					case E_LM_CUR_LIN:	// not executable in editor List mode, get a text
						editor_do_next = EF_INPUT;
						break;
					case EFAM_EXECUTE:
					default:
						break;
					}
				}
				if (editor_do_next == EF_NONE) {
#ifdef ENABLE_HISTORY
					memorize_cur_file_pos_null(last_touched_file_pos_str);
#endif // ENABLE_HISTORY
#if defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					memorize_undo_state_before_change(fkey_table->func_id);
#endif // defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					search_clear(&search__);
					mflf_d_printf("{{{{ CALL_FUNC_EDITOR [%s]\n", fkey_table->func_id);
					disp_status_bar_ing(_(fkey_table->desc));	// show what about to do
					//=========================
					(*fkey_table->func)();	// call function "doe__...()"
					//=========================
					mflf_d_printf("}}}} editor_do_next: EF__%d\n", editor_do_next);
					easy_buffer_switching_count();
#if defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
					check_undo_state_after_change();
#endif // defined(ENABLE_UNDO) && defined(ENABLE_DEBUG)
				}
			}
#ifdef ENABLE_HISTORY
			save_histories();
#endif // ENABLE_HISTORY
		}
flf_d_printf("app_mode: %d, edit-buffers: %d\n", GET_APPMD(app_LIST_MODE), edit_bufs_count_bufs());
		if (is_app_normal_mode()) {
			if (edit_bufs_count_bufs() == 0) {
flf_d_printf("all buffers closed\n");
#ifdef ENABLE_HISTORY
				update_history(HISTORY_TYPE_IDX_FILE, last_touched_file_pos_str);
#endif // ENABLE_HISTORY
				// If all files closed on editor, exit editor.
				break;
			}
		} else /* if (is_app_view_list_mode()) */ {
			if (editor_do_next) {
flf_d_printf("all buffers closed\n");
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
	return save_buf_to_file(CUT_BUFS_TOP_NODE, get_clipboard_file_path());
}

int load_clipboard_into_cut_buf()
{
	return load_file_into_buf(CUT_BUFS_TOP_NODE, get_clipboard_file_path());
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

//-----------------------------------------------------------------------------
#ifdef ENABLE_HELP
int doe_splash(void)
{
	disp_splash(100);

	examine_key_code();

	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_view_func_list(void)
{
	view_list(HELP_BUF_IDX_EDITOR_FUNC_LIST);
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
	tio_fill_screen(0);
	display_color_pairs(0, 0);
	if (examine_key_code()) {
		return;
	}

#ifdef ENABLE_DEBUG
	tio_fill_screen(0);
	display_item_colors(0, 0);
	if (examine_key_code()) {
		return;
	}
#ifdef ENABLE_REGEX
	tio_fill_screen(0);
	display_bracket_hl_colors(0, 0);
#endif // ENABLE_REGEX
#endif // ENABLE_DEBUG
}

int examine_key_code(void)
{
	disp_status_bar_ing(_("Input key to show key code"));
	key_code_t key = input_key_loop();
	disp_status_bar_done(_("Key code input: %04x: [%s]"), key, key_name_from_key_code(key, NULL));
	return key == K_ESC;
}

//-----------------------------------------------------------------------------
int doe_editor_menu_0(void)
{
	return editor_menu_n(-1);
}

int doe_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
#define MAX_APP_STACK_DEPTH		(1+3)						// 1 root + 3 sub
app_win_stack_entry app_win_stack[MAX_APP_STACK_DEPTH+1];	// 1 root + 3 sub + 1 current-state

int cur_app_win_stack_depth = 0;
void clear_app_win_stack_depth()
{
	cur_app_win_stack_depth = 0;
	clear_app_win_stack_entry(cur_app_win_stack_depth);
}
void set_app_win_stack_depth(int depth)
{
	cur_app_win_stack_depth = depth;
}
int get_app_win_stack_depth()
{
	return cur_app_win_stack_depth;
}
int inc_app_win_stack_depth()
{
	cur_app_win_stack_depth++;
	return cur_app_win_stack_depth;
}
int dec_app_win_stack_depth()
{
	if (cur_app_win_stack_depth > 0) {
		cur_app_win_stack_depth--;
	}
	return cur_app_win_stack_depth;
}
app_win_stack_entry *get_app_win_stack_entry(int depth)
{
	if (depth < 0) {
		depth = cur_app_win_stack_depth;
	}
	return &(app_win_stack[depth]);
}
void clear_app_win_stack_entry(int depth)
{
	app_win_stack_entry *app_win = get_app_win_stack_entry(depth);
	memset(app_win, 0x00, sizeof(*app_win));
}

#ifdef ENABLE_FILER
void push_app_win(editor_panes_t *next_eps, be_buf_t *buf, filer_panes_t *next_fps)
#else // ENABLE_FILER
void push_app_win(editor_panes_t *next_eps, be_buf_t *buf)
#endif // ENABLE_FILER
{
/////_D_(dump_editor_panes(NULL))
	app_win_stack_entry *app_win = get_app_win_stack_entry(-1);
	app_win->appmode_save = app_mode__;
	app_win->editor_panes_save = NULL;
#ifdef ENABLE_FILER
	app_win->filer_panes_save = NULL;
#endif // ENABLE_FILER
	if (next_eps) {
		app_win->editor_panes_save = cur_editor_panes;
		init_cur_editor_panes(next_eps, buf);
	}
#ifdef ENABLE_FILER
	if (next_fps) {
		int cur_pane_idx = get_filer_cur_pane_idx();
		filer_panes_t *prev_fps = cur_filer_panes;	// previous filer panes
		app_win->filer_panes_save = cur_filer_panes;
		init_cur_filer_panes(next_fps, prev_fps->filer_views[cur_pane_idx].cur_dir);
	}
#endif // ENABLE_FILER

	set_win_depth(inc_app_win_stack_depth());
	// clear previous message displayed on the status bar
	clear_app_win_stack_entry(-1);
}
void pop_app_win(BOOL return_buf)
{
mflf_d_printf("return_buf: %d\n", return_buf);
	set_win_depth(dec_app_win_stack_depth());

	app_win_stack_entry *app_win = get_app_win_stack_entry(-1);
	app_mode__ = app_win->appmode_save;
	if (app_win->editor_panes_save) {
		if (return_buf) {
			copy_editor_panes(app_win->editor_panes_save, cur_editor_panes);
		}
		destroy_editor_panes();
		set_cur_editor_panes(app_win->editor_panes_save);
	}
#ifdef ENABLE_FILER
	if (app_win->filer_panes_save) {
		destroy_filer_panes();
		set_cur_filer_panes(app_win->filer_panes_save);
	}
#endif // ENABLE_FILER
}
void save_cur_app_state(int depth)
{
	depth = MIN_MAX_(0, depth, MAX_APP_STACK_DEPTH);
	app_win_stack_entry *app_win = get_app_win_stack_entry(depth);
	app_win->appmode_save = app_mode__;
	if (cur_editor_panes) {
		app_win->editor_panes_save = cur_editor_panes;
	}
#ifdef ENABLE_FILER
	if (cur_filer_panes) {
		app_win->filer_panes_save = cur_filer_panes;
	}
#endif // ENABLE_FILER
}
void load_cur_app_state(int depth)
{
	depth = MIN_MAX_(0, depth, MAX_APP_STACK_DEPTH);
	app_win_stack_entry *app_win = get_app_win_stack_entry(depth);
	app_mode__ = app_win->appmode_save;
	if (app_win->editor_panes_save) {
		set_cur_editor_panes(app_win->editor_panes_save);
	}
#ifdef ENABLE_FILER
	if (app_win->filer_panes_save) {
		set_cur_filer_panes(app_win->filer_panes_save);
	}
#endif // ENABLE_FILER
}

PRIVATE void update_screen_app__(int status_bar, int refresh);
void update_screen_app(int status_bar, int refresh)
{
	int cur_app_win_stack_depth = get_app_win_stack_depth();
	save_cur_app_state(get_app_win_stack_depth());
	for (int depth = 0; ; depth++) {
		set_win_depth(depth);
		set_app_win_stack_depth(depth);
		load_cur_app_state(depth);

		update_screen_app__(status_bar, refresh && (depth >= cur_app_win_stack_depth));
		if (depth >= cur_app_win_stack_depth) {
			break;
		}
		inc_win_depth();
		set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
		main_win_clear_screen();		// draw dark frame
		inc_win_depth();
	}
}
PRIVATE void update_screen_app__(int status_bar, int refresh)
{
#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
#endif // ENABLE_FILER
		set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
		update_screen_editor(status_bar, refresh);
#ifdef ENABLE_FILER
	} else {
		update_screen_filer(status_bar, refresh);
	}
#endif // ENABLE_FILER
}

//-----------------------------------------------------------------------------

PRIVATE void disp_status_bar_editor(void);

void update_screen_editor(int status_bar, int refresh)
{
	win_select_win(WIN_IDX_MAIN);

	EPCBVC_CURS_Y = MIN(edit_win_get_text_lines()-1, EPCBVC_CURS_Y);
////mflf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{\n");
	// title bar
	disp_title_bar_editor();

	// status bar
	if (status_bar) {
		disp_status_bar_editor();
	} else {
		redisp_status_bar();
	}
	// key list
	disp_key_list_editor();

	if (get_edit_win_update_needed()) {
		if (GET_APPMD(ed_EDITOR_PANES) == 0) {		// 1 pane
			win_select_win(WIN_IDX_SUB_WHOLE);
			disp_edit_win(1);
		} else {									// 2 panes
			int pane_sel_idx;		// 0: not current pane, 1: current pane
			for (pane_sel_idx = 0; pane_sel_idx < EDITOR_PANES; pane_sel_idx++) {
				int pane_idx;			// pane index
				// 1st, update not current pane.
				// 2nd, update current pane.
				pane_idx = 1 - get_editor_cur_pane_idx();	// 0 ==> 1, 1 ==> 0
				win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
				if (pane_sel_idx == 0) {
					set_work_space_color_dark();
				}
///flf_d_printf("pane_sel_idx: %d, pane_idx: %d\n", pane_sel_idx, pane_idx);
				set_editor_cur_pane_idx(pane_idx);

				disp_edit_win(pane_sel_idx);
				if (pane_sel_idx == 0) {
					clear_work_space_color_dark();
				}
			}
		}
	}

	set_edit_cursor_pos();
	if (refresh) {
		tio_refresh();
	}
	clear_edit_win_update_needed();
////mflf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}\n");
}

//-----------------------------------------------------------------------------

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
	//  0 ==>  0 ==>  0 ...
	// +1 ==> -1 ==> +1 ...
	blink_counter *= -1;
	//  0 ==>  1 ==>  0 ...
	return blink_counter < 0;
}

PRIVATE int get_title_bar_inversion();
PRIVATE void blink_editor_title_bar();

#ifdef ENABLE_UNDO
#define SHOW_UNDO_BUFS
#endif // ENABLE_UNDO
#define SHOW_MEM_FREE_ON_EBUFS_CHG	// show memory free on edit buffers change
									//  012345678901234
#define BUF_BUF_LEN			15		// "E99 C99 U99 R99"
#define MEM_BUF_LEN			(1+8)	// " 9999000M" (9999G)
#define HHCMMCSS_BUF_LEN	(1+8)	// " 23:59:59"/" 24/10/09"
PRIVATE char editor_title_bar_buf[MAX_SCRN_LINE_BUF_LEN+1] = "";
//1:/home/...editor2.c[Mod]    Mc e99c0u0r0 1234M 11:55:04
void disp_title_bar_editor(void)
{
	static int prev_edit_bufs = 0;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_status[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_num[2+1];
	int edit_bufs;
	int cut_bufs;
#ifdef SHOW_UNDO_BUFS
	int undo_bufs;
	int redo_bufs;
#endif // SHOW_UNDO_BUFS
	char buf_bufs[BUF_BUF_LEN+1] = "";
#ifdef SHOW_MEM_FREE_ON_EBUFS_CHG
	char buf_mem[MEM_BUF_LEN+1];
#endif // SHOW_MEM_FREE_ON_EBUFS_CHG
	char buf_time[HHCMMCSS_BUF_LEN+1];

	int bufs_idx = bufs_get_bufs_idx_in_bufss(NODES_TOP_ANCH(&all_bufferss), get_epc_buf());
/////flf_d_printf("bufs_idx: %d\n", bufs_idx);
	int buf_idx = buf_get_buf_idx(get_epc_buf());
/////flf_d_printf("buf_idx: %d\n", buf_idx);
	char *path = get_epc_buf()->file_path_;

	tio_set_cursor_on(0);

	//-------------------------------------------------------------------------
	char separator_char = indication_of_app_mode();
	snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%s%d%c%d%c%d:%s",
	 root_notation(),
	 get_editor_cur_pane_idx()+1, separator_char, bufs_idx, separator_char, buf_idx,
	 (path[0] == '\0') ? _("New File") : path);
	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[Mod] "));
	}
	if (CUR_EBUF_STATE(buf_MODE)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, get_str_buf_view_mode());
	}
	if (is_st_writable(&get_epc_buf()->orig_file_stat) == 0) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[RO] "));
	}

	//-------------------------------------------------------------------------
	// edit buffer cut mode
	if (BUF_STATE(get_epc_buf(), buf_CUT_MODE) != CUT_MODE_0_LINE) {
		strcat_printf(buf_bufs, MAX_SCRN_LINE_BUF_LEN, " %s", buf_cut_mode_str(get_epc_buf()));
	}
	// edit buffers
	edit_bufs = edit_bufs_count_bufs();
	strcat_printf(buf_bufs, MAX_SCRN_LINE_BUF_LEN, " e%s", zz_from_num(edit_bufs, buf_num));
	// cut buffers
	cut_bufs = count_cut_bufs();
	strcat_printf(buf_bufs, MAX_SCRN_LINE_BUF_LEN, "c%s", zz_from_num(cut_bufs, buf_num));
#ifdef SHOW_UNDO_BUFS
	// undo buffers
	undo_bufs = count_undo_bufs() / 2;
	strcat_printf(buf_bufs, MAX_SCRN_LINE_BUF_LEN, "u%s", zz_from_num(undo_bufs, buf_num));
	// redo buffers
	redo_bufs = count_redo_bufs() / 2;
	strcat_printf(buf_bufs, MAX_SCRN_LINE_BUF_LEN, "r%s", zz_from_num(redo_bufs, buf_num));
#endif // SHOW_UNDO_BUFS

	// current date / time
	snprintf_(buf_time, HHCMMCSS_BUF_LEN+1, " %s",
	 cur_ctime_cdate(msec_past_input_key() < 1000));

	//-------------------------------------------------------------------------
#ifdef SHOW_MEM_FREE_ON_EBUFS_CHG
	if (edit_bufs != prev_edit_bufs) {
		prev_edit_bufs = edit_bufs;
		// free memory in MB
		snprintf_(buf_mem, MEM_BUF_LEN+1, " %7dM", get_mem_free_in_kb(1)/1000);
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_bufs, buf_mem);
	} else {
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_bufs, buf_time);
	}
#else // SHOW_MEM_FREE_ON_EBUFS_CHG
	snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_bufs, buf_time);
#endif // SHOW_MEM_FREE_ON_EBUFS_CHG

	int path_cols = LIM_MIN(0, main_win_get_columns() - strlen_path(buf_status));
	shrink_str(buf_path, path_cols, 2);
	adjust_utf8s_columns(buf_path, path_cols);

	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%s%s", buf_path, buf_status);
	strcpy__(editor_title_bar_buf, buffer);
	blink_editor_title_bar();

	tio_set_cursor_on(1);
}
PRIVATE void blink_editor_title_bar()
{
	set_title_bar_color_by_state(BUF_STATE(get_epc_buf(), buf_CUT_MODE),
	 CUR_EBUF_STATE(buf_MODIFIED) ? 2 : (is_any_edit_buf_modified() ? 1 : 0),
	 get_title_bar_inversion());
	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0, editor_title_bar_buf, -1);
}

//-----------------------------------------------------------------------------

PRIVATE void disp_status_bar_editor(void)
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
	disp_status_bar_cursor(
	 buffer, EPCBVC_CL->line_num, get_epc_buf()->buf_lines, xx, disp_len,
	 get_epc_buf()->buf_size, buf_lines_sel, buf_char_code,
	 buf_enc_str(get_epc_buf()), buf_eol_str(get_epc_buf()));
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
 "<doe_view_func_list>KeyList "
#endif // ENABLE_HELP
 "<doe_switch_to_prev_buffer>PrevFile "
 "<doe_switch_to_next_buffer>NextFile "
#if APP_REL_LVL <= APP_REL_LVL_TEST
 "<doe_switch_to_prev_buffers>PrevBufs "
 "<doe_switch_to_next_buffers>NextBufs "
#endif // APP_REL_LVL
	};
	disp_key_list(editor_key_lists);
}

//-----------------------------------------------------------------------------

int is_editor_view_mode_then_warn_it(void)
{
	if (is_app_view_list_mode()) {
		disp_status_bar_err(_("Modification not allowed in VIEW/LIST mode"));
		return 1;
	}
	if (is_epc_buf_view_mode()) {
		disp_status_bar_err(_("Modification not allowed in this buffer mode"));
		return 1;
	}
	return 0;
}
int is_editor_view_mode(void)
{
	return is_app_list_mode() || is_epc_buf_view_mode();
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
