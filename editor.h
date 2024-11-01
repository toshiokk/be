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

int call_editor(int push_win, int list_mode, char *str_buf, int buf_len);

int chk_inp_str_ret_val_editor(int ret);

char *get_app_dir(void);

const char *get_clipboard_file_path();
int save_cut_buf_to_clipboard_file();
int load_clipboard_into_cut_buf();

int doe_read_clipboard_into_cur_char();
int doe_read_clipboard_into_cur_line();
int doe_read_clipboard_into_cur_pos_(int char0_line1);

//-----------------------------------------------------------------------------

int doe_run_line_soon(void);
int doe_run_line_soon_w_log(void);
int doe_run_line_soon_(int logging);

#ifdef ENABLE_FILER
int doe_call_filer(void);
#endif // ENABLE_FILER

//-----------------------------------------------------------------------------

#ifdef ENABLE_HELP
int doe_splash(void);
int doe_view_key_list(void);
#endif // ENABLE_HELP

int doe_display_color_settings(void);
void display_color_settings(void);

int doe_editor_menu_0(void);
int doe_editor_menu_1(void);
int doe_editor_menu_2(void);
int doe_editor_menu_3(void);
int doe_editor_menu_4(void);
int doe_editor_menu_5(void);
int doe_editor_menu_6(void);
int doe_editor_menu_7(void);
int doe_editor_menu_8(void);

int doe_inc_key_list_lines(void);

void win_push_win_size(void);
void win_pop_win_size(void);
int update_screen_app(int title_bar, int status_bar, int refresh);

int update_screen_editor(int title_bar, int status_bar, int refresh);
int disp_status_bar_editor(void);
void disp_key_list_editor(void);

int is_editor_view_mode_then_warn_it(void);
int is_editor_view_mode(void);
int is_epc_view_mode(void);

#ifdef ENABLE_DEBUG
void dump_cur_pointers(void);
#endif // ENABLE_DEBUG

#endif // editor_h

// End of editor.h
