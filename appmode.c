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
* @file appmode.c
* @brief Application mode management
* @author 作成者
* @date 作成日
* @details 詳細な説明
*/

#include "headers.h"

app_mode_t app_mode__;

/**
* @fn void set_mode_idx_val(int mode_idx, int val)
* @brief 各属性に値を設定する
* @param[in] mode_idx(引数名) 引数の説明
* @param[in] val(引数名) 引数の説明
* @return bool 戻り値の説明
* @details 詳細な説明
*/
void set_mode_idx_val(mode_idx_t mode_idx, int val)
{
	switch (mode_idx) {
	case APMD_DRAW_CURSOR:
		SET_APPMD_VAL(app_DRAW_CURSOR, val);
		break;
	case APMD_KEY_LINES:
		SET_APPMD_VAL(app_KEY_LINES, val);
		break;
	case APMD_MAP_KEY_7F_BS:
		SET_APPMD_VAL(app_MAP_KEY_7F_BS, val);
		break;
#ifdef ENABLE_UTF8
	case APMD_UTF8:
		SET_APPMD_VAL(app_UTF8, val);
		break;
#endif // ENABLE_UTF8

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
	case EDMD_AUTO_INDENT:
		SET_APPMD_VAL(ed_AUTO_INDENT, val);
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

	case FLMD_SHOW_DOT_FILE:
		SET_APPMD_VAL(fl_SHOW_DOT_FILE, val);
		break;
	case FLMD_SHOW_FILE_INFO:
		SET_APPMD_VAL(fl_SHOW_FILE_INFO, val);
		break;
	case FLMD_FILE_SORT_BY:
		SET_APPMD_VAL(fl_FILE_SORT_BY, val);
		break;
	case FLMD_FILER_PANES:
		SET_APPMD_VAL(fl_FILER_PANES, val);
		break;

	case BUFST_VIEW_MODE:
	case BUFST_TAB_SIZE:
		break;
	}
}
const char *get_str_mode_idx_val(mode_idx_t mode_idx)
{
	switch (mode_idx) {
	case APMD_DRAW_CURSOR:
		return get_str_draw_cursor();
	case APMD_KEY_LINES:
		return get_str_key_list_lines();
	case APMD_MAP_KEY_7F_BS:
		return get_str_map_key_7f_bs();
#ifdef ENABLE_UTF8
	case APMD_UTF8:
		return get_str_utf8();
#endif // ENABLE_UTF8

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
	case EDMD_AUTO_INDENT:
		return get_str_auto_indent();
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

	case FLMD_SHOW_DOT_FILE:
		return get_str_show_dot_file();
	case FLMD_SHOW_FILE_INFO:
		return get_str_show_file_info();
	case FLMD_FILE_SORT_BY:
		return get_str_sort_by();
	case FLMD_FILER_PANES:
		return get_str_filer_panes();

	case BUFST_VIEW_MODE:
	case BUFST_TAB_SIZE:
		break;
	}
	return "!!";
}

//-----------------------------------------------------------------------------

int tog_draw_cursor(void)
{
	TOGGLE_APPMD(app_DRAW_CURSOR);
	return 0;
}
const char *get_str_draw_cursor(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(app_DRAW_CURSOR));
}

int tog_auto_indent(void)
{
	TOGGLE_APPMD(ed_AUTO_INDENT);
	return 0;
}
const char *get_str_auto_indent(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_AUTO_INDENT));
}

int tog_dual_scroll(void)
{
	TOGGLE_APPMD(ed_DUAL_SCROLL);
	return 0;
}
const char *get_str_dual_scroll(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_DUAL_SCROLL));
}

int tog_ignore_case(void)
{
	TOGGLE_APPMD(ed_IGNORE_CASE);
	return 0;
}
const char *get_str_ignore_case(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_IGNORE_CASE));
}

#ifdef ENABLE_REGEX
int tog_regexp(void)
{
	TOGGLE_APPMD(ed_USE_REGEXP);
	return 0;
}
const char *get_str_regexp(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_USE_REGEXP));
}
#endif // ENABLE_REGEX

