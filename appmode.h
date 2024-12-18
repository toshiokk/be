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
	unsigned char app_MAP_KEY_7F_BS:1;		// bit 3
#define MAX_KEY_LINES			3
#define KEY_LINES_0				0
#define KEY_LINES_1				1
#define KEY_LINES_2				2
#define KEY_LINES_3				3
#define KEY_LINES_MAX			KEY_LINES_3
	unsigned char app_KEY_LINES:2;			// bit 4,5
#define DEBUG_NONE				0
#define DEBUG_PRINTF			1
	unsigned char app_DEBUG_PRINTF:1;		// bit 6
#define EF_EDITOR				0
#define EF_FILER				1
	unsigned char app_EDITOR_FILER:1;		// bit 7
											// editor------	filer-------
#define APP_MODE_NORMAL			0			// editable		file manager
#define APP_MODE_VIEWER			1			// viewer		----
#define APP_MODE_LIST			2			// list			select file/dir
	unsigned char app_LIST_MODE:2;			// bit 8,9

	// editor settings
	unsigned char ed_EDITOR_PANES:1;		// bit 10 (0: 1 pane / 1: 2 panes)
	unsigned char ed_EDITOR_PANEX:1;		// bit 11 (0: pane-0 / 1: pane-1)
	unsigned char ed_DUAL_SCROLL:1;			// bit 12
	unsigned char ed_SHOW_RULER:1;			// bit 13
	unsigned char ed_SHOW_LINE_NUMBER:1;	// bit 14
#define CURS_POSITIONING_NONE		0
#define CURS_POSITIONING_TOP		1
#define CURS_POSITIONING_CENTER		2
#define CURS_POSITIONING_BOTTOM		3
	unsigned char ed_CURS_POSITIONING:2;	// bit 15,16
#ifdef ENABLE_SYNTAX
	unsigned char ed_SYNTAX_HIGHLIGHT:1;	// bit 17
	unsigned char ed_TAB_EOL_NOTATION:1;	// bit 18
#endif // ENABLE_SYNTAX
	unsigned char ed_AUTO_INDENT:1;			// bit 19
	unsigned char ed_REVERSE_SEARCH:1;		// bit 20
	unsigned char ed_IGNORE_CASE:1;			// bit 21
#ifdef ENABLE_REGEX
	unsigned char ed_USE_REGEXP:1;			// bit 22
#endif // ENABLE_REGEX
#ifdef USE_NKF
	unsigned char ed_USE_NKF:1;				// bit 23
#endif // USE_NKF
#define BACKUP_FILES_0			0
#define BACKUP_FILES_1			1
#define BACKUP_FILES_2			2
#define BACKUP_FILES_3			3
#define BACKUP_FILES_10			10
#define BACKUP_FILES_15			15
#define BACKUP_FILES_MAX		BACKUP_FILES_15
	unsigned char ed_BACKUP_FILES:4;		// bit 24-27

	// filer settings
	unsigned char fl_FILER_PANES:1;			// bit 28 (0: 1 pane / 1: 2 panes)
	unsigned char fl_FILER_PANEX:1;			// bit 29 (0: pane-0 / 1: panes-1)
	unsigned char fl_SHOW_DOT_FILE:1;		// bit 30
#define FILE_SORT_BY_NAME		0
#define FILE_SORT_BY_EXT		1
#define FILE_SORT_BY_TIME		2
#define FILE_SORT_BY_SIZE		3
#define FILE_SORT_BY_NAME_REV	4
#define FILE_SORT_BY_EXT_REV	5
#define FILE_SORT_BY_TIME_REV	6
#define FILE_SORT_BY_SIZE_REV	7
#define FILE_SORT_BY_MAX		FILE_SORT_BY_SIZE_REV
	unsigned char fl_FILE_SORT_BY:3;		// bit 31-33
#define SHOW_FILE_INFO_0				0	// None
#define SHOW_FILE_INFO_1				1	// Size
#define SHOW_FILE_INFO_2				2	// Size Time
#define SHOW_FILE_INFO_3				3	// Size Time 1777 User
#define SHOW_FILE_INFO_4				4	// Size Time lrwxrwxrwx User:group
#define SHOW_FILE_INFO_MAX				SHOW_FILE_INFO_4
	unsigned char fl_SHOW_FILE_INFO:3;		// bit 34-36
} app_mode_t;

//-----------------------------------------------------------------------------

typedef struct /*buf_state*/ {
	unsigned char buf_MODE:2;				// bit 0,1
#define buf_MODE_EDIT	0
#define buf_MODE_VIEW	1
#define buf_MODE_LIST	2
#define buf_MODE_ANCH	3
	unsigned char buf_MODIFIED:1;			// bit 2
	unsigned char buf_LINE_WRAP_MODE:1;		// bit 3
#if 0 // 0
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
	unsigned char buf_CUT_MODE:3;			// bit 4-6
#define TAB_SIZE_MIN			1
#define TAB_SIZE_0				0	// DEFAULT_TAB_SIZE
#define TAB_SIZE_1				1
#define TAB_SIZE_2				2
#define TAB_SIZE_3				3
#define TAB_SIZE_4				4
#define TAB_SIZE_5				5
#define TAB_SIZE_6				6
#define TAB_SIZE_7				7
#define TAB_SIZE_8				8
#define TAB_SIZE_MAX			TAB_SIZE_8
	unsigned char buf_TAB_SIZE:4;			// bit 7-10
#define EOL_NIX					0
#define EOL_MAC					1
#define EOL_DOS					2
#define EOL_MAX					EOL_DOS
	unsigned char buf_EOL:2;				// bit 11-12
#define ENCODE_ASCII			0
#define ENCODE_UTF8				1
#ifdef USE_NKF
#define ENCODE_EUCJP			2
#define ENCODE_SJIS				3
#define ENCODE_JIS				4
#endif // USE_NKF
#define ENCODE_BINARY			5
#define ENCODE_MAX				ENCODE_BINARY
	unsigned char buf_ENCODE:3;				// bit 13--15
} buf_state_t;

