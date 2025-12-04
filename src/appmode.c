/**************************************************************************
 *   appmode.c                                                            *
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
* @file		appmode.c
* @brief	Application mode management
* @author	Creator
* @date		Creation date
* @details	Detailed description
*/

#include "headers.h"

app_mode_t app_mode__;

void init_app_mode()
{
	// setup application settings
	memset(&app_mode__, 0x00, sizeof(app_mode__));

#ifdef ENABLE_DEBUG
	SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_PRINTF);
	set_debug_printf_output(GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF);
#endif // ENABLE_DEBUG

	// editor and filer
#ifdef ENABLE_RC
	SET_APPMD(app_RCFILE);
#endif // ENABLE_RC
	CLR_APPMD(app_DRAW_CURSOR);
#ifdef ENABLE_HIGH_BGC
	CLR_APPMD(app_HIGH_BGC);
#endif // ENABLE_HIGH_BGC
	SET_APPMD_VAL(app_KEY_LINES, 3);
	// editor mode
	SET_APPMD_VAL(app_EDITOR_FILER, EF_EDITOR);
	SET_APPMD_VAL(app_LIST_MODE, APP_MODE_NORMAL);
////	CLR_APPMD(app_MAP_KEY_7F_BS);
	SET_APPMD(app_MAP_KEY_7F_BS);

	CLR_APPMD(ed_EDITOR_PANES);
	CLR_APPMD(ed_EDITOR_PANEX);
	CLR_APPMD(ed_DUAL_SCROLL);
	SET_APPMD(ed_SHOW_RULER);
	SET_APPMD(ed_SHOW_LINE_NUMBER);
	CLR_APPMD(ed_CURS_POSITIONING);
#ifdef USE_NKF
	SET_APPMD(ed_USE_NKF);
#endif // USE_NKF
	SET_APPMD_VAL(ed_BACKUP_FILES, BACKUP_FILES_MAX);
#ifdef ENABLE_REGEX
	SET_APPMD(ed_USE_REGEXP);
#ifdef ENABLE_SYNTAX
	SET_APPMD(ed_SYNTAX_HIGHLIGHT);
	SET_APPMD(ed_TAB_EOL_NOTATION);
#endif // ENABLE_SYNTAX
#endif // ENABLE_REGEX

	// filer mode
	CLR_APPMD(fl_FILER_PANES);
	CLR_APPMD(fl_FILER_PANEX);
	SET_APPMD_VAL(fl_SHOW_FILE_INFO, SHOW_FILE_INFO_2);
	SET_APPMD_VAL(fl_SHOW_ZEBRA_STRIPE, SHOW_ZEBRA_STRIPING_OFF);
	SET_APPMD_VAL(fl_FILE_SORT_BY, 0);
}

