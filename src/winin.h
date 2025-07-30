/**************************************************************************
 *   winin.h                                                              *
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

#ifndef winin_h
#define winin_h

// editor_do_next_X, filer_do_next_X
typedef enum {
	EF_NONE						= 0,	// nothing done yet and nothing to do next
										// in filer loop:
	FL_UPDATE_FILE_LIST_AUTO	= 1,	//   periodic file list update
	FL_UPDATE_FILE_LIST_FORCE	= 2,	//   force immediate file list update
										// quit from editor/filer:
	EF_CANCELLED				= 3,	//   cancelled
	EF_QUIT						= 4,	//   quit editor/filer
	EF_LOADED					= 5,	//   file was loaded and return from editor/filer
	EF_EXECUTED					= 6,	//   command was executed and return from editor/filer
										// enter file/dir path from filer:
	FL_ENTER_FILE_NAME			= 7,	//   enter file name(file, dir)
	FL_ENTER_FILE_NAME_ADD		= 8,	//   enter file name(file, dir)
	FL_ENTER_FILE_PATH			= 9,	//   enter file path(/path/to/file, /path/to/dir)
	FL_ENTER_FILE_PATH_ADD		= 10,	//   enter file path(/path/to/file, /path/to/dir)
	FL_ENTER_DIR_PATH			= 11,	//   enter directory path (/path/to/dir)
	FL_ENTER_DIR_PATH_ADD		= 12,	//   enter directory path (/path/to/dir)
										// enter text from editor/filer:
	EF_ENTER_STRING				= 13,	//   enter string(file/dir name or path) to replace
	EF_ENTER_STRING_ADD			= 14,	//   enter string(file/dir name or path) to add
										// input text:
	EF_INPUT_PATH_TO_COPY		= 15,	//   input string(by Alt-c key)
	EF_INPUT_PATH_TO_MOVE		= 16,	//   input string(by Alt-m key)
} ef_do_next_t;

#define IS_EF_STRING_ENTERED(ret)		\
	((EF_ENTER_STRING <= (ret)) && ((ret) <= EF_ENTER_STRING_ADD))

extern int input_string_full_path;

int input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...);
int input_full_path(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...);

#define ASK_ALL			0x80
#define ASK_REDO		0x40
#define ASK_UNDO		0x20
#define ASK_END			0x10
#define ASK_BACKWARD	0x08
#define ASK_FORWARD		0x04
#define ASK_YES			0x02
#define ASK_NO			0x01
#define ASK_YES_NO		(ASK_YES | ASK_NO)

						// positive answers ----------------------
#define ANSWER_FORCE	9	// force save even if not-modified
#define ANSWER_ALL		8	// save all if modified or replace all
#define ANSWER_REDO		5	// Redo replace
#define ANSWER_UNDO		4	// Undo replace
#define ANSWER_FORWARD	3	// search Forward
#define ANSWER_BACKWARD	2	// search Backward
#define ANSWER_YES		1	// Yes (save or replace)
						// negative answers ----------------------
#define ANSWER_NO		0	// No (save or replace)
						// not valid answers ---------------------
#define ANSWER_NONE		-1	// Not yet answered
#define ANSWER_CANCEL	-2	// cancel string-replacing and not-return to beginning pos
#define ANSWER_END		-3	// cancel string-replacing and return to beginning pos
int ask_yes_no(int flags, const char *msg, ...);

void disp_fkey_list();
void disp_key_list_lines(const char *key_lists[]);

void sync_cut_buffers_and_histories();

#endif // winin_h

// End of winin.h
