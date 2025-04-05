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

// file open flags
#define TUL1		0x01	// Try to open upper lower cases of file name
#define TUL0		0x00	//  Not try to upper lower cases of file name
#define OOE1		0x02	// Open On Error
#define OOE0		0x00	//  Not open on error
#define MOE1		0x04	// Message On Error
#define MOE0		0x00	//  No message on error
#define LFH1		0x08	// Load a file memorized in history
#define LFH0		0x00	//  Not load a file memorized in history
#define RDOL1		0x10	// Load a file as Read Only
#define RDOL0		0x00	//  Not load a file as Read Only
#define FOLF1		0x20	// Load a locked file forcibly (Force Open Locked file)
#define FOLF0		0x00	//  Not load a locked file forcibly
#define RECURS1		0x40	// RECURSively open files
#define RECURS0		0x00	//  Not RECURSively open files
#define MFPL1		0x80	// Many Files Per Line
#define MFPL0		0x00	//  One File Per Line
// NOTE: LFH1 is very slow. Do not use unless special necessity.

#define FILE_PATH_SEPARATOR		"|"		// candidates are "|", "//", ""\\"

int doe_goto_column(void);
int doe_goto_line(void);
int doe_tag_jump_in_cur_line(void);
int doe_tag_jump_in_cur_cursor_pos(void);
#ifdef ENABLE_FILER
int doe_filer(void);
int doe_goto_directory_in_cur_line();
int doe_goto_directory_in_cur_cursor_pos();
int try_to_open_dir_in_cur_line_with_filer(int line_byte_idx);
int try_to_open_dir_in_str_with_filer(const char *str);
#endif // ENABLE_FILER

int doe_open_files_in_buf(void);
int doe_switch_to_prev_buffer(void);
int doe_switch_to_next_buffer(void);
int doe_switch_to_top_buffer(void);
int doe_switch_to_bot_buffer(void);

int doe_switch_to_prev_buffers(void);
int doe_switch_to_next_buffers(void);

void memorize_cur_file_pos_before_jump();
int doe_return_to_prev_file_pos(void);

// Top level functions: never called recursively
// |function name           |actions                                                    |
// |------------------------|-----------------------------------------------------------|
// |load_files_in_cur_buf() |Load files listed in current buffer                        |
// |load_files_in_string()  |Load files listed in one line of string                    |
// |load_file_name_upp_low()|Try to open file with name in as-it-is, UPPER or lower case|
//
// Top level functions must do below steps:
//  1. call clear_files_loaded()		# clear counter
//  2. call begin_check_break_key()		# begin checking break-key
//  3. call equivalent sub-function		# sub-function can be called recursively
//  4. call end_check_break_key()		# end   checking break-key

// Top level functions:
int load_file_name_upp_low(const char *file_name, int flags);
int load_files_in_cur_buf(void);
int load_files_in_string(const char *string, int flags);

int load_file_name_upp_low_(const char *file_name, int flags);

int is_file_name_proj_file(const char *file_name, int type);

#ifdef START_UP_TEST
void test_get_n_th_file(void);
#endif // START_UP_TEST
#define MAX_FILES_TO_TRY_TO_LOAD_IN_A_LINE	10
int get_n_th_file_line_col_from_str(const char *str, int field_idx,
 char *file_path, int *line_num, int *col_num);
const char *skip_n_file_names(const char *line, int field_idx);

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
int get_file_line_col_from_str(const char *str, char *file_path,
 int *line_num, int *col_num);

int switch_epc_buf_by_file_path(const char *abs_path);
int switch_epc_buf_by_file_name(const char *file_name);

int switch_epc_buf_to_edit_buf();
int switch_epc_buf_to_top_of_edit_buf();
int switch_epc_buf_to_top_buf(void);
int switch_epc_buf_to_bot_buf(void);
int switch_epc_buf_to_next_buf(int beep_at_end, int goto_top);
int switch_epc_buf_to_prev_buf(int beep_at_end, int goto_bottom);
int switch_epc_buf_to_another_buf(void);

#endif // editorgoto_h

// End of editorgoto.h