/**
* @fn			void set_mode_idx_val(int mode_idx, int val)
* @brief		change application state
* @param[in]	mode_idx index to select application state variable
* @param[in]	val application state value to be set to the state
* @return		void
* @details		change specified application state variable to the specified value
*/
void set_mode_idx_val(mode_idx_t mode_idx, int val)
{
	switch (mode_idx) {
	default:
	case APMD_DRAW_CURSOR:
		SET_APPMD_VAL(app_DRAW_CURSOR, val);
		break;
	case APMD_KEY_LINES:
		SET_APPMD_VAL(app_KEY_LINES, val);
		break;
	case APMD_MAP_KEY_7F_BS:
		SET_APPMD_VAL(app_MAP_KEY_7F_BS, val);
		break;

	case EDMD_EDITOR_PANES:
	case EDMD_CONT_VIEW:
	case EDMD_DUAL_SCROLL:
		SET_APPMD_VAL(ed_DUAL_SCROLL, val);
		break;
	case EDMD_SHOW_RULER:
		SET_APPMD_VAL(ed_SHOW_RULER, val);
		break;
	case EDMD_SHOW_LINE_NUMBER:
		SET_APPMD_VAL(ed_SHOW_LINE_NUMBER, val);
		break;
	case EDMD_CURS_POSITIONING:
		SET_APPMD_VAL(ed_CURS_POSITIONING, val);
		break;
	case EDMD_BACKUP_FILES:
		SET_APPMD_VAL(ed_BACKUP_FILES, val);
		break;
	case EDMD_IGNORE_CASE:
		SET_APPMD_VAL(ed_IGNORE_CASE, val);
		break;
#ifdef ENABLE_REGEX
	case EDMD_USE_REGEXP:
		SET_APPMD_VAL(ed_USE_REGEXP, val);
		break;
#endif // ENABLE_SYNTAX
#ifdef ENABLE_SYNTAX
	case EDMD_SYNTAX_HIGHLIGHT:
		SET_APPMD_VAL(ed_SYNTAX_HIGHLIGHT, val);
		break;
	case EDMD_TAB_EOL_NOTATION:
		SET_APPMD_VAL(ed_TAB_EOL_NOTATION, val);
		break;
#endif // ENABLE_REGEX

#ifdef ENABLE_FILER
	case FLMD_SHOW_DOT_FILE:
		SET_APPMD_VAL(fl_SHOW_DOT_FILE, val);
		break;
	case FLMD_SHOW_FILE_INFO:
		SET_APPMD_VAL(fl_SHOW_FILE_INFO, val);
		break;
	case FLMD_FILE_SORT_BY:
		SET_APPMD_VAL(fl_FILE_SORT_BY, val);
		break;
	case FLMD_SHOW_ZEBRA_STRIPING:
		SET_APPMD_VAL(fl_SHOW_ZEBRA_STRIPE, val);
		break;
	case FLMD_FILER_PANES:
		SET_APPMD_VAL(fl_FILER_PANES, val);
		break;
#endif // ENABLE_FILER
	}
}
const char *get_str_mode_idx_val(mode_idx_t mode_idx)
{
	switch (mode_idx) {
	default:
	case APMD_DRAW_CURSOR:
		return get_str_draw_cursor();
	case APMD_KEY_LINES:
		return get_str_key_list_lines();
	case APMD_MAP_KEY_7F_BS:
		return get_str_map_key_7f_bs();

	case EDMD_EDITOR_PANES:
	case EDMD_CONT_VIEW:
	case EDMD_DUAL_SCROLL:
		return get_str_dual_scroll();
	case EDMD_SHOW_RULER:
		return get_str_show_ruler();
	case EDMD_SHOW_LINE_NUMBER:
		return get_str_show_line_num();
	case EDMD_CURS_POSITIONING:
		return get_str_cursor_positioning();
	case EDMD_BACKUP_FILES:
		return get_str_backup_files();
	case EDMD_IGNORE_CASE:
		return get_str_ignore_case();
#ifdef ENABLE_REGEX
	case EDMD_USE_REGEXP:
		return get_str_regexp();
#endif // ENABLE_REGEX
#ifdef ENABLE_SYNTAX
	case EDMD_SYNTAX_HIGHLIGHT:
		return get_str_syntax_hl();
	case EDMD_TAB_EOL_NOTATION:
		return get_str_tab_eol_notation();
#endif // ENABLE_SYNTAX

#ifdef ENABLE_FILER
	case FLMD_SHOW_DOT_FILE:
		return get_str_show_dot_file();
	case FLMD_SHOW_FILE_INFO:
		return get_str_file_view_mode();
	case FLMD_FILE_SORT_BY:
		return get_str_file_sort_mode();
	case FLMD_SHOW_ZEBRA_STRIPING:
		return get_str_show_zebra_striping();
	case FLMD_FILER_PANES:
		return get_str_filer_panes();
#endif // ENABLE_FILER
	}
	return "!!";
}

//------------------------------------------------------------------------------

int inc_app_mode()
{
	INC_APPMD(app_LIST_MODE, APP_MODE_CHOOSER);
	return 0;
}
const char *get_str_app_mode()
{
	switch (GET_APPMD(app_LIST_MODE)) {
	default:
	case APP_MODE_NORMAL:	return "[NORMAL]";
	case APP_MODE_CHOOSER:	return "[CHOOSER]";
	case APP_MODE_VIEWER:	return "[VIEWER]";
	}
}

int tog_draw_cursor()
{
	return TOGGLE_APPMD(app_DRAW_CURSOR);
}
const char *get_str_draw_cursor()
{
	return BOOL_TO_ON_OFF(GET_APPMD(app_DRAW_CURSOR));
}


int tog_dual_scroll()
{
	return TOGGLE_APPMD(ed_DUAL_SCROLL);
}
const char *get_str_dual_scroll()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_DUAL_SCROLL));
}

int tog_ignore_case()
{
	return TOGGLE_APPMD(ed_IGNORE_CASE);
}
const char *get_str_ignore_case()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_IGNORE_CASE));
}

#ifdef ENABLE_REGEX
int tog_regexp()
{
	return TOGGLE_APPMD(ed_USE_REGEXP);
}
const char *get_str_regexp()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_USE_REGEXP));
}
#endif // ENABLE_REGEX

int inc_cursor_positioning()
{
	INC_APPMD(ed_CURS_POSITIONING, CURS_POSITIONING_BOTTOM);
	return 0;
}
const char *get_str_cursor_positioning()
{
	static char buf[10+1];

	snprintf(buf, 10+1, "%d", get_cursor_positioning());
	return buf;
}
int get_cursor_positioning()
{
	return GET_APPMD(ed_CURS_POSITIONING);
}

