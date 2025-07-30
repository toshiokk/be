/**************************************************************************
 *   editorfile.h                                                         *
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

#ifndef editorfile_h
#define editorfile_h

// "doe_..."  DO Editor function
void doe_open_file_recursive();
void doe_open_file_ro();
void doe_open_file_locked();
void doe_open_new_file();
void doe_open_proj_file();
void doe_open_exec_log_file();
int _doe_open_proj_file();
int _doe_open_exec_log_file();
void doe_reopen_file_last_line();
void doe_reopen_file();

void doe_write_file_to();

void doe_write_file_ask();
void doe_write_file_always();
void doe_write_all_ask();
void doe_write_all_modified();

void doe_close_file_ask();
void doe_close_file_always();
void doe_close_all_ask();
void doe_close_all_modified();

void doe_read_file_into_cur_buf();

//------------------------------------------------------------------------------
typedef enum {
	NO_CLOSE_AFTER_SAVE_0,	// 0: File will be kept open
	CLOSE_AFTER_SAVE_1		// 1: File will be closed after save
							//    (ANSWERING No WILL DISCARD CHANGES)
} close_after_save_t;

int write_all_ask(int yes_no, close_after_save_t close);
int close_all_not_modified();
int close_all_saved();
int write_file_ask(int yes_no, close_after_save_t close);

//------------------------------------------------------------------------------
void lock_epc_buf_if_file_already_locked(BOOL lock_buffer_if_already_locked);
void unlock_epc_buf_if_file_had_locked_by_myself();

#endif // editorfile_h

// End of editorfile.h
