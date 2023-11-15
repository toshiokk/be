/**************************************************************************
 *   appmode.h                                                            *
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

#ifndef appmode_h
#define appmode_h

/**
* @brief application mode
* @details
*/
typedef struct /*app_mode*/ {
	// app_XXXX		settings for application (common to editor and filer)
	// ed_XXXX		settings for editor
	// fl_XXXX		settings for filer

	// application (editor and filer) settings
#ifdef ENABLE_RC
	unsigned char app_RCFILE:1;				// bit 0
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	unsigned char app_HISTORY:1;			// bit 1
#endif // ENABLE_HISTORY
	unsigned char app_DRAW_CURSOR:1;		// bit 2
	unsigned char app_UTF8:1;				// bit 3
	unsigned char app_MAP_KEY_7F_BS:1;		// bit 4
#define MAX_KEY_LINES			3
#define KEY_LINES_0				0
#define KEY_LINES_1				1
#define KEY_LINES_2				2
#define KEY_LINES_3				3
#define KEY_LINES_MAX			KEY_LINES_3
	unsigned char app_KEY_LINES:2;			// bit 5,6
#define DEBUG_NONE			0
#define DEBUG_PRINTF		1
	unsigned char app_DEBUG_PRINTF:1;		// bit 7
#define EF_EDITOR				0
#define EF_FILER				1
	unsigned char app_EDITOR_FILER:1;		// bit 8
	unsigned char app_LIST_MODE:1;			// bit 9

	// editor settings
	unsigned char ed_EDITOR_PANES:1;		// bit 10 (0: 1 pane / 1: 2 panes)
	unsigned char ed_DUAL_SCROLL:1;			// bit 11
	unsigned char ed_CONT_VIEW:1;			// bit 12
	unsigned char ed_SHOW_RULER:1;			// bit 13
	unsigned char ed_SHOW_LINE_NUMBER:1;	// bit 14
#define CURS_POSITIONING_NONE		0
#define CURS_POSITIONING_TOP		1
#define CURS_POSITIONING_CENTER		2
#define CURS_POSITIONING_BOTTOM		3
	unsigned char ed_CURS_POSITIONING:2;	// bit 15
#ifdef ENABLE_SYNTAX
	unsigned char ed_SYNTAX_HIGHLIGHT:1;	// bit 16
	unsigned char ed_TAB_EOL_NOTATION:1;	// bit 17
#endif // ENABLE_SYNTAX
	unsigned char ed_AUTO_INDENT:1;			// bit 18
	unsigned char ed_REVERSE_SEARCH:1;		// bit 19
	unsigned char ed_IGNORE_CASE:1;			// bit 20
#ifdef ENABLE_REGEX
	unsigned char ed_USE_REGEXP:1;			// bit 21
#endif // ENABLE_REGEX
#ifdef USE_NKF
	unsigned char ed_USE_NKF:1;				// bit 22
#endif // USE_NKF
#define BACKUP_FILES_0			0
#define BACKUP_FILES_1			1
#define BACKUP_FILES_2			2
#define BACKUP_FILES_3			3
#define BACKUP_FILES_10			10
#define BACKUP_FILES_15			15
#define BACKUP_FILES_MAX		BACKUP_FILES_15
	unsigned char ed_BACKUP_FILES:4;		// bit 23-26

	// filer settings
	unsigned char fl_SHOW_DOT_FILE:1;		// bit 27
#define FILE_SORT_BY_NAME		0
#define FILE_SORT_BY_NAME_REV	1
#define FILE_SORT_BY_EXT		2
#define FILE_SORT_BY_EXT_REV	3
#define FILE_SORT_BY_TIME		4
#define FILE_SORT_BY_TIME_REV	5
#define FILE_SORT_BY_SIZE		6
#define FILE_SORT_BY_SIZE_REV	7
#define FILE_SORT_BY_MAX		FILE_SORT_BY_SIZE_REV
	unsigned char fl_FILE_SORT_BY:3;		// bit 28-30
#define SHOW_FILE_INFO_0				0	// None
#define SHOW_FILE_INFO_1				1	// Size
#define SHOW_FILE_INFO_2				2	// Time
#define SHOW_FILE_INFO_3				3	// Size Time
#define SHOW_FILE_INFO_4				4	// Size Time 1777 User
#define SHOW_FILE_INFO_5				5	// Size Time lrwxrwxrwx User:group
#define SHOW_FILE_INFO_MAX				SHOW_FILE_INFO_5
	unsigned char fl_SHOW_FILE_INFO:3;		// bit 31-33
	unsigned char fl_FILER_PANES:1;			// bit 34 (0: 1 pane / 1: 2 panes)
} app_mode_t;

