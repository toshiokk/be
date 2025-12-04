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
#ifndef ENABLE_DEBUG
#define SET_editor_do_next(do_next)		editor_do_next = (do_next)
#else // ENABLE_DEBUG
#define SET_editor_do_next(do_next)		editor_do_next = (do_next) \
  , hmflf_dprintf("set-editor_do_next(%s)\n", get_ef_name(editor_do_next))
#endif // ENABLE_DEBUG

int do_call_editor(int push_win, int list_mode, be_buf_t *buf, char *str_buf);

int chk_inp_str_ret_val_editor(int ret);

const char *get_app_dir();

const char *get_clipboard_file_path();
int save_newest_cut_buf_to_clipboard_file();

//------------------------------------------------------------------------------
void doe_run_line_soon();
void doe_run_line_input();

//------------------------------------------------------------------------------

#ifdef ENABLE_HELP
void doe_splash();
void do_splash();
void doe_view_file_list();
void doe_view_func_list();
#endif // ENABLE_HELP

void display_color_settings(key_code_t key);

key_code_t examine_key_code(key_code_t key);

void doe_quit_editor();

void doe_menu_0();

void doe_inc_key_list_lines();

typedef struct /*app_stack_entry*/ {
	app_mode_t appmode_save;
	editor_panes_t *editor_panes_save;
#ifdef ENABLE_FILER
	filer_panes_t *filer_panes_save;
#endif // ENABLE_FILER
	char status_bar_displayed;
	char status_bar_prev_msg[MAX_SCRN_LINE_BUF_LEN+1];
	char status_bar_color_idx;
	short status_bar_col_idx;	// column position of position indicator
	char status_bar_msg[MAX_SCRN_LINE_BUF_LEN+1];
} app_stack_entry;

void clear_app_stack_depth();
void set_app_stack_depth(int depth);
int inc_app_stack_depth();
int dec_app_stack_depth();
int get_app_stack_depth();
app_stack_entry *get_app_stack_ptr(int depth);
void clear_app_stack_entry(int depth);

#ifdef ENABLE_FILER
void push_app_stack(editor_panes_t *next_eps, be_buf_t *buf, filer_panes_t *next_fps);
#else // ENABLE_FILER
void push_app_stack(editor_panes_t *next_eps, be_buf_t *buf);
#endif // ENABLE_FILER
void pop_app_stack(BOOL change_parent_editor, BOOL change_parent_filer);
void save_cur_app_state(int depth);
void load_cur_app_state(int depth);

void update_screen_app(s_b_d_t status_bar, int refresh);

void update_screen_editor(s_b_d_t status_bar, int refresh);

void start_title_bar_blinking();
void stop_title_bar_blinking();
void disp_title_bar_editor();

int is_editor_unmodifiable_then_warn_it();

#ifdef ENABLE_DEBUG
void dump_cur_pointers();
#endif // ENABLE_DEBUG

#endif // editor_h

// End of editor.h
