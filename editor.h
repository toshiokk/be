/**************************************************************************
 *   editor.h                                                             *
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

#ifndef editor_h
#define editor_h

extern ef_do_next_t editor_do_next;

int do_call_editor(int push_win, int list_mode, be_buf_t *buf, char *str_buf, int buf_len);

int chk_inp_str_ret_val_editor(int ret);

char *get_app_dir(void);

const char *get_clipboard_file_path();
int save_cut_buf_to_clipboard_file();
int load_clipboard_into_cut_buf();

int doe_read_clipboard_into_cur_char();
int doe_read_clipboard_into_cur_line();
int doe_read_clipboard_into_cur_pos_(int char0_line1);

//------------------------------------------------------------------------------

int doe_run_line_soon(void);
int doe_run_line_soon_w_log(void);
int doe_run_line_soon_(int logging);

//------------------------------------------------------------------------------

#ifdef ENABLE_HELP
int doe_splash(void);
int doe_view_func_list(void);
#endif // ENABLE_HELP

int doe_display_color_settings(void);
void display_color_settings(void);

int examine_key_code(void);

int doe_editor_menu_0(void);

int doe_inc_key_list_lines(void);

typedef struct /*app_win_stack_entry*/ {
	app_mode_t appmode_save;
	editor_panes_t *editor_panes_save;
#ifdef ENABLE_FILER
	filer_panes_t *filer_panes_save;
#endif // ENABLE_FILER
	char status_bar_displayed;
	char status_bar_prev_msg[MAX_SCRN_LINE_BUF_LEN+1];
	char status_bar_color_idx;
	short status_bar_col_idx;
	char status_bar_msg[MAX_SCRN_LINE_BUF_LEN+1];
} app_win_stack_entry;

void clear_app_win_stack_depth();
void set_app_win_stack_depth(int depth);
int inc_app_win_stack_depth();
int dec_app_win_stack_depth();
int get_app_win_stack_depth();
app_win_stack_entry *get_app_win_stack_entry(int depth);
void clear_app_win_stack_entry(int depth);

#ifdef ENABLE_FILER
void push_app_win(editor_panes_t *next_eps, be_buf_t *buf, filer_panes_t *next_fps);
#else // ENABLE_FILER
void push_app_win(editor_panes_t *next_eps, be_buf_t *buf);
#endif // ENABLE_FILER
void pop_app_win(BOOL change_caller);
void save_cur_app_state(int depth);
void load_cur_app_state(int depth);

void update_screen_app(int status_bar, int refresh);

void update_screen_editor(int status_bar, int refresh);

void start_title_bar_blinking();
void stop_title_bar_blinking();
void disp_title_bar_editor(void);

void disp_key_list_editor(void);

int is_editor_view_mode_then_warn_it(void);

#ifdef ENABLE_DEBUG
void dump_cur_pointers(void);
#endif // ENABLE_DEBUG

#endif // editor_h

// End of editor.h
