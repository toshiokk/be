/**************************************************************************
 *   history.h                                                            *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#ifndef history_h
#define history_h

#define HISTORY_TYPE_IDX_FILE		0	// app
#define HISTORY_TYPE_IDX_DIR		1	// app
#define HISTORY_TYPE_IDX_SEARCH		2	// app
#define HISTORY_TYPE_IDX_EXEC		3	// app
#define HISTORY_TYPE_IDX_KEYMACRO	4	// app
#define HISTORY_TYPE_IDX_SHELL		5	// shell

#define HISTORY_TYPES_APP			5	// types of app histories (5)
#define HISTORY_TYPES_APP_AND_SHELL	6	// types of app and shell histories (5+1)

#ifdef ENABLE_HISTORY

#define _SEARCH_HISTORY_FILE_NAME		"search_history"
#define _EXEC_HISTORY_FILE_NAME			"exec_history"
#define _DIR_HISTORY_FILE_NAME			"dir_history"
#define _OPENFILE_HISTORY_FILE_NAME		"file_history"
#define _KEYMACRO_HISTORY_FILE_NAME		"keymacro_history"
#if defined(APP_DIR)
#define SEARCH_HISTORY_FILE_NAME		_SEARCH_HISTORY_FILE_NAME
#define EXEC_HISTORY_FILE_NAME			_EXEC_HISTORY_FILE_NAME
#define DIR_HISTORY_FILE_NAME			_DIR_HISTORY_FILE_NAME
#define OPENFILE_HISTORY_FILE_NAME		_OPENFILE_HISTORY_FILE_NAME
#define KEYMACRO_HISTORY_FILE_NAME		_KEYMACRO_HISTORY_FILE_NAME
#else // APP_DIR
#define SEARCH_HISTORY_FILE_NAME		"." _SEARCH_HISTORY_FILE_NAME
#define EXEC_HISTORY_FILE_NAME			"." _EXEC_HISTORY_FILE_NAME
#define DIR_HISTORY_FILE_NAME			"." _DIR_HISTORY_FILE_NAME
#define OPENFILE_HISTORY_FILE_NAME		"." _OPENFILE_HISTORY_FILE_NAME
#define KEYMACRO_HISTORY_FILE_NAME		"." _KEYMACRO_HISTORY_FILE_NAME
#endif // APP_DIR
#define BASH_HISTORY_FILE_NAME			".bash_history"

// Maximum number of search/execution/directory history strings saved
#define MAX_HISTORY_LINES		1000
#define MAX_HISTORY_LINES_10K	10000

void init_histories(void);

void load_histories(void);
void save_histories(void);

void update_history(int hist_type_idx, const char *str);
void append_history(int hist_type_idx, const char *str);

const char *get_history_newest(int hist_type_idx, int last_n);
const char *get_history_completion(int hist_type_idx, const char *str);
const char *search_history_file_path(int hist_type_idx, const char *path);

void set_history_newest(int hist_type_idx);
const char *get_history_older(int hist_type_idx);

const char* history_last_line_str(int hist_type_idx);

//------------------------------------------------------------------------------
void dir_history_append_temporarily(const char *dir);
int dir_history_remove();
int dir_history_fix();

//------------------------------------------------------------------------------
int select_from_history_list(int hist_type_idx, char *buffer);

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_history_ix(int hist_type_idx);
void dump_hist_bufs(void);
void dump_hist_bufs_lines(void);
#endif // ENABLE_DEBUG

#endif // ENABLE_HISTORY

#endif // history_h

// End of history.h
