/**************************************************************************
 *   appdefs.h                                                            *
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
/**
* @file appdefs.h
* @brief Application level definitions
* @author 作成者
* @date 作成日
* @details 詳細な説明
*/

#ifndef appdefs_h
#define appdefs_h

#define APP_REL_LVL 0		// Application Release Level
#define APP_REL_LVL_EXPERIMENTAL	0	// experimental release
#define APP_REL_LVL_TEST			1	// test         release
#define APP_REL_LVL_STABLE			2	// stable       release

#define APP_NAME		"BE"
#define BIN_NAME		"be"

#define APP_DIR				".be"
#define RC_FILE_NAME		"berc"		// default RC file name (changeable by command line option)
#define EXIT_FILE_NAME		".beexit"

#define MAX_APP_PANES_2		2

// top_win
#define TITLE_LINE		0
#define TITLE_LINES		1
// edit_win
#define RULER_LINE		0
// bottom_win
#define STATUS_LINE		0
#define STATUS_LINES	1
#define KEY_LIST_LINE	STATUS_LINES

#define DEFAULT_TAB_SIZE		8
#define MAX_TAB_SIZE			8

#define CURSOR_POS_BYTE		// col_num is byte count
///#define CURSOR_POS_COLUMN	// col_num is column in view
#if (!defined(CURSOR_POS_BYTE) && !defined(CURSOR_POS_COLUMN)) \
 || (defined(CURSOR_POS_BYTE) && defined(CURSOR_POS_COLUMN))
#warning "!!!! both of CURSOR_POS_BYTE and CURSOR_POS_COLUMN are defined !!!!"
#warning " or "
#warning "!!!! both of CURSOR_POS_BYTE and CURSOR_POS_COLUMN are NOT defined !!!!"
#endif

// NOTE: how to use single line comment "//", "///", "////" and "/////"
//   simple comment
//! "doxygen comment"
/// "doxygen comment"
////  code commented out
///// code obsolete and to be deleted

///PPP pending(temporarily commented-out) code
///TTT temporary test code
///DDD to be deleted code

// NOTE: length of string buffer must be categorized into three types
//	MAX_SCRN_LINE_BUF_LEN	384*3 = 1152
//	MAX_PATH_LEN			4096
//	MAX_EDIT_LINE_LEN		4096

#endif // appdefs_h

// End of appdefs.h
