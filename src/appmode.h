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
#ifdef ENABLE_DEBUG
#define DEBUG_NONE				0
#define DEBUG_PRINTF			1
	UINT8 app_DEBUG_PRINTF:1;		// bit 0
#endif // ENABLE_DEBUG
#ifdef ENABLE_RC
	UINT8 app_RCFILE:1;				// bit 1
#endif // ENABLE_RC
	UINT8 app_DRAW_CURSOR:1;		// bit 2
#ifdef ENABLE_HIGH_BGC
	UINT8 app_HIGH_BGC:1;			// bit 3
#endif // ENABLE_HIGH_BGC
	UINT8 app_MAP_KEY_7F_BS:1;		// bit 4
#define MAX_KEY_LINES			3
#define KEY_LINES_0				0
#define KEY_LINES_1				1
#define KEY_LINES_2				2
#define KEY_LINES_3				3
#define KEY_LINES_MAX			KEY_LINES_3
	UINT8 app_KEY_LINES:2;			// bit 5,6
#define EF_EDITOR				0
#define EF_FILER				1
	UINT8 app_EDITOR_FILER:1;		// bit 7
											// editor------		filer-------
#define APP_MODE_NORMAL			0			// text editor		file manager
#define APP_MODE_VIEWER			1			// text viewer		----
	UINT8 app_LIST_MODE:2;			// bit 8,9
	UINT8 app_SPLASH:1;				// bit 10

	// editor settings
	UINT8 ed_EDITOR_PANES:1;		// bit 11 (0: 1 pane / 1: 2 panes)
	UINT8 ed_EDITOR_PANEX:1;		// bit 12 (0: pane-0 / 1: pane-1)
	UINT8 ed_DUAL_SCROLL:1;			// bit 13
	UINT8 ed_SHOW_RULER:1;			// bit 14
	UINT8 ed_SHOW_LINE_NUMBER:1;	// bit 15
#define CURS_POSITIONING_NONE		0
#define CURS_POSITIONING_TOP		1
#define CURS_POSITIONING_CENTER		2
#define CURS_POSITIONING_BOTTOM		3
	UINT8 ed_CURS_POSITIONING:2;	// bit 16,17
#ifdef ENABLE_SYNTAX
	UINT8 ed_SYNTAX_HIGHLIGHT:1;	// bit 18
	UINT8 ed_TAB_EOL_NOTATION:1;	// bit 19
#endif // ENABLE_SYNTAX
	UINT8 ed_REVERSE_SEARCH:1;		// bit 20
	UINT8 ed_IGNORE_CASE:1;			// bit 21
#ifdef ENABLE_REGEX
	UINT8 ed_USE_REGEXP:1;			// bit 22
#endif // ENABLE_REGEX
#ifdef USE_NKF
	UINT8 ed_USE_NKF:1;				// bit 23
#endif // USE_NKF
#define BACKUP_FILES_0			0
#define BACKUP_FILES_1			1
#define BACKUP_FILES_2			2
#define BACKUP_FILES_3			3
#define BACKUP_FILES_10			10
#define BACKUP_FILES_15			15
#define BACKUP_FILES_MAX		BACKUP_FILES_15
	UINT8 ed_BACKUP_FILES:4;		// bit 24-27

	// filer settings
	UINT8 fl_FILER_PANES:1;			// bit 28 (0: 1 pane / 1: 2 panes)
	UINT8 fl_FILER_PANEX:1;			// bit 29 (0: pane-0 / 1: panes-1)
	UINT8 fl_SHOW_DOT_FILE:1;		// bit 30 (1: show hidden files)
#define FILE_SORT_BY_NAME		0
#define FILE_SORT_BY_EXT		1
#define FILE_SORT_BY_TIME		2
#define FILE_SORT_BY_SIZE		3
#define FILE_SORT_BY_NAME_REV	4
#define FILE_SORT_BY_EXT_REV	5
#define FILE_SORT_BY_TIME_REV	6
#define FILE_SORT_BY_SIZE_REV	7
#define FILE_SORT_BY_MAX		FILE_SORT_BY_SIZE_REV
	UINT8 fl_FILE_SORT_BY:3;		// bit 31-33
#define SHOW_FILE_INFO_0				0	// None
#define SHOW_FILE_INFO_1				1	// Size
#define SHOW_FILE_INFO_2				2	// Size Time
#define SHOW_FILE_INFO_3				3	// Size Time 1777 User
#define SHOW_FILE_INFO_4				4	// Size Time lrwxrwxrwx User:group
#define SHOW_FILE_INFO_MAX				SHOW_FILE_INFO_4
	UINT8 fl_SHOW_FILE_INFO:3;		// bit 34-36
#define SHOW_ZEBRA_STRIPING_OFF			0	// off
#define SHOW_ZEBRA_STRIPING_ON			1	// ON
	UINT8 fl_SHOW_ZEBRA_STRIPE:1;	// bit 37
} app_mode_t;

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
	FLMD_SHOW_ZEBRA_STRIPING,
	FLMD_FILER_PANES,