typedef struct /*buf_state*/ {
	unsigned char buf_MODIFIED:1;
	unsigned char buf_VIEW_MODE:1;
	unsigned char buf_LINE_WRAP_MODE:1;
#if 1
#define HV_IS_BOX_VH_IS_CHAR
#else
#define HV_IS_LINE_VH_IS_BOX
#endif

#define CUT_MODE_0_LINE			0	// no mark                                    (one line cut)
#define CUT_MODE_N_LINE			1	// marking started but cursor not moved           (line cut)
#define CUT_MODE_H_CHAR			2	//  and cursor moved horizontally                 (char cut)
#define CUT_MODE_HV_LINE		3	//  and cursor moved horizontally then vertically (line cut)
#define CUT_MODE_HV_BOX			4	//  and cursor moved horizontally then vertically (box cut)
#define CUT_MODE_V_LINE			5	//  and cursor moved vertically                   (line cut)
#define CUT_MODE_VH_CHAR		6	//  and cursor moved vertically then horizontally (char cut)
#define CUT_MODE_VH_BOX			7	//  and cursor moved vertically then horizontally (box cut)
#define IS_MARK_SET(cut_mode)	((cut_mode) != CUT_MODE_0_LINE)
	unsigned char buf_CUT_MODE:3;
#define TAB_SIZE_MIN		1
#define TAB_SIZE_1			1
#define TAB_SIZE_2			2
#define TAB_SIZE_3			3
#define TAB_SIZE_4			4
#define TAB_SIZE_5			5
#define TAB_SIZE_6			6
#define TAB_SIZE_7			7
#define TAB_SIZE_8			8
#define TAB_SIZE_MAX		TAB_SIZE_8
	unsigned char buf_TAB_SIZE:4;
#define EOL_NIX					0
#define EOL_MAC					1
#define EOL_DOS					2
#define EOL_MAX					EOL_DOS
	unsigned char buf_EOL:2;
#ifdef USE_NKF
#define ENCODE_ASCII			0
#define ENCODE_UTF8				1
#define ENCODE_EUCJP			2
#define ENCODE_SJIS				3
#define ENCODE_JIS				4
#define ENCODE_BINARY			5
#define ENCODE_MAX				ENCODE_BINARY
	unsigned char buf_ENCODE:3;
#endif // USE_NKF
} buf_state_t;

typedef enum /*_mode_idx_*/ {
///	APMD_RCFILE,
	APMD_DRAW_CURSOR,
	APMD_KEY_LINES,
	APMD_MAP_KEY_7F_BS,
#ifdef ENABLE_UTF8
	APMD_UTF8,
#endif // ENABLE_UTF8

	EDMD_EDITOR_PANES,
	EDMD_DUAL_SCROLL,
	EDMD_CONT_VIEW,
	EDMD_SHOW_RULER,
	EDMD_SHOW_LINE_NUMBER,
	EDMD_CURS_POSITIONING,
	EDMD_AUTO_INDENT,
	EDMD_BACKUP_FILES,
	EDMD_IGNORE_CASE,
#ifdef ENABLE_REGEX
	EDMD_USE_REGEXP,
#endif // ENABLE_REGEX
#ifdef ENABLE_SYNTAX
	EDMD_SYNTAX_HIGHLIGHT,
	EDMD_TAB_EOL_NOTATION,
#endif // ENABLE_SYNTAX

	FLMD_SHOW_DOT_FILE,
	FLMD_SHOW_FILE_INFO,
	FLMD_FILE_SORT_BY,
	FLMD_FILER_PANES,

	BUFST_VIEW_MODE,
	BUFST_TAB_SIZE,
} mode_idx_t;

extern app_mode_t app_mode__;

