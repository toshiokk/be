/**************************************************************************
 *   fileio.h                                                             *
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

#ifndef fileio_h
#define fileio_h

#define PROJ_FILE_EXTENSION1	".bep"		// BE Project file
#define PROJ_FILE_EXTENSION2	".pro"		// Qt PROject file

#define MAX_LINES_LOADABLE		9999999	// 9,999,999

int load_file_into_new_buf(const char *full_path, int flags);

int load_file_into_buf(be_buf_t *buf, const char *full_path);

#define BACKUP_FILE_SUFFIX		"~"
int backup_and_save_cur_buf_ask(void);
int input_new_file_name__ask(char *file_path);
int backup_and_save_cur_buf(const char *file_path);

int save_buf_to_file(be_buf_t *buf, const char *file_path);
int save_cur_buf_to_file(const char *file_path);

void clear_files_loaded(void);
int add_files_loaded(int files);
int get_files_loaded(void);
void disp_files_loaded_if_ge_0(void);
void disp_files_loaded(void);

#define MAX_LOG_FILE_SIZE_MB		2								// 2 * n [MB]
int reduce_log_file_size(const char *file_path, int size_in_mb);
const char *get_exec_log_file_path();

#endif // fileio_h

// End of fileio.h