#endif // ENABLE_FILER
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

void init_app_mode();

void set_mode_idx_val(mode_idx_t mode_idx, int val);
const char *get_str_mode_idx_val(mode_idx_t mode_idx);

int inc_app_mode();
const char *get_str_app_mode();
int tog_draw_cursor();
const char *get_str_draw_cursor();
int tog_dual_scroll();
const char *get_str_dual_scroll();
int tog_ignore_case();
const char *get_str_ignore_case();
#ifdef ENABLE_REGEX
int tog_regexp();
const char *get_str_regexp();
#endif // ENABLE_REGEX
int inc_cursor_positioning();
const char *get_str_curs_pos_mode();
int get_cursor_positioning();

#ifdef ENABLE_SYNTAX
int tog_syntax_hl();
const char *get_str_syntax_hl();
int tog_tab_eol_notation();
const char *get_str_tab_eol_notat();
#endif // ENABLE_SYNTAX

int tog_show_ruler();
const char *get_str_show_ruler();
int get_ruler_lines();
int tog_show_line_num();
const char *get_str_show_line_num();

int inc_backup_files();
const char *get_str_backup_files();
int get_backup_files();

int tog_editor_panes();
const char *get_str_editor_panes();
int tog_editor_panex();
const char *get_str_editor_panex();

int tog_show_dot_file();
const char *get_str_show_dot_file();
int inc_file_view_mode();
const char *get_str_file_view_mode();
int clear_file_sort_mode();
int inc_file_sort_mode();
const char *get_str_file_sort_mode();
int tog_zebra_striping();
const char *get_str_zebra_striping();

#ifdef ENABLE_FILER
int tog_filer_panes();
const char *get_str_filer_panes();
int tog_filer_panex();
const char *get_str_filer_panex();
#endif // ENABLE_FILER

int inc_key_list_lines();
const char *get_str_key_list_lines();
int get_key_list_lines();

int tog_map_key_7f_bs();
const char *get_str_map_key_7f_bs();

const char *get_str_none();

// mode of editor depends mainly on the buffer-mode:
// |buffer state |app-mode       |
// |-------------|---------------|
// |normal       |APP_MODE_NORMAL|
// |RO           |APP_MODE_VIEWER|
// |locked       |APP_MODE_VIEWER|
// |WP           |APP_MODE_VIEWER|
// |VIEW         |APP_MODE_VIEWER|
// |LIST         |APP_MODE_VIEWER|
// |anchor       |APP_MODE_VIEWER|
// |empty        |APP_MODE_VIEWER|

// |editor mode    |load|save|cursor   |can   |mark  |choose|purpose     |
// |               |    |    | move    |modify| /cut |by tap|            |
// |               |    |    |         |      | /copy|      |            |
// |---------------|----|----|---------|------|------|------|------------|
// |APP_MODE_NORMAL|yes |yes |horz/vert|yes   |yes   |no    |text editor |
// |APP_MODE_VIEWER|auto|auto|vertical |no    |yes   |yes   |text viewer |

// mode of filer depends only on the application-mode:
// |filer mode     |open/copy/move/ren|chdir|search|choose|purpose         |
// |               |/remove/mkdir/exec|     |      |by tap|                |
// |---------------|------------------|-----|------|------|----------------|
// |APP_MODE_NORMAL|yes               |yes  |yes   |no    |file manager    |
// |APP_MODE_VIEWER|no                |yes  |yes   |yes   |file/dir chooser|

BOOL is_app_normal_mode();
BOOL is_app_viewer_mode();
void set_editor_app_mode_based_on_buf_mode();

void doe_inc_app_mode();
void doe_tog_panes();
void doe_tog_panex();
void doe_tog_draw_cursor();
void doe_tog_dual_scroll();
void doe_tog_ignore_case();
void doe_tog_regexp();
void doe_inc_curs_pos_mode();
void doe_tog_syntax_hl();
void doe_tog_tab_eol_notat();
void doe_tog_show_ruler();
void doe_tog_show_line_num();
void doe_inc_backup_files();
int _dof_tog_show_dot_file();
int _dof_tog_zebra_striping();
int _dof_inc_key_list_lines();
void doe_tog_map_key_7f_bs();

inline char indication_of_app_mode()
{
	char separator_char = ':';
	if (is_app_viewer_mode()) {
		separator_char = '.';
	}
#ifdef ENABLE_DEBUG
	if (GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF) {
		separator_char = ';';
		if (is_app_viewer_mode()) {
			separator_char = ',';
		}
	}
#endif // ENABLE_DEBUG
	return separator_char;
}

int set_editor_cur_pane_idx(int pane_idx);
int get_editor_cur_pane_idx();
int get_editor_another_pane_idx();

int set_filer_cur_pane_idx(int pane_idx);
int get_filer_cur_pane_idx();
int get_filer_another_pane_idx();

#endif // appmode_h

// End of appmode.h
