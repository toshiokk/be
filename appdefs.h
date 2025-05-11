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
* @file		appdefs.h
* @brief	Application level definitions
* @author	Creator
* @date		Creation date
* @details	Detailed description
*/

#ifndef appdefs_h
#define appdefs_h

#define APP_NAME		"BE"
#define BIN_NAME		"be"
#define APP_LONG_NAME	APP_NAME " editor"

#define APP_DIR				".be"		// $HOME/.be
#define RC_FILE_NAME		".berc"		// default RC file name (changeable by command line)
#define EXIT_FILE_NAME		".beexit"
#define WARNING_FILE_NAME	"warning"

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

// NOTE: how to use single line comment
//	// simple comment
//	//! doxygen comment
//	/// doxygen comment
//	////  code commented out
//	(TABs)	////code temporarily commented out
//	//PPP : pending(temporarily commented-out) code
//	//TTT : temporary added or commented out code for test
//	//DDD : code to be deleted
//	///// : code to be deleted
//	//DDDD : code be able to be deleted soon
//	////// : code be able to be deleted soon
// DNU : Do Not Use

// NOTE: length of string are categorized into three types
//	MAX_SCRN_LINE_BUF_LEN	768*6 = 4608
//	MAX_PATH_LEN			4096
//	MAX_EDIT_LINE_LEN		4096

#endif // appdefs_h

// End of appdefs.h