typedef enum /*mode_idx*/ {
	APMD_DRAW_CURSOR,
	APMD_KEY_LINES,
	APMD_MAP_KEY_7F_BS,

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

#ifdef ENABLE_FILER
	FLMD_SHOW_DOT_FILE,
	FLMD_SHOW_FILE_INFO,
	FLMD_FILE_SORT_BY,
	FLMD_FILER_PANES,
#endif // ENABLE_FILER

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

#define SEARCH_DIR()	(GET_APPMD(ed_REVERSE_SEARCH) ? (BACKWARD_SEARCH) : (FORWARD_SEARCH))

#define SHOW_MODE(item_name, get_str)	disp_status_bar_done("%s: %s", item_name, get_str)

void set_mode_idx_val(mode_idx_t mode_idx, int val);
const char *get_str_mode_idx_val(mode_idx_t mode_idx);

int inc_app_mode(void);
const char *get_str_app_mode(void);
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

int tog_editor_panes();
const char *get_str_editor_panes(void);
int tog_editor_panex();
const char *get_str_editor_panex(void);

int tog_show_dot_file(void);
const char *get_str_show_dot_file(void);
int inc_show_file_info(void);
const char *get_str_show_file_info(void);
int clear_sort_by(void);
int inc_sort_by(void);
const char *get_str_sort_by(void);

#ifdef ENABLE_FILER
int tog_filer_panes(void);
const char *get_str_filer_panes(void);
int tog_filer_panex(void);
const char *get_str_filer_panex(void);
#endif // ENABLE_FILER

int inc_key_list_lines(void);
const char *get_str_key_list_lines(void);
int get_key_list_lines(void);

// View mode and List mode of editor:
// |editor mode    |load|edit|save|cursor    |select|search|editing |purpose       |
// |editor mode    |load|edit|save| move     | /cut |      |possible|              |
// |editor mode    |load|edit|save|          | /copy|      |        |              |
// |---------------|----|----|----|----------|------|------|--------|--------------|
// |APP_MODE_NORMAL|yes |yes |yes |everywhere|yes   |yes   |all     |text editor   |
// |APP_MODE_VIEWER|no  |no  |no  |everywhere|yes   |yes   |all     |file list/help|
// |APP_MODE_LIST  |auto|no  |auto|vertical  |no    |yes?? |none    |history viewer|

// View mode of filer:
// |filer mode     |open/copy/move/ren|chdir|search|select|modificatons|purpose          |
// |filer mode     |mkdir/exec        |     |      |by tap|of directory|                 |
// |---------------|------------------|-----|------|------|------------|-----------------|
// |APP_MODE_NORMAL|yes               |yes  |yes   |no    |yes         |file manager     |
// |APP_MODE_VIEWER|--                |--   |--    |--    |--          |--               |
// |APP_MODE_LIST  |no                |yes  |yes   |yes   |no          |file/dir selector|

BOOL is_app_normal_mode(void);
BOOL is_app_view_mode(void);
BOOL is_app_list_mode(void);
BOOL is_app_view_list_mode(void);

int doe_inc_app_mode(void);
int doe_tog_panes(void);
int doe_tog_panex(void);
int doe_tog_draw_cursor(void);
int doe_tog_auto_indent(void);
int doe_tog_dual_scroll(void);
int doe_tog_ignore_case(void);
int doe_tog_regexp(void);
int doe_inc_cursor_positioning(void);
int doe_tog_syntax_hl(void);
int doe_tog_tab_eol_notation(void);
int doe_tog_show_ruler(void);
int doe_tog_show_line_num(void);
int doe_tog_map_key_7f_bs(void);
int doe_inc_backup_files(void);
int do_tog_show_dot_file(void);
int do_inc_key_list_lines_(void);

inline char indication_of_app_mode()
{
	char separator_char = ':';
	if (is_app_list_mode()) {
		separator_char = '.';
	} else if (is_app_view_mode()) {
		separator_char = '!';
	}
#ifdef ENABLE_DEBUG
	if (GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF) {
		separator_char = ';';
		if (is_app_list_mode()) {
			separator_char = ',';
		} else if (is_app_view_mode()) {
			separator_char = '?';
		}
	}
#endif // ENABLE_DEBUG
	return separator_char;
}

int set_editor_cur_pane_idx(int pane_idx);
int get_editor_cur_pane_idx(void);
int get_editor_counter_pane_idx(void);

int set_filer_cur_pane_idx(int pane_idx);
int get_filer_cur_pane_idx();
int get_filer_counter_pane_idx();

#endif // appmode_h

// End of appmode.h