#ifdef ENABLE_UTF8
int tog_utf8(void)
{
	TOGGLE_APPMD(app_UTF8);
	return 0;
}
const char *get_str_utf8(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(app_UTF8));
}
#endif // ENABLE_UTF8

int inc_cursor_positioning(void)
{
	INC_APPMD(ed_CURS_POSITIONING, CURS_POSITIONING_BOTTOM);
	return 0;
}
const char *get_str_cursor_positioning(void)
{
	static char buf[10+1];

	snprintf(buf, 10+1, "%d", get_cursor_positioning());
	return buf;
}
int get_cursor_positioning(void)
{
	return GET_APPMD(ed_CURS_POSITIONING);
}

#ifdef ENABLE_SYNTAX
int tog_syntax_hl(void)
{
	TOGGLE_APPMD(ed_SYNTAX_HIGHLIGHT);
	return 0;
}
const char *get_str_syntax_hl(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_SYNTAX_HIGHLIGHT));
}

int tog_tab_eol_notation(void)
{
	TOGGLE_APPMD(ed_TAB_EOL_NOTATION);
	return 0;
}
const char *get_str_tab_eol_notation(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_TAB_EOL_NOTATION));
}
#endif // ENABLE_SYNTAX

int tog_show_ruler(void)
{
	TOGGLE_APPMD(ed_SHOW_RULER);
	return 0;
}
const char *get_str_show_ruler(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_SHOW_RULER));
}
int get_ruler_lines(void)
{
	return GET_APPMD(ed_SHOW_RULER);
}

int tog_show_line_num(void)
{
	TOGGLE_APPMD(ed_SHOW_LINE_NUMBER);
	return 0;
}
const char *get_str_show_line_num(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_SHOW_LINE_NUMBER));
}

int tog_map_key_7f_bs(void)
{
	TOGGLE_APPMD(app_MAP_KEY_7F_BS);
	return 0;
}
const char *get_str_map_key_7f_bs(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(app_MAP_KEY_7F_BS));
}

int inc_backup_files(void)
{
	INC_APPMD(ed_BACKUP_FILES, BACKUP_FILES_MAX);
	return 0;
}
const char *get_str_backup_files(void)
{
	static char buf[10+1];

	snprintf(buf, 10+1, "%d", get_backup_files());
	return buf;
}
int get_backup_files(void)
{
	return GET_APPMD(ed_BACKUP_FILES);
}

int do_tog_editor_panes(void)
{
	TOGGLE_APPMD(ed_EDITOR_PANES);
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
const char *get_editor_panes(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(ed_EDITOR_PANES));
}

int tog_show_dot_file(void)
{
	TOGGLE_APPMD(fl_SHOW_DOT_FILE);
	return 0;
}
const char *get_str_show_dot_file(void)
{
	return BOOL_TO_ON_OFF(GET_APPMD(fl_SHOW_DOT_FILE));
}

int inc_show_file_info(void)
{
	INC_APPMD(fl_SHOW_FILE_INFO, SHOW_FILE_INFO_MAX);
	return 0;
}
const char *get_str_show_file_info(void)
{
	switch(GET_APPMD(fl_SHOW_FILE_INFO)) {
	default:
	case SHOW_FILE_INFO_0:		return "None";
	case SHOW_FILE_INFO_1:		return "Siz";
	case SHOW_FILE_INFO_2:		return "Tim";
	case SHOW_FILE_INFO_3:		return "Siz+Tim";
	case SHOW_FILE_INFO_4:		return "SzTmt7777Us";
	case SHOW_FILE_INFO_5:		return "SzTmRwxUG";
	}
	return "";
}

int inc_sort_by(void)
{
	INC_APPMD(fl_FILE_SORT_BY, FILE_SORT_BY_MAX);
	return 0;
}
const char *get_str_sort_by(void)
{
	switch(GET_APPMD(fl_FILE_SORT_BY)) {
	default:
	case FILE_SORT_BY_NAME:
		return "Name";
	case FILE_SORT_BY_EXT:
		return "Ext";
	case FILE_SORT_BY_TIME:
		return "Time";
	case FILE_SORT_BY_SIZE:
		return "Size";
	}
}

