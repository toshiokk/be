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

extern int prev_key_executed;
extern int key_executed;
extern char prev_func_id[MAX_PATH_LEN+1];
typedef enum {
	EDITOR_NONE = 0,
	EDITOR_CANCELLED = 1,
	EDITOR_DONE = 2,
	EDITOR_ENTERED = 3,
} editor_quit_t;
extern editor_quit_t editor_quit;

int call_editor(int push_win, int list_mode);

//-----------------------------------------------------------------------------
// "doe_..."  DO Editor function
int doe_open_file(void);
int doe_open_new_file(void);
int doe_open_proj_file(void);
int doe_reopen_file_last_line(void);
int doe_reopen_file(void);

int doe_write_file_to(void);

int doe_write_file_ask(void);
int doe_write_file_always(void);
int doe_write_all_ask(void);
int doe_write_all_modified(void);

int doe_close_file_ask(void);
int doe_close_file_always(void);
int doe_close_all_ask(void);
int doe_close_all_modified(void);

int doe_read_file_into_cur_pos(void);

const char *get_clipboard_file_path();
int save_cut_buf_to_clipboard_file();
int load_clipboard_into_cut_buf();

int doe_read_clipboard_into_cur_char();
int doe_read_clipboard_into_cur_line();
int do_read_clipboard_into_cur_pos(int char0_line1);

//-----------------------------------------------------------------------------

#ifdef ENABLE_FILER
int doe_run_line_soon(void);
int doe_call_filer(void);
#endif // ENABLE_FILER

//-----------------------------------------------------------------------------

#ifdef ENABLE_HELP
int doe_editor_splash(void);
#endif // ENABLE_HELP

int doe_display_color_pairs(void);

int doe_editor_menu_0(void);
int doe_editor_menu_1(void);
int doe_editor_menu_2(void);
int doe_editor_menu_3(void);
int doe_editor_menu_4(void);
int doe_editor_menu_5(void);
int doe_editor_menu_6(void);
int doe_editor_menu_7(void);
int doe_editor_menu_8(void);
int doe_editor_menu_9(void);

typedef enum {
	NO_CLOSE_AFTER_SAVE_0,	// 0: File will be kept open
	CLOSE_AFTER_SAVE_1		// 1: File will be closed after save
							//    (ANSWERING No WILL DISCARD CHANGES)
} close_after_save_t;

int write_all_ask(int yes_no, close_after_save_t close);
int close_all_not_modified(void);
int close_all();
int write_file_ask(int yes_no, close_after_save_t close);

int doe_inc_key_list_lines(void);

void win_push_win_size(void);
void win_pop_win_size(void);
int update_screen_app(int title_bar, int status_bar, int refresh);

int update_screen_editor(int title_bar, int status_bar, int refresh);
int disp_status_bar_editor(void);
void disp_key_list_editor(void);

int is_view_mode_then_warn_it(void);

#ifdef ENABLE_DEBUG
void dump_cur_pointers(void);
#endif // ENABLE_DEBUG

#endif // editor_h

// End of editor.h