// Macros for set, clear, get and check the flags
#define SET_APPMD_PTR(appmode, param)				((appmode)->param = 1)
#define CLR_APPMD_PTR(appmode, param)				((appmode)->param = 0)
#define SET_APPMD_VAL_PTR(appmode, param, val)		((appmode)->param = val)
#define GET_APPMD_PTR(appmode, param)				((appmode)->param)
#define INC_APPMD_PTR(appmode, param, max)			\
 ((++((appmode)->param) <= max) ? ((appmode)->param) : ((appmode)->param = 0))
#define TOGGLE_APPMD_PTR(appmode, param)			((appmode)->param = ! (appmode)->param)
#define CMP_APPMD_PTR(appmode, param, val)			((appmode)->param == val)

#define SET_APPMD(param)				SET_APPMD_PTR(&app_mode__, param)
#define CLR_APPMD(param)				CLR_APPMD_PTR(&app_mode__, param)
#define SET_APPMD_VAL(param, val)		SET_APPMD_VAL_PTR(&app_mode__, param, val)
#define GET_APPMD(param)				GET_APPMD_PTR(&app_mode__, param)
#define INC_APPMD(param, max)			INC_APPMD_PTR(&app_mode__, param, max)
#define TOGGLE_APPMD(param)				TOGGLE_APPMD_PTR(&app_mode__, param)
#define CMP_APPMD(param, val)			CMP_APPMD_PTR(&app_mode__, param, val)

#define SHOW_MODE(item_name, get_str)	disp_status_bar_done("%s: %s", item_name, get_str)

void set_mode_idx_val(mode_idx_t mode_idx, int val);
const char *get_str_mode_idx_val(mode_idx_t mode_idx);

int tog_draw_cursor(void);
const char *get_str_draw_cursor(void);
int tog_auto_indent(void);
const char *get_str_auto_indent(void);
int tog_dual_scroll(void);
const char *get_str_dual_scroll(void);
int tog_ignore_case(void);
const char *get_str_ignore_case(void);
#ifdef ENABLE_REGEX
int tog_regexp(void);
const char *get_str_regexp(void);
#endif // ENABLE_REGEX
#ifdef ENABLE_UTF8
int tog_utf8(void);
const char *get_str_utf8(void);
#endif // ENABLE_UTF8
int inc_cursor_positioning(void);
const char *get_str_cursor_positioning(void);
int get_cursor_positioning(void);

#ifdef ENABLE_SYNTAX
int tog_syntax_hl(void);
const char *get_str_syntax_hl(void);
int tog_tab_eol_notation(void);
const char *get_str_tab_eol_notation(void);
#endif // ENABLE_SYNTAX

int tog_show_ruler(void);
const char *get_str_show_ruler(void);
int get_ruler_lines(void);
int tog_show_line_num(void);
const char *get_str_show_line_num(void);

int tog_map_key_7f_bs(void);
const char *get_str_map_key_7f_bs(void);

int inc_backup_files(void);
const char *get_str_backup_files(void);
int get_backup_files(void);

const char *get_editor_panes(void);

int tog_show_dot_file(void);
const char *get_str_show_dot_file(void);
int inc_show_file_info(void);
const char *get_str_show_file_info(void);
int clr_sort_by(void);
int inc_sort_by(void);
const char *get_str_sort_by(void);
int tog_filer_panes(void);
const char *get_str_filer_panes(void);
int inc_key_list_lines(void);
const char *get_str_key_list_lines(void);
int get_key_list_lines(void);

int doe_tog_panes(void);
int doe_tog_draw_cursor(void);
int doe_tog_auto_indent(void);
int doe_tog_dual_scroll(void);
int doe_tog_ignore_case(void);
int doe_tog_regexp(void);
int doe_tog_utf8(void);
int doe_inc_cursor_positioning(void);
int doe_tog_syntax_hl(void);
int doe_tog_tab_eol_notation(void);
int doe_tog_show_ruler(void);
int doe_tog_show_line_num(void);
int dof_tog_map_key_7f_bs(void);
int doe_inc_backup_files(void);
int do_tog_show_dot_file(void);
int do_inc_key_list_lines_(void);

int doe_examine_key_code(void);

#endif // appmode_h

// End of appmode.h
