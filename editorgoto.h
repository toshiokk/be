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

#define TUL1		1	// Try upper lower
#define TUL0		0	// No try upper lower
#define RECURSIVE1	1	// Recursive open
#define RECURSIVE0	0	// Non-recursive open

int doe_goto_input_line(void);
int doe_goto_file_in_cur_line(void);
int doe_goto_directory_in_cur_line(void);

int doe_open_files_in_buf(void);
int doe_switch_to_top_file(void);
int doe_switch_to_bot_file(void);
int doe_switch_to_prev_file(void);
int doe_switch_to_next_file(void);
#ifdef ENABLE_EXPERIMENTAL
int doe_switch_to_prev_buffers(void);
int doe_switch_to_next_buffers(void);
#endif // ENABLE_EXPERIMENTAL
int doe_return_to_prev_file_pos(void);

int doe_switch_editor_pane(void);
void do_switch_editor_pane_(void);

int load_file_in_string(const char *string,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive);

int load_file_name_upp_low(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive);

int load_file_name_recurs(const char *file_name, int open_on_err, int msg_on_err, int recursive);

int switch_cep_buf_to_file_name(const char *file_name);
int switch_cep_buf_to_abs_path(const char *abs_path);
int switch_cep_buf_to_top(void);
int switch_cep_buf_to_bot(void);
int switch_cep_buf_to_next(int beep_at_end, int goto_top);
int switch_cep_buf_to_prev(int beep_at_end, int goto_bottom);
int switch_cep_buf_to_valid_buf(void);
int switch_cep_buf_to_another_buf(void);

int is_file_name_proj_file(const char *file_name, int type);

#ifdef START_UP_TEST
void test_get_n_th_file_name(void);
#endif // START_UP_TEST

char *get_memorized_file_pos_str();
char *memorize_cur_file_pos_null(char *buffer);
int recall_cur_file_pos_null(const char *str);
int goto_str_line_col_in_cur_buf(const char *str);
int goto_line_col_in_cur_buf(int line_num, int col_no);

char *mk_cur_file_pos_str_static(void);
char *mk_cur_file_pos_str(char *buffer);
char *mk_file_pos_str(char *buffer, const char *file_path, int line_num, int col_no);
const char *get_file_line_col_from_str_null(const char *str, char *file_path,
 int *line_num, int *col_num);

#endif // editorgoto_h

// End of editorgoto.h