#ifdef ENABLE_SYNTAX
int tog_syntax_hl()
{
	return TOGGLE_APPMD(ed_SYNTAX_HIGHLIGHT);
}
const char *get_str_syntax_hl()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_SYNTAX_HIGHLIGHT));
}

int tog_tab_eol_notation()
{
	return TOGGLE_APPMD(ed_TAB_EOL_NOTATION);
}
const char *get_str_tab_eol_notation()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_TAB_EOL_NOTATION));
}
#endif // ENABLE_SYNTAX

int tog_show_ruler()
{
	return TOGGLE_APPMD(ed_SHOW_RULER);
}
const char *get_str_show_ruler()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_SHOW_RULER));
}
int get_ruler_lines()
{
	return GET_APPMD(ed_SHOW_RULER);
}

int tog_show_line_num()
{
	return TOGGLE_APPMD(ed_SHOW_LINE_NUMBER);
}
const char *get_str_show_line_num()
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_SHOW_LINE_NUMBER));
}

const char *get_str_map_key_7f_bs()
{
	return BOOL_TO_ON_OFF(GET_APPMD(app_MAP_KEY_7F_BS));
}

int inc_backup_files()
{
	INC_APPMD(ed_BACKUP_FILES, BACKUP_FILES_MAX);
	return 0;
}
const char *get_str_backup_files()
{
	static char buf[10+1];

	snprintf(buf, 10+1, "%d", get_backup_files());
	return buf;
}
int get_backup_files()
{
	return GET_APPMD(ed_BACKUP_FILES);
}
int tog_editor_panes()
{
	return TOGGLE_APPMD(ed_EDITOR_PANES);
}
const char *get_str_editor_panes()
{
	return BOOL_TO_1_2(GET_APPMD(ed_EDITOR_PANES));
}
int tog_editor_panex()
{
	return set_editor_cur_pane_idx(get_editor_another_pane_idx());
}
const char *get_str_editor_pane_num()
{
	return BOOL_TO_1_2(get_editor_cur_pane_idx());
}

int tog_show_dot_file()
{
	return TOGGLE_APPMD(fl_SHOW_DOT_FILE);
}
const char *get_str_show_dot_file()
{
	return BOOL_TO_ON_OFF(GET_APPMD(fl_SHOW_DOT_FILE));
}

int inc_file_view_mode()
{
	INC_APPMD(fl_SHOW_FILE_INFO, SHOW_FILE_INFO_MAX);
	return 0;
}
const char *get_str_file_view_mode()
{
	switch (GET_APPMD(fl_SHOW_FILE_INFO)) {
	case SHOW_FILE_INFO_0:	return "None";			// ""
	case SHOW_FILE_INFO_1:	return "Siz";			// " 4096"
	case SHOW_FILE_INFO_2:	return "Siz+Tim";		// " 4096 240628-124240"
	case SHOW_FILE_INFO_3:	return "SzTmT7777Us";	// " 4096 240628-124240 d0755 user"
	case SHOW_FILE_INFO_4:	return "SzTmRwxUG";		// " 4096 240628-124240  rwxr--r-- user user"
	default:	return "";
	}
}

int clear_file_sort_mode()
{
	CLR_APPMD(fl_FILE_SORT_BY);
	return 0;
}
int inc_file_sort_mode()
{
	INC_APPMD(fl_FILE_SORT_BY, FILE_SORT_BY_MAX);
	return 0;
}
const char *get_str_file_sort_mode()
{
	switch (GET_APPMD(fl_FILE_SORT_BY)) {
	default:
	case FILE_SORT_BY_NAME:
		return "name";
	case FILE_SORT_BY_EXT:
		return "ext";
	case FILE_SORT_BY_TIME:
		return "time";
	case FILE_SORT_BY_SIZE:
		return "size";
	case FILE_SORT_BY_NAME_REV:
		return "NAME(reverse)";
	case FILE_SORT_BY_EXT_REV:
		return "EXT(reverse)";
	case FILE_SORT_BY_TIME_REV:
		return "TIME(reverse)";
	case FILE_SORT_BY_SIZE_REV:
		return "SIZE(reverse)";
	}
}

int tog_show_zebra_striping()
{
	return TOGGLE_APPMD(fl_SHOW_ZEBRA_STRIPE);
}
const char *get_str_show_zebra_striping()
{
	return BOOL_TO_ON_OFF(GET_APPMD(fl_SHOW_ZEBRA_STRIPE));
}

#ifdef ENABLE_FILER
int tog_filer_panes()
{
	return TOGGLE_APPMD(fl_FILER_PANES);
}
const char *get_str_filer_panes()
{
	return BOOL_TO_1_2(GET_APPMD(fl_FILER_PANES));
}
int tog_filer_panex()
{
	return set_filer_cur_pane_idx(get_filer_another_pane_idx());
}
const char *get_str_filer_pane_num()
{
	return BOOL_TO_1_2(get_filer_cur_pane_idx());
}
#endif // ENABLE_FILER

