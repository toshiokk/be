/**************************************************************************
 *   editorgoto.h                                                         *
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

#ifndef editorgoto_h
#define editorgoto_h

#define TUL0		0	// Not try to upper lower cases of file name
#define TUL1		1	// Try to open upper lower cases of file name
#define LFH0		0	// Not load a file memorized in history
#define LFH1		1	// Load a file memorized in history
#define RECURSIVE0	0	// Not recursively open files
#define RECURSIVE1	1	// Recursively open files

int doe_goto_input_line(void);
int doe_goto_file_or_dir_in_cur_line(void);
int doe_goto_file_or_dir_in_cur_cursor_pos(void);
int doe_goto_file_in_cur_line(void);
int doe_goto_file_in_cur_cursor_pos(void);
int doe_goto_directory_in_cur_line(void);
int change_dirs_in_string(const char *string, char *cur_dir, char *prev_dir, char *next_file);

int doe_open_files_in_buf(void);
int doe_switch_to_top_file(void);
int doe_switch_to_bot_file(void);
int doe_switch_to_prev_file(void);
int doe_switch_to_next_file(void);
#if APP_REL_LVL == APP_REL_LVL_EXPERIMENTAL
int doe_switch_to_prev_buffers(void);
int doe_switch_to_next_buffers(void);
#endif // APP_REL_LVL

void memorize_cur_file_pos_before_search();
int doe_return_to_prev_file_pos(void);

int doe_switch_editor_pane(void);
void doe_switch_editor_pane_(void);

//-----------------------------------------------------------------------------
// Top level functions - never called recursively
// | function name            | actions                                                     |
// |--------------------------|-------------------------------------------------------------|
// | load_files_in_cur_buf()  | Load files listed in current buffer							|
// | load_files_in_string()   | Load files listed in one line of string						|
// | load_file_name_upp_low() | Try to open file with name in as-it-is, UPPER or lower case	|
// | load_file_name__         | Load file and goto pos (memorized in 'openfile_history')	|
//
// Top level functions must:
//  1. call begin_check_break_key()
//  2. call equivalent sub-function (sub-function can be called recursively)
//  3. call end_check_break_key()

int load_file_name_upp_low(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int load_from_history, int recursive);
int load_files_in_cur_buf(void);
int load_files_in_string(const char *string, int files_to_load,
 int try_upp_low, int open_on_err, int msg_on_err, int load_from_history, int recursive);

int load_file_name_upp_low_(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int load_from_history, int recursive);

int is_file_name_proj_file(const char *file_name, int type);

#ifdef START_UP_TEST
void test_get_n_th_file_name(void);
#endif // START_UP_TEST

#ifdef ENABLE_HISTORY
int goto_last_file_line_col_in_history();
#endif // ENABLE_HISTORY

char *get_memorized_file_pos_buf(char *buffer);
const char *get_memorized_file_pos_str(const char *str);
char *memorize_cur_file_pos_null(char *buffer);
int recall_file_pos_null(const char *str);
int goto_str_line_col_in_cur_buf(const char *str);
int goto_line_col_in_cur_buf(int line_num, int col_no);

char *mk_cur_file_pos_str_static(void);
char *mk_cur_file_pos_str_buf(char *buffer);
char *mk_file_pos_str(char *buffer, const char *file_path, int line_num, int col_no);
int get_file_line_col_from_str_null(const char *str, char *file_path,
 int *line_num, int *col_num);

int switch_cep_buf_by_file_name(const char *file_name);
int switch_cep_buf_by_file_path(const char *abs_path);

int switch_cep_buf_to_top(void);
int switch_cep_buf_to_bot(void);
int switch_cep_buf_to_next(int beep_at_end, int goto_top);
int switch_cep_buf_to_prev(int beep_at_end, int goto_bottom);
int switch_cep_buf_to_valid_buf(void);
int switch_cep_buf_to_another_buf(void);

#endif // editorgoto_h

// End of editorgoto.h
