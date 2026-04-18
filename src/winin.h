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

// editor_do_next, filer_do_next, ret__
typedef enum {
	EF_NONE						= 0,	// nothing done yet and nothing to do next
										// in filer loop:
	FL_UPDATE_AUTO				= 1,	//   automatic periodic file list update
	FL_UPDATE_FORCE				= 2,	//   force immediate file list update
										// input string:
	EF_CANCELLED				= 3,	//   input cancelled
										// quit from editor/filer:
	EF_TO_QUIT					= 4,	//   quit editor/filer
										// action done and next action to do:
	EF_LOADED_GO_TO_ROOT_EDITOR	= 5,	//   file was loaded and return from editor/filer
										//    ==> go to the root editor
	EF_GO_TO_LEVEL_FILER		= 6,	//   current directory changed in editor
										//   or requested to open filer
										//    ==> go to the filer of the same level
	EF_EXECUTED_RET_TO_CALLER	= 7,	//   command was executed and return from editor/filer
										//    ==> return to the root caller(filer/editor)
										// to enter text into input_string() from editor/filer:
	EF_ENTER_STRING				= 8,	//   enter string(file/dir name or path) to replace
	EF_ENTER_STRING_ADD			= 9,	//   enter string(file/dir name or path) to add
										// file path has input in input_string():
	EF_INPUT_PATH_TO_COPY		= 10,	//   input string(by Alt-c key)
	EF_INPUT_PATH_TO_MOVE		= 11,	//   input string(by Alt-m key)
} ef_do_next_t;

#define IS_EF_ENTER_STRING(ret)			\
	((EF_ENTER_STRING <= (ret)) && ((ret) <= EF_INPUT_PATH_TO_MOVE))
#define IS_EF_LOADED_OR_EXECUTED(ret)	\
	(((ret) == EF_LOADED_GO_TO_ROOT_EDITOR) || ((ret) == EF_EXECUTED_RET_TO_CALLER))

const char *get_ef_name(ef_do_next_t do_next);

extern int input_file_name_0_file_path_1;

int input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...);
int input_full_path(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...);

#define ASK_YES			0x001
#define ASK_NO			0x002
#define ASK_ALL_YES		0x004
#define ASK_BACKWARD	0x008
#define ASK_FORWARD		0x010
#define ASK_UNDO		0x020
#define ASK_REDO		0x040
#define ASK_STOP		0x080
#define ASK_END			0x100
#define ASK_QUIT		0x200
#define ASK_YES_NO		(ASK_YES | ASK_NO)
#define ASK_YES_NO_QUIT	(ASK_YES | ASK_NO | ASK_QUIT)
#define ASK_NO_QUIT		(ASK_NO | ASK_QUIT)
#define ASK_OTHERS		0x800	// other answers than expected

						// positive answers ----------------------
#define ANSWER_FORCE	9	// force saving even if not-modified
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
#define ANSWER_CANCEL	-2	// cancel string-replacing and NOT-return to the beginning pos
#define ANSWER_END		-3	// cancel string-replacing and return to the beginning pos
#define ANSWER_QUIT		-4	// quit from application / cancel execution
int ask_yes_no(int flags, const char *msg, ...);

void disp_fkey_list();
void disp_key_list_lines(const char *key_lists[]);

void sync_cut_buffers_and_histories(char soon);

#endif // winin_h

// End of winin.h