int inc_key_list_lines()
{
	INC_APPMD(app_KEY_LINES, KEY_LINES_MAX);
	return 0;
}
const char *get_str_key_list_lines()
{
	static char buf[1+1];

	snprintf(buf, 1+1, "%u", GET_APPMD(app_KEY_LINES));
	return buf;
}
int get_key_list_lines()
{
	return GET_APPMD(app_KEY_LINES);
}

const char *get_str_setting_none()
{
	return "--";
}

//------------------------------------------------------------------------------

BOOL is_app_normal_mode()	// in editor: text editor mode, in filer: file manager mode
{
	return GET_APPMD(app_LIST_MODE) == APP_MODE_NORMAL;
}
BOOL is_app_chooser_mode()		// in editor: text list mode, in filer: file list mode
{
	return GET_APPMD(app_LIST_MODE) == APP_MODE_CHOOSER;
}
BOOL is_app_viewer_mode()		// in editor: text viewer, in filer: directory viewer
{
	return GET_APPMD(app_LIST_MODE) == APP_MODE_VIEWER;
}
BOOL is_app_chooser_viewer_mode()
{
	return is_app_chooser_mode() || is_app_viewer_mode();
}

//=============================================================================

void doe_inc_app_mode()
{
	inc_app_mode();
	SHOW_MODE("App mode", get_str_app_mode());
}

void doe_tog_panes()
{
	tog_editor_panes();
	SHOW_MODE("Editor panes", get_str_editor_panes());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
void doe_tog_panex()
{
	tog_editor_panex();
	SHOW_MODE("Editor pane index", get_str_editor_pane_num());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}

//------------------------------------------------------------------------------

void doe_tog_draw_cursor()
{
	tog_draw_cursor();
	SHOW_MODE("Draw cursor", get_str_draw_cursor());
}
void doe_tog_dual_scroll()
{
	tog_dual_scroll();
	SHOW_MODE("Dual scroll", get_str_dual_scroll());
}
void doe_tog_ignore_case()
{
	tog_ignore_case();
	SHOW_MODE("Ignore case", get_str_ignore_case());
}
#ifdef ENABLE_REGEX
void doe_tog_regexp()
{
	tog_regexp();
	SHOW_MODE("RegExp", get_str_regexp());
}
#endif // ENABLE_REGEX

void doe_inc_cursor_positioning()
{
	inc_cursor_positioning();
	SHOW_MODE("Cursor center mode", get_str_cursor_positioning());
}

#ifdef ENABLE_SYNTAX
void doe_tog_syntax_hl()
{
	tog_syntax_hl();
	SHOW_MODE("Syntax Highlighting", get_str_syntax_hl());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
void doe_tog_tab_eol_notation()
{
	tog_tab_eol_notation();
	SHOW_MODE("Visible TAB/EOL", get_str_tab_eol_notation());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
#endif // ENABLE_SYNTAX

void doe_tog_show_ruler()
{
	tog_show_ruler();
	SHOW_MODE("Show ruler", get_str_show_ruler());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
void doe_tog_show_line_num()
{
	tog_show_line_num();
	SHOW_MODE("Show line number", get_str_show_line_num());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}

void doe_inc_backup_files()
{
	inc_backup_files();
	SHOW_MODE("Backup file(s)", get_str_backup_files());
}
int _dof_tog_show_dot_file()
{
	tog_show_dot_file();
	SHOW_MODE("Show dot file", get_str_show_dot_file());
	return 0;
}
int _dof_tog_show_zebra_striping()
{
	tog_show_zebra_striping();
	SHOW_MODE("Show zebra striping", get_str_show_zebra_striping());
	return 0;
}
int _dof_inc_key_list_lines()
{
	inc_key_list_lines();
	SHOW_MODE("Display key list lines", get_str_key_list_lines());
	win_reinit_win_size();
	return 0;
}
//------------------------------------------------------------------------------
int set_editor_cur_pane_idx(int pane_idx)
{
	return SET_APPMD_VAL(ed_EDITOR_PANEX, pane_idx);
}
int get_editor_cur_pane_idx()
{
	return GET_APPMD(ed_EDITOR_PANEX);
}
int get_editor_another_pane_idx()
{
	return BOOL_INV(GET_APPMD(ed_EDITOR_PANEX));
}

int set_filer_cur_pane_idx(int pane_idx)
{
	return SET_APPMD_VAL(fl_FILER_PANEX, pane_idx);
}
int get_filer_cur_pane_idx()
{
	return GET_APPMD(fl_FILER_PANEX);
}
int get_filer_another_pane_idx()
{
	return BOOL_INV(GET_APPMD(fl_FILER_PANEX));
}

// End of appmode.c
