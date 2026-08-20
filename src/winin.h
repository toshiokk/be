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

// app_do_next
typedef enum {
	EF_NONE						= 0,	// nothing done yet and nothing to do next
	// To update file list in the filer loop:
	FL_UPDATE_AUTO				= 2+0,	// automatic periodic file list update
	FL_UPDATE_FORCE				= 2+1,	// force immediate file list update
	EF_CANCELLED				= 4+0,	// input cancelled
	EF_TO_QUIT					= 4+1,	// quit editor/filer
	// Action done and next action to do:
	EF_LOADED_GO_TO_ROOT_EDITOR	= 6+0,	// file was loaded and return from editor/filer
										//  to the root editor
	EF_EXECUTED_RET_TO_CALLER	= 6+1,	// external command was executed
										// and return from editor/filer
										//  to the root caller(filer/editor)
#ifdef ENABLE_FILER
	EF_GO_TO_LEVEL_FILER		= 8,	// current directory changed in editor
										// or requested to open filer
										//  go to the filer of the same level
#endif // ENABLE_FILER
	// To enter text into input_string() from editor/filer:
	EF_ENTER_STRING				= 10+0,	// enter string(file/dir name or path) to replace
	EF_ENTER_STRING_ADD			= 10+1,	// enter string(file/dir name or path) to add
	// To enter file path to copy or move in input_string():
	EF_INPUT_PATH_TO_COPY		= 12+0,	// input file path to copy
	EF_INPUT_PATH_TO_MOVE		= 12+1,	// input file path to move
} do_next_t;

#define IS_EF_ENTER_STRING(do_next)				\
	(((do_next) == EF_ENTER_STRING)				\
	 || ((do_next) == EF_ENTER_STRING_ADD)		\
	 || ((do_next) == EF_INPUT_PATH_TO_COPY)	\
	 || ((do_next) == EF_INPUT_PATH_TO_MOVE))
#define IS_EF_LOADED_OR_EXECUTED(do_next)			\
	(((do_next) == EF_LOADED_GO_TO_ROOT_EDITOR)		\
	 || ((do_next) == EF_EXECUTED_RET_TO_CALLER))

const char *get_do_next_name(do_next_t do_next);

typedef enum {
	ENTER_FILE_NAME	= 0,		// filename
	ENTER_FILE_PATH	= 1,		// /path/to/file
	ENTER_DIR_PATH	= 2,		// /path/to/directory
} fname_0_fpath_1_dpath_2_t;
extern fname_0_fpath_1_dpath_2_t input_fname_0_fpath_1;

do_next_t input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...);
do_next_t input_full_path(const char *default__, char *input_buf, int cursor_byte_idx,
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
#define ASK_OTHERS		0x800	// other answers are to be mapped to CANCEL

							// positive answers ------------------------------
#define ANSWER_FORCE	9	//  force saving even if not-modified
#define ANSWER_ALL		8	//  save all if modified or replace all
#define ANSWER_REDO		5	//  Redo replace
#define ANSWER_UNDO		4	//  Undo replace
#define ANSWER_FORWARD	3	//  search Forward
#define ANSWER_BACKWARD	2	//  search Backward
#define ANSWER_YES		1	//  Yes (save or replace)
							// negative answers ------------------------------
#define ANSWER_NO		0	//  No (save or replace)
							// invalid answers -------------------------------
#define ANSWER_NONE		-1	//  Not yet answered
							// canceling process answers ---------------------
#define ANSWER_CANCEL	-2	//  cancel replacing and NOT-return to the beginning pos
#define ANSWER_END		-3	//  cancel replacing and return to the beginning pos
#define ANSWER_QUIT		-4	//  quit from application / cancel execution
int ask_yes_no(int flags, const char *msg, ...);

void disp_fkey_list();
void disp_key_list_lines(const char *key_lists[]);

void sync_cut_buffers_and_histories(char soon);

#endif // winin_h

// End of winin.h