int tog_filer_panes(void)
{
	TOGGLE_APPMD(fl_FILER_PANES);
	return 0;
}
const char *get_str_filer_panes(void)
{
	static char buf[1+1];

	snprintf(buf, 1+1, "%d", GET_APPMD(fl_FILER_PANES) + 1);
	return buf;
}

int inc_key_list_lines(void)
{
	INC_APPMD(app_KEY_LINES, KEY_LINES_MAX);
	return 0;
}
const char *get_str_key_list_lines(void)
{
	static char buf[1+1];

	snprintf(buf, 1+1, "%u", GET_APPMD(app_KEY_LINES));
	return buf;
}
int get_key_list_lines(void)
{
	return GET_APPMD(app_KEY_LINES);
}

//=============================================================================

int do_tog_draw_cursor(void)
{
	tog_draw_cursor();
	SHOW_MODE("Draw cursor", get_str_draw_cursor());
	return 0;
}
int do_tog_auto_indent(void)
{
	tog_auto_indent();
	SHOW_MODE("Auto indent", get_str_auto_indent());
	return 0;
}
int do_tog_dual_scroll(void)
{
	tog_dual_scroll();
	SHOW_MODE("Dual scroll", get_str_dual_scroll());
	return 0;
}
int do_tog_ignore_case(void)
{
	tog_ignore_case();
	SHOW_MODE("Ignore case", get_str_ignore_case());
	return 0;
}
#ifdef ENABLE_REGEX
int do_tog_regexp(void)
{
	tog_regexp();
	SHOW_MODE("RegExp", get_str_regexp());
	return 0;
}
#endif // ENABLE_REGEX

#ifdef ENABLE_UTF8
int do_tog_utf8(void)
{
	tog_utf8();
	SHOW_MODE("UTF-8", get_str_utf8());
	return 0;
}
#endif // ENABLE_UTF8

int do_inc_cursor_positioning(void)
{
	inc_cursor_positioning();
	SHOW_MODE("Cursor center mode", get_str_cursor_positioning());
	return 0;
}

#ifdef ENABLE_SYNTAX
int do_tog_syntax_hl(void)
{
	tog_syntax_hl();
	SHOW_MODE("Syntax Highlighting", get_str_syntax_hl());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_tog_tab_eol_notation(void)
{
	tog_tab_eol_notation();
	SHOW_MODE("Visible TAB/EOL", get_str_tab_eol_notation());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
#endif // ENABLE_SYNTAX

int do_tog_show_ruler(void)
{
	tog_show_ruler();
	SHOW_MODE("Show ruler", get_str_show_ruler());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_tog_show_line_num(void)
{
	tog_show_line_num();
	SHOW_MODE("Show line number", get_str_show_line_num());
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}

int do_tog_map_key_7f_bs(void)
{
	tog_map_key_7f_bs();
	SHOW_MODE("Map Key-Code-7F to Key-BS", get_str_map_key_7f_bs());
	return 0;
}
int do_inc_backup_files(void)
{
	inc_backup_files();
	SHOW_MODE("Backup file(s)", get_str_backup_files());
	return 0;
}
int do_tog_show_dot_file(void)
{
	tog_show_dot_file();
	SHOW_MODE("Show dot file", get_str_show_dot_file());
	return 0;
}
int do_inc_key_list_lines_(void)
{
	inc_key_list_lines();
	SHOW_MODE("Display key list lines", get_str_key_list_lines());
	win_reinit_win_size();
	return 0;
}

int do_examine_key_code(void)
{
	key_code_t key;

	disp_status_bar_ing(_("Input key to show key code"));
	key = input_key_loop();
	disp_status_bar_done(_("Key code: %04x"), key);
	return 0;
}

// End of appmode.c
