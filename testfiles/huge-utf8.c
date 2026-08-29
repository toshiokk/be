namespace be_editor {
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
		return get_str_curs_positioning();
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

int inc_curs_positioning(void)
{
	INC_APPMD(ed_CURS_POSITIONING, CURS_POSITIONING_BOTTOM);
	return 0;
}
const char *get_str_curs_positioning(void)
{
	static char buf[10+1];

	snprintf(buf, 10+1, "%d", get_curs_positioning());
	return buf;
}
int get_curs_positioning(void)
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

int do_inc_curs_positioning(void)
{
	inc_curs_positioning();
	SHOW_MODE("Cursor center mode", get_str_curs_positioning());
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
/**************************************************************************
 *   buffer.c                                                             *
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

#include "headers.h"

// be_buf_t manipulation routines ==========================================
// (common to edit-buffer, cut-buffer, undo-redo-buffer and history)

// Create a new be_buf_t
be_buf_t *buf_create(const char *full_path)
{
	be_buf_t *buf;

	buf = (be_buf_t *)malloc__(sizeof(be_buf_t));
///	get_mem_free_in_kb(1);
	return buf_init(buf, full_path);
}
// initialize be_buf_t members
be_buf_t *buf_init(be_buf_t *buf, const char *full_path)
{
	memset(buf, 0x00, sizeof(*buf));

	buf_clear_link(buf);
	buf_set_file_path(buf, full_path);
	buf->orig_file_stat.st_uid = geteuid();
	buf->orig_file_stat.st_gid = getegid();
	buf->orig_file_stat.st_mode = RW0RW0R00;		// regular file rw-rw-r--(664)
	buf->orig_file_stat.st_mtime = time(NULL);		// time file was created
	buf->orig_file_crc = 0;

	buf_init_line_anchors(buf);

	buf_view_init(&(buf->buf_views[0]), buf);
	buf_view_init(&(buf->buf_views[1]), buf);

	buf->mark_line = BUF_TOP_ANCH(buf);
	buf->mark_line_byte_idx = 0;
	buf->buf_lines = 0;
	buf->buf_size = 0;
	return buf;
}
void buf_view_init(be_buf_view_t *b_v, be_buf_t *buf)
{
	BUF_V_CL(b_v) = BUF_TOP_ANCH(buf);
	BUF_V_CLBI(b_v) = 0;
	BUF_V_CURSOR_Y(b_v) = 0;
	BUF_V_CURSOR_X_TO_KEEP(b_v) = 0;
	BUF_V_MIN_TEXT_X_TO_KEEP(b_v) = 0;
}
be_buf_t *buf_init_line_anchors(be_buf_t *buf)
{
	line_init(BUF_TOP_ANCH(buf), "#LINE-top-anchor");
	line_init(BUF_BOT_ANCH(buf), "#LINE-bot-anchor");
	line_link(BUF_TOP_ANCH(buf), BUF_BOT_ANCH(buf));
	return buf;
}
void buf_set_file_path(be_buf_t *buf, const char *file_path)
{
	strlcpy__(buf->file_path, file_path, MAX_PATH_LEN);
	if (IS_NODE_TOP_ANCH(buf)) {
		strlcpy__(buf->abs_path, file_path, MAX_PATH_LEN);
		return;
	}
	get_abs_path(file_path, buf->abs_path);
}
void buf_get_file_path(be_buf_t *buf, char *file_path)
{
	strlcpy__(file_path, buf->file_path, MAX_PATH_LEN);
}

// Splice a buffer into an existing be_buf_t
////be_buf_t *buf_insert(be_buf_t *buf, be_buf_t *new_buf,
//// insert_before_after_t before_after)
////{
////	if (before_after == INSERT_BEFORE) {
////		buf_insert_before(buf, new_buf);
////	} else {
////		buf_insert_after(buf, new_buf);
////	}
////	return new_buf;
////}

PRIVATE be_buf_t *buf_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next);
be_buf_t *buf_insert_before(be_buf_t *buf, be_buf_t *new_buf)
{
	return buf_insert_between(NODE_PREV(buf), new_buf, buf);
}
be_buf_t *buf_insert_after(be_buf_t *buf, be_buf_t *new_buf)
{
	return buf_insert_between(buf, new_buf, NODE_NEXT(buf));
}
PRIVATE be_buf_t *buf_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next)
{
	buf_link(prev, new_buf);
	buf_link(new_buf, next);
	return new_buf;
}
// Connect two buffers
be_buf_t *buf_link(be_buf_t *prev, be_buf_t *next)
{
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	return prev;
}

// This is NOT deep-copy
be_buf_t *buf_create_copy(be_buf_t *src)
{
	return buf_copy(buf_create(""), src);
}
// This is NOT deep-copy
be_buf_t *buf_copy(be_buf_t *dest, be_buf_t *src)
{
	memcpy__(dest, src, sizeof(*src));
	if (NODE_PREV(dest) != NULL && NODE_NEXT(dest) != NULL) {
		// buffer is linked.
		// clear link and anchors to avoid double linking
		buf_clear_link(dest);
		buf_init_line_anchors(dest);
	}
	BUF_V0_CL(dest) = BUF_V1_CL(dest) = BUF_TOP_ANCH(dest);
	dest->mark_line = BUF_TOP_ANCH(dest);
	return dest;
}

be_buf_t *buf_unlink_free(be_buf_t *buf)
{
	be_buf_t *prev_buf;

///_D_(buf_dump_state(buf));
	if (IS_NODE_VALID(buf) == 0) {
		progerr_printf("CANNOT free this buffer %s\n", buf->file_path);
		return NULL;
	}
	prev_buf = NODE_PREV(buf);
	buf_unlink(buf);
	buf_free(buf);
	return prev_buf;
}

// Unlink a line from the rest of the be_buf_t
be_buf_t *buf_unlink(be_buf_t *buf)
{
	buf_avoid_wild_ptr_cur(buf);
	if (NODE_PREV(buf)) {
		buf->prev->next = NODE_NEXT(buf);
	}
	if (NODE_NEXT(buf)) {
		buf->next->prev = NODE_PREV(buf);
	}
	buf_clear_link(buf);
	return buf;
}
void buf_clear_link(be_buf_t *buf)
{
	buf->prev = NULL;
	buf->next = NULL;
}

///be_buf_t *goto_top_buf(be_buf_t *buf)
///{
///	for ( ; IS_NODE_TOP_ANCH(buf) == 0; buf = NODE_PREV(buf)) {
///	}
///	return buf;
///}
///be_buf_t *goto_bottom_buf(be_buf_t *buf)
///{
///	for ( ; IS_NODE_BOT_ANCH(buf) == 0; buf = NODE_NEXT(buf)) {
///	}
///	return buf;
///}

// Free a buffer
void buf_free(be_buf_t *buf)
{
	buf_free_lines(buf);
	free__(buf);
///	get_mem_free_in_kb(1);
}
void buf_free_lines(be_buf_t *buf)
{
	be_line_t *line;

	// free all lines from top to bottom
	for (line = BUF_TOP_NODE(buf); IS_NODE_INT(line); ) {
		line = line_unlink_free(line);
	}
}

// compare two buffers
int buf_compare(be_buf_t *buf1, be_buf_t *buf2)
{
	be_line_t *line1;
	be_line_t *line2;
	int diff;

	for (line1 = BUF_TOP_NODE(buf1), line2 = BUF_TOP_NODE(buf2);
	 IS_NODE_INT(line1) && IS_NODE_INT(line2); 
	 line1 = NODE_NEXT(line1), line2 = NODE_NEXT(line2)) {
		diff = strncmp(line1->data, line2->data, MAX_EDIT_LINE_LEN);
		if (diff)
			return diff;
	}
	return IS_NODE_BOT_ANCH(line1) - IS_NODE_BOT_ANCH(line2);	// 0: exactly the same
}

int buf_renumber_from_top(be_buf_t *buf)
{
	return buf->buf_lines = buf_renumber_from_line(buf, NODES_TOP_NODE(buf));
}

int buf_renumber_from_line(be_buf_t *buf, be_line_t *line)
{
	return buf->buf_lines = line_renumber_from_line(line, &buf->buf_size);
}

int buf_guess_tab_size(be_buf_t *buf)
{
#define LINES_TO_GUESS_TAB_SIZE		1000
	int lines_checked = 0;
	int lines_space4 = 0;
	be_line_t *line;

	for (line = BUF_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (line_data_len(line) > 4) {
			if (strlcmp__(line->data, "    ") == 0 && line->data[4] != ' ')
				lines_space4++;
		}
		lines_checked++;
		if (lines_checked >= LINES_TO_GUESS_TAB_SIZE)
			break;
	}
	if (lines_space4 >= lines_checked / 10)
		return 8;
	return 0;
}

int buf_count_bufs(be_buf_t *buf)
{
	int count;

	for (count = 0; IS_NODE_INT(buf); count++) {
		buf = NODE_NEXT(buf);
	}
	return count;
}
int buf_count_lines(be_buf_t *buf)
{
	return line_count_lines(BUF_TOP_NODE(buf));
}

int buf_is_orig_file_updated(be_buf_t *buf)
{
	struct stat st;

	if (stat(buf->abs_path, &st)) {
		return -1;
	}
	return st.st_mtime > buf->orig_file_stat.st_mtime;
}
//-----------------------------------------------------------------------------
const char *buf_eol_str(be_buf_t *buf)
{
	switch(BUF_STATE(buf, buf_EOL)) {
	default:
	case EOL_NIX:
		return "LF(NIX)";		/*"NIX"*/;
	case EOL_MAC:
		return "CR(MAC)";		/*"MAC"*/
	case EOL_DOS:
		return "CR+LF(DOS)";	/*"DOS"*/
	}
}
const char *buf_encode_str(be_buf_t *buf)
{
#ifdef USE_NKF
	switch(BUF_STATE(buf, buf_ENCODE)) {
	default:
	case ENCODE_ASCII:		return "ASCII";
	case ENCODE_UTF8:		return "UTF8";
	case ENCODE_EUCJP:		return "EUCJP";
	case ENCODE_SJIS:		return "SJIS";
	case ENCODE_JIS:		return "JIS";
	case ENCODE_BINARY:		return "BIN";
	}
#else // USE_NKF
	return "ASCII";
#endif // USE_NKF
}
const char *buf_cut_mode_str(be_buf_t *buf)
{
	char *ptr;

	switch(BUF_STATE(buf, buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:	ptr = "  ";			break;
	case CUT_MODE_N_LINE:	ptr = "M ";			break;
	case CUT_MODE_H_CHAR:	ptr = "Mc";			break;
	case CUT_MODE_VH_CHAR:	ptr = "MC";			break;
	case CUT_MODE_V_LINE:	ptr = "Ml";			break;
	case CUT_MODE_HV_LINE:	ptr = "ML";			break;
	case CUT_MODE_HV_BOX:	ptr = "Mb";			break;
	case CUT_MODE_VH_BOX:	ptr = "MB";			break;
	}
	return ptr;
}
//-----------------------------------------------------------------------------
be_line_t *buf_set_cur_line(be_buf_t *buf, be_line_t *line)
{
	return BUF_V0_CL(buf) = line;
}
be_line_t *buf_cur_line(be_buf_t *buf)
{
	return BUF_V0_CL(buf);
}
be_line_t *buf_move_cur_line_to_prev(be_buf_t *buf)
{
	be_line_t *line = BUF_V0_CL(buf);
	if (IS_NODE_TOP_ANCH(line) == 0) {
		BUF_V0_CL(buf) = NODE_PREV(line);
		return line;
	}
	// do not move and return NULL
	return NULL;
}
be_line_t *buf_move_cur_line_to_next(be_buf_t *buf)
{
	be_line_t *line = BUF_V0_CL(buf);
	if (IS_NODE_BOT_ANCH(line) == 0) {
		BUF_V0_CL(buf) = NODE_NEXT(line);
		return line;
	}
	// do not move and return NULL
	return NULL;
}

be_line_t *buf_get_line_ptr_from_line_num(be_buf_t *buf, int line_num)
{
	be_line_t *line;

	for (line = BUF_TOP_NODE(buf); line_num > 1; line_num--) {
		if (IS_NODE_BOT(line))
			break;
		line = NODE_NEXT(line);
	}
	return line;
}

//-----------------------------------------------------------------------------

PRIVATE unsigned short file_crc;
PRIVATE long file_size;

void buf_update_crc(be_buf_t *buf)
{
	buf->orig_file_crc = buf_calc_crc(buf);
}
int buf_check_crc(be_buf_t *buf)
{
	return buf->orig_file_crc != buf_calc_crc(buf);
}
unsigned short buf_calc_crc(be_buf_t *buf)
{
	be_line_t *line;
	char *ptr;

	file_size = 0;
	clear_crc16ccitt();
	for (line = BUF_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		file_size += line_data_len(line) + 1;
		for (ptr = line->data; ; ptr++) {
			file_crc = calc_crc16ccitt(*ptr);
			if (*ptr == 0)	// count in last NUL code
				break;
		}
	}
flf_d_printf("crc: %04lx, size: %d\n", file_crc, file_size);
	return file_crc;
}
//-----------------------------------------------------------------------------

be_bufs_t *bufs_init(be_bufs_t *bufs, const char* buf_name)
{
	bufs->prev = bufs->next = NULL;
	strlcpy__(bufs->name, buf_name, MAX_NAME_LEN);
	init_bufs_top_bot_anchor(
	 BUFS_TOP_ANCH(bufs), "#BUF-top-anchor",
	 BUFS_BOT_ANCH(bufs), "#BUF-bot-anchor");
	bufs->cur_buf = BUFS_BOT_ANCH(bufs);
	return bufs;
}
be_bufs_t *bufs_link(be_bufs_t *top_anchor, be_bufs_t *bot_anchor)
{
	top_anchor->next = bot_anchor;
	return bot_anchor->prev = top_anchor;
}
be_bufs_t *bufs_insert_before(be_bufs_t *bufs, be_bufs_t *other)
{
	return bufs_insert_between(NODE_PREV(bufs), other, bufs);
}
be_bufs_t *bufs_insert_between(be_bufs_t *prev, be_bufs_t *mid, be_bufs_t *next)
{
	bufs_link(prev, mid);
	return bufs_link(mid, next);
}

be_bufs_t *bufs_free_all_bufss(be_bufs_t *bufs)
{
	flf_d_printf("bufs: %s\n", bufs->name);
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		flf_d_printf("bufs: %s\n", bufs->name);
		for ( ; ; ) {
			be_buf_t *buf = BUFS_TOP_NODE(bufs);
			flf_d_printf(" %cbuf:[%s]\n",
			 (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path);
			if (IS_NODE_VALID(buf) == 0) {
				break;
			}
			flf_d_printf("    unlink_free %cbuf:[%s]\n",
			 (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path);
			if (bufs->cur_buf == buf) {
				bufs->cur_buf = NODE_NEXT(buf);
			}
			buf_unlink_free(buf);
		}
	}
	return bufs;
}

be_bufs_t *get_bufs_contains_buf(be_bufs_t *bufs, be_buf_t *cur_buf)
{
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		for (be_buf_t *buf = BUFS_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
			if (buf == cur_buf) {
				return bufs;
			}
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------

void init_bufs_top_bot_anchor(
 be_buf_t *buf_top, const char *full_path_top,
 be_buf_t *buf_bot, const char *full_path_bot)
{
	buf_init(buf_top, full_path_top);
	buf_init(buf_bot, full_path_bot);
	buf_link(buf_top, buf_bot);
}

PRIVATE be_buf_t *make_sure_buf_is_top_buf(be_buf_t *bufs)
{
	if (IS_NODE_TOP_ANCH(bufs)) {
		bufs = NODE_NEXT(bufs);
	}
	while (IS_NODE_TOP(bufs) == 0) {
		bufs = NODE_PREV(bufs);
	}
	return bufs;
}

be_buf_t *get_buf_from_bufs_by_idx(be_buf_t *bufs, int buf_idx)
{
	// making sure that bufs is TOP_BUF
	bufs = make_sure_buf_is_top_buf(bufs);
	for ( ; IS_NODE_INT(bufs) && buf_idx > 0; buf_idx--) {
		bufs = NODE_NEXT(bufs);
	}
	return bufs;	// bufs may be top/bottom anchor
}
int get_buf_idx_in_bufs(be_buf_t *bufs, be_buf_t *buf)
{
	bufs = make_sure_buf_is_top_buf(bufs);
	for (int buf_idx = 0; IS_NODE_INT(bufs); buf_idx++, bufs = NODE_NEXT(bufs)) {
		if (bufs == buf)
			return buf_idx;	// found
	}
	return -1;	// not found
}
be_buf_t *get_buf_from_bufs_by_abs_path(be_buf_t *bufs, const char *abs_path)
{
	bufs = make_sure_buf_is_top_buf(bufs);
	for ( ; IS_NODE_INT(bufs); bufs = NODE_NEXT(bufs)) {
		if (strcmp(bufs->abs_path, abs_path) == 0) {
			return bufs;	// found
		}
	}
	return NULL;		// not found
}

//-----------------------------------------------------------------------------
void renumber_all_bufs_from_top(be_bufs_t *bufs)
{
	for (be_buf_t *buf = BUFS_TOP_NODE(bufs); IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		buf_renumber_from_top(buf);
	}
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void buf_dump_bufs(be_buf_t *buf)
{
	int cnt;

flf_d_printf("0============================================\n");
	for (cnt = 0; cnt < 100 && IS_PTR_VALID(buf); cnt++, buf = NODE_NEXT(buf)) {
		buf_dump_ptrs(buf);
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
flf_d_printf("9============================================\n");
}
void buf_dump_bufs_lines(be_buf_t *buf, const char *label)
{
	int cnt;

flf_d_printf("%s {{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n", label);
	for (cnt = 0; cnt < 100 && IS_PTR_VALID(buf); cnt++, buf = NODE_NEXT(buf)) {
		if (buf_count_lines(buf)) {
			buf_dump_lines(buf, 3);
		}
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
flf_d_printf("%s }}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n", label);
}
void buf_dump_lines(be_buf_t *buf, int lines)
{
	buf_dump_state(buf);
	if (buf == NULL) {
		return;
	}
	line_dump_lines(BUF_TOP_ANCH(buf), lines, BUF_V0_CL(buf));
}
void buf_dump_ptrs(be_buf_t *buf)
{
	flf_d_printf("%saddr:%08lx,prev:%08lx,next:%08lx,line:%08lx\n",
	 buf == get_cep_buf() ? ">" : " ",
	 buf, NODE_PREV(buf), NODE_NEXT(buf), NODES_TOP_NODE(buf));
	line_dump_lines(BUF_TOP_ANCH(buf), 3, BUF_V0_CL(buf));
}
void buf_dump_state(be_buf_t *buf)
{
	if (buf == NULL) {
		return;
	}
flf_d_printf("file_path: [%s], abs_path: [%s]\n", buf->file_path, buf->abs_path);
///	buf_dump_ptrs(buf);
///flf_d_printf("byte_idx: %d\n", buf->cur_line_byte_idx);
///flf_d_printf("buf->cursor_y: %d\n", buf->cursor_y);
///flf_d_printf("cursor_x_to_keep: %d\n", buf->cursor_x_to_keep);
///flf_d_printf("buf_lines: %d\n", buf->buf_lines);
///flf_d_printf("buf_size: %d\n", buf->buf_size);
///flf_d_printf("mark_line_byte_idx: %d\n", buf->mark_line_byte_idx);
///flf_d_printf("enc: %s, eol: %s\n", buf_encode_str(buf), buf_eol_str(buf));
}
be_line_t *buf_check_line_in_buf(be_buf_t *buf, be_line_t *line_)
{
	be_line_t *line;

	for (line = BUF_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (line == line_)
			return line_;
	}
	return NULL;
}

void bufs_dump_all_bufs(be_bufs_t *bufs)
{
flf_d_printf("00============================================\n");
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		flf_d_printf("bufs: %s\n", bufs->name);
		for (be_buf_t *buf = BUFS_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
			flf_d_printf(" %cbuf: %s\n", (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path);
			flf_d_printf("   buf->v0_str: %s\n", buf->buf_views[0].cur_line->data);
			flf_d_printf("   buf->v1_str: %s\n", buf->buf_views[1].cur_line->data);
		}
	}
flf_d_printf("99============================================\n");
}

#endif // ENABLE_DEBUG

// End of buffer.c
/**************************************************************************
 *   buffers.c                                                            *
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

#include "headers.h"

PRIVATE editor_panes_t editor_panes;
/////be_buf_t *c_e_b = NULL;			// pointer to the current edit buffer
/////be_buf_view_t *c_b_v = NULL;	// pointer to the current edit buffer view

// collection of collections of buffers
be_bufs_t bufs_top_anchor;		//< top buffers
be_bufs_t bufs_bot_anchor;		//< bottom buffers

// collection of edit buffers--------------------------------------------------
be_bufs_t edit_buffers;

// collection of cut-buffers---------------------------------------------------
be_bufs_t cut_buffers;

// history buffers ------------------------------------------------------------
be_bufs_t history_buffers;

// help buffers ------------------------------------------------------------
be_bufs_t help_buffers;

#ifdef ENABLE_UNDO
// undo buffers ---------------------------------------------------------------
be_bufs_t undo_buffers;
// redo buffers ---------------------------------------------------------------
be_bufs_t redo_buffers;
#endif // ENABLE_UNDO

//=============================================================================

void init_head_of_bufs(void)
{
	bufs_init(&bufs_top_anchor, "#BUFS-top-anchor");
	bufs_init(&bufs_bot_anchor, "#BUFS-bot-anchor");
	bufs_link(&bufs_top_anchor, &bufs_bot_anchor);

	bufs_insert_before(&bufs_bot_anchor, bufs_init(&edit_buffers, "#edit-buffers"));
	bufs_insert_before(&bufs_bot_anchor, bufs_init(&cut_buffers, "#cut-buffers"));
	bufs_insert_before(&bufs_bot_anchor, bufs_init(&history_buffers, "#history-buffers"));
	bufs_insert_before(&bufs_bot_anchor, bufs_init(&help_buffers, "#help-buffers"));
#ifdef ENABLE_UNDO
	bufs_insert_before(&bufs_bot_anchor, bufs_init(&undo_buffers, "#undo-buffers"));
	bufs_insert_before(&bufs_bot_anchor, bufs_init(&redo_buffers, "#redo-buffers"));
#endif // ENABLE_UNDO
}

// Initialize global variables.
void init_buffers(void)
{
	init_head_of_bufs();

	init_edit_bufs();
_FLF_
	init_cut_bufs();

#ifdef ENABLE_HISTORY
	init_hist_bufs();
#endif // ENABLE_HISTORY
#ifdef ENABLE_UNDO
	init_undo_redo_bufs();
#endif // ENABLE_UNDO
#ifdef ENABLE_HELP
	init_help_bufs();
#endif // ENABLE_HELP
}

void free_all_buffers(void)
{
	_D_(bufs_dump_all_bufs(&bufs_top_anchor));
	bufs_free_all_bufss(&bufs_top_anchor);
	_D_(bufs_dump_all_bufs(&bufs_top_anchor));
}

// Edit-buffer manipulation routines

void init_edit_bufs(void)
{
	init_bufs_top_bot_anchor(
	 EDIT_BUFS_TOP_ANCH, "#Edit-bufs-top-anchor",
	 EDIT_BUFS_BOT_ANCH, "#Edit-bufs-bot-anchor");
	init_editor_panes();
}

// free current be_buf_t
// the next or previous buffer will be set to current
int free_cur_edit_buf(void)
{
	return free_edit_buf(get_cep_buf());
}
int free_edit_buf(be_buf_t *edit_buf)
{
	int ret = 1;

#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_CURSPOS, mk_cur_file_pos_str_static());
#endif // ENABLE_HISTORY
	if (edit_buf == get_cep_buf()) {
		// select other buffer
		ret = switch_c_e_b_to_another_buf();
	}
#ifdef ENABLE_UNDO
	// free undo/redo buffers related to this edit_buf
	delete_undo_redo_buf(edit_buf);
#endif // ENABLE_UNDO
	buf_unlink_free(edit_buf);
	return ret;		// 0: no buffer remains
}
//-----------------------------------------------------------------------------
void buf_avoid_wild_ptr_cur(be_buf_t *buf)
{
	// avoid editor_panes.bufs[0] becoming wild-pointer 
	buf_avoid_wild_ptr(buf, &(editor_panes.bufs[0]));
	// avoid editor_panes.bufs[1] becoming wild-pointer 
	buf_avoid_wild_ptr(buf, &(editor_panes.bufs[1]));
}
void buf_avoid_wild_ptr(be_buf_t *buf, be_buf_t **buf_ptr)
{
	if (*buf_ptr == buf) {
		if (IS_NODE_BOT_ANCH(NODE_NEXT(*buf_ptr)) == 0) {
			*buf_ptr = NODE_NEXT(*buf_ptr);
		} else {
			*buf_ptr = NODE_PREV(*buf_ptr);
		}
	}
}
//-----------------------------------------------------------------------------
void line_avoid_wild_ptr_cur(be_line_t *line)
{
	// avoid EPBVX_CL(0) becoming wild-pointer
	line_avoid_wild_ptr(line, &EPBVX_CL(0));
	// avoid EPBVX_CL(1) becoming wild-pointer
	line_avoid_wild_ptr(line, &EPBVX_CL(1));
}
void line_avoid_wild_ptr(be_line_t *line, be_line_t **line_ptr)
{
	if (*line_ptr == line) {
		if (IS_NODE_TOP(*line_ptr) == 0) {
			*line_ptr = NODE_PREV(*line_ptr);
		} else {
			*line_ptr = NODE_NEXT(*line_ptr);
		}
	}
}
//-----------------------------------------------------------------------------
// Editor view management

void init_editor_panes()
{
	int pane_idx;

	for (pane_idx = 0; pane_idx < EDITOR_PANES; pane_idx++) {
		set_editor_pane_n_buf(pane_idx, EDIT_BUFS_TOP_ANCH);
	}
	set_editor_cur_pane_idx(0);
}

void set_editor_cur_pane_idx(int pane_idx)
{
	set_editor_pane_n_buf(pane_idx, NULL);
}
int get_editor_cur_pane_idx(void)
{
	return editor_panes.cur_pane_idx;
}

void set_cep_buf(be_buf_t *buf)	// set edit buffer to current pane
{
	set_editor_pane_n_buf(-1, buf);
}
// be_buf_t *get_cur_editor_pane_buf(void)
be_buf_t *get_cep_buf(void)
{
	return get_editor_pane_buf(-1);
/////	return c_e_b;
}
be_buf_t **get_cep_buf_ptr(void)
{
	return get_editor_pane_buf_ptr(-1);
}
// be_buf_view_t *get_cur_editor_pane_view(void)
be_buf_view_t *get_cep_buf_view(void)
{
	return &(get_cep_buf()->buf_views[get_editor_cur_pane_idx()]);
/////	return c_b_v;
}

void set_editor_pane_n_buf(int pane_idx, be_buf_t *buf)
{
///flf_d_printf("pane_idx: %d ------------\n", pane_idx);
	if (pane_idx < 0) {
		pane_idx = editor_panes.cur_pane_idx;
	} else {
		editor_panes.cur_pane_idx = pane_idx;
	}
	if (buf) {
		editor_panes.bufs[pane_idx] = buf;
	}
	////set_c_e_b(editor_panes.bufs[pane_idx]);
	////set_c_b_v(&(get_cep_buf()->buf_views[pane_idx]));
///_D_(dump_editor_panes());

#ifdef ENABLE_SYNTAX
	set_file_type_by_cur_file_path();
#endif // ENABLE_SYNTAX
	update_tab_size();
}
be_buf_t *get_editor_pane_buf(int pane_idx)
{
	if (pane_idx < 0) {
		pane_idx = editor_panes.cur_pane_idx;
	}
	return editor_panes.bufs[pane_idx];
}
be_buf_t **get_editor_pane_buf_ptr(int pane_idx)
{
	if (pane_idx < 0) {
		pane_idx = editor_panes.cur_pane_idx;
	}
	return &(editor_panes.bufs[pane_idx]);
}

#ifdef ENABLE_DEBUG
void dump_editor_panes(void)
{
	flf_d_printf("editor_panes.cur_pane_idx: %d ===========================\n",
	 editor_panes.cur_pane_idx);
	flf_d_printf("pane_idx-0 ---------------------------------------------\n");
	buf_dump_state(editor_panes.bufs[0]);
	flf_d_printf("get_cep_buf(): %p, &(get_cep_buf()->buf_views[0]): %p\n",
	 get_cep_buf(), &(get_cep_buf()->buf_views[0]));
	line_dump_byte_idx(EPBVX_CL(0), EPBVX_CLBI(0));
	flf_d_printf("EPBVX_CURSOR_Y(0): %d, EPBVX_CURSOR_X_TO_KEEP(0): %d, EPBVX_MIN_TEXT_X_TO_KEEP(0): %d\n",
	 EPBVX_CURSOR_Y(0), EPBVX_CURSOR_X_TO_KEEP(0), EPBVX_MIN_TEXT_X_TO_KEEP(0));
	flf_d_printf("pane_idx-1 ---------------------------------------------\n");
	buf_dump_state(editor_panes.bufs[1]);
	flf_d_printf("get_cep_buf(): %p, &(get_cep_buf()->buf_views[1]): %p\n",
	 get_cep_buf(), &(get_cep_buf()->buf_views[1]));
	line_dump_byte_idx(EPBVX_CL(1), EPBVX_CLBI(1));
	flf_d_printf("EPBVX_CURSOR_Y(1): %d, EPBVX_CURSOR_X_TO_KEEP(1): %d, EPBVX_MIN_TEXT_X_TO_KEEP(1): %d\n",
	 EPBVX_CURSOR_Y(1), EPBVX_CURSOR_X_TO_KEEP(1), EPBVX_MIN_TEXT_X_TO_KEEP(1));
	flf_d_printf("========================================================\n");
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------

be_buf_t *get_edit_buf_from_abs_path(const char *abs_path)
{
	return get_buf_from_bufs_by_abs_path(EDIT_BUFS_TOP_NODE, abs_path);
}
int get_edit_buf_idx_from_buf(be_buf_t *edit_buf)
{
	return get_buf_idx_in_bufs(EDIT_BUFS_TOP_NODE, edit_buf);
}

//-----------------------------------------------------------------------------
// create new be_buf_t
void create_edit_buf(const char *full_path)
{
	be_buf_t *buf;

	buf = buf_create(full_path);
	buf_insert_before(EDIT_BUFS_BOT_ANCH, buf);
///_FLF_
	set_cep_buf(buf);
	if (IS_NODE_VALID(editor_panes.bufs[0]) == 0) {
		// make view-0 buffer valid
		editor_panes.bufs[0] = buf;
	}
	if (IS_NODE_VALID(editor_panes.bufs[1]) == 0) {
		// make view-1 buffer valid
		editor_panes.bufs[1] = buf;
	}
}

//-----------------------------------------------------------------------------

// Append a new line to the bottom of the current buffer
be_line_t *append_string_to_cur_edit_buf(const char *string)
{
	EPBVX_CL(0) = EPBVX_CL(1) = line_insert_with_string(CUR_EDIT_BUF_BOT_ANCH, INSERT_BEFORE, string);
	EPBVX_CLBI(0) = EPBVX_CLBI(1) = 0;
	return CEPBV_CL;
}

// Append a new magic-line to the bottom of the current buffer
void append_magic_line(void)
{
	if (buf_count_lines(get_cep_buf()) == 0 || line_data_len(CUR_EDIT_BUF_BOT_NODE)) {
		append_string_to_cur_edit_buf("");
	}
}

int count_edit_bufs(void)
{
	return buf_count_bufs(EDIT_BUFS_TOP_NODE);
}

int is_c_e_b_valid(void)
{
	return IS_NODE_VALID(get_cep_buf());
}

// Cut-buffers manipulation routines -----------------------------------------

void init_cut_bufs(void)
{
	init_bufs_top_bot_anchor(
	 CUT_BUFS_TOP_ANCH, "#Cut-bufs-top-anchor",
	 CUT_BUFS_BOT_ANCH, "#Cut-bufs-bot-anchor");
}
void free_all_cut_bufs(void)
{
	while (IS_NODE_INT(CUR_CUT_BUF)) {
		pop_cut_buf();
	}
}
be_buf_t *push_cut_buf(void)
{
	be_buf_t *cut_buf;
	char buf_name[MAX_PATH_LEN+1];

	snprintf(buf_name, MAX_PATH_LEN, "#cut-buffer-%02d", count_cut_bufs());
	cut_buf = buf_create(buf_name);
	buf_insert_after(CUT_BUFS_TOP_ANCH, cut_buf);
	// copy cut-mode to cut-buffer
	SET_CUR_CBUF_STATE(buf_CUT_MODE, CUR_EBUF_STATE(buf_CUT_MODE));
	return cut_buf;
}
int pop_cut_buf(void)
{
	if (IS_NODE_BOT_ANCH(CUR_CUT_BUF))
		return 0;
	buf_unlink_free(CUR_CUT_BUF);
	return 1;
}
be_line_t *append_string_to_cur_cut_buf(const char *string)
{
	return line_insert_with_string(CUR_CUT_BUF_BOT_ANCH, INSERT_BEFORE, string);
}
int count_cut_bufs(void)
{
	return buf_count_bufs(CUR_CUT_BUF);
}
int count_cur_cut_buf_lines(void)
{
	return buf_count_lines(CUR_CUT_BUF);
}

//-----------------------------------------------------------------------------

void renumber_cur_buf_from_top(void)
{
	buf_renumber_from_line(get_cep_buf(), CUR_EDIT_BUF_TOP_NODE);
}

be_line_t *get_line_ptr_from_cur_buf_line_num(int line_num)
{
	return buf_get_line_ptr_from_line_num(get_cep_buf(), line_num);
}

//-----------------------------------------------------------------------------

void update_cur_buf_crc(void)
{
	disp_status_bar_ing(_("Calculating CRC..."));
	buf_update_crc(get_cep_buf());
}
int check_cur_buf_modified(void)
{
	int modified = 0;

	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		disp_status_bar_ing(_("Calculating CRC..."));
		modified = buf_check_crc(get_cep_buf());
	}
///flf_d_printf("modified: %d\n", modified);
	return modified;
}
void clear_cur_buf_modified(void)
{
	SET_CUR_EBUF_STATE(buf_MODIFIED, 0);
}

//-----------------------------------------------------------------------------

int tog_view_mode(void)
{
	TOGGLE_CUR_EBUF_STATE(buf_VIEW_MODE);
	return 0;
}
const char *get_str_view_mode(void)
{
	return BOOL_TO_ON_OFF(CUR_EBUF_STATE(buf_VIEW_MODE));
}

int tog_line_wrap_mode(void)
{
	TOGGLE_CUR_EBUF_STATE(buf_LINE_WRAP_MODE);
	return 0;
}
const char *get_str_line_wrap_mode(void)
{
	return BOOL_TO_ON_OFF(CUR_EBUF_STATE(buf_LINE_WRAP_MODE));
}

int tog_tab_size(void)
{
	CUR_EBUF_STATE(buf_TAB_SIZE)
	 = CUR_EBUF_STATE(buf_TAB_SIZE) == 8 ? 4 : 8;
	update_tab_size();
	return 0;
}
int inc_tab_size(void)
{
	INC_CUR_EBUF_STATE(buf_TAB_SIZE, TAB_SIZE_MIN, TAB_SIZE_MAX);
	update_tab_size();
	return 0;
}
const char *get_str_tab_size(void)
{
	static char buf[2+1];

	snprintf(buf, 2+1, "%d", get_cur_buf_tab_size());
	return buf;
}
int get_cur_buf_tab_size(void)
{
	// to avoid divided-by-0, check 0 and replace with non-zero-value
	return CUR_EBUF_STATE(buf_TAB_SIZE) == 0
	 ? DEFAULT_TAB_SIZE : CUR_EBUF_STATE(buf_TAB_SIZE);
}
void update_tab_size(void)
{
	set_wrap_line_tab_size(get_cur_buf_tab_size());
}

int set_nix_file(void)
{
	set_eol(EOL_NIX);
	return 0;
}
const char *get_str_nix_file(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_EOL, EOL_NIX));
}
int set_mac_file(void)
{
	set_eol(EOL_MAC);
	return 0;
}
const char *get_str_mac_file(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_EOL, EOL_MAC));
}
int set_dos_file(void)
{
	set_eol(EOL_DOS);
	return 0;
}
const char *get_str_dos_file(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_EOL, EOL_DOS));
}
int set_eol(int eol)
{
	CUR_EBUF_STATE(buf_EOL) = eol;
	return 0;
}
const char *get_str_eol(void)
{
	return buf_eol_str(get_cep_buf());
}

#ifdef USE_NKF
int set_encode_ascii(void)
{
	set_encode(ENCODE_ASCII);
	return 0;
}
const char *get_str_encode_ascii(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_ASCII));
}
int set_encode_utf8(void)
{
	set_encode(ENCODE_UTF8);
	return 0;
}
const char *get_str_encode_utf8(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8));
}
int set_encode_eucjp(void)
{
	set_encode(ENCODE_EUCJP);
	return 0;
}
const char *get_str_encode_eucjp(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_EUCJP));
}
int set_encode_sjis(void)
{
	set_encode(ENCODE_SJIS);
	return 0;
}
const char *get_str_encode_sjis(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_SJIS));
}
int set_encode_jis(void)
{
	set_encode(ENCODE_JIS);
	return 0;
}
const char *get_str_encode_jis(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_JIS));
}
int set_encode_binary(void)
{
	set_encode(ENCODE_BINARY);
	return 0;
}
const char *get_str_encode_binary(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_BINARY));
}

int set_encode(int encode)
{
	CUR_EBUF_STATE(buf_ENCODE) = encode;
	return 0;
}
const char *get_str_encode(void)
{
	return buf_encode_str(get_cep_buf());
}
#endif // USE_NKF

//-----------------------------------------------------------------------------

// If "modified" is not set, set it and update titlebar.
void set_cur_buf_modified(void)
{
	if (CUR_EBUF_STATE(buf_MODIFIED) == 0) {
		CUR_EBUF_STATE(buf_MODIFIED) = 1;
		disp_editor_title_bar();
	}
}

//-----------------------------------------------------------------------------

int do_tog_view_mode(void)
{
	tog_view_mode();
	SHOW_MODE("View mode", get_str_view_mode());
	return 0;
}
int do_tog_line_wrap_mode(void)
{
	tog_line_wrap_mode();
	SHOW_MODE("Line-wrap mode", get_str_line_wrap_mode());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_tog_tab_size(void)
{
	tog_tab_size();
	SHOW_MODE(_("Tab size"), get_str_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_inc_tab_size(void)
{
	inc_tab_size();
	SHOW_MODE(_("Tab size"), get_str_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_set_nix_file(void)
{
	set_nix_file();
	SHOW_MODE("File format", get_str_eol());
	return 0;
}
int do_set_mac_file(void)
{
	set_mac_file();
	SHOW_MODE("File format", get_str_eol());
	return 0;
}
int do_set_dos_file(void)
{
	set_dos_file();
	SHOW_MODE("File format", get_str_eol());
	return 0;
}
//-----------------------------------------------------------------------------
#ifdef USE_NKF
int do_set_encode_ascii(void)
{
	set_encode_ascii();
	SHOW_MODE("ASCII format", get_str_encode_ascii());
	return 0;
}
int do_set_encode_utf8(void)
{
	set_encode_utf8();
	SHOW_MODE("UTF-8 format", get_str_encode_utf8());
	return 0;
}
int do_set_encode_eucjp(void)
{
	set_encode_eucjp();
	SHOW_MODE("EUC-JP format", get_str_encode_eucjp());
	return 0;
}
int do_set_encode_sjis(void)
{
	set_encode_sjis();
	SHOW_MODE("SJIS format", get_str_encode_sjis());
	return 0;
}
int do_set_encode_jis(void)
{
	set_encode_jis();
	SHOW_MODE("JIS format", get_str_encode_jis());
	return 0;
}
int do_set_encode_binary(void)
{
	set_encode_binary();
	SHOW_MODE("BINARY format", get_str_encode_binary());
	return 0;
}
#endif // USE_NKF

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_cur_edit_buf_lines(void)
{
	buf_dump_lines(get_cep_buf(), 3);
}
void dump_edit_bufs(void)
{
	buf_dump_bufs(EDIT_BUFS_TOP_ANCH);
}
void dump_edit_bufs_lines(void)
{
	buf_dump_bufs_lines(EDIT_BUFS_TOP_ANCH, "edit-bufs");
}
void dump_cut_bufs(void)
{
	buf_dump_bufs(CUT_BUFS_TOP_ANCH);
}
void dump_cut_bufs_lines(void)
{
	buf_dump_bufs_lines(CUT_BUFS_TOP_ANCH, "cut-bufs");
}

// dump current buffer
void dump_cur_edit_buf(void)
{
flf_d_printf("<<<<<<<<<<<<<<<<<<<\n");
flf_d_printf("CUR_EDIT_BUF_TOP_NODE:%08lx\n", CUR_EDIT_BUF_TOP_NODE);
	if (CUR_EDIT_BUF_TOP_NODE) {
		flf_d_printf("CUR_EDIT_BUF_TOP_NODE->data:%08lx\n", CUR_EDIT_BUF_TOP_NODE->data);
	}
flf_d_printf("CUR_EDIT_BUF_BOT_NODE:%08lx\n", CUR_EDIT_BUF_BOT_NODE);
	if (CUR_EDIT_BUF_BOT_NODE) {
		flf_d_printf("CUR_EDIT_BUF_BOT_NODE->data:%08lx\n", CUR_EDIT_BUF_BOT_NODE->data);
	}
flf_d_printf("cur_line:%08lx\n", CEPBV_CL);
	if (CEPBV_CL) {
		flf_d_printf("cur_line->data:%08lx\n", CEPBV_CL->data);
	}
	line_dump_lines(CUR_EDIT_BUF_TOP_ANCH, INT_MAX, CEPBV_CL);
flf_d_printf(">>>>>>>>>>>>>>>>>>>\n");
}
#endif // ENABLE_DEBUG

// End of buffers.c
/**************************************************************************
 *   color.c                                                              *
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

#include "headers.h"

// Color table for the various items
item_color_t item_colors[MAX_ITEM_COLORS];

item_color_t default_item_colors[MAX_ITEM_COLORS] = {
	//  bgc, fgc
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_DEFAULT)			},
#ifndef ENABLE_EXPERIMENTAL
	{ CL_BL, CL_CY, S(ITEM_COLOR_IDX_TITLE)				},
	{ CL_BL, CL_CY, S(ITEM_COLOR_IDX_STATUS)			},
#else // ENABLE_EXPERIMENTAL
	{ CL_GR, CL_BL, S(ITEM_COLOR_IDX_TITLE)				},
	{ CL_GR, CL_BL, S(ITEM_COLOR_IDX_STATUS)			},
#endif // ENABLE_EXPERIMENTAL
	{ CL_WH, CL_BL, S(ITEM_COLOR_IDX_KEY_LIST)			},
	{ CL_BK, CL_BR, S(ITEM_COLOR_IDX_KEY_LIST2)			},
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_TEXT_NORMAL)		},
	{ CL_RD, CL_WH, S(ITEM_COLOR_IDX_TEXT_SELECTED)		},
///	{ CL_BR, CL_WH, S(ITEM_COLOR_IDX_TEXT_SELECTED2)	},
	{ CL_GR, CL_WH, S(ITEM_COLOR_IDX_TEXT_SELECTED2)	},
	{ CL_RD, CL_GR, S(ITEM_COLOR_IDX_WARNING)			},
	{ CL_BK, CL_WH, S(ITEM_COLOR_IDX_CURSOR_CHAR)		},
	{ CL_CY, CL_DF, S(ITEM_COLOR_IDX_CURSOR_LINE)		},
	{ CL_BK, CL_GR, S(ITEM_COLOR_IDX_LINE_NUMBER)		},
	{ CL_CY, CL_BL, S(ITEM_COLOR_IDX_MENU_FRAME)		},
	{ CL_BL, CL_CY, S(ITEM_COLOR_IDX_MENU_ITEM)			},
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_MENU_SELECTED)		},
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_INPUT)				},
};

// Initialize with default application color set.
void init_default_app_color(void)
{
	memcpy__(item_colors, default_item_colors, sizeof(default_item_colors));
}

PRIVATE int work_space_color_low = 0;
void set_work_space_color_low(void)
{
	work_space_color_low++;
}
void set_work_space_color_normal(void)
{
	if (work_space_color_low)
		work_space_color_low--;
}
int is_work_space_color_low(void)
{
	return work_space_color_low;
}

//-----------------------------------------------------------------------------

// register item color
void register_item_color(item_color_idx_t color_idx, int bgc, int fgc)
{
flf_d_printf("(%d bgc:%d, fgc:%d)\n", color_idx, bgc, fgc);
	if (0 <= color_idx && color_idx < MAX_ITEM_COLORS) {
		item_colors[color_idx].bgc = bgc;
		item_colors[color_idx].fgc = fgc;
	}
}

// get item color
void get_color_by_idx(item_color_idx_t color_idx, char *fgc, char *bgc)
{
	if (0 <= color_idx && color_idx < MAX_ITEM_COLORS) {
		*bgc = item_colors[color_idx].bgc;
		*fgc = item_colors[color_idx].fgc;
	}
}
// set current color by item_idx
void set_color_by_idx(item_color_idx_t color_idx, int reverse)
{
	int bgc;

	if (0 <= color_idx && color_idx < MAX_ITEM_COLORS) {
		bgc = item_colors[color_idx].bgc;
#if 1
		if (is_work_space_color_low()) {
			if (bgc == CL_WH) {
				bgc = CL_BR;
///				bgc = CL_CY;
///				bgc = CL_BL;
			}
		}
#endif
		tio_set_attrs(bgc, item_colors[color_idx].fgc, reverse);
	}
}

void set_item_color(const item_color_t *item_color)
{
	tio_set_attrs(item_color->bgc, item_color->fgc, 0);
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_SYNTAX

file_type_t *file_types_head = NULL;		//!< head of file types
file_type_t *cur_file_type = NULL;			//!< current file type
file_type_t *default_color_syntax = NULL;	//!< default color syntax

// Select color set by current file name
int set_file_type_by_cur_file_path(void)
{
	int ret;

	if ((ret = set_file_type_by_file_name(get_cep_buf()->file_path)) != 0) {
		CUR_EBUF_STATE(buf_TAB_SIZE) = cur_file_type->tab_size;
	}
	return ret;
}
int set_file_type_by_file_name(const char *file_path)
{
	file_type_t *file_type;
	file_type_t *last_file_type;
	regexp_matches_t regexp_matches;

	cur_file_type = NULL;
	for (file_type = last_file_type = file_types_head; file_type != NULL;
	 last_file_type = file_type, file_type = NODE_NEXT(file_type)) {
		// set cur_file_type if match the file name regexp
		if (regexp_search_compiled(file_type->regexp, file_path, 0,
		 REG_NONE, &regexp_matches, 1) == 0) {
			cur_file_type = file_type;
			break;
		}
	}
	// if we haven't found a match, use the last(default) color set
	if (cur_file_type == NULL) {
		cur_file_type = last_file_type;
		return 0;
	}
///_D_(dump_file_type(cur_file_type, 0));
	return 1;
}

// get the beginning of the currently selected color set
const color_syntax_t *get_color_syntax_head(void)
{
	if (cur_file_type)
		return cur_file_type->color_syntax;	// top of the color set
	return NULL;
}
const color_syntax_t *get_default_color_syntax_head(void)
{
	if (default_color_syntax)
		return default_color_syntax->color_syntax;	// top of the color set
	return NULL;
}

#endif // ENABLE_SYNTAX

//-----------------------------------------------------------------------------

PRIVATE void display_color_pattern(int yy, int xx, int reverse);
int display_color_pairs(int yy, int xx)
{
	tio_clear_flash_screen(1);
	display_color_pattern(yy, xx+ 0, 0);
	display_color_pattern(yy, xx+34, 1);
	tio_refresh();
	return 0;
}
// display color pattern to the specified screen position
PRIVATE void display_color_pattern(int yy, int xx, int reverse)
{
	int bgc;
	int fgc;
#define BUF_LEN		100
	char buf[BUF_LEN+1];
	char buffer[BUF_LEN+1];

#define X_OFF		0
							//01234567890123456789
	snprintf(buf, BUF_LEN+1, "COLOR_PAIRS: %d, REV: %d", COLOR_PAIRS, reverse);
	set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	snprintf(buffer, BUF_LEN+1, "%-*s", BUF_LEN, buf);
	tio_output_string(main_win_get_mid_win_y() + yy + 0, xx + 0, buffer, -1);

	for (bgc = 0; bgc < COLORS16; bgc++) {
		for (fgc = 0; fgc < COLORS16; fgc++) {
			tio_set_attrs(bgc, fgc, reverse);
			snprintf(buf, BUF_LEN+1, "%X%X", bgc, fgc);
			tio_output_string(main_win_get_mid_win_y() + yy + 1 + bgc,
			 xx + X_OFF + fgc * 2, buf, -1);
		}
	}
}

#ifdef ENABLE_DEBUG
int display_item_colors(int yy, int xx)
{
	int item_idx;
	char buffer[MAX_PATH_LEN+1];

	for (item_idx = 0; item_idx < MAX_ITEM_COLORS; item_idx++) {
		set_color_by_idx(item_idx, 0);
		snprintf(buffer, MAX_PATH_LEN, "%2d: %-40s",
		 item_idx, default_item_colors[item_idx].item_name);
		tio_output_string(main_win_get_mid_win_y() + yy + item_idx, xx + 0, buffer, -1);
	}
	tio_refresh();
	return 0;
}
int display_bracket_hl_colors(int yy, int xx)
{
	int color_idx;
	char fgc, bgc;
	char buffer[MAX_PATH_LEN+1];

	prepare_colors_for_bracket_hl(0);
	for (color_idx = 0; color_idx < get_colors_for_bracket_hl(); color_idx++) {
		set_color_for_bracket_hl(color_idx);
		snprintf(buffer, MAX_PATH_LEN, "%2d: ([{<>}])", color_idx);
		tio_output_string(main_win_get_mid_win_y() + yy + color_idx, xx + 0, buffer, -1);
	}
	tio_refresh();
	return 0;
}
#endif // ENABLE_DEBUG

// End of color.c
/**************************************************************************
 *   cursesif.c - CURSES interface                                        *
 *                                                                        *
 *   Copyright (C) 2005-2018 PCR                                          *
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

#include "headers.h"

#ifdef ENABLE_NCURSES

PRIVATE WINDOW *curses_win;

PRIVATE int get_color_pair(int bgc, int fgc);
PRIVATE int init_color_pairs(void);
PRIVATE key_code_t getch_(void);

//-----------------------------------------------------------------------------
PRIVATE struct termios term_settings_save;	/* The user's original term settings */
PRIVATE char curses_bgc = 0;
PRIVATE char curses_fgc = 7;
PRIVATE char curses_rev = 0;
PRIVATE char curses_color_pair = -1;
PRIVATE char curses_cur_color_pair = -1;

int curses_init(void)
{
	curses_bgc = 0;
	curses_fgc = 7;
	curses_rev = 0;
	curses_color_pair = -1;
	curses_cur_color_pair = -1;
	save_term_settings(&term_settings_save);
	return 0;
}
int curses_begin(void)
{
	set_term_no_intr();
	set_term_raw();
	curses_win = initscr();
	nonl();
	cbreak();
	noecho();
	raw();
	keypad(curses_win, TRUE);
	nodelay(curses_win, TRUE);
	ESCDELAY = 1;
	init_color_pairs();
	return 0;
}
int curses_end(void)
{
	keypad(curses_win, TRUE);
	nocbreak();
	echo();
	endwin();
	restore_term_settings(&term_settings_save);
	return 0;
}
//-----------------------------------------------------------------------------
void curses_set_screen_size(int lines, int columns)
{
	LINES = lines;
	COLS = columns;
}
int curses_get_lines(void)
{
	return LINES;
}
int curses_get_columns(void)
{
	return LIM_MAX(MAX_SCRN_COLS, COLS);
}

//-----------------------------------------------------------------------------

PRIVATE int get_color_pair(int bgc, int fgc)
{
	if (GET_BASE_COLOR(bgc) == CL_BK && GET_BASE_COLOR(fgc) == CL_BK) {
		fgc = CL_GY;	// (0, 0) ==> (0, 7)
	}
	return CP_FROM_CBF(bgc, fgc);	// [1, COLOR_PAIRS-1]
}

PRIVATE int init_color_pairs(void)
{
	int fgc, bgc;

	start_color();
////flf_d_printf("COLOR_PAIRS: %d\n", COLOR_PAIRS);
////flf_d_printf("COLORS: %d\n", COLORS);
	for (bgc = 0; bgc < COLORS; bgc++) {
		for (fgc = 0; fgc < COLORS; fgc++) {
			init_pair(get_color_pair(bgc, fgc), fgc, bgc);
		}
	}
	return 0;
}

void curses_set_attrs(int bgc, int fgc, int rev)
{
////flf_d_printf("rev:%d, bold:%d, bgc:%d, fgc:%d\n", rev, bold, bgc, fgc);
	if (bgc >= 0) {
		curses_bgc = bgc;
	}
	if (fgc >= 0) {
		curses_fgc = fgc;
	}
	if (rev >= 0) {
		curses_rev = rev;
	}
	if (curses_rev == 0) {
		bgc = curses_bgc;
		fgc = curses_fgc;
	} else {
		// reverse ==> swap foreground and background color
		bgc = curses_fgc;
		fgc = curses_bgc;
	}
	bgc = LIMIT_BGC(bgc);
	fgc = LIMIT_FGC(fgc);
	fgc = differ_fgc_to_bgc(bgc, fgc);
	if (fgc >= CL_HI) {
		wattron(curses_win, A_BOLD);	// only foreground(character) color will be lighted
	} else {
		wattroff(curses_win, A_BOLD);
	}
	curses_color_pair = get_color_pair(bgc, fgc);
	if (curses_cur_color_pair != curses_color_pair) {
		wattron(curses_win, COLOR_PAIR(curses_color_pair));
	}
	curses_cur_color_pair = curses_color_pair;
}
void curses_set_cursor_on(int on_off)
{
	curs_set(on_off);
}
void curses_set_cursor_pos(int yy, int xx)
{
////flf_d_printf("%d, %d\n", yy, xx);
	wmove(curses_win, yy, xx);
}
void curses_clear_screen(void)
{
	clear();
}
void curses_output_string(int yy, int xx, const char *string, int bytes)
{
	if (yy >= 0 && xx >= 0)
		curses_set_cursor_pos(yy, xx);
	waddnstr(curses_win, (char *)string, bytes);
}
void curses_beep(void)
{
	beep();
}
void curses_refresh(void)
{
	refresh();
}
//-----------------------------------------------------------------------------
key_code_t curses_input_key(void)
{
	key_code_t key1;
	key_code_t key2;

	if ((key1 = getch_()) < 0) {
		return key1;
	}
	if (key1 == CHAR_ESC) {
		if ((key2 = getch_()) >= 0) {
			key1 = KEY_META(key2);	// 0x1b, 0xXX ==> 0x1bXX
		}
	}
	return key1;
}

PRIVATE key_code_t getch_(void)
{
	int key;

	key = getch();
////	flf_d_printf("getch():%04x %d\n", key, key);
	return key;
}

#endif // ENABLE_NCURSES

// End of cursesif.c
/**************************************************************************
 *   debug.c                                                              *
 *                                                                        *
 *   Copyright (C) 2007-2008 PCR                                          *
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

#include "utilincs.h"

#ifdef ENABLE_DEBUG

void dump_memory(char *message, void *memory, int bytes)
{
	unsigned char *ptr;
	int cnt;
	int left;

	ptr = (unsigned char *)memory;
	e_printf("%s", message);
	for (cnt = 0; cnt < bytes; ) {
		e_printf("%08x ", ptr);
		for (left = cnt; cnt < left + 16 && cnt < bytes; cnt++) {
			if (cnt % 4 != 3)
				e_printf("%02x-", *ptr++);
			else
				e_printf("%02x ", *ptr++);
		}
		e_printf("\n");
	}
}
char *dump_str(char *str)
{
#define MAX_LN	(65536 * 2)
	static char buf[MAX_LN + 1];
	int cnt;
	int len;

	strcpy__(buf, "");
	len = 0;
	for (cnt = 0; str[cnt]; cnt++) {
		len = strnlen(buf, MAX_LN);
		if (str[cnt] == '\t') {
			strlcat__(buf, MAX_LN-len, "{TB}");
		} else if (str[cnt] == '\n') {
			strlcat__(buf, MAX_LN-len, "{NL}");
		} else if (str[cnt] == '\x7f') {
			strlcat__(buf, MAX_LN-len, "{DL}");
		} else {
			buf[len] = str[cnt];
			buf[len+1] = '\0';
		}
		len = strnlen(buf, MAX_LN);
		if (len >= MAX_LN-10)
			break;
	}
	return buf;
}

#define DEBUG_BUF_LEN	4096

#define RING_BUF_LINES		10
#ifdef RING_BUF_LINES
PRIVATE int ring_buf_idx = 0;
PRIVATE char ring_buffer[RING_BUF_LINES][DEBUG_BUF_LEN+1] =
 { "", "", "", "", "", "", "", "", "", "", };
#endif

void tflfl_d_printf_(int time, const char *file, int line,
 const char *func, const char *label,
 const char *format, ...)
{
	char buf[DEBUG_BUF_LEN+1];
	char buf_time[MAX_PATH_LEN+1] = "";
	char buf_file_line[MAX_PATH_LEN+1] = "";
	char buf_func[MAX_PATH_LEN+1] = "";
	char buf_label[MAX_PATH_LEN+1] = "";
	char buffer[DEBUG_BUF_LEN+1];
	va_list list;

	va_start(list, format);
	vsnprintf(buf, DEBUG_BUF_LEN, format, list);
	va_end(list);

	switch (time) {
	case 1:
		get_sssssspmmm(buffer);
		snprintf_(buf_time, MAX_PATH_LEN, "MSEC %s ", buffer);
		break;
	case 3:
		snprintf_(buf_time, MAX_PATH_LEN, "%s ", cur_hhcmmcss_mmm());
		break;
	case 6:
		snprintf_(buf_time, MAX_PATH_LEN, "%s ", cur_hhcmmcss_uuuuuu());
		break;
	}
	if (file[0]) {
		snprintf_(buf_file_line, MAX_PATH_LEN, "%s %d ", file, line);
	}
	if (func[0]) {
		snprintf_(buf_func, MAX_PATH_LEN, "%s ", func);
	}
	if (label[0]) {
		snprintf_(buf_label, MAX_PATH_LEN, "%s ", label);
	}
	snprintf_(buffer, DEBUG_BUF_LEN, "%s%s%s%s%s", buf_time, buf_file_line,
	 buf_func, buf_label, buf);
#ifdef RING_BUF_LINES
	strlcpy__(ring_buffer[ring_buf_idx], buffer, DEBUG_BUF_LEN);
	if (++ring_buf_idx >= RING_BUF_LINES)
		ring_buf_idx = 0;
#endif
	e_printf("%s", buffer);
}

void output_last_d_printf(void)
{
#ifdef RING_BUF_LINES
	FILE *fp;
	int lines;

	if ((fp = fopen("becrash.log", "w")) == NULL)
		return;
	for (lines = 0; lines < RING_BUF_LINES; lines++) {
		fprintf(fp, "%s", ring_buffer[ring_buf_idx]);
		if (++ring_buf_idx >= RING_BUF_LINES)
			ring_buf_idx = 0;
	}
	if (fclose(fp) != 0) {
		// error
	}
#endif
}

PRIVATE int debug_printf_output = 0;
void set_debug_printf_output(int on1_off0)
{
	debug_printf_output = on1_off0;
}
void debug_printf(const char *format, ...)
{
	va_list list;

	va_start(list, format);
	debug_vprintf(format, list);
	va_end(list);
}
void debug_vprintf(const char *format, va_list ap)
{
	if (debug_printf_output) {
		vfprintf(stderr, format, ap);
	}
}

#endif // ENABLE_DEBUG

// End of debug.c
/**************************************************************************
 *   disp.c                                                               *
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

#include "headers.h"

void set_title_bar_color_by_state(int c_e_b_cut_mode)
{
	if (key_macro_is_recording()) {
		// Recording key macro, you see strange color on title bar.
		set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, 0);
	} else
	if (c_e_b_cut_mode) {
		// Marking text, you see strange color on title bar.
		set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
	} else
	if (geteuid() == 0) {
		// If you are super user, you see strange color on title bar.
		set_color_by_idx(ITEM_COLOR_IDX_WARNING, 0);
	} else {
		// Normal color
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, 0);
	}
}

const char *root_notation(void)
{
	return (geteuid() == 0) ? "[ROOT] " : "";
}

//-----------------------------------------------------------------------------
PRIVATE s_b_d_t status_bar_displayed = S_B_D_NONE;

PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 int dividend, int divisor, const char *msg, va_list ap);

// enable status bar update
void clear_status_bar_displayed(void)
{
	set_status_bar_displayed(S_B_D_NONE);
}
void set_status_bar_displayed(s_b_d_t s_b_d)
{
	status_bar_displayed = s_b_d;
}
void disp_status_bar_ing(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_ING, 0, 0, msg, ap);
	va_end(ap);
	tio_refresh();	// update screen soon
}
void disp_status_bar_done(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_DONE, 0, 0, msg, ap);
	va_end(ap);
}
void disp_status_bar_err(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_ERR, 0, 0, msg, ap);
	va_end(ap);
	tio_beep();
}
void disp_status_bar_percent_editor(int dividend, int divisor, const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_PERCENT_EDITOR, dividend, divisor, msg, ap);
	va_end(ap);
}
void disp_status_bar_percent_filer(int dividend, int divisor, const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_PERCENT_FILER, dividend, divisor, msg, ap);
	va_end(ap);
}

// Examples:
//  Reading File filename.ext ...
//  Writing File filename.ext ...
//  "abc" not found  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CODE:2f ENC:ASCII EOL:LF(NIX)
//  2 files loaded  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CODE:2f ENC:ASCII EOL:LF(NIX)
PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 int dividend, int divisor, const char *msg, va_list ap)
{
	int update = 0;
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	static char prev_msg[MAX_SCRN_LINE_BUF_LEN+1];
	int color_idx;
	int col_idx;
	int byte_idx;

	switch (status_bar_to_display) {
	default:
	case S_B_D_NONE:
	case S_B_D_PERCENT_FILER:
	case S_B_D_ING:
	case S_B_D_DONE:
///		if (status_bar_to_display >= status_bar_displayed) {
			// display this if higher or the same priority
			status_bar_displayed = status_bar_to_display;
			update = 1;
///		}
		break;
	case S_B_D_ERR:
		if (status_bar_to_display > status_bar_displayed) {
			// display this if higher priority
			status_bar_displayed = status_bar_to_display;
			update = 1;
		}
		break;
	case S_B_D_PERCENT_EDITOR:
		update = 1;
		break;
	}

///mflf_d_printf("SBD: %d,%d [%s]\n", status_bar_displayed, status_bar_to_display, prev_msg);
	if (update) {
///mflf_d_printf("%d:[%s]\n", status_bar_displayed, buffer);
		vsnprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
///mflf_d_printf("msg:[%s]\n", msg);
///mflf_d_printf("[%s]\n", buf);
		switch (status_bar_to_display) {
		default:
		case S_B_D_NONE:
		case S_B_D_PERCENT_FILER:
		case S_B_D_ING:
			// this time: "NEW_MSG"
			strlcpy__(buffer, buf, MAX_SCRN_LINE_BUF_LEN);
			strcpy__(prev_msg, "");
			// next time: "NEW_MSG"
			break;
		case S_B_D_DONE:
		case S_B_D_ERR:
			// this time: "NEW_MSG"
			strlcpy__(buffer, buf, MAX_SCRN_LINE_BUF_LEN);
			strlcpy__(prev_msg, buf, MAX_SCRN_LINE_BUF_LEN);
			// next time: "PREV_MSG || NEW_MSG"
			break;
		case S_B_D_PERCENT_EDITOR:
			// this time: "PREV_MSG || NEW_MSG"
			strcpy__(buffer, "");
			if (strnlen(prev_msg, MAX_SCRN_LINE_BUF_LEN)) {
				strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, prev_msg);
				strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, "  |  ");
			}
			strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, buf);
			strcpy__(prev_msg, "");
			// next time: "NEW_MSG"
			break;
		}
		adjust_utf8s_columns(buffer, main_win_get_columns());
		switch (status_bar_displayed) {
		default:
		case S_B_D_NONE:
		case S_B_D_PERCENT_FILER:
		case S_B_D_ING:
		case S_B_D_DONE:
		case S_B_D_PERCENT_EDITOR:
			color_idx = ITEM_COLOR_IDX_STATUS;
			break;
		case S_B_D_ERR:
			color_idx = ITEM_COLOR_IDX_WARNING;
			break;
		}
		if (tio_is_initialized() == FALSE) {
			e_printf("Terminal I/O not initialized !! ");
			e_vprintf(msg, ap);
		} else {
			set_color_by_idx(color_idx, 0);
			blank_status_bar();
			main_win_output_string(main_win_get_bottom_win_y() + STATUS_LINE, 0, buffer, -1);
			if (divisor > 0) {
				// display percent indicator "bar"
				col_idx = 1 + (main_win_get_columns()-2) * dividend / divisor;
				// col_idx: 1 -- main_win_get_columns()-1
				byte_idx = byte_idx_from_col_idx(buffer, col_idx, CHAR_RIGHT, &col_idx);
				set_color_by_idx(color_idx, 1);
				main_win_output_string(main_win_get_bottom_win_y() + STATUS_LINE, col_idx,
				 &buffer[byte_idx], -1);
			}
		}
///mflf_d_printf("SB: [%s]\n", buffer);
	}

	if (status_bar_to_display == S_B_D_PERCENT_EDITOR) {
		clear_status_bar_displayed();
	}
}

void blank_status_bar(void)
{
	main_win_clear_lines(main_win_get_bottom_win_y() + STATUS_LINE, -1);
}
void blank_yes_no_line(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	main_win_clear_lines(get_yes_no_line_y(), -1);
}

void blank_key_list_lines(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_lines(main_win_get_bottom_win_y() + KEY_LIST_LINE,
	 main_win_get_bottom_win_y() + KEY_LIST_LINE + get_key_list_lines());
}

int get_input_line_y(void)
{
	return main_win_get_mid_win_y() + main_win_get_mid_win_lines() / 2;
}
int get_yes_no_line_y(void)
{
	// if there is key-list-line, input on KEY_LIST_LINE otherwise STATUS_LINE
	return main_win_get_bottom_win_y()
	 + (get_key_list_lines() ? KEY_LIST_LINE : STATUS_LINE);
}

// End of disp.c
/**************************************************************************
 *   editor.c                                                             *
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

#include "headers.h"

editor_quit_t editor_quit = EDITOR_NONE;
PRIVATE int editor_main_loop(void);

int call_editor(int push_win, int list_mode)
{
	app_mode_t appmode_save;
	int ret;

	if (push_win) {
		win_push_win_size();
	}

	memcpy(&appmode_save, &app_mode__, sizeof(app_mode__));
	CLR_APPMD(app_EDITOR_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);
	SET_APPMD_VAL(ed_EDITOR_PANES, 0);
	set_app_func_key_table();

flf_d_printf("push_win:%d, list_mode:%d\n", push_win, list_mode);
flf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n");
	ret = editor_main_loop();
flf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n");
	editor_quit = EDITOR_NONE;

	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));
	SET_APPMD_VAL(app_LIST_MODE, GET_APPMD_PTR(&appmode_save, app_LIST_MODE));
	SET_APPMD_VAL(ed_EDITOR_PANES, GET_APPMD_PTR(&appmode_save, ed_EDITOR_PANES));
	set_app_func_key_table();

	if (push_win) {
		win_pop_win_size();
	}

	return ret;		// 1: input, 0: cancelled
}

//-----------------------------------------------------------------------------
int prev_key_executed;
int key_executed;
char prev_func_id[MAX_PATH_LEN+1];
PRIVATE int editor_main_loop(void)
{
	key_code_t key_input;
	func_key_table_t *func_key_table;

	prev_key_executed = 0;
	key_executed = 0;

	search_clear(&search__);
#ifdef ENABLE_REGEX
	matches_clear(&matches__);
#endif // ENABLE_REGEX

	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL_SOON);

	// Main input loop
	key_input = 0;
	while (1) {
		editor_quit = EDITOR_NONE;
		if (key_macro_is_playing_back()) {
			// When playing back key-macro, do not update screen for speed up.
		} else {
			update_screen_editor(1, key_input >= 0, 1);
		}
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		if (key_input < 0) {
			// no key input
		} else {
mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));

#ifdef ENABLE_REGEX
			matches_clear(&matches__);
#endif // ENABLE_REGEX
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
			memorize_undo_state_before_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
			if ((func_key_table = get_func_key_table_from_key(editor_func_key_table, key_input))
			 == NULL) {
				if (IS_CHAR_KEY(key_input) == 0) {
					disp_status_bar_err(_("Key command not assigned: %04xh"), key_input);
				} else {
					do_enter_char(key_input);
				}
			} else {
				do_enter_char_send();
				if (is_app_list_mode() == 0 || func_key_table->list_mode) {
					search_clear(&search__);
					key_executed = key_input;
flf_d_printf("CALL_EDITOR_FUNC [%s]\n", func_key_table->func_id);
					//=========================
					(*func_key_table->func)();			// call function
					//=========================
					prev_key_executed = key_executed;
					strlcpy__(prev_func_id, func_key_table->func_id, MAX_PATH_LEN);
				}
			}
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
			check_undo_state_after_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
		}
		if (is_app_list_mode() == 0) {
			// normal mode
			if (count_edit_bufs() == 0) {
				// all files closed on edit mode, exit editor
				break;
			}
		}
		if (editor_quit) {
			break;
		}
	}
#ifdef ENABLE_HISTORY
	key_macro_cancel_recording();
#endif // ENABLE_HISTORY
	if (editor_quit == EDITOR_ABORT) {
		return 0;	// cancelled
	}
	return 1;		// selected
}

//-----------------------------------------------------------------------------

PRIVATE int open_file_recursive(int recursive);
int do_open_file(void)
{
	return open_file_recursive(RECURSIVE1);
}
PRIVATE int open_file_recursive(int recursive)
{
	char file_name[MAX_PATH_LEN+1] = "";
	int ret;

	clear_files_loaded();
	while (1) {
#ifdef ENABLE_FILER
		ret = call_filer(1, 0, "", file_name, file_name, MAX_PATH_LEN);
		if (ret > 0)
			break;
#endif // ENABLE_FILER

_FLF_
		ret = input_string("", file_name, HISTORY_TYPE_IDX_DIR, _("Open existing file:"));
_FLF_

		if (ret <= 0) {
			break;
		}
#ifdef ENABLE_FILER
		if (is_path_wildcard(file_name))
			continue;
#endif // ENABLE_FILER
		// CURDIR: changed in editor
		if (load_file_name_upp_low(file_name, TUL0, OOE0, MOE1, recursive) <= 0) {
			tio_beep();
		}
		break;
	}
	disp_files_loaded_ifnon0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_open_new_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_path, HISTORY_TYPE_IDX_DIR, _("Open new file:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	clear_files_loaded();
	// CURDIR: changed in editor
	if (load_file_name_upp_low(file_path, TUL0, OOE1, MOE0, RECURSIVE0) <= 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded_ifnon0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

int do_open_proj_file(void)
{
	char filter[MAX_PATH_LEN+1];
	int loop;
	DIR *dir;
	struct dirent *dirent;
	struct stat st;
	struct stat lst;
	char file_name[MAX_PATH_LEN+1];

	strcpy__(file_name, "");
	dir = opendir(".");
#if 0
	for (loop = 0; loop < 4; loop++) {
		switch(loop) {
		case 0:		strcpy__(filter, PROJ_FILE_FILTER1);	break;
		case 1:		strupper(filter);	break;	// "*.bep" ==> "*.BEP"
		case 2:		strcpy__(filter, PROJ_FILE_FILTER2);	break;
		case 3:		strupper(filter);	break;	// "*.pro" ==> "*.PRO"
		}
		rewinddir(dir);
		while ((dirent = readdir(dir)) != NULL) {
			lstat(dirent->d_name, &lst);
			if (S_ISLNK(lst.st_mode))
				stat(dirent->d_name, &st);
			else
				memcpy__(&st, &lst, sizeof(struct stat));
			if (S_ISREG(st.st_mode) && fnmatch(filter, dirent->d_name, 0) == 0) {
				strlcpy__(file_name, dirent->d_name, MAX_PATH_LEN);
				break;
			}
		}
		if (is_strlen_not_0(file_name)) {
			// file matched
			break;
		}
	}
#else
	for (loop = 0; loop < 2; loop++) {
		rewinddir(dir);
		while ((dirent = readdir(dir)) != NULL) {
			lstat(dirent->d_name, &lst);
			if (S_ISLNK(lst.st_mode))
				stat(dirent->d_name, &st);
			else
				memcpy__(&st, &lst, sizeof(struct stat));
			if (S_ISREG(st.st_mode) && is_file_name_proj_file(dirent->d_name, 1+loop)) {
				strlcpy__(file_name, dirent->d_name, MAX_PATH_LEN);
				break;
			}
		}
		if (is_strlen_not_0(file_name)) {
			// file matched
			break;
		}
	}
#endif
	closedir(dir);

	if (is_strlen_0(file_name)) {
		disp_status_bar_err(_("No project file found"));
		return 0;
	}

	// CURDIR: changed in editor
	if (load_file_name_recurs(file_name, OOE0, MOE1, RECURSIVE1) <= 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

int do_reopen_file_last_line(void)
{
	if (do_reopen_file() == 0) {
		return 0;
	}
	do_last_line();
	return 1;
}
int do_reopen_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	if (check_cur_buf_modified()) {
		ret = ask_yes_no(ASK_YES_NO,
		 _("Buffer is modified, Are you OK to reopen file ?"));
		if (ret <= 0)
			return 0;
	}
///_FLF_
	memorize_cur_file_pos_null(NULL);
///_FLF_
	free_cur_edit_buf();
///_FLF_
	// CURDIR: abs-path is specified
	if (load_file_name_recurs(get_file_line_col_from_str_null(NULL, file_path, NULL, NULL),
	 OOE0, MOE1, RECURSIVE1) <= 0) {
///_FLF_
		tio_beep();
		return 0;
	}
///_FLF_
	goto_str_line_col_in_cur_buf(NULL);
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
	// file was reopened, re-memorize undo state.
	memorize_undo_state_before_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("File re-loaded"));
	return 1;
}

//-----------------------------------------------------------------------------

// | Func name               |files|close|un-modified|ask Y/N|file-name|
// | ----------------------- |-----|-----|-----------|-------|---------|
// | do_write_file_to()      | one |none | Yes       | none  |New-name |write to new file
// | do_write_file_ask()     | one |none | no        | Ask   |cur-name |
// | do_write_file_always()  | one |none | Yes       | none  |cur-name |ask if not modified
// | do_write_all_ask()      | All |none | no        | Ask   |cur-name |
// | do_write_all_modified() | All |none | no        | none  |cur-name |
// | do_close_file_ask()     | one |Close| no        | Ask   |cur-name |
// | do_close_all_ask()      | All |Close| no        | Ask   |cur-name |
// | do_close_all_modified() | All |Close| no        | none  |cur-name |

int do_write_file_to(void)
{
	char file_path[MAX_PATH_LEN+1] = "";
	char org_file_path[MAX_PATH_LEN+1] = "";

	strlcpy__(file_path, get_cep_buf()->file_path, MAX_PATH_LEN);
flf_d_printf("[%s]\n", file_path);
	while (1) {
		if (input_new_file_name_n_ask(file_path) <= 0) {
			return -1;
		}
flf_d_printf("[%s]\n", file_path);
#ifdef ENABLE_FILER
		if (is_path_wildcard(file_path))
			continue;
#endif // ENABLE_FILER
flf_d_printf("[%s]\n", file_path);
		buf_get_file_path(get_cep_buf(), org_file_path);
		buf_set_file_path(get_cep_buf(), file_path);	// set new file name
		if (backup_and_save_cur_buf(file_path) < 0) {
flf_d_printf("[%s]\n", file_path);
			buf_set_file_path(get_cep_buf(), org_file_path);
			return -1;
		}
flf_d_printf("[%s]\n", file_path);
		break;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Written to the file: %s"), file_path);
	return 0;
}
int do_write_file_ask(void)
{
	return write_file_ask(ANSWER_NO, NO_CLOSE_AFTER_SAVE_0);
}
int do_write_file_always(void)
{
	return write_file_ask(ANSWER_FORCE, NO_CLOSE_AFTER_SAVE_0);
}
int do_write_all_ask(void)
{
	memorize_cur_file_pos_null(NULL);
	write_all_ask(ANSWER_NO, NO_CLOSE_AFTER_SAVE_0);
	recall_cur_file_pos_null(NULL);
	return 1;
}
int do_write_all_modified(void)
{
	memorize_cur_file_pos_null(NULL);
	write_all_ask(ANSWER_ALL, NO_CLOSE_AFTER_SAVE_0);
	recall_cur_file_pos_null(NULL);
	return 1;
}

int do_close_file_ask(void)
{
	int ret;

	if (is_app_list_mode()) {
		editor_quit = EDITOR_ABORT;
		return 0;
	}
	ret = write_file_ask(ANSWER_NO, CLOSE_AFTER_SAVE_1);
	if (ret <= ANSWER_CANCEL) {
		// Cancel/Error
		return -1;
	}
	// Yes/No
	if (free_cur_edit_buf() == 0)
		return 1;
	do_refresh_editor();
	disp_status_bar_done(_("One buffer closed"));
	return 2;
}
int do_close_all_ask(void)
{
	if (is_app_list_mode()) {
		editor_quit = EDITOR_ABORT;
		return 0;
	}
	write_close_all(ANSWER_NO);
	return 0;
}
int do_close_all_modified(void)
{
	if (is_app_list_mode()) {
		editor_quit = EDITOR_ABORT;
		return 0;
	}
	write_close_all(ANSWER_ALL);
	return 0;
}

//-----------------------------------------------------------------------------
int do_read_file_into_cur_pos(void)
{
	memorize_cur_file_pos_null(NULL);
	open_file_recursive(RECURSIVE0);
	if (get_files_loaded() <= 0) {
		return 0;
	}
	do_select_all_lines();
	do_copy_text();
	do_close_file_ask();
	recall_cur_file_pos_null(NULL);
	do_paste_text_with_pop();
	return 0;
}
//-----------------------------------------------------------------------------

#ifdef ENABLE_HELP
int do_editor_splash(void)
{
	disp_splash(100);
	input_key_loop();
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	return 0;
}
#endif // ENABLE_HELP

int do_editor_display_color_pairs(void)
{
	display_color_pairs(0, 0);
#ifdef ENABLE_DEBUG
	input_key_loop();
	display_item_colors(0, 0);
	display_bracket_hl_colors(0, 40);
#endif // ENABLE_DEBUG
	return 0;
}

int do_editor_menu_0(void)
{
	editor_menu_n(-1);
	return 0;
}
int do_editor_menu_1(void)
{
	if (is_app_list_mode()) {
		editor_quit = EDITOR_ABORT;
		return 0;
	}
	editor_menu_n(0);
	return 0;
}
int do_editor_menu_2(void)
{
	editor_menu_n(1);
	return 0;
}
int do_editor_menu_3(void)
{
	editor_menu_n(2);
	return 0;
}
int do_editor_menu_4(void)
{
	editor_menu_n(3);
	return 0;
}
int do_editor_menu_5(void)
{
	editor_menu_n(4);
	return 0;
}
int do_editor_menu_6(void)
{
	editor_menu_n(5);
	return 0;
}
int do_editor_menu_7(void)
{
	editor_menu_n(6);
	return 0;
}
int do_editor_menu_8(void)
{
	editor_menu_n(7);
	return 0;
}
int do_editor_menu_9(void)
{
	editor_menu_n(8);
	return 0;
}

//-----------------------------------------------------------------------------

int write_close_all(int yes)
{
	close_all_not_modified();
	if (write_all_ask(yes, CLOSE_AFTER_SAVE_1) < 0)
		return -1;
	close_all();
	return 0;
}
int write_all_ask(int yes, close_after_save_t close)
{
	int ret = yes;

	switch_c_e_b_to_top();
	while (is_c_e_b_valid()) {
		ret = write_file_ask(ret, close);
		if (ret <= ANSWER_CANCEL) {
			disp_status_bar_done(_("Cancelled"));
			return -1;
		}
		if (switch_c_e_b_to_next(0, 0) == 0)
			break;
	}
	disp_status_bar_done(_("All buffers are checked"));
	return 1;
}
int close_all_not_modified(void)
{
	disp_status_bar_ing(_("Freeing buffers..."));
	switch_c_e_b_to_top();
	while (is_c_e_b_valid()) {
		if (check_cur_buf_modified()) {
			if (switch_c_e_b_to_next(0, 0) == 0)
				break;
		} else {
			free_cur_edit_buf();
		}
		tio_refresh();
	}
	return 0;
}
int close_all(void)
{
	disp_status_bar_ing(_("Freeing all buffers..."));
	switch_c_e_b_to_top();
	while (free_cur_edit_buf()) {
		// loop
		tio_refresh();
	}
	return 0;
}

int write_file_ask(int yes, close_after_save_t close)
{
	int ret = yes;

	if (yes < ANSWER_FORCE && check_cur_buf_modified() == 0) {
		clear_cur_buf_modified();	// buffer is acctually NOT modified, so clear "modified" flag
		disp_status_bar_done(_("Buffer is NOT modified"));
		return ANSWER_NO;
	}
	if (yes == ANSWER_FORCE && check_cur_buf_modified() == 0) {
		ret = ask_yes_no(ASK_YES_NO | ASK_ALL, _("Save unmodified buffer ?"));
		if (ret < 0) {
			disp_status_bar_done(_("Cancelled"));
			return ANSWER_CANCEL;
		}
		ret = ANSWER_FORCE;
	}
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(1, 1, 1);
	if (ret < ANSWER_ALL) {
		ret = ask_yes_no(ASK_YES_NO | ASK_ALL,
		 close == 0
		  ? _("Save modified buffer ?")
		  : _("Save modified buffer (ANSWERING \"No\" WILL DISCARD CHANGES) ?"));
		if (ret < 0) {
			disp_status_bar_done(_("Cancelled"));
			return ANSWER_CANCEL;
		}
	}
	if (ret >= ANSWER_YES) {	// Yes
		if (backup_and_save_cur_buf_ask() < 0) {
			return ANSWER_CANCEL;
		}
		disp_status_bar_done(_("Buffer saved to file"));
	}
	return ret;		// all:2, yes:1
}

int do_editor_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
void win_push_win_size(void)
{
	// draw parent screen reversed
	set_work_space_color_low();
	update_screen_app(1, 1, 1);
	set_work_space_color_normal();

	inc_win_depth();
	win_reinit_win_size();
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_screen();		// draw dark frame
	inc_win_depth();
	win_reinit_win_size();
}
void win_pop_win_size(void)
{
	dec_win_depth();
	dec_win_depth();
	win_reinit_win_size();

	// draw parent screen
	update_screen_app(1, 1, 1);
}

int update_screen_app(int title_bar, int status_bar, int refresh)
{
#ifndef ENABLE_FILER
	set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
	update_screen_editor(title_bar, status_bar, refresh);
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
		update_screen_editor(title_bar, status_bar, refresh);
	} else {
		update_screen_filer(title_bar, status_bar, refresh);
	}
#endif // ENABLE_FILER
	return 0;
}

//-----------------------------------------------------------------------------

int update_screen_editor(int title_bar, int status_bar, int refresh)
{
	int pane_sel_idx;		// 0: not current pane, 1: current pane
	int cur_pane_idx;		// pane index
	int pane_idx;			// pane index

///mflf_d_printf("{{{{{{{{{{{{{{{{{{{{{{{{{\n");
///_FLF_
	// title bar
	if (title_bar) {
		disp_editor_title_bar();
	}

	if (GET_APPMD(ed_EDITOR_PANES) == 0) {		// 1 pane
		win_select_win(WIN_IDX_SUB_WHOLE);
		if (get_edit_win_update_needed()) {
			disp_edit_win(1);
		}
	} else {									// 2 panes
		cur_pane_idx = get_editor_cur_pane_idx();
		for (pane_sel_idx = 0; pane_sel_idx < EDITOR_PANES; pane_sel_idx++) {
			// 1st, update not current pane.
			// 2nd, update current pane.
			if (pane_sel_idx == 0) {
				// not current pane
				pane_idx = 1 - cur_pane_idx;	// 0 ==> 1, 1 ==> 0
			} else {
				// current pane
				pane_idx = cur_pane_idx;
			}
			win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
			if (pane_sel_idx == 0) {
				// not current pane
				set_work_space_color_low();
			} else {
				// current pane
				set_work_space_color_normal();
			}
///flf_d_printf("pane_sel_idx: %d, pane_idx: %d\n", pane_sel_idx, pane_idx);
			set_editor_cur_pane_idx(pane_idx);
			if (get_edit_win_update_needed()) {
				disp_edit_win(pane_sel_idx);
			}
		}
	}

	// status bar
	if (status_bar) {
		disp_status_bar_editor();
		// key list
		disp_key_list_editor();
	}

	set_edit_cursor_pos();

	if (refresh) {
		tio_refresh();
	}
	clear_edit_win_update_needed();
///mflf_d_printf("}}}}}}}}}}}}}}}}}}}}}}}}}\n");
	return 0;
}

int disp_status_bar_editor(void)
{
	int bytes, byte_idx;
#define UTF8_CODE_LEN		(17+1+8+1)		// "00-00-00-00-00-00(U+xxxxxx)"
	char buf_char_code[UTF8_CODE_LEN+1];	// "00-00-00-00-00-00(U+xxxxxx)"
	unsigned long xx;
	unsigned long disp_len;
#define SEL_LINES_LEN		(1+4+10+1)		// " LNS:9999999999"
	char buf_lines_sel[SEL_LINES_LEN] = "";
	char buffer[MAX_EDIT_LINE_LEN+1];

	xx = col_idx_from_byte_idx(CEPBV_CL->data, 0, CEPBV_CLBI) + 1;
	disp_len = col_idx_from_byte_idx(CEPBV_CL->data, 0, MAX_EDIT_LINE_LEN) + 1;

	strcpy__(buf_char_code, "");
	bytes = utf8c_bytes(&CEPBV_CL->data[CEPBV_CLBI]);
	for (byte_idx = 0; byte_idx < bytes; byte_idx++) {
		snprintf(&buf_char_code[strnlen(buf_char_code, UTF8_CODE_LEN)], 3+1,
		 byte_idx == 0 ? "%02x" : "-%02x",
		 (unsigned char)CEPBV_CL->data[CEPBV_CLBI + byte_idx]);
	}
#ifdef ENABLE_UTF8
	// show Unicode
	if (bytes >= 2) {
		snprintf(&buf_char_code[strnlen(buf_char_code, UTF8_CODE_LEN)], 8+1, "(U+%04x)",
		 (unsigned int)utf8c_decode(&CEPBV_CL->data[CEPBV_CLBI]));
	}
#endif // ENABLE_UTF8

	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE))) {
		snprintf(buf_lines_sel, SEL_LINES_LEN, " LNS:%2d", lines_selected());
	}

	strcpy__(buffer, "");
	strlcat__(buffer, MAX_EDIT_LINE_LEN,
	 _("LINE:%4lu/%-4lu COLUMN:%3lu/%-3lu SIZE:%6lu%s CODE:%s ENC:%s EOL:%s"));
	disp_status_bar_percent_editor(CEPBV_CL->line_num-1, get_cep_buf()->buf_lines-1,
	 buffer, CEPBV_CL->line_num, get_cep_buf()->buf_lines, xx, disp_len,
	 get_cep_buf()->buf_size, buf_lines_sel, buf_char_code,
	 buf_encode_str(get_cep_buf()), buf_eol_str(get_cep_buf()));
	return 1;
}

void disp_key_list_editor(void)
{
	char *editor_key_lists[] = {
 "{Menu} "
 " {Rec  } {Play } {SchBW} {SchFW} "
 " {Mark } {Cut  } {Copy } {Pop  } "
 " {Paste} {Dupli} {Files} {TagJp}",
 "<do_close_file_ask>Quit "
 "<do_cut_to_head>CutToHead "
 "<do_cut_text>CutLine "
 "<do_cut_to_tail>CutToTail "
 "<do_copy_text>CopyLine "
 "<do_paste_text_with_pop>PasteWPop "
 "<do_paste_text_without_pop>PasteWoPop "
 "<do_duplicate_text>DupLine "
 "<do_first_line>TopOfFile "
 "<do_last_line>BotOfFile "
 "<do_prev_word>PrevWord "
 "<do_next_word>NextWord ",

 "<do_close_all_ask>CloseAll "
 "<do_open_file>OpenFile "
 "<do_write_file_ask>WriteFile "
 "<do_search_backward_first>Search BW "
 "<do_search_forward_first>Search FW "
 "<do_replace>Replace "
 "<do_switch_to_file_list>FileList "
 "<do_switch_to_prev_file>PrevFile "
 "<do_switch_to_next_file>NextFile "
#ifdef ENABLE_EXPERIMENTAL
 "<do_switch_to_prev_buffers>PrevBufs "
 "<do_switch_to_next_buffers>NextBufs "
#endif // ENABLE_EXPERIMENTAL
 "<do_switch_to_key_list>KeyList "
 "<do_switch_to_func_list>FuncList ",
	};
	disp_key_list(editor_key_lists);
}

//-----------------------------------------------------------------------------

int is_app_list_mode(void)
{
	return GET_APPMD(app_LIST_MODE);			// 0: edit mode, 1: list mode
}

int is_view_mode_then_warn_it(void)
{
	if (IS_NODE_TOP_ANCH(get_cep_buf()) || IS_NODE_BOT_ANCH(get_cep_buf())) {
		disp_status_bar_done(_("Modification not allowed in Anchor buffer"));
		return 1;
	}
	if (is_app_list_mode()) {
		disp_status_bar_done(_("Modification not allowed in LIST mode"));
		return 1;
	}
	if (CUR_EBUF_STATE(buf_VIEW_MODE)) {
		disp_status_bar_done(_("Modification not allowed in VIEW mode"));
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_cur_pointers(void)
{
	flf_d_printf("%d:[%s]\n", CEPBV_CL->line_num, CEPBV_CL->data);
	flf_d_printf("CEPBV_CLBI:%d\n", CEPBV_CLBI);
	flf_d_printf("cursor_y:%d\n", CEPBV_CURSOR_Y);
	flf_d_printf("cursor_x_to_keep:%d\n", CEPBV_CURSOR_X_TO_KEEP);
	flf_d_printf("min_text_x_to_keep:%d\n", CEPBV_MIN_TEXT_X_TO_KEEP);
}
#endif // ENABLE_DEBUG

// End of editor.c
/**************************************************************************
 *   editor2.c                                                            *
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

#include "headers.h"

PRIVATE void disp_edit_line(int cur_pane, int yy, const be_buf_t *buf, const be_line_t *line,
 int byte_idx_1, int byte_idx_2);
#ifdef ENABLE_SYNTAX
PRIVATE void disp_edit_line_single_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax);
PRIVATE void disp_edit_line_multi_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax);
#endif // ENABLE_SYNTAX

PRIVATE int output_edit_line_text(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2);
PRIVATE int output_edit_line_num(int yy, const be_buf_t *buf, const be_line_t *line);
PRIVATE int output_edit_line_text__(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2, const char *vis_code);

PRIVATE int get_cursor_x_in_edit_win(void);
PRIVATE int get_cursor_x_in_text(void);
PRIVATE int get_max_text_x_to_be_disp(void);
PRIVATE int get_edit_win_x_in_text(int text_x);
PRIVATE int get_line_num_columns(void);

PRIVATE const char *get_ruler_text(int col_idx);
PRIVATE const char *make_ruler_text__(int col_x, int columns);

#define MAX_LINE_NUM_STR_LEN		10	// 4000000000
PRIVATE const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line,
 char *buf_line_num);
PRIVATE int get_buf_line_num_columns(const be_buf_t *buf);
PRIVATE int get_buf_line_num_digits(const be_buf_t *buf);
PRIVATE int get_line_num_digits(int max_line_num);

								//  0123456789012345678901
#define BUF_BUF_LEN			22	// "Mc Cut00 Undo00 Redo00"
#define MEM_BUF_LEN			8	// "999000Mb" (999Gb)
#define HHCMMCSS_LEN		8	// "23:59:59"
//1:/home/...editor2.c[Modified] Mc Cut00 Undo00 Redo00 123456MB 11:55:04
void disp_editor_title_bar(void)
{
	int buf_idx;
	char *path;
	char separator_char;
	int space;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_status[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_num[2+1];
	int edit_bufs;
	int cut_bufs;
#ifdef ENABLE_UNDO
	int undo_bufs;
	int redo_bufs;
#endif // ENABLE_UNDO
	char buf_buf[BUF_BUF_LEN+1];
#ifdef ENABLE_DEBUG
	char buf_mem[MEM_BUF_LEN+1];
#endif // ENABLE_DEBUG
	char buf_time[HHCMMCSS_LEN+1];

	buf_idx = get_edit_buf_idx_from_buf(get_cep_buf());
	path = get_cep_buf()->abs_path;

	tio_set_cursor_on(0);

	set_title_bar_color_by_state(BUF_STATE(get_cep_buf(), buf_CUT_MODE));
	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0,
	 tio_blank_line(), main_win_get_columns());

	//-------------------------------------------------------------------------
	separator_char = ':';
	if (is_app_list_mode()) {
		separator_char = '.';
	}
#ifdef ENABLE_DEBUG
	if (GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF) {
		separator_char = ';';
		if (is_app_list_mode()) {
			separator_char = ',';
		}
	}
#endif // ENABLE_DEBUG
	snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%s%d%c%d:%s",
	 root_notation(),
	 get_editor_cur_pane_idx()+1, separator_char, buf_idx+1,
	 (path[0] == '\0') ? _("New File") : path);
	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[Modified] "));
	} else if (CUR_EBUF_STATE(buf_VIEW_MODE)) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[VIEW mode] "));
	} else if (is_st_writable(&get_cep_buf()->orig_file_stat) == 0) {
		strlcat__(buf_path, MAX_SCRN_LINE_BUF_LEN, _("[WRITE-PROTECTED] "));
	}

	//-------------------------------------------------------------------------
	buf_buf[0] = '\0';
	// edit buffer cut mode
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, "%s", buf_cut_mode_str(get_cep_buf()));
#ifdef ENABLE_DEBUG
///	// cut buffer cut mode
///	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " %s", buf_cut_mode_str(CUR_CUT_BUF));
#endif // ENABLE_DEBUG
	// edit buffers
	edit_bufs = count_edit_bufs();
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " E%s", nn_from_num(edit_bufs, buf_num));
	// cut buffers
	cut_bufs = count_cut_bufs();
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " C%s", nn_from_num(cut_bufs, buf_num));
#ifdef ENABLE_UNDO
	// undo buffers
	undo_bufs = count_undo_bufs() / 2;
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " U%s", nn_from_num(undo_bufs, buf_num));
	// redo buffers
	redo_bufs = count_redo_bufs() / 2;
	strcat_printf(buf_buf, MAX_SCRN_LINE_BUF_LEN, " R%s", nn_from_num(redo_bufs, buf_num));
#endif // ENABLE_UNDO

#ifdef ENABLE_DEBUG
	// free memory in MB
	snprintf_(buf_mem, MEM_BUF_LEN+1, "%dMb", get_mem_free_in_kb(1)/1000);
#endif // ENABLE_DEBUG

	// current time
	strlcpy__(buf_time, cur_ctime(), HHCMMCSS_LEN);

	//-------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
	snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s %s %s", buf_buf, buf_mem, buf_time);
#else // ENABLE_DEBUG
	snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s %s", buf_buf, buf_time);
#endif // ENABLE_DEBUG
	space = LIM_MIN(0, main_win_get_columns() - strnlen(buf_status, MAX_SCRN_LINE_BUF_LEN) - 1);
	if (space < main_win_get_columns() / 2) {
#ifdef ENABLE_DEBUG
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s %s", buf_mem, buf_time);
#else // ENABLE_DEBUG
		snprintf_(buf_status, MAX_SCRN_LINE_BUF_LEN, "%s", buf_time);
#endif // ENABLE_DEBUG
		space = LIM_MIN(0,
		 main_win_get_columns() - strnlen(buf_status, MAX_SCRN_LINE_BUF_LEN) - 1);
	}
	shrink_str(buf_path, space, 2);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%-*s %s", space, buf_path, buf_status);

	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);

	tio_set_cursor_on(1);
}
//-----------------------------------------------------------------------------
PRIVATE int edit_win_update_needed = UPDATE_SCRN_NONE;
void set_edit_win_update_needed(int update_needed)
{
	if (edit_win_update_needed & (UPDATE_SCRN_PREV | UPDATE_SCRN_NEXT))
		update_needed |= UPDATE_SCRN_ALL;
	edit_win_update_needed |= update_needed;
}
int get_edit_win_update_needed(void)
{
	return edit_win_update_needed;
}
void clear_edit_win_update_needed(void)
{
	edit_win_update_needed = UPDATE_SCRN_NONE;
}

#ifdef ENABLE_REGEX
PRIVATE void disp_edit_win_bracket_hl();
#endif // ENABLE_REGEX

//-111111111111<   |
//22222222222222222|
//-2222#2222222<   |
//33333333333333333|
//-333333333333<   |
//44444444444444444|

// Refresh the editor screen.
void disp_edit_win(int cur_pane)
{
	int buf_idx;
	char buf_path[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_line_num[MAX_LINE_NUM_STR_LEN+1];
	be_line_t *line;
	int byte_idx;
	int byte_idx_1;
	int byte_idx_2;
	int yy;
	int max_wl_idx;
	int wl_idx;
	int cursor_line_text_right_x = -1;

	// Don't make the cursor jump around the screen while updating
	tio_set_cursor_on(0);

	if (edit_win_get_path_lines()) {
		// file path per pane
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, cur_pane);
		sub_win_clear_lines(edit_win_get_path_y(), -1);
		buf_idx = get_edit_buf_idx_from_buf(get_cep_buf());
		snprintf_(buf_path, MAX_SCRN_LINE_BUF_LEN+1, "%d%c%s",
		 buf_idx+1, ':', get_cep_buf()->abs_path);
		shrink_str(buf_path, sub_win_get_columns(), 2);
		sub_win_output_string(edit_win_get_path_y(), 0, buf_path, -1);
	}

	get_cur_screen_top(&line, &byte_idx);
	for (yy = 0; yy < edit_win_get_text_lines(); ) {
		if (IS_NODE_BOT_ANCH(line))
			break;
		te_tab_expand_line(line->data);
		max_wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
///flf_d_printf("[%s]\n", te_line_concat_linefeed);
///flf_d_printf("max_wl_idx: %d\n", max_wl_idx);
		wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, byte_idx, -1);
		for ( ; wl_idx <= max_wl_idx; wl_idx++) {
			byte_idx_1 = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, 0, -1);
			byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_line_concat_linefeed, wl_idx,
			 INT_MAX, -1);
			disp_edit_line(cur_pane, yy, get_cep_buf(), line, byte_idx_1, byte_idx_2);
			if (yy == CEPBV_CURSOR_Y) {
				cursor_line_text_right_x = LIM_MAX(get_edit_win_columns_for_text(),
				 end_col_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, byte_idx_2, -1));
			}
			yy++;
			if (yy >= edit_win_get_text_lines())
				break;
		}
		line = NODE_NEXT(line);
		byte_idx = 0;
	}
	// clear remaining edit-win lines
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_lines(edit_win_get_text_y() + yy,
	 edit_win_get_text_y() + edit_win_get_text_lines());

#ifdef ENABLE_REGEX
#define HL_SEARCH_CURSOR
///
#define HL_SEARCH_OTHER
///
#define HL_BRACKET_BW
///
#define HL_BRACKET_FW
#if defined(HL_BRACKET_BW) || defined(HL_BRACKET_FW)
	if ((search_is_needle_set(&search__) == 2) && cur_pane) {
		disp_edit_win_bracket_hl();
	}
#endif // defined(HL_BRACKET_BW) || defined(HL_BRACKET_FW)
#endif // ENABLE_REGEX

	if (GET_APPMD(ed_SHOW_RULER)) {
		int edit_win_text_x = get_line_num_columns();
		if (GET_APPMD(ed_SHOW_LINE_NUMBER)) {
			// display buffer total lines ("999 ")
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
			sub_win_output_string(edit_win_get_ruler_y(), 0,
			 get_line_num_string(get_cep_buf(), CUR_EDIT_BUF_BOT_NODE, buf_line_num),
			 edit_win_text_x);
		}
		// display ruler("1---5----10---15---20---25---30---35---40---45---50---55---60---65")
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
		sub_win_output_string(edit_win_get_ruler_y(), edit_win_text_x,
		 get_ruler_text(0), -1);
		// display cursor column indicator in reverse text on ruler
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
		sub_win_output_string(edit_win_get_ruler_y(), get_cursor_x_in_edit_win(),
		 get_ruler_text(get_cursor_x_in_text() - get_cep_buf_view_min_text_x_to_keep()), 1);
		// display line tail column indicator in reverse text on ruler
		if (cursor_line_text_right_x >= 0) {
			sub_win_output_string(edit_win_get_ruler_y(),
			 edit_win_text_x + (cursor_line_text_right_x-1 - get_cep_buf_view_min_text_x_to_keep()),
			 get_ruler_text(cursor_line_text_right_x-1 - get_cep_buf_view_min_text_x_to_keep()), 1);
		}
	}

	tio_set_cursor_on(1);
}

//-----------------------------------------------------------------------------

// Just update one line in the edit buffer.
PRIVATE void disp_edit_line(int cur_pane, int yy, const be_buf_t *buf, const be_line_t *line,
 int byte_idx_1, int byte_idx_2)
{
#ifdef ENABLE_SYNTAX
	int syntax_idx;
	const color_syntax_t *clr_syntax = NULL;
#endif // ENABLE_SYNTAX
	int left_byte_idx = 0, right_byte_idx = 0;
	int vis_idx;
	int byte_idx;
	matches_t matches;

///flf_d_printf("%d, [%d, %d]\n", yy, byte_idx_1, byte_idx_2);
///flf_d_printf("[%s]\n", te_line_concat_linefeed);
	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == CEPBV_CL) {
		// highlight current line by painting background
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// First, blank out the line (at a minimum)
	sub_win_clear_lines(edit_win_get_text_y() + yy, -1);

	// output line number -----------------------------------------------------
	if (byte_idx_1 == 0) {
		// first line of line wrapping, display line number
		output_edit_line_num(yy, buf, line);	// "9999 "
	} else {
		output_edit_line_num(yy, buf, NULL);	// "     "
	}

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	if (line == CEPBV_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_CURSOR_LINE, 0);
	}
	// display text simply =====================================================
	output_edit_line_text(yy, line->data, byte_idx_1, byte_idx_2);

#ifdef ENABLE_SYNTAX
	// display syntax color highlighting =======================================
	if (GET_APPMD(ed_SYNTAX_HIGHLIGHT)) {
		// check for all of syntaxes
		for (syntax_idx = 0; syntax_idx < 2; syntax_idx++) {
			if (syntax_idx == 0) {
				// syntax_idx = 0: loop 0
				clr_syntax = get_color_syntax_head();
			} else {
				// clr_syntax == NULL
				// syntax_idx = 1: loop 1
				if (GET_APPMD(ed_TAB_EOL_NOTATION)) {
					clr_syntax = get_default_color_syntax_head();
				}
			}
			for ( ; clr_syntax; clr_syntax = clr_syntax->next) {
///dump_color_syntax(clr_syntax);
				if (clr_syntax->regexp_end == NULL) {
					// single-line regexp
					disp_edit_line_single_line_regexp(yy, line,
					 byte_idx_1, byte_idx_2, clr_syntax);
				} else {
					// multi-line regexp
					disp_edit_line_multi_line_regexp(yy, line,
					 byte_idx_1, byte_idx_2, clr_syntax);
				}
			}
		}
	}
#endif // ENABLE_SYNTAX

#if 1
	// highlight marked segment ================================================
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE))
	 && (mark_min_line->line_num <= line->line_num
	  && line->line_num <= mark_max_line->line_num)) {
		// this line is at least partially selected
		switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
			break;
		case CUT_MODE_N_LINE:
		case CUT_MODE_H_CHAR:
		case CUT_MODE_VH_CHAR:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:
			if (mark_min_line == line && mark_max_line == line) {
				// selection starts and ends within this line
				left_byte_idx = mark_min_byte_idx;
				right_byte_idx = mark_max_byte_idx;
			} else if (mark_min_line->line_num < line->line_num 
			 && line->line_num < mark_max_line->line_num) {
				// whole line is selected
				left_byte_idx = 0;
				right_byte_idx = INT_MAX;
			} else if (mark_min_line->line_num == line->line_num) {
				// selection starts from this line
				left_byte_idx = mark_min_byte_idx;
				right_byte_idx = INT_MAX;
			} else if (line->line_num == mark_max_line->line_num) {
				// selection ends at this line
				left_byte_idx = 0;
				right_byte_idx = mark_max_byte_idx;
			}
			break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:
			// this line is partially selected
			left_byte_idx = byte_idx_from_col_idx(line->data, mark_min_col_idx,
			 CHAR_RIGHT, NULL);
			right_byte_idx = byte_idx_from_col_idx(line->data, mark_max_col_idx,
			 CHAR_LEFT, NULL);
			break;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 left_byte_idx, right_byte_idx, &left_byte_idx, &right_byte_idx) > 0) {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
			output_edit_line_text(yy, te_line_concat_linefeed, left_byte_idx, right_byte_idx);
		}
	}
#endif
#ifdef HL_SEARCH_OTHER
	if (search_is_needle_set(&search__) == 1) {
		// display all text matched in the screen =======================================
		for (byte_idx = 0; byte_idx < byte_idx_2; ) {
			if (search_str_in_line(&search__, &matches, NULL,
			 FORWARD_SEARCH, CASE_SENSITIVE, line->data, byte_idx) == 0) {
				// not found
				break;
			}
			if (get_intersection(byte_idx_1, byte_idx_2,
			 matches_start_idx(&matches), matches_end_idx(&matches),
			 &left_byte_idx, &right_byte_idx) > 0) {
				// match is in the line
				set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
				output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
			}
			byte_idx = matches_end_idx(&matches);
		}
	}
#endif // HL_SEARCH_OTHER
	if (cur_pane) {
#ifdef HL_SEARCH_CURSOR
		if ((search_is_needle_set(&search__) == 1) && (line == CEPBV_CL)) {
			// display matched text at cursor pos ===========================================
			if (search_str_in_line(&search__, &matches, NULL,
			 FORWARD_SEARCH, CASE_SENSITIVE, line->data, CEPBV_CLBI)) {
				// found
				if (matches_start_idx(&matches) == CEPBV_CLBI) {
					if (get_intersection(byte_idx_1, byte_idx_2,
					 CEPBV_CLBI, CEPBV_CLBI + matches_match_len(&matches),
					 &left_byte_idx, &right_byte_idx) > 0) {
						set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, 0);
						output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
					}
				}
			}
		}
#endif // HL_SEARCH_CURSOR
		// draw cursor myself ===========================================================
		if (GET_APPMD(app_DRAW_CURSOR)) {
			set_color_by_idx(ITEM_COLOR_IDX_CURSOR_CHAR, 1);
			vis_idx = vis_idx_from_byte_idx(CEPBV_CL->data, CEPBV_CLBI);
			output_edit_line_text(CEPBV_CURSOR_Y, te_line_visible_code,
			 vis_idx, vis_idx+utf8c_bytes(&te_line_visible_code[vis_idx]));
		}
	}
}

#ifdef ENABLE_SYNTAX
PRIVATE void disp_edit_line_single_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax)
{
	int byte_idx;
	regexp_matches_t regexp_matches;
	int min_byte_idx, max_byte_idx;

////flf_d_printf("[%d, %d]\n", byte_idx_1, byte_idx_2);
	for (byte_idx = 0; byte_idx < byte_idx_2; ) {
		if (regexp_search_compiled(clr_syntax->regexp_start, te_line_concat_linefeed, byte_idx,
		 REG_NONE, &regexp_matches, 1) != 0) {
			// not found
			break;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&regexp_matches, 0),
		 regexp_matches_end_idx(&regexp_matches, 0),
		 &min_byte_idx, &max_byte_idx) > 0) {
			set_item_color(&clr_syntax->color);
			output_edit_line_text(yy, te_line_concat_linefeed, min_byte_idx, max_byte_idx);
		}
		byte_idx = regexp_matches_end_idx(&regexp_matches, 0);
	}
}

#ifdef NOT_DEFINED
// test string of multi region syntax highlighting
// Ex.1
	/* comment */ not-comment /* comment */ not-comment /* comment */ not-comment /* comment */
// Ex.2
	not-comment /* comment */ not-comment /* comment */ not-comment /* comment */
// Ex.3
	/* comment
	   comment */ not-comment /* comment */ not-comment /* comment */ not-comment /* comment
	   comment */
// Ex.4
	/* comment
	   comment
	*/
// Ex.5
	/* comment
	/* comment (Compiler outputs warning error to this NESTED comment. DO NOT care !)
	*/
#endif

PRIVATE void disp_edit_line_multi_line_regexp(int yy, const be_line_t *line,
 int byte_idx_1, int byte_idx_2, const color_syntax_t *clr_syntax)
{
	// This is a multi-line regexp. There are two steps.
	// First step, we have to see if the beginning is there
	// on an earlier line, and an end on this line or later.
	// We find the first line before line_ptr matching the
	// start. If every match on that line is followed by an
	// end, then go to step two.  Otherwise, find the next line
	// after start_line matching the end.  If that line is not
	// before line_ptr, then paint the beginning of this line.
	regexp_matches_t matches_begin;	// match position for regexp_start
	regexp_matches_t matches_end;	// match position for regexp_end
	const be_line_t *start_line;	// the beginning line at which start regexp matched
	int byte_idx;					// where it starts in that line
	int line_cnt;
	int min_byte_idx, max_byte_idx;

	set_item_color(&clr_syntax->color);
	// First, search start or end syntax on earlier lines.
	for (line_cnt = 0, start_line = NODE_PREV(line); ;
	 line_cnt++, start_line = NODE_PREV(start_line)) {
#define MAX_SYNTAX_SEARCH_LINES		LIM_MIN(25, edit_win_get_text_lines() * 2)
		if (line_cnt >= MAX_SYNTAX_SEARCH_LINES || IS_NODE_TOP_ANCH(start_line)) {
			// No syntax found, so skip to the next step.
			goto step_two;
		}
		if (regexp_search_compiled(clr_syntax->regexp_start, start_line->data, 0,
		 REG_NONE, &matches_begin, 1) == 0) {
			// A start found before current line
			break;
		}
		if (regexp_search_compiled(clr_syntax->regexp_end, start_line->data, 0,
		 REG_NONE, &matches_end, 1) == 0) {
			// No start but there is an end before current line
			goto step_two;
		}
	}
	// Now start_line is the first line containing a start match.
	// Is there a start on this line not followed by an end on this line?
	byte_idx = regexp_matches_end_idx(&matches_begin, 0);
	for ( ; ; ) {
		if (regexp_search_compiled(clr_syntax->regexp_end, start_line->data, byte_idx,
		 REG_NONE, &matches_end, 1) != 0)
			// No end found after start on this line,
			// region is continuing to the current line
			break;
		byte_idx = regexp_matches_end_idx(&matches_end, 0);
		if (regexp_search_compiled(clr_syntax->regexp_start, start_line->data, byte_idx,
		 REG_NONE, &matches_begin, 1) != 0)
			// No more start on this line.
			goto step_two;
		byte_idx = regexp_matches_end_idx(&matches_begin, 0);
	}
	// Indeed, there is a start not followed by an end.
	// Region is continuing to the current line.
	if (regexp_search_compiled(clr_syntax->regexp_end, line->data, 0,
	 REG_NONE, &matches_end, 1) == 0) {
		// region is closed in the current line
		//		/*
		//		  comment */				<== current line
		byte_idx = regexp_matches_end_idx(&matches_end, 0);
	} else {
		// region is not closed in the current line
		//		/*
		//		  comment .....				<== current line
		//		*/
		byte_idx = te_line_concat_linefeed_bytes;
	}
	if (get_intersection(byte_idx_1, byte_idx_2, 0, byte_idx,
	 &min_byte_idx, &max_byte_idx) > 0) {
		// Now paint the region.
		output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
	}

step_two:
	// Second step, we look for start in the current line.
	for (byte_idx = 0; byte_idx < te_line_concat_linefeed_bytes; ) {
		if (regexp_search_compiled(clr_syntax->regexp_start, line->data, byte_idx,
		 REG_NONE, &matches_begin, 1) != 0)
			// No more start in the current line.
			break;
		if (regexp_search_compiled(clr_syntax->regexp_end,
		 line->data, regexp_matches_end_idx(&matches_begin, 0),
		 REG_NONE, &matches_end, 1) == 0) {
			// There is an end on this line. Paint the region.
			//		... /* comment */				<== current line
			byte_idx = regexp_matches_end_idx(&matches_end, 0);
		} else {
			//		... /* comment ...				<== current line
			//		*/
			byte_idx = te_line_concat_linefeed_bytes;
		}
		if (get_intersection(byte_idx_1, byte_idx_2,
		 regexp_matches_start_idx(&matches_begin, 0), byte_idx,
		 &min_byte_idx, &max_byte_idx) > 0) {
			output_edit_line_text(yy, line->data, min_byte_idx, max_byte_idx);
		}
		if (byte_idx == te_line_concat_linefeed_bytes) {
			// We painted to the end of the line, so
			// don't bother checking any more starts.
			break;
		}
	}
}
#endif // ENABLE_SYNTAX

#ifdef ENABLE_REGEX
PRIVATE void disp_edit_win_bracket_hl()
{
	char char_under_cursor;
	int search_dir;
	int depth_increase;
	be_line_t *match_line;
	int match_byte_idx;
	be_line_t *line;
	int byte_idx;

	int yy;
	int depth;
	int prev_depth;
	int match_len;
	int skip_here;
	int byte_idx_1;
	int byte_idx_2;
	int max_wl_idx;
	int wl_idx;
	int left_byte_idx, right_byte_idx;
	int safe_cnt = 0;

	int color_idx;
	char color_idx_stack[MAX_BRACKET_NESTINGS];

	char_under_cursor = *CEPBV_CL_CEPBV_CLBI;
	search_dir = setup_bracket_search(char_under_cursor, FORWARD_DIR, &depth_increase, NULL, NULL);
	prepare_colors_for_bracket_hl(MAX_BRACKET_NESTINGS/2);

#ifdef HL_BRACKET_BW
	// draw backward [0, yy] from cursor pos
	match_line = CEPBV_CL;
	match_byte_idx = CEPBV_CLBI;
	line = CEPBV_CL;
	byte_idx = CEPBV_CLBI;
	yy = CEPBV_CURSOR_Y;
	skip_here = 0;
	for (depth = MAX_BRACKET_NESTINGS/2, safe_cnt = 0;
	 ((0 < depth) && (depth < MAX_BRACKET_NESTINGS)) && (safe_cnt < MAX_BRACKET_STEPS);
	 safe_cnt++) {
flf_d_printf("depth: %d\n", depth);
		match_len = search_bracket_in_buffer(&match_line, &match_byte_idx,
		 char_under_cursor, search__.needle, BACKWARD_SEARCH, skip_here,
		 depth_increase, &depth, &prev_depth);
flf_d_printf("depth: %d\n", depth);
		skip_here = 1;
////flf_d_printf("match_len: %d\n", match_len);
		for ( ; yy >= 0; ) {
			if (match_len == 0)
				break;
			te_tab_expand_line(line->data);
			max_wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
			wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, byte_idx, -1);
			for ( ; wl_idx >= 0; wl_idx--) {
				byte_idx_1 = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, 0, -1);
				byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_line_concat_linefeed, wl_idx,
				 INT_MAX, -1);
				if (match_line == line && get_intersection(byte_idx_1, byte_idx_2,
				 match_byte_idx, match_byte_idx + match_len,
				 &left_byte_idx, &right_byte_idx) > 0) {
					set_color_for_bracket_hl(prev_depth); // select color by depth
line_dump_byte_idx(match_line, match_byte_idx);
flf_d_printf("yy: %d\n", yy);
					output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
					match_len = 0;	// clear match_len so that go to next bracket
					break;
				}
				yy--;
				if (yy < 0)
					break;
			}
			if (match_len == 0)
				break;
			line = NODE_PREV(line);
			if (IS_NODE_INT(line) == 0)
				break;
			byte_idx = line_data_strlen(line->data);	// goto the previous line's tail
		}
	}
#endif // HL_BRACKET_BW

	depth_increase = - depth_increase;	// invert increase (-1 ==> 1, 1 ==> -1)
#ifdef HL_BRACKET_FW
	// draw forward [yy, edit_win_get_text_lines()-1] from cursor pos
	match_line = CEPBV_CL;
	match_byte_idx = CEPBV_CLBI;
	line = CEPBV_CL;
	byte_idx = CEPBV_CLBI;
	yy = CEPBV_CURSOR_Y;
	skip_here = 0;
	for (depth = MAX_BRACKET_NESTINGS/2, safe_cnt = 0;
	 ((0 < depth) && (depth < MAX_BRACKET_NESTINGS)) && (safe_cnt < MAX_BRACKET_STEPS);
	 safe_cnt++) {
flf_d_printf("depth: %d\n", depth);
		match_len = search_bracket_in_buffer(&match_line, &match_byte_idx,
		 char_under_cursor, search__.needle, FORWARD_SEARCH, skip_here,
		 depth_increase, &depth, &prev_depth);
flf_d_printf("depth: %d\n", depth);
		skip_here = 1;
////flf_d_printf("match_len: %d\n", match_len);
		for ( ; yy < edit_win_get_text_lines(); ) {
			if (match_len == 0)
				break;
			te_tab_expand_line(line->data);
			max_wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
			wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, byte_idx, -1);
			for ( ; wl_idx <= max_wl_idx; wl_idx++) {
				byte_idx_1 = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, 0, -1);
				byte_idx_2 = end_byte_idx_of_wrap_line_ge(te_line_concat_linefeed, wl_idx,
				 INT_MAX, -1);
				if (match_line == line && get_intersection(byte_idx_1, byte_idx_2,
				 match_byte_idx, match_byte_idx + match_len,
				 &left_byte_idx, &right_byte_idx) > 0) {
					set_color_for_bracket_hl(prev_depth); // select color by depth
line_dump_byte_idx(match_line, match_byte_idx);
flf_d_printf("prev_depth/depth: %d/%d, yy: %d\n", prev_depth, depth, yy);
					output_edit_line_text(yy, line->data, left_byte_idx, right_byte_idx);
					match_len = 0;	// clear match_len so that go to next bracket
					break;
				}
				yy++;
				if (yy >= edit_win_get_text_lines())
					break;
			}
			if (match_len == 0)
				break;
			line = NODE_NEXT(line);
			if (IS_NODE_INT(line) == 0)
				break;
			byte_idx = 0;	// goto the next line's head
		}
	}
#endif // HL_BRACKET_FW
}
#endif // ENABLE_REGEX

// display output routines ----------------------------------------------------
/*
          |min_text_x_to_keep
qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq
rrrrrrrrrr+----------------------------------------------------+rrrrrrrrrrrrrrr
ssOUTPUTss+ssssssssssssssssssssssssssssssssssssssssssssssssssss+sssssssssssssss
tttttttttt|tttttttttttttttttttttttttttttttttttttttttttttttttttt|ttttttttttttttt
uuuuuuuOUTPUT-STRINGuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu|uuuuuuuuuuuuuuu
vvvvvvvvvv|vvvvvvvvvvvOUTPUT_STRINGvvvvvvvvvvvvvvvvvvvvvvvvvvvv|vvvvvvvvvvvvvvv
wwwwwwwwww|wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwOUTPUT_STRINGwwwwwww
xxxxxxxxxx|xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|xxOUTPUTxxxxxxx
yyyyyyyyyy+----------------------------------------------------+yyyyyyyyyyyyyyy
zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz
*/

PRIVATE int output_edit_line_num(int yy, const be_buf_t *buf, const be_line_t *line)
{
	char buf_line_num[MAX_LINE_NUM_STR_LEN+1];

	if (GET_APPMD(ed_SHOW_LINE_NUMBER) == 0) {
		return 0;
	}
	if (line == CEPBV_CL) {
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 1);
	} else {
		set_color_by_idx(ITEM_COLOR_IDX_LINE_NUMBER, 0);
	}
	sub_win_output_string(edit_win_get_text_y() + yy, 0,
	 get_line_num_string(buf, line, buf_line_num), -1);
	return 1;
}

// Set cursor at (cursor_y, cur_line_byte_idx).
void set_edit_cursor_pos(void)
{
	sub_win_set_cursor_pos(edit_win_get_text_y() + CEPBV_CURSOR_Y, get_cursor_x_in_edit_win());
}

PRIVATE int output_edit_line_text(int yy, const char *raw_code, int byte_idx_1, int byte_idx_2)
{
	return output_edit_line_text__(yy, raw_code, byte_idx_1, byte_idx_2, te_line_visible_code);
}
PRIVATE int output_edit_line_text__(int yy, const char *raw_code,
 int byte_idx_1, int byte_idx_2, const char *vis_code)
{
	int wl_idx;
	int left_x;
	int right_x;
	int left_byte_idx;
	int right_byte_idx;
	int left_vis_idx;
	int right_vis_idx;
	int bytes;

	wl_idx = start_wl_idx_of_wrap_line(raw_code, byte_idx_1, -1);
	left_x = start_col_idx_of_wrap_line(raw_code, byte_idx_1, -1);
	right_x = end_col_idx_of_wrap_line(raw_code, wl_idx, byte_idx_2, -1);
	left_x = LIM_MIN(left_x, get_cep_buf_view_min_text_x_to_keep());
	right_x = LIM_MAX(right_x, get_max_text_x_to_be_disp());
	left_byte_idx = end_byte_idx_of_wrap_line_ge(raw_code, wl_idx, left_x, -1);
	right_byte_idx = end_byte_idx_of_wrap_line_le(raw_code, wl_idx, right_x, -1);

	left_x = start_col_idx_of_wrap_line(raw_code, left_byte_idx, -1);
	right_x = end_col_idx_of_wrap_line(raw_code, wl_idx, right_byte_idx, -1);
	left_vis_idx = vis_idx_from_byte_idx(raw_code, left_byte_idx);
	right_vis_idx = vis_idx_from_byte_idx(raw_code, right_byte_idx);
	bytes = right_vis_idx - left_vis_idx;
///flf_d_printf("left_x:%d,right_x:%d,left_vis_idx:%d,right_vis_idx:%d,bytes:%d,[%s]\n",
/// left_x,right_x,left_vis_idx,right_vis_idx,bytes, &vis_code[left_vis_idx]);
	if (bytes > 0) {	// if (bytes <= 0), no output neccesary
///_FLF_
		sub_win_output_string(edit_win_get_text_y() + yy,
		 get_edit_win_x_in_text(left_x), &vis_code[left_vis_idx], bytes);
	}
///_FLF_
	return bytes;
}

PRIVATE int get_cursor_x_in_edit_win(void)
{
	return get_edit_win_x_in_text(get_cursor_x_in_text());
}
PRIVATE int get_cursor_x_in_text(void)
{
	return start_col_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
}
PRIVATE int get_max_text_x_to_be_disp(void)
{
	return get_cep_buf_view_min_text_x_to_keep() + get_edit_win_columns_for_text();
}
PRIVATE int get_edit_win_x_in_text(int text_x)
{
	return get_line_num_columns() + (text_x - get_cep_buf_view_min_text_x_to_keep());
}
// width of text view area (window-width - line-number-width)
int get_edit_win_columns_for_text(void)
{
	// <text-to-edit................................>
	// 999 <text-to-edit............................>
	return sub_win_get_columns() - get_line_num_columns();
}
// text x position (line-number-width)
PRIVATE int get_line_num_columns(void)
{
	return GET_APPMD(ed_SHOW_LINE_NUMBER) == 0
	// ^text-to-edit
	 ? 0
	// 999 ^text-to-edit
	 : get_buf_line_num_columns(get_cep_buf());
}

// ruler ======================================================================

#ifdef START_UP_TEST
void test_make_ruler_text(void)
{
flf_d_printf("[%s]\n", make_ruler_text__(0, 40));
flf_d_printf("[%s]\n", make_ruler_text__(97, 40));
flf_d_printf("[%s]\n", make_ruler_text__(98, 40));
flf_d_printf("[%s]\n", make_ruler_text__(99, 40));
flf_d_printf("[%s]\n", make_ruler_text__(100, 40));
}
#endif // START_UP_TEST

#define RULER_NUM_INTERVAL	5
#define R_N_I				RULER_NUM_INTERVAL
#define MAX_RULER_STR_LEN	(MAX_SCRN_LINE_BUF_LEN + R_N_I)
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
//1---5----10---15---20---25---30---35---40---45---50---55---60---65---70---75---80
//9910-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//----9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//---9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//--9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
PRIVATE const char *get_ruler_text(int col_idx)
{
	const char *str;

	str = make_ruler_text__(get_cep_buf_view_min_text_x_to_keep(), get_edit_win_columns_for_text());
	col_idx = MK_IN_RANGE(0, col_idx, strnlen(str, MAX_RULER_STR_LEN));
	return &str[col_idx];
}
PRIVATE const char *make_ruler_text__(int start_col_idx, int columns)
{
	static int start_col_idx__ = -1;
	static int columns__ = -1;
	static char ruler_line_buf[MAX_RULER_STR_LEN+1] = "";	// ruler line
	int col_num;
	char col_num_str[4+1];	// "9999"

	if (start_col_idx__ != start_col_idx || columns__ != columns) {
		start_col_idx__ = start_col_idx;
		columns__ = columns;

		strcpy__(ruler_line_buf, "");
		// 0, 5, 10, 15, 20, ...
		for (col_num = (start_col_idx / R_N_I) * R_N_I;
		 col_num <= MAX_EDIT_LINE_LEN * get_cur_buf_tab_size();
		 col_num = ((col_num / R_N_I) + 1) * R_N_I) {
			snprintf_(col_num_str, 4+1, "%d", col_num);
			if (col_num - 1 - start_col_idx < 0) {
				// line head
				if (start_col_idx == 0) {
					snprintf_(ruler_line_buf, R_N_I+1, "1----");
				} else {
					snprintf_(ruler_line_buf, R_N_I+1, "-----");
				}
			} else {
				snprintf_(&ruler_line_buf[col_num - 1 - start_col_idx], R_N_I+1,
				 "%s----", col_num_str);
			}
			if (strnlen(ruler_line_buf, MAX_RULER_STR_LEN) >= columns)
				break;
		}
///flf_d_printf("[%s]\n", ruler_line_buf);
		ruler_line_buf[columns] = '\0';
///flf_d_printf("[%s]\n", ruler_line_buf);
	}
	return ruler_line_buf;
}

// line number ================================================================

//   1 #include <stdio.h>
//   2 int main(int argc, char *argv[])
// 998 #include <stdio.h>
// 999 int main(int argc, char *argv[])
//    1 #include <stdio.h>
//    2 int main(int argc, char *argv[])
// 9998 #include <stdio.h>
// 9999 int main(int argc, char *argv[])
//          1 #include <stdio.h>
//          2 int main(int argc, char *argv[])
// 9999999998 #include <stdio.h>
// 9999999999 int main(int argc, char *argv[])

#define MIN_LINE_NUM_DIGITS			3		// 999(1000-1)
#define MIN_LINE_NUM_DIGITS_NUM		1000	// 10 ^ MIN_LINE_NUM_DIGITS
#define MAX_LINE_NUM_DIGITS			10		// 9999999999(10000000000-1)
#define LINE_NUM_SEPARATOR			" "		// "999 "
#define LINE_NUM_SEPARATOR_COLS		1		// strlen(LINE_NUM_SEPARATOR);

PRIVATE const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line,
 char *buf_line_num)
{
	if (line) {
		// "9999 "
		snprintf_(buf_line_num, MAX_LINE_NUM_STR_LEN+1,
		 "%*d" LINE_NUM_SEPARATOR, get_buf_line_num_digits(buf), line->line_num);
	} else {
		// "     "
		snprintf_(buf_line_num, MAX_LINE_NUM_STR_LEN+1,
		 "%*s" LINE_NUM_SEPARATOR, get_buf_line_num_digits(buf), "");
	}
	return buf_line_num;
}
PRIVATE int get_buf_line_num_columns(const be_buf_t *buf)
{
	return get_buf_line_num_digits(buf) + LINE_NUM_SEPARATOR_COLS;		// "123" + " "
}
PRIVATE int get_buf_line_num_digits(const be_buf_t *buf)
{
	return get_line_num_digits(BUF_BOT_NODE(buf)->line_num);
}
PRIVATE int get_line_num_digits(int max_line_num)
{
	int digits;		// 3,    4,     5, ...     10
	long long num;	// 1000, 10000, 100000, ...10000000000

	// 999, 9999, 99999, ... 999999999
	for (digits = MIN_LINE_NUM_DIGITS, num = MIN_LINE_NUM_DIGITS_NUM;
	 digits <= MAX_LINE_NUM_DIGITS;
	 digits++, num *= 10) {
		if (max_line_num < num)
			break;
	}
	return digits;
}

//-----------------------------------------------------------------------------
int edit_win_get_path_lines(void)
{
	if (GET_APPMD(ed_EDITOR_PANES) == 0) {
		return 0;
	} else {
		return 1;
	}
}
int edit_win_get_text_lines(void)
{
	return sub_win_get_lines() - edit_win_get_path_lines() - get_ruler_lines();
}

int edit_win_get_path_y(void)
{
	return 0;
}
int edit_win_get_ruler_y(void)
{
	return edit_win_get_path_y() + edit_win_get_path_lines();
}
int edit_win_get_text_y(void)
{
	return edit_win_get_path_y() + edit_win_get_path_lines() + get_ruler_lines();
}

#define TAB_NOTATION	'>'
#define EOL_NOTATION	'<'
int te_line_concat_linefeed_bytes = 0;							// bytes of (raw_byte + line-feed)
char te_line_concat_linefeed[MAX_EDIT_LINE_LEN * 2 +1];		// raw_byte + line-feed
int te_line_visible_code_columns;							// length of tab-expanded line
char te_line_visible_code[MAX_EDIT_LINE_LEN * MAX_TAB_SIZE +1];	// tab-expanded-visible-code
// tab-expansion
/* T:TAB, C:control-code ZZ:Zenkaku-space, L:'\n' */
/* original:     "TabcdCefghZZijkl" */
/* control_code: "TabcdCefghZZijklL" */
/* visible_code: ">   abcd^?efgh[]ijkl<" */
const char *te_tab_expand_line(const char *original)
{
	int col_idx;
	int bytes;
	const char *orig_ptr;
	char *vis_ptr;
#ifdef ENABLE_SYNTAX
	int notation = 0;		// is visible Tab/EOL/Zenkaku-space notation

	notation = (GET_APPMD(ed_SYNTAX_HIGHLIGHT) && GET_APPMD(ed_TAB_EOL_NOTATION));
#endif // ENABLE_SYNTAX

	te_concat_linefeed(original);
///flf_d_printf("original:%s\n", original);
	orig_ptr = original;
	vis_ptr = te_line_visible_code;
	for (col_idx = 0; *orig_ptr; ) {
		if (*orig_ptr == '\t') {
#ifdef ENABLE_SYNTAX
			*vis_ptr++ = notation ? TAB_NOTATION : ' ';
#else // ENABLE_SYNTAX
			*vis_ptr++ = ' ';
#endif // ENABLE_SYNTAX
			while (++col_idx % priv_tab_size) {
				*vis_ptr++ = ' ';
			}
			orig_ptr++;
		} else if (is_ctrl_char((unsigned char)*orig_ptr)) {
			*vis_ptr++ = '^';
			*vis_ptr++ = (*orig_ptr == 0x7f) ? '?' : *orig_ptr + '@';
			col_idx += 2;
			orig_ptr++;
		} else if ((unsigned char)*orig_ptr < 0x80) {	// ASCII
			*vis_ptr++ = *orig_ptr++;
			col_idx++;
		} else {
#ifdef ENABLE_SYNTAX
			if (notation && strlcmp__(orig_ptr, UTF8_ZEN_SPACE) == 0)
				strcpy__(vis_ptr, UTF8_ZEN_SPACE_NOTATION);
			else
				strlcpy__(vis_ptr, orig_ptr, utf8c_bytes(orig_ptr));
#else // ENABLE_SYNTAX
			strlcpy__(vis_ptr, orig_ptr, utf8c_bytes(orig_ptr));
#endif // ENABLE_SYNTAX
			col_idx += utf8c_columns(orig_ptr);
			bytes = utf8c_bytes(orig_ptr);
			vis_ptr += bytes;
			orig_ptr += bytes;
		}
	}
#ifdef ENABLE_SYNTAX
	*vis_ptr++ = notation ? EOL_NOTATION : ' ';
#else // ENABLE_SYNTAX
	*vis_ptr++ = ' ';
#endif // ENABLE_SYNTAX
	*vis_ptr = '\0';
	te_line_visible_code_columns = col_idx + 1;
	return te_line_visible_code;
}
const char *te_concat_linefeed(const char *original)
{
///flf_d_printf("[%s]\n", original);
	strlcpy__(te_line_concat_linefeed, original, MAX_EDIT_LINE_LEN * 2);
	strlcat__(te_line_concat_linefeed, MAX_EDIT_LINE_LEN * 2, "\n");
	te_line_concat_linefeed_bytes = strnlen(te_line_concat_linefeed, MAX_EDIT_LINE_LEN * 2);
	return te_line_concat_linefeed;
}

// End of editor2.c
/**************************************************************************
 *   editorcut.c                                                          *
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

#include "headers.h"

// CDPP(Copy Delete Paste Pop)
#define CDPP_COPY		0x01					// copy
#define CDPP_DELETE		0x02					// delete
#define CDPP_PASTE		0x04					// paste
#define CDPP_POP		0x08					// pop after paste
#define CDPP_NOPOP		0x00					// no pop after paste
#define CDPP_CUT		(CDPP_COPY | CDPP_DELETE)	// cut(copy and delete)
#define CDPP_DUPLICATE	(CDPP_COPY | CDPP_PASTE)	// duplicate(copy and paste)
#define CDPP_REPLACE	(CDPP_DELETE | CDPP_PASTE)	// replace(delete and paste)

PRIVATE int copy_delete_paste_pop(int cp_del_paste_pop);
PRIVATE int copy_delete_paste_pop__(int cp_del_paste_pop);

PRIVATE int copy_text_to_cut_buf(void);
PRIVATE int delete_text_in_cut_region(void);
PRIVATE int paste_text_from_cut_buf(void);

PRIVATE void copy_region_to_cut_buf(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx,
 int char_cut);
PRIVATE void delete_region(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx);

PRIVATE void copy_rect_region_to_cut_buf(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx);
PRIVATE void delete_rect_region(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx);

PRIVATE int paste_cut_buf_char(void);
PRIVATE int paste_cut_buf_line(void);
PRIVATE int paste_cut_buf_rect(void);

int do_clear_cut_buf(void)
{
	free_all_cut_bufs();
	disp_status_bar_done(_("Cut-buffer cleared"));
	return 1;
}
int do_pop_cut_buf(void)
{
	if (pop_cut_buf() == 0) {
		disp_status_bar_err(_("Cut-buffer empty !!"));
	} else {
		disp_status_bar_done(_("Cut-buffer popped"));
	}
	return 1;
}

int do_select_all_lines(void)
{
	do_first_line();
	do_start_of_line();

	do_set_mark();

	do_last_line();
	do_end_of_line();
	return 1;
}

int do_cut_to_head(void)
{
	do_set_mark();
	do_start_of_line();
	//TTTswap_m_l_b_i_with_c_l_b_i();
	do_cut_text();

	disp_status_bar_done(_("Cut to line head"));
	return 1;
}
int do_cut_to_tail(void)
{
	set_disable_update_min_x_to_keep();
	do_set_mark();
	do_end_of_line();
	//TTTswap_m_l_b_i_with_c_l_b_i();
	do_cut_text();

	disp_status_bar_done(_("Cut to line tail"));
	return 1;
}

int do_tog_mark(void)
{
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set ==> set mark
		do_set_mark();
	} else {
		// mark set ==> clear mark
		do_clear_mark();
	}
	return 1;
}
int do_set_mark(void)
{
	do_set_mark_();

	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Mark Set"));
	return 1;
}
void do_set_mark_(void)
{
	CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
	set_mark_pos();
	setup_cut_region();
}
int do_clear_mark(void)
{
	do_clear_mark_();

	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Mark UNset"));
	return 1;
}
void do_clear_mark_(void)
{
	CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_0_LINE;
	clear_mark_pos();
	setup_cut_region();
}

int do_copy_text(void)
{
	if (copy_delete_paste_pop(CDPP_COPY) <= 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Text Copied to Cut-buffer"));
	return 1;
}

int do_delete_text(void)
{
	if (copy_delete_paste_pop(CDPP_DELETE) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Deleted"));
	return 1;
}

int do_cut_text(void)
{
	if (copy_delete_paste_pop(CDPP_CUT) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Cut to Cut-buffer"));
	return 1;
}

int do_duplicate_text(void)
{
	if (copy_delete_paste_pop(CDPP_DUPLICATE | CDPP_POP) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Duplicated"));
	return 1;
}

int do_paste_text_with_pop(void)
{
	if (copy_delete_paste_pop(
	 IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0
	  ? (CDPP_PASTE | CDPP_POP) : (CDPP_REPLACE | CDPP_POP)) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Pasted with popping Cut-buffer"));
	return 1;
}
int do_paste_text_without_pop(void)
{
	if (copy_delete_paste_pop(
	 IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0
	  ? (CDPP_PASTE | CDPP_NOPOP) : (CDPP_REPLACE | CDPP_NOPOP)) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Pasted without popping Cut-buffer"));
	return 1;
}

PRIVATE int copy_delete_paste_pop(int cp_del_paste_pop)
{
	int ret;

	ret = copy_delete_paste_pop__(cp_del_paste_pop);
	do_clear_mark_();	// always clear mark
	return ret;
}
PRIVATE int copy_delete_paste_pop__(int cp_del_paste_pop)
{
	be_line_t *top_line;

///_FLF_
	if (cp_del_paste_pop & CDPP_REPLACE) {
		// in view mode, DELETE and PASTE may not be performed
		if (is_view_mode_then_warn_it())
			return -1;		// error
	}
	if ((cp_del_paste_pop & CDPP_COPY) == 0
	 && (cp_del_paste_pop & CDPP_PASTE)) {
		if (count_cut_bufs() == 0) {
			disp_status_bar_err(_("Cut-buffer empty !!"));
			return -1;		// error
		}
	}

	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set, setup cut-region
		set_mark_pos();
		setup_cut_region();
	}
	top_line = NODE_PREV(mark_min_line);
///_D_(line_dump_lines(top_line, 10, NULL));
	// ====  COPY  ====
	if (cp_del_paste_pop & CDPP_COPY) {
		copy_text_to_cut_buf();
	}
#ifdef ENABLE_UNDO
	if (((cp_del_paste_pop & CDPP_DELETE) && is_there_cut_region())
	 || ((cp_del_paste_pop & CDPP_PASTE) && count_cut_bufs())) {
		// buffer will be modified
		undo_set_region_save_before_change(mark_min_line, mark_max_line,
		 count_cur_cut_buf_lines());
	}
#endif // ENABLE_UNDO
	// ====  DELETE  ====
	if (cp_del_paste_pop & CDPP_DELETE) {
		delete_text_in_cut_region();
	}
	// ====  PASTE  ====
	if (cp_del_paste_pop & CDPP_PASTE) {
		if (count_cut_bufs() == 0) {
			disp_status_bar_err(_("Cut-buffer empty !!"));
		} else {
			paste_text_from_cut_buf();
#ifdef ENABLE_UNDO
///PPP			undo_adjust_max_line();
#endif // ENABLE_UNDO
		}
	}

	clear_disable_update_min_x_to_keep();

	if (cp_del_paste_pop & CDPP_REPLACE) {
		switch(CUR_CBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
			post_cmd_processing(top_line, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		case CUT_MODE_H_CHAR:
		case CUT_MODE_VH_CHAR:
			post_cmd_processing(top_line, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		case CUT_MODE_N_LINE:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:
			post_cmd_processing(top_line, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:
			post_cmd_processing(top_line, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		}
	}
	// ====  POP  ====
	if (cp_del_paste_pop & CDPP_POP) {
		pop_cut_buf();
	}
	return 1;		// done
}

//-----------------------------------------------------------------------------

PRIVATE int copy_text_to_cut_buf(void)
{
	int char_cut;

	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
		case CUT_MODE_N_LINE:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:
			char_cut = 0;
			break;
		case CUT_MODE_VH_CHAR:
		case CUT_MODE_H_CHAR:
			char_cut = 1;
			break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:
			char_cut = 0;
			break;
		}
		copy_region_to_cut_buf(
		 mark_min_line, mark_min_byte_idx,
		 mark_max_line, mark_max_byte_idx,
		 char_cut);
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		copy_rect_region_to_cut_buf(
		 mark_min_line, mark_min_col_idx,
		 mark_max_line, mark_max_col_idx);
		break;
	}
	return 1;
}
PRIVATE int delete_text_in_cut_region(void)
{
	set_cur_buf_modified();
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		delete_region(
		 mark_min_line, mark_min_byte_idx,
		 mark_max_line, mark_max_byte_idx);
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		delete_rect_region(
		 mark_min_line, mark_min_col_idx,
		 mark_max_line, mark_max_col_idx);
		break;
	}
	return 1;
}
PRIVATE int paste_text_from_cut_buf(void)
{
	switch(CUR_CBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
		paste_cut_buf_char();
		break;
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		paste_cut_buf_line();
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		paste_cut_buf_rect();
		break;
	}
	return 1;
}

//-------------------------------------------------------------------------------------------------

// Ex.1
//  aaaaAAAAAAAAAAAAAaaaa
// Ex.2
//  aaaaAAAAAAAAAAAAAAAAA
//  BBBBbbbbbbbbbbbbbbbbb
PRIVATE void copy_region_to_cut_buf(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx,
 int char_cut)
{
	be_line_t *line;
	char buf[MAX_EDIT_LINE_LEN+1];

	if (min_line == max_line && min_byte_idx == max_byte_idx)
		return;

	push_cut_buf();
	for (line = min_line; IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (line != max_line) {
			// first and intermediate line
////_D_(line_dump_byte_idx(line, min_byte_idx));
			append_string_to_cur_cut_buf(
			 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, line_data_len(line)));
		} else {
			// last line
			if (char_cut ? (min_byte_idx <= max_byte_idx) : (min_byte_idx < max_byte_idx)) {
				append_string_to_cur_cut_buf(
				 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, max_byte_idx));
			}
			break;
		}
		min_byte_idx = 0;
	}
}

// Ex.1
//  [Before]
//  aaaa<BBBBBBBBBBBBB>cccccc
//  [After]
//  aaaacccccc
// Ex.2
//  [Before]
//  aaaaaaaaaaaaaaaaaaaaa
//  <BBBBBBBBBBBBBBBBBBBBB>
//  ccccccccccccccccccccc
//  [After]
//  aaaaaaaaaaaaaaaaaaaaa
//  
//  ccccccccccccccccccccc
// Ex.3
//  [Before]
//  aaaaaaaaaaaaaaaaaaaaa
//  <BBBBBBBBBBBBBBBBBBBBB
//  >ccccccccccccccccccccc
//  [After]
//  aaaaaaaaaaaaaaaaaaaaa
//  ccccccccccccccccccccc
// Ex.4
//  [Before]
//  aaaa<AAAAAAAAAAAAAAAAAAAAA
//  BBBBBBBBBBBBBBBBBBBBBBBBBB
//  CCCCCCCCCCCCCCCCCCCCCCCCCC
//  DDDDDDDDDDDDDDDDDDD>dddddd
//  [After]
//  aaaadddddd
PRIVATE void delete_region(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx)
{
	be_line_t *line, *next;

	if (min_line == max_line && min_byte_idx == max_byte_idx)
		return;

	// adjust current line
	CEPBV_CL = min_line;
	CEPBV_CLBI = min_byte_idx;
	for (line = min_line; ; line = next) {
		next = NODE_NEXT(line);
		if (line == min_line) {
			if (line == max_line) {
				// first and last line
				// >aaaaDDDDbbbb
				line_delete_string(line, min_byte_idx, max_byte_idx - min_byte_idx);
				// >aaaabbbb
				break;
			} else {
				// first line
				// >aaaaDDDD
				//  DDDDDDDD
				//  DDDDbbbb
				line_delete_string(line, min_byte_idx, line_data_len(line) - min_byte_idx);
				// >aaaa
				//  DDDDDDDD
				//  DDDDbbbb
			}
		} else {
			if (line != max_line) {
				// intermediate line ==> delete whole line
				//  aaaa
				// >DDDDDDDD
				//  DDDDbbbb
				line_unlink_free(line);
				//  aaaa
				// >DDDDbbbb
			} else {
				// last line
				//  aaaa
				// >DDDDbbbb
				line_delete_string(line, min_byte_idx, max_byte_idx - min_byte_idx);
				//  aaaa
				// >bbbb
				line_concat_with_next(min_line);
				// >aaaabbbb
				break;
			}
		}
		min_byte_idx = 0;
	}
}

PRIVATE void copy_rect_region_to_cut_buf(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx)
{
	be_line_t *line;
	int min_byte_idx, max_byte_idx;
	char buf[MAX_EDIT_LINE_LEN+1];

	if (min_col_idx == max_col_idx)
		return;

	push_cut_buf();
	for (line = min_line; ; line = NODE_NEXT(line)) {
		min_byte_idx = byte_idx_from_col_idx(line->data, min_col_idx, CHAR_RIGHT, NULL);
		max_byte_idx = byte_idx_from_col_idx(line->data, max_col_idx, CHAR_LEFT, NULL);
		append_string_to_cur_cut_buf(
		 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, max_byte_idx));
		if (line == max_line)
			break;		// copy till the selection bottom ?
	}
}

PRIVATE void delete_rect_region(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx)
{
	be_line_t *line;
	int min_byte_idx, max_byte_idx;

	if (min_col_idx == max_col_idx)
		return;

	for (line = min_line; ; line = NODE_NEXT(line)) {
		min_byte_idx = byte_idx_from_col_idx(line->data, min_col_idx, CHAR_RIGHT, NULL);
		max_byte_idx = byte_idx_from_col_idx(line->data, max_col_idx, CHAR_LEFT, NULL);
		if (CEPBV_CL == line) {
			CEPBV_CLBI = min_byte_idx;
		}
		get_cep_buf()->buf_size -= (max_byte_idx - min_byte_idx);
		line_delete_string(line, min_byte_idx, max_byte_idx - min_byte_idx);
		if (line == max_line) {
			break;		// delete till the selection bottom ?
		}
	}
}

//-----------------------------------------------------------------------------
// paste data into edit buffer streamlike
// Ex.
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   aaaaAAAA
//   BBBB^bbbb
PRIVATE int paste_cut_buf_char(void)
{
	be_line_t *inserted_line;
	int cur_byte_idx;
	be_line_t *cut_line;

	set_cur_buf_modified();

	cur_byte_idx = CEPBV_CLBI;
	cut_line = CUR_CUT_BUF_TOP_NODE;
	// Paste the first line of the cut-buffer
	// >aaaa^bbbb
	inserted_line = line_separate(CEPBV_CL, cur_byte_idx, INSERT_BEFORE);
	//  aaaa
	// >bbbb
	line_insert_with_string(CEPBV_CL, INSERT_BEFORE, cut_line->data);
	//  aaaa
	//  AAAA
	// >bbbb
	line_concat(inserted_line, WITH_NEXT);
	//  aaaaAAAA
	// >bbbb
	for ( ; ; ) {
		cut_line = NODE_NEXT(cut_line);
		if (IS_NODE_BOT_ANCH(cut_line))
			break;
		inserted_line = line_insert_with_string(CEPBV_CL, INSERT_BEFORE, cut_line->data);
		//  aaaaAAAA
		//  BBBB
		// >bbbb
		CEPBV_CURSOR_Y++;
	}
	CEPBV_CLBI = line_data_len(inserted_line);
	line_concat(CEPBV_CL, WITH_PREV);
	//  aaaaAAAA
	// >BBBB^bbbb
	return 1;		// pasted
}
// paste cut-buffer data line by line
// Ex.
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   AAAA
//   BBBB
//   aaaa^bbbb
PRIVATE int paste_cut_buf_line(void)
{
	be_line_t *cut_line;

	set_cur_buf_modified();

	for (cut_line = CUR_CUT_BUF_TOP_NODE; IS_NODE_INT(cut_line); ) {
		line_insert_with_string(CEPBV_CL, INSERT_BEFORE, cut_line->data);
		cut_line = NODE_NEXT(cut_line);
		if (IS_MARK_SET(CUR_CBUF_STATE(buf_CUT_MODE))) {
			// marked cut/copy
			CEPBV_CURSOR_Y++;
		}
	}
	if (IS_MARK_SET(CUR_CBUF_STATE(buf_CUT_MODE)) == 0) {
		// unmarked cut/copy
		CEPBV_CL = NODE_PREV(CEPBV_CL);
		CEPBV_CLBI = LIM_MAX(CEPBV_CLBI, line_data_len(CEPBV_CL));	// limit cursor pos
	}
	return 1;		// pasted
}
// paste data into edit buffer rectangular
// Ex.1
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//   ccccdddd
//  [AFTER]
//   aaaaAAAAbbbb
//   ccccBBBB^dddd
// Ex.2
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   aaaaAAAAbbbb
//   BBBB^
PRIVATE int paste_cut_buf_rect(void)
{
	int cur_line_col_idx;
	be_line_t *cut_line;

	set_cur_buf_modified();

	cur_line_col_idx = col_idx_from_byte_idx(CEPBV_CL->data, 0, CEPBV_CLBI);
	for (cut_line = CUR_CUT_BUF_TOP_NODE; IS_NODE_INT(cut_line); ) {
		if (IS_NODE_BOT_ANCH(CEPBV_CL)) {
			// if no more lines in edit buffer, append line automatically
			CEPBV_CL = line_insert_with_string(CEPBV_CL, INSERT_BEFORE, "");
		}
		CEPBV_CLBI = byte_idx_from_col_idx(CEPBV_CL->data, cur_line_col_idx,
		 CHAR_LEFT, NULL);
		line_replace_string(CEPBV_CL, CEPBV_CLBI, 0, cut_line->data, -1);
		CEPBV_CLBI += line_data_len(cut_line);
		cut_line = NODE_NEXT(cut_line);
		if (IS_NODE_BOT_ANCH(cut_line))
			break;
		CEPBV_CL = NODE_NEXT(CEPBV_CL);
	}
	return 1;		// pasted
}

// End of editorcut.c
/**************************************************************************
 *   editorcut2.c                                                         *
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

#include "headers.h"

// mark beginning position

// cut region (minimum)
be_line_t *mark_min_line;		// the line top of the marked area
int mark_min_byte_idx;			// byte_idx in the line top of the marked area
int mark_min_col_idx;			// col_idx left most in the marked area
// cut region (maximum)
be_line_t *mark_max_line;		// the line bottom of the marked area
int mark_max_byte_idx;			// byte_idx in the line bottom of the marked area
int mark_max_col_idx;			// col_idx right most in the marked area

//-----------------------------------------------------------------------------
void clear_mark_pos(void)
{
	CEPB_ML = BUF_TOP_ANCH(get_cep_buf());
	CEPB_MLBI = 0;
}
void set_mark_pos(void)
{
	CEPB_ML = CEPBV_CL;
	CEPB_MLBI = CEPBV_CLBI;
}
// CEPB_MLBI <==> CEPBV_CLBI
///void swap_m_l_b_i_with_c_l_b_i(void)
///{
///	int c_l_b_i;
///
///	if (CEPB_ML == CEPBV_CL) {
///		c_l_b_i = CEPB_MLBI;
///		CEPB_MLBI = CEPBV_CLBI;
///		CEPBV_CLBI = c_l_b_i;
///	}
///}

PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t cursor_move);
PRIVATE void change_cut_mode_on_mark_region_special_cases(void);

void setup_cut_region_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set
		return;
	}
	change_cut_mode_after_cursor_horiz_vert_move(cursor_move);
	setup_cut_region();
	change_cut_mode_on_mark_region_special_cases();
	setup_cut_region();	// cut-mode may be changed, setup mark region again
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}
// [cut-mode transition]
//
// CUT_MODE_0_LINE:
// CUT_MODE_N_LINE:
//  (no move)
//
// CUT_MODE_H_CHAR:
//  ====>
//
// CUT_MODE_HV_LINE:
// CUT_MODE_HV_BOX:
//  ====>
//      |
//      v
//
// CUT_MODE_H_CHAR:
//  ====>
//      |
//      v
//      ====>
//
// CUT_MODE_V_LINE:
//  |
//  v
//
// CUT_MODE_VH_CHAR:
// CUT_MODE_VH_BOX:
//  |
//  v
//  ====>
//
// CUT_MODE_V_LINE:
//  |
//  v
//  ====>
//      |
//      v
//
PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t cursor_move)
{
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
		break;
	case CUT_MODE_N_LINE:
		if (cursor_move == CURS_MOVE_HORIZ) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (cursor_move == CURS_MOVE_VERT) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	case CUT_MODE_H_CHAR:
		if (cursor_move == CURS_MOVE_HORIZ) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (cursor_move == CURS_MOVE_VERT) {
#ifdef HV_IS_BOX_VH_IS_CHAR
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_BOX;
#else
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
#endif
		}
		break;
	case CUT_MODE_HV_LINE:
		if (cursor_move == CURS_MOVE_HORIZ) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (cursor_move == CURS_MOVE_VERT) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
		}
		break;
	case CUT_MODE_V_LINE:
		if (cursor_move == CURS_MOVE_HORIZ) {
#ifdef HV_IS_BOX_VH_IS_CHAR
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_CHAR;
#else
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
#endif
		} else if (cursor_move == CURS_MOVE_VERT) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	case CUT_MODE_VH_CHAR:
		if (cursor_move == CURS_MOVE_HORIZ) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_CHAR;
		} else if (cursor_move == CURS_MOVE_VERT) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	case CUT_MODE_HV_BOX:
		if (cursor_move == CURS_MOVE_HORIZ) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (cursor_move == CURS_MOVE_VERT) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_BOX;
		}
		break;
	case CUT_MODE_VH_BOX:
		if (cursor_move == CURS_MOVE_HORIZ) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
		} else if (cursor_move == CURS_MOVE_VERT) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	}
}
PRIVATE void change_cut_mode_on_mark_region_special_cases(void)
{
	if (CEPBV_CL == CEPB_ML) {
		// current == begin
		mark_min_line = CEPBV_CL;
		mark_max_line = CEPBV_CL;
		if (CEPBV_CLBI == CEPB_MLBI) {
			// the same line, the same column
			// change cut-mode to initial mode
///			// cut-mode is not initial mode, set to initial mode
///			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
		} else {
			// the same line, different column
			switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_LINE:
			case CUT_MODE_N_LINE:
			case CUT_MODE_H_CHAR:
			case CUT_MODE_VH_CHAR:
				break;
			case CUT_MODE_V_LINE:
			case CUT_MODE_HV_LINE:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
				break;
			case CUT_MODE_HV_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
				break;
			case CUT_MODE_VH_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
				break;
			}
		}
	}
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		// different line, the same column
		if (col_idx_from_byte_idx(mark_min_line->data, 0, mark_min_byte_idx)
		 == col_idx_from_byte_idx(mark_max_line->data, 0, mark_max_byte_idx)) {
			switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_LINE:
			case CUT_MODE_N_LINE:
			case CUT_MODE_H_CHAR:
			case CUT_MODE_HV_LINE:
			case CUT_MODE_V_LINE:
			case CUT_MODE_VH_CHAR:
				break;
			case CUT_MODE_HV_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
				break;
			case CUT_MODE_VH_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
				break;
			}
			break;
		}
	}
}
void setup_cut_region(void)
{
	int mark_min_line_col_idx;
	int mark_max_line_col_idx;

	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
		if (IS_NODE_BOT(CEPBV_CL)) {
			// No next line
			// {aaaaaaaaaaaaaaaaaa}
			mark_min_line = CEPBV_CL;
			mark_max_line = CEPBV_CL;
			mark_min_byte_idx = 0;
			mark_max_byte_idx = line_data_len(CEPBV_CL);
		} else {
			// There is next line
			// {aaaaaaaaaaaaaaaaaaa
			// }bbbbbbbbbbbbbbbbbbb
			mark_min_line = CEPBV_CL;
			mark_max_line = NODE_NEXT(CEPBV_CL);
			mark_min_byte_idx = 0;
			mark_max_byte_idx = 0;
		}
		break;
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		if (CEPBV_CL == CEPB_ML) {
			// current == begin
			mark_min_line = CEPBV_CL;
			mark_max_line = CEPBV_CL;
			if (CEPBV_CLBI == CEPB_MLBI) {
				// the same line, the same column
				mark_min_byte_idx = CEPBV_CLBI;
				mark_max_byte_idx = CEPBV_CLBI;
			} else {
				// the same line, different column
				if (CEPBV_CLBI < CEPB_MLBI) {
					mark_min_byte_idx = CEPBV_CLBI;
					mark_max_byte_idx = CEPB_MLBI;
				} else {
					mark_min_byte_idx = CEPB_MLBI;
					mark_max_byte_idx = CEPBV_CLBI;
				}
			}
		} else {
			// different line
			if (CEPB_ML->line_num < CEPBV_CL->line_num) {
				// begin < current
				mark_min_line = CEPB_ML;
				mark_min_byte_idx = CEPB_MLBI;
				mark_max_line = CEPBV_CL;
				mark_max_byte_idx = CEPBV_CLBI;
			} else {
				// current < begin
				mark_min_line = CEPBV_CL;
				mark_min_byte_idx = CEPBV_CLBI;
				mark_max_line = CEPB_ML;
				mark_max_byte_idx = CEPB_MLBI;
			}
		}
		break;
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		if (CEPBV_CL == CEPB_ML) {
			// current == begin
			mark_min_line = CEPBV_CL;
			mark_max_line = CEPBV_CL;
		} else {
			// different line
			if (CEPB_ML->line_num < CEPBV_CL->line_num) {
				// begin < current
				mark_min_line = CEPB_ML;
				mark_max_line = CEPBV_CL;
			} else {
				// current < begin
				mark_min_line = CEPBV_CL;
				mark_max_line = CEPB_ML;
			}
		}
		mark_min_byte_idx = 0;
		mark_max_byte_idx = 0;
		break;
	}

	// setup mark_min_col_idx, mark_max_col_idx
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		mark_min_col_idx = 0;	// not used
		mark_max_col_idx = 0;	// not used
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		mark_min_line_col_idx = col_idx_from_byte_idx(mark_min_line->data, 0, mark_min_byte_idx);
		mark_max_line_col_idx = col_idx_from_byte_idx(mark_max_line->data, 0, mark_max_byte_idx);
		if (mark_min_line_col_idx <= mark_max_line_col_idx) {
			mark_min_col_idx = mark_min_line_col_idx;
			mark_max_col_idx = mark_max_line_col_idx;
		} else {
			// swap left and right
			mark_min_col_idx = mark_max_line_col_idx;
			mark_max_col_idx = mark_min_line_col_idx;
		}
		break;
	}
flf_d_printf("CEPB_ML: [%s]\n", CEPB_ML->data);
flf_d_printf("CEPB_MLBI: %d\n", CEPB_MLBI);
flf_d_printf("CEPBV_CL: [%s]\n", CEPBV_CL->data);
flf_d_printf("CEPBV_CLBI: %d\n", CEPBV_CLBI);
flf_d_printf("mark_min_line: [%s]\n", mark_min_line->data);
flf_d_printf("mark_min_byte_idx: %d\n", mark_min_byte_idx);
flf_d_printf("mark_max_line: [%s]\n", mark_max_line->data);
flf_d_printf("mark_max_byte_idx: %d\n", mark_max_byte_idx);
flf_d_printf("mark_min_col_idx: %d\n", mark_min_col_idx);
flf_d_printf("mark_max_col_idx: %d\n", mark_max_col_idx);
}

int is_there_cut_region(void)
{
	return (mark_max_line - mark_min_line) | (mark_max_byte_idx - mark_min_byte_idx);
}

int lines_selected(void)
{
	int lines = abs(CEPB_ML->line_num - CEPBV_CL->line_num);
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		lines++;
		break;
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		break;
	}
	return lines;
}

// End of editorcut2.c
/**************************************************************************
 *   editorgoto.c                                                         *
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

#include "headers.h"

PRIVATE int load_files_in_cur_buf(void);
PRIVATE int load_files_in_string(const char *string, int files_to_load, int try_upp_low,
 int open_on_err, int msg_on_err, int recursive);

PRIVATE int load_file_name__(const char *file_name, int open_on_err, int msg_on_err);

PRIVATE const char *skip_n_file_names(const char *line, int field_idx);
#ifdef ENABLE_HISTORY
PRIVATE void goto_pos_by_history(const char *full_path);
#endif // ENABLE_HISTORY

// 123
// file.ext:123:45
int do_goto_input_line(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;
	int line_num;

	ret = input_string("", string, HISTORY_TYPE_IDX_CURSPOS, _("Enter line number:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (sscanf(string, "%d", &line_num) > 0) {
		// go to line
		goto_line_col_in_cur_buf(line_num, 1);
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
		return 1;
	}
	// go to file
	// CURDIR: changed in editor
	load_files_in_string(string, 1, TUL0, OOE0, MOE1, RECURSIVE0);	// file.ext:123:45
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return 1;
}
// do_tag_jump() and change directory
int do_goto_file_in_cur_line(void)
{
	char dir_save[MAX_PATH_LEN+1];
	int ret;

////flf_d_printf("[%s]\n", CEPBV_CL->data);
	memorize_cur_file_pos_null(NULL);
	clear_files_loaded();
	save_change_cur_dir(dir_save, strip_file_from_path(get_cep_buf()->abs_path));
	// CURDIR: changed to cur-file's abs-dir
	ret = load_files_in_string(CEPBV_CL->data, 10, TUL1, OOE0, MOE1, RECURSIVE1);
	change_cur_dir(dir_save);
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return 1;
}
int do_goto_directory_in_cur_line(void)
{
#ifdef ENABLE_FILER
	char file_path[MAX_PATH_LEN+1];
	if (get_file_line_col_from_str_null(CEPBV_CL->data, file_path, NULL, NULL) == 0) {
		return 0;
	}
	return filer_change_dir_parent(file_path) == 0;
#endif // ENABLE_FILER
}

int do_open_files_in_buf(void)
{
	clear_files_loaded();
	load_files_in_cur_buf();
	disp_files_loaded();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
int do_switch_to_top_file(void)
{
	if (switch_c_e_b_to_top() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int do_switch_to_bot_file(void)
{
	if (switch_c_e_b_to_bot() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
//-----------------------------------------------------------------------------
int do_switch_to_prev_file(void)
{
	int ret;

	if ((ret = switch_c_e_b_to_prev(1, 0)) == 0) {
		disp_status_bar_err(_("No previous open files"));
		return ret;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Previous file"));
	return ret;
}
int do_switch_to_next_file(void)
{
	int ret;

	if ((ret = switch_c_e_b_to_next(1, 0)) == 0) {
		disp_status_bar_err(_("No next open files"));
		return ret;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Next file"));
	return ret;
}
#ifdef ENABLE_EXPERIMENTAL
int do_switch_to_prev_buffers(void)
{
	be_bufs_t *bufs = get_bufs_contains_buf(&bufs_top_anchor, get_cep_buf());
	if (IS_NODE_TOP(bufs))
		return 0;
flf_d_printf("bufs: %s\n", bufs->name);
flf_d_printf("NODE_PREV(bufs)->name: %s\n", NODE_PREV(bufs)->name);
flf_d_printf("NODE_PREV(bufs)->cur_buf->name: %s\n", NODE_PREV(bufs)->cur_buf->file_path);
	set_cep_buf(NODE_PREV(bufs)->cur_buf);
	post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int do_switch_to_next_buffers(void)
{
	be_bufs_t *bufs = get_bufs_contains_buf(&bufs_top_anchor, get_cep_buf());
	if (IS_NODE_BOT(bufs))
		return 0;
flf_d_printf("bufs: %s\n", bufs->name);
flf_d_printf("NODE_NEXT(bufs)->name: %s\n", NODE_NEXT(bufs)->name);
flf_d_printf("NODE_NEXT(bufs)->cur_buf->name: %s\n", NODE_NEXT(bufs)->cur_buf->file_path);
	set_cep_buf(NODE_NEXT(bufs)->cur_buf);
	post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
#endif // ENABLE_EXPERIMENTAL
//-----------------------------------------------------------------------------
int do_return_to_prev_file_pos(void)
{
	recall_cur_file_pos_null(NULL);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	do_refresh_editor();
	disp_status_bar_done(_("Returned to previous pos"));
	return 1;
}
//-----------------------------------------------------------------------------
int do_switch_editor_pane(void)
{
	do_switch_editor_pane_();
///_FLF_
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
///_FLF_
	return 1;
}
void do_switch_editor_pane_(void)
{
	int pane_idx;			// pane index

	pane_idx = get_editor_cur_pane_idx() ? 0 : 1;
flf_d_printf("pane_idx: %d\n", pane_idx);
	set_editor_cur_pane_idx(pane_idx);
}

//-----------------------------------------------------------------------------

int load_files_in_cur_buf(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	int lines;
	char dir_save[MAX_PATH_LEN+1];
	int files = 0;

	memorize_cur_file_pos_null(file_pos_str);
	first_line();
	clear_handler_sigint_called();
#define MAX_LINES_TO_TRY_TO_LOAD		10000
#define MAX_FILES_TO_LOAD				2000
#define MIN_FREE_MEM_KB					(100 * 1000)	// 100 MB
	for (lines = 0; lines < MAX_LINES_TO_TRY_TO_LOAD; lines++) {
		if (is_handler_sigint_called())
			break;
		if (line_data_len(CEPBV_CL)) {
			if (CEPBV_CL->data[0] != '#') {
				memorize_cur_file_pos_null(NULL);
				save_change_cur_dir(dir_save, strip_file_from_path(get_cep_buf()->abs_path));
				// CURDIR: changed to cur-file's abs-dir
				files += load_files_in_string(CEPBV_CL->data, 10, TUL1, OOE0, MOE0, RECURSIVE0);
				change_cur_dir(dir_save);
				disp_editor_title_bar();
				tio_refresh();
				recall_cur_file_pos_null(NULL);
			}
		}
		if (cursor_next_line() == 0)
			break;
		if (files >= MAX_FILES_TO_LOAD)
			break;
		if (get_mem_free_in_kb(1) <= MIN_FREE_MEM_KB)
			break;
	}
	recall_cur_file_pos_null(file_pos_str);
	return files;
}

//=============================================================================

// supported formats:
//  ##						// ignore any string
//  #fileio.h				// file name in comment
//  #include <fileio.h>		// C include directive
//  fileio.h:10				// GCC error message
//  fileio.h,10				// C compiler error message
//  fileio.h(10)			// M$ C compiler error message
//  #:fileio.h,10			// gettext *.po file
//  fileio.h 10:10			// file-name line-num:col-num
//  11/04/13 16:40:00, fileio.h:10:10,	// event log file
//  <location filename="fileio.h" line="10"/>	// Qt-lupdate
//  diff fileio.h fileio.h~
//  SOURCES += fileio.h
PRIVATE int load_files_in_string(const char *string,
 int files_to_load, int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	int field_idx;
	int files_loaded = 0;
	int files;
	const char *ptr;

////flf_d_printf("[%s]\n", string);
	for (field_idx = 0; files_loaded < files_to_load; field_idx++) {
		ptr = skip_n_file_names(string, field_idx);
		if (*ptr == '\0')
			break;
		files = load_file_in_string(ptr, try_upp_low, open_on_err, msg_on_err, recursive);
		if (files > 0) {
			files_loaded++;
			open_on_err = 0;
			msg_on_err = 0;
		}
////flf_d_printf("files_loaded: %d\n", files_loaded);
	}
////flf_d_printf("files_loaded: %d\n", files_loaded);
	return files_loaded;
}
int load_file_in_string(const char *string,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;
	int files;

///flf_d_printf("string:[%s]\n", string);
	if (get_file_line_col_from_str_null(string, file_path, &line_num, &col_num) == 0) {
///_FLF_
		return 0;
	}
///flf_d_printf("file_path:[%s]\n", file_path);
	if ((files = load_file_name_upp_low(file_path,
	 try_upp_low, open_on_err, msg_on_err, recursive)) > 0) {
///flf_d_printf("loaded:[%s]\n", file_path);
		goto_line_col_in_cur_buf(line_num, col_num);
	}
	return files;
}
int load_file_name_upp_low(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	char file_name_buf[MAX_PATH_LEN+1];
	int files;

	// try to open specified file name (FileName.Ext)
	if ((files = load_file_name_recurs(file_name, open_on_err, msg_on_err, recursive)) > 0)
		return files;
	if (try_upp_low) {
		strlcpy__(file_name_buf, file_name, MAX_PATH_LEN);
		// try to open in upper case file name (FILENAME.EXT)
		strupper(file_name_buf);
		if ((files = load_file_name_recurs(file_name_buf, open_on_err, msg_on_err, recursive)) > 0)
			return files;
		// try to open in lower case file name (filename.ext)
		strlower(file_name_buf);
		if ((files = load_file_name_recurs(file_name_buf, open_on_err, msg_on_err, recursive)) > 0)
			return files;
	}
///	return files;	// 0
	return 0;	// 0
}
// Open file. If it is a project file, open file(s) described in it.
int load_file_name_recurs(const char *file_name, int open_on_err, int msg_on_err, int recursive)
{
	static int recursive_call_count = 0;
	int files;

///flf_d_printf("[%s], %d, %d, %d\n", file_name, open_on_err, msg_on_err, recursive);
	if (load_file_name__(file_name, open_on_err, msg_on_err) <= 0) {
		return 0;
	}
///_FLF_
	files = 1;
	if (recursive && recursive_call_count == 0 && is_file_name_proj_file(file_name, 0)) {
///flf_d_printf("recursive_call_count:%d\n", recursive_call_count);
		recursive_call_count++;
		files += load_files_in_cur_buf();		// recursive call
		recursive_call_count--;
	}
	add_files_loaded(1);
	return files;
}
PRIVATE int load_file_name__(const char *file_name, int open_on_err, int msg_on_err)
{
	char abs_path[MAX_PATH_LEN+1];

///flf_d_printf("[%s]\n", file_name);
	get_abs_path(file_name, abs_path);
///flf_d_printf("[%s]\n", abs_path);
	if (switch_c_e_b_to_file_name(abs_path)) {
///flf_d_printf("already loaded:[%s]\n", abs_path);
		// already loaded
		return 1;
	}
	if (load_file_into_new_buf(abs_path, open_on_err, msg_on_err) < 0) {
		return 0;
	}
///flf_d_printf("loaded:[%s]\n", abs_path);
#ifdef ENABLE_HISTORY
	goto_pos_by_history(abs_path);
#endif // ENABLE_HISTORY
	return 1;
}
//-----------------------------------------------------------------------------
int switch_c_e_b_to_file_name(const char *file_name)
{
	char abs_path[MAX_PATH_LEN+1];

	get_abs_path(file_name, abs_path);
	return switch_c_e_b_to_abs_path(abs_path);
}
int switch_c_e_b_to_abs_path(const char *abs_path)
{
	be_buf_t *buf;

	buf = get_edit_buf_from_abs_path(abs_path);
	if (buf) {
		set_cep_buf(buf);
		return 1;	// switched
	}
	return 0;		// not found
}
//-----------------------------------------------------------------------------
int switch_c_e_b_to_top(void)
{
	if (IS_NODE_BOT_ANCH(EDIT_BUFS_TOP_NODE))
		return 0;
	set_cep_buf(EDIT_BUFS_TOP_NODE);
	return 1;
}
int switch_c_e_b_to_bot(void)
{
	if (IS_NODE_TOP_ANCH(EDIT_BUFS_BOT_NODE))
		return 0;
	set_cep_buf(EDIT_BUFS_BOT_NODE);
	return 1;
}
int switch_c_e_b_to_prev(int beep_at_end, int goto_bottom)
{
	if (IS_NODE_TOP(get_cep_buf())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_cep_buf(NODE_PREV(get_cep_buf()));
	if (goto_bottom) {
		CEPBV_CL = CUR_EDIT_BUF_BOT_NODE;
	}
	return 1;
}
int switch_c_e_b_to_next(int beep_at_end, int goto_top)
{
	if (IS_NODE_BOT(get_cep_buf())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_cep_buf(NODE_NEXT(get_cep_buf()));
	if (goto_top) {
		CEPBV_CL = CUR_EDIT_BUF_TOP_NODE;
	}
	return 1;
}
int switch_c_e_b_to_valid_buf(void)
{
	if (IS_NODE_VALID(get_cep_buf()) == 0) {
		return switch_c_e_b_to_another_buf();
	}
	return 1;
}
int switch_c_e_b_to_another_buf(void)
{
	if (switch_c_e_b_to_next(0, 0) == 0) {
		if (switch_c_e_b_to_prev(0, 0) == 0) {
			set_cep_buf(EDIT_BUFS_TOP_ANCH);
			return 0;
		}
	}
	return 1;
}
//-----------------------------------------------------------------------------
int is_file_name_proj_file(const char *file_name, int type)
{
	if ((type == 0 || type == 1)
	 && strnlen(file_name, MAX_PATH_LEN) >= strlen(PROJ_FILE_EXTENSION1)) {
		if (strcasecmp(
		 &file_name[strnlen(file_name, MAX_PATH_LEN) - strlen(PROJ_FILE_EXTENSION1)],
	     PROJ_FILE_EXTENSION1) == 0) {
			return 1;
		}
	}
	if ((type == 0 || type == 2)
	 && strnlen(file_name, MAX_PATH_LEN) >= strlen(PROJ_FILE_EXTENSION2)) {
		if (strcasecmp(
		 &file_name[strnlen(file_name, MAX_PATH_LEN) - strlen(PROJ_FILE_EXTENSION2)],
	     PROJ_FILE_EXTENSION2) == 0) {
			return 1;
		}
	}
	return 0;	// not project file
}
PRIVATE const char *skip_n_file_names(const char *line, int field_idx)
{
	const char *ptr;
	int field_cnt;

////
flf_d_printf("[%s], %d\n", line, field_idx);
	ptr = line;
	for (field_cnt = 0; ; field_cnt++) {
		ptr = skip_to_file_path(ptr);
		if (*ptr == '\0')
			break;		// EOL
		if (field_cnt >= field_idx)
			break;
		const char *prev_ptr = ptr;
		ptr = skip_file_path(ptr);
		if (ptr == prev_ptr)
			// not progressed
			break;
	}
////
flf_d_printf("file: [%s]\n", ptr);
	return ptr;
}

#ifdef START_UP_TEST
void test_get_n_th_file_name(void)
{
	char test_str[] =
	 "history.c 345 hist_type_idx:3:['/home/user/ filename including space .txt '|1:1]";
	int field_idx;
	const char *ptr;
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

_FLF_
	for (field_idx = 0; field_idx < 10; field_idx++) {
		ptr = skip_n_file_names(test_str, field_idx);
		if (*ptr == '\0')
			break;
		if (get_file_line_col_from_str_null(ptr, file_path, &line_num, &col_num)) {
			flf_d_printf("%d: file_path:[%s],%d,%d\n", field_idx, file_path, line_num, col_num);
		}
	}
}
#endif // START_UP_TEST

//-----------------------------------------------------------------------------
#ifdef ENABLE_HISTORY
PRIVATE void goto_pos_by_history(const char *full_path)
{
	const char *str;

	// search in history
	str = search_history_file_path(HISTORY_TYPE_IDX_CURSPOS, full_path);
	// get line-num and col-num
	if (goto_str_line_col_in_cur_buf(str)) {
		EPBVX_CL(0) = EPBVX_CL(1) = CEPBV_CL;
		EPBVX_CLBI(0) = EPBVX_CLBI(1) = CEPBV_CLBI;
	}
}
#endif // ENABLE_HISTORY
//-----------------------------------------------------------------------------
PRIVATE char memorized_file_pos_str[MAX_PATH_LEN+1];
char *memorize_cur_file_pos_null(char *buffer)
{
	buffer = buffer ? buffer : memorized_file_pos_str;
	mk_cur_file_pos_str(buffer);
////flf_d_printf("buffer: [%s]\n", buffer);
	return buffer;
}

int recall_cur_file_pos_null(const char *str)
{
	char file_path[MAX_PATH_LEN+1];

	if (switch_c_e_b_to_file_name(get_file_line_col_from_str_null(
	 str, file_path, NULL, NULL)) == 0) {
		return 0;
	}
	goto_str_line_col_in_cur_buf(str);
	return 1;
}
int goto_str_line_col_in_cur_buf(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	get_file_line_col_from_str_null(str, file_path, &line_num, &col_num);
	return goto_line_col_in_cur_buf(line_num, col_num);
}
int goto_line_col_in_cur_buf(int line_num, int col_num)
{
////flf_d_printf("%d, %d\n", line_num, col_num);
	if (line_num <= 0) {
		return 0;
	}
	CEPBV_CL = get_line_ptr_from_cur_buf_line_num(line_num);
	if (col_num <= 0) {
		return 1;
	}
#ifdef CURSOR_POS_BYTE
	// col_num is byte count
	CEPBV_CLBI = byte_idx_from_byte_idx(CEPBV_CL->data, col_num-1);
#endif // CURSOR_POS_BYTE
#ifdef CURSOR_POS_COLUMN
	// col_num is column in view
	CEPBV_CLBI = byte_idx_from_col_idx(CEPBV_CL->data, col_num-1, CHAR_LEFT, NULL);
#endif // CURSOR_POS_COLUMN
	return 2;
}
//-----------------------------------------------------------------------------
char *mk_cur_file_pos_str_static(void)
{
	static char buffer[MAX_PATH_LEN+1];
	return mk_cur_file_pos_str(buffer);
}
char *mk_cur_file_pos_str(char *buffer)
{
#ifdef CURSOR_POS_BYTE
	// memorize byte number
	return mk_file_pos_str(buffer, get_cep_buf()->file_path, CEPBV_CL->line_num,
	 CEPBV_CL->data ? byte_idx_from_byte_idx(CEPBV_CL->data, CEPBV_CLBI)+1 : 0);
#endif // CURSOR_POS_BYTE
#ifdef CURSOR_POS_COLUMN
	// memorize column number
	return mk_file_pos_str(buffer, get_cep_buf()->file_path, CEPBV_CL->line_num,
	 CEPBV_CL->data ? col_idx_from_byte_idx(CEPBV_CL->data, 0, CEPBV_CLBI)+1 : 0);
#endif // CURSOR_POS_COLUMN
}
char *mk_file_pos_str(char *buffer, const char *file_path, int line_num, int col_num)
{
	file_path = quote_file_name(file_path);
	if (col_num <= 0) {
		if (line_num <= 0) {
			// /path/to/file.ext
			snprintf_(buffer, MAX_PATH_LEN+1, "%s", file_path);
		} else {
			// /path/to/file.ext|999
			snprintf_(buffer, MAX_PATH_LEN+1, "%s%s%d", file_path, FILE_PATH_SEPARATOR, line_num);
		}
	} else {
		// /path/to/file.ext|999:99
		snprintf_(buffer, MAX_PATH_LEN+1, "%s%s%d:%d",
		 file_path, FILE_PATH_SEPARATOR, line_num, col_num);
	}
	return buffer;
}
//-----------------------------------------------------------------------------
PRIVATE int get_file_line_col_from_str(const char *str, char *file_path,
 int *line_num_, int *col_num_);
const char *get_file_line_col_from_str_null(const char *str, char *file_path,
 int *line_num, int *col_num)
{
	str = str ? str : memorized_file_pos_str;
	get_file_line_col_from_str(str, file_path, line_num, col_num);
	return file_path;
}
// /home/user/tools/be/src/editorgoto.c|400:10
//  => "/home/user/tools/be/src/editorgoto.c", 400, 10
// '/home/user/tools/be/src/ file name.txt '|400:10
//  => "/home/user/tools/be/src/ file name.txt ", 400, 10
PRIVATE int get_file_line_col_from_str(const char *str, char *file_path,
 int *line_num_, int *col_num_)
{
	const char *ptr;
	const char *fn_begin;
	const char *fn_end;
	int line_num;
	int col_num;

	strcpy__(file_path, "");
	line_num = 0;
	col_num = 0;

	ptr = skip_to_file_path(str);
	if (is_file_path_char(ptr) == 0)
		goto no_file_path;
	fn_begin = ptr;
	ptr = skip_file_path(ptr);
	fn_end = ptr;
	strlcpy__(file_path, fn_begin, LIM_MAX(MAX_PATH_LEN, fn_end - fn_begin));
	unquote_string(file_path);
	// skip to beginning of a line number
	ptr = skip_to_digit(ptr);
	if (isdigit(*ptr)) {
		line_num = atoi(ptr);
	}
	ptr = skip_digits(ptr);
	// skip to beginning of a column number
	ptr = skip_to_digit(ptr);
	if (isdigit(*ptr)) {
		col_num = atoi(ptr);
	}
no_file_path:;
	if (line_num_)
		*line_num_ = line_num;
	if (col_num_)
		*col_num_ = col_num;
///
flf_d_printf("str:[%s] ==> path:[%s] line:%d col:%d\n",
///
 str, file_path, line_num, col_num);
	return strnlen(file_path, MAX_PATH_LEN);
}

// supported file names:
//  |No.| file type                                                | command line | file list |
//  |---|----------------------------------------------------------|--------------|-----------|
//  | 1 |" file name.txt "(includes spaces in head, middle or tail)| supported    | supported |
//  | 2 |'"filename".txt' (includes '"')                           | supported    | supported |
//  | 3 |"file|name.txt"  (includes special chars [|'])            | supported    | supported |
// workaround:
//  |No.| command line      | file list                 | project file             |
//  |---|-------------------|---------------------------|--------------------------|
//  | 1 | " file name.txt " | " file name.txt ",100,10  | 100,10," file name.txt " |
//  | 2 | "filename".txt    | "\"filename.txt\"",100,10 | 100,10,"\"filename.txt\""|
//  | 2 | "file|name.txt"   | "file|name.txt",100,10    | 100,10,"file|name.txt"   |
//  | 2 | "file'name.txt"   | "file'name.txt",100,10    | 100,10,"file|name.txt"   |

// End of editorgoto.c
/**************************************************************************
 *   editorkeys.c                                                         *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
 *   Copyright (C) 2010-2018 PCR                                          *
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

#include "headers.h"

func_key_table_t editor_func_key_table[] = {
//  1234567890123456   123456789012345678901234567890   123456789012345678901234567890
//      12345678901234567890
 { "",                "File Menu",                      K_ESC,        NKA, NKA,
   1,F_I(do_editor_menu_1), NULL },
 { "Prev.Menu",       "Previous Menu",                  K_M_ESC, K_M_BACKQUOTE, NKA,
   0,F_I(do_editor_menu_0), NULL },
 { "Open File",       "Open file",                      K_M_e,        NKA, NKA,
   0,F_I(do_open_file), NULL },
 { "Open New File",   "Open new file",                  K_M_E, K_M_n,      NKA,
   0,F_I(do_open_new_file), NULL },
 { "Reopen",          "Reopen file",                    K_C_Z,        NKA, NKA,
   0,F_I(do_reopen_file), NULL },
 { "Reopen, Last L.", "Reopen file and go to Last line",NKA,          NKA, NKA,
   0,F_I(do_reopen_file_last_line), NULL },
 { "Write File as",   "Write file as",                  K_M_s,        NKA, NKA,
   0,F_I(do_write_file_to), NULL },
 { "Write File",      "Write file",                     K_M_w,        NKA, NKA,
   0,F_I(do_write_file_ask), NULL },
 { "Write File Alwy.","Write file always",              K_M_W,        NKA, NKA,
   0,F_I(do_write_file_always), NULL },
 { "Write All",       "Write all files",                K_M_a,        NKA, NKA,
   0,F_I(do_write_all_ask), NULL },
 { "Write All Modif.","Write all files modified",       K_M_A, K_M_S,      NKA,
   0,F_I(do_write_all_modified), NULL },
 { "Close",           "Close file",                     K_C_Q,        NKA, NKA,
   1,F_I(do_close_file_ask), NULL },
 { "Close All inter.","Close all files",                K_M_q,        NKA, NKA,
   1,F_I(do_close_all_ask), NULL },
 { "Close All Modif.","Close all files modified",       K_M_Q,        NKA, NKA,
   1,F_I(do_close_all_modified), NULL },
 { "Open Files",      "Open files in buffer",           K_M_L,        NKA, NKA,
   0,F_I(do_open_files_in_buf), NULL },
 { "Read into buffer","Read file into current pos",     K_M_i,        NKA, NKA,
   0,F_I(do_read_file_into_cur_pos), NULL },

 { "",                "Cursor Menu",                    NKA,          NKA, NKA,
   1,F_I(do_editor_menu_2), NULL },
 { "Up",              "Move cursor UP",                 K_C_E, NKA,   K_UP,
   1,F_I(do_up), NULL },
 { "Down",            "Move cursor DOWN",               K_C_X, NKA,   K_DOWN,
   1,F_I(do_down), NULL },
 { "Back",            "Move cursor LEFT",               K_C_S, NKA,   K_LEFT,
   1,F_I(do_left), NULL },
 { "Forward",         "Move cursor RIGHT",              K_C_D, NKA,   K_RIGHT,
   1,F_I(do_right), NULL },
 { "Prev Page",       "Page UP",                        K_C_R, NKA,   K_PPAGE,
   1,F_I(do_page_up), NULL },
 { "Next Page",       "Page DOWN",                      K_C_C, NKA,   K_NPAGE,
   1,F_I(do_page_down), NULL },
 { "Prev Word",       "Move backward one word",         K_C_A,        NKA, NKA,
   1,F_I(do_prev_word), NULL },
 { "Next Word",       "Move forward one word",          K_C_F,        NKA, NKA,
   1,F_I(do_next_word), NULL },
 { "Start of line",   "Start of the line",              K_C_T, NKA,   K_HOME,
   1,F_I(do_start_of_line), NULL },
 { "End of line",     "End of the line",                K_C_B, K_C_V, K_END,
   1,F_I(do_end_of_line), NULL },
 { "First line",      "Top of the file",                K_M_t,        NKA, NKA,
   1,F_I(do_first_line), NULL },
 { "Last line",       "Bottom of the file",             K_M_b,        NKA, NKA,
   1,F_I(do_last_line), NULL },
 { "Go To Line",      "Go to specified line",           K_C_UNDERLINE,NKA, NKA,
   0,F_I(do_goto_input_line), NULL },
 { "TagJump",         "Tagged jump",                    K_M_j, K_F12,      NKA,
   1,F_I(do_goto_file_in_cur_line), NULL },
 { "Change directory", "Goto directory",                K_M_J, K_SF12,     NKA,
   1,F_I(do_goto_directory_in_cur_line), NULL },

 { "",                "Edit Menu",                      NKA,          NKA, NKA,
   0,F_I(do_editor_menu_3), NULL },
 { "Tab",             "Insert a TAB character",         K_TAB,        NKA, NKA,
   0,F_I(do_tab), NULL },
 { "Enter",           "Insert a carriage return",       K_C_M, NKA,   K_ENTER,
   1,F_I(do_carriage_return), NULL },
 { "Backspace",       "Backspace",                      K_C_H, NKA,   K_BS,
   0,F_I(do_backspace), NULL },
 { "Delete",          "Delete a character",             K_C_G, K_DEL, K_DC,
   0,F_I(do_delete_char), NULL },
 { "UpperLower",      "Invert upper/lower letter",      K_C_W,        NKA, NKA,
   0,F_I(do_conv_upp_low_letter), NULL },
 { "Ctrl code",       "Input control code",             K_C_AT, K_M_9,     NKA,
   0,F_I(do_control_code), NULL },
 { "Char code",       "Input character code",           K_M_AT, K_M_0,     NKA,
   0,F_I(do_charcode), NULL },
 { "Paste history",   "Paste from history",             K_M_h,        NKA, NKA,
   0,F_I(do_paste_history), NULL },
 { "Record/Stop-rec", "Start/Stop recording key-macro", K_F01, K_M_r     , NKA,
   0,F_I(do_start_rec__cancel_rec), NULL },
 { "End-rec/Playback","End-rec/Playback key-macro",     K_F02, K_M_p     , NKA,
   0,F_I(do_end_rec__playback), NULL },
 { "Playback",        "Playback key-macro",             K_M_2,         NKA, NKA,
   0,F_I(do_playback), NULL },
 { "Playback-1",      "Playback 1st newest key-macro",  K_SF01,        NKA, NKA,
   0,F_I(do_playback_last_1), NULL },
 { "Playback-2",      "Playback 2nd newest key-macro",  K_SF02,        NKA, NKA,
   0,F_I(do_playback_last_2), NULL },
#ifdef ENABLE_FILER
 { "Run line",        "Run current line as command",    K_M_x,        NKA, NKA,
   0,F_I(do_run_line), NULL },
#endif // ENABLE_FILER
#ifdef ENABLE_UNDO
 { "Undo",            "Undo",                           K_M_u, K_M_z,      NKA,
   0,F_I(do_undo), NULL },
 { "Redo",            "Redo",                           K_M_o,        NKA, NKA,
   0,F_I(do_redo), NULL },
#endif // ENABLE_UNDO

 { "",                "Search/Replace Menu",            NKA,          NKA, NKA,
   0,F_I(do_editor_menu_4), NULL },
 { "Search Bw",       "Search keyword forward",         K_SF03, K_C_RBRACKET, NKA,
   0,F_I(do_search_backward_first), NULL },
 { "Search Fw",       "Search keyword backward",        K_SF04, K_C_BACKSLASH,  NKA,
   0,F_I(do_search_forward_first), NULL },
 { "Srch Nx Bw",      "Search keyword backward again",  K_F03,        NKA, NKA,
   0,F_I(do_search_backward_next), NULL },
 { "Srch Nx Fw",      "Search keyword forward again",   K_F04,        NKA, NKA,
   0,F_I(do_search_forward_next), NULL },
 { "Replace",         "Replace keyword",                K_C_CARET,    NKA, NKA,
   0,F_I(do_replace), NULL },
#ifdef ENABLE_REGEX
 { "Find Bracket()",  "Find counterpart bracket()",     K_M_RBRACKET, K_M_LBRACE, K_M_BACKSLASH,
   0,F_I(do_find_bracket), NULL },
 { "Find Bracket)(",  "Find counterpart bracket)(",     K_M_LBRACKET, K_M_RBRACE, K_M_VERTBAR,
   0,F_I(do_find_bracket_reverse), NULL },
#endif // ENABLE_REGEX

 { "",                "Buffer Menu",                    NKA,          NKA, NKA,
   0,F_I(do_editor_menu_5), NULL },
 { "Previous File",   "Open previous file",             K_M_COMMA,    NKA, NKA,
   1,F_I(do_switch_to_prev_file), NULL },
 { "Next File",       "Open next file",                 K_M_PERIOD,   NKA, NKA,
   1,F_I(do_switch_to_next_file), NULL },
 { "Top File",        "Open top file",                  K_M_LESSTHAN, NKA, NKA,
   0,F_I(do_switch_to_top_file), NULL },
 { "Bottom File",     "Open bottom file",               K_M_GREATERTHAN, NKA, NKA,
   0,F_I(do_switch_to_bot_file), NULL },
 { "File List",       "Display File List",              K_M_SEMICOLON, K_F11, NKA,
   0,F_I(do_switch_to_file_list), NULL },
#ifdef ENABLE_EXPERIMENTAL
 { "Previous buffers","Goto previous buffers",          K_M_LPARENTHESIS, NKA, NKA,
   0,F_I(do_switch_to_prev_buffers), NULL },
 { "Next buffers",    "Goto next buffers",              K_M_RPARENTHESIS, NKA, NKA,
   0,F_I(do_switch_to_next_buffers), NULL },
#endif // ENABLE_EXPERIMENTAL
#ifdef ENABLE_HELP
 { "Key List",        "Display Key List",               K_M_k,        NKA, NKA,
   0,F_I(do_switch_to_key_list), NULL },
 { "Func List",       "Display Function List",          K_M_f,        NKA, NKA,
   0,F_I(do_switch_to_func_list), NULL },
#endif // ENABLE_HELP
 { "Open Proj File",  "Open Project file",              K_M_l,        NKA, NKA,
   0,F_I(do_open_proj_file), NULL },
 { "Goto prev. pos",  "Goto previous pos.",             K_M_SLASH,    NKA, NKA,
   0,F_I(do_return_to_prev_file_pos), NULL },
 { "Split pane",      "Split screen into panes",        K_MC_BACKSLASH, NKA, NKA,
   0,F_I(do_tog_editor_panes), get_editor_panes },
 { "Switch pane",     "Switch editor pane",             K_M_TAB,      NKA, NKA,
   0,F_I(do_switch_editor_pane), NULL },

 { "",                "Cut-buffer Menu",                NKA,          NKA, NKA,
   0,F_I(do_editor_menu_6), NULL },
 { "Mark Text",       "Mark text",                      K_C_N, K_F05,      NKA,
   0,F_I(do_tog_mark), NULL },
 { "Clear Cut-buf",   "Clear Cut-buffers",              K_SF05,       NKA, NKA,
   0,F_I(do_clear_cut_buf), NULL },
 { "Pop Cut-buf",     "Pop Cut-buffer",                 K_SF08,       NKA, NKA,
   0,F_I(do_pop_cut_buf), NULL },
 { "Select all",      "Select all of buffer",           K_M_m,        NKA, NKA,
   0,F_I(do_select_all_lines), NULL },
 { "Cut Text",        "Cut into Cut-buffer",            K_C_K, K_F06,      NKA,
   0,F_I(do_cut_text), NULL },
 { "Delete Text",     "Delete text",                    K_SF06,       NKA, NKA,
   0,F_I(do_delete_text), NULL },
 { "Copy Text",       "Copy to Cut-buffer",             K_C_Y, K_F07,      NKA,
   1,F_I(do_copy_text), NULL },
 { "Paste Text",      "Paste from Cut-buf with pop",    K_C_P, K_F08,      NKA,
   0,F_I(do_paste_text_with_pop), NULL },
 { "Paste2 Text",     "Paste from Cut-buf without pop", K_C_O, K_F09, K_IC,    
   0,F_I(do_paste_text_without_pop), NULL },
 { "Duplic Text",     "Duplicate line",                 K_C_U, K_F10,      NKA,
   0,F_I(do_duplicate_text), NULL },
 { "CutToHead",       "Cut to head of the line",        K_C_J,        NKA, NKA,
   0,F_I(do_cut_to_head), NULL },
 { "CutToTail",       "Cut to tail of the line",        K_C_L,        NKA, NKA,
   0,F_I(do_cut_to_tail), NULL },

 { "",                "Settings Menu-1",                NKA,          NKA, NKA,
   0,F_I(do_editor_menu_7), NULL },
 { "Tab size",        "Toggle Tab size",                K_MC_T,       NKA, NKA,
   0,F_I(do_tog_tab_size),       get_str_tab_size },
 { "Tab size",        "Incurement Tab size",            K_MC_Y,       NKA, NKA,
   0,F_I(do_inc_tab_size),       get_str_tab_size },
 { "Draw cursor",     "Toggle Drawing cursor",          K_MC_C,       NKA, NKA,
   0,F_I(do_tog_draw_cursor),    get_str_draw_cursor },
 { "Ignore case",     "Toggle Ignore case",             K_MC_I,       NKA, NKA,
   0,F_I(do_tog_ignore_case),    get_str_ignore_case },
 { "Auto indent",     "Toggle Auto indent",             K_MC_O,       NKA, NKA,
   0,F_I(do_tog_auto_indent),    get_str_auto_indent },
 { "Display key list","Toggle Displaying key list",     K_MC_K,       NKA, NKA,
   0,F_I(do_editor_inc_key_list_lines), get_str_key_list_lines },
 { "Back up files",   "Increment Back up files",        K_MC_B,       NKA, NKA,
   0,F_I(do_inc_backup_files),   get_str_backup_files },
#ifdef ENABLE_REGEX
 { "RegExp",          "Toggle Regex search/replace",    K_MC_X,       NKA, NKA,
   0,F_I(do_tog_regexp),         get_str_regexp },
#endif // ENABLE_REGEX
#ifdef ENABLE_UTF8
 { "UTF-8",           "Toggle Display/Input UTF-8 code",K_MC_U,       NKA, NKA,
   0,F_I(do_tog_utf8),           get_str_utf8 },
#endif // ENABLE_UTF8

 { "",                "Settings Menu-2",                NKA,          NKA, NKA,
   0,F_I(do_editor_menu_8), NULL },
 { "Cursor center",   "Toggle Cursor-center mode",      K_MC_S,       NKA, NKA,
   0,F_I(do_inc_curs_positioning),  get_str_curs_positioning },
 { "Dual scroll",     "Toggle Dual scroll",             K_MC_D,       NKA, NKA,
   0,F_I(do_tog_dual_scroll),    get_str_dual_scroll },
 { "View mode",       "Toggle View mode",               K_MC_V,       NKA, NKA,
   0,F_I(do_tog_view_mode),      get_str_view_mode },
 { "Show ruler",      "Toggle Show Ruler",              K_MC_R,       NKA, NKA,
   0,F_I(do_tog_show_ruler),     get_str_show_ruler },
 { "Show line number","Toggle Show Line number",        K_MC_N,       NKA, NKA,
   0,F_I(do_tog_show_line_num),  get_str_show_line_num },
 { "Line-wrap mode",  "Toggle Line-wrap mode",          K_MC_L,       NKA, NKA,
   0,F_I(do_tog_line_wrap_mode), get_str_line_wrap_mode },
#ifdef ENABLE_SYNTAX
 { "Syntax HL",       "Toggle Syntax Highlighting",     K_MC_H,       NKA, NKA,
   0,F_I(do_tog_syntax_hl),      get_str_syntax_hl },
 { "TAB/EOL notation","Toggle TAB/EOL notation",        K_MC_Z,       NKA, NKA,
   0,F_I(do_tog_tab_eol_notation), get_str_tab_eol_notation },
#endif // ENABLE_SYNTAX
 { "Code-7F Key-BS",  "Toggle Code-7F Key-BS",          K_M_DEL,      NKA, NKA,
   0,F_I(do_tog_map_key_7f_bs), get_str_map_key_7f_bs },

 { "",                "File type Menu",                 NKA,          NKA, NKA,
   0,F_I(do_editor_menu_9), NULL },
 { "nix format",      "Set nix format (LF)",            NKA,          NKA, NKA,
   0,F_I(do_set_nix_file),       get_str_nix_file },
 { "Mac format",      "Set Mac format (CR)",            NKA,          NKA, NKA,
   0,F_I(do_set_mac_file),       get_str_mac_file },
 { "DOS format",      "Set DOS format (CR+LF)",         NKA,          NKA, NKA,
   0,F_I(do_set_dos_file),       get_str_dos_file },
#ifdef USE_NKF
 { "ASCII  encoding", "Set ASCII  encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_ascii),   get_str_encode_ascii },
 { "UTF-8  encoding", "Set UTF-8  encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_utf8),    get_str_encode_utf8 },
 { "EUC-JP encoding", "Set EUC-JP encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_eucjp),   get_str_encode_eucjp },
 { "S-JIS  encoding", "Set S-JIS  encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_sjis),    get_str_encode_sjis },
 { "JIS    encoding", "Set JIS    encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_jis),     get_str_encode_jis },
 { "BINARY encoding", "Set BINARY encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_binary),  get_str_encode_binary },
#endif // USE_NKF
#ifdef ENABLE_HELP
 { "Splash",          "Splash screen",                  K_M_v,        NKA, NKA,
   0,F_I(do_editor_splash), NULL },
#endif // ENABLE_HELP
 { "Color pairs",     "Display color pairs",            K_M_c,        NKA, NKA,
   0,F_I(do_editor_display_color_pairs), NULL },
 { "Refresh",         "Refresh editor screen",          K_M_ASTERISK, NKA, NKA,
   1,F_I(do_refresh_editor), NULL },

 { "",                "",                               NKA,          NKA, NKA,
   0,F_I(NULL), NULL },
};

// End of editorkeys.c
/**************************************************************************
 *   editorlist.c                                                         *
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

#include "headers.h"

int do_switch_to_file_list(void)
{
	be_buf_t *prev_cur_edit_buf;
	be_line_t *line_to_go = NULL;
	be_buf_t *edit_buf;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

_FLF_
	prev_cur_edit_buf = get_cep_buf();
	set_cep_buf(EDIT_BUFS_TOP_ANCH);
	buf_free_lines(EDIT_BUFS_TOP_ANCH);
	buf_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Files currently loaded"));
	for (edit_buf = EDIT_BUFS_TOP_NODE; IS_NODE_INT(edit_buf);
	 edit_buf = NODE_NEXT(edit_buf)) {
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, "%-60s %-5s %s %s",
		 quote_file_name(edit_buf->abs_path),
		 buf_encode_str(edit_buf), buf_eol_str(edit_buf),
		 BUF_STATE(edit_buf, buf_MODIFIED) ? "Mo" : "--");
		append_string_to_cur_edit_buf(buffer);
		if (edit_buf == prev_cur_edit_buf)
			line_to_go = CUR_EDIT_BUF_BOT_NODE;
	}
	append_magic_line();
	if (line_to_go) {
		CEPBV_CL = line_to_go;
	} else {
		CEPBV_CL = CUR_EDIT_BUF_TOP_NODE;
	}
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);

	post_cmd_processing(CUR_EDIT_BUF_TOP_NODE, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("File List"));
	return 1;
}

#ifdef ENABLE_HELP

#define HELP_BUF_IDX_KEY_LIST		0
#define HELP_BUF_IDX_FUNC_LIST		1
#define HELP_BUFS					2

void init_help_bufs(void)
{
	init_bufs_top_bot_anchor(
	 HELP_BUFS_TOP_ANCH, "#Help-bufs-top-anchor",
	 HELP_BUFS_BOT_ANCH, "#Help-bufs-bot-anchor");
	buf_insert_before(HELP_BUFS_BOT_ANCH, buf_create(_("#List of Editor Key Bindings")));
	buf_insert_before(HELP_BUFS_BOT_ANCH, buf_create(_("#List of Editor Functions")));
}
be_buf_t *get_help_buf(int help_buf_idx)
{
	return get_buf_from_bufs_by_idx(HELP_BUFS_TOP_NODE, help_buf_idx);
}

//-----------------------------------------------------------------------------
int make_help_buf_call_editor(int help_idx);
void make_help_buf(int help_idx);
void make_help_key_list(void);
void make_help_func_list(void);

int do_switch_to_key_list(void)
{
	make_help_buf_call_editor(HELP_BUF_IDX_KEY_LIST);
	return 1;
}
int do_switch_to_func_list(void)
{
	make_help_buf_call_editor(HELP_BUF_IDX_FUNC_LIST);
	return 1;
}

int make_help_buf_call_editor(int help_idx)
{
	be_buf_t *edit_buf_save = get_cep_buf();

	make_help_buf(HELP_BUF_IDX_KEY_LIST);
	make_help_buf(HELP_BUF_IDX_FUNC_LIST);

	set_cep_buf(get_help_buf(help_idx));
	switch(help_idx) {
	case HELP_BUF_IDX_KEY_LIST:
		disp_status_bar_done(_("Key List"));
		break;
	case HELP_BUF_IDX_FUNC_LIST:
		disp_status_bar_done(_("Function List"));
		break;
	}

	int ret = call_editor(1, 1);

	set_cep_buf(edit_buf_save);
	return ret;
}
void make_help_buf(int help_idx)
{
	be_buf_t *buf = get_help_buf(help_idx);
	set_cep_buf(buf);
	buf_free_lines(buf);
	switch(help_idx) {
	case HELP_BUF_IDX_KEY_LIST:
		make_help_key_list();
		break;
	case HELP_BUF_IDX_FUNC_LIST:
		make_help_func_list();
		break;
	}
	append_magic_line();
	CEPBV_CL = CUR_EDIT_BUF_TOP_NODE;
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);
	// renumber
	post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}

void make_help_key_list(void)
{
	int key_idx;
	key_code_t key;
	func_key_table_t *func_key_table;
	char *template_ = "%-5s  %-32s  %-32s";
	//			   12345678901234567890123456789012
	char *key_  = "-----";
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	buf_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Editor Functions"));
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, "Key", "Function", "func_id");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, key_, func_, func_);
	append_string_to_cur_edit_buf(buffer);
	for (key_idx = 0; key_idx < get_key_name_table_entries(); key_idx++) {
		key = key_name_table[key_idx].key_code;
		func_key_table = get_func_key_table_from_key(editor_func_key_table, key);
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
		 key_name_table[key_idx].key_name,
		 func_key_table ? func_key_table->help : "-- No function assigned --",
		 func_key_table ? func_key_table->func_id : "-- None --");
		append_string_to_cur_edit_buf(buffer);
	}
}
void make_help_func_list(void)
{
	func_key_table_t *table;
	int idx;
	static char buf1[MAX_KEY_NAME_LEN+1];
	static char buf2[MAX_KEY_NAME_LEN+1];
	static char buf3[MAX_KEY_NAME_LEN+1];
	char *template_ = "%-32s  %-5s %-5s %-5s  %-32s";
	//			   12345678901234567890123456789012
	char *key_  = "-----";
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

_FLF_
	table = get_func_key_table_from_key_group(0);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
	 "Function", "Key1", "Key2", "Key3", "func_id");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, func_, key_, key_, key_, func_);
	append_string_to_cur_edit_buf(buffer);
	for (idx = 0; table[idx].help[0]; idx++) {
		if (idx != 0 && table[idx].desc[0] == 0) {
			append_string_to_cur_edit_buf("");
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
		 table[idx].help,
		 short_key_name_from_key_code(table[idx].key1, buf1),
		 short_key_name_from_key_code(table[idx].key2, buf2),
		 short_key_name_from_key_code(table[idx].key3, buf3),
		 table[idx].func_id);
		append_string_to_cur_edit_buf(buffer);
	}
}

#endif // ENABLE_HELP

// End of list.c
/**************************************************************************
 *   editormove.c                                                         *
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

#include "headers.h"

typedef enum {
	TBF_NONE,
	TBF_TOP,		// cursor positioned previously top of buffer
	TBF_BOTTOM,		// cursor positioned previously bottom of buffer
} top_bottom_of_file_t;
PRIVATE top_bottom_of_file_t top_bottom_of_file = 0;

//-----------------------------------------------------------------------------

int do_left(void)
{
	if (is_app_list_mode()) {
		return do_page_up();
	}
	move_cursor_left(1);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int do_right(void)
{
	if (is_app_list_mode()) {
		return do_page_down();
	}
	move_cursor_right();

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

PRIVATE int get_char_type(char chr)
{
/* '\0':0, space:1, alpha-numeric:2, others:3, UTF-8:4 */
#define CHR_TYPE(chr)	((chr) == '\0' ? 0 :								\
						 (((chr) == ' ' || (chr) == '\t') ? 1 :				\
						  ((isalnum(chr) || (chr) == '_') ? 2 :				\
						    (((unsigned char)(chr)) < 0x80 ? 3 : 4)			\
						  )													\
						 )													\
						)
	return CHR_TYPE(chr);
}

// go to previous word
int do_prev_word(void)
{
	if (CEPBV_CLBI > 0) {
		// is head of the word ?
		if (get_char_type(CEPBV_CL->data[CEPBV_CLBI-1])
		  == get_char_type(CEPBV_CL->data[CEPBV_CLBI])) {
			// not head of the word
		} else {
			// head of the word
			move_cursor_left(1);
		}
		while (CEPBV_CLBI > 0
		 && (get_char_type(CEPBV_CL->data[CEPBV_CLBI-1])
		  == get_char_type(CEPBV_CL->data[CEPBV_CLBI])) ) {
			move_cursor_left(1);
		}
	} else {
		// if top of the line, move to the end of the previous line
		move_cursor_left(1);
	}

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 0;
}
// go to next word
int do_next_word(void)
{
	int chr_type;

	if (CEPBV_CL->data[CEPBV_CLBI]) {
		// skip current block
		chr_type = get_char_type(CEPBV_CL->data[CEPBV_CLBI]);
		// move to the beginning of the next block
		while (CEPBV_CL->data[CEPBV_CLBI]
		 && chr_type == get_char_type(CEPBV_CL->data[CEPBV_CLBI]))
			move_cursor_right();
	} else {
		// if end of the line, move to the top of the next line
		move_cursor_right();
	}

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 0;
}

int do_start_of_line(void)
{
	CEPBV_CLBI = 0;

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int do_end_of_line(void)
{
	CEPBV_CLBI = line_data_len(CEPBV_CL);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

//-----------------------------------------------------------------------------

PRIVATE void do_up_(void);
int do_up(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_up_();
	} else {
		do_switch_editor_pane_();
		do_up_();
		do_switch_editor_pane_();
		do_up_();
	}
	return 1;
}
PRIVATE void do_up_(void)
{
	if (c_l_up(&CEPBV_CL, &CEPBV_CLBI)) {
		CEPBV_CURSOR_Y--;
		top_bottom_of_file = TBF_NONE;
	} else {
		top_bottom_of_file = TBF_TOP;
	}
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
PRIVATE void do_down_(void);
int do_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_down_();
	} else {
		do_switch_editor_pane_();
		do_down_();
		do_switch_editor_pane_();
		do_down_();
	}
	return 1;
}
PRIVATE void do_down_(void)
{
	if (c_l_down(&CEPBV_CL, &CEPBV_CLBI)) {
		CEPBV_CURSOR_Y++;
		top_bottom_of_file = TBF_NONE;
	} else {
		top_bottom_of_file = TBF_BOTTOM;
	}
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}

PRIVATE void do_page_up_(void);
int do_page_up(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_page_up_();
	} else {
		do_switch_editor_pane_();
		do_page_up_();
		do_switch_editor_pane_();
		do_page_up_();
	}
	return 1;
}
PRIVATE void do_page_up_(void)
{
	int lines;
	int cnt;

	if (c_l_up(&CEPBV_CL, &CEPBV_CLBI) == 0) {
		if (top_bottom_of_file == TBF_TOP) {
			// already top of buffer, go to the previous buffer's last line
			top_bottom_of_file = TBF_NONE;
			tio_beep();
			if (switch_c_e_b_to_prev(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			}
		}
	} else {
///		lines = (CEPBV_CURSOR_Y - TOP_SCROLL_MARGIN_IDX) + EDITOR_VERT_SCROLL_LINES - 1;
		lines = EDITOR_VERT_SCROLL_LINES - 1;
		for (cnt = 0; cnt < lines; cnt++) {
			CEPBV_CURSOR_Y--;
			if (c_l_up(&CEPBV_CL, &CEPBV_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
}

PRIVATE int do_page_down_(void);
int do_page_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_page_down_();
	} else {
		do_switch_editor_pane_();
		do_page_down_();
		do_switch_editor_pane_();
		do_page_down_();
	}
	return 1;
}
PRIVATE int do_page_down_(void)
{
	int lines;
	int cnt;

	if (c_l_down(&CEPBV_CL, &CEPBV_CLBI) == 0) {
		if (top_bottom_of_file == TBF_BOTTOM) {
			// already bottom of buffer, go to the next buffer's top line
			top_bottom_of_file = TBF_NONE;
			tio_beep();
			if (switch_c_e_b_to_next(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
			}
		}
	} else {
///		lines = (BOTTOM_SCROLL_MARGIN_IDX - CEPBV_CURSOR_Y) + EDITOR_VERT_SCROLL_LINES - 1;
		lines = EDITOR_VERT_SCROLL_LINES - 1;
		for (cnt = 0; cnt < lines; cnt++) {
			CEPBV_CURSOR_Y++;
			if (c_l_down(&CEPBV_CL, &CEPBV_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
	return 1;
}

int do_first_line(void)
{
	first_line();
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
	return 1;
}
int do_last_line(void)
{
	CEPBV_CL = CUR_EDIT_BUF_BOT_NODE;
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_BOTTOM, UPDATE_SCRN_ALL);
	return 1;
}

PRIVATE int do_enter_utf8s(const char *utf8s);
PRIVATE int do_enter_utf8c(const char *utf8c);
int do_control_code(void)
{
	key_code_t key;

	if (is_view_mode_then_warn_it())
		return 0;
	disp_status_bar_ing(_("Input control character [^A-^Z,^[,^\\,^],^^,^_,\x7f]"));
	key = input_key_loop();
	disp_status_bar_ing(_("Key code: %04x"), key);
	if ((0x01 <= key && key < 0x20) || key == 0x7f) {
		do_enter_char(key);
	}
	return 1;
}
int do_charcode(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;
	unsigned int chr;
#ifdef ENABLE_UTF8
	char utf8c[MAX_UTF8C_BYTES+1];
#endif // ENABLE_UTF8

	if (is_view_mode_then_warn_it())
		return 0;

	ret = input_string("", string,
	 HISTORY_TYPE_IDX_SEARCH, _("Enter Unicode number in hex:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		disp_key_list_editor();
		return 0;
	}
	if (sscanf(string, "%x", &chr) == 1) {
#ifdef ENABLE_UTF8
		utf8c_encode(chr, utf8c);
		do_enter_utf8s(utf8c);
#else // ENABLE_UTF8
		do_enter_char(chr);
#endif // ENABLE_UTF8
		return 1;
	}
	return 0;
}

int do_paste_history(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;

	if (is_view_mode_then_warn_it())
		return 0;

	ret = input_string("", string,
	 HISTORY_TYPE_IDX_SEARCH, _("Select history string to paste:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		disp_key_list_editor();
		return 0;
	}
	do_enter_utf8s(string);
	return 0;
}

int do_tab(void)
{
	return do_enter_char('\t');
}

#define UTF8S_SEND_BUF_LEN			(MAX_UTF8C_BYTES * 2)
PRIVATE int utf8s_send_buf_bytes = 0;
PRIVATE char utf8s_send_buf[UTF8S_SEND_BUF_LEN+1] = "";
int do_enter_char(char chr)
{
	static char utf8c_state = 0;

	utf8c_state = utf8c_len(utf8c_state, chr);
flf_d_printf("<%02x>\n", (unsigned char)chr);
	// put to send buffer
	if (utf8s_send_buf_bytes < UTF8S_SEND_BUF_LEN) {
		utf8s_send_buf[utf8s_send_buf_bytes++] = chr;
		utf8s_send_buf[utf8s_send_buf_bytes] = '\0';
	}
	if (utf8c_state == 0) {
		do_enter_char_send();
	}
	return 1;
}
int do_enter_char_send(void)
{
	if (utf8s_send_buf_bytes == 0) {
		return 0;	// no character sent
	}
	do_enter_utf8s(utf8s_send_buf);
	// clear send buffer
	utf8s_send_buf_bytes = 0;
	utf8s_send_buf[utf8s_send_buf_bytes] = '\0';
	return 1;	// sent
}
PRIVATE int do_enter_utf8s(const char *utf8s)
{
	int bytes_str;
	int byte_idx;
#ifdef ENABLE_UTF8
	int bytes_chr;
#endif // ENABLE_UTF8
	char utf8c[MAX_UTF8C_BYTES+1];

flf_d_printf("[%s]\n", utf8s);
	do_clear_mark_();
	if (is_view_mode_then_warn_it()) {
///_FLF_
		return 0;
	}
///_FLF_
#ifdef ENABLE_UNDO
	undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO

	bytes_str = strnlen(utf8s, MAX_PATH_LEN);
#ifdef ENABLE_UTF8
	for (byte_idx = 0; byte_idx < bytes_str; byte_idx += bytes_chr) {
		bytes_chr = utf8c_bytes(&utf8s[byte_idx]);
		strlcpy__(utf8c, &utf8s[byte_idx], bytes_chr);
		do_enter_utf8c(utf8c);
	}
#else // ENABLE_UTF8
	for (byte_idx = 0; byte_idx < bytes_str; byte_idx++) {
		utf8c[0] = utf8s[byte_idx];
		utf8c[1] = '\0';
		do_enter_utf8c(utf8c);
	}
#endif // ENABLE_UTF8
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}
PRIVATE int do_enter_utf8c(const char *utf8c)
{
	int bytes_insert;

flf_d_printf("[%s]\n", utf8c);
	bytes_insert = strnlen(utf8c, MAX_UTF8C_BYTES);
	if (line_data_len(CEPBV_CL) + bytes_insert > MAX_EDIT_LINE_LEN) {
		// exceeds MAX_EDIT_LINE_LEN, do not enter
		return 0;
	}
	line_insert_string(CEPBV_CL, CEPBV_CLBI, utf8c, bytes_insert);
	CEPBV_CLBI += bytes_insert;
	get_cep_buf()->buf_size += bytes_insert;

	set_cur_buf_modified();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}

int do_carriage_return(void)
{
	const char *ptr_s, *ptr_d;
	int len_s, len_d;

///_FLF_
	do_clear_mark_();
	if (is_app_list_mode()) {
		editor_quit = EDITOR_QUIT;
		return 0;
	}
	if (is_view_mode_then_warn_it())
		return 0;

#ifdef ENABLE_UNDO
	undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO

	set_cur_buf_modified();
	// >aaaa^bbbb
	line_separate(CEPBV_CL, CEPBV_CLBI, INSERT_BEFORE);
	CEPBV_CLBI = 0;
	//  aaaa
	// >^bbbb

	if (GET_APPMD(ed_AUTO_INDENT)) {
		// autoindent: auto insert the previous lines preceeding spaces to the next line
		ptr_s = NODE_PREV(CEPBV_CL)->data;
		SKIP_SPACE(ptr_s);
		ptr_d = CEPBV_CL->data; 
		SKIP_SPACE(ptr_d);
		len_s = ptr_s - NODE_PREV(CEPBV_CL)->data;
		len_d = ptr_d - CEPBV_CL->data;
		line_replace_string(CEPBV_CL, 0, len_d, NODE_PREV(CEPBV_CL)->data, len_s);
		CEPBV_CLBI = len_s;
	}

	if (CEPBV_CURSOR_Y < BOTTOM_SCROLL_MARGIN_IDX) {
		CEPBV_CURSOR_Y++;
		post_cmd_processing(NODE_PREV(CEPBV_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
		post_cmd_processing(NODE_PREV(CEPBV_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
	return 1;
}

int do_backspace(void)
{
	int bytes;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;
///_D_(dump_editor_panes());
	if (CEPBV_CLBI <= 0) {
		if (IS_NODE_TOP(CEPBV_CL)) {
			// top of file, can not backspace
			return 0;
		}
		// line top, concatenate to the previous line
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(NODE_PREV(CEPBV_CL), CEPBV_CL, 1);
#endif // ENABLE_UNDO
		CEPBV_CLBI = line_data_len(NODE_PREV(CEPBV_CL));
		CEPBV_CL = line_concat_with_prev(CEPBV_CL);
		if (CEPBV_CURSOR_Y > 0) {
			CEPBV_CURSOR_Y--;
		}
		post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO
		// not line top, delete character
		bytes = utf8c_prev_bytes(CEPBV_CL->data, &CEPBV_CL->data[CEPBV_CLBI]);
		CEPBV_CLBI -= bytes;
		line_delete_string(CEPBV_CL, CEPBV_CLBI, bytes);
		get_cep_buf()->buf_size -= bytes;
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	}
	set_cur_buf_modified();
	return 1;
}

int do_delete_char(void)
{
	int bytes;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;

	if (CEPBV_CLBI < line_data_len(CEPBV_CL)) {
		// not line end
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO
		bytes = utf8c_bytes(&CEPBV_CL->data[CEPBV_CLBI]);
		line_delete_string(CEPBV_CL, CEPBV_CLBI, bytes);
		if (CEPB_ML == CEPBV_CL && CEPBV_CLBI < CEPB_MLBI) {
			// adjust mark position
			CEPB_MLBI -= bytes;
		}

		get_cep_buf()->buf_size -= bytes;
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	} else {
		if (IS_NODE_BOT(CEPBV_CL)) {
			// line end and the last line
			return 0;
		}
		// line end, concatenate with the next line
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, NODE_NEXT(CEPBV_CL), 1);
#endif // ENABLE_UNDO
		if (CEPB_ML == NODE_NEXT(CEPBV_CL)) {
			// next line will be freed, adjust mark position
			CEPB_ML = CEPBV_CL;
			CEPB_MLBI += line_data_len(CEPB_ML);
		}
		line_concat_with_next(CEPBV_CL);

		get_cep_buf()->buf_lines--;
		get_cep_buf()->buf_size--;
		post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	}
	set_cur_buf_modified();
	return 1;
}

int do_conv_upp_low_letter(void)
{
	int byte_idx;
	char *data;
	char first_chr;
	char chr;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;
	byte_idx = CEPBV_CLBI;
	data = CEPBV_CL->data;
	if (isalpha(data[byte_idx])) {
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, NODE_NEXT(CEPBV_CL), 1);
#endif // ENABLE_UNDO
		first_chr = data[byte_idx];
		while ((chr = data[byte_idx]) != '\0') {
			if ((isalpha(chr) || chr == '_' || isalnum(chr)) == 0)
				break;
			if (isalpha(chr)) {
				// Upper ==> Lower, Lower ==> Upper ==> Lower
				data[byte_idx] = isupper(first_chr) ? tolower(chr) : toupper(chr);
				set_cur_buf_modified();
			}
			byte_idx++;
		}
	}
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}

//-----------------------------------------------------------------------------

int do_refresh_editor(void)
{
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

//-----------------------------------------------------------------------------

int move_cursor_left(int move_disp_y)
{
	int wl_idx;

	if (CEPBV_CLBI <= 0) {
		// line top
		if (IS_NODE_TOP(CEPBV_CL)) {
			return 0;
		}
		CEPBV_CL = NODE_PREV(CEPBV_CL);
		CEPBV_CLBI = line_data_len(CEPBV_CL);
		if (move_disp_y) {
			if (CEPBV_CURSOR_Y > EDITOR_VERT_SCROLL_MARGIN_LINES) {
				CEPBV_CURSOR_Y--;
				set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
			}
		}
	} else {
		wl_idx = start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
		CEPBV_CLBI -= utf8c_prev_bytes(CEPBV_CL->data, &CEPBV_CL->data[CEPBV_CLBI]);
		if (move_disp_y) {
			if (start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1) < wl_idx) {
				CEPBV_CURSOR_Y--;
			}
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
	}
	return 1;
}

int move_cursor_right(void)
{
	int wl_idx;

	if (CEPBV_CLBI < line_data_len(CEPBV_CL)) {
		wl_idx = start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
		CEPBV_CLBI += utf8c_bytes(&CEPBV_CL->data[CEPBV_CLBI]);
		if (start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1) > wl_idx) {
			CEPBV_CURSOR_Y++;
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
	} else {
		if (IS_NODE_BOT(CEPBV_CL)) {
			return 0;
		}
		CEPBV_CL = NODE_NEXT(CEPBV_CL);
		CEPBV_CLBI = 0;
		if (CEPBV_CURSOR_Y < BOTTOM_SCROLL_MARGIN_IDX) {
			CEPBV_CURSOR_Y++;
			set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
		}
	}
	return 1;
}

int c_l_up(be_line_t **line, int *byte_idx)
{
	int line_byte_idx;
	int wl_idx;
	int col_idx;

	line_byte_idx = *byte_idx;

	te_concat_linefeed((*line)->data);
	wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	col_idx = start_col_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	if (wl_idx > 0) {
		wl_idx--;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_line_concat_linefeed, wl_idx, col_idx, -1);
	} else {
		if (IS_NODE_TOP(*line)) {
			return 0;	// no move
		}
		*line = NODE_PREV(*line);
		te_concat_linefeed((*line)->data);
		wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
		line_byte_idx = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, col_idx, -1);
	}

	*byte_idx = line_byte_idx;
	return 1;
}
int c_l_down(be_line_t **line, int *byte_idx)
{
	int line_byte_idx;
	int wl_idx;
	int col_idx;

	line_byte_idx = *byte_idx;

	te_concat_linefeed((*line)->data);
	wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	col_idx = start_col_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	if (wl_idx < max_wrap_line_idx(te_line_concat_linefeed, -1)) {
		wl_idx++;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_line_concat_linefeed, wl_idx, col_idx, -1);
	} else {
		if (IS_NODE_BOT(*line)) {
			return 0;	// no move
		}
		*line = NODE_NEXT(*line);
		te_concat_linefeed((*line)->data);
		wl_idx = 0;
		line_byte_idx = end_byte_idx_of_wrap_line_ge(te_line_concat_linefeed, wl_idx, col_idx, -1);
	}

	*byte_idx = line_byte_idx;
	return 1;
}
int cursor_next_line(void)
{
	if (IS_NODE_BOT(CEPBV_CL))
		return 0;
	CEPBV_CL = NODE_NEXT(CEPBV_CL);
	CEPBV_CLBI = 0;
	return IS_NODE_BOT(CEPBV_CL) ? 1 : 2;
}

//-----------------------------------------------------------------------------

int first_line(void)
{
	CEPBV_CL = CUR_EDIT_BUF_TOP_NODE;
	return 1;
}

// End of editormove.c
/**************************************************************************
 *   editormove2.c                                                        *
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

#include "headers.h"

// name								to does
// -------------------------------- ------------------------------------------
// update buffer size				after deletion and insertion of lines
// renumber lines					after deletion and insertion of lines
// fix buffer state					update cursor_x_to_keep, update cut region
//
// position cursor in edit_win		
//
// set edit_win_update_needed		update screen of cur-line or whole win
//
int post_cmd_processing(be_line_t *renum_from, cursor_horiz_vert_move_t cursor_move,
 locate_cursor_to_t locate_cursor, int update_needed)
{
	switch (GET_APPMD(ed_CURS_POSITIONING)) {
	case CURS_POSITIONING_NONE:		break;
	case CURS_POSITIONING_TOP:		locate_cursor = LOCATE_CURS_TOP;		break;
	case CURS_POSITIONING_CENTER:	locate_cursor = LOCATE_CURS_CENTER;		break;
	case CURS_POSITIONING_BOTTOM:	locate_cursor = LOCATE_CURS_BOTTOM;		break;
	}
	if (renum_from) {
		buf_renumber_from_line(get_cep_buf(), renum_from);
	}
	fix_buf_state_after_cursor_move(cursor_move);
	setup_cut_region_after_cursor_move(cursor_move);
	locate_cursor_in_edit_win(locate_cursor);
	set_edit_win_update_needed(update_needed);
#ifdef ENABLE_UNDO
	undo_save_after_change();
#endif // ENABLE_UNDO
	return 0;
}

//-----------------------------------------------------------------------------

//	+---------------------------------+
//	|        LOCATE_CURS_TOP          |
//	|                                 |
//	|                                 |
//	|                                 |
//	|                                 |
//	|       LOCATE_CURS_CENTER        |
//	|                                 |
//	|                                 |
//	|                                 |
//	|                                 |
//	|       LOCATE_CURS_BOTTOM        |
//	+---------------------------------+
void locate_cursor_in_edit_win(locate_cursor_to_t locate_curs)
{
	int disp_y_preferred;

	switch(locate_curs) {
	default:
	case LOCATE_CURS_NONE:
		disp_y_preferred = CEPBV_CURSOR_Y;
		break;
	case LOCATE_CURS_JUMP_BACKWARD:	// locate cursor keeping screen if possible.
	case LOCATE_CURS_JUMP_CENTER:	// locate cursor keeping screen if possible.
	case LOCATE_CURS_JUMP_FORWARD:	// locate cursor keeping screen if possible.
		// In search string,
		// Case-A: When the next cursor position is in screen,
		//         it does not move contents and locate cursor in it.
		// Case-B: When the next cursor position is out of screen,
		//         it does move contents and locate cursor at the center of screen.
		if ((disp_y_preferred = get_disp_y_after_cursor_move()) >= 0) {
			// Case-A: current line is in previous screen
		} else {
			// Case-B: current line is out of previous screen
			// LOCATE_CURS_CENTER
			switch(locate_curs) {
			default:
			case LOCATE_CURS_JUMP_BACKWARD:	// 2 line upper than center
				disp_y_preferred = edit_win_get_text_lines() / 2 - 2;
				break;
			case LOCATE_CURS_JUMP_CENTER:	// center
				disp_y_preferred = edit_win_get_text_lines() / 2;
				break;
			case LOCATE_CURS_JUMP_FORWARD:	// 2 line lower than center
				disp_y_preferred = edit_win_get_text_lines() / 2 + 2;
				break;
			}
			disp_y_preferred = MK_IN_RANGE(0, disp_y_preferred, edit_win_get_text_lines());
		}
		break;
	case LOCATE_CURS_TOP:
		disp_y_preferred = 0;
		break;
	case LOCATE_CURS_CENTER:
		disp_y_preferred = edit_win_get_text_lines() / 2;
		break;
	case LOCATE_CURS_BOTTOM:
		disp_y_preferred = edit_win_get_text_lines() - 1;
		break;
	}
	CEPBV_CURSOR_Y = disp_y_preferred;
	fix_cursor_y_keeping_vert_scroll_margin();
}

//	+---------------------------------+
//	| top-of-screen                   | ^
//	|                                 | |
//	|                                 | | cursor_y
//	|                                 | |
//	|                                 | v
//	|                                 | <= prev_cur_line, prev_cur_line_byte_idx
//	|                                 |
//	|                                 |
//	+---------------------------------+
void fix_cursor_y_keeping_vert_scroll_margin(void)
{
	int disp_y_preferred;
	int lines_go_up;
	be_line_t *line;
	int byte_idx;

	// keep top/bottom scroll margin
	disp_y_preferred = MIN_MAX_(
	 TOP_SCROLL_MARGIN_IDX,
	 CEPBV_CURSOR_Y,
	 BOTTOM_SCROLL_MARGIN_IDX);

	line = CEPBV_CL;
	byte_idx = CEPBV_CLBI;
	for (lines_go_up = 0; lines_go_up < disp_y_preferred; lines_go_up++) {
		if (c_l_up(&line, &byte_idx) == 0) {
			break;
		}
	}
	CEPBV_CURSOR_Y = lines_go_up;
}

PRIVATE be_line_t *prev_cur_line = NULL;	// Previous CEPBV_CL
PRIVATE int prev_cur_line_byte_idx = 0;		// Previous CEPBV_CLBI
PRIVATE int prev_cursor_y = 0;				// Previous cursor_y
void memorize_cursor_pos_before_move(void)
{
	prev_cur_line = CEPBV_CL;
	prev_cur_line_byte_idx = CEPBV_CLBI;
	prev_cursor_y = CEPBV_CURSOR_Y;
}
int get_disp_y_after_cursor_move(void)
{
	int cur_wl_idx;
	be_line_t *line;
	int byte_idx;
	int wl_idx;
	int yy;

	///if (IS_PTR_NULL(prev_cur_line) || IS_PTR_NULL(prev_cur_line->data)) {
	///	// Avoid editor crash !!!!
	///	_PROGERR_
	///	return -1;
	///}
	cur_wl_idx = start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);

	// check if cur_line is in [0, prev_cursor_y]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (yy = prev_cursor_y; yy >= 0; yy--) {
		wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == CEPBV_CL && wl_idx == cur_wl_idx) {
			// found in screen
			return yy;
		}
		if (c_l_up(&line, &byte_idx) == 0) {
			break;
		}
	}

	// check if cur_line is in [prev_cursor_y, edit_win_get_text_lines()]
	line = prev_cur_line;
	byte_idx = prev_cur_line_byte_idx;
	for (yy = prev_cursor_y; yy < edit_win_get_text_lines(); yy++) {
		wl_idx = start_wl_idx_of_wrap_line(line->data, byte_idx, -1);
		if (line == CEPBV_CL && wl_idx == cur_wl_idx) {
			// found in screen
			return yy;	// current line is in previous screen
		}
		if (c_l_down(&line, &byte_idx) == 0) {
			break;
		}
	}

	return -1;	// current line is out of previous screen
}

int get_cur_screen_top(be_line_t **line, int *byte_idx)
{
	return get_screen_top(CEPBV_CL, CEPBV_CLBI, CEPBV_CURSOR_Y, line, byte_idx);
}
// go backward to screen top and return line and byte_idx
int get_screen_top(be_line_t *_cl_, int _clbi_, int yy,
 be_line_t **line, int *byte_idx)
{
	int line_cnt = 0;
	int wl_idx;

	if (_cl_->data == NULL) {
		*line = _cl_;
		*byte_idx = _clbi_;
		return 0;
	}
	te_concat_linefeed(_cl_->data);
	wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, _clbi_, -1);
	for ( ; ; ) {
		if (yy <= 0)
			break;
		if (wl_idx <= 0) {
			if (IS_NODE_TOP(_cl_) == 0)
				_cl_ = NODE_PREV(_cl_);
			te_concat_linefeed(_cl_->data);
			wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
		} else {
			wl_idx--;
		}
		yy--;
		line_cnt++;
	}
	*line = _cl_;
	*byte_idx = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, 0, -1);
	return line_cnt;
}

//-----------------------------------------------------------------------------

// adjust pointers after moving horizontally or vertically
void fix_buf_state_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	int wl_idx;
	int cursor_x_in_text;

	if (is_disable_update_min_x_to_keep() == 0) {
		if (cursor_move == CURS_MOVE_HORIZ) {
			CEPBV_CURSOR_X_TO_KEEP = start_col_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
			update_min_text_x_to_keep(CEPBV_CURSOR_X_TO_KEEP);
		} else {
			wl_idx = start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
			CEPBV_CLBI = end_byte_idx_of_wrap_line_le(CEPBV_CL->data, wl_idx,
			 CEPBV_CURSOR_X_TO_KEEP, -1);
			cursor_x_in_text = start_col_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
			update_min_text_x_to_keep(cursor_x_in_text);
		}
	}
}

PRIVATE int calc_min_text_x_to_keep();
PRIVATE int recalc_min_text_x_to_keep(int disp_width, int text_width, int horiz_margin,
 int cursor_x, int cur_min_text_x);

// sample long line:
// 00000000001111111111222222222233333333334444444444
// 000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888

void update_min_text_x_to_keep(int text_x)
{
	int min_text_x_to_keep = calc_min_text_x_to_keep(text_x);
	if (min_text_x_to_keep != CEPBV_MIN_TEXT_X_TO_KEEP) {
		CEPBV_MIN_TEXT_X_TO_KEEP = min_text_x_to_keep;
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
}
PRIVATE int calc_min_text_x_to_keep()
{
	te_concat_linefeed(CEPBV_CL->data);
	return recalc_min_text_x_to_keep(get_edit_win_columns_for_text(),
	 end_col_idx_of_wrap_line(te_line_concat_linefeed, 0, INT_MAX, -1),
	 HORIZ_SCROLL_MARGIN,
	 start_col_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1),
	 CEPBV_MIN_TEXT_X_TO_KEEP);
}

// Calculate the column number of the first character displayed left most
// in window when the cursor is at the given column.
// c: cursor-pos, <: left-most, >: right-most
PRIVATE int recalc_min_text_x_to_keep(int disp_width, int text_width, int margin,
 int cursor_text_x, int cur_min_text_x)
{
	return MIN_MAX_(
	 LIM_MAX(text_width - disp_width, cursor_text_x - (disp_width - margin)),
	 cur_min_text_x,
	 LIM_MIN(0, cursor_text_x - margin));
//      cccccccccccccccccccccccccccccccccccccccc
//      0000000000111111111122222222223333333333
//      <--disp_width-------------------------->
//      <--->                              <---> horizontal-margin

//      ccccc
//      000000000011111111112222222222333333333344444444445555555555
//      <--disp_width-------------------------->
//      <--->                              <---> margin

//           cccccccccccccccccccccccccccccc
//      000000000011111111112222222222333333333344444444445555555555
//      <--disp_width-------------------------->
//      <--->                              <---> margin

//                     cccccccccccccccccccccccccccccccccccccccc
//      000000000011111111112222222222333333333344444444445555555555
//                          <--disp_width-------------------------->
//                                                             <---> margin

//                                                             ccccc
//      000000000011111111112222222222333333333344444444445555555555
//                          <--disp_width-------------------------->
//                          <--->                              <---> margin
}

// min_text_x_to_keep = 0
// |<-- display width ----------------------------->|
// 00000000001111111111222222222233333333334444444444555555555566666666667777777777
//
//           min_text_x_to_keep = 10
//           |<-- display width ----------------------------->|
// 00000000001111111111222222222233333333334444444444555555555566666666667777777777
int get_cep_buf_view_min_text_x_to_keep(void)
{
	return CEPBV_MIN_TEXT_X_TO_KEEP;
}

//-----------------------------------------------------------------------------

PRIVATE char disable_update_min_x_to_keep = 0;
void set_disable_update_min_x_to_keep()
{
	disable_update_min_x_to_keep = 1;
}
void clear_disable_update_min_x_to_keep()
{
	disable_update_min_x_to_keep = 0;
}
char is_disable_update_min_x_to_keep()
{
	return disable_update_min_x_to_keep;
}

// End of editormove2.c
/**************************************************************************
 *   editorundo.c                                                         *
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

#include "headers.h"

#ifdef ENABLE_UNDO

// Undo-buffers manipulation routines -----------------------------------------

void init_undo_redo_bufs(void)
{
	init_bufs_top_bot_anchor(
	 UNDO_BUFS_TOP_ANCH, "#Undo-bufs-top-anchor",
	 UNDO_BUFS_BOT_ANCH, "#Undo-bufs-bot-anchor");

	init_bufs_top_bot_anchor(
	 REDO_BUFS_TOP_ANCH, "#Redo-bufs-top-anchor",
	 REDO_BUFS_BOT_ANCH, "#Redo-bufs-bot-anchor");
}

be_buf_t *push_undo_buf(be_buf_t *buf)
{
	buf = buf_create_copy(buf);
	snprintf(buf->file_path, MAX_PATH_LEN, "undo-buf-%02d", count_undo_bufs());
	return buf_insert_after(UNDO_BUFS_TOP_ANCH, buf);
}
be_buf_t *pop_undo_buf(void)
{
	if (IS_NODE_BOT_ANCH(CUR_UNDO_BUF))
		return NULL;
	return buf_unlink(CUR_UNDO_BUF);
}
be_buf_t *push_redo_buf(be_buf_t *buf)
{
	buf = buf_create_copy(buf);
	snprintf(buf->file_path, MAX_PATH_LEN, "redo-buf-%02d", count_redo_bufs());
	return buf_insert_after(REDO_BUFS_TOP_ANCH, buf);
}
be_buf_t *pop_redo_buf(void)
{
	if (IS_NODE_BOT_ANCH(CUR_REDO_BUF))
		return NULL;
	return buf_unlink(CUR_REDO_BUF);
}
int delete_undo_redo_buf(be_buf_t *edit_buf)
{
	// delete undo, redo buffers related to edit_buf
	return delete_do_buf(edit_buf, UNDO_BUFS_TOP_NODE)
	 + delete_do_buf(edit_buf, REDO_BUFS_TOP_NODE);
}
int delete_do_buf(be_buf_t *edit_buf, be_buf_t *do_buf)
{
	int deleted = 0;

	for ( ; IS_NODE_INT(do_buf); do_buf = NODE_NEXT(do_buf)) {
		if (strcmp(do_buf->abs_path, edit_buf->abs_path) == 0) {
			do_buf = buf_unlink_free(do_buf);
			deleted++;
		}
	}
	return deleted;
}
int count_undo_bufs(void)
{
	return buf_count_bufs(CUR_UNDO_BUF);
}
int count_redo_bufs(void)
{
	return buf_count_bufs(CUR_REDO_BUF);
}

#ifdef ENABLE_DEBUG
PRIVATE be_buf_t *prev_c_e_b;
PRIVATE size_t prev_buf_size;
PRIVATE int prev_count_undo_bufs;
void memorize_undo_state_before_change(void)
{
	prev_c_e_b = get_cep_buf();
	prev_buf_size = get_cep_buf()->buf_size;
	prev_count_undo_bufs = count_undo_bufs();
}
void check_undo_state_after_change(void)
{
	if (get_cep_buf() != EDIT_BUFS_TOP_ANCH
	 && get_cep_buf() == prev_c_e_b && get_cep_buf()->buf_size != prev_buf_size
		// edit buffer has been modified
	 && count_undo_bufs() == prev_count_undo_bufs) {
		// but no undo info pushed
		// warn it by setting unusual application color
		set_work_space_color_low();
		disp_status_bar_err(_("!!!! No UNDO info pushed !!!!"));
		progerr_printf("No UNDO info pushed for %s\n", prev_func_id);
	}
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------
// aaaa				<== undo_min_line (line not-modified)
// BBBB				first line which will be modified
// CCCC				last line which will be modified
// dddd				<== undo_max_line (line not-modified)
PRIVATE be_line_t *undo_min_line = NULL;
PRIVATE be_line_t *undo_max_line = NULL;
PRIVATE int undo_lines = 0;
PRIVATE void save_region_to_undo_buf(void);
PRIVATE be_line_t *append_line_to_cur_undo_buf(be_line_t *line);

void undo_set_region_save_before_change(be_line_t *min_line, be_line_t *max_line,
 int cut_buf_lines)
{
	undo_set_region(min_line, max_line, cut_buf_lines);
	undo_save_before_change();
}
void undo_set_region(be_line_t *min_line, be_line_t *max_line, int cut_buf_lines)
{
	undo_min_line = NODE_PREV(min_line);
	undo_max_line = NODE_NEXT(max_line);
	undo_lines = cut_buf_lines;

	undo_adjust_max_line();
}
// save undo info before change
void undo_save_before_change(void)
{
	save_region_to_undo_buf();
}
void undo_adjust_max_line(void)
{
	be_line_t *line;
	int lines;
	int past_max_line = 0;

	line = NODE_NEXT(undo_min_line);
	for (lines = 0 ; IS_NODE_VALID(line); lines++) {
		if (line == undo_max_line) {
			past_max_line = 1;
		}
		if (lines >= undo_lines && past_max_line) {
			break;
		}
		line = NODE_NEXT(line);
	}
	undo_max_line = line;	// adjust max line
}
// save undo info after change
void undo_save_after_change(void)
{
	if (count_undo_bufs() % 2) {
		// count_undo_bufs() is odd.
		// The state before change was saved.
		// Save the state after change.
		save_region_to_undo_buf();	// save the state after change
		if (count_undo_bufs() >= 2) {
			// compare before and after
			if (buf_compare(CUR_UNDO_BUF, NODE_NEXT(CUR_UNDO_BUF)) == 0) {
				// not changed, pop two buffer (after and before)
				buf_free(pop_undo_buf());
				buf_free(pop_undo_buf());
			}
		}
	}
}
PRIVATE void save_region_to_undo_buf(void)
{
	be_line_t *line;

	push_undo_buf(get_cep_buf());
	for (line = NODE_NEXT(undo_min_line); line != undo_max_line; line = NODE_NEXT(line)) {
		append_line_to_cur_undo_buf(line_create_copy(line));
	}
}
PRIVATE be_line_t *append_line_to_cur_undo_buf(be_line_t *line)
{
	return line_insert(CUR_UNDO_BUF_BOT_ANCH, line, INSERT_BEFORE);
}

typedef enum /*undo0_redo1*/ {
	UNDO0,	// undo
	REDO1	// redo
} undo0_redo1_t;

PRIVATE int do_undo_redo_(undo0_redo1_t undo0_redo1);
PRIVATE be_line_t *restore_region_from_buffer(undo0_redo1_t undo0_redo1);
PRIVATE be_line_t *delete_region_in_buf(be_buf_t *buf);
PRIVATE be_line_t *insert_region_from_buf(be_line_t *edit_line, be_buf_t *buf);

int do_undo(void)
{
	do_clear_mark_();
	if (count_undo_bufs() < 2) {
		disp_status_bar_err(_("Undo-buffer empty !!"));
		return 0;
	}
	return do_undo_redo_(UNDO0);
}
int do_redo(void)
{
	do_clear_mark_();
	if (count_redo_bufs() < 2) {
		disp_status_bar_err(_("Redo-buffer empty !!"));
		return 0;
	}
	return do_undo_redo_(REDO1);
}
PRIVATE int do_undo_redo_(undo0_redo1_t undo0_redo1)
{
	be_line_t *top_line;

	set_cur_buf_modified();
	top_line = restore_region_from_buffer(undo0_redo1);
	post_cmd_processing(top_line, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

// modification:
//	[edit-buf(before)]	[edit-buf(after)]	[undo-stack]	[redo-stack]
//	aaaa				aaaa				BBBB
//	bbbb				BBBB				bbbb
//	cccc				cccc
//
// undo:
//	[edit-buf(before)]	[edit-buf(after)]	[undo-stack]	[redo-stack]
//	aaaa				aaaa								bbbb
//	BBBB				bbbb								BBBB
//	cccc				cccc
//
// redo:
//	[edit-buf(before)]	[edit-buf(after)]	[undo-stack]	[redo-stack]
//	aaaa				aaaa				BBBB
//	bbbb				BBBB				bbbb
//	cccc				cccc
//
PRIVATE be_line_t *restore_region_from_buffer(undo0_redo1_t undo0_redo1)
{
	be_buf_t *buf_after;
	be_buf_t *buf_before;
	be_line_t *edit_line;
	be_line_t *top_line;

	if (undo0_redo1 == UNDO0) {
		// undo (move undo information from undo-buf to redo-buf)
		buf_after = pop_undo_buf();
		push_redo_buf(buf_after);
		buf_before = pop_undo_buf();
		push_redo_buf(buf_before);
	} else {
		// redo (move undo information from redo-buf to undo-buf)
		buf_after = pop_redo_buf();
		push_undo_buf(buf_after);
		buf_before = pop_redo_buf();
		push_undo_buf(buf_before);
	}

	// delete modified lines
	edit_line = delete_region_in_buf(buf_after);
	top_line = NODE_PREV(edit_line);
	// insert unmodified lines
	insert_region_from_buf(edit_line, buf_before);
	return top_line;
}
PRIVATE be_line_t *delete_region_in_buf(be_buf_t *buf)
{
	be_line_t *edit_line;
	be_line_t *undo_line;

	if (switch_c_e_b_to_abs_path(buf->abs_path) == 0) {
		progerr_printf("No such buffer: %s\n", buf->abs_path);
		return CUR_EDIT_BUF_BOT_NODE;
	}
	edit_line = get_line_ptr_from_cur_buf_line_num(BUF_TOP_NODE(buf)->line_num);
	for (undo_line = BUF_TOP_NODE(buf); IS_NODE_INT(undo_line);
	 undo_line = NODE_NEXT(undo_line)) {
		edit_line = line_unlink_free(edit_line);	// delete line
	}
	return edit_line;
}
PRIVATE be_line_t *insert_region_from_buf(be_line_t *edit_line, be_buf_t *buf)
{
	be_line_t *undo_line;

	if (switch_c_e_b_to_abs_path(buf->abs_path) == 0) {
		progerr_printf("No such buffer: %s\n", buf->abs_path);
		return CUR_EDIT_BUF_BOT_NODE;
	}
	for (undo_line = BUF_TOP_NODE(buf); IS_NODE_INT(undo_line);
	 undo_line = NODE_NEXT(undo_line)) {
		line_insert(edit_line, line_create_copy(undo_line), INSERT_BEFORE);
	}
	// restore pointers
	CEPBV_CLBI = buf->buf_views[0].cur_line_byte_idx;
	CEPBV_CL = get_line_ptr_from_cur_buf_line_num(BUF_TOP_NODE(buf)->line_num);
	return edit_line;
}

#ifdef ENABLE_DEBUG
void dump_undo_bufs_lines(void)
{
	buf_dump_bufs_lines(UNDO_BUFS_TOP_ANCH, "undo-bufs");
}
void dump_redo_bufs_lines(void)
{
	buf_dump_bufs_lines(REDO_BUFS_TOP_ANCH, "redo-bufs");
}
#endif // ENABLE_DEBUG

#endif // ENABLE_UNDO

// End of editorundo.c
/**************************************************************************
 *   fileio.c                                                             *
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

#include "headers.h"

PRIVATE int load_file_into_new_buf__(const char *full_path, int open_on_err, int msg_on_err);
PRIVATE int load_file_into_cur_buf__(const char *full_path, int load_binary_file, int msg_on_err);

PRIVATE int load_file_into_cur_buf_ascii(const char *file_name);
#ifdef USE_NKF
PRIVATE int guess_encoding_by_nkf(const char *full_path);
PRIVATE int load_file_into_cur_buf_nkf(const char *full_path, const char *nkf_options);
#endif // USE_NKF
PRIVATE int load_fp_into_cur_buf(FILE *fp);

int load_file_into_new_buf(const char *full_path, int open_on_err, int msg_on_err)
{
	int tab_size;
	int lines;

	lines = load_file_into_new_buf__(full_path, open_on_err, msg_on_err);

///flf_d_printf("%s:%d\n", full_path, lines);
	if (lines < 0) {
		return lines;
	}
	append_magic_line();
	CEPBV_CL = CUR_EDIT_BUF_TOP_NODE;
	renumber_cur_buf_from_top();
	update_cur_buf_crc();

	if ((tab_size = buf_guess_tab_size(get_cep_buf())) != 0) {
		CUR_EBUF_STATE(buf_TAB_SIZE) = tab_size;
	}
	disp_status_bar_ing(P_(_("%d line read %s"),
						   _("%d lines read %s"),
						   _("%d liness read %s"),
						   _("%d linesss read %s"),
	 lines), lines, buf_eol_str(get_cep_buf()));
	return lines;
}
PRIVATE int load_file_into_new_buf__(const char *full_path, int open_on_err, int msg_on_err)
{
	struct stat fileinfo;
	int ret = -1;

	if (is_strlen_not_0(full_path)) {
		ret = stat(full_path, &fileinfo);
	}
///flf_d_printf("[%s], ret %d\n", full_path, ret);
	if (ret < 0) {
		// New file
		if (open_on_err == 0) {
			if (msg_on_err) {
				disp_status_bar_err(_("File [%s] not found"), shrink_str_to_scr_static(full_path));
			}
			return -1;	// open error
		}
		create_edit_buf(full_path);
		disp_status_bar_ing(_("New File"));
		return 1;		// new file
	}
	if (S_ISREG(fileinfo.st_mode) == 0) {
		// special file
		if (msg_on_err) {
			disp_status_bar_err(S_ISDIR(fileinfo.st_mode)
			 ? _("[%s] is a directory") : _("[%s] is a special file"),
			 shrink_str_to_scr_static(full_path));
		}
		return -2;		// open error
	}
	// Max. file size loadable is half of free memory.
	if ((fileinfo.st_size / 1000) >= get_mem_free_in_kb(1) / 2) {
		// file size too large
		if (msg_on_err) {
			disp_status_bar_err(_("[%s] is too large to read into buffer"),
			 shrink_str_to_scr_static(full_path));
		}
		return -3;		// open error
	}
	// regular file
	disp_status_bar_ing(_("Reading File %s ..."), shrink_str_to_scr_static(full_path));
	create_edit_buf(full_path);
	memcpy__(&get_cep_buf()->orig_file_stat, &fileinfo, sizeof(fileinfo));

	ret = load_file_into_cur_buf__(full_path, 1, msg_on_err);

	if (ret < 0) {
		free_cur_edit_buf();
		return -1;
	}
	return ret;				// loaded
}

//-----------------------------------------------------------------------------

PRIVATE int load_file_into_cur_buf__(const char *full_path, int load_binary_file, int msg_on_err)
{
#ifdef USE_NKF
	const char *nkf_options;
#endif // USE_NKF

////flf_d_printf("full_path: [%s]\n", full_path);
#ifdef USE_NKF
	if (GET_APPMD(ed_USE_NKF)) {
		if (CUR_EBUF_STATE(buf_ENCODE) == ENCODE_ASCII) {
			// encoding is not specified on command line
			guess_encoding_by_nkf(full_path);
			if (CUR_EBUF_STATE(buf_ENCODE) == ENCODE_BINARY && load_binary_file == 0) {
				if (msg_on_err) {
					disp_status_bar_err(_("BINARY file !! [%s]"),
					 shrink_str_to_scr_static(full_path));
				}
				return -1;		// do not load binary file
			}
		}
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
			break;
		case ENCODE_EUCJP:
			nkf_options = "-Ewx";	// input EUCJP, output UTF8, preserve HankakuKana
			break;
		case ENCODE_SJIS:
			nkf_options = "-Swx";	// input SJIS, output UTF8, preserve HankakuKana
			break;
		case ENCODE_JIS:
			nkf_options = "-Jwx";	// input JIS, output UTF8, preserve HankakuKana
			break;
		}
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			break;
		case ENCODE_EUCJP:
		case ENCODE_SJIS:
		case ENCODE_JIS:
			return load_file_into_cur_buf_nkf(full_path, nkf_options);
		}
	} // if (GET_APPMD(ed_USE_NKF))
#endif // USE_NKF
	return load_file_into_cur_buf_ascii(full_path);
}

PRIVATE int load_file_into_cur_buf_ascii(const char *full_path)
{
	FILE *fp;
	int lines;

	if ((fp = fopen(full_path, "rb")) == NULL) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	lines = load_fp_into_cur_buf(fp);
	if (fclose(fp) != 0) {	// 0: OK, EOF: error
		lines = -1;
	}
	return lines;
}

#ifdef USE_NKF
PRIVATE int guess_encoding_by_nkf(const char *full_path)
{
	char buffer[MAX_PATH_LEN+1];
	FILE *fp;

	CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	// No encoding specified in command line
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf -g \"%s\"", full_path);
	if ((fp = popen(buffer, "r")) <= 0) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	if (fgets(buffer, MAX_PATH_LEN, fp) == NULL)
		buffer[0] = '\0';
	if (pclose(fp) == -1) {
		return -1;
	}
flf_d_printf("guessed encoding by nkf: %s", buffer);
	if (strlcmp__(buffer, "ASCII") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	} else if (strlcmp__(buffer, "UTF-8") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_UTF8;
	} else if (strlcmp__(buffer, "EUC-JP") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_EUCJP;
	} else if (strlcmp__(buffer, "Shift_JIS") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_SJIS;
	} else if (strlcmp__(buffer, "ISO-2022-JP") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_JIS;
	} else if (strlcmp__(buffer, "BINARY") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_BINARY;
	} else {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_BINARY;
	}
	return 0;
}
PRIVATE int load_file_into_cur_buf_nkf(const char *full_path, const char *nkf_options)
{
	char buffer[MAX_PATH_LEN+1];
	FILE *fp;
	int lines;

///flf_d_printf("nkf_options[%s]\n", nkf_options);
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf %s \"%s\"", nkf_options, full_path);
///flf_d_printf("[%s]\n", buffer);
	if ((fp = popen(buffer, "r")) <= 0) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	lines = load_fp_into_cur_buf(fp);
	if (pclose(fp) == -1) {	// -1: error
		lines = -1;
	}
	return lines;
}
#endif // USE_NKF

PRIVATE void fgetc_buffered_clear(void);
PRIVATE int fgetc_buffered(FILE *fp);

PRIVATE int load_fp_into_cur_buf(FILE *fp)
{
	int file_format_idx = 0;	// 0 = nix, 1 = Mac, 2 = DOS
	int chr_int;			// read character
	int prev_chr = '\0';	// previous read character
	char buf[MAX_EDIT_LINE_LEN+1];
	int len;
	int lines_read = 0;

	len = 0;
	buf[len] = '\0';
	fgetc_buffered_clear();
	for ( ; ; ) {
		chr_int = fgetc_buffered(fp);
		switch(chr_int) {
		case '\n':
			if (prev_chr == '\r') {
				file_format_idx = 2;	// LF after CR (DOS format)
				break;
			}
			// LF (nix format)
			goto append_line;
		case '\r':
			// CR (DOS/Mac format)
			file_format_idx = 1;		// line end is only CR (Mac format)
			goto append_line;
		default:
			if (len >= MAX_EDIT_LINE_LEN) {
				append_string_to_cur_edit_buf(buf);
				lines_read++;
				len = 0;
				buf[len] = '\0';
			}
			buf[len++] = chr_int;
			buf[len] = '\0';
			break;
		case EOF:
			if (len == 0)
				break;
append_line:;
			append_string_to_cur_edit_buf(buf);
			lines_read++;
			len = 0;
			buf[len] = '\0';
			break;
		}
		prev_chr = chr_int;
		if (chr_int == EOF)
			break;
	}
	switch(file_format_idx) {
	case 0:
		set_eol(EOL_NIX);
		break;
	case 1:
		set_eol(EOL_MAC);
		break;
	case 2:
		set_eol(EOL_DOS);
		break;
	}
////flf_d_printf("file_format_idx: %d\n", file_format_idx);
////flf_d_printf("CUR_EBUF_STATE(buf_EOL) => [%s]\n", buf_eol_str(get_cep_buf()));
	return lines_read;
}

PRIVATE char fgetc_buffered_buf[MAX_EDIT_LINE_LEN+1];
PRIVATE int fgetc_buffered_read_len = 0;
PRIVATE int fgetc_buffered_byte_idx = 0;

PRIVATE void fgetc_buffered_clear(void)
{
	fgetc_buffered_read_len = 0;
	fgetc_buffered_byte_idx = 0;
}
PRIVATE int fgetc_buffered(FILE *fp)
{
	int chr;

	if (fgetc_buffered_byte_idx >= fgetc_buffered_read_len) {
		if ((fgetc_buffered_read_len = fread(fgetc_buffered_buf, 1, MAX_EDIT_LINE_LEN, fp)) <= 0)
			return EOF;
		fgetc_buffered_byte_idx = 0;
	}
	chr = (unsigned char)fgetc_buffered_buf[fgetc_buffered_byte_idx++];
#define NUL_REPLACE_CHR		' '		// replace '\0' to ' '
	if (chr == '\0') {
		chr = NUL_REPLACE_CHR;		// replace '\0'
	}
	return chr;
}

//-----------------------------------------------------------------------------

int backup_and_save_cur_buf_ask(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = 0;

	strlcpy__(file_path, get_cep_buf()->abs_path, MAX_PATH_LEN);
	if (is_strlen_0(file_path)) {
		if (input_new_file_name_n_ask(file_path) <= 0) {
			return -1;
		}
	}
///flf_d_printf("[%s]\n", file_path);
	if (buf_is_orig_file_updated(get_cep_buf()) > 0) {
		// file is modified by others
		ret = ask_yes_no(ASK_YES_NO,
		 _("File was modified by another program, OVERWRITE ?"));
		if (ret < 0) {
			disp_status_bar_done(_("Cancelled"));
			return -1;
		}
	}
	if (is_strlen_0(file_path) || file_path[0] == '#') {
		disp_status_bar_done(_("Cancelled"));
		return -1;
	}
///flf_d_printf("[%s]\n", file_path);
	if ((ret = backup_and_save_cur_buf(file_path)) < 0) {
		disp_status_bar_err(_("File [%s] can NOT be written !!"),
		 shrink_str_to_scr_static(file_path));
	}
	return ret;
}

int input_new_file_name_n_ask(char *file_path)
{
	int ret = 0;

	while (1) {
		ret = input_string(file_path, file_path,
		 HISTORY_TYPE_IDX_DIR, "%s:", _("File Name to Write"));

		if (ret <= 0) {
			set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
			return 0;		// cancelled
		}
#ifdef ENABLE_FILER
		if (strcmp(file_path, "") == 0 || is_path_wildcard(file_path)) {
			ret = call_filer(1, 1, "", "", file_path, MAX_PATH_LEN);
			if (ret <= 0)
				continue;
			strlcpy__(file_path, file_path, MAX_PATH_LEN);
		}
#endif // ENABLE_FILER
		if (is_path_exist(file_path)) {
			if (is_path_regular_file(file_path) > 0) {
				// ask Overwrite
				ret = ask_yes_no(ASK_YES_NO,
				 _("File exists, OVERWRITE it ?"));
				if (ret < 0) {
					return 0;		// cancelled
				}
				if (ret == 0)
					continue;
			} else {
				// ask non regular file
				ret = ask_yes_no(ASK_YES_NO,
				 _("Path is not file, can not WRITE it"));
				if (ret < 0) {
					return 0;		// cancelled
				}
				continue;
			}
		}
		break;
	}
///flf_d_printf("[%s]\n", file_path);
	return 1;		// input
}

//-----------------------------------------------------------------------------

PRIVATE int backup_files(const char *file_path, int depth);
PRIVATE char *make_backup_file_path(const char *orig_path, char *backup_path, int depth);
PRIVATE int save_cur_buf_to_file__(const char *file_path);

int backup_and_save_cur_buf(const char *file_path)
{
	char abs_path[MAX_PATH_LEN+1];
	int mask = 0;
	int lines_written;

///flf_d_printf("[%s]\n", file_path);
	get_abs_path(file_path, abs_path);
	// TODO: do minimum check
	//  file_path is regular file and not dir and special file
	if (is_path_regular_file(file_path) == 0) {
		disp_status_bar_err(_("File [%s] is NOT regular file !!"),
		 shrink_str_to_scr_static(file_path));
		return -1;
	}
	disp_status_bar_ing(_("Writing File %s ..."), shrink_str_to_scr_static(get_cep_buf()->abs_path));
	if (GET_APPMD(ed_BACKUP_FILES)) {
		if (backup_files(file_path, get_backup_files()) < 0) {
			return -1;
		}
	}

	lines_written = save_cur_buf_to_file__(abs_path);

	if (S_ISREG(get_cep_buf()->orig_file_stat.st_mode)) {
		mask = get_cep_buf()->orig_file_stat.st_mode & 07777;
///flf_d_printf("chmod([%s], %05o)\n", abs_path, mask);
		if (chmod(abs_path, mask) < 0) {
			disp_status_bar_err(_("Can not set permissions %1$o on [%2$s]: %3$s"),
			 mask, shrink_str_to_scr_static(abs_path), strerror(errno));
		}
	}

	// get current file stat in orig_file_stat
	stat(get_cep_buf()->file_path, &get_cep_buf()->orig_file_stat);
	update_cur_buf_crc();

	disp_status_bar_ing(P_(_("%d line written"),
						   _("%d lines written"),
						   _("%d liness written"),
						   _("%d linesss written"),
	 lines_written), lines_written);
	CUR_EBUF_STATE(buf_MODIFIED) = 0;
	disp_editor_title_bar();

	return lines_written;		// -1: error
}
PRIVATE int backup_files(const char *file_path, int depth)
{
	char orig_path[MAX_PATH_LEN+1];
	char backup_path[MAX_PATH_LEN+1];
	struct stat st;

	depth = LIM_MIN(0, depth);
	for ( ; depth > 0; depth--) {
		make_backup_file_path(file_path, backup_path, depth);
		make_backup_file_path(file_path, orig_path, depth-1);
		if (!stat(orig_path, &st)) {
			remove(backup_path);
			if (rename(orig_path, backup_path) < 0) {
				return -1;
			}
		}
	}
	return 0;
}
PRIVATE char *make_backup_file_path(const char *orig_path, char *backup_path, int depth)
{
	strlcpy__(backup_path, orig_path, MAX_PATH_LEN);
	while (depth--)
		strlcat__(backup_path, MAX_PATH_LEN, "~");
	return backup_path;
}

PRIVATE int save_cur_buf_to_file_ascii(const char *file_path);
#ifdef USE_NKF
PRIVATE int save_cur_buf_to_file_nkf(const char *file_path, const char *nkf_options);
#endif // USE_NKF
PRIVATE int save_cur_buf_to_fp(const char *file_path, FILE *fp);

PRIVATE int save_cur_buf_to_file__(const char *file_path)
{
#ifdef USE_NKF
	const char *nkf_options;
#endif // USE_NKF

#ifdef USE_NKF
	if (GET_APPMD(ed_USE_NKF)) {
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
			break;
		case ENCODE_EUCJP:
			nkf_options = "-Wex";	// input UTF8, output EUCJP, preserve HankakuKana
			break;
		case ENCODE_SJIS:
			nkf_options = "-Wsx";	// input UTF8, output SJIS, preserve HankakuKana
			break;
		case ENCODE_JIS:
			nkf_options = "-Wjx";	// input UTF8, output JIS, preserve HankakuKana
			break;
		}
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			break;
		case ENCODE_EUCJP:
		case ENCODE_SJIS:
		case ENCODE_JIS:
			return save_cur_buf_to_file_nkf(file_path, nkf_options);
		}
	}
#endif // USE_NKF
	return save_cur_buf_to_file_ascii(file_path);
}

PRIVATE int save_cur_buf_to_file_ascii(const char *file_path)
{
	FILE *fp = 0;
	int lines;

	if ((fp = fopen(file_path, "wb")) == NULL) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	if ((lines = save_cur_buf_to_fp(file_path, fp)) < 0) {
		lines = -2;
	}
	if (fclose(fp) != 0) {
		disp_status_bar_err(_("Can not close file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		lines = -3;
	}
	return lines;
}
#ifdef USE_NKF
PRIVATE int save_cur_buf_to_file_nkf(const char *file_path, const char *nkf_options)
{
	char buffer[MAX_PATH_LEN+1] = "";
	FILE *fp = 0;
	int lines;

///flf_d_printf("nkf_options[%s]\n", nkf_options);
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf %s >\"%s\"", nkf_options, file_path);
///flf_d_printf("[%s]\n", buffer);
	if ((fp = popen(buffer, "w")) <= 0) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	if ((lines = save_cur_buf_to_fp(file_path, fp)) < 0) {
		return -2;
	}
	if (pclose(fp) == -1) {	// -1: error
		disp_status_bar_err(_("Can not close file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -3;
	}
	return lines;
}
#endif // USE_NKF

PRIVATE int save_cur_buf_to_fp(const char *file_path, FILE *fp)
{
	int lines_written;
	const be_line_t *line;
	int line_len;
	long size;

	lines_written = 0;
	for (line = CUR_EDIT_BUF_TOP_NODE; IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (IS_NODE_BOT(line) && line_data_len(line) == 0)
			break;			// do not output the magic line
		line_len = line_data_len(line);
		size = fwrite(line->data, 1, line_len, fp);
		if (size < line_len) {
			disp_status_bar_err(_("Can not write file [%s]: %s"),
			 shrink_str_to_scr_static(file_path), strerror(errno));
			return -1;
		}
		switch (CUR_EBUF_STATE(buf_EOL)) {
		default:
		case EOL_NIX:
			putc('\n', fp);
			break;
		case EOL_MAC:
			putc('\r', fp);
			break;
		case EOL_DOS:
			putc('\r', fp);
			putc('\n', fp);
			break;
		}
		lines_written++;
	}
	return lines_written;
}
//-----------------------------------------------------------------------------
PRIVATE int files_loaded = 0;
void clear_files_loaded(void)
{
_FLF_
	files_loaded = 0;
}
int add_files_loaded(int files)
{
	return files_loaded += files;
}
int get_files_loaded(void)
{
	return files_loaded;
}
void disp_files_loaded_ifnon0(void)
{
	if (files_loaded > 0) {
		disp_files_loaded();
	}
}
void disp_files_loaded(void)
{
	disp_status_bar_done(P_(_("%d file loaded"),
							_("%d files loaded"),
							_("%d filess loaded"),
							_("%d filesss loaded"),
	 files_loaded), files_loaded);
}

// End of fileio.c
/**************************************************************************
 *   filer.c                                                              *
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

#include "headers.h"

#ifdef ENABLE_FILER

filer_panes_t *cur_filer_panes;		// Current Filer Views
/////filer_view_t *cur_fv;				// Current Filer View

PRIVATE char filer_cur_path[MAX_PATH_LEN+1];	// /directory/filter.*
int filer_do_next = FILER_DO_NOTHING;

PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir);
PRIVATE filer_panes_t *inherit_filer_panes(filer_panes_t *next_fps);
PRIVATE void free_filer_panes(filer_panes_t *fps, filer_panes_t *prev_fps);
PRIVATE int get_other_filer_pane_idx(int filer_pane_idx);
#if 0
#ifdef ENABLE_DEBUG
PRIVATE void dump_filer_view(filer_view_t *fv);
PRIVATE void dump_filer_panes(void);
#endif // ENABLE_DEBUG
#endif // 0

PRIVATE int filer_main_loop(const char *directory, const char *filter,
 char *file_path, int buf_len);
PRIVATE int check_filer_cur_dir(void);
PRIVATE int update_all_file_list(const char *filter, int force_update);
PRIVATE int update_file_list(filer_view_t *fv, const char *filter, int force_update);

PRIVATE void disp_filer_title_bar(const char *path,
 int cur_idx, int files_selected, int files_total);
PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane);

PRIVATE int filer_win_get_file_path_lines(void);
PRIVATE int filer_win_get_file_list_lines(void);
PRIVATE int filer_win_get_file_path_y(void);
PRIVATE int filer_win_get_file_list_y(void);

PRIVATE void disp_key_list_filer(void);

#define FILER_VERT_SCROLL_MARGIN_LINES	LIM_MAX(5, filer_win_get_file_list_lines()/3)
#define FILER_VERT_SCROLL_LINES			\
	MIN_MAX_(1, filer_win_get_file_list_lines()-1 - FILER_VERT_SCROLL_MARGIN_LINES, 50)

void init_filer_panes(filer_panes_t *fps, const char *cur_dir)
{
	int filer_pane_idx;

	cur_filer_panes = fps;
	strlcpy__(cur_filer_panes->org_cur_dir, cur_dir, MAX_PATH_LEN);
	set_filer_cur_pane_idx(0);
	for (filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		// set initial value
		init_filer_view(&cur_filer_panes->filer_views[filer_pane_idx], cur_dir);
	}
}
void set_filer_cur_pane_idx(int cur_pane_idx)
{
	cur_filer_panes->cur_pane_idx = cur_pane_idx;
}
int get_filer_cur_pane_idx()
{
	int cur_pane_idx = cur_filer_panes->cur_pane_idx;
	if (cur_pane_idx < 0) {
		cur_pane_idx = 0;
	}
	return cur_pane_idx;
}
PRIVATE void init_filer_view(filer_view_t *fv, const char *cur_dir)
{
	memset(fv, 0x00, sizeof(*fv));
	strlcpy__(fv->cur_dir, cur_dir, MAX_PATH_LEN);
	strcpy__(fv->cur_filter, "");
	fv->file_list_entries = 0;
	fv->file_list = NULL;
	fv->cur_sel_idx = -1;
	fv->top_idx = 0;
	strcpy__(fv->prev_dir, "");
	strcpy__(fv->next_file, "");
}
filer_view_t *get_filer_view(int pane_idx)
{
	if (pane_idx < 0) {
		return get_cur_filer_view();
	}
	return &cur_filer_panes->filer_views[pane_idx];
	////cur_fv = &cur_filer_panes->filer_views[pane_idx];
	////return cur_fv;
}
filer_view_t *get_cur_filer_view(void)
{
	return &cur_filer_panes->filer_views[get_filer_cur_pane_idx()];
	////cur_fv = &cur_filer_panes->filer_views[get_filer_cur_pane_idx()];
	////return cur_fv;
}

PRIVATE filer_panes_t *inherit_filer_panes(filer_panes_t *next_fps)
{
	filer_panes_t *prev_fps = cur_filer_panes;	// previous file panes

	init_filer_panes(next_fps, prev_fps->filer_views[prev_fps->cur_pane_idx].cur_dir);
	return prev_fps;
}
PRIVATE void free_filer_panes(filer_panes_t *fps, filer_panes_t *prev_fps)
{
	int filer_pane_idx;

	for (filer_pane_idx = 0; filer_pane_idx < FILER_PANES; filer_pane_idx++) {
		free_file_list(&fps->filer_views[filer_pane_idx]);
	}
	cur_filer_panes = prev_fps;
}
filer_view_t *get_other_filer_view(void)
{
	return &cur_filer_panes->filer_views[get_other_filer_pane_idx(get_filer_cur_pane_idx())];
}
PRIVATE int get_other_filer_pane_idx(int filer_pane_idx)
{
	return filer_pane_idx == 0 ? 1 : 0;
}

#if 0
#ifdef ENABLE_DEBUG
PRIVATE void dump_filer_view(filer_view_t *fv)
{
flf_d_printf("fv:%p\n", fv);
flf_d_printf("cur_dir:[%s]\n", fv->cur_dir);
flf_d_printf("file_list_entries: %d\n", fv->file_list_entries);
flf_d_printf("file_list: %p\n", fv->file_list);
flf_d_printf("top_idx:[%d]\n", fv->top_idx);
flf_d_printf("cur_sel_idx:[%d]\n", fv->cur_sel_idx);
flf_d_printf("next_file:[%s]\n", fv->next_file);
}
PRIVATE void dump_filer_panes(void)
{
	flf_d_printf("cur_filer_panes: %p\n", cur_filer_panes);
	flf_d_printf("cur_pane_idx: %d\n", cur_filer_panes->cur_pane_idx);
	flf_d_printf("get_cur_filer_view(): %p\n", get_cur_filer_view());
}
#endif // ENABLE_DEBUG
#endif // 0

//-----------------------------------------------------------------------------

int call_filer(int push_win, int list_mode,
 char *dir, char *filter, char *file_path, int buf_len)
{
	filer_panes_t *prev_fps = NULL;
	filer_panes_t next_filer_panes;
	app_mode_t appmode_save;
	int ret;

	if (push_win) {
		win_push_win_size();

		prev_fps = inherit_filer_panes(&next_filer_panes);
		////set_cur_filer_view();
	}

	memcpy(&appmode_save, &app_mode__, sizeof(app_mode__));
	SET_APPMD(app_EDITOR_FILER);
	SET_APPMD_VAL(app_LIST_MODE, list_mode);
	SET_APPMD_VAL(ed_EDITOR_PANES, 0);
	set_app_func_key_table();

flf_d_printf("push_win:%d, list_mode:%d\n", push_win, list_mode);
flf_d_printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	ret = filer_main_loop(dir, filter, file_path, buf_len);
flf_d_printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	filer_do_next = FILER_DO_NOTHING;

	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));
	SET_APPMD_VAL(app_LIST_MODE, GET_APPMD_PTR(&appmode_save, app_LIST_MODE));
	SET_APPMD_VAL(ed_EDITOR_PANES, GET_APPMD_PTR(&appmode_save, ed_EDITOR_PANES));
	set_app_func_key_table();

	if (push_win) {
		free_filer_panes(&next_filer_panes, prev_fps);
		////set_cur_filer_view();
		change_cur_dir(get_cur_filer_view()->cur_dir);

		win_pop_win_size();
	}

	return ret;
}

//-----------------------------------------------------------------------------

PRIVATE int filer_main_loop(const char *directory, const char *filter,
 char *file_path, int buf_len)
{
#ifdef ENABLE_HISTORY
	char prev_cur_dir[MAX_PATH_LEN+1];
#endif // ENABLE_HISTORY
	key_code_t key_input;
	func_key_table_t *func_key_table;
	int file_idx;

#ifdef ENABLE_HISTORY
	get_cur_dir(prev_cur_dir);		// memorize prev. current dir
#endif // ENABLE_HISTORY
	if (is_strlen_not_0(directory)) {
		strlcpy__(get_cur_filer_view()->cur_dir, directory, MAX_PATH_LEN);
	}

	filer_do_next = FILER_DO_REFRESH_FORCE;

	while (1) {
		check_filer_cur_dir();
		cat_dir_and_file(filer_cur_path, MAX_PATH_LEN, get_cur_filer_view()->cur_dir, filter);
#ifdef ENABLE_HISTORY
		if (strcmp(prev_cur_dir, get_cur_filer_view()->cur_dir) != 0) {
			update_history(HISTORY_TYPE_IDX_DIR, get_cur_filer_view()->cur_dir);
			strlcpy__(prev_cur_dir, get_cur_filer_view()->cur_dir, MAX_PATH_LEN);
		}
#endif // ENABLE_HISTORY
		if (filer_do_next >= FILER_DO_UPDATE_SCREEN) {
			update_all_file_list(filter, filer_do_next == FILER_DO_REFRESH_FORCE ? 2
			 : (filer_do_next == FILER_DO_REFRESH_AUTO ? 1 : 0));
		}
		update_screen_filer(1, filer_do_next >= FILER_DO_UPDATE_SCREEN, 1);
		//----------------------------------
		key_input = input_key_wait_return();
		//----------------------------------
		if (key_input >= 0) {
			// some key input
mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));
			clear_status_bar_displayed();
		}

		strcpy__(get_cur_filer_view()->next_file, "");

		filer_do_next = FILER_DO_UPDATE_SCREEN;
		switch (key_input) {
		case K_NONE:
			filer_do_next = FILER_DO_REFRESH_AUTO;
			break;
		case K_UP:
			get_cur_filer_view()->cur_sel_idx = MIN_MAX_(0, get_cur_filer_view()->cur_sel_idx - 1,
			 get_cur_filer_view()->file_list_entries-1);
			break;
		case K_DOWN:
			get_cur_filer_view()->cur_sel_idx = MIN_MAX_(0, get_cur_filer_view()->cur_sel_idx + 1,
			 get_cur_filer_view()->file_list_entries-1);
			break;
		case K_PPAGE:
		case K_LEFT:
			get_cur_filer_view()->cur_sel_idx = MIN_MAX_(0,
			 get_cur_filer_view()->cur_sel_idx - FILER_VERT_SCROLL_LINES,
			 get_cur_filer_view()->file_list_entries-1);
			break;
		case K_NPAGE:
		case K_RIGHT:
			get_cur_filer_view()->cur_sel_idx = MIN_MAX_(0,
			 get_cur_filer_view()->cur_sel_idx + FILER_VERT_SCROLL_LINES,
			 get_cur_filer_view()->file_list_entries-1);
			break;
		case K_HOME:
			do_top_of_list();
			break;
		case K_END:
			do_bottom_of_list();
			break;
		case K_ESC:
			filer_do_next = FILER_DO_ABORT;
			break;
		default:
			filer_do_next = FILER_DO_NOTHING;
			if ((func_key_table = get_func_key_table_from_key(filer_func_key_table,
			 key_input)) == NULL) {
				func_key_table = get_func_key_table_from_key(filer_func_key_table,
				 tolower_if_alpha(key_input));
			}
			if (func_key_table == NULL) {
				disp_status_bar_err(_("Key command not assigned: %04xh"), key_input);
				filer_do_next = FILER_DO_NOTHING;
			} else {
				strlcpy__(get_cur_filer_view()->next_file,
				 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name, MAX_PATH_LEN);
				if (is_app_list_mode() == 0 || func_key_table->list_mode) {
flf_d_printf("CALL_FILER_FUNC [%s]\n", func_key_table->func_id);
					//=========================
					(*func_key_table->func)();			// call function
					//=========================
				}
				unselect_all_files_auto(_FILE_SEL_AUTO_);
			}
			break;
		}
		if (filer_do_next >= FILER_DO_QUIT) {
			break;
		}
	}

flf_d_printf("filer_do_next: %d\n", filer_do_next);
	strcpy__(file_path, "");
	if (filer_do_next == FILER_DO_ABORT) {
		return -1;		// aborted
	}
	if (filer_do_next == FILER_DO_QUIT) {
		return 0;		// quitted
	}
	if (filer_do_next == FILER_DO_ENTERED_FILE
	 || filer_do_next == FILER_DO_ENTERED_FILE_PATH) {
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (filer_do_next == FILER_DO_ENTERED_FILE) {
				// file-1 "file name 2" "file name 3"
				concat_file_name_separating_by_space(file_path, buf_len,
				 get_cur_filer_view()->file_list[file_idx].file_name);
			} else {
				// /dir/to/file-path-1 "/dir/to/file path 2" "/dir/to/file path 3"
				char path[MAX_PATH_LEN];

				cat_dir_and_file(path, MAX_PATH_LEN,
				 get_cur_filer_view()->cur_dir,
				 get_cur_filer_view()->file_list[file_idx].file_name);
				concat_file_name_separating_by_space(file_path, buf_len, path);
			}
		}
	}
	if (filer_do_next == FILER_DO_ENTERED_DIR_PATH) {
		strlcpy__(file_path, get_cur_filer_view()->cur_dir, MAX_PATH_LEN);
	}
flf_d_printf("[%s]\n", file_path);
	return 1;		// file or dir is entered
}

PRIVATE int check_filer_cur_dir(void)
{
	// check if cur_dir is readable
	if (is_dir_readable(get_cur_filer_view()->cur_dir) == 0) {
		// current directory is not readable or disappeared
		while (is_dir_readable(get_cur_filer_view()->cur_dir) == 0) {
			// go up to the root dir
			do_parent_directory();
///			if (strcmp(get_cur_filer_view()->cur_dir, get_cur_filer_view()->next_file) == 0)
///				break;		// the same dir
		}
		tio_beep();
	}
	if (is_dir_readable(get_cur_filer_view()->cur_dir) == 0) {
		get_cur_dir(get_cur_filer_view()->cur_dir);
	}
	change_cur_dir(get_cur_filer_view()->cur_dir);
	return 0;
}

PRIVATE int update_all_file_list(const char *filter, int force_update)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		update_file_list(get_cur_filer_view(), filter, force_update);
	} else {
		update_file_list(get_filer_view(0), filter, force_update);
		update_file_list(get_filer_view(1), filter, force_update);
	}
	return 0;
}
PRIVATE int update_file_list(filer_view_t *fv, const char *filter, int force_update)
{
	int files = 0;

	if (get_cur_filer_view()->file_list == NULL
	 || strcmp(fv->listed_dir, fv->cur_dir) != 0
	 || (force_update == 1 && get_files_selected(fv) == 0)
	 || force_update >= 2) {
		files = make_file_list(fv, filter);
		strcpy__(fv->listed_dir, fv->cur_dir);
		sort_file_list(fv);
		research_file_name_in_file_list(fv);
	}
	return files;
}

int update_screen_filer(int title_bar, int status_bar, int refresh)
{
	int files_selected;
	int pane_sel_idx;		// 0: not current pane, 1: current pane
	int pane_idx;			// pane index

	tio_set_cursor_on(0);

	files_selected = get_files_selected_cfv();

	// title bar
	disp_filer_title_bar(filer_cur_path,
	 get_cur_filer_view()->cur_sel_idx, files_selected, get_cur_filer_view()->file_list_entries);

	if (GET_APPMD(fl_FILER_PANES) == 0) {		// 1 pane
		win_select_win(WIN_IDX_SUB_WHOLE);
		disp_file_list(get_cur_filer_view(), 1);
	} else {									// 2 panes
		for (pane_sel_idx = 0; pane_sel_idx < FILER_PANES; pane_sel_idx++) {
			// 1st, update not current pane.
			// 2nd, update current pane.
			if (pane_sel_idx == 0) {
				pane_idx = get_filer_cur_pane_idx() == 0 ? 1 : 0;	// not current pane
			} else {
				pane_idx = get_filer_cur_pane_idx();	// current pane
			}
			win_select_win(WIN_IDX_SUB_LEFT + pane_idx);
			if (pane_sel_idx == 0) {
				// not current pane
				set_work_space_color_low();
			} else {
				// current pane
				set_work_space_color_normal();
			}
			disp_file_list(&cur_filer_panes->filer_views[pane_idx], pane_sel_idx);
		}
	}
	// status bar
	if (status_bar) {
		disp_status_bar_percent_filer(get_cur_filer_view()->cur_sel_idx,
		 get_cur_filer_view()->file_list_entries - 1,
		 "%s", file_info_str(&(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx]),
		 0, 1, 0));
	}
	// key list
	disp_key_list_filer();

	// Set cursor position
	sub_win_set_cursor_pos(filer_win_get_file_list_y()
	 + get_cur_filer_view()->cur_sel_idx
	 - get_cur_filer_view()->top_idx, 0);

	set_work_space_color_normal();

	if (refresh) {
		tio_refresh();
	}

	tio_set_cursor_on(1);
	return 0;
}

#define HHCMMCSS_LEN		8	// "23:59:59"
PRIVATE void disp_filer_title_bar(const char *path,
 int cur_idx, int files_selected, int files_total)
{
	int space;
	char separator_char;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_dir[MAX_PATH_LEN+1];
	char buf_files[MAX_SCRN_LINE_BUF_LEN+1];
	char buf_time[HHCMMCSS_LEN+1];

	set_title_bar_color_by_state(0);
	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0,
	 tio_blank_line(), main_win_get_columns());

	//-------------------------------------------------------------------------
	separator_char = ':';
	if (is_app_list_mode()) {
		separator_char = '.';
	}
#ifdef ENABLE_DEBUG
	if (GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF) {
		separator_char = ';';
		if (is_app_list_mode()) {
			separator_char = ',';
		}
	}
#endif // ENABLE_DEBUG
	snprintf_(buf_dir, MAX_SCRN_LINE_BUF_LEN, "%s%d%c%s",
	 root_notation(), get_filer_cur_pane_idx()+1, separator_char, path);

	// current time
	strlcpy__(buf_time, cur_ctime(), HHCMMCSS_LEN);

	//-------------------------------------------------------------------------
	snprintf_(buf_files, MAX_SCRN_LINE_BUF_LEN+1, "%d/%d:%d %c %s",
	 cur_idx, files_total, files_selected, *get_str_sort_by(), buf_time);
	space = LIM_MIN(0, main_win_get_columns() - strnlen(buf_files, MAX_SCRN_LINE_BUF_LEN) - 1);
	shrink_str(buf_dir, space, 2);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN, "%-*s %s", space, buf_dir, buf_files);

	main_win_output_string(main_win_get_top_win_y() + TITLE_LINE, 0, buffer, -1);
}

PRIVATE int disp_file_list(filer_view_t *fv, int cur_pane)
{
	int line_idx;
	int cur_sel_idx;
	int bottom_idx;
	int file_idx;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char *ptr;

	cur_sel_idx = fv->cur_sel_idx;
	if (cur_sel_idx < fv->top_idx + FILER_VERT_SCROLL_MARGIN_LINES) {
		fv->top_idx = LIM_MIN(0, cur_sel_idx - FILER_VERT_SCROLL_MARGIN_LINES);
	}
	bottom_idx = fv->top_idx + (filer_win_get_file_list_lines()-1);
	if (bottom_idx - FILER_VERT_SCROLL_MARGIN_LINES < cur_sel_idx) {
		bottom_idx = LIM_MAX(fv->file_list_entries-1,
		 cur_sel_idx) + FILER_VERT_SCROLL_MARGIN_LINES;
		fv->top_idx = LIM_MIN(0, bottom_idx - (filer_win_get_file_list_lines()-1));
	}

	set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
	sub_win_clear_screen();

	if (filer_win_get_file_path_lines()) {
		// on two pane mode, show each path
		strlcpy__(buffer, fv->cur_dir, MAX_SCRN_LINE_BUF_LEN);
		shrink_str(buffer, main_win_get_columns(), 2);
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, cur_pane);
		sub_win_output_string(filer_win_get_file_path_y(), 0, buffer, -1);
	}

	for (line_idx = 0, file_idx = fv->top_idx;
	 line_idx < filer_win_get_file_list_lines() && file_idx < fv->file_list_entries;
	 line_idx++, file_idx++) {
		// Highlight the currently selected file/dir.
		ptr = file_info_str(&(fv->file_list[file_idx]),
		 1, file_idx != cur_sel_idx, fv->file_list[file_idx].selected);
		adjust_utf8s_columns(ptr, main_win_get_columns());
		if (fv->file_list[file_idx].selected) {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
		} else {
			set_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, 0);
		}
		if (cur_pane && file_idx == cur_sel_idx) {
			tio_set_attr_rev(1);		// display current line inverted
		}
		sub_win_output_string(filer_win_get_file_list_y() + line_idx, 0, ptr, -1);
		if (cur_pane && file_idx == cur_sel_idx) {
			tio_set_attr_rev(0);
		}
	}
	if (cur_pane) {
		// Set cursor position
		sub_win_set_cursor_pos(filer_win_get_file_list_y() + cur_sel_idx - fv->top_idx, 0);
	}
	return 0;
}
//-------------------------------------
PRIVATE int filer_win_get_file_path_lines(void)
{
	if (GET_APPMD(fl_FILER_PANES) == 0) {
		return 0;
	} else {
		return 1;
	}
}
PRIVATE int filer_win_get_file_list_lines(void)
{
	return LIM_MIN(0, sub_win_get_lines() - filer_win_get_file_path_lines());
}
//-------------------------------------
PRIVATE int filer_win_get_file_path_y(void)
{
	return 0;
}
PRIVATE int filer_win_get_file_list_y(void)
{
	return filer_win_get_file_path_y() + filer_win_get_file_path_lines();
}
//-------------------------------------
PRIVATE void disp_key_list_filer(void)
{
	char *filer_key_lists[] = {
 "{Menu}  {Edit } {Copy } {CpyUd} {Renam}  {Move } {Delet} {MkDel} {MkDir} "
 " {ChDir} {Exec } {Run  } {Home }",

 "<do_quit_filer>Quit "
 "<do_edit_file>Edit "
 "<do_edit_new_file>EditNewFile "
 "<do_copy_file>Copy "
 "<do_copy_file_update>UpdateCopy "
 "<do_rename_file>Rename "
 "<do_move_file>Move "
 "<do_trash_file>Trash "
 "<do_delete_file>Delete "
 "<do_mark_to_delete_file>MarkToDelete "
 "<do_exec_command_with_file>Exec "
 "<do_run_command_rel>Run "
 "<do_select_file>SelectFile "
 "<do_select_no_file>SelNoFile "
 "<do_select_all_files>SelAllFiles ",

 "<do_home_directory>HomeDir "
 "<do_root_directory>RootDir "
 "<do_change_directory>ChgDir "
 "<do_parent_directory>ParentDir "
 "<do_enter_file>Pager/EnterDir "
 "<do_make_directory>MkDir "
 "<do_tog_filer_panes>TwoPane "
 "<do_switch_filer_pane>SwPane "
 "<do_filer_inc_sort_by>Sort "
 "<do_filer_inc_show_file_info>Info "
 "<do_refresh_filer>Refresh "
 "<do_filer_tog_show_dot_file>ShowDotFile "
 "<do_find_file>FindFile ",
	};
	disp_key_list(filer_key_lists);
}

#endif // ENABLE_FILER

// End of filer.c
/**************************************************************************
 *   filer2.c                                                             *
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

#include "headers.h"

#ifdef ENABLE_FILER

void begin_fork_exec_repeat(void)
{
	restore_term_for_shell();
	clear_fork_exec_counter();
	clear_handler_sigint_called();
}
void end_fork_exec_repeat(void)
{
	pause_after_exec();
	reinit_term_for_filer();
}

//-----------------------------------------------------------------------------

#define MAX_EXECV_ARGS		10

PRIVATE int fork_exec_sh_c(int set_term, int separate_bef_exec, int pause_aft_exec,
 const char *command);
PRIVATE int args_from_va_list(char **args, va_list ap);
PRIVATE int fork_execv_hist(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const *args);
#ifdef ENABLE_HISTORY
PRIVATE void output_exec_args_history(char * const *args);
#endif // ENABLE_HISTORY
PRIVATE int fork_execv(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[]);

int fork_exec_once_sh_c(int separate_bef_exec, int pause_aft_exec, const char *command)
{
	clear_fork_exec_counter();
	return fork_exec_sh_c(SETTERM1, separate_bef_exec, pause_aft_exec, command);
}
int fork_exec_repeat_sh_c(int separate_bef_exec, const char *command)
{
	return fork_exec_sh_c(SETTERM0, separate_bef_exec, PAUSE0, command);
}
PRIVATE int fork_exec_sh_c(int set_term, int separate_bef_exec, int pause_aft_exec,
 const char *command)
{
	char *args[MAX_EXECV_ARGS+1];

	// sh -c "command arg1 arg2 arg3"
	args[0] = "sh";
	args[1] = "-c";
	args[2] = (char *)command;
	args[3] = NULL;

#ifdef ENABLE_HISTORY
	if (get_fork_exec_counter() == 0) {
		update_history(HISTORY_TYPE_IDX_EXEC, command);
	}
#endif // ENABLE_HISTORY
	return fork_execv(set_term, separate_bef_exec, pause_aft_exec, args);
}
int fork_exec_once(int separate_bef_exec, int pause_aft_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];

	clear_fork_exec_counter();

	va_start(ap, pause_aft_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_execv_hist(SETTERM1, separate_bef_exec, pause_aft_exec, args);
}
int fork_exec_repeat(int separate_bef_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];

	va_start(ap, separate_bef_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_execv_hist(SETTERM0, separate_bef_exec, PAUSE0, args);
}

// convert va_list to "char *argv[]"
PRIVATE int args_from_va_list(char **args, va_list ap)
{
	int arg_idx;

	for (arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		args[arg_idx] = va_arg(ap, char *);
		if (args[arg_idx] == NULL)
			break;
	}
	args[arg_idx] = NULL;
	return arg_idx;
}

PRIVATE int fork_execv_hist(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const *args)
{
#ifdef ENABLE_HISTORY
	if (get_fork_exec_counter() == 0) {
		output_exec_args_history(args);
	}
#endif // ENABLE_HISTORY
	return fork_execv(set_term, separate_bef_exec, pause_aft_exec, args);
}

#ifdef ENABLE_HISTORY
PRIVATE void output_exec_args_history(char * const *args)
{
	int arg_idx;
	const char *arg;
	char buffer[MAX_PATH_LEN+1];

	buffer[0] = '\0';
	for (arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		arg = args[arg_idx];
		if (arg == NULL)
			break;
		concat_file_name_separating_by_space(buffer, MAX_PATH_LEN, arg);
	}
	update_history(HISTORY_TYPE_IDX_EXEC, buffer);
}
#endif // ENABLE_HISTORY

PRIVATE int fork_execv(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[])
{
	pid_t pid;
	int exit_status;
	int ret;

	if (set_term && get_fork_exec_counter() == 0) {
		restore_term_for_shell();
	}
	if (separate_bef_exec > 0 && get_fork_exec_counter() == 0) {
		// output separator line
		printf("\n-------- command execution started here --------\n");
		fflush(stdout);
	}

	exit_status = -10000;
	if ((pid = fork()) == 0) {
		signal_clear();
		init_stderr();
///flf_d_printf("args[0]: %s\n", args[0]);
		execvp(args[0], args);
		exit(-10001);					// execution error
	} else {
		for ( ; ; ) {
			ret = waitpid(pid, &exit_status, 0);
mflf_d_printf("ret: %d, exit_status: %d\n", ret, exit_status);
			if (ret != -1)
				break;
		}
	}

	if (pause_aft_exec > 0) {
		printf("\n\aStatus:%d ", exit_status);
		pause_after_exec();
	}
	if (set_term && get_fork_exec_counter() == 0) {
		reinit_term_for_filer();
	}
	inc_fork_exec_counter();
	return exit_status;
}

PRIVATE int fork_exec_counter = 0;
void clear_fork_exec_counter(void)
{
	fork_exec_counter = 0;
}
int get_fork_exec_counter(void)
{
	return fork_exec_counter;
}
int inc_fork_exec_counter(void)
{
	return fork_exec_counter++;
}

void pause_after_exec(void)
{
	set_term_raw();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	getchar();	getchar();	getchar();	getchar();	getchar();
	getchar();	getchar();	getchar();	getchar();	getchar();
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	printf(
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
 "======== Hit any key to return to %s ======== ", APP_NAME);
	fflush(stdout);
	getchar();
	printf("\n");
}

//-----------------------------------------------------------------------------
int restore_term_for_shell(void)
{
	tio_clear_screen();
	tio_set_cursor_on(1);
	tio_suspend();
	signal_fork();
	return 0;
}
int reinit_term_for_filer(void)
{
	signal_init();
	tio_resume();
	tio_clear_flash_screen(1);
	return 0;
}
//-----------------------------------------------------------------------------

PRIVATE char *get_file_size_str(char *buf_size, loff_t size);
PRIVATE const char *get_1k_to_999k_str(long size, char *buf);
#ifdef START_UP_TEST
void test_get_file_size_str(void)
{
	loff_t size;
	loff_t zeros;
	int bits;
	int bit;
#ifdef ENABLE_DEBUG
	char buf_size[20+1];
#endif // ENABLE_DEBUG

	for (bits = 1; bits <= 64; bits++) {
		// 0x01, 0x03, ... 0x7fffffffffffffff, 0xffffffffffffffff
		size = 0;
		for (bit = 0; bit < bits; bit++) {
			size |= ((loff_t)1) << bit;
		}
		flf_d_printf("%16llx ==> %20llu ==> %s\n", size, size, get_file_size_str(buf_size, size));
	}
	//                   PPPPPTTTGGGMMMKKK000
	for (zeros = 1; zeros <= 1000000000000000; zeros *= 10) {
		// 12345, 123450, ... 12,345,000,000,000,000
		size = 12345 * zeros;
		flf_d_printf("%16llx ==> %20llu ==> %s\n", size, size, get_file_size_str(buf_size, size));
	}
}
#endif // START_UP_TEST

// show_link==1: show symlink, ==0: show symlinked file
// truncate==1: truncate file name to show whole of file attribute
char *file_info_str(file_info_t *file_info, int show_link, int trunc_file_name, int selected)
{
	struct stat *st_ptr;
	struct stat *lst_ptr;
	int is_link;
	int is_link_dead = 0;
	struct tm *tm_ptr;
	char buf_name[MAX_PATH_LEN*3+1];		// for UTF8 file name
	loff_t size;
	char buf_size[20+1];
	char buf_time[20+1];
	int mode;
	char *type_str;
	char buf_mode[20+1];
#define FILE_INFO_BUF_LEN	100
	char buf_info[FILE_INFO_BUF_LEN+1];
	int info_str_len;
	char *info_str_ptr;
	int file_name_cols;
	int file_name_space;
	int rest_space_for_file_name;
	int info_space;
// *filename.ext 123456 070113-125959 user---- group---
#define SELECTED_MARK_LEN	1
#define FILE_NAME_INFO_BUF_LEN	(SELECTED_MARK_LEN + MAX_PATH_LEN*3+1 +6+1+(6+1+6)+(8+1+8)+1)
	static char buffer[FILE_NAME_INFO_BUF_LEN+1];
#define MIN_FILE_NAME_SPACE		12		// "filename.ext"(DOS8.3)

	st_ptr = &file_info->st;
	lst_ptr = &file_info->lst;
	is_link = S_ISLNK(lst_ptr->st_mode);
	if (is_link)
		is_link_dead = (memcmp(st_ptr, lst_ptr, sizeof(*st_ptr)) == 0);

	strcpy__(buf_name, "");
	if (show_link && is_link) {
		strlcat__(buf_name, MAX_PATH_LEN*3, file_info->file_name);
		if (S_ISDIR(lst_ptr->st_mode)) {
			strlcat__(buf_name, MAX_PATH_LEN*3, "/");
		} else if (lst_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
			// Symlink's 'x' bits are usually set.
			// strlcat__(buf_name, "*", MAX_PATH_LEN*3);
		}
#define LINK_ARROW		"-->"
		strlcat__(buf_name, MAX_PATH_LEN*3, LINK_ARROW);
		if (file_info->symlink)
			strlcat__(buf_name, MAX_PATH_LEN*3, file_info->symlink);
		if (is_link_dead)
			strlcat__(buf_name, MAX_PATH_LEN*3, "!");
	} else {
		if (is_link)
			strlcat__(buf_name, MAX_PATH_LEN*3, file_info->symlink);
		else
			strlcat__(buf_name, MAX_PATH_LEN*3, file_info->file_name);
	}
	if (S_ISDIR(st_ptr->st_mode)) {
		strlcat__(buf_name, MAX_PATH_LEN*3, "/");
	} else if (is_link_dead == 0
	 && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
		strlcat__(buf_name, MAX_PATH_LEN*3, "*");
	}

	strcpy__(buf_size, "");
	size = show_link ? lst_ptr->st_size : st_ptr->st_size;
///#define TEST_HUGE_SIZE
#ifdef	TEST_HUGE_SIZE
	size *= 1000000000000;
#endif // TEST_HUGE_SIZE
	// 2^32 = 4294967296 = 4e9 = 4Giga, 2^64 = 1844674407000000000 = 18e18 = 18Exa = 18446Peta
	if ((show_link && is_link)		// link itself
	 || (S_ISREG(st_ptr->st_mode) || S_ISDIR(st_ptr->st_mode))) {
		get_file_size_str(buf_size, size);
	} else if (S_ISCHR(st_ptr->st_mode)) {
		size = st_ptr->st_rdev;
		snprintf_(buf_size, 20+1, "%3dC%3d", (int)((size & 0xff00) >> 8), (int)(size & 0xff));
	} else if (S_ISBLK(st_ptr->st_mode)) {
		size = st_ptr->st_rdev;
		snprintf_(buf_size, 20+1, "%3dB%3d", (int)((size & 0xff00) >> 8), (int)(size & 0xff));
	} else if (S_ISFIFO(st_ptr->st_mode)) {
		snprintf_(buf_size, 20+1, "   FIFO");
	} else if (S_ISSOCK(st_ptr->st_mode)) {
		snprintf_(buf_size, 20+1, " SOCKET");
	}

	tm_ptr = localtime(show_link ? &lst_ptr->st_mtime : &st_ptr->st_mtime);
	snprintf_(buf_time, 20+1, "%02d%02d%02d-%02d%02d%02d",
	 tm_ptr->tm_year % 100, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
	 tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);

	mode = show_link ? lst_ptr->st_mode : st_ptr->st_mode;
	if (S_ISREG(mode)) {
		type_str = " ";
	} else if (S_ISDIR(mode)) {
		type_str = "d";
	} else if (S_ISLNK(mode)) {
		type_str = "l";
	} else if (S_ISCHR(mode)) {
		type_str = "c";
	} else if (S_ISBLK(mode)) {
		type_str = "b";
	} else if (S_ISFIFO(mode)) {
		type_str = "f";
	} else if (S_ISSOCK(mode)) {
		type_str = "s";
	} else {
		type_str = "?";		// unknown file type
	}
	switch(GET_APPMD(fl_SHOW_FILE_INFO)) {
	default:
	case SHOW_FILE_INFO_0:
	case SHOW_FILE_INFO_1:
	case SHOW_FILE_INFO_2:
	case SHOW_FILE_INFO_3:
		snprintf_(buf_mode, 20+1, "%s", "");
		break;
	case SHOW_FILE_INFO_4:
		//						   t7777
		snprintf_(buf_mode, 20+1, "%s%04o", type_str, mode & 07777);
		break;
	case SHOW_FILE_INFO_5:
		//						   t r w x r w x r w x
		snprintf_(buf_mode, 20+1, "%s%c%c%c%c%c%c%c%c%c", type_str,
		 (mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-',
		 (mode & S_IXUSR) ? ((mode & S_ISUID) ? 's' : 'x') : ((mode & S_ISUID) ? 'S' : '-'),
		 (mode & S_IRGRP) ? 'r' : '-', (mode & S_IWGRP) ? 'w' : '-',
		 (mode & S_IXGRP) ? ((mode & S_ISGID) ? 's' : 'x') : ((mode & S_ISGID) ? 'S' : '-'),
		 (mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-',
		 (mode & S_IXOTH) ? 'x' : '-');
		break;
	}

///flf_d_printf("<%s %s %s %-8s %-8s>\n", buf_size, buf_time, buf_mode,
/// get_user_name(show_link ? lst_ptr->st_uid : st_ptr->st_uid),
/// get_group_name(show_link ? lst_ptr->st_gid : st_ptr->st_gid));
	switch(GET_APPMD(fl_SHOW_FILE_INFO)) {
	default:
	case SHOW_FILE_INFO_0:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, "%s", " ");
		break;
	case SHOW_FILE_INFO_1:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s", buf_size);
		break;
	case SHOW_FILE_INFO_2:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s", buf_time);
		break;
	case SHOW_FILE_INFO_3:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s %s", buf_size, buf_time);
		break;
	case SHOW_FILE_INFO_4:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s %s %s %-8s",
		 buf_size, buf_time, buf_mode,
		 get_user_name(show_link ? lst_ptr->st_uid : st_ptr->st_uid));
		break;
	case SHOW_FILE_INFO_5:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s %s %s %-8s %-8s",
		 buf_size, buf_time, buf_mode,
		 get_user_name(show_link ? lst_ptr->st_uid : st_ptr->st_uid),
		 get_group_name(show_link ? lst_ptr->st_gid : st_ptr->st_gid));
		break;
	}
	for (info_str_ptr = buf_info; *info_str_ptr; info_str_ptr++) {
		if (info_str_ptr[1] != ' ')	// skip space
			break;
	}
	info_str_len = strnlen(info_str_ptr, FILE_INFO_BUF_LEN);

	file_name_cols = utf8s_columns(buf_name, INT_MAX);
	rest_space_for_file_name = LIM_MIN(0,
	 sub_win_get_columns() - (SELECTED_MARK_LEN + info_str_len));
	if (trunc_file_name) {
		// truncate file name not to overwrap to file info.
		// *file.ext    iiiiiiiiiiiiiiiiiiiiiii
		// *filename.e~ iiiiiiiiiiiiiiiiiiiiiii
		file_name_space = LIM_MIN(LIM_MAX(file_name_cols, MIN_FILE_NAME_SPACE),
		 rest_space_for_file_name);
	} else {
		// overwrap file name to file info.
		// *file.ext    iiiiiiiiiiiiiiiiiiiiiii
		// *filename.ext ~iiiiiiiiiiiiiiiiiiiii
		file_name_space = LIM_MAX(LIM_MIN(file_name_cols, rest_space_for_file_name),
		 sub_win_get_columns() - SELECTED_MARK_LEN);
	}
	if (file_name_cols > file_name_space) {
		truncate_tail_utf8s_columns(buf_name, file_name_space-1);
		strlcat__(buf_name, MAX_PATH_LEN*3, "~");	// add truncated-mark
	}
	expand_utf8s_columns(buf_name, file_name_space);
	info_space = LIM_MIN(0, sub_win_get_columns() - (SELECTED_MARK_LEN + file_name_space));
	if (info_space < strnlen(info_str_ptr, FILE_INFO_BUF_LEN)) {
		// truncate head
		info_str_ptr = &info_str_ptr[strnlen(info_str_ptr, FILE_INFO_BUF_LEN) - info_space];
		if (info_space >= 3) {
			info_str_ptr[0] = ' ';	// truncate-head mark
			info_str_ptr[1] = '~';	// truncate-head mark
		}
	}
	if (show_link == 0 && is_link_dead) {
		strlcpy__(buffer, _(" <<broken-link>>"), FILE_NAME_INFO_BUF_LEN);
	} else {
		snprintf_(buffer, FILE_NAME_INFO_BUF_LEN+1, "%c%s%s",
		 selected ? '*' : ' ', buf_name, info_str_ptr);
	}
///dump_cur_win();
///flf_d_printf("[%s]\n", buffer);
	return buffer;
}
PRIVATE char *get_file_size_str(char *buf_size, loff_t size)
{
	char buf_1_to_999[5+1];		// 1.000 - 999.9 - 10000 - 99999

	if (size < 1000000ULL)					// less than 1 MEGA (0 - 999999)
		snprintf_(buf_size, 6+1, "%6d", (int)size);
	else if (size < 1000000000ULL)			// less than 1000 MEGA (1.000M - 999.9M)
		snprintf_(buf_size, 6+1, "%sM", get_1k_to_999k_str(size / 1000ULL, buf_1_to_999));
	else if (size < 1000000000000ULL)		// less than 1000 GIGA (1.000G - 999.9G)
		snprintf_(buf_size, 6+1, "%sG", get_1k_to_999k_str(size / 1000000ULL, buf_1_to_999));
	else if (size < 1000000000000000ULL)	// less than 1000 TERA (1.000T - 9999.9T)
		snprintf_(buf_size, 6+1, "%sT", get_1k_to_999k_str(size / 1000000000ULL, buf_1_to_999));
	else if (size < 1000000000000000000ULL)	// less than 1000 PETA (1.000P - 9999.9P)
		snprintf_(buf_size, 6+1, "%sP", get_1k_to_999k_str(size / 1000000000000ULL, buf_1_to_999));
	else									// more than 1000 PETA (100P - 18446P)
		snprintf_(buf_size, 6+1, "%sP", get_1k_to_999k_str(size / 1000000000000ULL, buf_1_to_999));
	return buf_size;
}
PRIVATE const char *get_1k_to_999k_str(long size, char *buf)
{
	if (size < 10000UL)					// less than 10 K (1.000K - 9.999K)
		snprintf_(buf, 5+1, "%1d.%03d",
		 (int)(size / 1000), (int)((size / 1) % 1000));
	else if (size < 100000UL)			// less than 100 K (10.00M - 99.99G)
		snprintf_(buf, 5+1, "%2d.%02d",
		 (int)(size / 1000), (int)((size / 10) % 100));
	else if (size < 1000000UL)			// less than 1000 K (100.0K - 999.9K)
		snprintf_(buf, 5+1, "%3d.%1d",
		 (int)(size / 1000), (int)((size / 100) % 10));
	else if (size < 1000 * 99999ULL)	// 1000K - 99999K
		snprintf_(buf, 5+1, "%5d", (int)(size / 1000));
	return buf;
}
//-----------------------------------------------------------------------------
int make_file_list(filer_view_t *fv, const char *filter)
{
	char dir_save[MAX_PATH_LEN+1];
	DIR *dir;
	struct dirent *dirent;
	char symlink[MAX_PATH_LEN+1];
	file_info_t *ent_ptr;	// file entry
	struct stat st;
	struct stat lst;
	int entries;
	int file_idx;
	int len;

	save_change_cur_dir(dir_save, fv->cur_dir);

	free_file_list(fv);

	if (strcmp(filter, "") == 0)
		filter = "*";
	if ((dir = opendir(fv->cur_dir)) == NULL) {
		goto make_file_list_ret;
	}
	for (file_idx = 0; (dirent = readdir(dir)) != NULL; file_idx++) {
		// count files
	}
	entries = file_idx;
	fv->file_list = (file_info_t *)malloc__(sizeof(file_info_t) * entries);

	rewinddir(dir);
	for (file_idx = 0; file_idx < entries && (dirent = readdir(dir)) != NULL; ) {
		if (lstat(dirent->d_name, &lst) < 0)	// stat link itself
			continue;
		if (S_ISLNK(lst.st_mode)
		 && stat(dirent->d_name, &st) >= 0) {	// stat linked file
			// stating file succeeded
		} else {
			// copy from lst to st
			memcpy__(&st, &lst, sizeof(struct stat));
		}
		if (S_ISDIR(st.st_mode)
		 || (S_ISDIR(st.st_mode) == 0 && fnmatch(filter, dirent->d_name, 0) == 0)) {
			if (GET_APPMD(fl_SHOW_DOT_FILE) == 0
			 && ((strncmp(dirent->d_name, ".", 1) == 0 && strcmp(dirent->d_name, "..") != 0)
			  || ((st.st_mode & RWXRWXRWX) == 0)
			  || ((st.st_mode & RWXRWXRWX) == RW0000RW0)))
				// ".", ".????" or (mode == 000)
				continue;
			ent_ptr = &fv->file_list[file_idx];
			// fill file_info_t
			ent_ptr->file_name = malloc_strcpy(dirent->d_name);
			memcpy__(&ent_ptr->st, &st, sizeof(struct stat));
			memcpy__(&ent_ptr->lst, &lst, sizeof(struct stat));
			ent_ptr->symlink = NULL;
			if (S_ISLNK(lst.st_mode)) {
				if ((len = readlink__(dirent->d_name, symlink, MAX_PATH_LEN)) > 0) {
					ent_ptr->symlink = malloc_strcpy(symlink);
				}
			}
			ent_ptr->selected = 0;
			file_idx++;
		}
	}
	closedir(dir);
	fv->file_list_entries = file_idx;

make_file_list_ret:;
	change_cur_dir(dir_save);
	return fv->file_list_entries;
}
// Free malloc()ed memory
void free_file_list(filer_view_t *fv)
{
	int file_idx;

	if (fv->file_list) {
		for (file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
			if (fv->file_list[file_idx].file_name) {
				FREE_CLR_PTR(fv->file_list[file_idx].file_name);
			}
			if (fv->file_list[file_idx].symlink) {
				FREE_CLR_PTR(fv->file_list[file_idx].symlink);
			}
		}
		FREE_CLR_PTR(fv->file_list);
	}
	fv->file_list_entries = 0;
}

//-----------------------------------------------------------------------------
// sort file list
PRIVATE int comp_file_name(const void *aa, const void *bb);
PRIVATE int comp_file_extension(const void *aa, const void *bb);
PRIVATE int comp_file_time(const void *aa, const void *bb);
PRIVATE int comp_file_size(const void *aa, const void *bb);
PRIVATE int comp_file_type(file_info_t *aa, file_info_t *bb);
PRIVATE int comp_file_executable(file_info_t *aa, file_info_t *bb);
PRIVATE int get_file_type_no(struct stat *st);
PRIVATE int get_file_executable(struct stat *st);
PRIVATE int strtypecasecmp(const char *s1, const char *s2);
void sort_file_list(filer_view_t *fv)
{
	switch(GET_APPMD(fl_FILE_SORT_BY)) {
	case FILE_SORT_BY_NAME:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_name);
		break;
	case FILE_SORT_BY_EXT:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_extension);
		break;
	case FILE_SORT_BY_TIME:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_time);
		break;
	case FILE_SORT_BY_SIZE:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_size);
		break;
	}
}

// Comparison functions for file listings ------------------------------------------

// sort directories before files,
// sort by file name.
PRIVATE int comp_file_name(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	return strtypecasecmp(((file_info_t *)aa)->file_name, ((file_info_t *)bb)->file_name);
}
// sort directories before files,
// sort executable files before non-executables,
// sort by file extension then by file name.
PRIVATE int comp_file_extension(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;
	if ((ret = comp_file_executable((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	if ((ret = strcmp(get_file_name_extension(((file_info_t *)aa)->file_name),
	 get_file_name_extension(((file_info_t *)bb)->file_name))) != 0)
		return ret;
	return comp_file_name(aa, bb);
}
// sort directories before files,
// and then by modification time stamp.
PRIVATE int comp_file_time(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	return ((file_info_t *)aa)->st.st_mtime - ((file_info_t *)bb)->st.st_mtime;
}
// sort directories before files,
// and then by file size.
PRIVATE int comp_file_size(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	return ((file_info_t *)aa)->st.st_size - ((file_info_t *)bb)->st.st_size;
}
// compare file type
PRIVATE int comp_file_type(file_info_t *aa, file_info_t *bb)
{
	int file_type_a;
	int file_type_b;

	file_type_a = get_file_type_no(&aa->st);
	file_type_b = get_file_type_no(&bb->st);
	return file_type_a - file_type_b;
}
// sort executable files before non-executables
PRIVATE int comp_file_executable(file_info_t *aa, file_info_t *bb)
{
	int file_type_a;
	int file_type_b;

	file_type_a = get_file_executable(&aa->st);
	file_type_b = get_file_executable(&bb->st);
	return file_type_a - file_type_b;
}
// rank by file type
PRIVATE int get_file_type_no(struct stat *st)
{
	if (S_ISDIR(st->st_mode))
		return 1;
	if (S_ISLNK(st->st_mode))
		return 2;
	if (S_ISFIFO(st->st_mode))
		return 3;
	if (S_ISSOCK(st->st_mode))
		return 4;
	if (S_ISCHR(st->st_mode))
		return 5;
	if (S_ISBLK(st->st_mode))
		return 5;
	if (S_ISREG(st->st_mode))
		return 7;
	return 0;
}
PRIVATE int get_file_executable(struct stat *st)
{
	if (st->st_mode & S_IXUSR)
		return 0;
	return 1;
}
// smaller:
//  ispunct()
//  isdigit()
//  isalpha()
// larger:
PRIVATE int strtypecasecmp(const char *s1, const char *s2)
{
	int diff;

	for ( ; *s1 != '\0' && *s2 != '\0'; s1++, s2++) {
		if ((diff = ispunct(*s1) - ispunct(*s2)) != 0) {
			return -diff;
		}
		if (tolower(*s1) != tolower(*s2))
			break;
	}
	return (tolower(*s1) - tolower(*s2));
}

//-----------------------------------------------------------------------------

int get_files_selected_cfv(void)
{
	return get_files_selected(get_cur_filer_view());
}
int get_files_selected(filer_view_t *fv)
{
	int idx;
	int files_selected = 0;

	for (idx = 0; idx < fv->file_list_entries; idx++) {
		if (fv->file_list[idx].selected) {
			files_selected++;
		}
	}
	return files_selected;
}

int select_and_get_first_file_idx_selected(void)
{
	select_file_if_none_selected();
	return get_first_file_idx_selected();
}
int get_first_file_idx_selected(void)
{
	int file_idx;

	for (file_idx = 0; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		if (get_cur_filer_view()->file_list[file_idx].selected)
			break;
	}
	if (file_idx < get_cur_filer_view()->file_list_entries)
		return file_idx;
	// no file selected, return current file
	return get_cur_filer_view()->cur_sel_idx;
}
int get_next_file_idx_selected(int start_file_idx)
{
	int file_idx = start_file_idx < 0 ? 0 : start_file_idx+1;

	for ( ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		if (get_cur_filer_view()->file_list[file_idx].selected)
			break;
	}
	if (file_idx < get_cur_filer_view()->file_list_entries)
		return file_idx;
	return -1;	// no selected file found
}
int select_file_if_none_selected(void)
{
	int files_selected;
	int sel_idx = get_cur_filer_view()->cur_sel_idx;

	files_selected = get_files_selected_cfv();
	if (files_selected == 0) {
		get_cur_filer_view()->file_list[sel_idx].selected = _FILE_SEL_AUTO_;
	}
	return files_selected;
}
void unselect_all_files_auto(char selection_bit)
{
	int file_idx;

	for (file_idx = 0 ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		get_cur_filer_view()->file_list[file_idx].selected
		 = get_cur_filer_view()->file_list[file_idx].selected & ~selection_bit;
	}
}

//-----------------------------------------------------------------------------

int research_file_name_in_file_list(filer_view_t *fv)
{
	int cur_sel_idx;

	cur_sel_idx = search_file_name_in_file_list(fv, fv->next_file);
	if (cur_sel_idx < 0) {
		if (fv->cur_sel_idx >= 0)
			cur_sel_idx = fv->cur_sel_idx;
		else
			cur_sel_idx = 0;
	}
	cur_sel_idx = MIN_MAX_(0, cur_sel_idx, fv->file_list_entries-1);
	fv->cur_sel_idx = cur_sel_idx;
	return 0;
}

int search_file_name_in_file_list(filer_view_t *fv, const char *file_name)
{
	int file_idx;
	int file_name_len;
	// 0,1: regular <-> non-regular file, 2,3: all file
	// 0,2: case sensitive, 1,3: case ignorant
	// 0: the same file type    , case sensitive
	// 1: the same file type    , case ignorant
	// 2: may diffrent file type, case sensitive
	// 3: may diffrent file type, case ignorant
	int cmp_type;

	// exact match
	for (file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
		if (strcmp(fv->file_list[file_idx].file_name, file_name) == 0) {
			return file_idx;
		}
	}
	// partial match in regular file
	for (cmp_type = 0; cmp_type < 4; cmp_type++) {
		for (file_name_len = strlen(file_name); file_name_len; file_name_len--) {
			for (file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
				if (cmp_type < 2) {		// cmp_type = 0, 1
					if ((S_ISREG(get_cur_filer_view()
					   ->file_list[get_cur_filer_view()->cur_sel_idx].st.st_mode)
					  == S_ISREG(get_cur_filer_view()
					   ->file_list[file_idx].st.st_mode))
					 && (cmp_type == 0
						// case sensitive
					  ? strncmp(fv->file_list[file_idx].file_name, file_name, file_name_len) == 0
						// case ignorant
					  : strncasecmp(fv->file_list[file_idx].file_name, file_name,
						 file_name_len) == 0)) {
						return file_idx;
					}
				} else {	// cmp_type = 2, 3
					if (cmp_type == 2
						// case sensitive
					  ? strncmp(fv->file_list[file_idx].file_name, file_name, file_name_len) == 0
						// case ignorant
					  : strncasecmp(fv->file_list[file_idx].file_name, file_name,
						 file_name_len) == 0) {
						return file_idx;
					}
				}
			}
		}
	}
	return -1;
}

#endif // ENABLE_FILER

// End of filer2.c
/**************************************************************************
 *   filercmd.c                                                           *
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

#include "headers.h"

#ifdef ENABLE_FILER

PRIVATE int do_edit_file_(int recursive);

PRIVATE int if_dir_change_dir(void);
PRIVATE int filer_change_dir_if_not_yet(const char *dir);
PRIVATE int filer_change_prev_dir(void);

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"

#define BECMD		"becmd"		// becmd?

int do_top_of_list(void)
{
	get_cur_filer_view()->cur_sel_idx = 0;
	return 1;
}
int do_bottom_of_list(void)
{
	get_cur_filer_view()->cur_sel_idx = get_cur_filer_view()->file_list_entries-1;
	return 1;
}

int do_switch_filer_pane(void)
{
	set_filer_cur_pane_idx((get_filer_cur_pane_idx() + 1) % FILER_PANES);
	////set_cur_filer_view();
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 1;
}
int do_refresh_filer(void)
{
	disp_status_bar_done(_("File view refreshed"));
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 1;
}
int do_enter_file(void)
{
	if (if_dir_change_dir()) {
		return 0;
	}
	if (S_ISREG(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].st.st_mode)) {
		if (count_edit_bufs() == 0) {
			return do_view_file();
		} else {
			return do_edit_file_non_recursive();
		}
	}
	return 0;
}
int do_edit_file(void)
{
	do_edit_file_(RECURSIVE1);
	return 1;
}
int do_edit_file_non_recursive(void)
{
	do_edit_file_(RECURSIVE0);
	return 1;
}
int do_edit_new_file(void)
{
	char file_name[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_name, HISTORY_TYPE_IDX_DIR, _("Edit new file:"));

	if (ret < 0) {
		// do_edit_new_file -> FILER_DO_ENTERED_DIR_PATH
		filer_do_next = FILER_DO_ENTERED_DIR_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	clear_files_loaded();
	if (load_file_name_upp_low(file_name, TUL0, OOE1, MOE0, RECURSIVE0) <= 0) {
		tio_beep();
		return 0;
	}
	disp_files_loaded_ifnon0();
	filer_do_next = FILER_DO_LOADED_FILE;
	return 1;
}
int do_view_file(void)
{
	int file_idx;
	char *file_name;

	if (if_dir_change_dir()) {
		return 0;
	}
	if (is_app_list_mode()) {
		// do_view_file -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	file_idx = get_cur_filer_view()->cur_sel_idx;
	file_name = get_cur_filer_view()->file_list[file_idx].file_name;
	if (S_ISREG(get_cur_filer_view()->file_list[file_idx].st.st_mode)) {
		if (fork_exec_once(SEPARATE1, PAUSE0, BEPAGER, file_name, 0))
			if (fork_exec_once(SEPARATE1, PAUSE0, "less", file_name, 0))
				fork_exec_once(SEPARATE1, PAUSE0, "more", file_name, 0);
	}
	return 0;
}
int do_tail_file(void)	// view file with "tail" command
{
	int file_idx;
	char *file_name;

	if (if_dir_change_dir()) {
		return 0;
	}
	if (is_app_list_mode()) {
		// do_tail_file -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	file_idx = get_cur_filer_view()->cur_sel_idx;
	file_name = get_cur_filer_view()->file_list[file_idx].file_name;
	if (S_ISREG(get_cur_filer_view()->file_list[file_idx].st.st_mode)) {
		fork_exec_once(SEPARATE1, PAUSE0, BETAIL, file_name, 0);
	}
	return 0;
}
int do_copy_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	file_idx = get_first_file_idx_selected();

	ret = input_string(get_other_filer_view()->cur_dir, file_path,
	 HISTORY_TYPE_IDX_DIR, _("Copy to:"));

	if (ret < 0) {
		// do_copy_file -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_handler_sigint_called())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_repeat(SEPARATE1, "cp", "-afv",
		 get_cur_filer_view()->file_list[file_idx].file_name, file_path, 0);
#else
		fork_exec_repeat(SEPARATE1, "cp", "-a",
		 get_cur_filer_view()->file_list[file_idx].file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_copy_file_update(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	ret = input_string(get_other_filer_view()->cur_dir, file_path,
	 HISTORY_TYPE_IDX_DIR, _("Copy to (Update):"));

	if (ret < 0) {
		// do_copy_file_update -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_handler_sigint_called())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_repeat(SEPARATE1, "cp", "-aufv",
		 get_cur_filer_view()->file_list[file_idx].file_name, file_path, 0);
#else
		fork_exec_repeat(SEPARATE1, "cp", "-a",
		 get_cur_filer_view()->file_list[file_idx].file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_rename_file(void)
{
	char file_name[MAX_PATH_LEN+1];
	int ret;

	strlcpy__(file_name, get_cur_filer_view()
	 ->file_list[get_cur_filer_view()->cur_sel_idx].file_name, MAX_PATH_LEN);

	ret = input_string(file_name, file_name, HISTORY_TYPE_IDX_EXEC, _("Rename to:"));

	if (ret < 0) {
		// do_rename_file -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (fork_exec_once(SEPARATE1, PAUSE1, "mv", "-i",
	 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name,
	 file_name, 0) == 0) {
		strlcpy__(get_cur_filer_view()->next_file, file_name, MAX_PATH_LEN);
		filer_do_next = FILER_DO_REFRESH_FORCE;
	}
	return 0;
}
int do_move_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	ret = input_string(get_other_filer_view()->cur_dir, file_path,
	 HISTORY_TYPE_IDX_DIR, _("Move to:"));

	if (ret < 0) {
		// do_move_file -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	begin_fork_exec_repeat();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_handler_sigint_called())
			break;
#ifndef	USE_BUSYBOX
		fork_exec_repeat(SEPARATE1, "mv", "-ufv",
		 get_cur_filer_view()->file_list[file_idx].file_name, file_path, 0);
#else
		fork_exec_repeat(SEPARATE1, "mv",
		 get_cur_filer_view()->file_list[file_idx].file_name, file_path, 0);
#endif
	}
	end_fork_exec_repeat();
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_trash_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Trash file %s ?"),
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Trash %d files ?"), files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			if (fork_exec_repeat(SEPARATE1, BETRASH,
			 get_cur_filer_view()->file_list[file_idx].file_name, 0)) {
#ifndef	USE_BUSYBOX
				fork_exec_repeat(SEPARATE1, "rm", "-rv",
				 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#else
				fork_exec_repeat(SEPARATE1, "rm", "-r",
				 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#endif
			}
		}
		end_fork_exec_repeat();
	}
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_delete_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Delete file %s ?"),
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Delete %d files ?"), files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
#ifndef	USE_BUSYBOX
			fork_exec_repeat(SEPARATE1, "rm", "-rv",
			 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#else
			fork_exec_repeat(SEPARATE1, "rm", "-r",
			 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#endif
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_DO_REFRESH_FORCE;
	}
	return 0;
}
int do_mark_to_delete_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Mark file %s to be Deleted later ?"),
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Mark %d files to be Deleted later ?"),
		 files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			if (fork_exec_repeat(SEPARATE1, BEMARKDEL,
			 get_cur_filer_view()->file_list[file_idx].file_name, 0)) {
#ifndef	USE_BUSYBOX
				fork_exec_repeat(SEPARATE1, "chmod", "-v", "606",
				 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#else
				fork_exec_repeat(SEPARATE1, "chmod", "606",
				 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#endif
			}
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_DO_REFRESH_FORCE;
	}
	return 0;
}
int do_size_zero_file(void)
{
	int ret;
	int file_idx;
	int files_selected;

	if ((files_selected = get_files_selected_cfv()) == 0)
		ret = ask_yes_no(ASK_YES_NO, _("Make size of file %s 0 ?"),
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
	else
		ret = ask_yes_no(ASK_YES_NO, _("Make size of %d files 0 ?"),
		 files_selected);
	if (ret > 0) {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			if (fork_exec_repeat(SEPARATE1, BESIZE0,
			 get_cur_filer_view()->file_list[file_idx].file_name, 0)) {
#ifndef	USE_BUSYBOX
				fork_exec_repeat(SEPARATE1, "chmod", "-v", "000",
				 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#else
				fork_exec_repeat(SEPARATE1, "chmod", "000",
				 get_cur_filer_view()->file_list[file_idx].file_name, 0);
#endif
			}
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_DO_REFRESH_FORCE;
	}
	return 0;
}
int do_find_file(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_path, HISTORY_TYPE_IDX_DIR, _("Find:"));

	if (ret < 0) {
		// do_find_file -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	strlcpy__(get_cur_filer_view()->next_file, file_path, MAX_PATH_LEN);
	get_cur_filer_view()->top_idx = 0;
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_make_directory(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", file_path, HISTORY_TYPE_IDX_DIR, _("Mkdir:"));

	if (ret < 0) {
		// do_make_directory -> FILER_DO_ENTERED_DIR_PATH
		filer_do_next = FILER_DO_ENTERED_DIR_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	fork_exec_once(SEPARATE1, PAUSE1, "mkdir", "-p", file_path, 0);
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_change_directory(void)
{
	char string[MAX_PATH_LEN+1];
	char file_path[MAX_PATH_LEN+1];
	int ret;

	ret = input_string("", string, HISTORY_TYPE_IDX_DIR, _("Chdir to:"));
	get_file_line_col_from_str_null(string, file_path, NULL, NULL);

	if (ret < 0) {
		// do_change_directory -> FILER_DO_ENTERED_DIR_PATH
		filer_do_next = FILER_DO_ENTERED_DIR_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	return filer_change_dir_parent(file_path);
}

int do_parent_directory(void)
{
	if (filer_change_dir("..") == 0)
		return 0;
	separate_dir_and_file(get_cur_filer_view()->cur_dir, get_cur_filer_view()->next_file);
	return 1;
}
int do_beginning_directory(void)
{
	return filer_change_dir_if_not_yet(cur_filer_panes->org_cur_dir);
}
int do_home_directory(void)
{
	return filer_change_dir_if_not_yet("~");
}
int do_root_directory(void)
{
	return filer_change_dir_if_not_yet("/");
}
int do_prev_directory(void)
{
	return filer_change_prev_dir();
}
int do_real_path(void)
{
	char chg_dir[MAX_PATH_LEN+1];

	return filer_change_dir_if_not_yet(get_cwd(chg_dir));
}
//-----------------------------------------------------------------------------
int do_select_file(void)
{
	int files_selected;

	get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].selected
	 = get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].selected
	  ^ _FILE_SEL_MAN_;
	files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected"),
							_("%d files selected"),
							_("%d filess selected"),
							_("%d filesss selected"),
	 files_selected), files_selected);
	get_cur_filer_view()->cur_sel_idx = MIN_MAX_(0,
	 get_cur_filer_view()->cur_sel_idx + 1, get_cur_filer_view()->file_list_entries-1);
//	filer_do_next = FILER_DO_UPDATE_SCREEN;
	return 0;
}
int do_select_no_file(void)
{
	int file_idx;

	for (file_idx = 0 ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		get_cur_filer_view()->file_list[file_idx].selected = 0;
	}
	disp_status_bar_done(_("File selection cleared"));
//	filer_do_next = FILER_DO_UPDATE_SCREEN;
	return 0;
}
int do_select_all_files(void)
{
	int file_idx;
	int files_selected;

	for (file_idx = 0 ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		if (strcmp(get_cur_filer_view()->file_list[file_idx].file_name, ".") == 0
		 || strcmp(get_cur_filer_view()->file_list[file_idx].file_name, "..") == 0)
			get_cur_filer_view()->file_list[file_idx].selected = 0;
		else
			get_cur_filer_view()->file_list[file_idx].selected
			 = get_cur_filer_view()->file_list[file_idx].selected ^ _FILE_SEL_MAN_;
	}
	files_selected = get_files_selected_cfv();
	disp_status_bar_done(P_(_("%d file selected"),
							_("%d files selected"),
							_("%d filess selected"),
							_("%d filesss selected"),
	 files_selected), files_selected);
//	filer_do_next = FILER_DO_UPDATE_SCREEN;
	return 0;
}
int do_quit_filer(void)
{
	filer_do_next = FILER_DO_QUIT;
	return 0;
}
int do_quit_home_dir(void)
{
	do_home_directory();
	do_quit_filer();
	return 0;
}
int do_filer_tog_show_dot_file(void)
{
	do_tog_show_dot_file();
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_filer_inc_show_file_info(void)
{
	inc_show_file_info();
	SHOW_MODE("File information", get_str_show_file_info());
//	filer_do_next = FILER_DO_UPDATE_SCREEN;
	return 0;
}
int do_filer_inc_sort_by(void)
{
	inc_sort_by();
	SHOW_MODE("File sort", get_str_sort_by());
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_tog_filer_panes(void)
{
	tog_filer_panes();
	SHOW_MODE("Filer panes", get_str_filer_panes());
	filer_do_next = FILER_DO_REFRESH_AUTO;
	return 0;
}
int do_filer_inc_key_list_lines(void)
{
	do_inc_key_list_lines_();
	return 0;
}

int do_filer_display_color_pairs(void)
{
	display_color_pairs(0, 0);
	input_key_loop();
#ifdef ENABLE_DEBUG
	display_item_colors(0, 0);
	display_bracket_hl_colors(0, 40);
	input_key_loop();
#endif // ENABLE_DEBUG
	filer_do_next = FILER_DO_NOTHING;
	return 0;
}

#ifdef ENABLE_HELP
int do_filer_splash(void)
{
	disp_splash(100);
	input_key_loop();
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
#endif // ENABLE_HELP

int do_filer_menu_0(void)
{
	filer_menu_n(-1);
	return 0;
}
int do_filer_menu_1(void)
{
	if (is_app_list_mode()) {
		filer_do_next = FILER_DO_QUIT;
		return 0;
	}
	filer_menu_n(0);
	return 0;
}
int do_filer_menu_2(void)
{
	filer_menu_n(1);
	return 0;
}
int do_filer_menu_3(void)
{
	filer_menu_n(2);
	return 0;
}
int do_filer_menu_4(void)
{
	filer_menu_n(2);
	return 0;
}
int do_filer_menu_5(void)
{
	filer_menu_n(2);
	return 0;
}

//-----------------------------------------------------------------------------

PRIVATE int do_edit_file_(int recursive)
{
	int file_idx;

	if (if_dir_change_dir()) {
		return 0;
	}
	if (is_app_list_mode()) {
		// do_edit_file_ -> FILER_DO_ENTERED_FILE
		filer_do_next = FILER_DO_ENTERED_FILE;
		return 0;
	}
	clear_files_loaded();
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (S_ISREG(get_cur_filer_view()->file_list[file_idx].st.st_mode)) {
			if (load_file_name_upp_low(get_cur_filer_view()->file_list[file_idx].file_name,
			 TUL0, OOE0, MOE1, recursive) <= 0) {
				tio_beep();
			}
		}
	}
	if (get_files_loaded() == 0) {
		filer_do_next = FILER_DO_REFRESH_FORCE;
	} else {
		disp_files_loaded();
		filer_do_next = FILER_DO_LOADED_FILE;
	}
	unselect_all_files_auto(_FILE_SEL_MAN_ | _FILE_SEL_AUTO_);
////_D_(bufs_dump_all_bufs(&bufs_top_anchor));
	return 0;
}

PRIVATE int if_dir_change_dir(void)
{
	if (S_ISDIR(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].st.st_mode)) {
		filer_change_dir(get_cur_filer_view()
		 ->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		return 1;
	}
	return 0;
}
PRIVATE int filer_change_dir_if_not_yet(const char *dir)
{
	char buf[MAX_PATH_LEN+1];

	if (strcmp(get_cur_filer_view()->cur_dir, get_abs_path(dir, buf)) == 0) {
		return filer_change_prev_dir();
	} else {
		return filer_change_dir(dir);
	}
}
PRIVATE int filer_change_prev_dir(void)
{
	if (strlen(get_cur_filer_view()->prev_dir)) {
		return filer_change_dir(get_cur_filer_view()->prev_dir);
	}
	return 0;
}
int filer_change_dir_parent(const char *dir)
{
	char chg_dir[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];

	strlcpy__(chg_dir, dir, MAX_PATH_LEN);
	for ( ; ; ) {
flf_d_printf("try to chdir[%s]\n", chg_dir);
		if (strcmp(chg_dir, "/") == 0) {
			return -1;	// error
		}
		if (filer_change_dir(chg_dir) == 0) {
			break;
		}
		// If can not change to dir, try parent dir
		// /try/to/change/dir/file ==> /try/to/change/dir
		separate_dir_and_file(chg_dir, file);
	}
	return 0;	// changed
}
int filer_change_dir(const char *dir)
{
	char buf[MAX_PATH_LEN+1];
	char chg_dir[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];

	if (strcmp(dir, ".") == 0) {
		return 0;
	} else if (strcmp(dir, "..") == 0) {
		strlcpy__(chg_dir, get_cur_filer_view()->cur_dir, MAX_PATH_LEN);
		strlcpy__(get_cur_filer_view()->next_file, separate_dir_and_file(chg_dir, file), MAX_PATH_LEN);
	} else if (strcmp(dir, "~") == 0) {
		strcpy__(get_cur_filer_view()->next_file, "..");
		get_abs_path("~", chg_dir);
	} else if (dir[0] == '/') {
		// absolute path
		strcpy__(get_cur_filer_view()->next_file, "..");
		strlcpy__(chg_dir, dir, MAX_PATH_LEN);
	} else {
		// relative path
		strcpy__(get_cur_filer_view()->next_file, "..");
		cat_dir_and_file(chg_dir, MAX_PATH_LEN, get_cur_filer_view()->cur_dir, dir);	// /dir1 ==> /dir1/dir2
	}
	normalize_full_path(chg_dir);
	if (is_dir_readable(chg_dir) == 0) {
		// We can't open this dir for some reason. Complain.
		disp_status_bar_err(_("Can not change current directory to [%s]: %s"),
		 shrink_str_to_scr_static(chg_dir), strerror(errno));
		filer_do_next = FILER_DO_NOTHING;
		return 1;
	}
	strlcpy__(get_cur_filer_view()->prev_dir, get_cur_filer_view()->cur_dir, MAX_PATH_LEN);
	change_cur_dir(chg_dir);
	strlcpy__(get_cur_filer_view()->cur_dir, chg_dir, MAX_PATH_LEN);
	get_cur_filer_view()->top_idx = 0;
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_DIR, chg_dir);
#endif // ENABLE_HISTORY
	filer_do_next = FILER_DO_REFRESH_FORCE;
	disp_status_bar_done("Changed current directory to [%s]", chg_dir);
	return 0;
}

#endif // ENABLE_FILER

// End of filercmd.c
/**************************************************************************
 *   filerkeys.c                                                          *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
 *   Copyright (C) 2010-2018 PCR                                          *
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

#include "headers.h"

#ifdef ENABLE_FILER

func_key_table_t filer_func_key_table[] = {
//  1234567890123456   123456789012345678901234567890   123456789012345678901234567890
//       12345678901234567890
 { "",                "Filer Menu-1",                   K_ESC,        NKA, NKA,
   1,F_I(do_filer_menu_1), NULL },
 { "Prev.Menu",       "Previous Menu",                  K_M_ESC, K_M_BACKQUOTE, NKA,
   0,F_I(do_filer_menu_0), NULL },
 { "Quit",            "Quit filer",                     'q', K_C_Q, K_M_q,
   1,F_I(do_quit_filer), NULL },
 { "Quit to Home dir","Quit to Home directory",         'Q', K_M_Q,        NKA,
   1,F_I(do_quit_home_dir), NULL },
 { "Edit (project)",  "Edit file (project file)",       'e', K_F01,        NKA,
   0,F_I(do_edit_file), NULL },
 { "Edit file",       "Edit file",                      K_M_e,        NKA, NKA,
   1,F_I(do_edit_file_non_recursive), NULL },
 { "Edit new file",   "Edit new file",                  'E', K_M_E,        NKA,
   1,F_I(do_edit_new_file), NULL },
 { "Enter file/dir",  "Enter file/directory",           K_ENTER, K_C_M,    NKA,
   1,F_I(do_enter_file), NULL },
 { "Parent directory","Parent directory",               K_BS, K_C_H,       NKA,
   1,F_I(do_parent_directory), NULL },

 { "",                "Filer Menu-2",                   NKA,          NKA, NKA,
   1,F_I(do_filer_menu_2), NULL },
 { "Copy file",       "Copy file",                      'c', K_F02,        NKA,
   1,F_I(do_copy_file), NULL },
 { "Copy update",     "Copy file update",               'C', K_F03,        NKA,
   1,F_I(do_copy_file_update), NULL },
 { "Rename",          "Rename file",                    'n', K_F04,        NKA,
   1,F_I(do_rename_file), NULL },
 { "Move",            "Move file",                      'm', K_F05,        NKA,
   1,F_I(do_move_file), NULL },
 { "Trash file",      "Trash file",                     'd', K_F06,        NKA,
   0,F_I(do_trash_file), NULL },
 { "Delete file",     "Delete file",                    'D', K_DEL, K_DC,
   0,F_I(do_delete_file), NULL },
 { "View",            "View file",                      'p',          NKA, NKA,
   0,F_I(do_view_file), NULL },
 { "Tail",            "View tail of file",              'l',          NKA, NKA,
   0,F_I(do_tail_file), NULL },
 { "Mark to delete",  "Mark to be Deleted",             'f', K_F07,        NKA,
   0,F_I(do_mark_to_delete_file), NULL },
 { "Size zero",       "File size zero",                 'F', K_F07,        NKA,
   0,F_I(do_size_zero_file), NULL },
 { "Find file",       "Find file",                      'j',          NKA, NKA,
   0,F_I(do_find_file), NULL },

 { "",                "Filer Menu-3",                   NKA,          NKA, NKA,
   0,F_I(do_filer_menu_3), NULL },
 { "Top",             "Top of list",                    K_C_T,        K_M_t, NKA,
   1,F_I(do_top_of_list), NULL },
 { "Bottom",          "Bottom of list",                 K_C_B,        K_M_b, NKA,
   1,F_I(do_bottom_of_list), NULL },
 { "Select",          "Select file",                    ' ',          NKA, NKA,
   1,F_I(do_select_file), NULL },
 { "Select all",      "Select all files",               'a',          NKA, NKA,
   1,F_I(do_select_all_files), NULL },
 { "Select none",     "Select no files",                'z',          NKA, NKA,
   1,F_I(do_select_no_file), NULL },
 { "Make directory",  "Make directory",                 'k', K_F08,        NKA,
   1,F_I(do_make_directory), NULL },
 { "Change directory","Change directory",               'h', K_F09,        NKA,
   1,F_I(do_change_directory), NULL },
 { "Beginning dir.",  "Beginning directory",            '~',          NKA, NKA,
   1,F_I(do_beginning_directory), NULL },
 { "Home directory",  "Home directory",                 '`', K_F12,        NKA,
   1,F_I(do_home_directory), NULL },
 { "Root directory",  "Root directory",                 '/',          NKA, NKA,
   1,F_I(do_root_directory), NULL },
 { "Previous dir.",   "Previous directory",             'b',          NKA, NKA,
   1,F_I(do_prev_directory), NULL },

 { "",                "Filer Menu-4",                   NKA,          NKA, NKA,
   0,F_I(do_filer_menu_4), NULL },
 { "Execute command", "Execute command",                'x', K_F10,        NKA,
   1,F_I(do_exec_command_with_file), NULL },
 { "Execute command", "Execute command with files",     'X',          NKA, NKA,
   1,F_I(do_exec_command_with_files), NULL },
 { "Run command rel.","Run command (relative path)",    'r', K_F11,        NKA,
   1,F_I(do_run_command_rel), NULL },
 { "Run command abs.","Run command (absolute path)",    'R',          NKA, NKA,
   1,F_I(do_run_command_abs), NULL },
 { "Run command (src/dest)", "Run command (src/dest)",  't',          NKA, NKA,
   1,F_I(do_run_command_src_dst), NULL },
 { "Run command (sh)","Run command (sh path)",          'T',          NKA, NKA,
   1,F_I(do_run_command_sh), NULL },
 { "Real path",       "Convert Symlink to Real path",   K_M_l,        NKA, NKA,
   0,F_I(do_real_path), NULL },
#ifdef ENABLE_HELP
 { "Splash",          "Splash screen",                  K_M_v,        NKA, NKA,
   0,F_I(do_filer_splash), NULL },
#endif // ENABLE_HELP
 { "Color pairs",     "Display color pairs",            K_M_c,        NKA, NKA,
   0,F_I(do_filer_display_color_pairs), NULL },

 { "",                "Filer Menu-5",                   NKA,          NKA, NKA,
   0,F_I(do_filer_menu_5), NULL },
 { "Split pane",      "Split screen into panes",        '\\', K_MC_BACKSLASH, NKA,
   0,F_I(do_tog_filer_panes), get_str_filer_panes },
 { "Switch pane",     "Switch filer pane",              K_TAB, K_M_TAB, NKA,
   1,F_I(do_switch_filer_pane), NULL },
 { "Refresh",         "Refresh filer screen",           ',',          NKA, NKA,
   0,F_I(do_refresh_filer), NULL },
 { "Show dot file",   "Show dot file",                  '.',          NKA, NKA,
   0,F_I(do_filer_tog_show_dot_file), get_str_show_dot_file },
 { "Show file info.", "Show file information",          '-',          NKA, NKA,
   0,F_I(do_filer_inc_show_file_info), get_str_show_file_info },
 { "Sort by ext",     "Sort by extension",              's',          NKA, NKA,
   0,F_I(do_filer_inc_sort_by), get_str_sort_by },
 { "Display key list","Toggle Displaying key list",     K_M_K,        NKA, NKA,
   0,F_I(do_filer_inc_key_list_lines), get_str_key_list_lines },
 { "Code-7F Key-BS",  "Toggle Code-7F Key-BS",          K_M_D,        NKA, NKA,
   0,F_I(do_tog_map_key_7f_bs), get_str_map_key_7f_bs },
 { "Exam key code",   "Examine key code",               K_M_k,        NKA, NKA,
   0,F_I(do_examine_key_code), NULL },

 { "user command-1",  "user defined command-1",         'y',          NKA, NKA,
   0,F_I(do_filer_cmd_1), NULL },
 { "user command-2",  "user defined command-2",         'u',          NKA, NKA,
   0,F_I(do_filer_cmd_2), NULL },
 { "user command-3",  "user defined command-3",         'i',          NKA, NKA,
   0,F_I(do_filer_cmd_3), NULL },
 { "user command-4",  "user defined command-4",         'o',          NKA, NKA,
   0,F_I(do_filer_cmd_4), NULL },

 { "",                "",                               NKA,          NKA, NKA,
   0,F_I(NULL), NULL },
};

#endif // ENABLE_FILER

// End of filerkeys.c
/**************************************************************************
 *   filerrun.c                                                           *
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

#include "headers.h"

#ifdef ENABLE_FILER

PRIVATE int do_run_command_(int mode);
PRIVATE int do_filer_cmd_(char *cmd_file);

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"

#define BECMD		"becmd"		// becmd?

//-----------------------------------------------------------------------------

int do_run_line(void)
{
	fork_exec_once_sh_c(SEPARATE1, PAUSE1, CEPBV_CL->data);
	do_refresh_editor();
	return 0;
}

#define STR_TO_BE_REPLACED_WITH_FILE_NAME		"{}"
#define STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN	strlen(STR_TO_BE_REPLACED_WITH_FILE_NAME)
int do_exec_command_with_file(void)
{
	char command_str[MAX_PATH_LEN+1];
	int ret;
	int file_idx;
	char *ptr_replace;
	char buffer[MAX_PATH_LEN+1];
#define MAX_REPLACEMENTS	10
	int cnt;

	ret = input_string("", command_str,
	 HISTORY_TYPE_IDX_EXEC, _("Execute({} will be replaced with file-name):"));

	if (ret < 0) {
		// do_exec_command_with_file -> FILER_DO_ENTERED_FILE_PATH
		filer_do_next = FILER_DO_ENTERED_FILE_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
	if (get_files_selected_cfv() == 0) {
		fork_exec_once_sh_c(SEPARATE1, PAUSE1, command_str);
		filer_do_next = FILER_DO_REFRESH_FORCE;
	} else {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			// replace "{}" with filename
			// Ex. "cp -auv {} dir/{}" ==> "cp -auv filename dir/filename"
			strlcpy__(buffer, command_str, MAX_PATH_LEN);
			for (cnt = 0; cnt < MAX_REPLACEMENTS; cnt++) {
				if ((ptr_replace = strstr(buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME))
				 == NULL)
					break;
				replace_str(buffer, MAX_PATH_LEN,
				 ptr_replace - buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN,
				 quote_file_name(get_cur_filer_view()->file_list[file_idx].file_name), -1);
			}
			fork_exec_repeat_sh_c(SEPARATE1, buffer);
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_DO_REFRESH_FORCE;
	}
	return 0;
}
int do_exec_command_with_files(void)
{
	char command_str[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	command_str[0] = '\0';
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_name_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_filer_view()->file_list[file_idx].file_name);
	}

	ret = input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC, _("Execute with files:"));

	if (ret < 0) {
		// do_exec_command_with_files -> FILER_DO_ENTERED_FILE_PATH
		filer_do_next = FILER_DO_ENTERED_FILE_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	fork_exec_once_sh_c(SEPARATE1, PAUSE1, command_str);
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}
int do_run_command_rel(void)
{
	struct stat *st_ptr;

	st_ptr = &get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].st;
	if ((st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) == 0)
		do_run_command_(0);
	else
		do_run_command_(1);
	return 0;
}
int do_run_command_abs(void)
{
	do_run_command_(2);
	return 0;
}
int do_run_command_src_dst(void)
{
	do_run_command_(5);
	return 0;
}
int do_run_command_sh(void)
{
	do_run_command_(4);
	return 0;
}
PRIVATE int do_run_command_(int mode)
{
	const char *explanation;
	char buf_s[MAX_PATH_LEN+1];
	char buf_d[MAX_PATH_LEN+1];
	char buf1[MAX_PATH_LEN+1];
	char buf2[MAX_PATH_LEN+1];
	char command_str[MAX_PATH_LEN+1];
	struct stat *st_ptr;
	int src_fv_idx = 0;
	int dst_fv_idx = 1;
	int ret;

	st_ptr = &get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].st;
	switch(mode) {
	default:
	case 0:
		explanation = _("Run (with file):");
		snprintf_(command_str, MAX_PATH_LEN+1, " %s",
		 quote_file_name(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name));
		break;
	case 1:
		explanation = _("Run (current-directory-file):");
		snprintf_(command_str, MAX_PATH_LEN+1, "./%s ",
		 quote_file_name(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name));
		break;
	case 2:
		explanation = _("Run (with real-path):");
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_view()->cur_dir,
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		quote_file_name_buf(command_str, buf_s);
		break;
	case 3:
		explanation = _("Run (script):");
		snprintf_(command_str, MAX_PATH_LEN+1, ". %s",
		 quote_file_name(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name));
		break;
	case 4:
		explanation = _("Run (script by shell):");
		snprintf_(command_str, MAX_PATH_LEN+1, "sh %s",
		 quote_file_name(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name));
		break;
	case 5:
		explanation = _("Run (with SRC-path and DEST-path):");
		if (get_filer_cur_pane_idx() == 0) {
			src_fv_idx = 0;
			dst_fv_idx = 1;
		} else {
			src_fv_idx = 1;
			dst_fv_idx = 0;
		}
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_panes->filer_views[src_fv_idx].cur_dir,
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		snprintf_(buf_d, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_panes->filer_views[dst_fv_idx].cur_dir,
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		snprintf_(command_str, MAX_PATH_LEN+1, " %s %s",
		 quote_file_name_buf(buf1, buf_s),
		 quote_file_name_buf(buf2, buf_d));
		break;
	}

	ret = input_string_pos(command_str, command_str,
	 (S_ISREG(st_ptr->st_mode) && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
	  ? MAX_PATH_LEN : 0,
	 HISTORY_TYPE_IDX_EXEC, explanation);

	if (ret < 0) {
		// do_run_command_ -> FILER_DO_ENTERED_FILE_PATH
		filer_do_next = FILER_DO_ENTERED_FILE_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
	fork_exec_once_sh_c(SEPARATE1, PAUSE1, command_str);
	filer_do_next = FILER_DO_REFRESH_FORCE;
	return 0;
}

int do_filer_cmd_1(void)
{
	return do_filer_cmd_(BECMD "1");
}
int do_filer_cmd_2(void)
{
	return do_filer_cmd_(BECMD "2");
}
int do_filer_cmd_3(void)
{
	return do_filer_cmd_(BECMD "3");
}
int do_filer_cmd_4(void)
{
	return do_filer_cmd_(BECMD "4");
}

PRIVATE int do_filer_cmd_(char *cmd_file)
{
	int file_idx;
	char *file_name;

	file_idx = get_cur_filer_view()->cur_sel_idx;
	file_name = get_cur_filer_view()->file_list[file_idx].file_name;
	if (is_app_list_mode()) {
		// do_filer_cmd -> FILER_DO_ENTERED_FILE_PATH
		filer_do_next = FILER_DO_ENTERED_FILE_PATH;
		return 0;
	}
	if (S_ISREG(get_cur_filer_view()->file_list[file_idx].st.st_mode)) {
		fork_exec_once(SEPARATE1, PAUSE0, cmd_file, file_name, 0);
		return 0;
	}
	return 0;
}

#endif // ENABLE_FILER

// End of filerrun.c
/**************************************************************************
 *   history.c                                                            *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#include "headers.h"

#ifdef ENABLE_HISTORY

PRIVATE int save_history_if_modified(int hist_type_idx);
PRIVATE be_buf_t *get_history_buf(int hist_type_idx);

PRIVATE int save_history_idx(int hist_type_idx);
PRIVATE int load_history_idx(int hist_type_idx);
PRIVATE char *get_history_file_path(int hist_type_idx);

PRIVATE char *get_app_dir(void);

PRIVATE void init_history(int hist_type_idx);
PRIVATE void append_history(int hist_type_idx, const char *str);
PRIVATE void clear_history(int hist_type_idx);

PRIVATE void clear_history_modified(int hist_type_idx);
PRIVATE void set_history_modified(int hist_type_idx);
PRIVATE int is_history_modified(int hist_type_idx);

PRIVATE void set_history_oldest(int hist_type_idx);
PRIVATE void set_history_newest(int hist_type_idx);
PRIVATE const char *get_history_older(int hist_type_idx);
PRIVATE const char *get_history_newer(int hist_type_idx);

PRIVATE const char *is_the_last_line(int hist_type_idx, const char *str);
PRIVATE be_line_t *search_history_str_complete(int hist_type_idx, const char *str);
PRIVATE be_line_t *search_history_str_partial(int hist_type_idx, const char *str);

// search/replace(directory and execution) history support functions

void init_hist_bufs(void)
{
	// history_buffers
	init_bufs_top_bot_anchor(
	 HIST_BUFS_TOP_ANCH, "#History-bufs-top-anchor",
	 HIST_BUFS_BOT_ANCH, "#History-bufs-bot-anchor");
}

void init_histories(void)
{
	int hist_type_idx;
	char buf_name[MAX_PATH_LEN+1];

///_FLF_
	// prepare ~/.be directory
	if (is_path_exist(get_app_dir()) == 0) {
		if (mkdir(get_app_dir(), S_IRUSR | S_IWUSR | S_IXUSR) < 0) {
			e_printf(_("Unable to create directory %s, %s"),
			 get_app_dir(), strerror(errno));
		}
	} else {
		if (is_path_dir(get_app_dir()) == 0) {
			e_printf(_("No application directory %s"),
			 get_app_dir());
		}
	}
///_FLF_
	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		snprintf_(buf_name, MAX_PATH_LEN, "#%d:%s",
		 hist_type_idx, get_history_file_path(hist_type_idx));
		buf_insert_before(HIST_BUFS_BOT_ANCH, buf_create(buf_name));
		init_history(hist_type_idx);
	}
}

//-----------------------------------------------------------------------------

// load history files into buffers
void load_histories(void)
{
	int hist_type_idx;

////_FLF_
	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		load_history_idx(hist_type_idx);
	}
////_D_(set_history_modified(hist_type_idx));
////_D_(dump_hist_bufs_lines());
}

// save histories to file
void save_histories(void)
{
	int hist_type_idx;

	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP; hist_type_idx++) {
		save_history_if_modified(hist_type_idx);
	}
}

void load_last_searched_needle(void)
{
	// load last_searched_needle
	strlcpy__(last_searched_needle, get_history_newest(HISTORY_TYPE_IDX_SEARCH, 1), MAX_PATH_LEN);
///flf_d_printf("last_searched_needle[%s]\n", last_searched_needle);
}

//-----------------------------------------------------------------------------

PRIVATE be_buf_t *get_history_buf(int hist_type_idx)
{
	be_buf_t *buf;
	int idx;

	if ((0 <= hist_type_idx && hist_type_idx < HISTORY_TYPES_APP_AND_SHELL) == 0) {
		e_printf(_("hist_type_idx out of range: %d"), hist_type_idx);
		hist_type_idx = 0;
	}
	return get_buf_from_bufs_by_idx(HIST_BUFS_TOP_NODE, hist_type_idx);
}

// update history list (load, modify, save)
void update_history(int hist_type_idx, const char *str)
{
	be_line_t *line;

////
flf_d_printf("hist_type_idx:%d:[%s]\n", hist_type_idx, str);
	if (is_the_last_line(hist_type_idx, str) != NULL) {
		// str is registered in the last line, no need update
		return;
	}
	load_history_idx(hist_type_idx);
	if ((line = search_history_str_complete(hist_type_idx, str)) != NULL) {
		line_unlink_free(line);	// delete older line
	}
	append_history(hist_type_idx, str);
	set_history_modified(hist_type_idx);
	save_history_if_modified(hist_type_idx);
	////save_histories();
}

const char *get_history_newest(int hist_type_idx, int last_n)
{
	int cnt;
	const char *history = "";

	set_history_newest(hist_type_idx);
	for (cnt = 0; cnt < last_n; cnt++) {
		history = get_history_older(hist_type_idx);
	}
	return history;
}

// get a completion
const char *get_history_completion(int hist_type_idx, const char *str)
{
	be_line_t *line;

	if ((line = search_history_str_partial(hist_type_idx, str)) != NULL) {
		return line->data;
	}
	return str;		// return original string
}

//------------------------------------------------------------------------------------
PRIVATE char *get_app_dir(void)
{
	static char dir[MAX_PATH_LEN+1];

#if defined(APP_DIR)
	snprintf_(dir, MAX_PATH_LEN+1, "%s/%s", get_home_dir(), APP_DIR);
#else
	snprintf_(dir, MAX_PATH_LEN+1, "%s", get_home_dir());
#endif
	return dir;
}

//-----------------------------------------------------------------------------

PRIVATE int save_history_if_modified(int hist_type_idx)
{
	if (is_history_modified(hist_type_idx) == 0)
		return -1;
	return save_history_idx(hist_type_idx);
}

PRIVATE int save_history_idx(int hist_type_idx)
{
	const char *file_path;
	FILE *fp;
	int lines;
	const char *str;
	char buf[MAX_EDIT_LINE_LEN+1];
	int error = 0;

	if (GET_APPMD(app_HISTORY) == 0)
		return 0;
	file_path = get_history_file_path(hist_type_idx);
flf_d_printf("hist_type_idx:%d[%s]\n", hist_type_idx, file_path);
	if ((fp = fopen(file_path, "w")) == NULL) {
		e_printf(_("Unable to create history file: %s, %s"),
		 file_path, strerror(errno));
		error = 1;
		goto save_history_2;
	}
	lines = 0;
	for (set_history_oldest(hist_type_idx); *(str = get_history_newer(hist_type_idx)); ) {
		// count lines
		lines++;
	}
	lines = LIM_MIN(0, lines - MAX_HISTORY_LINES); // lines to skip
	// write oldest first
	for (set_history_oldest(hist_type_idx); *(str = get_history_newer(hist_type_idx)); ) {
		if (lines-- > 0)
			continue;
		snprintf_(buf, MAX_EDIT_LINE_LEN+1, "%s\n", str);
		if (fputs(buf, fp) == EOF) {
			e_printf(_("Unable to write history file: %s, %s"),
			 file_path, strerror(errno));
			error = 1;
			goto save_history_1;
		}
	}
save_history_1:;
	if (fclose(fp) != 0) {
		error = 1;
	}
	// set rw only by owner for security
	chmod(file_path, S_IRUSR | S_IWUSR);
save_history_2:;
	clear_history_modified(hist_type_idx);
	return error;
}

PRIVATE int load_history_idx(int hist_type_idx)
{
	const char *file_path;
	FILE *fp;
	int lines;
	char str[MAX_EDIT_LINE_LEN+1];
	int error = 0;

	if (GET_APPMD(app_HISTORY) == 0)
		return 0;
	file_path = get_history_file_path(hist_type_idx);
flf_d_printf("hist_type_idx:%d[%s]\n", hist_type_idx, file_path);
	clear_history(hist_type_idx);

	if ((fp = fopen(file_path, "r")) == NULL) {
		if (errno != ENOENT) {
			e_printf(_("Unable to open history file: %s, %s"),
			 file_path, strerror(errno));
		}
	} else {
		// count total lines
		for (lines = 0; fgets(str, MAX_EDIT_LINE_LEN, fp) != NULL; ) {
			// count lines
			lines++;
		}
		lines = LIM_MIN(0, lines - MAX_HISTORY_LINES);
		fseek(fp, 0, SEEK_SET);
		// skip lines
		for ( ; fgets(str, MAX_EDIT_LINE_LEN, fp) != NULL; ) {
			if (lines-- > 0)
				continue;
			// load the last MAX_HISTORY_LINES lines
			str[MAX_EDIT_LINE_LEN] = '\0';
			remove_line_tail_lf(str);
			if (strlen(str) > 0) {
				append_history(hist_type_idx, str);
			}
		}
		if (fclose(fp) != 0) {
			error = 1;
		}
	}
	clear_history_modified(hist_type_idx);
///_D_(dump_history_ix(hist_type_idx));
	return error;
}
PRIVATE char *get_history_file_path(int hist_type_idx)
{
	const char *dir;
	const char *file;
	static char file_path[MAX_PATH_LEN+1];

	switch(hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_SEARCH:
		dir = get_app_dir();
		file = SEARCH_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_EXEC:
		dir = get_app_dir();
		file = EXEC_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_DIR:
		dir = get_app_dir();
		file = DIR_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_CURSPOS:
		dir = get_app_dir();
		file = OPENFILE_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_KEYMACRO:
		dir = get_app_dir();
		file = KEYMACRO_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_SHELL:
		dir = get_home_dir();
		file = BASH_HISTORY_FILE_NAME;
		break;
	}
	snprintf_(file_path, MAX_PATH_LEN+1, "%s/%s", dir, file);
	return file_path;
}

//-----------------------------------------------------------------------------

// initialize search and replace history lists
PRIVATE void init_history(int hist_type_idx)
{
	set_history_newest(hist_type_idx);
	clear_history_modified(hist_type_idx);
}
// add string to the bottom of history list
PRIVATE void append_history(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
///flf_d_printf("[%s]\n", str);
	buf_set_cur_line(buf, line_insert_with_string(BUF_BOT_ANCH(buf), INSERT_BEFORE, str));
}
PRIVATE void clear_history(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_free_lines(buf);
}
PRIVATE void clear_history_modified(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	BUF_STATE(buf, buf_MODIFIED) = 0;
}
PRIVATE void set_history_modified(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	BUF_STATE(buf, buf_MODIFIED) = 1;
}
PRIVATE int is_history_modified(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	return BUF_STATE(buf, buf_MODIFIED);
}

PRIVATE void set_history_oldest(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, BUF_TOP_NODE(buf));
///_D_(buf_dump_lines(buf, INT_MAX));
}
PRIVATE void set_history_newest(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, BUF_BOT_NODE(buf));
///_D_(buf_dump_lines(buf, INT_MAX));
}
PRIVATE const char *get_history_older(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	if ((line = buf_move_cur_line_to_prev(buf)) != NULL) {
		if (line->data) {
			return line->data;
		}
	}
	return "";			// end of list
}
PRIVATE const char *get_history_newer(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	if ((line = buf_move_cur_line_to_next(buf)) != NULL) {
		if (line->data) {
			return line->data;
		}
	}
	return "";			// end of list
}

PRIVATE const char *is_the_last_line(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	line = BUF_BOT_NODE(buf);
	if (IS_NODE_TOP_ANCH(line))
		return NULL;
	if (strcmp(line->data, str) == 0)	// exact match
		return line->data;
	return NULL;
}
// find first line containing string str in history list
PRIVATE be_line_t *search_history_str_complete(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	for (line = BUF_BOT_NODE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		if (strcmp(str, line->data) == 0)	// exact match
			return line;
	}
	return NULL;
}
PRIVATE be_line_t *search_history_str_partial(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	for (line = BUF_BOT_NODE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		if (strlcmp__(line->data, str) == 0)	// partial match
			return line;
	}
	return NULL;
}
const char *search_history_file_path(int hist_type_idx, const char *path)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;
	const char *ptr;
	size_t len;

	path = quote_file_name(path);
	// search from the newest to the oldest
	for (line = BUF_BOT_NODE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		// /home/user/filename.exp|1234
		// '/home/user/ filename.exp '|1234
		if ((ptr = strstr(line->data, FILE_PATH_SEPARATOR)) != NULL) {
			len = ptr - line->data;
		} else {
			len = line_data_len(line);
		}
		if (strncmp(line->data, path, len) == 0) {
			return line->data;
		}
	}
	return path;		// return original string
}

//-----------------------------------------------------------------------------

int select_from_history_list(int hist_type_idx, char *buffer)
{
	be_buf_t *edit_buf_save;
	int ret;

	edit_buf_save = get_cep_buf();
	load_histories();
	renumber_all_bufs_from_top(&history_buffers);
	set_cep_buf(get_history_buf(hist_type_idx));
	CEPBV_CL = CUR_EDIT_BUF_BOT_NODE;
	post_cmd_processing(CUR_EDIT_BUF_TOP_NODE, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);

	ret = call_editor(1, 1);

	if (ret == 0)
		strcpy__(buffer, "");
	else
		strlcpy__(buffer, CEPBV_CL->data, MAX_EDIT_LINE_LEN);
///_FLF_
	set_cep_buf(edit_buf_save);

	return ret;
}

//------------------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_history_ix(int hist_type_idx)
{
	buf_dump_lines(get_history_buf(hist_type_idx), INT_MAX);
}
void dump_hist_bufs(void)
{
	buf_dump_bufs(HIST_BUFS_TOP_ANCH);
}
void dump_hist_bufs_lines(void)
{
	buf_dump_bufs_lines(HIST_BUFS_TOP_ANCH, "hist-bufs");
}
#endif // ENABLE_DEBUG

#endif // ENABLE_HISTORY

// End of history.c
/**************************************************************************
 *   keymacro.c                                                           *
 *                                                                        *
 *   Copyright (C) 2005-2006 PCR                                          *
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

#include "headers.h"

#ifdef ENABLE_HISTORY
void load_key_macro(int last_n)
{
	get_key_macro_from_string(get_history_newest(HISTORY_TYPE_IDX_KEYMACRO, last_n));
}
void save_key_macro(void)
{
	update_history(HISTORY_TYPE_IDX_KEYMACRO, get_string_from_key_macro());
}
#endif // ENABLE_HISTORY

//-----------------------------------------------------------------------------

// start/abort recording
int do_start_rec__cancel_rec(void)
{
	if (key_macro_is_recording() == 0)
		do_start_recording();
	else
		do_cancel_recording();
	return 0;
}
// end recording/start playback
int do_end_rec__playback(void)
{
	if (key_macro_is_recording()) {
		do_end_recording();
	} else {
		do_playback();
	}
	return 0;
}
//-----------------------------------------------------------------------------
PRIVATE int start_playback_last_n(int last_n);
int do_start_recording(void)
{
	disp_status_bar_done(_("Start key macro recording"));
	key_macro_start_recording();

	disp_editor_title_bar();
	tio_refresh();
	return 0;
}
int do_cancel_recording(void)
{
	disp_status_bar_done(_("Cancel key macro recording"));
	key_macro_cancel_recording();

	disp_editor_title_bar();
	tio_refresh();
	return 0;
}
int do_end_recording(void)
{
	disp_status_bar_done(_("End key macro recording"));
	key_macro_end_recording();

#ifdef ENABLE_HISTORY
	save_key_macro();
///_D_(dump_history_ix(HISTORY_TYPE_IDX_KEYMACRO));
#endif // ENABLE_HISTORY

	disp_editor_title_bar();
	tio_refresh();
	return 0;
}
int do_playback(void)
{
	if (key_macro_start_playback() == 0) {
		disp_status_bar_done(_("No key macro recorded"));
	} else {
		disp_status_bar_done(_("Start key macro playback"));
	}
	return 0;
}
int do_playback_last_1(void)
{
	start_playback_last_n(1);
	return 0;
}
int do_playback_last_2(void)
{
	start_playback_last_n(2);
	return 0;
}
PRIVATE int start_playback_last_n(int last_n)
{
	disp_status_bar_done(_("Start key macro playback"));
#ifdef ENABLE_HISTORY
	load_key_macro(last_n);
#endif // ENABLE_HISTORY
	key_macro_start_playback();
	return 0;
}
//-----------------------------------------------------------------------------
// Keyboard macro

#define KEY_CODE_STR_LEN	(2+MAX_KEY_NAME_LEN+1)	// "\(RIGHT)"

#define MAX_KEY_STROKES			MAX_EDIT_LINE_LEN / KEY_CODE_STR_LEN
#define MAX_KEY_MACRO_STR_LEN	(MAX_KEY_STROKES * KEY_CODE_STR_LEN)
// key macro recording --------------------------------------------------------
PRIVATE int key_macro_recording = -1;		// >=0:recording, -1: not recording
PRIVATE key_code_t key_codes_recording[MAX_KEY_STROKES+1];
// key macro recorded and for play back ---------------------------------------
PRIVATE int key_macro_recorded = 0;			// recorded key strokes
PRIVATE int key_macro_playing_back = -1;	// >=0:playing back
PRIVATE key_code_t key_codes_recorded[MAX_KEY_STROKES+1];
void key_macro_start_recording(void)
{
	key_macro_recording = 0;		// start recording
}
void key_macro_put_key(key_code_t key)
{
	if (key >= 0 && key_macro_is_recording()) {
		// recording
		if (key_macro_recording < MAX_KEY_STROKES)
			// record one key stroke
			key_codes_recording[key_macro_recording++] = key;
	}
}
void key_macro_delete_last_key(void)
{
	if (key_macro_recording > 0) {
		key_macro_recording--;	// cancel the last key (End-rec Key)
	}
}
int key_macro_is_recording(void)
{
	return key_macro_recording >= 0;
}
void key_macro_cancel_recording(void)
{
	key_macro_recording = -1;		// abort recording
}
void key_macro_end_recording(void)
{
	if (key_macro_is_recording()) {
		key_macro_delete_last_key();	// cancel the last key (End-rec Key)
	}
flf_d_printf("key_macro_recording: %d\n", key_macro_recording);
	key_macro_recorded = key_macro_recording;
	// copy "recording" to "recorded"
	memcpy__(key_codes_recorded, key_codes_recording,
	 key_macro_recorded * sizeof(key_codes_recording[0]));
	key_macro_cancel_recording();
}

//-----------------------------------------------------------------------------
////PRIVATE int curs_positioning_mode_save = 0;
int key_macro_start_playback(void)
{
//#ifdef ENABLE_HISTORY
//	load_key_macro(1);
//#endif // ENABLE_HISTORY
	if (key_macro_is_recording()) {
		key_macro_delete_last_key();	// cancel the last key (Start-playback Key)
	}
	if (key_macro_recorded > 0) {	// recorded ?
		key_macro_playing_back = 0;	// playback key strokes
	}
	return key_macro_recorded;
////	curs_positioning_mode_save = GET_APPMD(ed_CURS_POSITIONING);	// save CURS_POSITIONING
////	SET_APPMD_VAL(ed_CURS_POSITIONING, CURS_POSITIONING_CENTER);	// set CURS_POSITIONING_CENTER
}
key_code_t key_macro_get_key(void)
{
	key_code_t key = -1;

	if (key_macro_is_playing_back()) {
		if (key_macro_playing_back < key_macro_recorded) {
			// playback one key stroke
			key = key_codes_recorded[key_macro_playing_back++];
			if (key_macro_playing_back >= key_macro_recorded) {
				key_macro_end_playback();
				set_edit_win_update_needed(UPDATE_SCRN_ALL);
			}
		}
	}
	return key;
}
void key_macro_end_playback(void)
{
	key_macro_playing_back = -1;		// stop playback
////	SET_APPMD_VAL(ed_CURS_POSITIONING, curs_positioning_mode_save);	// recover CURS_POSITIONING
}
int key_macro_is_playing_back(void)
{
	return key_macro_playing_back >= 0;
}
//-----------------------------------------------------------------------------
char *get_string_from_key_macro(void)
{
	static char line_buf[MAX_KEY_MACRO_STR_LEN + 1];
	int stroke_idx;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"
	const char *key_name;
	char buf_key_code[KEY_CODE_STR_LEN+1];		// "a", "\\", "\(RIGHT)", "\1234"

	strcpy__(line_buf, "");
	for (stroke_idx = 0; stroke_idx < key_macro_recorded; stroke_idx++) {
		if (key_codes_recorded[stroke_idx] == '\\') {
			// "\\" ==> '\'
			snprintf(buf_key_code, KEY_CODE_STR_LEN+1, "\\\\");
		} else if (isgraph(key_codes_recorded[stroke_idx])) {
			snprintf(buf_key_code, KEY_CODE_STR_LEN+1,
			 "%c", (unsigned short)key_codes_recorded[stroke_idx]);
		} else {
			key_name = key_name_from_key_code(key_codes_recorded[stroke_idx], buf_key_name);
			if (key_name[0])
				// "\(UP)"
				snprintf(buf_key_code, KEY_CODE_STR_LEN+1, "\\(%s)", key_name);
			else
				// "\0102"
				snprintf(buf_key_code, KEY_CODE_STR_LEN+1, "\\%04x",
				 key_codes_recorded[stroke_idx]);
		}
		if (strlen(line_buf) + strlen(buf_key_code) < MAX_KEY_MACRO_STR_LEN) {
			strlcat__(line_buf, MAX_KEY_MACRO_STR_LEN, buf_key_code);
		}
	}
flf_d_printf("%d [%s]\n", key_macro_recorded, line_buf);
///get_key_macro_from_string(line_buf);
	return line_buf;
}
void get_key_macro_from_string(const char *string)
{
	int stroke_idx;
	const char *str;
	char key_name[MAX_KEY_NAME_LEN+1];
	int len;
	key_code_t key_code;
	int int_key_code;

flf_d_printf("[%s]\n", string);
	if (string == NULL)
		return;
	str = string;
	for (stroke_idx = 0; str[0]; stroke_idx++) {
		if (str[0] == '\\') {
			if (str[1] == '\\') {
				key_codes_recorded[stroke_idx] = str[1];	// "\\"
				for (len = 0; len < 2 && *str; len++) {
					str++;
				}
			} else
			if (str[1] == '(') {
				// "\(UP)"
				for (len = 2; len < 2+MAX_KEY_NAME_LEN+1 && str[len]; len++) {
					if (str[len] == ')') {
						break;
					}
				}
				if (str[len] == ')') {
					strlcpy__(key_name, &str[2], len-2);	// "RIGHT"
					key_code = key_code_from_key_name(key_name);
					if (key_code >= 0) {
						key_codes_recorded[stroke_idx] = key_code;
					}
					str = &str[len+1];
				} else {
					str = &str[len];
				}
			} else {
				// "\0102"
				sscanf(&str[1], "%04x", &int_key_code);
				key_codes_recorded[stroke_idx] = int_key_code;
				for (len = 0; len < 5 && *str; len++) {
					str++;
				}
			}
		} else {
			key_codes_recorded[stroke_idx] = *str++;
		}
	}
	key_macro_recorded = stroke_idx;
flf_d_printf("%d key strokes\n", key_macro_recorded);
}

// End of keymacro.c
/**************************************************************************
 *   keys.c                                                               *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
 *   Copyright (C) 2010-2018 PCR                                          *
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

#include "headers.h"

//-----------------------------------------------------------------------------

func_key_table_t *app_func_key_table = editor_func_key_table;
PRIVATE void app_menu_n(int *group_idx_, int *entry_idx_);

void set_app_func_key_table(void)
{
#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
#endif // ENABLE_FILER
		app_func_key_table = editor_func_key_table;
#ifdef ENABLE_FILER
	} else {
		app_func_key_table = filer_func_key_table;
	}
#endif // ENABLE_FILER
}

void editor_menu_n(int grp_idx)
{
	static int prev_group_idx;
	static int prev_entry_idx;

	if (grp_idx >= 0) {
		prev_group_idx = grp_idx;
		prev_entry_idx = 1;
	}
	app_menu_n(&prev_group_idx, &prev_entry_idx);
}

#ifdef ENABLE_FILER
void filer_menu_n(int grp_idx)
{
	static int prev_group_idx;
	static int prev_entry_idx;

	if (grp_idx >= 0) {
		prev_group_idx = grp_idx;
		prev_entry_idx = 1;
	}
	app_menu_n(&prev_group_idx, &prev_entry_idx);
}
#endif // ENABLE_FILER

PRIVATE void app_menu_n(int *group_idx_, int *entry_idx_)
{
	key_code_t key_input;
	int again_ret;
	int group_idx = *group_idx_;
	int entry_idx = *entry_idx_;

app_menu_n_again:;
	while (1) {
		again_ret = 0;
		update_screen_app(1, 1, 0);
		disp_drop_down_menu(group_idx, entry_idx, main_win_get_mid_win_y(), group_idx * 2);
		tio_refresh();

		tio_set_cursor_on(0);
		//---------------------------
		key_input = input_key_loop();
		//---------------------------
mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));

		switch (key_input) {
		case K_LEFT:
			group_idx = group_idx - 1;
			goto do_menu_n_left_right;
		case K_RIGHT:
			group_idx = group_idx + 1;
			goto do_menu_n_left_right;
do_menu_n_left_right:;
			if (group_idx < 0)
				group_idx = get_func_key_table_from_key_groups() - 1;
			if (group_idx > get_func_key_table_from_key_groups() - 1)
				group_idx = 0;
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_table_from_key_entries(group_idx));
			again_ret = 1;
			break;
		case K_UP:
			entry_idx = entry_idx - 1;
			goto do_menu_n_up_down;
		case K_DOWN:
			entry_idx = entry_idx + 1;
			goto do_menu_n_up_down;
		case K_PPAGE:
			entry_idx = entry_idx - 5;
			goto do_menu_n_up_down;
		case K_NPAGE:	
			entry_idx = entry_idx + 5;
			goto do_menu_n_up_down;
do_menu_n_up_down:;
			if (entry_idx < 1)
				entry_idx = get_func_key_table_from_key_entries(group_idx);
			if (entry_idx > get_func_key_table_from_key_entries(group_idx))
				entry_idx = 1;
			entry_idx = MIN_MAX_(1, entry_idx, get_func_key_table_from_key_entries(group_idx));
			break;
		case K_C_M:
		case K_ENTER:
			exec_func(group_idx, entry_idx);
#ifndef ENABLE_FILER
			if (count_edit_bufs())
#else // ENABLE_FILER
			if (count_edit_bufs() || GET_APPMD(app_EDITOR_FILER))
#endif // ENABLE_FILER
				set_menu_key_for_do_app_menu_0();
			again_ret = 2;
			break;
		case K_ESC:
		case K_M_ESC:
			key_input = -1;
			again_ret = 2;
			break;
		default:
			if (get_app_function_for_key(key_input) != NULL)
				set_menu_key(key_input);
			again_ret = 2;
			break;
		}
		if (again_ret)
			break;
	}
	if (again_ret == 1)
		goto app_menu_n_again;

#ifndef ENABLE_FILER
	if (count_edit_bufs())
#else // ENABLE_FILER
	if (count_edit_bufs() || GET_APPMD(app_EDITOR_FILER))
#endif // ENABLE_FILER
		update_screen_app(1, 1, 1);

	tio_set_cursor_on(1);
	*group_idx_ = group_idx;
	*entry_idx_ = entry_idx;
}

int disp_drop_down_menu(int group_idx, int entry_idx, int yy, int xx)
{
	func_key_table_t *table;
	int idx;
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char template[] = "%-32s  %-5s %-5s  %-12s";
	char buffer[MAX_PATH_LEN+1];

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return 0;
	for (idx = 0; ; idx++) {
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(yy + idx, xx, " ", -1);
		if (table[idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		if (table[idx].desc[0]) {
			if (idx == entry_idx) {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_SELECTED, 0);
			} else {
				set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
			}
		}
		if (idx != 0 && table[idx].desc[0] == 0)
			snprintf(buffer, MAX_PATH_LEN+1, template, "", "", "", "");
		else
			snprintf(buffer, MAX_PATH_LEN+1, template,
			 table[idx].help,
			 short_key_name_from_key_code(table[idx].key1, buf1),
			 short_key_name_from_key_code(table[idx].key2, buf2),
			 table[idx].func_get ? (*table[idx].func_get)() : "" );
		main_win_output_string(-1, -1, buffer, -1);
		if (table[idx].desc[0]) {
			set_color_by_idx(ITEM_COLOR_IDX_MENU_ITEM, 0);
		}
		main_win_output_string(-1, -1, " ", -1);
		set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
		main_win_output_string(-1, -1, " ", -1);
		if (idx != 0 && table[idx].desc[0] == 0)
			break;
	}
	return 0;
}

int get_func_key_table_from_key_groups(void)
{
	int idx;
	int group_idx;

	group_idx = 0;
	for (idx = 0; app_func_key_table[idx].help[0]; idx++) {
		if (app_func_key_table[idx].desc[0] == 0) {
			group_idx++;
		}
	}
	return group_idx;
}

int get_func_key_table_from_key_entries(int group_idx)
{
	int idx;
	func_key_table_t *table;

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return 0;
	for (idx = 1; table[idx].desc[0]; idx++) {
		// loop
	}
	return idx - 1;
}

key_code_t get_func_key_code(int group_idx, int entry_idx)
{
	func_key_table_t *table;

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return -1;
	return table[entry_idx].key1;
}

void exec_func(int group_idx, int entry_idx)
{
	func_key_table_t *table;

	if ((table = get_func_key_table_from_key_group(group_idx)) == NULL)
		return;
	table[entry_idx].func();
}

func_key_table_t *get_func_key_table_from_key_group(int group_idx)
{
	int idx;

	for (idx = 0; app_func_key_table[idx].help[0]; idx++) {
////flf_d_printf("%s|%s\n", app_func_key_table[idx].desc, app_func_key_table[idx].help);
		if (app_func_key_table[idx].desc[0] == 0) {
			if (group_idx == 0)
				return &app_func_key_table[idx];
			group_idx--;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------

int cmp_func_id(const char *func_id_1, const char *func_id_2)
{
	return strcmp(func_id_1, func_id_2) == 0;
}
void *get_app_function_for_key(key_code_t key)
{
	func_key_table_t *func_key_table;

	func_key_table = get_func_key_table_from_key(app_func_key_table, key);
	if (func_key_table)
		return (void *)func_key_table->func;
	return NULL;
}
const char *get_func_id_from_key(key_code_t key)
{
	func_key_table_t *func_key_table;

	func_key_table = get_func_key_table_from_key(editor_func_key_table, key);
	if (func_key_table)
		return func_key_table->func_id;
#ifdef ENABLE_FILER
	func_key_table = get_func_key_table_from_key(filer_func_key_table, key);
	if (func_key_table)
		return func_key_table->func_id;
#endif // ENABLE_FILER
	return "";
}
func_key_table_t *get_func_key_table_from_key(func_key_table_t *key_table, key_code_t key)
{
	int idx;

	for (idx = 0; key_table[idx].help[0]; idx++) {
		if (is_key_bound_to_func(key, &key_table[idx])) {
////flf_d_printf("func_id: %s\n", key_table[idx].func_id);
			return &key_table[idx];
		}
	}
	return NULL;
}
key_code_t get_key_for_func_id(char *func_id)
{
	func_key_table_t *func_key_table;

	func_key_table = get_func_table_from_func_id(func_id);
	if (func_key_table == NULL)
		return 0;
	return func_key_table->key1;
}
PRIVATE func_key_table_t *get_func_table_from_func_id__(func_key_table_t *func_key_table,
 const char *func_id);
func_key_table_t *get_func_table_from_func_id(const char *func_id)
{
	func_key_table_t *func_key_table;

	func_key_table = get_func_table_from_func_id__(editor_func_key_table, func_id);
	if (func_key_table)
		return func_key_table;
#ifdef ENABLE_FILER
	func_key_table = get_func_table_from_func_id__(filer_func_key_table, func_id);
	if (func_key_table)
		return func_key_table;
#endif // ENABLE_FILER
	return NULL;
}
PRIVATE func_key_table_t *get_func_table_from_func_id__(func_key_table_t *func_key_table,
 const char *func_id)
{
	int idx;

	for (idx = 0; func_key_table[idx].help[0]; idx++) {
		if (strcmp(func_key_table[idx].func_id, func_id) == 0) {
			return &func_key_table[idx];
		}
	}
	return NULL;
}

int is_key_bound_to_func(key_code_t key, func_key_table_t *func_key_table)
{
	return key != NKA
	 && (key == func_key_table->key1
	  || key == func_key_table->key2
	  || key == func_key_table->key3);
}
void clear_keys_if_bound(key_code_t *keys)
{
	func_key_table_t *func_key_table = editor_func_key_table;
	int idx;

	for (idx = 0; func_key_table[idx].func != NULL; idx++) {
		clear_key_if_bound_to_func(keys[0], &func_key_table[idx]);
		clear_key_if_bound_to_func(keys[1], &func_key_table[idx]);
	}
}
void clear_key_if_bound_to_func(key_code_t key, func_key_table_t *func_key_table)
{
	if (func_key_table->key1 == key) {
		func_key_table->key1 = NKA;
	}
	if (func_key_table->key2 == key) {
		func_key_table->key2 = NKA;
	}
	if (func_key_table->key3 == key) {
		func_key_table->key3 = NKA;
	}
}
void clear_keys_bound_to_func(func_key_table_t *func_key_table)
{
	func_key_table->key1 = NKA;
	func_key_table->key2 = NKA;
}

void bind_key_to_func(func_key_table_t *func_key_table, key_code_t *keys)
{
	if (keys[0] >= 0)
		func_key_table->key1 = keys[0];
	if (keys[1] >= 0)
		func_key_table->key2 = keys[1];
	if (keys[2] >= 0)
		func_key_table->key3 = keys[2];
}

// 0x01 ==> "^A"
const char *short_key_name_from_func_id(char *buf)
{
	return short_key_name_from_key_code(get_key_for_func_id(buf), buf);
}

//-----------------------------------------------------------------------------

PRIVATE key_code_t menu_key = -1;
void set_menu_key(key_code_t key)
{
flf_d_printf("set_menu_key(%04x)\n", key);
	menu_key = key;
}
key_code_t get_menu_key(void)
{
	key_code_t key = -1;

	if (menu_key >= 0) {
		key = menu_key;
		menu_key = -1;
flf_d_printf("get_menu_key(%04x)\n", key);
	}
	return key;
}

void set_menu_key_for_do_app_menu_0(void)
{
#ifndef ENABLE_FILER
	set_menu_key(get_key_for_func_id("do_editor_menu_0"));
#else // ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0)
		set_menu_key(get_key_for_func_id("do_editor_menu_0"));
	else
		set_menu_key(get_key_for_func_id("do_filer_menu_0"));
#endif // ENABLE_FILER
}

//-----------------------------------------------------------------------------

PRIVATE key_code_t input_key_timeout(void);
PRIVATE key_code_t input_key_macro(void);
PRIVATE key_code_t input_key_bs_del(void);

key_code_t input_key_loop(void)
{
	key_code_t key;

	while ((key = input_key_wait_return()) < 0) {
	}
	return key;
}
key_code_t input_key_wait_return(void)
{
	static key_code_t prev_key = -1;
	key_code_t key = input_key_timeout();
	if (key < 0 && prev_key >= 0) {
		tio_repaint_all();
	}
	prev_key = key;
	return key;
}

PRIVATE key_code_t input_key_timeout(void)
{
	key_code_t key;
#define KEY_WAIT_TIME_USEC		1000000		// return every 1[Sec]
	long usec_enter;

	usec_enter = get_usec();
	while ((key = input_key_macro()) < 0) {
		if (tio_check_update_terminal_size()) {
			win_reinit_win_size();
			///win_show_win_size();
#ifdef ENABLE_HELP
			disp_splash(0);
			MSLEEP(1000);
#endif // ENABLE_HELP
			update_screen_app(1, 1, 1);
		}
		if (get_usec() - usec_enter >= KEY_WAIT_TIME_USEC)
			break;
		MSLEEP(10);		// wait 10[mS]
	}
	return key;
}
PRIVATE key_code_t input_key_macro(void)
{
	key_code_t key = -1;

	if ((key = get_menu_key()) >= 0) {
		return key;
	}
#ifndef ENABLE_HISTORY
	key = input_key_bs_del();
#else
	key = key_macro_get_key();
	if (key < 0) {
		key = input_key_bs_del();
	}
	key_macro_put_key(key);
#endif // ENABLE_HISTORY
	return key;
}
PRIVATE key_code_t input_key_bs_del(void)
{
	key_code_t key;

	key = tio_input_key();
	switch(key) {
	case 0x7f:
#define MAP_KEY_7F_BS
#ifdef MAP_KEY_7F_BS
		if (GET_APPMD(app_MAP_KEY_7F_BS)) {
flf_d_printf("KEY_7F ==> BS\n");
			key = K_BS;		// 0x7f ==> BS
		} else {
#endif // MAP_KEY_7F_BS
flf_d_printf("KEY_7F ==> DEL\n");
			key = K_DEL;	// 0x7f ==> Delete
#ifdef MAP_KEY_7F_BS
		}
#endif // MAP_KEY_7F_BS
		break;
	case KEY_BACKSPACE:		// 0x0107
#ifdef MAP_KEY_7F_BS
		if (GET_APPMD(app_MAP_KEY_7F_BS) == 0) {
flf_d_printf("KEY_BACKSPACE ==> BS\n");
			key = K_BS;		// KEY_BACKSPACE ==> BS
		} else {
#endif // MAP_KEY_7F_BS
flf_d_printf("KEY_BACKSPACE ==> DEL\n");
			key = K_DEL;	// KEY_BACKSPACE ==> DEL
#ifdef MAP_KEY_7F_BS
		}
#endif // MAP_KEY_7F_BS
		break;
	case K_DC:
flf_d_printf("K_DC\n");
		// Delete(0x0113) key code was seen.
		SET_APPMD(app_MAP_KEY_7F_BS);	// DEL(0x7f) key code ==> BS
		break;
	}
	return key;
}

//-----------------------------------------------------------------------------

key_name_table_t key_name_table[] = {
//					   12345
	{ ' '			, "SP", },
	{ K_BS			, "BS", },
	{ K_TAB			, "TAB", },
	{ K_ENTER		, "ENTER", },
	{ K_ESC			, "ESC", },
	{ K_C_AT		, "C-@", },
	{ K_C_A			, "C-A", },
	{ K_C_B			, "C-B", },
	{ K_C_C			, "C-C", },
	{ K_C_D			, "C-D", },
	{ K_C_E			, "C-E", },
	{ K_C_F			, "C-F", },
	{ K_C_G			, "C-G", },
	{ K_C_H			, "C-H", },
	{ K_C_J			, "C-J", },
	{ K_C_K			, "C-K", },
	{ K_C_L			, "C-L", },
	{ K_C_M			, "C-M", },
	{ K_C_N			, "C-N", },
	{ K_C_O			, "C-O", },
	{ K_C_P			, "C-P", },
	{ K_C_Q			, "C-Q", },
	{ K_C_R			, "C-R", },
	{ K_C_S			, "C-S", },
	{ K_C_T			, "C-T", },
	{ K_C_U			, "C-U", },
	{ K_C_V			, "C-V", },
	{ K_C_W			, "C-W", },
	{ K_C_X			, "C-X", },
	{ K_C_Y			, "C-Y", },
	{ K_C_Z			, "C-Z", },
	{ K_C_LBRACKET	, "C-[", },
	{ K_C_BACKSLASH	, "C-\\", },
	{ K_C_RBRACKET	, "C-]", },
	{ K_C_CARET		, "C-^", },
	{ K_C_UNDERLINE	, "C-_", },
	{ K_M_ESC		, "M-ESC", },
	{ K_M_0			, "M-0", },
	{ K_M_1			, "M-1", },
	{ K_M_2			, "M-2", },
	{ K_M_3			, "M-3", },
	{ K_M_4			, "M-4", },
	{ K_M_5			, "M-5", },
	{ K_M_6			, "M-6", },
	{ K_M_7			, "M-7", },
	{ K_M_8			, "M-8", },
	{ K_M_9			, "M-9", },
	{ K_M_a			, "M-a", },
	{ K_M_b			, "M-b", },
	{ K_M_c			, "M-c", },
	{ K_M_d			, "M-d", },
	{ K_M_e			, "M-e", },
	{ K_M_f			, "M-f", },
	{ K_M_g			, "M-g", },
	{ K_M_h			, "M-h", },
	{ K_M_i			, "M-i", },
	{ K_M_j			, "M-j", },
	{ K_M_k			, "M-k", },
	{ K_M_l			, "M-l", },
	{ K_M_m			, "M-m", },
	{ K_M_n			, "M-n", },
	{ K_M_o			, "M-o", },
	{ K_M_p			, "M-p", },
	{ K_M_q			, "M-q", },
	{ K_M_r			, "M-r", },
	{ K_M_s			, "M-s", },
	{ K_M_t			, "M-t", },
	{ K_M_u			, "M-u", },
	{ K_M_v			, "M-v", },
	{ K_M_w			, "M-w", },
	{ K_M_x			, "M-x", },
	{ K_M_y			, "M-y", },
	{ K_M_z			, "M-z", },
	{ K_M_A			, "M-A", },
	{ K_M_B			, "M-B", },
	{ K_M_C			, "M-C", },
	{ K_M_D			, "M-D", },
	{ K_M_E			, "M-E", },
	{ K_M_F			, "M-F", },
	{ K_M_G			, "M-G", },
	{ K_M_H			, "M-H", },
	{ K_M_I			, "M-I", },
	{ K_M_J			, "M-J", },
	{ K_M_K			, "M-K", },
	{ K_M_L			, "M-L", },
	{ K_M_M			, "M-M", },
	{ K_M_N			, "M-N", },
	{ K_M_O			, "M-O", },
	{ K_M_P			, "M-P", },
	{ K_M_Q			, "M-Q", },
	{ K_M_R			, "M-R", },
	{ K_M_S			, "M-S", },
	{ K_M_T			, "M-T", },
	{ K_M_U			, "M-U", },
	{ K_M_V			, "M-V", },
	{ K_M_W			, "M-W", },
	{ K_M_X			, "M-X", },
	{ K_M_Y			, "M-Y", },
	{ K_M_Z			, "M-Z", },
	{ K_MC_A		, "MC-A", },
	{ K_MC_B		, "MC-B", },
	{ K_MC_C		, "MC-C", },
	{ K_MC_D		, "MC-D", },
	{ K_MC_E		, "MC-E", },
	{ K_MC_F		, "MC-F", },
	{ K_MC_G		, "MC-G", },
	{ K_MC_H		, "MC-H", },
	{ K_MC_I		, "MC-I", },
	{ K_MC_J		, "MC-J", },
	{ K_MC_K		, "MC-K", },
	{ K_MC_L		, "MC-L", },
	{ K_MC_M		, "MC-M", },
	{ K_MC_N		, "MC-N", },
	{ K_MC_O		, "MC-O", },
	{ K_MC_P		, "MC-P", },
	{ K_MC_Q		, "MC-Q", },
	{ K_MC_R		, "MC-R", },
	{ K_MC_S		, "MC-S", },
	{ K_MC_T		, "MC-T", },
	{ K_MC_U		, "MC-U", },
	{ K_MC_V		, "MC-V", },
	{ K_MC_W		, "MC-W", },
	{ K_MC_X		, "MC-X", },
	{ K_MC_Y		, "MC-Y", },
	{ K_MC_Z		, "MC-Z", },
	{ K_F01			, "F1", },
	{ K_F02			, "F2", },
	{ K_F03			, "F3", },
	{ K_F04			, "F4", },
	{ K_F05			, "F5", },
	{ K_F06			, "F6", },
	{ K_F07			, "F7", },
	{ K_F08			, "F8", },
	{ K_F09			, "F9", },
	{ K_F10			, "F10", },
	{ K_F11			, "F11", },
	{ K_F12			, "F12", },
	{ K_SF01		, "S-F1", },
	{ K_SF02		, "S-F2", },
	{ K_SF03		, "S-F3", },
	{ K_SF04		, "S-F4", },
	{ K_SF05		, "S-F5", },
	{ K_SF06		, "S-F6", },
	{ K_SF07		, "S-F7", },
	{ K_SF08		, "S-F8", },
	{ K_SF09		, "S-F9", },
	{ K_SF10		, "S-F10", },
	{ K_SF11		, "S-F11", },
	{ K_SF12		, "S-F12", },
	{ K_IC			, "INS", },
	{ K_DC			, "DC", },
	{ K_DEL			, "DEL", },
	{ K_HOME		, "HOME", },
	{ K_END			, "END", },
	{ K_PPAGE		, "PGUP", },
	{ K_NPAGE		, "PGDN", },
	{ K_UP			, "UP", },
	{ K_DOWN		, "DOWN", },
	{ K_LEFT		, "LEFT", },
	{ K_RIGHT		, "RIGHT", },
};

const char *short_key_name_from_key_code(key_code_t key_code, char *buf)
{
	static char buf_s_[MAX_KEY_NAME_LEN+1];
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	if (buf == NULL) {
		buf = buf_s_;
	}
	return short_key_name_from_key_name(key_name_from_key_code(key_code, buf_key_name), buf);
}
const char *key_name_from_key_code(key_code_t key_code, char *buf)
{
	int idx;

	for (idx = 0; idx < ARRAY_SIZE_OF(key_name_table); idx++) {
		if (key_name_table[idx].key_code == key_code)
			return key_name_table[idx].key_name;
	}
///flf_d_printf("key_code:[%d]\n", key_code);
	if (isgraph(key_code)) {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%c", key_code);
	} else if ((key_code & 0xff00) == K_M(0)) {	// 0x1bxx
		if (isgraph(key_code & 0x00ff)) {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "M-%c", key_code & 0x00ff);
		} else if (0 <= (key_code & 0x00ff) && (key_code & 0x00ff) < 0x20) {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "MC-%c", '@' + (key_code & 0xff));
		} else {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "%04x", key_code);
		}
	} else if (key_code == NKA) {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "---");
	} else {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%04x", key_code);
	}
	return buf;
}
const char *short_key_name_from_key_name(const char *key_name, char *buf)
{
	if (strlcmp__(key_name, "C-") == 0) {
		// "C-x" ==> "^x"
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%s%s", "^", &key_name[2]);
	} else
	if (strlcmp__(key_name, "M-") == 0) {
		// "M-x" ==> "@x"
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%s%s", "@", &key_name[2]);
	} else
	if (strlcmp__(key_name, "MC-") == 0) {
		// "MC-x" ==> "@^x"
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%s%s", "@^", &key_name[3]);
	} else {
		strlcpy__(buf, key_name, MAX_KEY_NAME_LEN);
	}
	return buf;
}

key_code_t key_code_from_key_name(char *key_name)
{
	int idx;

	for (idx = 0; idx < ARRAY_SIZE_OF(key_name_table); idx++) {
		if (strcmp(key_name_table[idx].key_name, key_name) == 0)
			return key_name_table[idx].key_code;
	}
	return -1;
}
key_code_t key_code_from_short_key_name(char *short_key_name)
{
	int idx;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	for (idx = 0; idx < ARRAY_SIZE_OF(key_name_table); idx++) {
		if (strcmp(short_key_name_from_key_name(key_name_table[idx].key_name, buf_key_name),
		 short_key_name) == 0)
			return key_name_table[idx].key_code;
	}
	return -1;
}

int get_key_name_table_entries(void)
{
	return ARRAY_SIZE_OF(key_name_table);
}

// End of keys.c
/**************************************************************************
 *   line.c                                                               *
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

#include "headers.h"

// be_line_t manipulation routines ============================================

be_line_t *line_create(void)
{
	be_line_t *line;

	line = (be_line_t *)malloc__(sizeof(be_line_t));
	return line_init(line, "");
}
be_line_t *line_init(be_line_t *line, const char *string)
{
	// initialize be_line_t
	line->prev = NULL;
	line->next = NULL;
	line->data = NULL;
	line->size = 0;
	line->line_num = 0;
	line->buf_size = 0;
	if (string && string[0]) {
		line_set_string(line, string);
	}
	return line;
}
be_line_t *line_create_with_string(const char *string)
{
	return line_create_with_string_len(string, -1);
}
be_line_t *line_create_with_string_len(const char *string, len_t len)
{
	be_line_t *new_line;

	new_line = line_create();
	return line_set_string_len(new_line, string, len);
}
be_line_t *line_set_string(be_line_t *line, const char *string)
{
	return line_set_string_len(line, string, -1);
}
be_line_t *line_set_string_len(be_line_t *line, const char *string, len_t len)
{
	char buffer[MAX_EDIT_LINE_LEN + 1];

	if (line->data) {
		if (IS_OVERWRAP(line->data, line->data + MAX_EDIT_LINE_LEN,
		 string, string + MAX_EDIT_LINE_LEN)) {
			// string will be destroyed in remalloc(), copy it to temporary buffer
			strlcpy__(buffer, string, MAX_EDIT_LINE_LEN);
			string = buffer;
		}
	}
	if (len < 0) {
		// limit length by MAX_EDIT_LINE_LEN
		len = byte_idx_from_byte_idx(string, MAX_EDIT_LINE_LEN);
	} else {
		len = byte_idx_from_byte_idx(string, LIM_MAX(len, MAX_EDIT_LINE_LEN));
	}
	line->size = len + 1;
	line->data = char_remalloc(line->data, len + 1);
	strlcpy__(line->data, string, len);	// copy string
	return line;			// return line
}
char *line_get_string(be_line_t *line)
{
	return line->data;
}
void line_free(be_line_t *line)
{
	if (line) {
		if (line->data) {
			FREE_CLR_PTR(line->data);
		}
		free__(line);
	}
}

// This is deep-copy.
be_line_t *line_create_copy(be_line_t *src)
{
	return line_copy(line_create(), src);
}
// This is deep-copy.
be_line_t *line_copy(be_line_t *dest, be_line_t *src)
{
	memcpy__(dest, src, sizeof(*src));
	dest->prev = NULL;
	dest->next = NULL;
	dest->data = NULL;
	return line_set_string(dest, src->data);
}

be_line_t *line_insert(be_line_t *line, be_line_t *new_line,
 insert_before_after_t before_after)
{
	if (before_after == INSERT_BEFORE) {
		return line_insert_before(line, new_line);
	} else {
		return line_insert_after(line, new_line);
	}
}
be_line_t *line_insert_before(be_line_t *line, be_line_t *new_line)
{
	return line_insert_between(NODE_PREV(line), new_line, line);
}
be_line_t *line_insert_after(be_line_t *line, be_line_t *new_line)
{
	return line_insert_between(line, new_line, NODE_NEXT(line));
}
// Splice a line into an existing be_line_t
be_line_t *line_insert_between(be_line_t *prev, be_line_t *new_line, be_line_t *next)
{
	line_link(prev, new_line);
	line_link(new_line, next);
	return new_line;
}
// Connect two lines
be_line_t *line_link(be_line_t *prev, be_line_t *next)
{
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	return prev;
}

be_line_t *line_unlink_free(be_line_t *line)
{
	be_line_t *next_line;

	next_line = NODE_NEXT(line);
	line_unlink(line);
	line_free(line);
	return next_line;	// return the next line
}
// Unlink a line from the rest of the be_line_t
be_line_t *line_unlink(be_line_t *line)
{
	line_avoid_wild_ptr_cur(line);
	if (line->prev) {
		line->prev->next = NODE_NEXT(line);
	}
	if (line->next) {
		line->next->prev = NODE_PREV(line);
	}
	line->prev = NULL;
	line->next = NULL;
	return line;
}

be_line_t *line_concat(be_line_t *line, concat_prev_next_t prev_next)
{
	if (prev_next == WITH_PREV) {
		return line_concat_with_prev(line);
	} else {
		return line_concat_with_next(line);
	}
}
// concatenate the line with the previous line
//[before]
//  aaaa			This line is freed.
// >bbbb			This line is returned.
//[after]
// >aaaabbbb
be_line_t *line_concat_with_prev(be_line_t *line)
{
	if (IS_NODE_TOP_ANCH(line)) {
		_PROGERR_
		return line;
	}
	if (IS_NODE_BOT_ANCH(NODE_PREV(line))) {
		_PROGERR_
		return line;
	}
	//  aaaa
	// >bbbb
	line_insert_string(line, 0, NODE_PREV(line)->data, -1);
	//  aaaa
	// >aaaabbbb
	line_unlink_free(NODE_PREV(line));
	// >aaaabbbb
	return line;
}
// concatenate the line with the next line
//[before]
// >aaaa			This line is returned.
//  bbbb			This line is freed.
//[after]
// >aaaabbbb
be_line_t *line_concat_with_next(be_line_t *line)
{
	if (IS_NODE_TOP_ANCH(line)) {
		_PROGERR_
		return line;
	}
	if (IS_NODE_BOT_ANCH(NODE_NEXT(line))) {
		_PROGERR_
		return line;
	}
	// >aaaa
	//  bbbb
	line_insert_string(line, line_data_len(line), NODE_NEXT(line)->data, -1);
	// >aaaabbbb
	//  bbbb
	line_unlink_free(NODE_NEXT(line));
	// >aaaabbbb
	return line;
}
//-----------------------------------------------------------------------------
be_line_t *line_delete_string(be_line_t *line, int byte_idx, int delete_len)
{
	return line_replace_string(line, byte_idx, delete_len, "", 0);
}
be_line_t *line_insert_string(be_line_t *line, int byte_idx, const char *string, int insert_len)
{
	return line_replace_string(line, byte_idx, 0, string, insert_len);
}
be_line_t *line_replace_string(be_line_t *line, int byte_idx, int delete_len,
 const char *string, len_t insert_len)
{
	char buffer[MAX_EDIT_LINE_LEN + 1];

	strlcpy__(buffer, line->data, MAX_EDIT_LINE_LEN);
	replace_str(buffer, MAX_EDIT_LINE_LEN, byte_idx, delete_len, string, insert_len);
	return line_set_string(line, buffer);
}
//-----------------------------------------------------------------------------
be_line_t *line_insert_with_string(be_line_t *line, insert_before_after_t before_after,
 const char *string)
{
	return line_insert_with_string_len(line, before_after, string, -1);
}
be_line_t *line_insert_with_string_len(be_line_t *line, insert_before_after_t before_after,
 const char *string, len_t len)
{
	be_line_t *new_line;

	new_line = line_create_with_string_len(string, len);
	line_insert(line, new_line, before_after);
	return new_line;			// return inserted line
}
//-----------------------------------------------------------------------------
be_line_t *line_separate(be_line_t *line, int byte_idx, insert_before_after_t before_after)
{
	be_line_t *new_line;

	if (before_after == INSERT_BEFORE) {
		// insert before
		// aaaa^BBBBBB
		new_line = line_insert_with_string_len(line, before_after, line->data, byte_idx);
		// aaaa				<== newly created line
		// aaaa^BBBBBB
		line_set_string(line, &(line->data[byte_idx]));		// aaaaBBBBBB ==> BBBBBB
		// aaaa				<== newly created line
		// BBBBBB
	} else {
		// insert after
		// aaaa^bbbbbb
		new_line = line_insert_with_string(line, before_after, &line->data[byte_idx]);
		// aaaa^BBBBBB
		// BBBBBB			<== newly created line
		line_set_string_len(line, line->data, byte_idx);	// aaaaBBBBBB ==> aaaa
		// aaaa
		// BBBBBB			<== newly created line
	}
	return new_line;			// return newly created(inserted) line
}
//-----------------------------------------------------------------------------
int line_renumber_from_line(be_line_t *line, size_t *_buf_size_)
{
	int line_num = 0;
	size_t buf_size = 0;

	if (IS_NODE_TOP_ANCH(line)) {
		line = NODE_NEXT(line);
	} else if (IS_NODE_TOP(line)) {
		// nothing to do
	} else {
		// continue renumbering from middle line
		line_num = NODE_PREV(line)->line_num;
		buf_size = NODE_PREV(line)->buf_size;
	}
	for ( ; line && IS_NODE_INT(line); line = NODE_NEXT(line)) {
		line->line_num = ++line_num;
		if (line->size <= 0) {
			if (line->data)
				line->size = line_data_len(line) + 1;
		}
		buf_size += line->size;
		line->buf_size = buf_size;
	}
	if (_buf_size_) {
		*_buf_size_ = buf_size;
	}
	return line_num;
}

int line_count_lines(be_line_t *line)
{
	int count;

	for (count = 0; IS_NODE_INT(line); count++) {
		line = NODE_NEXT(line);
	}
	return count;
}

const be_line_t *line_get_top_anch(const be_line_t *line)
{
	for ( ; IS_NODE_INT(line); ) {
		line = NODE_PREV(line);
	}
	return line;
}

size_t line_data_len(const be_line_t *line)
{
	return line_data_strlen(line->data);
}
size_t line_data_strlen(const char *str)
{
	return strnlen(str, MAX_EDIT_LINE_LEN);
}

#ifdef ENABLE_DEBUG
void line_dump_lines_from_top(const be_line_t *line, int lines, const be_line_t *cur_line)
{
	line_dump_lines(line_get_top_anch(line), lines, cur_line);
}
void line_dump_lines(const be_line_t *line, int lines, const be_line_t *cur_line)
{
	for ( ; line && lines > 0; line = NODE_NEXT(line), lines--) {
		line_dump_cur(line, cur_line);
	}
}
void line_dump(const be_line_t *line)
{
	line_dump_cur(line, NULL);
}
void line_dump_cur(const be_line_t *line, const be_line_t *cur_line)
{
	if (line == NULL) {
		progerr_printf("line == NULL\n");
		return;
	}
	flf_d_printf("%s%03d,%08lx,<%08lx,>%08lx,%04d,%06d,%08lx[%s]\n",
	 line == cur_line ? ">" : " ",
	 line->line_num,
	 line, NODE_PREV(line), NODE_NEXT(line),
	 line->size, line->buf_size,
	 line->data, line->data);
}
void line_dump_byte_idx(const be_line_t *line, int byte_idx)
{
	flf_d_printf("%3d: ", line->line_num);	dump_str_w_caret(line->data, byte_idx);
}
#endif // ENABLE_DEBUG

// End of line.c
/**************************************************************************
 *   linewrap.c                                                           *
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

#include "headers.h"

int priv_tab_size = DEFAULT_TAB_SIZE;	// Tab size gotten from buf_state

void set_wrap_line_tab_size(int tab_size)
{
	priv_tab_size = tab_size;
////flf_d_printf("priv_tab_size:%d\n", priv_tab_size);
}

//-----------------------------------------------------------------------------

#ifdef START_UP_TEST
void test_wrap_line(void)
{
flf_d_printf("----------------------------------------------------------------------\n");
flf_d_printf("priv_tab_size: %d\n", priv_tab_size);
#define CMP_RET(func_call, ret)		_FLF_; if (func_call != ret) _WARNING_
#define C_R(func_call, ret)			CMP_RET(func_call, ret)
	C_R(max_wrap_line_idx("1234567890", 10), 0)
	C_R(max_wrap_line_idx("123456789012", 10), 1)
	C_R(max_wrap_line_idx("1234567890123456789012", 10), 2)
	C_R(max_wrap_line_idx("一二三四五六七八九零", 10), 1)
	C_R(max_wrap_line_idx("一二三四五六七八九零一", 10), 2)
	C_R(max_wrap_line_idx("1一二三四五六七八九", 10), 1)
	C_R(max_wrap_line_idx("1一二三四五六七八九零", 10), 2)
	C_R(max_wrap_line_idx("1一二三四五123456789012", 10), 2)
	C_R(max_wrap_line_idx("123456\t78", 10), 0)
	C_R(max_wrap_line_idx("1234567\t89", 10), 0)
	C_R(max_wrap_line_idx("12345678\t90", 10), 1)
	C_R(max_wrap_line_idx("123456789\t01", 10), 1)
	C_R(max_wrap_line_idx("123456\t12", 12), 0)
	C_R(max_wrap_line_idx("1234567\t12", 12), 0)
	C_R(max_wrap_line_idx("12345678\t12", 12), 1)
	C_R(max_wrap_line_idx("123456789\t12", 12), 1)
	C_R(max_wrap_line_idx("1234567890\t12", 12), 1)
	C_R(max_wrap_line_idx("12345678901\t12", 12), 1)
	C_R(max_wrap_line_idx("123456789012\t12", 12), 1)
	C_R(max_wrap_line_idx("1234567890123\t12", 12), 1)
	C_R(max_wrap_line_idx("12345678901234\t12", 12), 1)

	C_R(start_wl_idx_of_wrap_line("1234567890", 8, 10), 0)
	C_R(start_wl_idx_of_wrap_line("1234567890", 10, 10), 1)
	C_R(start_wl_idx_of_wrap_line("123456789012", 10, 10), 1)
	C_R(start_wl_idx_of_wrap_line("123456789012", 12, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1234567890123456789012", 20, 10), 2)
	C_R(start_wl_idx_of_wrap_line("1234567890123456789012", 22, 10), 2)

	C_R(start_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 10, 10), 0)
	C_R(start_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 10, 10), 0)
	C_R(end_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 0, 10, 10), 10)
	C_R(end_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 1, 10, 10), 10)
	C_R(start_byte_idx_of_wrap_line("1234567890123456789012345678901234567890", 0, 10, 10), 10)
	C_R(start_byte_idx_of_wrap_line("1234567890123456789012345678901234567890", 1, 10, 10), 20)
	C_R(end_byte_idx_of_wrap_line_le("1234567890123456789012345678901234567890", 0, 10, 10), 10)
	C_R(end_byte_idx_of_wrap_line_le("1234567890123456789012345678901234567890", 1, 10, 10), 20)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 0, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 15, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 15, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 30, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 30, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 45, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 0, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 13, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 13, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 28, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 28, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 43, 10), 4)

	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")-1, 10), 0)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四"), 10), 0)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+2, 10), 1)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五"), 10), 1)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三"), 10), 0)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+2, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四"), 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+2, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五"), 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五")+1, 10), 1)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")-1, 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+1, 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+2, 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五")+1, 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+1, 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+2, 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+1, 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+2, 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五"), 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五")+1, 10), 1)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三"), 10), 6)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三")+1, 10), 8)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三")+2, 10), 8)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四"), 10), 8)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+1, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+2, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五"), 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五")+1, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三"), 10), 7)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+1, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+2, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四"), 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+1, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+2, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五"), 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五")+1, 10), 4)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 0, strlen("一二三四"), 10), 8)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 0, strlen("一二三四")+1, 10), 10)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 0, strlen("一二三四")+2, 10), 10)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 1, strlen("一二三四五"), 10), 10)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 1, strlen("一二三四五")+1, 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 0, strlen("1一二三四"), 10), 9)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 0, strlen("1一二三四")+1, 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 0, strlen("1一二三四")+2, 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 1, strlen("1一二三四五"), 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 1, strlen("1一二三四五")+1, 10), 4)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 7, 10), 12)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 8, 10), 12)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 9, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 10, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 11, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 0, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 1, 10), 18)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 2, 10), 18)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 3, 10), 21)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 7, 10), 10)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 8, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 9, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 10, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 11, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 0, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 1, 10), 16)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 2, 10), 16)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 3, 10), 19)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 9, 10), 12)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 10, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 11, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 0, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 1, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 2, 10), 18)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 3, 10), 18)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 9, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 10, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 11, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 0, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 1, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 2, 10), 16)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 3, 10), 16)
}
#endif // START_UP_TEST

// line wrapping macros
#define DEFINE_WRAP_LINE_VARS	\
	const char *ptr = line;		\
	int wl_idx = 0;				\
	int col_idx = 0;			\
	int total_col_idx = 0;		\
	int chr_cols;
#define CHAR_COLUMNS(ptr)													\
	((*ptr == '\0') ? 0 :													\
	 ((*ptr == '\t') ? (priv_tab_size - total_col_idx % priv_tab_size) :	\
	  ((*ptr == '\n') ? 1 :													\
	   ((is_ctrl_char((unsigned char)*ptr)) ? 2 :							\
	    (((unsigned char)*ptr < 0x80) ? 1 : utf8c_columns(ptr))				\
	   )																	\
	  )																		\
	 )																		\
	)
#define FORWARD_WRAP_LINE_COL_IDX()		\
	chr_cols = CHAR_COLUMNS(ptr);		\
	total_col_idx += chr_cols;			\
	col_idx += chr_cols;
#define FORWARD_WRAP_LINE_PTR()			\
	ptr += utf8c_bytes(ptr);
#define GOTO_NEXT_LINE_IF_LINE_EXCEED()		\
	if (*ptr == '\t') {						\
		if (col_idx >= width) {				\
			col_idx -= width;				\
			wl_idx++;						\
		}									\
	} else {								\
		if (col_idx > width) {				\
			col_idx = chr_cols;				\
			wl_idx++;						\
		}									\
	}
#define GOTO_NEXT_LINE_IF_LINE_END_COL()	\
	if (col_idx == width) {					\
		col_idx = 0;						\
		wl_idx++;							\
	}
#define GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS()	\
	if (*ptr != '\t') {						\
		/* If the next char would cause line-wrapping, */ \
		/* go to the next line earlier. */	\
		chr_cols = CHAR_COLUMNS(ptr);		\
		if (col_idx + chr_cols > width) {	\
			col_idx = 0;	/* go to top of the next line */ \
			wl_idx++;						\
		}									\
	}

//[全角wrapping]
// Ex1
//  "aaaaaaaa漢"
//  |aaaaaaaa漢|
// Ex2
//  "aaaaaaaaa漢"
//  |aaaaaaaaa |
//  |漢        |
//[TAB wrapping]
// Ex1
//  "aaaaaaaa>>>>bbbb"
//  |aaaaaaaa>>|
//  |>>bbbb    |

int max_wrap_line_idx(const char *line, int width)
{
	int line_idx;

////flf_d_printf("[%s]\n", line);
////flf_d_printf("width:%d\n", width);
	line_idx = end_wl_idx_of_wrap_line(line, INT_MAX, width);
////flf_d_printf(" ==>max_line_idx:%d\n", line_idx);
	return line_idx;
}
int start_wl_idx_of_wrap_line(const char *line, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("byte_idx:%d width:%d\n", byte_idx, width);
//// if (byte_idx < strlen(line)) {
////  flf_d_printf("[%s]\n", &line[byte_idx]);
//// }
	for ( ; *ptr; ) {
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d byte_idx:%d\n",
//// wl_idx, col_idx, ptr - line, byte_idx);
		if (ptr - line >= byte_idx)
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
////flf_d_printf(" ==>(wl_idx:%d) col_idx:%d byte_idx:%d\n", wl_idx, col_idx, ptr - line);
	return wl_idx;
}
int end_wl_idx_of_wrap_line(const char *line, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("[%s]\n", line);
////flf_d_printf("byte_idx:%d width:%d\n", byte_idx, width);
//// if (byte_idx < strlen(line)) {
////  flf_d_printf("[%s]\n", &line[byte_idx]);
//// }
	for ( ; *ptr; ) {
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d byte_idx:%d\n",
//// wl_idx, col_idx, ptr - line, byte_idx);
		if (ptr - line >= byte_idx)
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d byte_idx:%d\n",
//// wl_idx, col_idx, ptr - line, byte_idx);
		if (ptr - line >= byte_idx)
			break;
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
////flf_d_printf(" ==>(wl_idx:%d) col_idx:%d byte_idx:%d\n", wl_idx, col_idx, ptr - line);
	return wl_idx;
}
// line_idx=0, byte_idx=80 ==> col_idx=0
int start_col_idx_of_wrap_line(const char *line, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("[%s]\n", line);
////flf_d_printf("byte_idx:%d width:%d\n", byte_idx, width);
//// if (byte_idx < strlen(line)) {
////  flf_d_printf("[%s]\n", &line[byte_idx]);
//// }
	for ( ; *ptr; ) {
		if (ptr - line >= byte_idx)
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
////flf_d_printf(" ==>wl_idx:%d (col_idx:%d) byte_idx:%d\n", wl_idx, col_idx, ptr - line);
	return col_idx;
}
// line_idx=0, byte_idx=80 ==> col_idx=80
int end_col_idx_of_wrap_line(const char *line, int line_idx, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("[%s]\n", line);
////flf_d_printf("line_idx:%d byte_idx:%d width:%d\n", line_idx, byte_idx, width);
//// if (byte_idx < strlen(line)) {
////  flf_d_printf("[%s]\n", &line[byte_idx]);
//// }
	for ( ; *ptr; ) {
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d byte_idx:%d\n",
//// wl_idx, col_idx, ptr - line, byte_idx);
		if ((ptr - line >= byte_idx)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		if ((ptr - line >= byte_idx)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
	}
////flf_d_printf(" ==>wl_idx:%d (col_idx:%d) byte_idx:%d\n", wl_idx, col_idx, ptr - line);
	return col_idx;
}
int start_byte_idx_of_wrap_line(const char *line, int line_idx, int column, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("[%s]\n", line);
////flf_d_printf("line_idx:%d column:%d width:%d\n", line_idx, column, width);
//// if (column <= 256) {
////  flf_d_printf("[%*s|]\n", column, "");
//// }
	for ( ; *ptr; ) {
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d column:%d\n",
//// wl_idx, col_idx, ptr - line, column);
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
		if ((wl_idx >= line_idx && col_idx > column)
		 || (wl_idx > line_idx))
			break;
	}
////flf_d_printf(" ==>wl_idx:%d col_idx:%d (byte_idx:%d)\n", wl_idx, col_idx, ptr - line);
	return ptr - line;
}
int end_byte_idx_of_wrap_line_le(const char *line, int line_idx, int column, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("[%s]\n", line);
////flf_d_printf("line_idx:%d column:%d width:%d\n", line_idx, column, width);
//// if (column <= 256) {
////  flf_d_printf("[%*s|]\n", column, "");
//// }
	for ( ; *ptr; ) {
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d column:%d\n",
//// wl_idx, col_idx, ptr - line, column);
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
		FORWARD_WRAP_LINE_COL_IDX();
		if ((wl_idx >= line_idx && col_idx > column)
		 || (wl_idx > line_idx))
			break;
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
	}
////flf_d_printf(" ==>wl_idx:%d col_idx:%d (byte_idx:%d)\n", wl_idx, col_idx, ptr - line);
	return ptr - line;
}
int end_byte_idx_of_wrap_line_ge(const char *line, int line_idx, int column, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
////flf_d_printf("[%s]\n", line);
////flf_d_printf("line_idx:%d column:%d width:%d\n", line_idx, column, width);
//// if (column <= 256) {
////  flf_d_printf("[%*s|]\n", column, "");
//// }
	for ( ; *ptr; ) {
////flf_d_printf("wl_idx:%d col_idx:%d ptr-line:%d column:%d\n",
//// wl_idx, col_idx, ptr - line, column);
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
////flf_d_printf(" ==>wl_idx:%d col_idx:%d (byte_idx:%d)\n", wl_idx, col_idx, ptr - line);
	return ptr - line;
}
int wrap_line_length(int width)
{
	return width > 0
	 ? width
	 : (CUR_EBUF_STATE(buf_LINE_WRAP_MODE) == 0
	  ? (MAX_EDIT_LINE_LEN * priv_tab_size + 1)		// will never wrap
	  : get_edit_win_columns_for_text());			// wrap line by editor window columns
}

//-----------------------------------------------------------------------------

#define FORWARD_COL_IDX()										\
	if (*ptr == '\t') {											\
		col_idx += (priv_tab_size - col_idx % priv_tab_size);	\
		ptr++;													\
	} else if (*ptr == '\n') {									\
		col_idx++;												\
		ptr++;													\
	} else if (is_ctrl_char((unsigned char)*ptr)) {				\
		col_idx += 2;											\
		ptr++;													\
	} else if ((unsigned char)*ptr < 0x80) {					\
		col_idx++;												\
		ptr++;													\
	} else {													\
		col_idx += utf8c_columns(ptr);							\
		ptr += utf8c_bytes(ptr);								\
	}
#define FORWARD_VIS_IDX()										\
	if (*ptr == '\t') {											\
		vis_idx += (priv_tab_size - col_idx % priv_tab_size);	\
		col_idx += (priv_tab_size - col_idx % priv_tab_size);	\
		ptr++;													\
	} else if (*ptr == '\n') {									\
		vis_idx++;												\
		col_idx++;												\
		ptr++;													\
	} else if (is_ctrl_char((unsigned char)*ptr)) {				\
		vis_idx += 2;											\
		col_idx += 2;											\
		ptr++;													\
	} else if ((unsigned char)*ptr < 0x80) {					\
		vis_idx++;												\
		col_idx++;												\
		ptr++;													\
	} else {													\
		vis_idx += utf8c_bytes(ptr);							\
		col_idx += utf8c_columns(ptr);							\
		ptr += utf8c_bytes(ptr);								\
	}

// return byte_idx not exceed bytes
int byte_idx_from_byte_idx(const char *utf8s, int bytes)
{
	const char *ptr;
	const char *prev_ptr;

	for (prev_ptr = ptr = utf8s; *ptr && ptr - utf8s < bytes; ) {
		prev_ptr = ptr;
		ptr += utf8c_bytes(ptr);
	}
	if (ptr - utf8s > bytes)
		ptr = prev_ptr;
	return ptr - utf8s;
}

int byte_idx_from_col_idx(const char *utf8s, int columns, char_left_right_t left_right,
 int *col_idx__)
{
	int col_idx;
	int prev_col_idx;
	const char *ptr;
	const char *prev_ptr;

	prev_col_idx = columns;
	for (prev_ptr = ptr = utf8s, col_idx = 0; *ptr && col_idx < columns; ) {
		prev_col_idx = col_idx;
		prev_ptr = ptr;
		FORWARD_COL_IDX()
	}
///flf_d_printf("([%s], %d) ==> %d\n", utf8s, columns, ptr - utf8s);
	if (left_right == CHAR_LEFT && columns < col_idx) {
		col_idx = prev_col_idx;
		ptr = prev_ptr;
	}
	if (col_idx__)
		*col_idx__ = col_idx;
	return ptr - utf8s;
}

int col_idx_from_byte_idx(const char *utf8s, int col_idx, int bytes)
{
	const char *ptr;

	for (ptr = utf8s; *ptr && ptr - utf8s < bytes; ) {
		FORWARD_COL_IDX()
	}
	return col_idx;
}

int vis_idx_from_byte_idx(const char *utf8s, int bytes)
{
	int vis_idx;
	int col_idx;
	const char *ptr;

	for (ptr = utf8s, col_idx = 0, vis_idx = 0; *ptr && ptr - utf8s < bytes; ) {
		FORWARD_VIS_IDX()
	}
	return vis_idx;
}

#if 0
int char_idx_from_byte_idx(const char *utf8s, int bytes);
int char_idx_from_byte_idx(const char *utf8s, int bytes)
{
	int char_idx;
	int col_idx;
	const char *ptr;

	char_idx = 0;
	for (ptr = utf8s, col_idx = 0; *ptr && ptr - utf8s < bytes;
	 char_idx++) {
		FORWARD_COL_IDX()
	}
	return char_idx;
}
int vis_idx_from_col_idx(const char *utf8s, int columns);
int vis_idx_from_col_idx(const char *utf8s, int columns)
{
	int vis_idx;
	int col_idx;
	const char *ptr;

///_D_(dump_memory("vis_idx_from_col_idx\n", utf8s, strlen(utf8s)));
	for (ptr = utf8s, col_idx = 0, vis_idx = 0; *ptr && col_idx < columns; ) {
		FORWARD_VIS_IDX()
	}
///flf_d_printf("vis_idx_from_col_idx([%s], %d) ==> %d\n", utf8s, columns, vis_idx);
	return vis_idx;
}
#endif

//------------------------------------------------------------------------------
#ifdef START_UP_TEST
PRIVATE void test_get_intersection_(int min1, int max1, int min2, int max2);
void test_get_intersection(void)
{
_FLF_
	// aaaaaa bbbbbb
	test_get_intersection_(0, 54, 64, 65);
	// aaaaaa bbbbbb
	test_get_intersection_(10, 20, 30, 40);
	// bbbbbb aaaaaa
	test_get_intersection_(60, 100, 10, 50);

	// aaaaaabbbbbb
	test_get_intersection_(10, 20, 20, 40);
	// bbbbbbaaaaaa
	test_get_intersection_(60, 100, 10, 60);

	// aaaaaa
	//    bbbbbb
	test_get_intersection_(10, 100, 90, 110);
	//   aaaaaa
	// bbbbbb
	test_get_intersection_(50, 100, 10, 60);
	//   aaaaaa
	// bbbbbbbbbb
	test_get_intersection_(20, 100, 10, 110);
	// aaaaaaaaaa
	//   bbbbbb
	test_get_intersection_(10, 100, 50, 60);
}
// [not intersect]
//  <-------->
//             <-------->
// [intersect pattern 1]
//  <-------->
//      <-------->
// [intersect pattern 2]
//    <-------->
//  <------------>
PRIVATE void test_get_intersection_(int min1, int max1, int min2, int max2)
{
	int min;
	int max;

	get_intersection(min1, max1, min2, max2, &min, &max);
flf_d_printf("get_intersection(min1:%d, max1:%d, min2:%d, max2:%d)\n"
 " ==> %d, min:%d, max:%d\n",
 min1, max1, min2, max2, get_intersection(min1, max1, min2, max2, &min, &max), min, max);
}
#endif // START_UP_TEST
int get_intersection(int min1, int max1, int min2, int max2, int *min, int *max)
{
	int _min, _max;

	_min = MAX_(min1, min2);
	_max = MIN_(max1, max2);
	if (min)
		*min = _min;
	if (max)
		*max = _max;
	return _max - _min;		// >0: intersect, <=0: not intersect
}

// End of linewrap.c
/**************************************************************************
 *   main.c                                                               *
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

#include "headers.h"

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif // HAVE_GETOPT_H

#ifdef ENABLE_DEBUG
#warning "**** ENABLE_DEBUG defined (debug output is ENABLED) ****"
#endif // ENABLE_DEBUG

#ifdef ENABLE_NCURSES
#warning "Terminal control via ncurses (curses_...)"
#else // ENABLE_NCURSES
///#warning "Terminal control via own terminal interface library (termif_...)"
#endif // ENABLE_NCURSES

PRIVATE int init_locale(void);
PRIVATE int init_app_mode(void);
PRIVATE int parse_options(int argc, char *argv[]);
#ifdef START_UP_TEST
PRIVATE void start_up_test(void);
PRIVATE void start_up_test2(void);
#endif // START_UP_TEST
PRIVATE void exit_app(void);
PRIVATE int write_cur_dir_to_exit_file(void);
PRIVATE void die_save_file(const char *die_file_path);

char *main_rc_file_name = RC_FILE_NAME;	// standard rc file

int main(int argc, char *argv[])
{
#ifdef ENABLE_FILER
	filer_panes_t filer_panes;
#endif // ENABLE_FILER
	int start_line_num = 0;			// Line to start at

flf_d_printf("Start-------------------------------------------------------------\n");
	get_start_dir();
	signal_init();
_FLF_
	init_locale();
_FLF_
	init_app_mode();
_FLF_
	init_buffers();		// parse_options() needs c_e_b. So do here.
#ifdef ENABLE_FILER
	init_filer_panes(&filer_panes, get_start_dir());
	////set_cur_filer_view();
#endif // ENABLE_FILER
_FLF_
	parse_options(argc, argv);		// parse command line options
flf_d_printf("Starting %s ----------------\n", APP_NAME " " __DATE__ " " __TIME__);
	cache_users();
	cache_groups();
_FLF_
	init_default_app_color();
_FLF_
#ifdef START_UP_TEST
	start_up_test();
#endif // START_UP_TEST

#ifdef ENABLE_RC
	if (GET_APPMD(app_RCFILE)) {
		read_rc_file(main_rc_file_name);			// read standard rc file
	}
#endif // ENABLE_RC
#ifdef ENABLE_SYNTAX
	register_default_color_syntax();
///_D_(dump_file_types());
#endif // ENABLE_SYNTAX

	// setup terminal
flf_d_printf("initializing terminal\n");
	// initialize terminal interface(curses/termif)
	tio_init();
flf_d_printf("columns:%d lines:%d\n", tio_get_columns(), tio_get_lines());
flf_d_printf("setting up windows\n");
	win_init_win_size();
#ifdef SCRN_COLS_TWO_PANES
	if (COLS >= SCRN_COLS_TWO_PANES) {
		// set two pane
		SET_APPMD(fl_FILER_PANES);
	}
#endif // SCRN_COLS_TWO_PANES

#ifdef ENABLE_HISTORY
flf_d_printf("init_histories()\n");
	init_histories();
flf_d_printf("load_histories()\n");
	load_histories();
	load_last_searched_needle();
flf_d_printf("load_key_macro()\n");
	load_key_macro(1);
#endif // ENABLE_HISTORY

#ifdef START_UP_TEST
	start_up_test2();
#endif // START_UP_TEST

	set_die_on_callback(app_die_on);

flf_d_printf("opening files --------------------------------------------\n");
#ifdef ENABLE_HELP
	disp_splash(0);
	MSLEEP(500);
#endif // ENABLE_HELP
_FLF_
	// If there's a +LINE flag, it is the first non-option argument
	if (0 < optind && optind < argc && argv[optind][0] == '+') {
flf_d_printf("optind:%d: %s\n", optind, argv[optind]);
		sscanf(&argv[optind][1], "%d", &start_line_num);
		optind++;
	}
_FLF_
	// More than one file is specified on the command line,
	// load them all and switch to the first one afterward.
	if (optind < argc) {
		clear_files_loaded();
		for ( ; optind < argc; optind++) {
flf_d_printf("optind:%d: %s\n", optind, argv[optind]);
			// CURDIR: changed in editor
			if (load_file_name_upp_low(argv[optind], TUL0, OOE1, MOE0, RECURSIVE1) <= 0) {
				tio_beep();
			}
			tio_refresh();
		}
	}
_FLF_
	if (count_edit_bufs()) {
		do_switch_to_top_file();
		if (start_line_num > 0)
			goto_line_col_in_cur_buf(start_line_num, 1);
		disp_files_loaded();
	}

_FLF_
	app_main_loop();
_FLF_

	set_die_on_callback(NULL);
	exit_app();
	return 0;
}

int app_main_loop(void)
{
#ifdef ENABLE_FILER
	char file_name[MAX_PATH_LEN+1];

	if (count_edit_bufs()) {
		// application was started as a EDITOR
///_FLF_
		call_editor(0, 0);
///_FLF_
	} else {
		// application was started as a FILER
		while (1) {
///_FLF_
			call_filer(0, 0, "", "", file_name, MAX_PATH_LEN);
///_FLF_
			if (count_edit_bufs() == 0) {
				// no file loaded in filer
				break;
			}
			call_editor(0, 0);
		}
	}
#else // ENABLE_FILER
	if (count_edit_bufs() == 0) {
///_FLF_
		do_switch_to_file_list();
		do_open_file();
///_FLF_
	}
	if (count_edit_bufs()) {
		call_editor(0, 0);
	}
#endif // ENABLE_FILER
	return 0;
}

//-----------------------------------------------------------------------------

PRIVATE int init_locale(void)
{
	// setup system environment
	setlocale(LC_ALL, "");	// set locale so that wchar related functions work
#if defined(ENABLE_NLS) && defined(ENABLE_UTF8)
e_printf("LANG: [%s]\n", getenv__("LANG"));
//	setenv("LANG", "ja_JP.UTF-8", 1);
//e_printf("LANG: [%s]\n", getenv__("LANG"));
	setlocale(LC_ALL, getenv__("LANG"));
e_printf("cur locale: %s\n", setlocale(LC_ALL, NULL));
e_printf("PACKAGE: %s, LOCALEDIR: %s\n", PACKAGE, LOCALEDIR);
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
//e_printf("MB_LEN_MAX: %d\n", MB_LEN_MAX);
//e_printf("MB_CUR_MAX: %d\n", MB_CUR_MAX);
//e_printf("MAX_UTF8C_BYTES: %d\n", MAX_UTF8C_BYTES);
#endif // defined(ENABLE_NLS) && defined(ENABLE_UTF8)
	return 0;
}

PRIVATE int init_app_mode(void)
{
	// setup application settings
	memset(&app_mode__, 0x00, sizeof(app_mode__));

	// editor and filer
#ifdef ENABLE_RC
	SET_APPMD(app_RCFILE);
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	SET_APPMD(app_HISTORY);
#endif // ENABLE_HISTORY
	CLR_APPMD(app_DRAW_CURSOR);
	SET_APPMD_VAL(app_KEY_LINES, 3);
	SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_NONE);
	// editor mode
	CLR_APPMD(app_EDITOR_FILER);
	set_app_func_key_table();
	CLR_APPMD(app_LIST_MODE);
	SET_APPMD(app_UTF8);
///	CLR_APPMD(app_MAP_KEY_7F_BS);
	SET_APPMD(app_MAP_KEY_7F_BS);

	CLR_APPMD(ed_EDITOR_PANES);
	CLR_APPMD(ed_DUAL_SCROLL);
	SET_APPMD(ed_SHOW_RULER);
	SET_APPMD(ed_SHOW_LINE_NUMBER);
	CLR_APPMD(ed_CURS_POSITIONING);
#ifdef USE_NKF
	SET_APPMD(ed_USE_NKF);
#endif // USE_NKF
	CLR_APPMD(ed_AUTO_INDENT);
	SET_APPMD_VAL(ed_BACKUP_FILES, BACKUP_FILES_MAX);
#ifdef ENABLE_REGEX
	SET_APPMD(ed_USE_REGEXP);
#ifdef ENABLE_SYNTAX
	SET_APPMD(ed_SYNTAX_HIGHLIGHT);
	SET_APPMD(ed_TAB_EOL_NOTATION);
#endif // ENABLE_SYNTAX
#endif // ENABLE_REGEX

	// filer mode
	SET_APPMD_VAL(fl_SHOW_FILE_INFO, SHOW_FILE_INFO_5);
	SET_APPMD_VAL(fl_FILE_SORT_BY, 0);
	CLR_APPMD(fl_FILER_PANES);

	return 0;
}

const char short_options[] = "C:cht:rne:dv?k";
#ifdef HAVE_GETOPT_LONG
int option_index = 0;
const struct option long_options[] = {
#ifdef ENABLE_RC
	{ "rcfile",        required_argument, 0, 'C' },
	{ "norcfile",      no_argument,       0, 'c' },
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	{ "nohistory",     no_argument,       0, 'h' },
#endif // ENABLE_HISTORY
	{ "tabsize",       required_argument, 0, 't' },
	{ "view",          no_argument,       0, 'r' },
#ifdef USE_NKF
	{ "nonkf",         no_argument,       0, 'n' },
	{ "encoding",      required_argument, 0, 'e' },
#endif // USE_NKF
	{ "debug",         no_argument,       0, 'd' },
	{ "version",       no_argument,       0, 'v' },		// show version
	{ "help",          no_argument,       0, '?' },		// show help text
#ifndef ENABLE_NCURSES
	{ "keyseq",        no_argument,       0, 'k' },		// investigate key sequence
#endif // ENABLE_NCURSES
	{ 0,               no_argument,       0, 0 }
};
#endif // HAVE_GETOPT_LONG

PRIVATE int parse_options(int argc, char *argv[])
{
	int optchr;

#ifdef ENABLE_DEBUG
	for (optchr = 0; optchr < argc; optchr++) {
		flf_d_printf("optind:%d: %s\n", optchr, argv[optchr]);
	}
#endif // ENABLE_DEBUG
#ifdef HAVE_GETOPT_LONG
	while ((optchr = getopt_long(argc, argv,
	 short_options, long_options, &option_index)) != -1) { // }
#else
	while ((optchr = getopt(argc, argv,
	 short_options)) != -1) {
#endif
flf_d_printf("optchr: %c\n", optchr);
if (optarg) {
flf_d_printf("*optarg: %c\n", *optarg);
}
		switch (optchr) {
#ifdef ENABLE_RC
		case 'C':
			main_rc_file_name = optarg;
flf_d_printf("main_rc_file_name: [%s]\n", main_rc_file_name);
			break;
		case 'c':
			CLR_APPMD(app_RCFILE);
			break;
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
		case 'h':
			CLR_APPMD(app_HISTORY);
			break;
#endif // ENABLE_HISTORY
		case 't':
			if (optarg) {
				int tab_size = 0;
				sscanf(optarg, "%d", &tab_size);
				if ((tab_size < 1) || (MAX_TAB_SIZE < tab_size)) {
flf_d_printf("Illegal tab size: [%d]\n", tab_size);
					show_usage();
				}
				SET_CUR_EBUF_STATE(buf_TAB_SIZE, tab_size);
			}
			break;
		case 'r':
			SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);
			break;
#ifdef USE_NKF
		case 'n':
			CLR_APPMD(ed_USE_NKF);
			break;
#endif // USE_NKF
#ifdef USE_NKF
		case 'e':
			if (optarg) {
				switch (optarg[0]) {
				case 'j':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_JIS);
					break;
				case 'e':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_EUCJP);
					break;
				case 's':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_SJIS);
					break;
				case 'u':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8);
					break;
				case 'a':
				default:
					break;
				}
			}
			break;
#endif // USE_NKF
		case 'd':
			SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_PRINTF);
#ifdef ENABLE_DEBUG
			set_debug_printf_output(1);
#endif // ENABLE_DEBUG
			break;
		case 'v':
			show_version();
			exit(0);
		case '?':
			show_usage();
			exit(0);
#ifndef ENABLE_NCURSES
		case 'k':
			investigate_key_sequence();
			exit(0);
#endif // ENABLE_NCURSES
		default:
			fprintf(stderr, "Unknown option: [%c]\n", optchr);
			break;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
#ifdef START_UP_TEST
PRIVATE void start_up_test(void)
{
	char buf[MAX_PATH_LEN+1];
	void *allocated;

///	tio_test();
	get_cwd(buf);
	flf_d_printf("getcwd: [%s]\n", buf);
	getenv_pwd(buf);
	flf_d_printf("getenv(PWD): [%s]\n", buf);
	flf_d_printf("getenv(USER): [%s]\n", getenv__("USER"));
	flf_d_printf("getenv(HOSTNAME): [%s]\n", getenv__("HOSTNAME"));
	flf_d_printf("getenv(LANG): [%s]\n", getenv__("LANG"));
	flf_d_printf("ttyname(0): [%s]\n", ttyname(0));

	flf_d_printf("sizeof(int): %d\n", sizeof(int));
	flf_d_printf("sizeof(long): %d\n", sizeof(long));
	flf_d_printf("sizeof(int *): %d\n", sizeof(int *));
	flf_d_printf("sizeof(app_mode_t): %d\n", sizeof(app_mode_t));
	flf_d_printf("sizeof(buf_state_t): %d\n", sizeof(buf_state_t));
	flf_d_printf("MAX_UTF8C_BYTES: %d\n", MAX_UTF8C_BYTES);
	flf_d_printf("PATH_MAX: %d\n", PATH_MAX);
	flf_d_printf("MAX_PATH_LEN: %d\n", MAX_PATH_LEN);
	flf_d_printf("MAX_SCRN_LINE_BUF_LEN: %d\n", MAX_SCRN_LINE_BUF_LEN);
	flf_d_printf("MAX_EDIT_LINE_LEN: %d\n", MAX_EDIT_LINE_LEN);

	// memory address of various object
	flf_d_printf("mem type: 0x1234567890123456\n");
	flf_d_printf("auto buf: %p\n", buf);
	flf_d_printf("\"string\": %p\n", "string");
	allocated = malloc(100);
	flf_d_printf("malloc  : %p\n", allocated);
	free(allocated);

	flf_d_printf("#define KEY_RESIZE	0x%04x\n", KEY_RESIZE);
	flf_d_printf("#define KEY_HOME		0x%04x\n", KEY_HOME);
	flf_d_printf("#define KEY_END		0x%04x\n", KEY_END);
	flf_d_printf("#define KEY_UP		0x%04x\n", KEY_UP);
	flf_d_printf("#define KEY_DOWN		0x%04x\n", KEY_DOWN);
	flf_d_printf("#define KEY_LEFT		0x%04x\n", KEY_LEFT);
	flf_d_printf("#define KEY_RIGHT		0x%04x\n", KEY_RIGHT);
	flf_d_printf("#define KEY_PPAGE		0x%04x\n", KEY_PPAGE);
	flf_d_printf("#define KEY_NPAGE		0x%04x\n", KEY_NPAGE);
	flf_d_printf("#define KEY_IC		0x%04x\n", KEY_IC);
	flf_d_printf("#define KEY_DC		0x%04x\n", KEY_DC);
	flf_d_printf("#define KEY_BACKSPACE	0x%04x\n", KEY_BACKSPACE);
	flf_d_printf("#define KEY_ENTER		0x%04x\n", KEY_ENTER);
	flf_d_printf("#define KEY_F(0)		0x%04x\n", KEY_F(0));

///	test_wrap_line();
	test_normalize_full_path();
	test_get_full_path();
	test_realpath();
	test_get_file_name_extension();

///	test_get_intersection();
	get_mem_free_in_kb(1);
///	test_nn_from_num();
///	test_utf8c_encode();
	test_utf8c_bytes();
///	test_wcwidth();
#ifdef ENABLE_REGEX
	test_regexp();
#endif // ENABLE_REGEX
///
	test_make_ruler_text();
#ifdef ENABLE_FILER
	test_get_file_size_str();
#endif // ENABLE_FILER
	test_replace_str();

	test_get_n_th_file_name();
}
PRIVATE void start_up_test2(void)
{
flf_d_printf("modulo test --------------\n");
	for (int nn = -9; nn < 10; nn++) {
flf_d_printf("%2d mod 7 = %2d, ", nn, nn % 7);
flf_d_printf("%2d mod 5 = %2d, ", nn, nn % 5);
flf_d_printf("%2d mod 3 = %2d\n", nn, nn % 3);
	}
}
#endif // START_UP_TEST
//-----------------------------------------------------------------------------

// exit application
PRIVATE void exit_app(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	tio_clear_screen();
	tio_destroy();

	write_cur_dir_to_exit_file();

	free_all_allocated_memory();

	signal_clear();

flf_d_printf("Exit %s --------------------\n", APP_NAME " " __DATE__ " " __TIME__);
	printf("\n");
	exit(0);
}

PRIVATE int write_cur_dir_to_exit_file(void)
{
	char buffer[MAX_PATH_LEN+1];
	FILE *fp;

	// write current directory to the $HOME/EXIT_FILE_NAME
	snprintf(buffer, MAX_PATH_LEN+1, "%s/%s", get_home_dir(), EXIT_FILE_NAME);
	if ((fp = fopen(buffer, "w")) != NULL) {
		fputs(get_cur_dir(buffer), fp);
		if (fclose(fp) != 0) {
			return -1;
		}
		return 0;
	}
	return -1;
}

//-----------------------------------------------------------------------------

// Die (gracefully?)
void app_die_on(const char *msg)
{
	be_buf_t *buf;

	tio_destroy();

	e_printf("%s", msg);

	// then save all of the modified buffers, if any
	for (buf = EDIT_BUFS_TOP_NODE; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		set_cep_buf(buf);
		if (check_cur_buf_modified()) {
			// save the file if it's been modified
			die_save_file(get_cep_buf()->abs_path);
		}
	}

	signal_clear();
	exit(255);
}

// save modified files to save-file before dying
PRIVATE void die_save_file(const char *die_file_path)
{
	char file_path[MAX_PATH_LEN+5+1];
	int ret = 0;

	// If we can't save, we have REAL bad problems, but we might as well TRY.
	if (die_file_path[0] == '\0') {
		e_printf(_("\nNo file written\n"));
		return;
	} else {
		strlcpy__(file_path, die_file_path, MAX_PATH_LEN);
		strlcat__(file_path, MAX_PATH_LEN, "#");
	}
	if (is_strlen_not_0(file_path)) {
		ret = backup_and_save_cur_buf(file_path);
		if (ret == 0) {
			e_printf(_("\nBuffer written to %s\n"), file_path);
			return;
		}
	}
}

//-----------------------------------------------------------------------------

void free_all_allocated_memory(void)
{
#ifdef ENABLE_HISTORY
///_FLF_
///	save_key_macro();
#endif // ENABLE_HISTORY
	free_all_buffers();

#ifdef ENABLE_SYNTAX
///_FLF_
	free_file_types();
#endif // ENABLE_SYNTAX
///_FLF_
}

//-----------------------------------------------------------------------------

PRIVATE void show_one_option(const char *shortflag, const char *longflag,
 const char *desc);
void show_usage(void)
{
	printf(_("\nUsage: " BIN_NAME " [+LINE] [option] [file(s)]\n"));
	//               12345678901234567890 12345678901234567890 12345678901234567890
	show_one_option(_("Option"),         _("Long option"),    _("Meaning"));
#ifdef ENABLE_RC
	show_one_option("-C RCFILE",         "--rcfile=RCFILE",   _("Read RCFILE"));
	show_one_option("-c",                "--norcfile",        _("Don't look at berc files"));
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	show_one_option("-h",                "--nohistory",       _("Don't use history file"));
#endif // ENABLE_HISTORY
	show_one_option("-t NUM",            "--tabsize=NUM",     _("Set tab-size to NUM[1-15]"));
	show_one_option("-r",                "--view",            _("View mode(Read only mode)"));
#ifdef USE_NKF
	show_one_option("-n",                "--nonkf",           _("Don't use nkf"));
	show_one_option("-e a",              "--encoding=a",      _("ASCII character encoding"));
	show_one_option("-e j",              "--encoding=j",      _("JIS character encoding"));
	show_one_option("-e e",              "--encoding=e",      _("EUCJP character encoding"));
	show_one_option("-e s",              "--encoding=s",      _("SJIS character encoding"));
	show_one_option("-e u",              "--encoding=u",      _("UTF8 character encoding"));
#endif // USE_NKF
#ifdef ENABLE_DEBUG
	show_one_option("-d",                "--debug",           _("Output debug log to stderr"));
#endif // ENABLE_DEBUG
	show_one_option("-v",                "--version",         _("Show version information"));
	show_one_option("-?",                "--help",            _("Show this message"));
	show_one_option("+NUM",              "",                  _("Start at line number NUM"));
#ifndef ENABLE_NCURSES
	show_one_option("-k",                "--keyseq",          _("Investigate key codes"));
#endif // ENABLE_NCURSES
}

// Print one option explanation to the screen
PRIVATE void show_one_option(const char *shortflag, const char *longflag,
 const char *desc)
{
	printf(" %-20s", shortflag);
#ifdef HAVE_GETOPT_LONG
	printf(" %-20s", longflag);
#endif // HAVE_GETOPT_LONG
	printf(" %s\n", desc);
}

void show_version(void)
{
	printf(_("%s editor version %s (compiled at %s, %s)\n"),
	 APP_NAME, VERSION, __TIME__, __DATE__);
	printf(_(" Build Options:\n"));
#ifdef ENABLE_FILER
	printf("   --enable-filer\n");
#else
	printf("   --disable-filer\n");
#endif
#ifdef ENABLE_NLS
	printf("   --enable-nls\n");
#else
	printf("   --disable-nls\n");
#endif
#ifdef ENABLE_RC
	printf("   --enable-rc\n");
#else
	printf("   --disable-rc\n");
#endif
#ifdef ENABLE_UTF8
	printf("   --enable-utf8\n");
#else
	printf("   --disable-utf8\n");
#endif
#if defined(USE_NCURSES) || defined(USE_NCURSESW)
	printf("   --enable-ncurses\n");
#else
	printf("   --disable-ncurses\n");
#endif
#ifdef USE_NKF
	printf("   --enable-nkf\n");
#else
	printf("   --disable-nkf\n");
#endif
#ifdef ENABLE_HISTORY
	printf("   --enable-history\n");
#else
	printf("   --disable-history\n");
#endif
#ifdef ENABLE_REGEX
	printf("   --enable-regex\n");
#else
	printf("   --disable-regex\n");
#endif
#ifdef ENABLE_SYNTAX
	printf("   --enable-syntax\n");
#else
	printf("   --disable-syntax\n");
#endif
#ifdef USE_PCRE
	printf("   --enable-pcre\n");
#else
	printf("   --disable-pcre\n");
#endif
#ifdef ENABLE_UNDO
	printf("   --enable-undo\n");
#else
	printf("   --disable-undo\n");
#endif
#ifdef USE_BUSYBOX
	printf("   --enable-busybox\n");
#else
	printf("   --disable-busybox\n");
#endif
#ifdef ENABLE_HELP
	printf("   --enable-help\n");
#else // ENABLE_HELP
	printf("   --disable-help\n");
#endif // ENABLE_HELP
#ifdef ENABLE_DEBUG
	printf("   --enable-debug\n");
#else
	printf("   --disable-debug\n");
#endif
	printf("\n");
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_HELP
const char *splash_text_b[] = {
//012345678901234567890123456789
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
 "b                            b",
 "b                            b",
 "b                            b",
 "b                            b",
 "b                            b",
 "b                            b",
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
 "",
};
const char *splash_text_s[] = {
//0123456789012345678901234567890123456789012345678
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
 "b    FFs   FFs     FFs                          d",
 "b    FFs   FFs     FFs    BBBBBBBs   BBBBBBBs   d",
 "b  FFs FFs FFs FFs FFs    BBs   BBs  BBs        d",
 "bFFs   FFs FFs FFs FFFFFFsBBBBBBBs   BBBBBBs    d",
 "b    FFFFFFFFFFFFsFFsFFs  BBs   BBs  BBs        d",
 "b  FFs           FFs FFs  BBBBBBBs   BBBBBBBs   d",
 "bFFFFs FFFFFFFFFFFFs FFs                        d",
 "b  FFs   FFs FFs   FFs    EEEEs  EsEs           d",
 "b  FFs   FFs FFs   FFs    Es     Es   Es        d",
 "b  FFs   FFs FFs FFs FFs  EEEs EEEsEsEEEsEEEsEEsd",
 "b  FFs FFs     FFs   FFs  Es   EsEsEs Es EsEsEs d",
 "b  FFs       FFs       FFsEEEEsEEEsEs Es EEEsEs d",
 "bdddddddddddddddddddddddddddddddddddddddddddddddd",
 "",
};
const char *splash_text_m[] = {
//0123456789012345678901234567890123456789012345678901234567
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
 "b      FFs    FFFs       FFs                            d",
 "b    FFFFs    FFs        FFFs                           d",
 "b  FFFs   FFs FFs FFFs  FFs       BBBBBBBs  BBBBBBBs    d",
 "bFFFs     FFs FFs FFs  FFs        BBs   BBs BBs         d",
 "b      FFsFFs FFs FFs FFFFFFFFFFFsBBBBBBBs  BBBBBBs     d",
 "b     FFFsFFFFFFFFFFsFFs     FFs  BBs   BBs BBs         d",
 "b    FFFs               FFs  FFs  BBBBBBBs  BBBBBBBs    d",
 "b  FFFFFsFFFFFFFFFFFFs   FFsFFs                         d",
 "bFFF  FFs   FFs FFs       FFFFs   EEEEs  EsEs           d",
 "b     FFs   FFs FFs        FFs    Es     Es   Es        d",
 "b     FFs   FFs FFs FFs   FFFs    EEEs EEEsEsEEEsEEEsEEsd",
 "b     FFs  FFs  FFFFFFs  FFsFFs   Es   EsEsEs Es EsEsEs d",
 "b     FFs FFs   FFFs   FFs   FFFs EEEEsEEEsEs Es EEEsEs d",
 "b     FFsFFs         FFs      FFFFs                     d",
 "bdddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
 "",
};
const char *splash_text_l[] = {
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
 "b         FFFs      FFFFFs      FFFFs                                          d",
 "b       FFFFFFs     FFFFs       FFFFFs                                         d",
 "b      FFFFFs  FFFs FFFFs FFFFs FFFFs                                          d",
 "b     FFFFs    FFFs FFFFs FFFs  FFFFs            BBBBBBs   BBBBBBBs            d",
 "b   FFFFs      FFFs FFFFs FFFs FFFFs             BBs   BBs BBs                 d",
 "b FFFFs    FFFsFFFs FFFFs FFFs FFFs        FFFs  BBs   BBs BBs                 d",
 "b         FFFFsFFFs FFFFs FFFsFFFFFFFFFFFFFFFFFFsBBBBBBs   BBBBBs              d",
 "b        FFFFFsFFFFFFFFFFFFFFFFFs       FFFFs    BBs   BBs BBs                 d",
 "b       FFFFs  FFs        FFFFFFs       FFFFs    BBs   BBs BBs                 d",
 "b      FFFFs               FFFsFFs      FFFs     BBBBBBs   BBBBBBBs            d",
 "b     FFFFFFs             FFs   FFs    FFFs                                    d",
 "b   FFFFFFFFsFFFFFFFFFFFFFFFFFs  FFs  FFFFs                                    d",
 "b FFFFs FFFFs   FFFFs FFFFs       FFFFFFFs       EEEEEEs    Es                 d",
 "b       FFFFs   FFFFs FFFFs        FFFFFFs       Es         EsEs Es            d",
 "b       FFFFs   FFFFs FFFFs         FFFFs        Es         Es   Es            d",
 "b       FFFFs   FFFFs FFFFs FFs    FFFFFFFs      EEEEEs  EEEEsEsEEEs EEEs EsEs d",
 "b       FFFFs  FFFFs  FFFFFFFs    FFFFFFFFFFs    Es     Es  EsEs Es Es  EsEEs  d",
 "b       FFFFs  FFFs FFFFFFFs    FFFFs   FFFFFFs  Es     Es  EsEs Es Es  EsEs   d",
 "b       FFFFs FFs     FFFs    FFFFs     FFFFFFFFsEEEEEEs EEEEsEs  Es EEEs Es   d",
 "b       FFFFsFFs            FFFFs        FFFFFs                                d",
 "b       FFFFs           FFFFFFs                                                d",
 "bddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
 "",
};

void disp_splash(int delay)
{
	int splash_size_idx;
	const char **splash_text;
	int top_y;
	int left_x;
	int msg1_x, msg2_x, msg3_x, msg4_x, msg5_x;
	int msg1_y, msg2_y, msg3_y, msg4_y, msg5_y;
	int cols;
	int lines;
	int yy;
	int xx;
	char buffer[100+1];

	tio_set_cursor_on(0);
	if (delay >= 0) {
		// refresh whole screen
		tio_clear_flash_screen(delay);
	}
	for (splash_size_idx = 3; splash_size_idx >= 0; splash_size_idx--) {
		switch (splash_size_idx) {
		case 3:
			splash_text = splash_text_l;
			msg1_y = 1;
			msg2_y = 2;
			msg3_y = 3;
			msg4_y = 12;
			msg5_y = 21;
			msg1_x = 49;
			msg2_x = 49;
			msg3_x = 49;
			msg4_x = 49;
			msg5_x = 49;
			break;
		case 2:
			splash_text = splash_text_m;
			msg1_y = 1;
			msg2_y = 1;
			msg3_y = 2;
			msg4_y = 8;
			msg5_y = 14;
			msg1_x = 32;
			msg2_x = 42;
			msg3_x = 34;
			msg4_x = 34;
			msg5_x = 28;
			break;
		case 1:
			splash_text = splash_text_s;
			msg1_y = 0;
			msg2_y = 0;
			msg3_y = 1;
			msg4_y = 7;
			msg5_y = 13;
			msg1_x = 24;
			msg2_x = 34;
			msg3_x = 26;
			msg4_x = 26;
			msg5_x = 20;
			break;
		default:
		case 0:
			splash_text = splash_text_b;
			msg1_y = 1;
			msg2_y = 2;
			msg3_y = 3;
			msg4_y = 4;
			msg5_y = 6;
			msg1_x = 3;
			msg2_x = 3;
			msg3_x = 3;
			msg4_x = 3;
			msg5_x = 1;
			break;
		}
		cols = strlen(splash_text[0]);
		for (lines = 0; *splash_text[lines]; lines++) {
			// nothing to do
		}
		if (cols <= tio_get_columns() - 2
		 && lines <= tio_get_lines()) {
			break;	// splash frame is smaller than window
		}
	}
	top_y = (tio_get_lines() - lines) / 2;
	left_x = (tio_get_columns() - cols) / 2;
	for (yy = 0; yy < lines; yy++) {
		for (xx = 0; xx < cols; xx++) {
			switch(splash_text[yy][xx]) {
			default:	tio_set_attrs(CL_CY, CL_BK, 0);		break;
			case 'F':	tio_set_attrs(CL_BK, CL_GR, 1);		break;
			case 'B':	tio_set_attrs(CL_BK, CL_RD, 1);		break;
			case 'E':	tio_set_attrs(CL_BK, CL_MG, 1);		break;
			case 'b':	tio_set_attrs(CL_BK, CL_RD, 1);		break;
			case 'd':	tio_set_attrs(CL_BK, CL_RD, 1);		break;
			case 's':	tio_set_attrs(CL_BL, CL_BK, 0);		break;
			}
			tio_output_string(top_y + yy, left_x + xx, " ", -1);
		}
	}
	tio_set_attrs(CL_CY, CL_BK, 0);
	tio_output_string(top_y + msg1_y, left_x + msg1_x, "BE-editor", -1);
	tio_output_string(top_y + msg2_y, left_x + msg2_x, "Version " VERSION, -1);
	tio_output_string(top_y + msg3_y, left_x + msg3_x, __DATE__ " " __TIME__, -1);
	snprintf(buffer, 100+1, _("Screen size: %dx%d"), tio_get_columns(), tio_get_lines());
	if (msg2_x >= 0) {
	tio_output_string(top_y + msg4_y, left_x + msg4_x, buffer, -1);
	}
	if (msg3_x >= 0) {
	tio_output_string(top_y + msg5_y, left_x + msg5_x, _("We're invincible with this!!"), -1);
	}
	tio_refresh();
}
#endif // ENABLE_HELP

// End of main.c
/*****************************************************************************
	myutf8.c
 *****************************************************************************/

#include "utilincs.h"

#ifdef ENABLE_UTF8
#define WIDE_CHARS		(20 + 12 + 14 + 1)
const char *my_wide_utf8c[WIDE_CHARS] = {
 "①", "②", "③", "④", "⑤", "⑥", "⑦", "⑧", "⑨", "⑩",
 "⑪", "⑫", "⑬", "⑭", "⑮", "⑯", "⑰", "⑱", "⑲", "⑳",
 "■", "□", "◆", "◇", "○", "◎", "●", "★", "☆", "☐", "☑", "☒",
 "", "", "", "", "", "", "", "", "", "", "", "", "", "",
 "" };
PRIVATE wchar_t my_wide_wchar[WIDE_CHARS] = { 0 };
#endif // ENABLE_UTF8

PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len);

#ifdef ENABLE_UTF8
#ifdef ENABLE_DEBUG
void test_my_mbwidth(void)
{
	int chr_idx;

	for (chr_idx = 0; *my_wide_utf8c[chr_idx]; chr_idx++) {
		flf_d_printf("[%s]: %d\n", my_wide_utf8c[chr_idx],
		 my_mbwidth(my_wide_utf8c[chr_idx], MAX_UTF8C_BYTES));
		flf_d_printf("[%s]: %04x\n", my_wide_utf8c[chr_idx],
		 my_mbtowc(my_wide_utf8c[chr_idx], MAX_UTF8C_BYTES));
	}
}
#endif // ENABLE_DEBUG
PRIVATE int make_my_wcwidth_table(void)
{
	int chr_idx;

	if (my_wide_wchar[0])
		return 0;	// already made
	for (chr_idx = 0; *my_wide_utf8c[chr_idx]; chr_idx++) {
		my_wide_wchar[chr_idx] = my_mbtowc(my_wide_utf8c[chr_idx], MAX_UTF8C_BYTES);
	}
	return 1;
}
#endif // ENABLE_UTF8
int my_mbwidth(const char *utf8c, int max_len)
{
#ifndef VAGUE_WIDE_CHR
	return my_wcwidth(my_mbtowc(utf8c, max_len));
#else // VAGUE_WIDE_CHR
	wchar_t wc = my_mbtowc(utf8c, max_len);
	return is_vague_wide_chr(wc) ? 2 : my_wcwidth(wc);
#endif // VAGUE_WIDE_CHR
}
int my_wcwidth(wchar_t wc)
{
#ifndef ENABLE_UTF8
	return 1;
#else // ENABLE_UTF8
	int chr_idx;
	int columns;

	make_my_wcwidth_table();
	for (chr_idx = 0; *my_wide_utf8c[chr_idx]; chr_idx++) {
		if (my_wide_wchar[chr_idx] == wc)
			return 2;
	}
	columns = wcwidth(wc);
	if (columns < 1)
		columns = 1;		// narrow char.
	return columns;
#endif // ENABLE_UTF8
}

#ifdef VAGUE_WIDE_CHR
int is_vague_wide_chr(wchar_t wc)
{
	return 0
	// WIDE_CHAR_LIST
	 || (0x2000 <= wc && wc < 0x33e0)
	 || (0xa000 <= wc && wc < 0xac00)
	 || (0xe000 <= wc && wc < 0xf900)
	;
}
#endif // VAGUE_WIDE_CHR

// UTF8 character byte length
int my_mblen(const char *utf8c, int max_len)
{
	wchar_t wc;

	return my_mbtowc__(&wc, utf8c, max_len);
}
int my_mbtowc(const char *utf8c, int max_len)
{
	wchar_t wc;

	my_mbtowc__(&wc, utf8c, max_len);
	return wc;
}
#define IS_UTF8_2ND_BYTE(byte)		(((byte) & 0xc0) == 0x80)	// 10xxxxxx
PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len)
{
	wchar_t wc = 0;
	int len = 1;
	int idx;

	if (max_len < 1) {
		*pwc = wc;
		return 0;
	}
	wc = utf8c[0];
	if ((utf8c[0] & 0x80) == 0x00) {	// 0xxxxxxx
		wc = utf8c[0] & 0x7f;			// 0xxxxxxx
		len = 1;
	} else
	if ((utf8c[0] & 0xe0) == 0xc0) {	// 110xxxxx-10xxxxxx
		wc = utf8c[0] & 0x1f;			// 110xxxxx
		len = 2;
	} else
	if ((utf8c[0] & 0xf0) == 0xe0) {	// 1110xxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x0f;			// 1110xxxx
		len = 3;
	} else
	if ((utf8c[0] & 0xf8) == 0xf0) {	// 11110xxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x07;			// 11110xxx
		len = 4;
	} else
	if ((utf8c[0] & 0xfc) == 0xf8) {	// 111110xx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x03;			// 111110xx
		len = 5;
	} else
	if ((utf8c[0] & 0xfe) == 0xfc) {	// 1111110x-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x01;			// 1111110x
		len = 6;
	}
	for (idx = 1; idx < max_len; idx++) {
		if (idx >= len)
			break;
		if (IS_UTF8_2ND_BYTE(utf8c[idx]) == 0)
			break;		// UTF8 sequence trancated !!
		wc = (wc << 6) | (utf8c[idx] & 0x3f);
	}
	len = idx;
	*pwc = wc;
	return len;		// return length
}

// End of myutf8.c
/**************************************************************************
 *   rcfile.c                                                             *
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

#include "headers.h"

#ifdef ENABLE_RC

PRIVATE int read_rc_path(const char *rc_file_path, int complain);
PRIVATE int read_rc_path__(const char *rc_file_path, int complain);

#ifdef ENABLE_SYNTAX
PRIVATE int add_file_type(const char *file_type_name, const char *regexp_file_name);
PRIVATE int add_color_syntax(const char *regexp_start, const char *regexp_end, int bgc, int fgc);
PRIVATE int link_color_syntax_w_file_type(file_type_t *file_type, color_syntax_t *clr_syntax);
#endif // ENABLE_SYNTAX

PRIVATE void rcfile_error(const char *msg, ...);

const char *rc_file_path_reading = NULL;
#define MAX_RC_NEST_COUNT		3
int rc_file_nest_count = 0;

// The main rc file function, tries to open the rc file
// config file priority:
//		'pwd'/.berc
//		$HOME/.berc
//		SYSCONFDIR/berc
int read_rc_file(const char *rc_file_name)
{
	char rc_file_path[MAX_PATH_LEN+1];

	// read ./.berc
	if (get_start_dir()) {
		snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/.%s", get_start_dir(), rc_file_name);
		if (read_rc_path(rc_file_path, 0) == 0)
			return 0;
	}
	// read $HOME/.berc
	snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/.%s", get_home_dir(), rc_file_name);
	if (read_rc_path(rc_file_path, 0) == 0)
		return 0;

////#define SYSCONFDIR		"/usr/local/etc"
#ifdef SYSCONFDIR
	// read /usr/local/etc/berc
	snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/%s", SYSCONFDIR, rc_file_name);
	/* Try to open system wide rc file */
	if (read_rc_path(rc_file_path, 0) == 0)
		return 0;
#endif // SYSCONFDIR
	return -1;
}

PRIVATE int parse_number(void);
PRIVATE int parse_color_default(void);
PRIVATE int parse_color_title(void);
PRIVATE int parse_color_status(void);
PRIVATE int parse_color_key_list(void);
PRIVATE int parse_color_key_list2(void);
PRIVATE int parse_color_text_normal(void);
PRIVATE int parse_color_text_selected(void);
PRIVATE int parse_color_text_selected2(void);
PRIVATE int parse_color_warning(void);
PRIVATE int parse_color_cursor(void);
PRIVATE int parse_color_cursor_line(void);
PRIVATE int parse_color_line_number(void);
PRIVATE int parse_color_menu_frame(void);
PRIVATE int parse_color_menu_item(void);
PRIVATE int parse_color_menu_selected(void);
PRIVATE int parse_color_input(void);
#ifdef ENABLE_SYNTAX
PRIVATE int parse_file_type(void);
PRIVATE int parse_color_syntax(void);
PRIVATE int parse_tab_size(void);
#endif // ENABLE_SYNTAX
PRIVATE int parse_include(void);

PRIVATE int parse_bgc_fgc_register(int color_idx);
PRIVATE int parse_bgc_fgc(int *bgc, int *fgc);
PRIVATE int parse_key_names(key_code_t *keys, int max_keys_bind);
PRIVATE int parse_word(char *buffer, int buf_len);
#ifdef ENABLE_SYNTAX
PRIVATE int parse_string(char *buffer, int buf_len);
#endif // ENABLE_SYNTAX

PRIVATE int parse_color_name(int *bright, int *color);
PRIVATE int parse_keyword(const char **rc_line_ptr, const char *keyword);

// commands which take number parameter
// Ex. "backup_files 3"
struct /*_rc_cmd_idx_*/ {
	char *command;
	mode_idx_t mode_idx;
} rc_cmd_idx[] = {
 { "draw_cursor",		APMD_DRAW_CURSOR,		},
 { "key_list_lines",	APMD_KEY_LINES,			},
 { "map_key_7f_bs",		APMD_MAP_KEY_7F_BS,		},
#ifdef ENABLE_UTF8
 { "utf8",				APMD_UTF8,				},
#endif // ENABLE_UTF8

 { "editor_panes",		EDMD_EDITOR_PANES,		},
 { "dual_scroll",		EDMD_DUAL_SCROLL,		},
 { "continuous_view",	EDMD_CONT_VIEW,			},
 { "show_ruler",		EDMD_SHOW_RULER,		},
 { "show_line_num",		EDMD_SHOW_LINE_NUMBER,	},
 { "cursor_center",		EDMD_CURS_POSITIONING,	},
 { "auto_indent",		EDMD_AUTO_INDENT,		},
 { "backup_files",		EDMD_BACKUP_FILES,		},
 { "ignore_case",		EDMD_IGNORE_CASE,		},
#ifdef ENABLE_REGEX
 { "regexp",			EDMD_USE_REGEXP,		},
#endif // ENABLE_REGEX
#ifdef ENABLE_SYNTAX
 { "syntax_hl",			EDMD_SYNTAX_HIGHLIGHT,	},
 { "tab_eol_notation",	EDMD_TAB_EOL_NOTATION,	},
#endif // ENABLE_SYNTAX

 { "show_dot_file",		FLMD_SHOW_DOT_FILE,		},
 { "show_file_info",	FLMD_SHOW_FILE_INFO,	},
 { "sort_by",			FLMD_FILE_SORT_BY,		},
 { "filer_panes",		FLMD_FILER_PANES,		},
};

// commands which take various type of parameter
// Ex. "color_default -white +black"
struct /*_rc_cmd_func_*/ {
	char *command;
	int (*func)();
} rc_cmd_func[] = {
	{ "color_default",			parse_color_default,		},
	{ "color_title",			parse_color_title,			},
	{ "color_status",			parse_color_status,			},
	{ "color_key_list",			parse_color_key_list,		},
	{ "color_key_list2",		parse_color_key_list2,		},
	{ "color_text_normal",		parse_color_text_normal,	},
	{ "color_text_selected",	parse_color_text_selected,	},
	{ "color_text_selected2",	parse_color_text_selected2,	},
	{ "color_warning",			parse_color_warning,		},
	{ "color_cursor",			parse_color_cursor,			},
	{ "color_cursor_line",		parse_color_cursor_line,	},
	{ "color_line_number",		parse_color_line_number,	},
	{ "color_menu_frame",		parse_color_menu_frame,		},
	{ "color_menu_item",		parse_color_menu_item,		},
	{ "color_menu_selected",	parse_color_menu_selected,	},
	{ "color_input",			parse_color_input,			},
#ifdef ENABLE_SYNTAX
	{ "file_type",				parse_file_type,			},
	{ "color_syntax",			parse_color_syntax,			},
	{ "tab_size",				parse_tab_size,				},
#endif // ENABLE_SYNTAX
	{ "include",				parse_include,				},
};

// Parse the RC file
#define IS_CHAR_FOR_ID(chr)			(isalnum(chr) || strchr__("_-", chr) != NULL)
PRIVATE const char *rc_line_ptr;
PRIVATE int rc_line_num = 0;

PRIVATE int read_rc_path(const char *rc_file_path, int complain)
{
	const char *rc_file_path_save;
	int ret;

	rc_file_path_save = rc_file_path_reading;
	rc_file_path_reading = rc_file_path;
	ret = read_rc_path__(rc_file_path, complain);
	rc_file_path_reading = rc_file_path_save;
	return ret;
}
PRIVATE int read_rc_path__(const char *rc_file_path, int complain)
{
	FILE *fp;
	char rc_line_buf[MAX_SCRN_LINE_BUF_LEN+1];
	const char *begin;
	char cmd[MAX_SCRN_LINE_BUF_LEN+1];
	int idx;
	int val;
	func_key_table_t *func_key_table;
#define MAX_KEYS_BOUND	3
	key_code_t keys[MAX_KEYS_BOUND];

///#ifdef ENABLE_SYNTAX
///	free_file_types();
///#endif // ENABLE_SYNTAX

///
flf_d_printf("Attempting to read rc file %s ...\n", rc_file_path);
	rc_line_num = 0;
	if ((fp = fopen(rc_file_path, "r")) == NULL) {
///flf_d_printf("No rc file %s\n", rc_file_path);
		/* complain about the file not existing */
		if (complain) {
			rcfile_error(_("Unable to open config file: %s, %s\n"),
			 rc_file_path, strerror(errno));
		}
		return 1;
	}
///flf_d_printf("Reading rc file %s ...\n", rc_file_path);
	while (fgets(rc_line_buf, MAX_SCRN_LINE_BUF_LEN, fp) != 0) {
		remove_line_tail_lf(rc_line_buf);
///flf_d_printf("parsing_line: %s\n", rc_line_buf);
		rc_line_ptr = rc_line_buf;
		rc_line_num++;
		if (SKIP_SPACE(rc_line_ptr))
			continue;
		if (*rc_line_ptr == '#') {
			continue;		// Skip comment line
		}
		begin = rc_line_ptr;
		for ( ; *rc_line_ptr; rc_line_ptr++) {
			if (IS_CHAR_FOR_ID(*rc_line_ptr) == 0)
				break;
		}
		strlcpy__(cmd, begin, rc_line_ptr - begin);
///flf_d_printf("rc_command: %s\n", cmd);
		if (strlen(cmd)) {
			for (idx = 0; idx < ARRAY_SIZE_OF(rc_cmd_idx); idx++) {
				if (strcmp(rc_cmd_idx[idx].command, cmd) == 0) {
					val = parse_number();
					if (val >= 0) {
						set_mode_idx_val(rc_cmd_idx[idx].mode_idx, val);
					}
					goto read_rc_path_match;
				}
			}
			for (idx = 0; idx < ARRAY_SIZE_OF(rc_cmd_func); idx++) {
				if (strcmp(rc_cmd_func[idx].command, cmd) == 0) {
					(*rc_cmd_func[idx].func)();
					goto read_rc_path_match;
				}
			}
			if ((func_key_table = get_func_table_from_func_id(cmd)) != NULL) {
///
flf_d_printf("parsing: %s\n", rc_line_ptr);
				parse_key_names(keys, MAX_KEYS_BOUND);
				if (keys[0] >= 0) {
///
flf_d_printf("clear_keys_bound_to_func(%s)\n", func_key_table->func_id);
					clear_keys_if_bound(keys);
					clear_keys_bound_to_func(func_key_table);
					bind_key_to_func(func_key_table, keys);
					goto read_rc_path_match;
				}
			}
read_rc_path_match:;
		}
	}
	if (fclose(fp) != 0) {
		return 1;
	}
	return 0;
}

#ifdef ENABLE_DEBUG
void dump_app_mode(void)
{
	int idx;

	for (idx = 0; idx < ARRAY_SIZE_OF(rc_cmd_idx); idx++) {
		flf_d_printf("%s = %s\n",
		 rc_cmd_idx[idx].command, get_str_mode_idx_val(rc_cmd_idx[idx].mode_idx));
	}
}
#endif // ENABLE_DEBUG

PRIVATE int parse_number(void)
{
	int val;

	if (sscanf(rc_line_ptr, "%d", &val) == 0) {
		rcfile_error(_("Not valid number: %s\n"), rc_line_ptr);
		return -1;
	}
	return val;
}

PRIVATE int parse_color_default(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_DEFAULT);
}
PRIVATE int parse_color_title(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TITLE);
}
PRIVATE int parse_color_status(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_STATUS);
}
PRIVATE int parse_color_key_list(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_KEY_LIST);
}
PRIVATE int parse_color_key_list2(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_KEY_LIST2);
}
PRIVATE int parse_color_text_normal(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TEXT_NORMAL);
}
PRIVATE int parse_color_text_selected(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TEXT_SELECTED);
}
PRIVATE int parse_color_text_selected2(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TEXT_SELECTED2);
}
PRIVATE int parse_color_warning(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_WARNING);
}
PRIVATE int parse_color_cursor(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_CURSOR_CHAR);
}
PRIVATE int parse_color_cursor_line(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_CURSOR_LINE);
}
PRIVATE int parse_color_line_number(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_LINE_NUMBER);
}
PRIVATE int parse_color_menu_frame(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_MENU_FRAME);
}
PRIVATE int parse_color_menu_item(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_MENU_ITEM);
}
PRIVATE int parse_color_menu_selected(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_MENU_SELECTED);
}
PRIVATE int parse_color_input(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_INPUT);
}

PRIVATE int fgc, bgc;

#ifdef ENABLE_SYNTAX
PRIVATE int parse_file_type(void)
{
#define MAX_FILE_TYPE_NAME	40
#define MAX_REGEXP_LEN		256
	char file_type_name[MAX_FILE_TYPE_NAME+1] = "";
	char file_name_regexp[MAX_REGEXP_LEN+1] = "";

	parse_string(file_type_name, MAX_FILE_TYPE_NAME);
	parse_string(file_name_regexp, MAX_REGEXP_LEN);
	return add_file_type(file_type_name, file_name_regexp);
}

// Parse the color stuff into the item_colors array
PRIVATE int parse_color_syntax(void)
{
	char regexp_start[MAX_REGEXP_LEN+1] = "";
	char regexp_end[MAX_REGEXP_LEN+1] = "";

	while (*rc_line_ptr) {
		if (SKIP_SPACE(rc_line_ptr))
			break;
///flf_d_printf("parsing: %s", rc_line_ptr);
		switch(*rc_line_ptr) {
		case '<':
			rc_line_ptr++;
			if (parse_string(regexp_start, MAX_REGEXP_LEN))
				return 1;
			break;
		case '>':
			rc_line_ptr++;
			if (parse_string(regexp_end, MAX_REGEXP_LEN))
				return 1;
			break;
		case '=':
			rc_line_ptr++;
			if (parse_string(regexp_start, MAX_REGEXP_LEN))
				return 1;
			break;
		case '+':
		case '-':
		case ':':
			parse_bgc_fgc(&bgc, &fgc);
			break;
		default:
			rcfile_error(_("color %s not understood.\n"), rc_line_ptr);
			return 1;
		}
	}
	return add_color_syntax(regexp_start, regexp_end, bgc, fgc);
}
PRIVATE int parse_tab_size(void)
{
	int tab_size;

	SKIP_SPACE(rc_line_ptr);
	tab_size = DEFAULT_TAB_SIZE;
	if (sscanf(rc_line_ptr, "%d", &tab_size) == 1) {
		tab_size = tab_size ? tab_size : DEFAULT_TAB_SIZE;
	}
	if (cur_file_type) {
		cur_file_type->tab_size = tab_size;
	}
	return 0;
}
#endif // ENABLE_SYNTAX

// "include .emacs.berc"
PRIVATE int parse_include(void)
{
	char dir_part[MAX_PATH_LEN+1];
	char file_part[MAX_PATH_LEN+1];
	char rc_file_path[MAX_PATH_LEN+1];
	int ret = 0;

	SKIP_SPACE(rc_line_ptr);
	if (rc_file_nest_count < MAX_RC_NEST_COUNT) {
		++rc_file_nest_count;
		separate_dir_part_and_file_part(rc_file_path_reading, dir_part, file_part);
		snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/%s", dir_part, rc_line_ptr);
		ret = read_rc_path(rc_file_path, 1);
		--rc_file_nest_count;
	}
	return ret;
}

PRIVATE int parse_bgc_fgc_register(int color_idx)
{
	int ret;

	ret = parse_bgc_fgc(&bgc, &fgc);
	register_item_color(color_idx, bgc, fgc);
	return ret;
}
PRIVATE int parse_bgc_fgc(int *bgc, int *fgc)
{
	int bright;

	*fgc = *bgc = -1;
	while (*rc_line_ptr) {
		if (SKIP_SPACE(rc_line_ptr))
			break;
///flf_d_printf("parsing: %s", rc_line_ptr);
		switch(*rc_line_ptr) {
		case '-':
			rc_line_ptr++;
			parse_color_name(&bright, bgc);
			break;
		case '+':
			rc_line_ptr++;
			parse_color_name(&bright, fgc);
			break;
		default:
			rcfile_error(_("color %s not understood.\n"), rc_line_ptr);
			return 1;
		}
	}
	return 0;
}

// "^Q UP @q"
PRIVATE int parse_key_names(key_code_t *keys, int max_keys_bind)
{
	int key_idx;
	char key_name[MAX_KEY_NAME_LEN+1];
	short key;

	// clear all keys
	for (key_idx = 0; key_idx < max_keys_bind; key_idx++) {
		keys[key_idx] = -1;
	}
	for (key_idx = 0; key_idx < max_keys_bind; ) {
		if (SKIP_SPACE(rc_line_ptr))
			break;
///
flf_d_printf("parsing: %s\n", rc_line_ptr);
		if (parse_word(key_name, MAX_KEY_NAME_LEN)) {
			return 1;
		}
		if ((key = key_code_from_short_key_name(key_name)) < 0) {
			// unknown key name
		} else {
			if (key_idx >= max_keys_bind) {
				return 1;
			}
			keys[key_idx] = key;
			key_idx++;
		}
	}
	return 0;
}

PRIVATE int parse_word(char *buffer, int buf_len)
{
	const char *begin;

	if (SKIP_SPACE(rc_line_ptr))
		return 1;
	for (begin = rc_line_ptr; *rc_line_ptr; rc_line_ptr++) {
		if (IS_SPACE(rc_line_ptr) || IS_EOL(rc_line_ptr))
			break;
	}
	if (rc_line_ptr - begin > buf_len)
		return 1;
	strlcpy__(buffer, begin, rc_line_ptr - begin);
	return 0;
}

#ifdef ENABLE_SYNTAX
PRIVATE int parse_string(char *buffer, int buf_len)
{
	const char *begin;

	if (SKIP_SPACE(rc_line_ptr))
		return 1;
	if (*rc_line_ptr != '"')
		return 1;
	rc_line_ptr++;
	begin = rc_line_ptr;
	while (*rc_line_ptr) {
		if (*rc_line_ptr == '"' || IS_EOL(rc_line_ptr))
			break;
		if (*rc_line_ptr == '\\' && !IS_EOL(rc_line_ptr+1))
			rc_line_ptr++;
		rc_line_ptr++;
	}
	if (IS_EOL(rc_line_ptr))
		return 1;
	if (rc_line_ptr - begin > buf_len)
		return 1;
	strlcpy__(buffer, begin, rc_line_ptr - begin);
	rc_line_ptr++;
	return 0;
}
#endif // ENABLE_SYNTAX

PRIVATE struct /*_color_index_*/ {
	char *color_name;
	int color_index;
} color_names[] = {
	{ "black",		CL_BK,	},	//  0
	{ "dgray",		CL_DGY,	},	//  8
	{ "red",		CL_RD,	},	//  1
	{ "lred",		CL_LRD,	},	//  9
	{ "green",		CL_GR,	},	//  2
	{ "lgreen",		CL_LGR,	},	// 10
	{ "brown",		CL_BR,	},	//  3
	{ "yellow",		CL_YL,	},	// 11
	{ "blue",		CL_BL,	},	//  4
	{ "lblue",		CL_LBL,	},	// 12
	{ "magenta",	CL_MG,	},	//  5
	{ "lmagenta",	CL_LMG,	},	// 13
	{ "cyan",		CL_CY,	},	//  6
	{ "lcyan",		CL_LCY,	},	// 14
	{ "gray",		CL_GY,	},	//  7
	{ "white",		CL_WH,	},	// 15
	{ "none",		-1,	},
};

PRIVATE int parse_color_name(int *bright, int *color)
{
	int index;

///flf_d_printf("colorname: %s\n", colorname);
	*bright = 0;
	*color = -1;
	if (parse_keyword(&rc_line_ptr, "bright") == 0) {
		*bright = 1;
	}
	if (parse_keyword(&rc_line_ptr, "dark") == 0) {
		*bright = 0;
	}
	for (index = 0; index < ARRAY_SIZE_OF(color_names); index++) {
		if (parse_keyword(&rc_line_ptr, color_names[index].color_name) == 0) {
			*color = color_names[index].color_index;
			break;
		}
	}
	if (index >= ARRAY_SIZE_OF(color_names)) {
		rcfile_error(_("color name %s not understood.\n"), rc_line_ptr);
		return 1;
	}
	return 0;
}

PRIVATE int parse_keyword(const char **rc_line_ptr, const char *keyword)
{
	if (strlcasecmp(*rc_line_ptr, keyword) == 0) {
		*rc_line_ptr += strlen(keyword);
		return 0;
	}
	return -1;
}

#ifdef ENABLE_SYNTAX

int register_default_color_syntax(void)
{
#define CL_TAB_EOL	CL_GR	// color for TAB/EOL/"　" notation

	add_file_type("*", ".*");
	cur_file_type->tab_size = DEFAULT_TAB_SIZE;	// set default tab size
#if defined(ENABLE_UTF8)
	add_color_syntax(UTF8_ZEN_SPACE, "", -1, CL_TAB_EOL);	// Full width space
#endif // ENABLE_UTF8
	add_color_syntax("[\\t]", "", -1, CL_TAB_EOL);		// TAB
	add_color_syntax("[\\n]", "", -1, CL_TAB_EOL);		// EOL(End of line)
	default_color_syntax = cur_file_type;	// memorize this color syntax
	return 0;
}

PRIVATE int add_file_type(const char *file_type_name, const char *regexp_file_name)
{
	file_type_t **file_type_ptr;
	file_type_t *file_type = NULL;

///flf_d_printf("adding file type: [%s] [%s]\n", file_type_name, regexp_file_name);
	file_type = (file_type_t *)malloc__(sizeof(file_type_t));
	file_type->regexp = regexp_alloc();
	file_type->next = NULL;
	if (regexp_compile(file_type->regexp, regexp_file_name, 0)) {
		rcfile_error(_("Bad regexp: [%s]:%s\n"),
		 regexp_file_name, file_type->regexp->regex_err_msg);
		goto add_file_type_err;
	}
	file_type->desc = malloc_strcpy(file_type_name);
	file_type->color_syntax = NULL;
	file_type->tab_size = DEFAULT_TAB_SIZE;
	for (file_type_ptr = &file_types_head; *file_type_ptr != NULL; ) {
		file_type_ptr = &((*file_type_ptr)->next);
	}
	cur_file_type = *file_type_ptr = file_type;
	return 0;

add_file_type_err:;
////_FLF_
	regexp_free(file_type->regexp);
	return 1;
}

PRIVATE int add_color_syntax(const char *regexp_start, const char *regexp_end, int bgc, int fgc)
{
	color_syntax_t *clr_syntax;

////flf_d_printf("adding color syntax: [%s] [%s] %d %d %d %d\n",
//// regexp_start, regexp_end, fgc, bgc, reverse, bold);
	if (strlen(regexp_start) == 0)
		return 1;

	clr_syntax = (color_syntax_t *)malloc__(sizeof(color_syntax_t));
	clr_syntax->next = NULL;
	clr_syntax->regexp_end = NULL;

	clr_syntax->regexp_start = regexp_alloc();
	if (regexp_compile(clr_syntax->regexp_start, regexp_start, 0)) {
		rcfile_error(_("Bad regexp: [%s]:%s\n"),
		 regexp_start, clr_syntax->regexp_start->regex_err_msg);
		goto add_color_syntax_err1;
	}

	if (strlen(regexp_end)) {
		clr_syntax->regexp_end = regexp_alloc();
		if (regexp_compile(clr_syntax->regexp_end, regexp_end, 0)) {
			rcfile_error(_("Bad regexp: [%s]:%s\n"),
			 regexp_end, clr_syntax->regexp_end->regex_err_msg);
			goto add_color_syntax_err2;
		}
	}
	clr_syntax->color.bgc = bgc;
	clr_syntax->color.fgc = fgc;
///	clr_syntax->color.reverse = reverse;

	link_color_syntax_w_file_type(cur_file_type, clr_syntax);
	return 0;

add_color_syntax_err2:;
	regexp_free(clr_syntax->regexp_end);
add_color_syntax_err1:;
	regexp_free(clr_syntax->regexp_start);
	return 1;
}

PRIVATE int link_color_syntax_w_file_type(file_type_t *file_type, color_syntax_t *clr_syntax)
{
	color_syntax_t **color_syntax_ptr_ptr = NULL;

	for (color_syntax_ptr_ptr = &file_type->color_syntax;
	 *color_syntax_ptr_ptr != NULL; ) {
///_D_(dump_color_syntax(*color_syntax_ptr_ptr));
		color_syntax_ptr_ptr = &(*color_syntax_ptr_ptr)->next;
	}
	*color_syntax_ptr_ptr = clr_syntax;
	return 0;
}

void free_file_types(void)
{
	file_type_t *file_type, *f_next;
	color_syntax_t *clr_syntax, *c_next;

	for (file_type = file_types_head; file_type != NULL; ) {
		FREE_CLR_PTR(file_type->desc);
		regexp_free(file_type->regexp);
		for (clr_syntax = file_type->color_syntax; clr_syntax != NULL; ) {
			if (clr_syntax->regexp_start) {
				regexp_free(clr_syntax->regexp_start);
			}
			if (clr_syntax->regexp_end) {
				regexp_free(clr_syntax->regexp_end);
			}
			c_next = clr_syntax->next;
			FREE_CLR_PTR(clr_syntax);
			clr_syntax = c_next;
		}
		f_next = file_type->next;
		FREE_CLR_PTR(file_type);
		file_type = f_next;
	}
	file_types_head = NULL;
	cur_file_type = NULL;
}

//------------------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_file_types(void)
{
	file_type_t *file_type;

flf_d_printf("{{\n");
	for (file_type = file_types_head; file_type != NULL; file_type = file_type->next) {
		dump_file_type(file_type, 0);
	}
flf_d_printf("--------------------------------------------------\n");
	for (file_type = file_types_head; file_type != NULL; file_type = file_type->next) {
		dump_file_type(file_type, 1);
	}
flf_d_printf("}}\n");
}
void dump_file_type(const file_type_t *file_type, int syntax)
{
	color_syntax_t *clr_syntax;

////_FLF_
	if (file_type == NULL) {
		flf_d_printf("file_type: NULL\n");
	}
	flf_d_printf("%sfile_type:[%s][%s] tab_size:%d\n",
	 file_type == cur_file_type ? ">" : " ",
	 file_type->desc, file_type->regexp->needle_compiled, file_type->tab_size);
	if (syntax) {
		for (clr_syntax = file_type->color_syntax; clr_syntax != NULL;
		 clr_syntax = clr_syntax->next) {
			dump_color_syntax(clr_syntax);
		}
	}
}
void dump_color_syntax(const color_syntax_t *clr_syntax)
{
_FLF_
	flf_d_printf("%p: [%s]-[%s] %d/%d ->%p\n",
	 clr_syntax,
	 clr_syntax->regexp_start->needle_compiled,
	 clr_syntax->regexp_end ? clr_syntax->regexp_end->needle_compiled : "",
	 clr_syntax->color.bgc, clr_syntax->color.fgc,
	 clr_syntax->next);
_FLF_
}
#endif // ENABLE_DEBUG

#endif // ENABLE_SYNTAX

PRIVATE void rcfile_error(const char *msg, ...)
{
	va_list ap;

	e_printf("\n");
	if (rc_line_num > 0) {
		e_printf(_("Error in file: %s on line: %d"), rc_file_path_reading, rc_line_num);
	}
	va_start(ap, msg);
	e_vprintf(msg, ap);
	va_end(ap);
}

#endif // ENABLE_RC

// End of rcfile.c
/**************************************************************************
 *   regexp.c                                                             *
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

#include "headers.h"

void search_clear(search_t *search)
{
	strcpy__(search->needle, "");
}
int search_is_needle_set(search_t *search)
{
	if ((strlen(search->needle) == 4) && (search->needle[0] == '[') && (search->needle[3] == ']')) {
		return 2;	// Bracket search "[()]"
	}
	if (strlen(search->needle)) {
		return 1;		// Not bracket search (ordinaly search)
	}
	return 0;			// needle not set
}

int search_str_in_line(search_t *search, matches_t *matches,
 const char *needle, int search_dir, int ignore_case, const char *haystack, int byte_idx)
{
	int match_len;

	if (needle) {
		strlcpy__(search->needle, needle, MAX_PATH_LEN);
		search->ignore_case = ignore_case;
	}
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP) == 0) {
#endif // ENABLE_REGEX
		match_len = search_str_in_line_strstr(search->needle, matches,
		 search_dir, search->ignore_case, haystack, byte_idx);
#ifdef ENABLE_REGEX
	} else {
		match_len = search_str_in_line_regexp(&search->regexp, &matches->regexp_matches,
		 needle, search_dir, ignore_case, haystack, byte_idx);
		matches_set_start_idx(matches,
		 regexp_matches_start_idx(&matches->regexp_matches, 0));
		matches_set_end_idx(matches,
		 regexp_matches_end_idx(&matches->regexp_matches, 0));
	}
#endif // ENABLE_REGEX
////flf_d_printf("haystack: [%s], needle: [%s], dir: %d ==> match_len: %d\n", &haystack[byte_idx], needle, search_dir, match_len);
	return match_len;	// >0: match
}

//------------------------------------------------------------------------------

void matches_clear(matches_t *matches)
{
	matches->so = 0;
	matches->eo = 0;
#ifdef ENABLE_REGEX
	regexp_matches_clear(&matches->regexp_matches);
#endif // ENABLE_REGEX
}
void matches_set_start_idx(matches_t *matches, int byte_idx)
{
	matches->so = byte_idx;
#ifdef ENABLE_REGEX
	regexp_matches_set_start_idx(&matches->regexp_matches, 0, byte_idx);
#endif // ENABLE_REGEX
}
void matches_set_end_idx(matches_t *matches, int byte_idx)
{
	matches->eo = byte_idx;
#ifdef ENABLE_REGEX
	regexp_matches_set_end_idx(&matches->regexp_matches, 0, byte_idx);
#endif // ENABLE_REGEX
}
void matches_set_match_len(matches_t *matches, int match_len)
{
	matches->eo = matches->so + match_len;
#ifdef ENABLE_REGEX
	regexp_matches_set_match_len(&matches->regexp_matches, 0, match_len);
#endif // ENABLE_REGEX
}
int matches_start_idx(matches_t *matches)
{
	return matches->so;
}
int matches_end_idx(matches_t *matches)
{
	return matches->eo;
}
int matches_match_len(matches_t *matches)
{
	return matches->eo - matches->so;
}

void matches_dump_matches(matches_t *matches)
{
	flf_d_printf("so: %d\n", matches_start_idx(matches));
	flf_d_printf("eo: %d\n", matches_end_idx(matches));
	flf_d_printf("match_len: %d\n", matches_match_len(matches));
}

//------------------------------------------------------------------------------

#ifdef ENABLE_REGEX
int search_str_in_line_regexp(regexp_t *regexp, regexp_matches_t *regexp_matches,
 const char *needle, int search_dir, int ignore_case, const char *haystack, int byte_idx)
{
	int cflags = ignore_case ? REG_ICASE : 0;
	int ret;
	int prev_match_byte_idx;

	if (search_dir < 0) {
		// backward search. The last match is what we need.
		// First, search from the line head
		ret = regexp_search(regexp, regexp_matches, needle,
		 haystack, 0, cflags, REG_NONE, 1);
		if (ret == 0 && regexp_matches_start_idx(regexp_matches, 0) <= byte_idx) {
			// Search forward until there is no more match.
			while (1) {
				prev_match_byte_idx = regexp_matches_start_idx(regexp_matches, 0);
				// Second, search from the next byte of previous match pos
				ret = regexp_search(regexp, regexp_matches, needle,
				 haystack, regexp_matches_start_idx(regexp_matches, 0) + 1, cflags, REG_NONE, 1);
				if (ret == 0 && regexp_matches_start_idx(regexp_matches, 0) <= byte_idx) {
					// continue;
				} else {
					break;
				}
			}
			// Finally, put the subexpression matches in regexp_matches.
			// The REG_NOTBOL flag doesn't matter now.
			ret = regexp_search(regexp, regexp_matches, needle,
			 haystack, prev_match_byte_idx, cflags, REG_NONE, MAX_REGEX_MATCHES);
			if (ret == 0 && regexp_matches_start_idx(regexp_matches, 0) <= byte_idx) {
				return regexp_matches_match_len(regexp_matches, 0);
			}
		}
	} else {
		// forward search. The first match is what we need.
		// if byte_idx == 0, haystack is line top
		// if byte_idx > 0, haystack is not line top and add REG_NOTBOL eflag
		if (regexp_search(regexp, regexp_matches, needle,
		 haystack, byte_idx, cflags, REG_NONE, MAX_REGEX_MATCHES) == 0) {
			// Put the subexpression matches in regexp_matches.
			return regexp_matches_match_len(regexp_matches, 0);
		}
	}
	return 0;		// not found
}
#endif // ENABLE_REGEX

//-----------------------------------------------------------------------------

PRIVATE const char *revstrstr(const char *haystack, const char *tail_of_haystack,
 const char *needle);
PRIVATE const char *revstristr(const char *haystack, const char *tail_of_haystack,
 const char *needle);
PRIVATE const char *stristr(const char *haystack, const char *needle);

int search_str_in_line_strstr(const char *needle, matches_t *matches,
 int search_dir, int ignore_case, const char *haystack, int byte_idx)
{
	const char *match_start;

	matches_clear(matches);
	if (search_dir < 0) {
		if (ignore_case == 0)
			match_start = revstrstr(haystack, &haystack[byte_idx], needle);
		else
			match_start = revstristr(haystack, &haystack[byte_idx], needle);
	} else {
		if (ignore_case == 0)
			match_start = strstr(&haystack[byte_idx], needle);
		else
			match_start = stristr(&haystack[byte_idx], needle);
	}
	if (match_start) {
		matches_set_start_idx(matches, match_start - haystack);
		matches_set_match_len(matches, strlen(needle));
	}
	return matches_match_len(matches);
}
PRIVATE const char *revstrstr(const char *haystack, const char *tail_of_haystack,
 const char *needle)
{
	const char *hh, *nn;

	for ( ; tail_of_haystack >= haystack; tail_of_haystack--) {
		for (hh = tail_of_haystack, nn = needle;
		 *nn && (*hh == *nn);
		 hh++, nn++) {
			// nothing to do
		}
		if (*nn == '\0')
			return tail_of_haystack;
	}
	return NULL;
}
PRIVATE const char *revstristr(const char *haystack, const char *tail_of_haystack,
 const char *needle)
{
	const char *hh, *nn = needle;

	for ( ; tail_of_haystack >= haystack; tail_of_haystack--) {
		for (hh = tail_of_haystack, nn = needle;
		 *nn && (tolower(*hh) == tolower(*nn));
		 hh++, nn++) {
			// nothing to do
		}
		if (*nn == '\0')
			return tail_of_haystack;
	}
	return NULL;
}
// This is now mutt's version (called mutt_stristr) because it doesn't
// use memory allocation to do a simple search (yuck).
PRIVATE const char *stristr(const char *haystack, const char *needle)
{
	const char *hh, *nn;

	if (haystack == NULL)
		return NULL;
	if (needle == NULL)
		return haystack;
	
	for ( ; *haystack; haystack++) {
		for (hh = haystack, nn = needle;
		 *hh && *nn && (tolower(*hh) == tolower(*nn));
		 hh++, nn++) {
			// nothing to do
		}
		if (*nn == 0)
			return haystack;
	}
	return NULL;
}

#ifdef ENABLE_REGEX

// Regular expression helper functions ----------------------------------------

#ifdef START_UP_TEST
void test_regexp(void)
{
	regexp_t regexp;
	regexp_matches_t regexp_matches;
	const char *needle;
	const char *replace_to;
	const char *haystack;
	char output[MAX_EDIT_LINE_LEN+1];
	int ret;

	flf_d_printf("---------------------------------------------------------\n");
#ifdef USE_PCRE
	flf_d_printf("regexp library: PCRE\n");
#else // USE_PCRE
	flf_d_printf("regexp library: glibc POSIX ERE(Extended Regular Expression)\n");
#endif // USE_PCRE

	regexp_init(&regexp);

	// test tab, EOL match
	needle = "\\t\\n";
	haystack = "aaa\t\nbbbtnccc";
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);

	needle = "\\t";
	haystack = "aaa\t\nbbbtnccc";
	////regexp_init(&regexp);
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);

	needle = "\\n";
	haystack = "aaa\t\nbbbtnccc";
	////regexp_init(&regexp);
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);

	// test replacement in sub-expression
	////regexp_init(&regexp);
// REGEX: "abc(def|ghi)(jkl|mno)pqr" REPLACE-TO: "123\\2\\1456"
// INPUT-TEXT: "abcghijklpqr" OUTPUT-TEXT: "123jklghi456"
	needle = "abc(def|ghi)(jkl|mno)pqr";
	haystack = "xxxxxabcghijklpqryyyyy";
	strlcpy__(output, haystack, MAX_EDIT_LINE_LEN);
	replace_to = "123\\2\\1456";
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	regexp_replace(&regexp, &regexp_matches, output, MAX_EDIT_LINE_LEN, replace_to);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);
	flf_d_printf("needle: [%s]\n", needle);
	flf_d_printf("replace_to: [%s]\n", replace_to);
	flf_d_printf("before: [%s]\n", haystack);
	flf_d_printf("after : [%s]\n", output);
	flf_d_printf("---------------------------------------------------------\n");
}
#endif // START_UP_TEST

regexp_t *regexp_alloc(void)
{
	regexp_t *regexp;

	regexp = (regexp_t*)malloc(sizeof(regexp_t));
	return regexp_init(regexp);
}
regexp_t *regexp_init(regexp_t *regexp)
{
	memset(regexp, 0x00, sizeof(regexp_t));
///	regexp->is_compiled = 0;
///	strcpy__(regexp->needle_compiled, "");
///	regexp->cflags_compiled = 0;
	return regexp;
}
int regexp_free_regex_compiled(regexp_t *regexp)
{
	if (regexp->is_compiled == 0)
		return 0;
	regexp->is_compiled = 0;
	return regfree__(&regexp->regex_compiled);
}
int regexp_free(regexp_t *regexp)
{
	regexp_free_regex_compiled(regexp);
	free(regexp);
	return 1;
}
int regexp_search(regexp_t *regexp, regexp_matches_t *regexp_matches, const char *needle,
 const char *haystack, int byte_idx, int cflags, int eflags, size_t max_matches)
{
	int ret;

///flf_d_printf("[%s]\n", needle);
	regexp_matches_clear(regexp_matches);
	if (needle) {
		// auto compile regexp
		if (regexp_compile(regexp, needle, cflags))
			return -2;		// -2: compilation error
	}
	ret = regexp_search_compiled(regexp, haystack, byte_idx,
	 eflags, regexp_matches, max_matches);
	return ret;		// 0: match, -1:no match
}
int regexp_compile(regexp_t *regexp, const char *needle, int cflags)
{
////flf_d_printf("[%s]\n", needle);
	if (regexp->is_compiled) {
		if (strcmp(needle, regexp->needle_compiled) != 0
		 || cflags != regexp->cflags_compiled) {
			regexp_free_regex_compiled(regexp);
		}
	}
	if (regexp->is_compiled == 0) {
		strlcpy__(regexp->needle_compiled, needle, MAX_PATH_LEN);
		regexp->cflags_compiled = cflags;
		if ((regexp->error = regcomp__(&regexp->regex_compiled, needle, cflags)) != 0) {
			regerror(regexp->error, &regexp->regex_compiled,
			 regexp->regex_err_msg, MAX_PATH_LEN);
			regexp->is_compiled = 1;	// compilation error
		} else {
			regexp->is_compiled = 2;	// compiled
		}
	}
	return regexp->is_compiled == 2 ? 0 : -1;	// 0: compilation succeeded, -1: compilation error
}
int regexp_search_compiled(regexp_t *regexp, const char *haystack, int byte_idx,
 int eflags, regexp_matches_t *regexp_matches, size_t max_matches)
{
	int ret;

///_D_(dump_str_w_caret(haystack, byte_idx));
	if (regexp->is_compiled < 2) {
		return -1;	// compilation error
	}
////flf_d_printf("[%s]\n", regexp->needle_compiled);
	if ((ret = regexec_n(&regexp->regex_compiled, &haystack[byte_idx],
	 max_matches, regexp_matches->matches,
	 eflags | (byte_idx == 0 ? REG_NONE : REG_NOTBOL))) == 0) {
		regexp_matches_set_start_idx(regexp_matches, 0,
		 byte_idx + regexp_matches_start_idx(regexp_matches, 0));
		regexp_matches_set_end_idx(regexp_matches, 0,
		 byte_idx + regexp_matches_end_idx(regexp_matches, 0));
///_D_(regexp_dump_matches(regexp, regexp_matches, haystack));
	}
	return ret;		// 0: match
}
#ifdef ENABLE_DEBUG
void regexp_dump_matches(regexp_t *regexp, regexp_matches_t *regexp_matches,
 const char *haystack)
{
	int match_idx;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	flf_d_printf("needle[%s]\n", regexp->needle_compiled);
	flf_d_printf("haystack[%s]\n", haystack);
	if (regexp_matches_match_len(regexp_matches, 0) == 0) {
		flf_d_printf("match_len: %d\n", regexp_matches_match_len(regexp_matches, 0));
		return;
	}
	for (match_idx = 0; match_idx <= regexp->regex_compiled.re_nsub; match_idx++) {
		if (regexp_matches_match_len(regexp_matches, match_idx) > 0) {
			flf_d_printf("regexp_matches[%d]=%d,%d[%s]\n", match_idx,
			 regexp_matches_start_idx(regexp_matches, match_idx),
			 regexp_matches_end_idx(regexp_matches, match_idx),
			 strcut__(buffer, MAX_SCRN_LINE_BUF_LEN, haystack,
			  regexp_matches_start_idx(regexp_matches, match_idx),
			  regexp_matches_end_idx(regexp_matches, match_idx)));
		}
	}
}
#endif // ENABLE_DEBUG
// replace including sub expressions
int regexp_replace(regexp_t *regexp, regexp_matches_t *regexp_matches,
 char *buffer, int buf_len, const char *replace_to)
{
	char output[MAX_EDIT_LINE_LEN+1];
	char *output_ptr;
	const char *replace_ptr;
	int subexp_num;		// sub-expression number(1--9)
	int match_len;

	// Iterate through the replacement text to handle subexpression replacement
	// using \1, \2, \3, etc.
	for (output_ptr = output, replace_ptr = replace_to;
	 *replace_ptr && output_ptr - output < buf_len; ) {
		if (*replace_ptr != '\\') {
			*output_ptr++ = *replace_ptr++;
		} else {
			subexp_num = replace_ptr[1] - '0';	// "\1" ==> 1
			if (1 <= subexp_num && subexp_num <= 9
			 && subexp_num <= regexp->regex_compiled.re_nsub) {
				replace_ptr += 2;	// Skip over the replacement expression (Ex. "\1")
				match_len = regexp_matches_match_len(regexp_matches, subexp_num);
				if (output_ptr - output + match_len < buf_len) {
					strlcpy__(output_ptr,
					 &buffer[regexp_matches_start_idx(regexp_matches, subexp_num)],
					 match_len);
					output_ptr += match_len;
				}
			} else {
				replace_ptr++;	// Skip over "\"
				*output_ptr++ = *replace_ptr++;
			}
		}
	}
	*output_ptr = '\0';
///flf_d_printf("before: [%s]\n", buffer);
///flf_d_printf("output: [%s]\n", output);
	replace_str(buffer, buf_len,
	 regexp_matches_start_idx(regexp_matches, 0),
	 regexp_matches_match_len(regexp_matches, 0),
	 output, -1);
///flf_d_printf("after : [%s]\n", buffer);
	return output_ptr - output;	// return length of change
}

//------------------------------------------------------------------------------

void regexp_matches_clear(regexp_matches_t *regexp_matches)
{
#if 0
	int match_idx;

	for (match_idx = 0; match_idx < MAX_REGEX_MATCHES; match_idx__) {
		regexp_matches->matches[match_idx].rm_so = 0;
		regexp_matches->matches[match_idx].rm_eo = 0;
	}
#else
	memset(regexp_matches, 0x00, sizeof(*regexp_matches));
#endif
}
void regexp_matches_set_start_idx(regexp_matches_t *regexp_matches, int match_idx,
 int byte_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	regexp_matches->matches[match_idx].rm_so = byte_idx;
}
void regexp_matches_set_end_idx(regexp_matches_t *regexp_matches, int match_idx,
 int byte_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	regexp_matches->matches[match_idx].rm_eo = byte_idx;
}
void regexp_matches_set_match_len(regexp_matches_t *regexp_matches, int match_idx,
 int match_len)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	regexp_matches->matches[match_idx].rm_eo
	 = regexp_matches->matches[match_idx].rm_so + match_len;
}
int regexp_matches_start_idx(regexp_matches_t *regexp_matches, int match_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	return regexp_matches->matches[match_idx].rm_so;
}
int regexp_matches_end_idx(regexp_matches_t *regexp_matches, int match_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	return regexp_matches->matches[match_idx].rm_eo;
}
int regexp_matches_match_len(regexp_matches_t *regexp_matches, int match_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	return regexp_matches->matches[match_idx].rm_eo
	 - regexp_matches->matches[match_idx].rm_so;
}

//------------------------------------------------------------------------------
#ifndef USE_PCRE
PRIVATE char *conv_regex_pcre_to_posix(const char *regexp, char *regex_buf);
#endif // USE_PCRE

// Compile the regular expression regexp to preg.
// Returns 0 on success, non-zero if the expression is invalid.
int regcomp__(regex_t *preg, const char *regexp, int cflags)
{
	char buf[MAX_PATH_LEN+1];
	int ret;

#ifndef USE_PCRE
	regexp = conv_regex_pcre_to_posix(regexp, buf);
#endif // USE_PCRE
	ret = regcomp(preg, regexp, REG_EXTENDED | cflags);	
	if (ret) {
		regerror(ret, preg, buf, MAX_PATH_LEN);
		warning_printf("regerror: [%s]\n", buf);
	}
	return ret;
}

int regexec_1(const regex_t *preg, const char *haystack,
 regmatch_t pmatch[], int eflags)
{
	int ret;

	ret = regexec_n(preg, haystack, 1, pmatch, eflags);
	return ret;
}

int regexec_n(const regex_t *preg, const char *haystack,
 size_t nmatch, regmatch_t pmatch[], int eflags)
{
	int ret;

	if (haystack != NULL && *haystack != '\0') {
		ret = regexec(preg, haystack, nmatch, pmatch, eflags);
		if (ret == 0 && pmatch) {
			// check match-length != 0
			if (pmatch[0].rm_so == pmatch[0].rm_eo) {
///
				_WARNING_
				pmatch[0].rm_eo++;
			}
		}
		return ret;
	}
	return REG_NOMATCH;
}

int regfree__(regex_t *preg)
{
	regfree(preg);
	return 0;
}

//------------------------------------------------------------------------------

#ifndef USE_PCRE
// GNU C library's POSIX regex implementation does not support escape sequence such as "\t".
// So convert two bytes string "\t" to one byte character '\t'.
PRIVATE char *conv_regex_pcre_to_posix(const char *regexp, char *regex_buf)
{
	const char *src;
	char *dest;

	dest = regex_buf;
	for (src = regexp; *src && (dest - regex_buf < MAX_PATH_LEN-1); src++) {
		switch(*src) {
		case '\\':
			switch(*(src+1)) {
			case 't':
				*dest++ = '\t';		// "\t" ==> '\t'
				src++;
				break;
			case 'n':
				*dest++ = '\n';		// "\n" ==> '\n'
				src++;
				break;
			default:
				*dest++ = *src++;	// "\x" ==> "\x"
				*dest++ = *src;
				break;
			}
			break;
		default:
			*dest++ = *src;
			break;
		}
	}
	*dest = '\0';
///	flf_d_printf("\n<{%s}\n>{%s}\n", regexp, regex_buf);
	return regex_buf;
}
#endif // USE_PCRE

#endif // ENABLE_REGEX

// End of regexp.c
/**************************************************************************
 *   search.c                                                             *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#include "headers.h"

#define SEARCH0		0
#define REPLACE1	1

char last_searched_needle[MAX_PATH_LEN+1] = "";	// Last search string

PRIVATE int found_in_prev_search = 1;
PRIVATE int direction_of_prev_search = 0;
PRIVATE be_line_t *line_of_prev_search = NULL;
PRIVATE int byte_idx_of_prev_search = 0;

PRIVATE int input_search_str(int search0_replace1, char *input_buf);
PRIVATE int input_replace_str(char *input_buf);

int do_search_backward_first(void)
{
	char needle[MAX_PATH_LEN+1];

	SET_APPMD(ed_REVERSE_SEARCH);
	if (input_search_str(SEARCH0, needle) <= 0) {
		return -1;
	}
	found_in_prev_search = 1;
	if (search_string_once(needle, 0))
		return 1;
	TOGGLE_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(needle, 0);
}
int do_search_forward_first(void)
{
	char needle[MAX_PATH_LEN+1];

	CLR_APPMD(ed_REVERSE_SEARCH);
	if (input_search_str(SEARCH0, needle) <= 0) {
		return -1;
	}
	found_in_prev_search = 1;
	if (search_string_once(needle, 0))
		return 1;
	TOGGLE_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(needle, 0);
}
int do_search_backward_next(void)
{
	SET_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(last_searched_needle, 1);
}
int do_search_forward_next(void)
{
	CLR_APPMD(ed_REVERSE_SEARCH);
	return search_string_once(last_searched_needle, 1);
}

//------------------------------------------------------------------------------

// TODO: search wrap around of files
//  on forward search, end of the last file ==> top of the first file
//  on reverse search, top of the first file ==> end of the last file

// Replace a string
int do_replace(void)
{
	char replace_from[MAX_PATH_LEN+1];
	char replace_to[MAX_PATH_LEN+1];
	int ret;
	char prev_file_pos[MAX_PATH_LEN+1];
	int num_replaced;

	if (is_view_mode_then_warn_it())
		return 0;

	CLR_APPMD(ed_REVERSE_SEARCH);

	ret = input_search_str(REPLACE1, replace_from);
	if (ret <= 0) {
		return 0;
	}

	ret = input_replace_str(replace_to);
	if (ret <= 0) {
		return 0;
	}
	conv_esc_str(replace_to);

	memorize_cur_file_pos_null(prev_file_pos);

	ret = replace_string_loop(replace_from, replace_to, &num_replaced);

	if (ret == ANSWER_END) {
		// return to original file pos
		recall_cur_file_pos_null(prev_file_pos);
	} else {
		// not return to original file pos
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);

	if (num_replaced >= 0)
		disp_status_bar_done(P_(_("Replaced %d occurrence"),
								_("Replaced %d occurrences"),
								_("Replaced %d occurrencess"),
								_("Replaced %d occurrencesss"),
		 num_replaced), num_replaced);
	else
		disp_status_bar_not_found_msg(replace_from);

	return 0;
}

//------------------------------------------------------------------------------

// search0_replace1 = 0: search, 1: replace
int input_search_str(int search0_replace1, char *input_buf)
{
	char default_needle[MAX_PATH_LEN+1];
	int ret = 0;

	if (strlen(last_searched_needle)) {
		// We use main_win_get_columns() / 3 here because we need to see more on the line
		snprintf(default_needle, MAX_PATH_LEN+1, "(%.*s%s)",
		 main_win_get_columns() / 3, last_searched_needle,
		 strlen(last_searched_needle) > main_win_get_columns() / 3 ? "..." : "");
	} else {
		strcpy__(default_needle, "");
	}

	ret = input_string("", input_buf, HISTORY_TYPE_IDX_SEARCH, "%s%s%s%s%s:",
	 search0_replace1 == 0 ? _("Search") : _("Replace"),
	 GET_APPMD(ed_IGNORE_CASE) ? _("[Ignore-case]") : _("[Differenciate-case]"),
#ifdef ENABLE_REGEX
	 GET_APPMD(ed_USE_REGEXP) ? _("[Regexp]") : "",
#else // ENABLE_REGEX
	 "",
#endif // ENABLE_REGEX
	 GET_APPMD(ed_REVERSE_SEARCH) ? _("[Backward]") : _("[Forward]"), default_needle);

	if (ret <= 0) {
		// cancelled
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
		return ret;						// cancelled
	}
	if (strlen(input_buf) == 0) {
		// nothing input, get last searched string
		strlcpy__(input_buf, last_searched_needle, MAX_PATH_LEN);
	}
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP))
		if (regexp_compile(&search__.regexp, input_buf,
		 GET_APPMD(ed_IGNORE_CASE) ? REG_ICASE : 0)) {
			disp_status_bar_err(_("Invalid regexp: [%s]:%s"),
			 search__.regexp.needle_compiled, search__.regexp.regex_err_msg);
			set_edit_win_update_needed(UPDATE_SCRN_ALL);
			return -1;			// regexp error
		}
#endif // ENABLE_REGEX
	return 1;							// input normally
}

int input_replace_str(char *input_buf)
{
	int ret;

	ret = input_string("", input_buf, HISTORY_TYPE_IDX_SEARCH, "%s:", _("Replace with"));

	if (ret <= 0) {
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
	return ret;
}

//=============================================================================

search_t search__;
matches_t matches__;

//-----------------------------------------------------------------------------

#define INNER_BUFFER_SEARCH		0
#define INTER_BUFFER_SEARCH		1
PRIVATE int search_needle_in_buffers(
 const char *needle, int search_dir, int ignore_case, int skip_here);
PRIVATE int search_needle_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 const char *needle, int search_dir, int ignore_case, int skip_here, int global_search);

#define SEARCH_DIR()	(GET_APPMD(ed_REVERSE_SEARCH) ? (BACKWARD_SEARCH) : (FORWARD_SEARCH))

// Search for a string
int search_string_once(const char *needle, int search_count)
{
	int match_len;

	if (found_in_prev_search == 0 && direction_of_prev_search == SEARCH_DIR()
	 && line_of_prev_search == CEPBV_CL && byte_idx_of_prev_search == CEPBV_CLBI
	 && strcmp(last_searched_needle, needle) == 0) {
		disp_status_bar_not_found_msg(needle);
		return 0;
	}
	if (strlen(needle)) {
		strlcpy__(last_searched_needle, needle, MAX_PATH_LEN);
	}

	match_len = search_needle_in_buffers(needle,
	 SEARCH_DIR(), GET_APPMD(ed_IGNORE_CASE), SKIP);

	if (match_len > 0) {
		if (search_count == 0) {
			// first time
			if (GET_APPMD(ed_REVERSE_SEARCH)) {
				post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_BACKWARD, UPDATE_SCRN_ALL);
			} else {
				post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_FORWARD, UPDATE_SCRN_ALL);
			}
		} else {
			// next time
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_CENTER, UPDATE_SCRN_ALL);
		}
		disp_status_bar_done(_("\"%s\" found in %s search"), needle,
		 GET_APPMD(ed_REVERSE_SEARCH) ? _("backward") : _("forward"));
	}

	found_in_prev_search = match_len;
	direction_of_prev_search = SEARCH_DIR();
	line_of_prev_search = CEPBV_CL;
	byte_idx_of_prev_search = CEPBV_CLBI;
	return match_len;
}

//------------------------------------------------------------------------------
int replace_string_loop(const char *needle, const char *replace_to, int *num_replaced_)
{
	int num_replaced = 0;
#ifdef ENABLE_UNDO
	int num_undone = 0;
#endif // ENABLE_UNDO
	int skip_here;
	int match_len;
	int ret = 0;
	long length_change;
	int prev_ed_REVERSE_SEARCH;
	key_code_t key;

	if (strlen(needle)) {
		strlcpy__(last_searched_needle, needle, MAX_PATH_LEN);
	}

	prev_ed_REVERSE_SEARCH = GET_APPMD(ed_REVERSE_SEARCH);
	for (skip_here = NO_SKIP; ; ) {
		if (prev_ed_REVERSE_SEARCH != GET_APPMD(ed_REVERSE_SEARCH)) {
			prev_ed_REVERSE_SEARCH = GET_APPMD(ed_REVERSE_SEARCH);
			// search direction changed, not skip here on the next search
			skip_here = NO_SKIP;
		}

		match_len = search_needle_in_buffers(needle,
		 SEARCH_DIR(), GET_APPMD(ed_IGNORE_CASE), skip_here);

		if (match_len) {
			// found
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_CENTER, UPDATE_SCRN_ALL);
			update_screen_editor(1, 1, 1);
			if (ret < ANSWER_ALL) {
#ifdef ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO | ASK_ALL | ASK_BACKWARD | ASK_FORWARD | ASK_END
				 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0),
#else // ENABLE_UNDO
				ret = ask_yes_no(ASK_YES_NO | ASK_ALL | ASK_BACKWARD | ASK_FORWARD | ASK_END,
#endif // ENABLE_UNDO
				 _("Replace from [%s] to [%s] ?"), needle, replace_to);
			} else {
				// break ALL-replacing loop
				if ((key = tio_input_key()) >= 0) {
					if (key == K_M_ESC) {
						ret = ANSWER_END;
					} else if (key == K_C_C) {
						ret = ANSWER_CANCEL;
					}
				}
			}
		} else {
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_CENTER, UPDATE_SCRN_ALL);
			update_screen_editor(1, 1, 1);
			// not found message has been displayed
#ifdef ENABLE_UNDO
			ret = ask_yes_no(ASK_NO | ASK_BACKWARD | ASK_FORWARD | ASK_END
			 | (num_replaced ? ASK_UNDO : 0) | (num_undone ? ASK_REDO : 0), "");
#else // ENABLE_UNDO
			ret = ask_yes_no(ASK_NO | ASK_BACKWARD | ASK_FORWARD | ASK_END, "");
#endif // ENABLE_UNDO
		}
		if (ret == ANSWER_NO || ret == ANSWER_FORWARD) {
			// forward search
			CLR_APPMD(ed_REVERSE_SEARCH);
			skip_here = SKIP;
			continue;
		} else if (ret == ANSWER_BACKWARD) {
			// backward search
			SET_APPMD(ed_REVERSE_SEARCH);
			skip_here = SKIP;
			continue;
		} else if (ret == ANSWER_YES || ret == ANSWER_ALL) {
			if (match_len) {
#ifdef ENABLE_UNDO
#ifdef ENABLE_DEBUG
				memorize_undo_state_before_change();
#endif // ENABLE_DEBUG
				undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO
				// ----------------------------------------------------------------------
				length_change = replace_str_in_buffer(&search__, &matches__, replace_to);
				// ----------------------------------------------------------------------
#ifdef ENABLE_UNDO
				undo_save_after_change();
#ifdef ENABLE_DEBUG
				check_undo_state_after_change();
#endif // ENABLE_DEBUG
#endif // ENABLE_UNDO
				// Set the cursor at the last character of the replacement
				// text, so searching will resume after the replacement text. */
				if (GET_APPMD(ed_REVERSE_SEARCH) == 0) {
					// forward search
					CEPBV_CLBI += matches_match_len(&matches__) + length_change;
					skip_here = NO_SKIP;	// CEPBV_CLBI already forwarded to skip word
				} else {
					// backward search
					skip_here = SKIP;		// skip
				}
				get_cep_buf()->buf_size += length_change;
				set_cur_buf_modified();
				num_replaced++;
			}
#ifdef ENABLE_UNDO
		} else if (ret == ANSWER_UNDO) {
			// undo replace
			do_undo();
			num_replaced--;
			num_undone++;
			skip_here = NO_SKIP;
			continue;
		} else if (ret == ANSWER_REDO) {
			// redo replace
			do_redo();
			num_undone--;
			num_replaced++;
			skip_here = NO_SKIP;
			continue;
#endif // ENABLE_UNDO
		} else {			// Cancel
			break;
		}
	}
	*num_replaced_ = num_replaced;
	if (num_replaced && strlen(replace_to)) {
		// copy [string replace_to] ==> [next string to search]
		strlcpy__(last_searched_needle, replace_to, MAX_PATH_LEN);
	}
	return ret;
}

int replace_str_in_buffer(search_t *search, matches_t *matches, const char *replace_to)
{
	char before[MAX_EDIT_LINE_LEN+1];
	char after[MAX_EDIT_LINE_LEN+1];

	strlcpy__(before, CEPBV_CL->data, MAX_EDIT_LINE_LEN);
	strlcpy__(after, CEPBV_CL->data, MAX_EDIT_LINE_LEN);
	// replace in buffer
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP) == 0) {
#endif // ENABLE_REGEX
		replace_str(after, MAX_EDIT_LINE_LEN,
		 matches_start_idx(matches), matches_match_len(matches),
		 replace_to, -1);
#ifdef ENABLE_REGEX
	} else {
		regexp_replace(&search->regexp, &matches->regexp_matches,
		 after, MAX_EDIT_LINE_LEN, replace_to);
	}
#endif // ENABLE_REGEX
	line_set_string(CEPBV_CL, after);		// replace whole of the line
	return strlen(after) - strlen(before);
}

//------------------------------------------------------------------------------

#ifdef ENABLE_REGEX
PRIVATE int do_find_bracket_(int reverse);

// [test string]
// 12345678901234567890123456789012345678901234567890
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
//  {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
//  }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
//  }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// (((((((((((((((((((((((((((((((((((((((((((((((((((
//  ((((((((((((((((((((((((((((((((((((((((((((((((((
//  ))))))))))))))))))))))))))))))))))))))))))))))))))
//  )))))))))))))))))))))))))))))))))))))))))))))))))))
// {{ {{}}} <<>>> {{{}} }} <<<>>
int do_find_bracket(void)
{
	return do_find_bracket_(FORWARD_SEARCH);
}

// [test string]
// 12345678901234567890123456789012345678901234567890
// {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
//  }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
//  {{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{
// }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}
// )))))))))))))))))))))))))))))))))))))))))))))))))))
//  ))))))))))))))))))))))))))))))))))))))))))))))))))
//  ((((((((((((((((((((((((((((((((((((((((((((((((((
//  (((((((((((((((((((((((((((((((((((((((((((((((((((
// {{ }}{{{ }}}{{ }} >><<< >>><<
int do_find_bracket_reverse(void)
{
	return do_find_bracket_(BACKWARD_SEARCH);
}
PRIVATE int do_find_bracket_(int reverse)
{
	char char_under_cursor;
	char char_counterpart;
	int depth_increase;
	char needle[BRACKET_SEARCH_REGEXP_STR_LEN+1];
	int search_dir;			// search direction (FORWARD_SEARCH / BACKWARD_SEARCH)
	be_line_t *line;
	int byte_idx;
	int depth;
	int match_len;
	int skip_here;
	int safe_cnt = 0;

	char_under_cursor = *CEPBV_CL_CEPBV_CLBI;
	search_dir = setup_bracket_search(char_under_cursor, reverse,
	 &depth_increase, &char_counterpart, needle);
	if (search_dir == 0) {
		disp_status_bar_done(_("Not a bracket"));
		return 1;
	}
////flf_d_printf("needle: [%s], %d\n", needle, search_dir);

	set_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();

	line = CEPBV_CL;
	byte_idx = CEPBV_CLBI;
	SET_APPMD(ed_USE_REGEXP);

	memorize_cursor_pos_before_move();

	skip_here = 0;
	for (match_len = 0, depth = 0, safe_cnt = 0; safe_cnt < MAX_BRACKET_STEPS; safe_cnt++) {
flf_d_printf("depth: %d\n", depth);
		match_len = search_bracket_in_buffer(&line, &byte_idx,
		 char_under_cursor, needle, search_dir, skip_here, depth_increase, &depth, NULL);
flf_d_printf("depth: %d\n", depth);
		if ((depth <= 0) || (MAX_BRACKET_NESTINGS <= depth) || (match_len == 0))
			break;
		skip_here = 1;
	}
	if ((match_len > 0) && (depth == 0)) {
		// found peer bracket
		disp_status_bar_done(_("Peer bracket found"));
	} else if (depth < MAX_BRACKET_NESTINGS) {
		// didn't find peer bracket
		disp_status_bar_err(_("No peer bracket found"));
	} else {
		// didn't find peer bracket
		disp_status_bar_err(_("Bracket nesting too deep"));
	}
	if (depth == 0) {
		// found peer bracket
		CEPBV_CL = line;
		CEPBV_CLBI = byte_idx;
		if (search_dir < 0) {
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_BACKWARD, UPDATE_SCRN_ALL);
		} else {
			post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_JUMP_FORWARD, UPDATE_SCRN_ALL);
		}
	}
	return 0;
}

int setup_bracket_search(char char_under_cursor, int reverse,
 int *depth_increase, char *counterpart_char, char *needle)
{
	const char *brackets = "([{<>}])";
	const char *ptr;
	char char_counterpart;
	char regexp_str[] = "[xy]";
	int offset;
	int search_dir;			// search direction (FORWARD_SEARCH / BACKWARD_SEARCH)

////flf_d_printf("char_under_cursor: [%s][%c]\n", brackets, char_under_cursor);
	if ((ptr = strchr__(brackets, char_under_cursor)) == NULL) {
////flf_d_printf("char_under_cursor: [%s][%c]\n", brackets, char_under_cursor);
		return 0;
	}
	offset = ptr - brackets;
	char_counterpart = brackets[strlen(brackets) - (offset + 1)];

	// apparent near redundancy with regexp_str[] here is needed,
	// "[][]" works, "[[]]" doesn't
	if (offset < (strlen(brackets) / 2)) {	// on a left bracket
		regexp_str[1] = char_counterpart;	// ']'
		regexp_str[2] = char_under_cursor;	// '['
		if (reverse >= 0) {
			// [0]
			// ^
			search_dir = FORWARD_SEARCH;	// forward
		} else {
			// ]0[
			//   ^
			search_dir = BACKWARD_SEARCH;	// backward
		}
	} else {								// on a right bracket
		regexp_str[1] = char_under_cursor;	// ']'
		regexp_str[2] = char_counterpart;	// '['
		if (reverse >= 0) {
			// [0]
			//   ^
			search_dir = BACKWARD_SEARCH;	// backward
		} else {
			// ]0[
			// ^
			search_dir = FORWARD_SEARCH;	// forward
		}
	}
	if (depth_increase) {
		*depth_increase = reverse;
	}
	if (counterpart_char) {
		*counterpart_char = char_counterpart;
	}
	if (needle) {
		strlcpy__(needle, regexp_str, BRACKET_SEARCH_REGEXP_STR_LEN);
	}
flf_d_printf("depth_increase/char_counterpart/needle: [%d][%d][%s]\n", *depth_increase, char_counterpart, needle);
	return search_dir;
}

// center of color_idx: 5
// center of depth    : 5
//
// BRACKET_HL_TEST                   V                                 
//         ( ( ( ( ( ( ( ( ( ( ) ) ( ( ( ) ) ) ( ) ( ) ) ) ) ) ) ) ) ) 
// depth  4 5 6 7 0 1 2 3 4 5 6 5 4 5 6 7 6 5 4 5 4 5 4 3 2 1 0 7 6 5 4
// color   4 5 6 7 0 1 2 3 4 5 5 4 4 5 6 6 5 4 4 4 4 4 3 2 1 0 7 6 5 4 
//
// BRACKET_HL_TEST                         V                           
//         ( ( ( ( ( ( ( ( ( ( ) ) ( ( ( ) ) ) ( ) ( ) ) ) ) ) ) ) ) ) 
// depth  3 4 5 6 7 0 1 2 3 4 5 4 3 4 5 6 5 4 3 4 3 4 3 2 1 0 7 6 5 4 3
// color   4 5 6 7 0 1 2 3 4 5 5 4 4 5 6 6 5 4 4 4 4 4 3 2 1 0 7 6 5 4 

int search_bracket_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 char char_under_cursor, const char *needle, int search_dir, int skip_here,
 int depth_increase, int *ptr_depth, int *prev_depth)
{
_D_(line_dump_byte_idx(*ptr_line, *ptr_byte_idx));
flf_d_printf("needle: {%s}, search_dir: %d, skip_here: %d\n", needle, search_dir, skip_here);
	int match_len = search_needle_in_buffer(ptr_line, ptr_byte_idx,
	 needle, search_dir, CASE_SENSITIVE, skip_here, INNER_BUFFER_SEARCH);
	if (match_len > 0) {
		// found bracket
		if ((*ptr_line)->data[*ptr_byte_idx] == char_under_cursor) {
			// found original bracket ==> increase depth
			if (prev_depth) {
				*prev_depth = *ptr_depth;
			}
			(*ptr_depth) += depth_increase;	// post increment/decrement
		} else {
			// found counterpart bracket ==> decrease depth
			(*ptr_depth) -= depth_increase;	// pre increment/decrement
			if (prev_depth) {
				*prev_depth = *ptr_depth;
			}
		}
	}
_D_(line_dump_byte_idx(*ptr_line, *ptr_byte_idx));
flf_d_printf("match_len: %d\n", match_len);
////flf_d_printf("match_len: %d, depth: %d\n", match_len, *ptr_depth);
	return match_len;
}

#define COLORS_FOR_BRACKET_HL	((COLORS16)-2)	// color pairs for counter bracket highlighting

PRIVATE int num_colors_for_bracket_hl = 0;
PRIVATE int color_idx_offset = 0;
PRIVATE item_color_t colors_for_bracket_hl[COLORS_FOR_BRACKET_HL];

void prepare_colors_for_bracket_hl(int offset)
{
	color_idx_offset = offset;
	if (num_colors_for_bracket_hl == 0) {
		char fgc_txt;
		char bgc_txt;
		char fgc_sel;
		char bgc_sel;
		int color_idx = 0;
		get_color_by_idx(ITEM_COLOR_IDX_TEXT_NORMAL, &fgc_txt, &bgc_txt);
		get_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, &fgc_sel, &bgc_sel);
		colors_for_bracket_hl[color_idx].fgc = fgc_sel;
		colors_for_bracket_hl[color_idx].bgc = bgc_sel;
////flf_d_printf("color_idx/fgc/bgc: %d/%d/%d\n", color_idx, fgc_sel, bgc_sel);
		color_idx++;
		for (char fgc = 0; color_idx < COLORS_FOR_BRACKET_HL; fgc++) {
			fgc = fgc % COLORS16;
			if (fgc != fgc_txt && fgc != fgc_sel
			 && fgc != (bgc_txt % COLORS8)) {	// Because there is no light color in BGC
////flf_d_printf("color_idx/fgc/bgc: %d/%d/%d\n", color_idx, fgc, bgc_txt);
				colors_for_bracket_hl[color_idx].fgc = fgc;
				colors_for_bracket_hl[color_idx].bgc = bgc_txt;
				color_idx++;
			}
		}
		num_colors_for_bracket_hl = color_idx;
	}
}
int get_colors_for_bracket_hl()
{
	if (num_colors_for_bracket_hl == 0) {
		prepare_colors_for_bracket_hl(0);
	}
	return num_colors_for_bracket_hl;
}
void set_color_for_bracket_hl(int color_idx)
{
	char fgc, bgc;

	color_idx -= color_idx_offset;
	color_idx %= num_colors_for_bracket_hl;		// (-num_colors, num_colors)
	color_idx += num_colors_for_bracket_hl;		// make [0, 2*num_colors)
	color_idx %= num_colors_for_bracket_hl;
	color_idx %= num_colors_for_bracket_hl;
	get_color_for_bracket_hl(color_idx, &fgc, &bgc);
flf_d_printf("color_idx-%d %d/%d\n", color_idx, fgc, bgc);
	tio_set_attrs(bgc, fgc, 0);
}
void get_color_for_bracket_hl(int color_idx, char *fgc, char *bgc)
{
	if (get_colors_for_bracket_hl() <= 0) {
		get_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, fgc, bgc);
	} else {
		*bgc = colors_for_bracket_hl[color_idx].bgc;
		*fgc = colors_for_bracket_hl[color_idx].fgc;
	}
}

#endif // ENABLE_REGEX

//------------------------------------------------------------------------------

PRIVATE int search_needle_in_buffers(
 const char *needle, int search_dir, int ignore_case, int skip_here)
{
	int match_len;

	disp_status_bar_ing(_("Searching word: [%s]..."), needle);

	match_len = search_needle_in_buffer(&(CEPBV_CL), &(CEPBV_CLBI),
	 needle, search_dir, ignore_case, skip_here, INTER_BUFFER_SEARCH);

	if (match_len > 0) {
		// found
		return match_len;
	}
	tio_beep();
	disp_status_bar_not_found_msg(needle);
	return 0;
}

// search keyword in buffer (when global_search is false) or buffers (when true).
// - when keyword not found, return to original cursor position.
// WARNING: When "global_search" is true, c_e_b may be changed.
PRIVATE int search_needle_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 const char *needle, int search_dir, int ignore_case, int skip_here, int global_search)
{
	be_line_t *line;
	int byte_idx;
	int match_len;

	line = *ptr_line;
	byte_idx = *ptr_byte_idx;
	match_len = 0;

	memorize_cur_file_pos_null(NULL);
	memorize_cursor_pos_before_move();

	if (search_dir <= BACKWARD_SEARCH) {
		// search backward -----------------------------------------------------
		while (1) {
			if (skip_here) {
				// move cur-pos left at least one char
				// if cur-pos is left most, move cur-pos up at least one line
				if (byte_idx > 0) {
					byte_idx--;
				} else if (IS_NODE_TOP(line) == 0) {
					line = NODE_PREV(line);
					byte_idx = line_data_len(line);
				} else if (global_search && switch_c_e_b_to_prev(0, 0)) {
					// update pointers after switching buffer
					// but not update pointers in buffer
					ptr_line = &(CEPBV_CL);
					ptr_byte_idx = &(CEPBV_CLBI);
					line = CUR_EDIT_BUF_BOT_NODE;
					byte_idx = line_data_len(line);
				} else {
					break;
				}
			}
			// look for needle backward in haystack
			if ((match_len = search_str_in_line(&search__, &matches__,
			 needle, search_dir, ignore_case, line->data, byte_idx)) > 0) {
				// found
				break;
			}
			byte_idx = 0;
			skip_here = 1;
		}
	} else {
		// search forward ------------------------------------------------------
		while (1) {
			if (skip_here) {
				// move cur-pos right at least one char
				// if cur-pos is right most, move cur-pos down at least one line
				if (byte_idx < line_data_len(line)) {
					byte_idx++;
				} else if (IS_NODE_BOT(line) == 0) {
					line = NODE_NEXT(line);
					byte_idx = 0;
				} else if (global_search && switch_c_e_b_to_next(0, 0)) {
					// update pointers after switching buffer
					// but not update pointers in buffer
					ptr_line = &(CEPBV_CL);
					ptr_byte_idx = &(CEPBV_CLBI);
					line = CUR_EDIT_BUF_TOP_NODE;
					byte_idx = 0;
				} else {
					break;
				}
			}
			// look for needle forward in haystack
			if ((match_len = search_str_in_line(&search__, &matches__,
			 needle, search_dir, ignore_case, line->data, byte_idx)) > 0) {
				// found
				break;
			}
			byte_idx = line_data_len(line);
			skip_here = 1;
		}
	}

	if (match_len > 0) {
		// found and update current line pointer
		*ptr_line = line;
		*ptr_byte_idx = matches_start_idx(&matches__);
///_D_(dump_editor_panes());
		return match_len;
	}
	// not found then return to begining position
	recall_cur_file_pos_null(NULL);
	return match_len;
}

void disp_status_bar_not_found_msg(const char *str)
{
	disp_status_bar_err(_("\"%s\" not found"), shrink_str_to_scr_static(str));
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}

// End of search.c
/**************************************************************************
 *   sig.c                                                                *
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

#include "headers.h"

void signal_init(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(struct sigaction));

	// Trap SIGINT and SIGQUIT cuz we want them to do useful things.
///	sigact.sa_handler = SIG_IGN;
	sigact.sa_handler = handler_sigint;
	sigaction(SIGINT, &sigact, NULL);

	// Trap SIGHUP and SIGTERM cuz we want to write the file out.
	sigact.sa_handler = handler_sighup;
	sigaction(SIGHUP, &sigact, NULL);
	sigact.sa_handler = handler_sigterm;
	sigaction(SIGTERM, &sigact, NULL);

	// Trap SIGWINCH.
	sigact.sa_handler = handle_sigwinch;
	sigaction(SIGWINCH, &sigact, NULL);

	// Trap SIGSEGV.
	sigact.sa_handler = handler_sigsegv;
	sigaction(SIGSEGV, &sigact, NULL);

#ifndef _POSIX_VDISABLE
	sigact.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &sigact, NULL);		// Terminal SToP signal
#endif
}

void signal_fork(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(struct sigaction));

	// Trap SIGINT.
	sigact.sa_handler = handler_sigint;
	sigaction(SIGINT, &sigact, NULL);
	// Trap SIGWINCH.
	sigact.sa_handler = handle_sigwinch;
	sigaction(SIGWINCH, &sigact, NULL);
}

void signal_clear(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(struct sigaction));

	sigact.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGWINCH, &sigact, NULL);
	sigaction(SIGSEGV, &sigact, NULL);
#ifndef _POSIX_VDISABLE
	sigaction(SIGTSTP, &sigact, NULL);
#endif
}

PRIVATE int handler_sigint_called = 0;
void clear_handler_sigint_called(void)
{
	handler_sigint_called = 0;
}
RETSIGTYPE handler_sigint(int signal)
{
	handler_sigint_called = 1;
}
int is_handler_sigint_called(void)
{
	return handler_sigint_called;
}

RETSIGTYPE handler_sighup(int signal)
{
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(500);
	die_on("Received SIGHUP\n");
}
RETSIGTYPE handler_sigterm(int signal)
{
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(500);
	die_on("Received SIGTERM\n");
}
RETSIGTYPE handler_sigsegv(int signal)
{
	flf_d_printf("Received SIGSEGV\n");
#if 0
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(500);
#endif
	_D_(output_last_d_printf());
	exit(255);
}

PRIVATE int sigwinch_signaled = 0;
void clear_sigwinch_signaled(void)
{
	sigwinch_signaled = 0;
}
RETSIGTYPE handle_sigwinch(int signal)
{
	sigwinch_signaled = 1;
	ioctl_get_win_size();
}
int is_sigwinch_signaled(void)
{
	return sigwinch_signaled;
}

int ioctl_ws_col = -1;
int ioctl_ws_row = -1;
int ioctl_get_win_size(void)
{
	const char *tty;
	int fd;
	int result = 0;
	struct winsize winsz;

	tty = ttyname(0);
	if (tty == NULL) {
		///_FLF_
		return -1;
	}
	fd = open(tty, O_RDWR);
	if (fd < 0) {
		///flf_d_printf("%s: tty: [%s]\n", strerror(errno), tty);
		///_FLF_
		return -2;
	}
	result = ioctl(fd, TIOCGWINSZ, &winsz);
	close(fd);
	if (result < 0) {
		///flf_d_printf("%s: tty: [%s]\n", strerror(errno), tty);
		///_FLF_
		return -3;
	}
	// screen size gotten.
	// in some cases ncurses has already updated them but not in all cases.
	ioctl_ws_col = winsz.ws_col;
	ioctl_ws_row = winsz.ws_row;
///flf_d_printf("cols: %d, lines: %d\n", ioctl_ws_col, ioctl_ws_row);
	return 0;
}

// End of sig.c
/**************************************************************************
 *   termif.c - Linux terminal interface                                  *
 *                                                                        *
 *   Copyright (C) 2020-2020 Toshio Koike                                 *
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

#include "headers.h"

#ifndef ENABLE_NCURSES

// Escape sequences used:
//   ESC c						// clear screen
//   ESC [ 6 n					// report cursor pos.
//   ESC [ ?25h"				// cursor ON
//   ESC [ ?25l"				// cursor OFF
//   ESC [ {row} ; {col} H		// set cursor pos.
//   ESC [ {n} m				// set attributes
//     n : 0, 1, 7, 30--37, 40--47

////#define TERMIF_MAX_SCRN_COLS		384		// = 1920[pixels] / 5[pixels/char] (Full HD)
////#define TERMIF_MAX_SCRN_COLS		512		// = 2560[pixels] / 5[pixels/char] (WQXGA)
#define TERMIF_MAX_SCRN_COLS		768		// = 3840[pixels] / 5[pixels/char] (4K landscape)
////#define TERMIF_MAX_SCRN_LINES		108		// = 1080[pixels] / 10[pixels/char] (Full HD)
#define TERMIF_MAX_SCRN_LINES		384		// = 3840[pixels] / 10[pixels/char] (4K portrait)
#define TERMIF_LINE_BUF_LEN			(TERMIF_MAX_SCRN_COLS * MAX_UTF8C_BYTES)

// Virtual screen
// ++++-------------------------------- Background color
// ||||++++---------------------------- Foreground color
// |||||||| +-------------------------- 1:Reverse
// |||||||| |+------------------------- 1:Bold
// |||||||| ||+------------------------ 1:1st place of wide char., 0:narrow char.
// |||||||| |||+++++-++++++++-++++++++- UCS21
// |||||||| |||||||| |||||||| ||||||||
// bbbbffff RBWccccc cccccccc cccccccc
#define VSCR_CHAR_BGC		0xf0000000UL
#define VSCR_CHAR_FGC		0x0f000000UL
#define VSCR_CHAR_REV		0x00800000UL
#define VSCR_CHAR_BOLD		0x00400000UL
#define VSCR_CHAR_WIDTH1	0x00000000UL	// narrow char. or 2nd place of wide char.
#define VSCR_CHAR_WIDTH2	0x00200000UL	// 1st place of wide char.
#define VSCR_IS_WIDE_CHAR_COL1(vscr_char)	((vscr_char) & VSCR_CHAR_WIDTH2)
#define VSCR_CHAR_UCS21		0x001fffffUL	// 21 bits
#define VSCR_CHAR_UCS21_SPACE		' '		// space
#define VSCR_CHAR_ATTRS		(VSCR_CHAR_BGC | VSCR_CHAR_FGC | VSCR_CHAR_REV | VSCR_CHAR_BOLD)
#define VSCR_CHAR_ATTRS_DEFAULT				0x07000000UL
#define VSCR_CHAR_ATTRS_FROM_BGC(bgc)		(((bgc) << 28) & VSCR_CHAR_BGC)	// 0xf0000000
#define VSCR_CHAR_ATTRS_FROM_FGC(fgc)		(((fgc) << 24) & VSCR_CHAR_FGC)	// 0x0f000000
#define SET_ATTRS_FROM_BGC(attrs, bgc)		\
	((attrs) = VSCR_CHAR_ATTRS_FROM_BGC(bgc) | ((attrs) & ~VSCR_CHAR_BGC))
#define SET_ATTRS_FROM_FGC(attrs, fgc)		\
	((attrs) = VSCR_CHAR_ATTRS_FROM_FGC(fgc) | ((attrs) & ~VSCR_CHAR_FGC))
#define BGC_FROM_VSCR_CHAR_ATTRS(attrs)		(((attrs) & VSCR_CHAR_BGC) >> 28)	// 0x0f
#define FGC_FROM_VSCR_CHAR_ATTRS(attrs)		(((attrs) & VSCR_CHAR_FGC) >> 24)	// 0x0f
#define VSCR_CHAR_REPLACE_WIDTH_UCS21(vscr_char, width, ucs21)	\
	(vscr_char) = ((vscr_char) & VSCR_CHAR_ATTRS) | (width) | (ucs21)
typedef unsigned long vscreen_char_t;
typedef vscreen_char_t vscreen_buf_t[TERMIF_MAX_SCRN_LINES][TERMIF_MAX_SCRN_COLS];
PRIVATE vscreen_char_t vscreen_to_paint[TERMIF_MAX_SCRN_LINES][TERMIF_MAX_SCRN_COLS];
PRIVATE vscreen_char_t vscreen_painted[TERMIF_MAX_SCRN_LINES][TERMIF_MAX_SCRN_COLS];

PRIVATE vscreen_char_t termif_attrs;
PRIVATE int termif_cursor_on = 0;
PRIVATE int termif_cursor_yy = 0;
PRIVATE int termif_cursor_xx = 0;

PRIVATE int termif_lines = 25;
PRIVATE int termif_columns = 80;

//-----------------------------------------------------------------------------

PRIVATE struct termios term_settings_save;	/* The user's original term settings */

//-----------------------------------------------------------------------------

PRIVATE void set_string_to_vscreen(const char *string, int bytes);
PRIVATE void put_narrow_char_to_vscreen(vscreen_char_t ucs21);
PRIVATE void put_wide_char_to_vscreen(vscreen_char_t ucs21);

#ifdef ENABLE_DEBUG
PRIVATE void dump_vscreen(int yy, int len);
#endif // ENABLE_DEBUG

PRIVATE void send_cursor_pos_string_to_term(int yy, int xx, const char *string, int bytes);
PRIVATE void send_cursor_on_to_term(int on_off);
PRIVATE void send_cursor_pos_to_term(int yy, int xx);
PRIVATE vscreen_char_t attrs_sent = VSCR_CHAR_ATTRS_DEFAULT;	// attributes sent to terminal
PRIVATE void send_attrs_to_term(vscreen_char_t attrs);
PRIVATE void send_all_off_to_term(void);
PRIVATE void send_bold_to_term(int bold);
///PRIVATE void send_reverse_to_term(int reverse);
PRIVATE void send_bgc_to_term(int bgc);
PRIVATE void send_fgc_to_term(int fgc);
PRIVATE void send_printf_to_term(const char *format, ...);
PRIVATE void send_string_to_term(const char *string, int bytes);
PRIVATE void send_string_to_term__(const char *string, int bytes);
PRIVATE void send_string_to_term_flush(void);

PRIVATE key_code_t input_and_decode_key_sequences(void);
PRIVATE key_code_t compare_key_seq(char *key_sequences);
PRIVATE key_code_t input_key(void);

//-----------------------------------------------------------------------------

int termif_init(void)
{
	save_term_settings(&term_settings_save);
	return 0;
}
int termif_begin(void)
{
	set_term_no_intr();
	set_term_raw();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	termif_get_screen_size_from_term();
	termif_attrs = VSCR_CHAR_ATTRS_DEFAULT;
	termif_clear_screen();
	return 0;
}
int termif_end(void)
{
	send_all_off_to_term();
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	restore_term_settings(&term_settings_save);
	return 0;
}
//-----------------------------------------------------------------------------
// get screen size from terminal
int termif_get_screen_size_from_term(void)
{
	int termif_lines_save;
	int termif_columns_save;
	int tries;
	int lines;
	int cols;

#define MAX_REPORT_TRIES		3
	termif_lines_save = termif_get_lines();
	termif_columns_save = termif_get_columns();
	termif_set_screen_size(999, 999);
	for (tries = 0; tries < MAX_REPORT_TRIES; tries++) {
		send_cursor_pos_to_term(998, 998);
		if (termif_get_cursor_pos(&lines, &cols)) {
			if (lines <= TERMIF_MAX_SCRN_LINES && cols <= TERMIF_MAX_SCRN_COLS) {
				termif_set_screen_size(lines, cols);
				return 1;
			}
		}
	}
	termif_set_screen_size(termif_lines_save, termif_columns_save);
	return 0;
}
void termif_set_screen_size(int lines, int columns)
{
	termif_lines = lines;
	termif_columns = columns;
}
int termif_get_lines(void)
{
	return termif_lines;
}
int termif_get_columns(void)
{
	return termif_columns;
}
//-----------------------------------------------------------------------------
void termif_clear_screen(void)
{
	send_all_off_to_term();
	send_printf_to_term("\x1b" "c");
	memset((void *)vscreen_to_paint, 0x00, sizeof(vscreen_to_paint));
	termif_clear_vscreen_painted();
}
void termif_clear_vscreen_painted(void)
{
	memset((void *)vscreen_painted, 0x00, sizeof(vscreen_painted));
}
void termif_set_cursor_pos(int yy, int xx)
{
	if (yy >= 0 && xx >= 0) {
		termif_cursor_yy = yy;
		termif_cursor_xx = xx;
		send_cursor_pos_to_term(termif_cursor_yy, termif_cursor_xx);
	}
}
int termif_get_cursor_pos(int *yy, int *xx)
{
#define MIN_REPORT_LEN		6	// "e[9;9R"
#define MAX_REPORT_LEN		(10+10)	// "e[999;999R"
	char buf[MAX_REPORT_LEN+1];	// "e[999;999R"
	int len;
	const char *ptr;
	int lines, cols;

	fflush(stdin);
	send_printf_to_term("\x1b[6n");
	MSLEEP(100);	// wait for receiving answer back
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in read()
	if ((len = read(STDIN_FILENO, buf, MAX_REPORT_LEN)) >= MIN_REPORT_LEN) {
		buf[len] = '\0';
///flf_d_printf("report:[%s]\n", buf);
		for (ptr = buf; *ptr; ptr++) {
			if (isdigit(*ptr))
				break;
		}
///flf_d_printf("report:[%s]\n", ptr);
		// ESC [ lines ; cols R
		if (sscanf(ptr, "%d;%d", &lines, &cols) >= 2) {
///flf_d_printf("cursor-pos: %d, %d\n", lines, cols);
			*yy = lines;
			*xx = cols;
			return 1;
		}
	}
	return 0;
}
void termif_set_attrs(int bgc, int fgc, int rev)
{
///flf_d_printf("bgc:%d, fgc:%d, rev:%d, bold:%d\n", bgc, fgc, rev, bold);
	if (rev >= 0) {
		if (rev == 0) {
			termif_attrs &= ~VSCR_CHAR_REV;
		} else {
			termif_attrs |= VSCR_CHAR_REV;
		}
	}
	if (bgc >= 0) {
		SET_ATTRS_FROM_BGC(termif_attrs, bgc);
	}
	if (fgc >= 0) {
		SET_ATTRS_FROM_FGC(termif_attrs, fgc);
	}
///flf_d_printf("attrs: %08lx\n", termif_attrs);
}

void termif_set_cursor_on(int on_off)
{
	termif_cursor_on = on_off;
	send_cursor_on_to_term(on_off);
}
void termif_beep(void)
{
	send_printf_to_term("\x07");	// "\a"(^G)
}
void termif_output_string(int yy, int xx, const char *string, int bytes)
{
	if (yy >= 0 && xx >= 0) {
		termif_cursor_yy = yy;
		termif_cursor_xx = xx;
	}
	if (bytes < 0) {
		bytes = strlen(string);
	}
	set_string_to_vscreen(string, bytes);
}
PRIVATE void set_string_to_vscreen(const char *string, int bytes)
{
	const char *str;
	vscreen_char_t ucs21;
	int width;

///flf_d_printf("[%s]\n", string);
///flf_d_printf("bytes: %d\n", bytes);
	for (str = string; str - string < bytes; str += utf8c_bytes(str)) {
///flf_d_printf("%d\n", str - string);
		ucs21 = utf8c_decode(str);
		width = utf8c_columns(str);
///flf_d_printf("%04x: %d\n", ucs21, width);
		if (width == 1) {			// narrow char.
			put_narrow_char_to_vscreen(ucs21);
		} else if (width == 2) {	// wide char.
			put_wide_char_to_vscreen(ucs21);
		}
	}
///_FLF_
}
PRIVATE void put_narrow_char_to_vscreen(vscreen_char_t ucs21)
{
	if ((0 <= termif_cursor_yy && termif_cursor_yy < termif_lines)
	 && (0 <= termif_cursor_xx && termif_cursor_xx < termif_columns)) {
#define CHECK_AND_CORRECT_INCONSISTENCY
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
		if ((0 <= termif_cursor_xx-1)
		 && VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1])) {
			// putting narrow char. to the 2nd byte of wide char.
			//                       clear 1st byte of wide char.
			//  v      v
			// [] ==> _A
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
		if (VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx])
		 && (termif_cursor_xx+1 < termif_columns)) {
			// putting narrow char. to the 1st byte of wide char.
			//                       clear 2nd byte of wide char.
			// v      v
			// [] ==> A_
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
#endif // CHECK_AND_CORRECT_INCONSISTENCY
		vscreen_to_paint[termif_cursor_yy][termif_cursor_xx]
		 = (termif_attrs & VSCR_CHAR_ATTRS) | VSCR_CHAR_WIDTH1 | (ucs21 & VSCR_CHAR_UCS21);
		termif_cursor_xx++;
	}
}
PRIVATE void put_wide_char_to_vscreen(vscreen_char_t ucs21)
{
	if ((0 <= termif_cursor_yy && termif_cursor_yy < termif_lines)
	 && (0 <= termif_cursor_xx && termif_cursor_xx < termif_columns)
	 && (0 <= termif_cursor_xx+1 && termif_cursor_xx+1 < termif_columns)) {
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
		if ((0 <= termif_cursor_xx-1)
		 && VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1])) {
			// putting 1st byte of wide char to the 2nd byte of wide char which is prevly placed.
			//                                clear 1st byte of wide char which is prevly placed.
			//  v      v
			// [] ==> _[]
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
		if (VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+1])
		 && (termif_cursor_xx+2 < termif_columns)) {
			// putting 2nd byte of wide char to the 1st byte of prevly placed wide char.
			//                                clear 2nd byte of prevly placed wide char.
			//  v       v
			//   [] ==> []_
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+2],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
#endif // CHECK_AND_CORRECT_INCONSISTENCY
		vscreen_to_paint[termif_cursor_yy][termif_cursor_xx]
		 = (termif_attrs & VSCR_CHAR_ATTRS) | VSCR_CHAR_WIDTH2 | (ucs21 & VSCR_CHAR_UCS21);
		termif_cursor_xx++;
		vscreen_to_paint[termif_cursor_yy][termif_cursor_xx]
		 = (termif_attrs & VSCR_CHAR_ATTRS) | VSCR_CHAR_WIDTH1 | (ucs21 & VSCR_CHAR_UCS21);
		termif_cursor_xx++;
	}
}
#ifdef ENABLE_DEBUG
PRIVATE void dump_vscreen(int yy, int len)
{
	char utf8c[MAX_UTF8C_BYTES + 1];
	for (int vscr_idx = 0; vscr_idx < 2; vscr_idx++) {
		vscreen_buf_t *buf = (vscr_idx == 0) ? &vscreen_to_paint : &vscreen_painted;
		flf_d_printf("%s(%d): ", (vscr_idx == 0) ? "topaint" : "painted", yy);
		for (int idx = 0; idx < len; idx++) {
			utf8c_encode((*buf)[yy][idx] & VSCR_CHAR_UCS21, utf8c);
			d_printf(" %s", utf8c);
		}
		d_printf("\n");
	}
}
#endif // ENABLE_DEBUG
//-----------------------------------------------------------------------------
// If narrow char, compare 1st place.
// If wide   char, compare 1st and 2nd place.
#define CMP_NARR_OR_WIDE_CHR()										\
	(VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[yy][xx]) == 0			\
	 ? (vscreen_painted[yy][xx] != vscreen_to_paint[yy][xx])		\
	 : (vscreen_painted[yy][xx] != vscreen_to_paint[yy][xx]			\
	  || vscreen_painted[yy][xx+1] != vscreen_to_paint[yy][xx+1]))

#ifdef VAGUE_WIDE_CHR
///
#define OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
#endif // VAGUE_WIDE_CHR
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
// If is_vague_wide_chr(), display this char as if wide char.
// (display "  " and display this wide char to 1st place)
// Expected effect:
//   If this char is narrow char, displayed "N ".
//   If this char is wide   char, displayed "WW".
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE

// refresh screen by sending pending data in vscreen_to_paint to the screen.
void termif_refresh(void)
{
	int cursor_on = 1;
	int yy, xx;
	int start_xx;
	vscreen_char_t start_attrs;
	char utf8c[MAX_UTF8C_BYTES + 1];
	wchar_t ucs21;
	char line_buf[TERMIF_LINE_BUF_LEN + 1];

	for (yy = 0; yy < termif_lines; yy++) {
		for (xx = 0; xx < termif_columns; ) {
			start_attrs = (vscreen_to_paint[yy][xx] & VSCR_CHAR_ATTRS);
			if (CMP_NARR_OR_WIDE_CHR()) {
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
				ucs21 = vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21;
				if (is_vague_wide_chr(ucs21)) {
					if (cursor_on) {
						// erase cursor before painting
						send_cursor_on_to_term(0);
						cursor_on = 0;
					}
					send_attrs_to_term(start_attrs);
					send_cursor_pos_string_to_term(yy, xx, "  ", -1);	// "  "
					utf8c_encode(ucs21, utf8c);
					send_cursor_pos_string_to_term(yy, xx, utf8c, -1);	// "H " or "WW"
					vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
					xx++;
					vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
					xx++;
				} else {
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
					start_xx = xx;
					strcpy(line_buf, "");
					for ( ; xx < termif_columns; ) {
						ucs21 = vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21;
						if (((vscreen_to_paint[yy][xx] & VSCR_CHAR_ATTRS) != start_attrs)
						 || CMP_NARR_OR_WIDE_CHR() == 0
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
						 || is_vague_wide_chr(ucs21)
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
													) {
							break;
						}
						utf8c_encode(ucs21, utf8c);
						strlcat__(line_buf, TERMIF_LINE_BUF_LEN, utf8c);
						if (VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[yy][xx])
						 && ((vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21)
						  == (vscreen_to_paint[yy][xx+1] & VSCR_CHAR_UCS21))) {
							// wide char and, 1st and 2nd UCS are the same.
							vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
							xx++;
						}
						vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
						xx++;
					}
					if (cursor_on) {
						// erase cursor before painting
						send_cursor_on_to_term(0);
						cursor_on = 0;
					}
					send_attrs_to_term(start_attrs);
					send_cursor_pos_string_to_term(yy, start_xx, line_buf, -1);
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
				}
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
			} else {
				xx++;
			}
		}
	}
	if (cursor_on == 0) {
		// If cursor erased in this func, restore cursor.
		send_cursor_pos_to_term(termif_cursor_yy, termif_cursor_xx);
		send_cursor_on_to_term(termif_cursor_on);
	}
///_FLF_
}

PRIVATE void send_cursor_pos_string_to_term(int yy, int xx, const char *string, int bytes)
{
	send_cursor_pos_to_term(yy, xx);
	send_string_to_term(string, bytes);
}

PRIVATE void send_cursor_on_to_term(int on_off)
{
	if (on_off) {
		send_printf_to_term("\x1b[?25h");
	} else {
		send_printf_to_term("\x1b[?25l");
	}
}
PRIVATE void send_cursor_pos_to_term(int yy, int xx)
{
	if ((0 <= yy && yy < termif_lines)
	 && (0 <= xx && xx < termif_columns)) {
#if 1
		send_printf_to_term("\x1b[%d;%dH", yy+1, xx+1);
#else
		send_printf_to_term("\x1b[%d;%df", yy+1, xx+1);
#endif
	} else {
		warning_printf("(%d, %d)\n", xx, yy);
mflf_d_printf("(%d, %d)\n", termif_columns, termif_lines);
	}
}
PRIVATE void send_attrs_to_term(vscreen_char_t attrs)
{
	vscreen_char_t attrs_xor;
	attrs_xor = (attrs_sent ^ attrs) & VSCR_CHAR_ATTRS;
	if (attrs_xor == 0) {		// attributes changed ?
		return;
	}

	int bgc;
	int fgc;
	int real_bgc;
	int real_fgc;
	bgc = BGC_FROM_VSCR_CHAR_ATTRS(attrs);
	fgc = FGC_FROM_VSCR_CHAR_ATTRS(attrs);
	if ((attrs & VSCR_CHAR_REV) == 0) {
		real_bgc = bgc;
		real_fgc = fgc;
	} else {
		// reverse ==> swap foreground and background color
		real_bgc = fgc;
		real_fgc = bgc;
	}
	real_bgc = LIMIT_BGC(real_bgc);
	real_fgc = LIMIT_FGC(real_fgc);
	real_fgc = differ_fgc_to_bgc(real_bgc, real_fgc);
	send_all_off_to_term();
	send_bgc_to_term(real_bgc);
	send_fgc_to_term(real_fgc);
	attrs_sent = attrs;
}
PRIVATE void send_all_off_to_term(void)
{
	send_printf_to_term("\x1b[0m");
	attrs_sent = VSCR_CHAR_ATTRS_DEFAULT;
}
PRIVATE void send_bold_to_term(int bold)
{
	if (bold) {
		send_printf_to_term("\x1b[1m");
///	} else {
///		send_printf_to_term("\x1b[21m");	// DNU: Linux console does NOT support this.
	}
}
///PRIVATE void send_reverse_to_term(int reverse)
///{
///	if (reverse) {
///		send_printf_to_term("\x1b[7m");
//////	} else {
//////		send_printf_to_term("\x1b[27m");	// DNU: Linux console does NOT support this.
///	}
///}
PRIVATE void send_bgc_to_term(int bgc)
{
	send_printf_to_term("\x1b[%dm", 40 + (bgc % COLORS));
	// NOTE: Linux console supports highlight only for foreground
	//       and does not support highlight for background
}
PRIVATE void send_fgc_to_term(int fgc)
{
	send_printf_to_term("\x1b[%dm", 30 + (fgc % COLORS));
	send_bold_to_term(fgc >= COLORS);
}
PRIVATE void send_printf_to_term(const char *format, ...)
{
#define MAX_ESC_SEQ_LEN		(10+10)	// "e[999;999R"
	va_list ap;
	char buffer[MAX_ESC_SEQ_LEN+1];

	va_start(ap, format);
	vsnprintf(buffer, MAX_ESC_SEQ_LEN+1, format, ap);
	va_end(ap);
///d_printf("XXX{%s}\n", buffer);
	send_string_to_term__(buffer, -1);
}
PRIVATE void send_string_to_term(const char *string, int bytes)
{
	send_string_to_term__(string, bytes);
}
PRIVATE void send_string_to_term__(const char *string, int bytes)
{
	int written;

	if (bytes < 0) {
		bytes = strlen(string);
	}
	fcntl(STDOUT_FILENO, F_SETFL, 0);		// Block in write()
	if ((written = write(STDOUT_FILENO, string, bytes)) < bytes) {
		mflf_d_printf("ERROR: writing to STDOUT(%s)(%d < %d)\n",
		 strerror(errno), written, bytes);
		mflf_d_printf("[%s]\n", string);
	}
	send_string_to_term_flush();
}
PRIVATE void send_string_to_term_flush(void)
{
	fsync(STDOUT_FILENO);
}

//-----------------------------------------------------------------------------

key_code_t termif_input_key(void)
{
	return input_and_decode_key_sequences();
}

#define MAX_KEY_SEQ		7

struct _key_seq_ {
	key_code_t key;
	char *sequences;
} key_seq_table[] = {
	//			 1---2---3---4---5---6---7---
	// linux console
	{ K_F01,	"\x1b\x5b\x5b\x41" },
	{ K_F02,	"\x1b\x5b\x5b\x42" },
	{ K_F03,	"\x1b\x5b\x5b\x43" },
	{ K_F04,	"\x1b\x5b\x5b\x44" },
	{ K_F05,	"\x1b\x5b\x5b\x45" },
	{ K_F06,	"\x1b\x5b\x31\x37\x7e" },
	{ K_F07,	"\x1b\x5b\x31\x38\x7e" },
	{ K_F08,	"\x1b\x5b\x31\x39\x7e" },
	{ K_F09,	"\x1b\x5b\x32\x30\x7e" },
	{ K_F10,	"\x1b\x5b\x32\x31\x7e" },
	{ K_F11,	"\x1b\x5b\x32\x33\x7e" },
	{ K_F12,	"\x1b\x5b\x32\x34\x7e" },
	{ K_SF01,	"\x1b\x5b\x32\x35\x7e" },
	{ K_SF02,	"\x1b\x5b\x32\x36\x7e" },
	{ K_SF03,	"\x1b\x5b\x32\x38\x7e" },
	{ K_SF04,	"\x1b\x5b\x32\x39\x7e" },
	{ K_SF05,	"\x1b\x5b\x33\x31\x7e" },
	{ K_SF06,	"\x1b\x5b\x33\x32\x7e" },
	{ K_SF07,	"\x1b\x5b\x33\x33\x7e" },
	{ K_SF08,	"\x1b\x5b\x33\x34\x7e" },
	{ K_IC,		"\x1b\x5b\x32\x7e" },
	{ K_DC,		"\x1b\x5b\x33\x7e" },
	{ K_HOME,	"\x1b\x5b\x31\x7e" },
	{ K_END,	"\x1b\x5b\x34\x7e" },
	{ K_PPAGE,	"\x1b\x5b\x35\x7e" },
	{ K_NPAGE,	"\x1b\x5b\x36\x7e" },
	{ K_UP,		"\x1b\x5b\x41" },
	{ K_DOWN,	"\x1b\x5b\x42" },
	{ K_LEFT,	"\x1b\x5b\x44" },
	{ K_RIGHT,	"\x1b\x5b\x43" },
	// xterm
	{ K_F01,	"\x1b\x4f\x50" },
	{ K_F02,	"\x1b\x4f\x51" },
	{ K_F03,	"\x1b\x4f\x52" },
	{ K_F04,	"\x1b\x4f\x53" },
	{ K_F05,	"\x1b\x5b\x31\x35\x7e" },
	{ K_F06,	"\x1b\x5b\x31\x37\x7e" },
	{ K_F07,	"\x1b\x5b\x31\x38\x7e" },
	{ K_F08,	"\x1b\x5b\x31\x39\x7e" },
	{ K_F09,	"\x1b\x5b\x32\x30\x7e" },
	{ K_F10,	"\x1b\x5b\x32\x31\x7e" },
	{ K_F11,	"\x1b\x5b\x32\x33\x7e" },
	{ K_F12,	"\x1b\x5b\x32\x34\x7e" },
	{ K_SF01,	"\x1b\x4f\x32\x50" },
	{ K_SF02,	"\x1b\x4f\x32\x51" },
	{ K_SF03,	"\x1b\x4f\x32\x52" },
	{ K_SF04,	"\x1b\x4f\x32\x53" },
	{ K_SF05,	"\x1b\x5b\x31\x35\x3b\x32\x7e" },
	{ K_SF06,	"\x1b\x5b\x31\x37\x3b\x32\x7e" },
	{ K_SF07,	"\x1b\x5b\x31\x38\x3b\x32\x7e" },
	{ K_SF08,	"\x1b\x5b\x31\x39\x3b\x32\x7e" },
	{ K_SF09,	"\x1b\x5b\x32\x30\x3b\x32\x7e" },
	{ K_SF10,	"\x1b\x5b\x32\x31\x3b\x32\x7e" },
	{ K_SF11,	"\x1b\x5b\x32\x33\x3b\x32\x7e" },
	{ K_SF12,	"\x1b\x5b\x32\x34\x3b\x32\x7e" },
	{ K_HOME,	"\x1b\x5b\x48" },
	{ K_END,	"\x1b\x5b\x46" },
//	{ KEY_SDC,	"\x1b\x5b\x33\x3b\x32\x7e" },
//	{ KEY_SHOME,"\x1b\x5b\x32\x48" },
//	{ KEY_SEND,	"\x1b\x5b\x32\x46" },
};

PRIVATE key_code_t input_and_decode_key_sequences(void)
{
	key_code_t key1;
	key_code_t key2;
	char key_sequences[MAX_KEY_SEQ+1];
///	static char key_sequences[MAX_KEY_SEQ+1];
	int key_idx;

again:;
	key_idx = 0;
	if ((key1 = input_key()) < 0) {
		return key1;
	}
	if (key1 == 0x1b) {
		key_sequences[key_idx++] = key1;
		key_sequences[key_idx] = '\0';
		key1 = -1;
		for ( ; key_idx < MAX_KEY_SEQ; ) {
			if ((key2 = input_key()) < 0)
				break;
			key_sequences[key_idx++] = key2;
			key_sequences[key_idx] = '\0';
			if ((key1 = compare_key_seq(key_sequences)) >= 0)
				break;
		}
	}
	if (key1 < 0) {
		if (strlen(key_sequences) > 1)
			goto again;
		key1 = key_sequences[0];
	}
	return key1;
}

PRIVATE key_code_t compare_key_seq(char *key_sequences)
{
	int seq_idx;

	if (strlen(key_sequences) == 2) {
		for (seq_idx = 0; seq_idx < sizeof(key_seq_table) / sizeof(key_seq_table[0]);
		 seq_idx++) {
			if (strncmp(key_sequences, key_seq_table[seq_idx].sequences, 2) == 0)
				return -1;
		}
		return (key_sequences[0] << 8) | key_sequences[1];
	} else if (strlen(key_sequences) >= 3) {
		for (seq_idx = 0; seq_idx < sizeof(key_seq_table) / sizeof(key_seq_table[0]);
		 seq_idx++) {
			if (strcmp(key_sequences, key_seq_table[seq_idx].sequences) == 0)
				return key_seq_table[seq_idx].key;
		}
	}
	return -1;
}

PRIVATE key_code_t input_key(void)
{
	key_code_t key = -1;
	unsigned char buf[1];

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	if (read(STDIN_FILENO, buf, 1) >= 1) {
		key = buf[0];
	}
///	flf_d_printf("getch():%04x %d\n", key, key);
	return key;
}

void investigate_key_sequence(void)
{
	key_code_t key;
	char key_sequences[MAX_KEY_SEQ+1];
///	static char key_sequences[MAX_KEY_SEQ+1];
	int key_idx = 0;
	int keys;
	long start_msec;

	termif_init();
	termif_begin();
	for (key = -1 ; key < 0; ) {
		key = input_key();
	}
	key_sequences[key_idx] = key;
	key_idx++;
	key_sequences[key_idx] = '\0';
	for (start_msec = get_msec(); key_idx < MAX_KEY_SEQ && get_msec() < start_msec + 1000; ) {
		if ((key = input_key()) >= 0) {
			key_sequences[key_idx] = key;
			key_idx++;
			key_sequences[key_idx] = '\0';
		}
	}
	keys = key_idx;
	termif_end();
	printf("\n");
	for (key_idx = 0; key_idx < keys; key_idx++) {
		printf("%02x ", key_sequences[key_idx]);
	}
	printf("\n");
}

#endif // ENABLE_NCURSES

// End of termif.c
/**************************************************************************
 *   tio.c - terminal I/O                                                 *
 *                                                                        *
 *   Copyright (C) 2005-2018 PCR                                          *
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

#include "headers.h"

PRIVATE char tio_blank_line_buf[MAX_SCRN_LINE_BUF_LEN+1] = "";	// A blank line

#ifdef START_UP_TEST
void tio_test(void)
{
	int yy = 0;
	int xx = 0;
	int bgc_fgc = 0;
	key_code_t key;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"
#define BUF_LEN		100
	char buf[BUF_LEN+1];

	tio_init();

	tio_clear_screen();
	tio_set_cursor_on(1);
	tio_set_attrs(-1, -1, 0);
#ifdef ENABLE_NCURSES
	tio_output_string(0, 0, "          curses", -1);
#else // ENABLE_NCURSES
	tio_output_string(0, 0, "          termif", -1);
#endif // ENABLE_NCURSES
	tio_refresh();
	for ( ; ; ) {
		tio_set_cursor_pos(yy, xx);
		for ( ; ; ) {
			key = tio_input_key();
			if (key >= 0)
				break;
			MSLEEP(10);
		}
		switch (key) {
		case 'c':
			tio_clear_screen();
			break;
		case 'o':
			tio_set_cursor_on(1);
			break;
		case 'p':
			tio_set_cursor_on(0);
			break;
		case 'r':
			tio_set_attrs(-1, -1, 1);
			break;
		case 't':
			tio_set_attrs(-1, -1, 0);
			break;
		case 'h':
			tio_set_attrs(-1, -1, -1);
			break;
		case 'j':
			tio_set_attrs(-1, -1, -1);
			break;
		case 'f':
			bgc_fgc = 1;
			break;
		case 'g':
			bgc_fgc = 0;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			if (bgc_fgc == 0) {
				tio_set_attrs(key - '0', -1, -1);
			} else {
				tio_set_attrs(-1, key - '0', -1);
			}
			break;
		case 'b':
			tio_beep();
			break;
		case 's':
			xx--;
			break;
		case 'd':
			xx++;
			break;
		case 'e':
			yy--;
			break;
		case 'x':
			yy++;
			break;
		case 'q':
			break;
		}
		if (key == 'q')
			break;
		snprintf(buf, BUF_LEN+1, "%04x{%s}",
		 key, short_key_name_from_key_code(key, buf_key_name));
		tio_output_string(yy, xx, buf, -1);
	}

	tio_destroy();
_FLF_
	exit(0);
}
#endif // START_UP_TEST

//-----------------------------------------------------------------------------
PRIVATE int tio_initialized = FALSE;
//-----------------------------------------------------------------------------
int tio_is_initialized(void)
{
	return tio_initialized;
}
int tio_init(void)
{
	strnset__(tio_blank_line_buf, ' ', MAX_SCRN_LINE_BUF_LEN);
///	strnset__(tio_blank_line_buf, '_', MAX_SCRN_LINE_BUF_LEN);
#ifdef ENABLE_NCURSES
	flf_d_printf("Using curses (cursesif)\n");
	curses_init();
#else // ENABLE_NCURSES
	flf_d_printf("Direct terminal control (termif)\n");
	termif_init();
#endif // ENABLE_NCURSES
	tio_begin();
	return 0;
}
int tio_destroy(void)
{
	tio_end();
	return 0;
}

int tio_begin(void)
{
#ifdef ENABLE_NCURSES
	curses_begin();
#else // ENABLE_NCURSES
	termif_begin();
#endif // ENABLE_NCURSES
	tio_initialized = TRUE;
	return 0;
}
int tio_end(void)
{
#ifdef ENABLE_NCURSES
	curses_end();
#else // ENABLE_NCURSES
	termif_end();
#endif // ENABLE_NCURSES
	tio_initialized = FALSE;
	return 0;
}

int tio_resize(void)
{
	// update screen size
///	tio_set_screen_size(lines, cols);
	// re-initialize terminal interface to reflect new terminal size
	tio_end();
	tio_begin();
	return 0;
}
int tio_suspend(void)
{
	tio_end();
	return 0;
}
int tio_resume(void)
{
	tio_begin();
	return 0;
}

//-----------------------------------------------------------------------------
///int tio_get_screen_size_from_term(void)
///{
///	int lines = tio_get_lines();
///	int cols = tio_get_columns();
///#ifdef ENABLE_NCURSES
///#else // ENABLE_NCURSES
///	termif_get_screen_size_from_term();
///#endif // ENABLE_NCURSES
///	return (lines != tio_get_lines()) || (cols != tio_get_columns());
///}

int tio_check_update_terminal_size(void)
{
	if (tio_check_terminal_resized()) {
		tio_resize();
		return 1;
	}
	return 0;
}

// How to know win size changed:
//  1. signal sigwinch()
// How to get win size:
//  1. ioctl TIOCGWINSZ
//  2. terminal set ("0x1b[999;999R") and get("\x1b[6n") cursor pos.
int tio_check_terminal_resized(void)
{
	if (is_sigwinch_signaled()) {
		clear_sigwinch_signaled();
///flf_d_printf("cols: %d, lines: %d\n", ioctl_ws_col, ioctl_ws_row);
		return 1;
	}
	return 0;
}

//-----------------------------------------------------------------------------

void tio_set_screen_size(int lines, int columns)
{
#ifdef ENABLE_NCURSES
#define UPDATE_CURSES_COLS_LINES
#ifdef UPDATE_CURSES_COLS_LINES
	curses_set_screen_size(lines, columns);
#endif // UPDATE_CURSES_COLS_LINES
#else // ENABLE_NCURSES
	termif_set_screen_size(lines, columns);
#endif // ENABLE_NCURSES
}
int tio_get_lines(void)
{
#ifdef ENABLE_NCURSES
	return curses_get_lines();
#else // ENABLE_NCURSES
	return termif_get_lines();
#endif // ENABLE_NCURSES
}
int tio_get_columns(void)
{
#ifdef ENABLE_NCURSES
	return curses_get_columns();
#else // ENABLE_NCURSES
	return termif_get_columns();
#endif // ENABLE_NCURSES
}

//-----------------------------------------------------------------------------

void tio_set_attrs(int bgc, int fgc, int rev)
{
#ifdef ENABLE_NCURSES
	curses_set_attrs(bgc, fgc, rev);
#else // ENABLE_NCURSES
	termif_set_attrs(bgc, fgc, rev);
#endif // ENABLE_NCURSES
}
void tio_set_attr_rev(int rev)
{
	tio_set_attrs(-1, -1, rev);
}

void tio_set_cursor_on(int on_off)
{
#ifdef ENABLE_NCURSES
	curses_set_cursor_on(on_off);
#else // ENABLE_NCURSES
	termif_set_cursor_on(on_off);
#endif // ENABLE_NCURSES
}

void tio_set_cursor_pos(int yy, int xx)
{
#ifdef ENABLE_NCURSES
	curses_set_cursor_pos(yy, xx);
#else // ENABLE_NCURSES
	termif_set_cursor_pos(yy, xx);
#endif // ENABLE_NCURSES
}

void tio_clear_flash_screen(int delay)
{
	tio_set_attrs(CL_WH, CL_BK, 1);
	tio_clear_screen();
	MSLEEP(delay);
	tio_set_attrs(CL_WH, CL_BK, 0);
	tio_clear_screen();
}
void tio_clear_screen(void)
{
#if 0
#ifdef ENABLE_NCURSES
	curses_clear_screen();
#else // ENABLE_NCURSES
	termif_clear_screen();
#endif // ENABLE_NCURSES
#else
	tio_clear_lines(0, tio_get_lines());
	tio_refresh();
#endif
}
void tio_clear_lines(int line_1, int line_2)
{
	int yy;

	for (yy = line_1; yy < line_2; yy++) {
		tio_output_string(yy, 0, tio_blank_line(), tio_get_columns());
	}
}
const char *tio_blank_line(void)
{
	return tio_blank_line_buf;
}

void tio_output_string(int yy, int xx, const char *string, int bytes)
{
///flf_d_printf("bytes: %d\n", bytes);
	if (bytes < 0) {
		bytes = byte_idx_from_col_idx(string, tio_get_columns() - xx, CHAR_LEFT, NULL);
	}
///flf_d_printf("bytes: %d\n", bytes);
#ifdef ENABLE_NCURSES
	curses_output_string(yy, xx, string, bytes);
#else // ENABLE_NCURSES
	termif_output_string(yy, xx, string, bytes);
#endif // ENABLE_NCURSES
}

void tio_beep(void)
{
#ifdef ENABLE_NCURSES
	curses_beep();
#else // ENABLE_NCURSES
	termif_beep();
#endif // ENABLE_NCURSES
}

void tio_refresh(void)
{
#ifdef ENABLE_NCURSES
	curses_refresh();
#else // ENABLE_NCURSES
	termif_refresh();
#endif // ENABLE_NCURSES
}
void tio_repaint_all(void)
{
#ifdef ENABLE_NCURSES
	// no function
#else // ENABLE_NCURSES
	termif_clear_vscreen_painted();
#endif // ENABLE_NCURSES
}

key_code_t tio_input_key(void)
{
#ifdef ENABLE_NCURSES
	return curses_input_key();
#else // ENABLE_NCURSES
	return termif_input_key();
#endif // ENABLE_NCURSES
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_term_settings(char *msg, struct termios *term)
{
	char buf[NCCS*3+1];
	int ncc;

	for (ncc = 0; ncc < NCCS; ncc++) {
		snprintf(&buf[ncc * 3], 3+1, " %02x", term->c_cc[ncc]);
	}
	flf_d_printf("term_settings-%s i: %08lx o: %08lx c: %08lx l: %08lx\ncc:%s\n",
	 msg, term->c_iflag, term->c_oflag, term->c_cflag, term->c_lflag, buf);
}
#endif // ENABLE_DEBUG

void save_term_settings(struct termios *term_settings)
{
	/* First back up the old settings so they can be restored, duh */
	tcgetattr(STDIN_FILENO, term_settings);
}
void restore_term_settings(struct termios *term_settings)
{
	/* Restore the old term settings */
	tcsetattr(STDIN_FILENO, TCSANOW, term_settings);
}

// disable program termination by ^C
void set_term_no_intr(void)
{
#ifdef _POSIX_VDISABLE
	struct termios term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_cc[VINTR] = _POSIX_VDISABLE;
	term.c_cc[VQUIT] = _POSIX_VDISABLE;
	term.c_lflag &= ~IEXTEN;
	term.c_cc[VSTOP] = _POSIX_VDISABLE;
	term.c_cc[VSTART] = _POSIX_VDISABLE;
#ifdef VDSUSP
	term.c_cc[VDSUSP] = _POSIX_VDISABLE;
#endif // VDSUSP
	term.c_cc[VSUSP] = _POSIX_VDISABLE;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
#endif // _POSIX_VDISABLE
}

// raw terminal mode (disable local echo, turn off cooked mode)
void set_term_raw(void)
{
	struct termios term;
	struct termios term_raw;

	tcgetattr(STDIN_FILENO, &term);
	cfmakeraw(&term_raw);
	term.c_iflag = term_raw.c_iflag;
///	term.c_oflag = term_raw.c_oflag;
	term.c_cflag = term_raw.c_cflag;
	term.c_lflag = term_raw.c_lflag;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int init_stderr(void)
{
	return dup2(STDOUT_FILENO, STDERR_FILENO);
}

int differ_fgc_to_bgc(int bgc, int fgc)
{
	if (fgc == bgc) {
		// select different color as foreground color with background color
		// so that you can recognize character
		switch(fgc) {
		default:
		case CL_BK:		fgc = CL_DGY;	break;
		case CL_RD:		fgc = CL_LRD;	break;
		case CL_GR:		fgc = CL_LGR;	break;
		case CL_BR:		fgc = CL_YL;	break;
		case CL_BL:		fgc = CL_LBL;	break;
		case CL_MG:		fgc = CL_LMG;	break;
		case CL_CY:		fgc = CL_LCY;	break;
		case CL_GY:		fgc = CL_DGY;	break;
		case CL_DGY:	fgc = CL_BK;	break;
		case CL_LRD:	fgc = CL_RD;	break;
		case CL_LGR:	fgc = CL_GR;	break;
		case CL_YL:		fgc = CL_BR;	break;
		case CL_LBL:	fgc = CL_BL;	break;
		case CL_LMG:	fgc = CL_MG;	break;
		case CL_LCY:	fgc = CL_CY;	break;
///		case CL_WH:		fgc = CL_GY;	break;
		case CL_WH:		fgc = CL_DGY;	break;
		}
	}
#if 1 // Avoid similar fgc/bgc
	if ((bgc == CL_CY) && (fgc == CL_GR)) {
		fgc = CL_LGR;
	}
	if ((bgc == CL_GR) && (fgc == CL_CY)) {
		fgc = CL_LCY;
	}
	if ((bgc == CL_BR) && (fgc == CL_DGY)) {
		fgc = CL_GY;
	}
#endif
	return fgc;
}

// End of tio.c
/**************************************************************************
 *   utf8.c                                                               *
 *                                                                        *
 *   Copyright (C) 2005-2006 PCR                                          *
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

#include "utilincs.h"

#ifdef START_UP_TEST
void test_utf8c_encode(void)
{
	wchar_t wc;
	char utf8c[MAX_UTF8C_BYTES+1];
#ifdef ENABLE_DEBUG
	char buf[DUMP_UTF8_BUF_LEN+1];
#endif // ENABLE_DEBUG

	for (wc = 0x0000; wc < 0xffff; wc++) {
		utf8c_encode(wc, utf8c);
		flf_d_printf("%04x ==> [%s]\n", wc, dump_utf8c(utf8c, buf));
	}
}
const char *dump_utf8c(const char *utf8c, char *buf)
{
	int idx;

	strcpy(buf, "");
	for (idx = 0; idx < strlen(utf8c); idx++) {
		strcat_printf(buf, DUMP_UTF8_BUF_LEN+1, "%02x-", (unsigned char)utf8c[idx]);
	}
	return buf;
}

void test_utf8c_bytes(void)
{
	const char str[] = "\xed\xa0\x80\xed\xa0\x81 \xed\xa0\xa0\xed\xa0\xa1";
	const char *ptr;

	for (ptr = str; *ptr; ) {
		flf_d_printf("%d\n", ptr - str);
		ptr += utf8c_bytes(ptr);
	}
}
void test_wcwidth(void)
{
	wchar_t wc;
#ifdef ENABLE_DEBUG
	int columns;
#endif // ENABLE_DEBUG

	for (wc = 0x0000; wc < 0xffff; wc++) {
		columns = my_wcwidth(wc);
		if ((wc & 0x1f) == 0) {
			flf_d_printf("%04x", wc);
		}
		d_printf(" %d", columns);
		if ((wc & 0x1f) == 0x1f) {
			d_printf("\n");
		}
	}
	d_printf("\n");
}
#endif // START_UP_TEST

// UTF-8 support functions ----------------------------------------------------

int utf8s_chars(const char *utf8s)
{
	int chars;

	for (chars = 0; *utf8s; chars++) {
		utf8s += utf8c_bytes(utf8s);
	}
	return chars;
}

int utf8s_columns(const char *utf8s, int bytes)
{
	const char *ptr;
	int columns;

	for (ptr = utf8s, columns = 0; *ptr && ptr - utf8s < bytes; ) {
		columns += utf8c_columns(ptr);
		ptr += utf8c_bytes(ptr);
	}
	return columns;
}

int utf8c_bytes(const char *utf8s)
{
#ifndef ENABLE_UTF8
	return 1;
#else // ENABLE_UTF8
	return my_mblen(utf8s, MAX_UTF8C_BYTES);
#endif // ENABLE_UTF8
}

int utf8c_len(char utf8c_state, char utf8c)
{
	if (utf8c_state <= 0) {
		if ((unsigned char)utf8c < 0x80) {	// 0xxxxxxx
			utf8c_state = 0;
		} else
		if ((utf8c & 0xe0) == 0xc0) {		// 110xxxxx-10xxxxxx
			utf8c_state = 1;
		} else
		if ((utf8c & 0xf0) == 0xe0) {		// 1110xxxx-10xxxxxx-10xxxxxx
			utf8c_state = 2;
		} else
		if ((utf8c & 0xf8) == 0xf0) {		// 11110xxx-10xxxxxx-10xxxxxx-10xxxxxx
			utf8c_state = 3;
		} else {							// 11111xxx
			utf8c_state = 0;
		}
	} else {
		if ((unsigned char)utf8c < 0x80) {	// 0xxxxxxx
			utf8c_state = 0;
		} else
		if ((utf8c & 0xc0) == 0x80) {		// 10xxxxxx
			utf8c_state--;
		} else {							// 11xxxxxx
			utf8c_state = 0;
		}
	}
	return utf8c_state;				// return remaining bytes
}

int utf8c_prev_bytes(const char *utf8s_min, const char *utf8s)
{
#ifndef ENABLE_UTF8
	return (utf8s_min < utf8s) ? 1 : 0;
#else // ENABLE_UTF8
	int bytes;
	const char *ptr;

	for (ptr = utf8s; utf8s_min < ptr && utf8s - ptr < MAX_UTF8C_BYTES; ) {
		ptr--;
		if (*(unsigned char *)ptr < 0x80 || 0xc0 <= *(unsigned char *)ptr)
			// ascii(0xxxxxxx) or first byte of UTF8(11xxxxxx)
			break;
	}
	bytes = utf8s - ptr;
///	if (bytes < 1)
///		bytes = 1;
	return bytes;
#endif // ENABLE_UTF8
}

int utf8c_columns(const char *utf8c)
{
#ifndef ENABLE_UTF8
	return *utf8c ? 1 : 0;
#else // ENABLE_UTF8
	return my_mbwidth(utf8c, MAX_UTF8C_BYTES);
#endif // ENABLE_UTF8
}

wchar_t utf8c_decode(const char *utf8c)
{
#ifndef ENABLE_UTF8
	return utf8c[0];
#else // ENABLE_UTF8
	wchar_t wc;

	wc = my_mbtowc(utf8c, MAX_UTF8C_BYTES);
	return wc;
#endif // ENABLE_UTF8
}

int utf8c_encode(wchar_t wc, char *utf8c)
{
#ifndef ENABLE_UTF8
	utf8c[0] = wc;
	utf8c[1] = '\0';
	return 1;
#else // ENABLE_UTF8
	int bytes;

	bytes = wctomb(utf8c, wc);
	utf8c[bytes] = '\0';
	return bytes;
#endif // ENABLE_UTF8
}

// End of utf8.c
/**************************************************************************
 *   utilpath.c                                                           *
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

#include "utilincs.h"

/* Strip one dir from the end of the string */
// /dir1/dir2/file ==> /dir1/dir2
// /dir1/dir2/ ==> /dir1/dir2
// /dir1 ==> /
///char *strip_one_dir(char *path, char *dir)
///{
///	char *ptr;
///
//////flf_d_printf("strip_one_dir(%s)\n", path);
///	if (strcmp(path, "/") != 0 && strlen(path)) {
///		if (path[strlen(path)-1] == '/')
///			// "/dir1/dir2/" ==> "/dir1/dir2"
///			path[strlen(path)-1] = '\0';
///	}
///	ptr = separate_dir_and_file(path, dir);
//////flf_d_printf(" ==> return:[%s], path:[%s], dir:[%s]\n", ptr, path, dir);
///	return ptr;							// return stripped dir or file
///}
// /dir1/dir2/file ==> /dir1/dir2
char *strip_file_from_path(const char *path)
{
	static char dir[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];

	strlcpy__(dir, path, MAX_PATH_LEN);
	separate_dir_and_file(dir, file);
	return dir;
}
// /dir1/dir2/file ==> /dir1/dir2  file
// /dir1/dir2 ==> /dir1  dir2
// /file      ==> /  file
// file       ==> /  file
// /          ==> /  ""
// ""         ==> /  ""
char *separate_dir_and_file(char *path, char *file)
{
	char *ptr;

	ptr = get_last_slash(path);
	if (*ptr == '/') {
		*ptr++ = '\0';	// NUL terminate ("/dir/file" ==> "/dir")
		strlcpy__(file, ptr, MAX_PATH_LEN);	// "file"
		normalize_root_dir(path);			// "" ==> "/"
	} else {
		strcpy__(path, "/");				// "file" ==> "/"
		strlcpy__(file, ptr, MAX_PATH_LEN);	// "file" ==> "file"
	}
///flf_d_printf("separate_dir_and_file(%s)\n", path);
///flf_d_printf(" ==> dir/file:[%s]/[%s]\n", dir, file);
	return file;		// return file
}

// "" ==> "/"
char *normalize_root_dir(char *dir)
{
	if (dir[0] == '\0')
		strcpy__(dir, "/");
	return dir;
}

///// point to the last part of a path
///// /dir1/dir2/file ==> file
///char *get_last_part_of_path(char *path);
///char *get_last_part_of_path(char *path)
///{
///	char *ptr;
///
///	ptr = get_last_slash(path);
///	if (*ptr == '/')
///		ptr++;
///	return ptr;
///}

// /dir1/dir2/file ==> /dir1/dir2
// /file           ==> /file
// file            ==> file
char *remove_last_slash(char *path)
{
	char *ptr;

	ptr = get_last_slash(path);
	if (ptr) {
		if (path < ptr && ptr[0] == '/' && ptr[1] == '\0')
			*ptr = '\0';
	}
	return path;
}

// point to the last '/'
// /dir1/dir2/file    file
//           ^        ^
char *get_last_slash(char *path)
{
	char *ptr;

	ptr = strrchr(path, '/');
	if (ptr == NULL)
		ptr = path;
	return ptr;
}
////int is_path_only_file_name(const char *path)
////{
////	return strchr(path, '/') == NULL;
////}

int is_path_exist(const char *path)
{
	struct stat st;

	return stat(path, &st) == 0;
}
int is_path_regular_file(const char *path)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return -1;				// no such file nor directory
	return S_ISREG(st.st_mode);	// 1:file, 0:non-file
}
int is_path_dir(const char *path)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return -1;				// no such file nor directory
	return S_ISDIR(st.st_mode);	// 1:directory, 0:non-directory
}
int is_file_writable(const char *path)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return -1;
	return is_st_writable(&st);
}
int is_st_writable(struct stat *st)
{
	if (st->st_uid == geteuid()) {
		return (st->st_mode & S_IWUSR) != 0;
	} else if (st->st_gid == getegid()) {
		return (st->st_mode & S_IWGRP) != 0;
	}
	return (st->st_mode & S_IWOTH) != 0;
}

#ifdef ENABLE_FILER
int is_dir_readable(const char *path)
{
	DIR *dir;

////flf_d_printf("path: %s\n", path);
	dir = opendir(path);
	// If dir is NULL, don't call closedir()
	if (dir)
		closedir(dir);
	return dir != NULL;
}

int is_path_wildcard(char *path)
{
#ifdef ENABLE_DEBUG
	char *ptr;

	ptr = strchr(path, '*');
	if (ptr)
		flf_d_printf("strchr(path, '*') ==> [%s]\n", ptr);
	ptr = strchr(path, '?');
	if (ptr)
		flf_d_printf("strchr(path, '?') ==> [%s]\n", ptr);
#endif // ENABLE_DEBUG
	return strchr(path, '*') || strchr(path, '?');
}
#endif // ENABLE_FILER

char *separate_dir_part_and_file_part(const char *path,
 char *dir_part, char *file_part)
{
	struct stat fileinfo;
	char *last_slash;

	if (stat(path, &fileinfo) == 0 && S_ISDIR(fileinfo.st_mode)) {
		// /dir1/dir2
		strlcpy__(dir_part, path, MAX_PATH_LEN);
		strcpy__(file_part, "");
	} else {
		// /dir1/dir2/filename or filename only
		last_slash = strrchr(path, '/');
		if (last_slash != NULL) {
			// /dir1/dir2/filename
			strlcpy__(dir_part, path, last_slash - path);
			strlcpy__(file_part, last_slash+1, MAX_PATH_LEN);
		} else {
			// filename only
			strcpy__(dir_part, "");
			strlcpy__(file_part, path, MAX_PATH_LEN);
		}
	}
	return dir_part;
}

char *get_home_dir(void)
{
	char *env_home;
	const struct passwd *userdata = 0;
	static char buffer[MAX_PATH_LEN+1];

	if (strlen(env_home = getenv__("HOME"))) {
		strlcpy__(buffer, env_home, MAX_PATH_LEN);
		return buffer;
	}
	if ((userdata = getpwuid(geteuid())) != NULL) {
		strlcpy__(buffer, userdata->pw_dir, MAX_PATH_LEN);
		return buffer;
	}
	strcpy__(buffer, "/");
	return buffer;
}

//------------------------------------------------------------------------------
const char *get_start_dir(void)
{
	static char start_dir[MAX_PATH_LEN+1] = "";

	if (strcmp(start_dir, "") == 0) {
		if (strlen(get_cwd(start_dir)) == 0) {
			getenv_pwd(start_dir);
		}
	}
	return start_dir;
}
int save_change_cur_dir(char *dir_save, const char *dir)
{
	get_cur_dir(dir_save);
	return change_cur_dir(dir);
}
PRIVATE char current_directory[MAX_PATH_LEN+1] = "";
char *get_cur_dir(char *dir)
{
	if (strcmp(current_directory, "") == 0) {
		if (strlen(get_cwd(current_directory)) == 0) {
			getenv_pwd(current_directory);
		}
	}
	strlcpy__(dir, current_directory, MAX_PATH_LEN);
	return dir;
}
int change_cur_dir(const char *dir)
{
	int ret;

	if ((ret = chdir(dir)) == 0) {
		strlcpy__(current_directory, dir, MAX_PATH_LEN);
	}
	return ret;
}
//------------------------------------------------------------------------------

// get real current directory(symbolic link is expanded to absolute path)
char *get_cwd(char *cwd)
{
	if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
		strcpy__(cwd, "");
	}
	return cwd;
}

char *getenv_pwd(char *cwd)
{
	strlcpy__(cwd, getenv__("PWD"), MAX_PATH_LEN);
	return cwd;
}

char *getenv__(char *env)
{
	char *ptr;

	if ((ptr = getenv(env)) == NULL) {
		ptr = "";
	}
	return ptr;
}

char *add_last_slash_to_dir(char *dir)
{
	if (strcmp(dir, "/") != 0 && strlen(dir) && dir[strlen(dir) - 1] != '/') {
		strlcat__(dir, MAX_PATH_LEN, "/");
	}
	return dir;
}

// Concatenate path and file
// /dir1/dir2  file ==> /dir1/dir2/file
// /dir1/dir2/  file ==> /dir1/dir2/file
// dir1/dir2  file ==> dir1/dir2/file
// ""  file ==> file
char *cat_dir_and_file(char *buf, int buf_len, const char *dir, const char *file)
{
	int last;

	last = LIM_MIN(0, strnlen(dir, MAX_PATH_LEN) - 1);
	if (file[0] == '/') {
		// "/file" ==> "file"
		file++;
	}
	if (is_strlen_0(dir) || dir[last] == '/') {
///flf_d_printf("dir file [%s %s]\n", dir, file);
		snprintf(buf, buf_len+1, "%s%s", dir, file);
	} else {
///flf_d_printf("dir / file [%s / %s]\n", dir, file);
		snprintf(buf, buf_len+1, "%s/%s", dir, file);
	}
	return buf;
}

// read link and null terminate string
int readlink__(const char *path, char *buffer, int len)
{
	int ret;

	if ((ret = readlink(path, buffer, len)) > 0)
		buffer[ret] = '\0';
	return ret;
}

//-----------------------------------------------------------------------------------
// NOTE: Meaning of file_name, file_path, full_path and abs_path(real_path)
//  file_name          : filename.ext
//  file_path          : ../src/filename.ext
//  full_path          : /home/user/symlink/../src/filename.ext
//						 start by '/' and may contain symlinks and ".."
//  abs_path(real_path): /home/user/tools/src/filename.ext
//						 start by '/' and may not contain symlinks and ".."
//-----------------------------------------------------------------------------------

#ifdef START_UP_TEST
// /aaa/bbb/.. ==> /aaa
// /aaa/bbb/../ccc ==> /aaa/ccc
// /aaa/bbb/../../ccc ==> /ccc
// /../ccc ==> error
// /aaa/bbb/. ==> /aaa/bbb
// /aaa/bbb/./ccc ==> /aaa/bbb/ccc
// /. ==> /
PRIVATE void test_normalize_full_path_(const char *abs_path);
PRIVATE void test_normalize_full_path__(const char *abs_path);
int test_normalize_full_path(void)
{
	flf_d_printf("-----------------------\n");
	test_normalize_full_path_(".");
	test_normalize_full_path_("././.");
	test_normalize_full_path_("..");
	test_normalize_full_path_("../../..");
	test_normalize_full_path_("aaa/bbb/..");
	test_normalize_full_path_("aaa/bbb/../ccc");
	test_normalize_full_path_("aaa/bbb/../../ccc");
	test_normalize_full_path_("aaa/bbb/ccc/../../ddd");
	test_normalize_full_path_("aaa/bbb/ccc/../../../ddd");
	test_normalize_full_path_("../ccc");
	test_normalize_full_path_("../../../ccc");
	test_normalize_full_path_("../aaa/bbb/..");
	test_normalize_full_path_("aaa/bbb/.");
	test_normalize_full_path_("aaa/bbb/./ccc");
	test_normalize_full_path_(".");
	test_normalize_full_path_("./aaa/bbb/.");
	return 0;
}
PRIVATE void test_normalize_full_path_(const char *abs_path)
{
	char buffer[MAX_PATH_LEN+1];

	snprintf(buffer, MAX_PATH_LEN, "%s", abs_path);
	test_normalize_full_path__(buffer);
	snprintf(buffer, MAX_PATH_LEN, "/%s", abs_path);
	test_normalize_full_path__(buffer);
	snprintf(buffer, MAX_PATH_LEN, "%s/", abs_path);
	test_normalize_full_path__(buffer);
	snprintf(buffer, MAX_PATH_LEN, "/%s/", abs_path);
	test_normalize_full_path__(buffer);
}
PRIVATE void test_normalize_full_path__(const char *abs_path)
{
	char buffer[MAX_PATH_LEN+1];

	strlcpy__(buffer, abs_path, MAX_PATH_LEN);
	normalize_full_path(buffer);
	flf_d_printf("[%s] ==> [%s]\n", abs_path, buffer);
}
#endif // START_UP_TEST

// /dir1/.  ==> /dir1/
// /dir1/.. ==> /
// /..      ==> /
// /dir1/./dir2  ==> /dir1/dir2
// /dir1/../dir2 ==> /dir2
// /dir1/dir2/.. ==> /dir1/
// /dir1/dir2/dir3/../../.. ==> /
PRIVATE char *normalize_full_path__(char *abs_path, char *parent, char *child);
char *normalize_full_path(char *abs_path)
{
	// "/dir1/../????"
	//  ^
	normalize_full_path__(abs_path, abs_path, abs_path);
	remove_last_slash(abs_path);
	return abs_path;
}
// "/dir1/../????"
//   ^   ^
PRIVATE char *normalize_full_path__(char *abs_path, char *parent, char *child)
{
	char *grandchild;

////flf_d_printf(">[%s], [%s], [%s]\n", abs_path, parent, child);
	if (*parent == '/')
		parent++;
	if (*child == '/')
		child++;
	for ( ; is_file_path_char(child); ) {
////flf_d_printf("=[%s], [%s], [%s]\n", abs_path, parent, child);
		if (strcmp(child, ".") == 0) {
			// "/dir1/." ==> "/dir1/"
			// "/."      ==> "/"
			strlcpy__(child, child+1, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
		} else
		if (strlcmp__(child, "./") == 0) {
			// "/dir1/./????" ==> "/dir1/????"
			// "/./????"      ==> "/????"
			strlcpy__(child, child+2, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
			child = parent;
		} else
		if (strcmp(child, "..") == 0) {
			// "/dir1/.." ==> "/"
			// "/.."      ==> "/"
			strlcpy__(parent, child+2, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
			child = parent;
		} else
		if (strlcmp__(child, "../") == 0) {
			// "/dir1/../????" ==> "/????"
			// "/../????"      ==> "/????"
			strlcpy__(parent, child+3, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
			child = parent;
			if (abs_path+1 < child) {
				// not string top
				break;
			}
		} else {
			grandchild = skip_file_name(child);
			if (is_file_path_char(grandchild) == 0) {
				child = grandchild;
				break;
			}
			if (*grandchild == '/') {
				// "/dir1/../????"
				//   ^   ^
				child = normalize_full_path__(abs_path, child, grandchild);	// recursive call
////flf_d_printf("-[%s], [%s], [%s]\n", abs_path, parent, child);
			}
		}
	}
////flf_d_printf("<[%s]\n", child);
	return child;
}

//-----------------------------------------------------------------------------------

// get absolute path (not include symlinks)
PRIVATE char *get_full_path(const char *path, char *buf);

#ifdef START_UP_TEST
PRIVATE void test_get_full_path_(const char *path);
void test_get_full_path(void)
{
	flf_d_printf("-----------------------\n");
	test_get_full_path_("~");
	test_get_full_path_("~user");
	test_get_full_path_("~root");

	test_get_full_path_("~/abc");
	test_get_full_path_("~user/abc");
	test_get_full_path_("~root/abc");

	test_get_full_path_("abc");
	test_get_full_path_("./abc");
	test_get_full_path_("../abc");

	test_get_full_path_("aaa/./bbb");
	test_get_full_path_("./aaa/./bbb");
	test_get_full_path_("../aaa/./bbb");

	test_get_full_path_("aaa/../bbb");
	test_get_full_path_("./aaa/../bbb");
	test_get_full_path_("../aaa/../bbb");

	test_get_full_path_(" abc ");	// filename includes space in head or tail
}
PRIVATE void test_get_full_path_(const char *path)
{
	char full_path[MAX_PATH_LEN+1];

	get_full_path(path, full_path);
	flf_d_printf("path:[%s] ==> full_path:[%s]\n", path, full_path);
}
#endif // START_UP_TEST

//-----------------------------------------------------------------------------------

char *get_abs_path(const char *path, char *buf)
{
	char full_path[MAX_PATH_LEN+1];

	get_full_path(path, full_path);
	get_real_path(full_path, buf, MAX_PATH_LEN);
	return buf;
}

char *get_real_path(const char *path, char *buf, int buf_len)
{
#if defined(HAVE_REALPATH)
	return realpath__(path, buf, buf_len);
#else // HAVE_REALPATH
	return my_realpath(path, buf, buf_len);
#endif // HAVE_REALPATH
}

#if defined(HAVE_REALPATH)
#ifdef START_UP_TEST
PRIVATE void test_realpath_(const char *path);
void test_realpath(void)
{
	flf_d_printf("-----------------------\n");
	test_realpath_("~");
	test_realpath_("~user");
	test_realpath_("~root");

	test_realpath_("/home/user/tools/be/src");
	test_realpath_("/home/user/tools/./be/src");
	test_realpath_("/home/user/../user/tools/be/src");

	test_realpath_("/dev/stdin");
	test_realpath_("/dev/fd");
}
PRIVATE void test_realpath_(const char *path)
{
	char buf[MAX_PATH_LEN+1];

	realpath__(path, buf, MAX_PATH_LEN);
	flf_d_printf("path:[%s] ==> buf:[%s]\n", path, buf);
}
#endif // START_UP_TEST
// return normalized(canonicalized) absolute file path
char *realpath__(const char *path, char *buf, int buf_len)
{
	char buffer[MAX_PATH_LEN+1];

	if (realpath(path, buffer) == NULL) {
		strlcpy__(buf, path, buf_len);	// error, return original path
	} else {
		strlcpy__(buf, buffer, buf_len);
	}
////flf_d_printf("realpath__([%s]) ==> [%s]\n", path, buf);
	return buf;
}
#else // HAVE_REALPATH
char *my_realpath(const char *path, char *buf, int buf_len)
{
	strlcpy__(buf, path, buf_len);
	return normalize_full_path(buf);
}
#endif // HAVE_REALPATH

// get full path (path begins from "/") but do not resolve symlink
//                                            (may contain symlink)
// ~     ==> /home/user
// ~user ==> /home/user
// ~/tools/src/filename.ext     ==> /home/user/tools/src/filename.ext
// ~user/tools/src/filename.ext ==> /home/user/tools/src/filename.ext
// ./filename.ext               ==> /home/user/tools/src/filename.ext
PRIVATE char *get_full_path(const char *path, char *buf)
{
	char username[MAX_PATH_LEN+1];
	const char *userdir;
	char cur_dir[MAX_PATH_LEN+1];
	size_t len;
	const struct passwd *userdata = 0;

	strlcpy__(buf, path, MAX_PATH_LEN);
	if (path[0] == '/') {
		// "/..." already full path
	} else if (path[0] == '~') {
		// "~", "~user", "~/..." or "~user/..."
		for (len = 0; path[len]; len++) {
			if (path[len] == '/') {
				break;
			}
		}
		// Determine home directory using getpwuid() or getpwent(), don't rely on $HOME
		if (len == 1) {		// "~"
			userdata = getpwuid(geteuid());
			userdir = userdata->pw_dir;
		} else {			// "~user"
			strlcpy__(username, &path[1], len);
			userdir = get_user_home_dir(username);
///flf_d_printf("[%s] ==> [%s]\n", username, userdir);
		}
		if (path[len] == '/') {
			// "~/..."   "~/..."
			//   ^    ==>   ^
			len++;
		}
		cat_dir_and_file(buf, MAX_PATH_LEN, userdir, &path[len]);
	} else {
		// "./..." ==> "/home/user/tools/..."
		get_cur_dir(cur_dir);
		cat_dir_and_file(buf, MAX_PATH_LEN, cur_dir, path);
	}
///flf_d_printf("[%s] ==> [%s]\n", path, buf);
	normalize_full_path(buf);
///flf_d_printf("[%s] ==> [%s]\n", path, buf);
	return buf;
}

#ifdef START_UP_TEST
PRIVATE void get_file_name_extension_(char *file_name);
void test_get_file_name_extension(void)
{
	flf_d_printf("-----------------------\n");
	get_file_name_extension_("filename");
	get_file_name_extension_(".filename");
	get_file_name_extension_(".");
	get_file_name_extension_("filename.");
	get_file_name_extension_("filename.ext");
	get_file_name_extension_(".filename.ext");
}
PRIVATE void get_file_name_extension_(char *file_name)
{
	flf_d_printf("[%s] ==> [%s]\n", file_name, get_file_name_extension(file_name));
}
#endif // START_UP_TEST
// "filename.ext" ==> "ext"
// "filename" ==> ""
// "." ==> ""
// "filename." ==> ""
// ".filename" ==> "filename"
// ".filename.ext" ==> "ext"
char *get_file_name_extension(char *file_name)
{
	char *last_period = NULL;
	char *ptr;

	ptr = file_name;
	if (*ptr == '.') {
		// "." ==> ""
		// ".filename" ==> "filename"
		ptr++;
	}
	for ( ; *ptr; ptr++) {
		if (*ptr == '.') {
			last_period = ptr;
		}
	}
	if (last_period == NULL) {
		last_period = ptr;
	}
	if (*last_period == '.') {
		last_period++;
	}
	return last_period;
}

// End of utilpath.c
/**************************************************************************
 *   utils.c                                                              *
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

#include "utilincs.h"

PRIVATE void (*die_on_callback)(const char *msg) = NULL;
void set_die_on_callback(void (*die_on_cb)(const char *msg))
{
	die_on_callback = die_on_cb;
}
void die_on(const char *msg)
{
	if (die_on_callback) {
		(*die_on_callback)(msg);
		// never returns
	}
	exit(255);
}

// Copy one malloc()ed string to another pointer.
// Should be used as: dest = malloc_strcpy(string);
char *malloc_strcpy(const char *string)
{
	char *buf;

	buf = char_malloc(strlen(string) + 1);
	strcpy__(buf, string);
	return buf;
}

// Thanks BG, many ppl have been asking for this...
void *malloc__(size_t bytes)
{
	void *ptr;

	ptr = malloc(bytes);
	if (ptr == NULL && bytes != 0) {
		_FATALERR_
		die_on(_("Out of memory!"));
	}
	return ptr;
}

void *remalloc__(void *ptr, size_t bytes)
{
	ptr = realloc(ptr, bytes);

	if (ptr == NULL && bytes != 0) {
		_FATALERR_
		die_on(_("Out of memory!"));
	}
	return ptr;
}

void free_ptr(void **ptr)
{
	free__(*ptr);
	*ptr = NULL;
}

void free__(void *ptr)
{
	if (ptr) {
		free(ptr);
	}
}

//-----------------------------------------------------------------------------

// remalloc memory just enough for a string
// NOTE: DONOT use for be_line_t::data
void remalloc_and_null_terminate_string(char **ptr)
{
	remalloc_and_null_terminate_length(ptr, strlen(*ptr));
}
// remalloc memory for the length of string
// NOTE: DONOT use for be_line_t::data
void remalloc_and_null_terminate_length(char **ptr, size_t len)
{
	*ptr = char_remalloc(*ptr, len + 1);
	(*ptr)[len] = '\0';
}

//=============================================================================

#define POLYNOMIAL_CRC16CCITT	0x1021
unsigned short crc16ccitt = 0xffff;

unsigned short clear_crc16ccitt(void)
{
	crc16ccitt = 0xffff;
	return crc16ccitt;
}

unsigned short calc_crc16ccitt(unsigned char byte)
{
	int cnt;
	int do_xor;

	for (cnt = 0; cnt < 8; cnt++) {
		if (crc16ccitt & 0x8000)
			do_xor = 1;
		else
			do_xor = 0;
		crc16ccitt <<= 1;
		if (byte & 0x80)
			crc16ccitt += 1;
		if (do_xor)
			crc16ccitt ^= POLYNOMIAL_CRC16CCITT;
		byte <<= 1;
	}
	return crc16ccitt;
}
//-----------------------------------------------------------------------------
const char *cur_ctime(void)
{
	time_t cur_time;
#define HHCMMCSS_LEN		8	// "23:59:59"
	static char buf_time[HHCMMCSS_LEN+1];

	cur_time = time(NULL);
	strlcpy__(buf_time, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	return buf_time;
}
//-----------------------------------------------------------------------------
char *get_ssspuuuuuu(char *buf)
{
	long usec;

	usec = get_usec();
	snprintf_(buf, 10+1, "%03d.%06d",
	 (int)(usec / 1000000), (int)(usec % 1000000));
	return buf;
}
char *get_sssssspmmm(char *buf)
{
	long msec;

	msec = get_msec();
	snprintf_(buf, 10+1, "%06d.%03d",
	 (int)(msec / 1000), (int)(msec % 1000));
	return buf;
}
unsigned long get_msec(void)
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	// 999999.999
	return (tv.tv_sec % 1000000) * 1000 + (tv.tv_usec / 1000) % 1000;
}
unsigned long get_usec(void)
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	// 999.999999
	return (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;
}
const char *cur_hhcmmcss_mmm(void)
{
#define HHCMMCSS_LEN		8	// "23:59:59"
	struct timeval tv;
	struct timezone tz;
	time_t cur_time;
	char hhcmmcss[HHCMMCSS_LEN+1];
#define HHCMMCSS_MMM_LEN		(8+1+3)
	static char buf_time[HHCMMCSS_MMM_LEN+1];

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec;
	strlcpy__(hhcmmcss, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	snprintf_(buf_time, HHCMMCSS_MMM_LEN+1, "%s.%03d", hhcmmcss, (int)(tv.tv_usec / 1000 % 1000));
	return buf_time;
}
const char *cur_hhcmmcss_uuuuuu(void)
{
#define HHCMMCSS_LEN		8	// "23:59:59"
	struct timeval tv;
	struct timezone tz;
	time_t cur_time;
	char hhcmmcss[HHCMMCSS_LEN+1];
#define HHCMMCSS_UUUUUU_LEN		(8+1+6)
	static char buf_time[HHCMMCSS_UUUUUU_LEN+1];

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec;
	strlcpy__(hhcmmcss, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	snprintf_(buf_time, HHCMMCSS_UUUUUU_LEN+1, "%s.%06d", hhcmmcss, (int)tv.tv_usec);
	return buf_time;
}

//-----------------------------------------------------------------------------------

int int_min(int aa, int bb)
{
	return aa <= bb ? aa : bb;
}
int int_max(int aa, int bb)
{
	return aa >= bb ? aa : bb;
}

//-----------------------------------------------------------------------------------

// /proc/meminfo
// MemTotal:        3917308 kB
// MemFree:         2777988 kB
int get_mem_free_in_kb(int update)
{
	FILE *fp;
	char buffer[100+1];
	char buf[100+1];
	static int kb = 256 * 1024;		// 256 MB

	if (update) {
///_TFLF_
		if ((fp = fopen("/proc/meminfo", "r")) != NULL) {
			while (fgets(buffer, 100, fp) != 0) {
///flf_d_printf("%s", buffer);
				if (strncmp(buffer, "MemFree:", 8) == 0) {
					if (sscanf(buffer, "%10s %d", buf, &kb) >= 2) {
						break;
					}
				}
			}
		}
		if (fclose(fp) != 0) {
			// error
		}
///_TFLF_
	}

#ifdef ENABLE_DEBUG
///#define DEBUG_MEM_SHORTAGE
#ifdef DEBUG_MEM_SHORTAGE
	kb /= 100;		// 1GB ==> 10MB
#endif // DEBUG_MEM_SHORTAGE
#endif // ENABLE_DEBUG

///flf_d_printf("MemFree:%d\n", kb);
	return kb;
}

//-----------------------------------------------------------------------------------

#ifdef START_UP_TEST
void test_nn_from_num(void)
{
	int num;
#ifdef ENABLE_DEBUG
	char buf[2+1];
#endif // ENABLE_DEBUG

	for (num = -20; num < 260 + 260 + 100 + 20; num++) {
		flf_d_printf("%d ==> [%s]\n", num, nn_from_num(num, buf));
	}
}
#endif // START_UP_TEST
char *nn_from_num(int num, char *buf)
{
	if (num < 0) {
		snprintf_(buf, 2+1, "-%d", (-num) % 10);	// -9 -- -1
	} else if (num < 100) {
		snprintf_(buf, 2+1, "%02d", num);	// 00 -- 99
	} else if (num < 260 + 100) {
		num -= 100;
		snprintf_(buf, 2+1, "%c%d", 'A' + num / 10, num % 10);	// a0 -- z9
	} else if (num < 260 + 260 + 100) {
		num -= (260 + 100);
		snprintf_(buf, 2+1, "%d%c", num / 26, 'A' + num % 26);	// 0a -- 9z
	} else {
		snprintf_(buf, 2+1, "%c%d", '@', num % 10);
	}
	return buf;
}

//-----------------------------------------------------------------------------

#define USR_NAME_LEN	8
#define GRP_NAME_LEN	8
#define MAX_USERS	200
#define MAX_GROUPS	100

static struct uid_name {
	uid_t uid;
	char username[USR_NAME_LEN+1];
	char homedir[MAX_PATH_LEN+1];
} uid_name_cache[MAX_USERS];
static int num_users = -1;

static struct gid_name {
	gid_t gid;
	char grpname[GRP_NAME_LEN+1];
} gid_name_cache[MAX_GROUPS];
static int num_groups = -1;

const char *get_user_name(uid_t uid)
{
#define USER_ID_LEN		8
	static char user_id[USER_ID_LEN+1];
	int idx;

	if (num_users < 0) {
		cache_users();
	}
	for (idx = 0; idx < num_users; idx++) {
		if (uid == uid_name_cache[idx].uid)
			return uid_name_cache[idx].username;
	}
	snprintf_(user_id, USER_ID_LEN+1, "%d", uid);
	return user_id;
}
const char *get_user_home_dir(const char *username)
{
	int idx;

	for (idx = 0; idx < num_users; idx++) {
		if (strlcmp__(username, uid_name_cache[idx].username) == 0) {
			return uid_name_cache[idx].homedir;
		}
	}
	return username;
}
const char *get_group_name(gid_t gid)
{
	static char group_id[USER_ID_LEN+1];
	int idx;

	if (num_groups < 0) {
		cache_groups();
	}
	for (idx = 0; idx < num_groups; idx++) {
		if (gid == gid_name_cache[idx].gid)
			return gid_name_cache[idx].grpname;
	}
	snprintf_(group_id, USER_ID_LEN+1, "%d", gid);
	return group_id;
}
int cache_users(void)
{
	struct passwd *pwent;

	// cache user name
	setpwent();
	for (num_users = 0; num_users < MAX_USERS; num_users++) {
		if ((pwent = getpwent()) == NULL)
			break;
		uid_name_cache[num_users].uid = pwent->pw_uid;
		strlcpy__(uid_name_cache[num_users].username, pwent->pw_name, USR_NAME_LEN);
		strlcpy__(uid_name_cache[num_users].homedir, pwent->pw_dir, MAX_PATH_LEN);
///flf_d_printf("%2d:%4d:[%s]:[%s]\n",
/// num_users,
/// uid_name_cache[num_users].uid,
/// uid_name_cache[num_users].username,
/// uid_name_cache[num_users].homedir);
	}
	endpwent();
	return num_users;
}
int cache_groups(void)
{
	struct group *grent;

	// cache group name
	setgrent();
	for (num_groups = 0; num_groups < MAX_GROUPS; num_groups++) {
		if ((grent = getgrent()) == NULL)
			break;
		gid_name_cache[num_groups].gid = grent->gr_gid;
		strlcpy__(gid_name_cache[num_groups].grpname, grent->gr_name, GRP_NAME_LEN);
///flf_d_printf("%2d:%4d:[%s]\n",
/// num_groups,
/// gid_name_cache[num_groups].gid,
/// gid_name_cache[num_groups].grpname);
	}
	endgrent();
	return num_groups;
}

// End of utils.c
/**************************************************************************
 *   utilstr.c                                                            *
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

#include "utilincs.h"

#define STR_BUF_LEN		4096

// [0x00, 0x1f], 0x7f
int is_ctrl_char(unsigned char uchar)
{
	int chr;

	chr = uchar;
	return (0x00 <= chr && chr < 0x20) || (0x7f <= chr && chr < 0x80);
}

//-----------------------------------------------------------------------------

#ifndef HAVE_STRCASECMP
/* This function is equivalent to strcasecmp(). */
int my_stricmp(const char *s1, const char *s2)
{
	for ( ; *s1 && *s2; s1++, s2++) {
		if (tolower(*s1) != tolower(*s2))
			break;
	}
	return (tolower(*s1) - tolower(*s2));
}
#endif
#ifndef HAVE_STRNCASECMP
/* This function is equivalent to strncasecmp(). */
int my_strnicmp(const char *s1, const char *s2, size_t n)
{
	for ( ; n > 0 && *s1 && *s2; n--, s1++, s2++) {
		if (tolower(*s1) != tolower(*s2))
			break;
	}
	if (n > 0)
		return (tolower(*s1) - tolower(*s2));
	return 0;
}
#endif

//-----------------------------------------------------------------------------

char *conv_esc_str(char *string)
{
	char *src;
	char *dest;
	char buf[MAX_PATH_LEN+1];

	dest = buf;
	for (src = string; *src; src++) {
		switch(*src) {
		case '\\':
			switch(*(src+1)) {
			case 'a':
			case 't':
			case 'r':
				switch(*(src+1)) {
				case 'a':
					*dest = '\a';
					break;
				case 't':
					*dest = '\t';
					break;
				case 'r':
					*dest = '\r';
					break;
				}
				src++;
				dest++;
				break;
			}
			break;
		default:
			*dest++ = *src;
			break;
		}
	}
	*dest = '\0';
	strlcpy__(string, buf, MAX_PATH_LEN);
	return string;
}

//-----------------------------------------------------------------------------
#ifdef START_UP_TEST
void test_replace_str(void)
{
	char buffer[100+1];

	strcpy(buffer, "abcdefghijklmnop");
	flf_d_printf("%s\n", buffer);
	flf_d_printf("%s\n", replace_str(buffer, 100, 7, 4, "HIJK", 4));
	strcpy(buffer, "abcdefghijklmnop");
	flf_d_printf("%s\n", replace_str(buffer, 100, 7, 4, "", 0));
	strcpy(buffer, "abcdefghijklmnop");
	flf_d_printf("%s\n", replace_str(buffer, 100, 7, 0, "HIJK", 4));
}
#endif // START_UP_TEST

char *replace_str(char *buffer, size_t buf_len, size_t start, int delete_len,
 const char *str, len_t insert_len)
{
///flf_d_printf("[%s], [%s]:%d, [%s]:%d\n",
/// buffer, &buffer[start], delete_len, str, insert_len);
	if (delete_len > 0) {
		delete_str(buffer, start, delete_len);
	}
///flf_d_printf("[%s]\n", buffer);
	if (insert_len) {	// (insert_len > 0) or (insert_len == -1)
		insert_str(buffer, buf_len, start, str, insert_len);
	}
	return buffer;
}

int insert_str_separating_by_space(char *buffer, size_t buf_len, size_t offset,
 const char *str)
{
///	offset = strnlen(buffer, buf_len);
	if (offset) {
		// not head of line
		if (buffer[offset-1] != ' ') {
			// "abcdef" ==> "abc def"
			//     ^             ^
			insert_str(buffer, buf_len, offset, " ", -1);
			offset++;
		}
	}
	if (buffer[offset]) {
		// not tail of line
		if (buffer[offset] != ' ') {
			// "abc def" ==> "abc  def"
			//      ^             ^
			insert_str(buffer, buf_len, offset, " ", -1);
		}
	}
	// "abc  def" ==> "abc xyz def"
	//      ^              ^
	insert_str(buffer, buf_len, offset, str, -1);
	offset += strlen(str);
	// "abc xyz def"
	//         ^
///flf_d_printf("buffer: [%s]\n", buffer);
	return offset;
}

// "AAAAbbbbCCCC" ==> "AAAACCCC"
char *delete_str(char *buffer, size_t start, len_t delete_len)
{
	strcpy__(&buffer[start], &buffer[start+delete_len]);
	return buffer;
}

// "AAAACCCC", "bbbb" ==> "AAAAbbbbCCCC"
char *insert_str(char *buffer, size_t buf_len, size_t offset,
 const char *string, len_t insert_len)
{
	int org_len;

///flf_d_printf("buffer: [%s]\n", buffer);
///flf_d_printf("string: [%s]\n", string);
	org_len = strnlen(buffer, buf_len);
	if (insert_len < 0) {
		insert_len = strnlen(string, buf_len);
	}
	if (insert_len > 0) {
		if (org_len + insert_len <= buf_len) {
			strlcpy__(&buffer[offset + insert_len], &buffer[offset], org_len - offset);
			memmove(&buffer[offset], string, insert_len);
		} else {
			// buffer overflow
		}
	}
///flf_d_printf("buffer: [%s]\n", buffer);
	return buffer;
}
char *concat_file_name_separating_by_space(char *buffer, size_t buf_len,
 const char *string)
{
	if (is_strlen_not_0(buffer) && buffer[strlen(buffer)-1] != ' ') {
		// "command" ==> "command "
		strlcat__(buffer, buf_len, " ");
	}
	string = quote_file_name(string);
	if (strnlen(buffer, buf_len) + strnlen(string, buf_len) <= buf_len) {
		strlcat__(buffer, buf_len, string);
	}
	return buffer;
}
const char *quote_file_name(const char *string)
{
	static char buf[MAX_PATH_LEN+1];

	return quote_file_name_if_necessary(buf, string);
}
const char *quote_file_name_buf(char *buf, const char *string)
{
	return quote_file_name_if_necessary(buf, string);
}
const char *quote_file_name_if_necessary(char *buf, const char *string)
{
	if (contain_chr(string, ' ')
	 || contain_chr(string, '\'') || contain_chr(string, '"')) {
		if (contain_chr(string, '\'')) {
			// abc'def.txt ==> "abc'def.txt"
			return quote_string(buf, string, '"');
		} else {
			// abc"def.txt ==> 'abc"def.txt'
			return quote_string(buf, string, '\'');
		}
	}
	return string;
}

int is_strlen_0(const char *str)
{
	return is_strlen_not_0(str) == '\0';
}
int is_strlen_not_0(const char *str)
{
	return *str;
}

size_t str_path_len(const char *str)
{
	return strnlen(str, MAX_PATH_LEN);
}

char *strcat_printf(char *buffer, size_t buf_len, const char *format, ...)
{
	va_list ap;
	char buf[MAX_PATH_LEN+1];

	va_start(ap, format);
	vsnprintf(buf, MAX_PATH_LEN+1, format, ap);
	va_end(ap);
	strlcat__(buffer, buf_len, buf);
	return buffer;
}
int snprintf_(char *buffer, size_t buf_len, const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vsnprintf(buffer, buf_len, format, ap);
	va_end(ap);
	return ret;
}

// dest and src are overlappable
char *strcpy__(char *dest, const char *src)
{
	return strlcpy__(dest, src, strlen(src));
}
char *strlcpy__(char *dest, const char *src, size_t len)
{
	len = LIM_MAX(strlen(src), len);
	memmove(dest, src, len);
	dest[len] = '\0';	// always NUL terminate string
	return dest;
}

// dest and src are overlappable
char *strcat__(char *dest, const char *src)
{
	return memmove(&dest[strlen(dest)], src, strlen(src) + 1);
}
char *strlcat__(char *dest, size_t buf_len, const char *src)
{
	return strlncat__(dest, buf_len, src, strlen(src));
}
char *strlncat__(char *dest, size_t buf_len, const char *src, size_t cat_len)
{
	int len;

	len = strlen(dest);
	strlcpy__(&dest[len], src, LIM_MAX(buf_len - len, cat_len));
	return dest;
}

char *strcut__(char *buffer, size_t buf_len, const char *string, size_t so, size_t eo)
{
	buf_len = LIM_MAX(buf_len, eo - so);
	return strlcpy__(buffer, &string[so], buf_len);
}

int strlcmp__(const char *dest, const char *src)
{
	return strncmp(dest, src, strlen(src));
}

int strlcasecmp(const char *str1, const char *str2)
{
	return strncasecmp(str1, str2, strlen(str2));
}

char *strchr__(const char *str, char chr)
{
	return chr ? strchr(str, chr) : NULL;
}
char *strnset__(char *buf, char chr, size_t len)
{
	memset(buf, chr, len);
	buf[len] = '\0';	// always NUL terminate string
	return buf;
}
// dest and src are overlappable
char *strncpy__(char *dest, const char *src, size_t buf_len)
{
	size_t copy_len;

	copy_len = LIM_MAX(strlen(src), buf_len);
	memmove(dest, src, copy_len);
	memset(&dest[copy_len], '\0', buf_len - copy_len);
	return dest;
}
// dest and src are overlappable
void *memcpy__(void *dest, const void *src, size_t len)
{
	return memmove(dest, src, len);
}

int tolower_if_alpha(int chr)
{
	if (isalpha(chr))
		return tolower(chr);
	return chr;
}

char *strupper(char *buffer)
{
	char *ptr;

	for (ptr = buffer ; *ptr; ptr++) {
		*ptr = toupper(*ptr);
	}
	return buffer;
}

char *strlower(char *buffer)
{
	char *ptr;

	for (ptr = buffer ; *ptr; ptr++) {
		*ptr = tolower(*ptr);
	}
	return buffer;
}

//-----------------------------------------------------------------------------

char *shrink_str(char *str, int space, int n_for_10)
{
	char buf_[MAX_PATH_LEN+1];

	shrink_str_buf(buf_, str, space, n_for_10);
	strlcpy__(str, buf_, MAX_PATH_LEN);
	return str;
}
///char *shrink_str_static(const char *str, int space, int n_for_10)
///{
///	static char buf[MAX_PATH_LEN+1];
///
///	shrink_str_buf(buf, str, space, n_for_10);
///	return buf;
///}
// "/very/long/long/path/to/file" ==> "/very/lo...th/to/file"
//                                       n/10         (10-n)/10
char *shrink_str_buf(char *buf, const char *str, int space, int n_for_10)
{
	int str_cols;
	int space1 = 0, space2 = 0;
	int byte_idx1, byte_idx2;

	str_cols = utf8s_columns(str, INT_MAX);
	if (str_cols <= space) {
		// enough space
		strlcpy__(buf, str, MAX_PATH_LEN);
	} else {
#define STR_PPP		"~~"
#define STRLEN_PPP	2	// strlen(STR_PPP)
		if (space > STRLEN_PPP) {
			space1 = LIM_MIN(0, (space - STRLEN_PPP) / 10 * n_for_10);
			space2 = LIM_MIN(0, (space - STRLEN_PPP) - space1);
			byte_idx1 = get_byte_idx_from_col_idx(str, space1, -1, NULL);
			byte_idx2 = get_byte_idx_from_col_idx(str, str_cols - space2, +1, NULL);
			strlcpy__(buf, str, byte_idx1);
			strcat__(buf, STR_PPP);
			strcat__(buf, &str[byte_idx2]);
		} else {
			space2 = LIM_MIN(0, space);
			byte_idx2 = get_byte_idx_from_col_idx(str, str_cols - space2, +1, NULL);
			strlcpy__(buf, &str[byte_idx2], MAX_PATH_LEN);
		}
	}
	return buf;
}

int adjust_utf8s_columns(char *utf8s, int columns)
{
	truncate_tail_utf8s_columns(utf8s, columns);
	return expand_utf8s_columns(utf8s, columns);
}
int truncate_tail_utf8s_columns(char *utf8s, int columns)
{
	int bytes;

	bytes = get_byte_idx_from_col_idx(utf8s, columns, -1, NULL);
	utf8s[bytes] = '\0';
	return bytes;
}
int get_byte_idx_from_col_idx(const char *utf8s, int columns, int left_right,
 int *col_idx__)
{
	int col_idx;
	int prev_col_idx;
	const char *ptr;
	const char *prev_ptr;

	prev_col_idx = columns;
	for (prev_ptr = ptr = utf8s, col_idx = 0; *ptr && col_idx < columns; ) {
		prev_col_idx = col_idx;
		prev_ptr = ptr;
		col_idx += utf8c_columns(ptr);
		ptr += utf8c_bytes(ptr);
	}
///flf_d_printf("([%s], %d) ==> %d\n", utf8s, columns, ptr - utf8s);
	if (left_right < 0 && columns < col_idx) {
		col_idx = prev_col_idx;
		ptr = prev_ptr;
	}
	if (col_idx__)
		*col_idx__ = col_idx;
	return ptr - utf8s;
}

int expand_utf8s_columns(char *utf8s, int columns)
{
	int cols;
	int bytes;

	cols = utf8s_columns(utf8s, INT_MAX);
	if (columns - cols > 0) {
		bytes = strlen(utf8s);
		strnset__(&utf8s[bytes], ' ', columns - cols);
	}
	return LIM_MIN(columns, cols);
}

//-----------------------------------------------------------------------------

int skip_space(const char **ptr)
{
	while (IS_SPACE(*ptr))
		(*ptr)++;
	return IS_EOL(*ptr);
}
const char *skip_chars(const char *ptr, const char *chars)
{
	while (*ptr && strchr__(chars, *ptr)) {
		ptr++;
	}
	return ptr;
}

const char *skip_to_file_path(const char *ptr)
{
	while (*ptr && is_file_path_char(ptr) == 0) {
		// skip to beginning of a file path
		ptr++;
	}
	return ptr;
}
const char *skip_file_path(const char *ptr)
{
	if (*ptr == '\'' || *ptr == '"') {
		// '\'abc/def\'ghi jkl.txt\''
		//  --       --
		char beginning_chr = *ptr;
		ptr++;	// skip beginning_chr
		while (*ptr) {
			if (*ptr == beginning_chr) {
				ptr++;		// skip closing '\'' or '"'
				break;
			}
			if (ptr[0] == '\\') {
				if (ptr[1])
					ptr++;		// skip '\\'
			}
			ptr++;		// skip the next char of '\\'
		}
	} else {
		// filename.txt
		while (is_file_path_char(ptr)) {
			ptr += utf8c_bytes(ptr);
		}
	}
	return ptr;
}
char *skip_file_name(char *ptr)
{
	while (is_file_name_char(ptr)) {
		// skip file name
		ptr += utf8c_bytes(ptr);
	}
	return ptr;
}
#if 0
const char *skip_separator(const char *ptr)
{
	for ( ; is_separator(*ptr); ) {
		ptr++;
		// skip to the next token
	}
	return ptr;
}
#endif
const char *skip_to_digit(const char *ptr)
{
	for ( ; *ptr && isdigit(*ptr) == 0; ) {
		ptr++;
		// skip digits
	}
	return ptr;
}
const char *skip_digits(const char *ptr)
{
	for ( ; isdigit(*ptr); ) {
		ptr++;
		// skip digits
	}
	return ptr;
}

char *remove_line_tail_lf(char *line)
{
	int len;

	len = LIM_MIN(0, (int)strlen(line) - 1);
	if (line[len] == '\n')
		line[len] = '\0';
	return line;
}

int is_file_path_char(const char *ptr)
{
	return is_file_name_char(ptr) || strchr__("/", *ptr);
}
int is_file_name_char(const char *ptr)
{
///	return (((' ' < *ptr) && (*ptr < 0x80) && (*ptr != '/')) || (utf8c_bytes(ptr) >= 2));
	return isalnum(*ptr) || strchr__("_-+.~$%&@=\"\'", *ptr) || (utf8c_bytes(ptr) >= 2);
}
int is_separator(char chr)
{
	return strchr__(" \t,:", chr) != NULL;
}
int contain_chr(const char *string, char chr)
{
	return strchr(string, chr) != NULL;
}
char *quote_string(char *buffer, const char *string, char quote_chr)
{
	char buf[MAX_PATH_LEN+1];
	// file name ==> "file name" or 'file name'
	if (contain_chr(string, '\'')) {
		// "'file'name'" ==> "'\'file\'name\''"
		escape_quote_chr(buf, string, quote_chr);
		snprintf(buffer, MAX_PATH_LEN+1, "%c%s%c", quote_chr, buf, quote_chr);
	} else {
		// "filename" ==> "'filename'"
		snprintf(buffer, MAX_PATH_LEN+1, "%c%s%c", quote_chr, string, quote_chr);
	}
	return buffer;
}
// "'file'name'" ==> "\'file\'name\'"
char *escape_quote_chr(char *buffer, const char *string, char quote_chr)
{
	char buf[2+1];	// "\'"
	buffer[0] = '\0';
	for ( ; *string; string++) {
		if (*string == quote_chr) {
			snprintf(buf, 2+1, "%c%c", '\\', quote_chr);
		} else {
			snprintf(buf, 1+1, "%c", *string);
		}
		strlcat__(buffer, MAX_PATH_LEN+1, buf);
	}
	return buffer;
}

char *unquote_string(char *buffer)
{
///flf_d_printf("     [%s] ==>\n", buffer);
	if (is_quoted(buffer, '\'') || is_quoted(buffer, '"')) {
		char quote_chr = buffer[0];
		size_t len = str_path_len(buffer);
		char *ptr = buffer;
		for (int idx = 1; idx < len-1; idx++) {
			if (buffer[idx] == '\\' && buffer[idx+1] == quote_chr) {
				*ptr++ = buffer[idx+1];	// "\'" ==> "'", '\"' ==> '"'
				idx++;
			} else {
				*ptr++ = buffer[idx];
			}
		}
		*ptr = '\0';	// NUL terminate
	}
///flf_d_printf(" ==> [%s]\n", buffer);
	return buffer;
}
char is_quoted(const char *str, char quote_chr)
{
	if (str_path_len(str) >= 2) {
		if (str[0] == quote_chr && str[str_path_len(str) - 1] == quote_chr) {
			// 'string-quoted' or "string-quoted"
			return quote_chr;
		}
	}
	return '\0';
}

//-----------------------------------------------------------------------------------

char *select_plural_form(char *singular, char *plural, char *type3, char *type4, int number)
{
///flf_d_printf("number:%d ==> plural_form_index:%d\n", number, get_plural_form_index(number));
	switch(get_plural_form_index(number)) {
	default:
	case 0:
		return singular;
	case 1:
		return plural;
	case 2:
		return type3;
	case 3:
		return type4;
	}
}

int get_plural_form_index(int number)
{
#ifdef ENABLE_NLS
#define LANG_STR_LEN	2
	enum lang {
		LANG_AR,	// = 0
		LANG_BG,
		LANG_CS,
		LANG_DA,
		LANG_DE,
		LANG_EN,
		LANG_ES,
		LANG_FR,
		LANG_FI,
		LANG_GA,
		LANG_EL,
		LANG_HR,
		LANG_HU,
		LANG_IS,
		LANG_IT,
		LANG_IW,
		LANG_JA,
		LANG_KO,
		LANG_LT,
		LANG_LV,
		LANG_NL,
		LANG_NO,
		LANG_PL,
		LANG_PT,
		LANG_RO,
		LANG_RU,
		LANG_SK,
		LANG_SL,
		LANG_SV,
		LANG_TH,
		LANG_TR,
		LANG_ZH,
		LANG_ZZ,
	};
	char lang_names[LANG_ZZ+1][LANG_STR_LEN+1] = {
		"ar",	// LANG_AR
		"bg",	// LANG_BG
		"cs",	// LANG_CS
		"da",	// LANG_DA
		"de",	// LANG_DE
		"en",	// LANG_EN
		"es",	// LANG_ES
		"fr",	// LANG_FR
		"fi",	// LANG_FI
		"ga",	// LANG_GA
		"el",	// LANG_EL
		"hr",	// LANG_HR
		"hu",	// LANG_HU
		"is",	// LANG_IS
		"it",	// LANG_IT
		"iw",	// LANG_IW
		"ja",	// LANG_JA
		"ko",	// LANG_KO
		"lt",	// LANG_LT
		"lv",	// LANG_LV
		"nl",	// LANG_NL
		"no",	// LANG_NO
		"pl",	// LANG_PL
		"pt",	// LANG_PT
		"ro",	// LANG_RO
		"ru",	// LANG_RU
		"sk",	// LANG_SK
		"sl",	// LANG_SL
		"sv",	// LANG_SV
		"th",	// LANG_TH
		"tr",	// LANG_TR
		"zh",	// LANG_ZH
		"--",	// LANG_ZZ
	};
	static char cur_lang[LANG_STR_LEN+1] = "";
	static enum lang cur_lang_idx = LANG_EN;

	if (cur_lang[0] == '\0') {
		// current language is not set, set cur_lang_idx by env "LANG"
		strlcpy__(cur_lang, getenv__("LANG"), LANG_STR_LEN);	// "ja_JP.UTF-8"
///flf_d_printf("cur_lang:[%s]\n", cur_lang);
		for (cur_lang_idx = 0; cur_lang_idx < LANG_ZZ; cur_lang_idx++) {
			if (strncmp(cur_lang, lang_names[cur_lang_idx], LANG_STR_LEN) == 0)
				break;
		}
		if (cur_lang_idx == LANG_ZZ) {
			cur_lang_idx = LANG_EN;
		}
///flf_d_printf("cur_lang:[%s] ==> cur_lang_idx:%d\n", cur_lang, cur_lang_idx);
	}

	switch(cur_lang_idx) {
	default:
	case LANG_EN:
	case LANG_DE:
		if (number == 1)
			return 0;
		return 1;
	case LANG_JA:
	case LANG_KO:
	case LANG_ZH:
		return 0;
	case LANG_FR:
		if (number <= 1)		// 0,1
			return 0;
		return 1;
	case LANG_LV:
		if (number % 10 == 1 && number % 100 != 11)
			return 0;			// 1,21,31,...91,101,121,131,...191,...
		if (number != 0)		// 2,3,4,...20,22,23,24,...
			return 1;
		return 2;				// 0
	case LANG_GA:
		if (number == 1)		// 1
			return 0;
		if (number == 2)		// 2
			return 1;
		return 2;				// 0,3,4,5,...
	case LANG_LT:
		if (number % 10 == 1 && number % 100 != 11)
			return 0;			// 1,21,31,...91,101,121,131,...191,...
		if (number % 10 >= 2 && ((number % 100 < 10) || (number % 100 >= 20)))
			return 1;			// 2,3,4,...9,22,23,24,...29,32,33,34,...39,...
		return 2;				// 0,10,11,12,13,...19,20,30,...
	case LANG_CS:
	case LANG_HR:
	case LANG_RU:
		if (number % 10 == 1 && number % 100 != 11)
			return 0;			// 1,21,31,...91,101,121,131,...191,...
		if ((number % 10 >= 2) && (number % 10 <= 4)
		 && ((number % 100 < 10) || (number % 100 >= 20)))
			return 1;			// 2,3,4,22,23,24,32,33,34,...
		return 2;				// 0,5,6,7,8,9,10,11,12,13,...19,20,30,...
	case LANG_PL:
		if (number == 1)		// 1
			return 0;
		if ((number % 10 >= 2) && (number % 10 <= 4)
		 && ((number % 100 < 10) || (number % 100 >= 20)))
			return 1;			// 2,3,4,22,23,24,32,33,34,...
		return 2;				// 0,5,6,7,8,9,10,11,12,13,...19,20,30,...
	case LANG_SL:
		if (number % 100 == 1)	// 1,101,201,...
			return 0;
		if (number % 100 == 2)	// 2,102,202,...
			return 1;
		if (number % 100 == 3)	// 3,103,203,...
			return 2;
		return 3;				// 0,4,5,6,7,8,9,10,12,13,...100,104,105,106,...
	}
#else
	if (number == 1)
		return 0;
	return 1;
#endif
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_str_w_caret(const char *string, int byte_idx)
{
	char buf1[STR_BUF_LEN+1];
	char buf2[STR_BUF_LEN+1];

	if (IS_BETWEEN(0, byte_idx, strlen(string)) == 0) {
		warning_printf("byte_idx: %d out of range[%d, %d]\n",
		 byte_idx, 0, strlen(string));
		byte_idx = MIN_MAX_(0, byte_idx, strlen(string));
	}
	strcut__(buf1, STR_BUF_LEN, string, 0, byte_idx);
	strcut__(buf2, STR_BUF_LEN, string, byte_idx, strlen(string));
	d_printf("[%s^%s]\n", buf1, buf2);
}
void dump_str_w_caret2(const char *string, int byte_idx_1, int byte_idx_2)
{
	char buf1[STR_BUF_LEN+1];
	char buf2[STR_BUF_LEN+1];
	char buf3[STR_BUF_LEN+1];

	if (IS_BETWEEN(0, byte_idx_1, strlen(string)) == 0
	 || IS_BETWEEN(0, byte_idx_2, strlen(string)) == 0) {
		warning_printf("byte_idx_1:%d, byte_idx_2:%d out of range[%d, %d]\n",
		 byte_idx_1, byte_idx_2, 0, strlen(string));
		byte_idx_1 = MIN_MAX_(0, byte_idx_1, strlen(string));
		byte_idx_2 = MIN_MAX_(0, byte_idx_2, strlen(string));
	}
	strcut__(buf1, STR_BUF_LEN, string, 0, byte_idx_1);
	strcut__(buf2, STR_BUF_LEN, string, byte_idx_1, byte_idx_2);
	strcut__(buf3, STR_BUF_LEN, string, byte_idx_2, strlen(string));
	flf_d_printf("[%s{%s}%s]\n", buf1, buf2, buf3);
}
#endif // ENABLE_DEBUG

// End of utilstr.c
/**************************************************************************
 *   winin.c                                                              *
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

#include "headers.h"

PRIVATE int input_str_pos_(const char *default__, char *input_buf, int curs_byte_idx,
 int hist_type_idx, const char *msg, va_list ap);
PRIVATE int input_str_pos__(const char *default__, char *input_buf, int curs_byte_idx,
 int hist_type_idx, const char *msg);
PRIVATE void disp_input_box(const char *buf, const char *input_buf, int x);
PRIVATE void blank_input_box(void);

// input string
int input_string(const char *default__, char *input_buf,
 int hist_type_idx, const char *msg, ...)
{
	va_list ap;
	int ret;

	va_start(ap, msg);
	ret = input_str_pos_(default__, input_buf, MAX_PATH_LEN, hist_type_idx, msg, ap);
	va_end(ap);
	return ret;
}

int input_string_pos(const char *default__, char *input_buf, int curs_byte_idx,
 int hist_type_idx, const char *msg, ...)
{
	va_list ap;
	int ret;

	va_start(ap, msg);
	ret = input_str_pos_(default__, input_buf, curs_byte_idx, hist_type_idx, msg, ap);
	va_end(ap);
	return ret;
}

// return -1: recursively called
// return 0 : cancelled
// return 1 : input normally
PRIVATE int input_str_pos_(const char *default__, char *input_buf, int curs_byte_idx,
 int hist_type_idx, const char *msg, va_list ap)
{
	static int recursively_called = 0;
	char dir_save[MAX_PATH_LEN + 1];
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];
	int byte_idx;
	int ret;

	if (recursively_called) {
///flf_d_printf("recursively_called\n");
		return -1;				// -1: recursive called
	}
	recursively_called = 1;

	vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
	byte_idx = byte_idx_from_col_idx(msg_buf, main_win_get_columns(), CHAR_LEFT, NULL);
	msg_buf[byte_idx] = '\0';		// limit message length

	set_work_space_color_low();
	update_screen_app(1, 1, 1);
	set_work_space_color_normal();
	get_cur_dir(dir_save);

	tio_set_cursor_on(1);
	//---------------------------------------------------------------------------------
	ret = input_str_pos__(default__, input_buf, curs_byte_idx, hist_type_idx, msg_buf);
	//---------------------------------------------------------------------------------
	// 0: cancelled
	// 1: string is normally input
	tio_set_cursor_on(0);

	change_cur_dir(dir_save);
	update_screen_app(1, 1, 1);
	recursively_called = 0;

	if (ret <= 0) {
		disp_status_bar_done(_("Cancelled"));
		return 0;				// 0: cancelled
	}
#ifdef ENABLE_HISTORY
	if (is_strlen_not_0(input_buf)) {
		update_history(hist_type_idx, input_buf);
	}
#endif
///flf_d_printf("-9 input_buf:[%s] default__:[%s] ret:%d\n", input_buf, default__, ret);
	return 1;					// 1: input normally
}

// Input string. This should only be called from input_string().
// return 0 : cancelled
// return 1 : input normally
PRIVATE int input_str_pos__(const char *default__, char *input_buf, int curs_byte_idx,
 int hist_type_idx, const char *msg)
{
	int key_input;
	const char *func_id;
	int bytes;
#if defined(ENABLE_HISTORY) || defined(ENABLE_FILER)
	char buffer[MAX_PATH_LEN+1];
	int ret;
#endif
	char cut_buf[MAX_PATH_LEN+1] = "";

	strlcpy__(input_buf, default__, MAX_PATH_LEN);
	curs_byte_idx = MIN_MAX_(0, curs_byte_idx, strnlen(default__, MAX_PATH_LEN));

	blank_key_list_lines();
	for ( ; ; ) {
		disp_input_box(msg, input_buf, curs_byte_idx);
		tio_refresh();
		//---------------------------
		key_input = input_key_loop();
		//---------------------------
mflf_d_printf("input%ckey:0x%04x(%s)=======================================\n",
 '_', key_input, short_key_name_from_key_code(key_input, NULL));

		if (IS_CHAR_KEY(key_input)) {
			// character key
			if (strnlen(input_buf, MAX_PATH_LEN) < MAX_PATH_LEN) {
				memmove(input_buf + curs_byte_idx + 1, input_buf + curs_byte_idx,
				 strnlen(input_buf, MAX_PATH_LEN) - curs_byte_idx + 1);
				input_buf[curs_byte_idx] = key_input;
				curs_byte_idx++;
			}
			func_id = "";
		} else {
			// function key
			func_id = get_func_id_from_key(key_input);
///flf_d_printf("func_id:[%s]\n", func_id);
		}
		if (key_input == K_ESC || key_input == K_M_ESC
		 || cmp_func_id(func_id, "do_close_file_ask")
		 || cmp_func_id(func_id, "do_close_all_ask")) {
			strcpy__(input_buf, "");
			key_input = K_ESC;
		} else
		if (key_input == K_C_M || key_input == K_ENTER) {
			key_input = K_C_M;
		} else
		if (cmp_func_id(func_id, "do_left")) {
			// cursor left
			if (curs_byte_idx > 0) {
				bytes = utf8c_prev_bytes(input_buf, &input_buf[curs_byte_idx]);
				curs_byte_idx -= bytes;
			}
		} else
		if (cmp_func_id(func_id, "do_right")) {
			// cursor right
			if (curs_byte_idx < strnlen(input_buf, MAX_PATH_LEN)) {
				bytes = utf8c_bytes(&input_buf[curs_byte_idx]);
				curs_byte_idx += bytes;
			}
		} else
		if (cmp_func_id(func_id, "do_start_of_line")
		 || cmp_func_id(func_id, "do_prev_word")) {
			// goto line head
			curs_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "do_end_of_line")
		 || cmp_func_id(func_id, "do_next_word")) {
			// goto line tail
			curs_byte_idx = strnlen(input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "do_backspace")
		 || (key_input == K_BS)) {
			// backspace
			if (curs_byte_idx > 0) {
				bytes = utf8c_prev_bytes(input_buf, &input_buf[curs_byte_idx]);
				curs_byte_idx -= bytes;
				delete_str(input_buf, curs_byte_idx, bytes);
			}
		} else
		if (cmp_func_id(func_id, "do_delete_char")
		 || (key_input == K_DEL)) {
			// delete
			if (curs_byte_idx < strnlen(input_buf, MAX_PATH_LEN)) {
				bytes = utf8c_bytes(&input_buf[curs_byte_idx]);
				delete_str(input_buf, curs_byte_idx, bytes);
			}
		} else
		if (cmp_func_id(func_id, "do_cut_to_head")) {
			// cut to line head
			strcut__(cut_buf, MAX_PATH_LEN, input_buf, 0, curs_byte_idx);
			delete_str(input_buf, 0, curs_byte_idx);
			curs_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "do_cut_text")) {
			// cut line
			strlcpy__(cut_buf, input_buf, MAX_PATH_LEN);
			strcpy(input_buf, "");
			curs_byte_idx = 0;
		} else
		if (cmp_func_id(func_id, "do_cut_to_tail")) {
			// cut to line tail
			strcut__(cut_buf, MAX_PATH_LEN,
			 input_buf, curs_byte_idx, strnlen(input_buf, MAX_PATH_LEN));
			delete_str(input_buf, curs_byte_idx, strnlen(input_buf, MAX_PATH_LEN) - curs_byte_idx);
		} else
		if (cmp_func_id(func_id, "do_copy_text")) {
			// copy to the cut buffer
			strlcpy__(cut_buf, input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "do_paste_text_with_pop")) {
			// paste from the cut buffer
			insert_str(input_buf, MAX_PATH_LEN, curs_byte_idx, cut_buf, -1);
#ifdef ENABLE_HISTORY
		} else
		if (key_input == K_TAB) {
			// tab history completion
			strlcpy__(input_buf, get_history_completion(hist_type_idx, input_buf),
			 MAX_PATH_LEN);
			curs_byte_idx = strnlen(input_buf, MAX_PATH_LEN);
		} else
		if (cmp_func_id(func_id, "do_up")
		 || cmp_func_id(func_id, "do_page_up")
		 || cmp_func_id(func_id, "do_first_line")) {
			ret = select_from_history_list(hist_type_idx, buffer);
			if (ret > 0) {
#if 1
				if (cmp_func_id(func_id, "do_up")) {
					// clear input buffer
					strcpy__(input_buf, "");
					curs_byte_idx = 0;
				}
#endif
				curs_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 curs_byte_idx, buffer);
			}
#endif // ENABLE_HISTORY
#ifdef ENABLE_FILER
		} else
		if (cmp_func_id(func_id, "do_down")
		 || cmp_func_id(func_id, "do_page_down")
		 || cmp_func_id(func_id, "do_last_line")) {
			ret = call_filer(1, 1, "", "", buffer, MAX_PATH_LEN);
			if (ret > 0) {
///flf_d_printf("[%s]\n", buffer);
#if 1
				if (cmp_func_id(func_id, "do_down")) {
					// clear input buffer
					strcpy__(input_buf, "");
					curs_byte_idx = 0;
				}
#endif
				curs_byte_idx = insert_str_separating_by_space(input_buf, MAX_PATH_LEN,
				 curs_byte_idx, buffer);
			}
#endif // ENABLE_FILER
		} else
		if (cmp_func_id(func_id, "do_search_backward_first")
		 || cmp_func_id(func_id, "do_search_forward_first")
		 || cmp_func_id(func_id, "do_replace")) {
			// get string from edit buffer current cursor position
			if (count_edit_bufs()) {
				char *line;
				int byte_idx;

////flf_d_printf("input_buf:[%s]\n", input_buf);
				line = CEPBV_CL->data;
				byte_idx = byte_idx_from_byte_idx(line,
				 CEPBV_CLBI + strnlen(input_buf, MAX_PATH_LEN));
				// copy one token (at least copy one character)
				curs_byte_idx = 0;
				for ( ;
				 (strnlen(input_buf, MAX_PATH_LEN) < MAX_PATH_LEN - MAX_UTF8C_BYTES)
				  && (byte_idx < strnlen(line, MAX_PATH_LEN))
				  && (curs_byte_idx == 0
				   || (isalnum(line[byte_idx]) || line[byte_idx] == '_')); ) {
					strlncat__(input_buf, MAX_PATH_LEN, &line[byte_idx],
					 utf8c_bytes(&line[byte_idx]));
					curs_byte_idx++;
					byte_idx += utf8c_bytes(&line[byte_idx]);
				}
				curs_byte_idx = strnlen(input_buf, MAX_PATH_LEN);
			}
		}
		if (key_input == K_ESC || key_input == K_C_M)
			break;
	}

	if (key_input == K_ESC)
		return 0;						// cancelled, no input
	return 1;							// input
}

/* display input box
#Prompt message#######################################################
#input-string                                                        #
###UP:history##DOWN:browser###########################################

-Prompt message-------------------------------------------------------
-input-string                                                        -
----------------------------------------------------------------------

-Prompt message-------------------------------------------------------
|input-string                                                        |
----------------------------------------------------------------------

+Prompt message------------------------------------------------------+
|input-string                                                        |
+--------------------------------------------------------------------+
+Prompt message------------------------------------------------------+
+input-string                                                        +
+--------------------------------------------------------------------+
*/
PRIVATE void disp_input_box(const char *msg, const char *input_buf, int curs_byte_idx)
{
	int cursor_col_idx = col_idx_from_byte_idx(input_buf, 0, curs_byte_idx);
	int input_area_width;
	int start_byte_idx;
	int bytes;

	blank_input_box();
	set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	main_win_output_string(get_input_line_y(), 1, msg, -1);
	main_win_output_string(get_input_line_y()+2, 1,
	 _("UP:history, DOWN:browser, PgUp:insert from history, PgDn:insert from browser"), -1);
	set_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);

	input_area_width = main_win_get_columns()-2;
	if (cursor_col_idx < input_area_width) {
		bytes = byte_idx_from_col_idx(input_buf, input_area_width, CHAR_LEFT, NULL);
		main_win_output_string(get_input_line_y()+1, 1, input_buf, bytes);
		main_win_set_cursor_pos(get_input_line_y()+1, 1 + cursor_col_idx);
	} else {
#define TRUNCATION_MARK	"."
		//"abcdefghijklmnopqrstuvwxyz"
		// ==> |.ghijklmnopqrstuvwxyz |
		//                           ^cursor
		//
		//    "abcdefghijklmnopqrstuvwxyz"
		// ==> |.cdefghijklmnopqrstuvw|
		//                           ^cursor
		//
		//     "abcdefghijklmnopqrstuvwxyz"
		// ==> |abcdefghijklmnopqrstuv|
		//                           ^cursor
		start_byte_idx = byte_idx_from_col_idx(input_buf,
		 cursor_col_idx - (input_area_width-2), CHAR_RIGHT, NULL);
		bytes = byte_idx_from_col_idx(&input_buf[start_byte_idx],
		 input_area_width-utf8s_columns(TRUNCATION_MARK, INT_MAX), CHAR_LEFT, NULL);
		main_win_output_string(get_input_line_y()+1, 1, TRUNCATION_MARK, -1);
		main_win_output_string(-1, -1, &input_buf[start_byte_idx], bytes);
		main_win_set_cursor_pos(get_input_line_y()+1,
		 1 + utf8s_columns(TRUNCATION_MARK, INT_MAX)
		 + col_idx_from_byte_idx(&input_buf[start_byte_idx],
		 0, curs_byte_idx - start_byte_idx));
	}
}
PRIVATE void blank_input_box(void)
{
	// display frame
	set_color_by_idx(ITEM_COLOR_IDX_MENU_FRAME, 0);
	//+--------------------------------------------------------------------+
	//|                                                                    |
	//+--------------------------------------------------------------------+
	main_win_clear_lines(get_input_line_y(), get_input_line_y() + 3);
	// clear input area
	set_color_by_idx(ITEM_COLOR_IDX_INPUT, 0);
	//+--------------------------------------------------------------------+
	//|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|
	//+--------------------------------------------------------------------+
	main_win_output_string(get_input_line_y()+1, 1, tio_blank_line(), main_win_get_columns()-2);
}

//-----------------------------------------------------------------------------

PRIVATE void list_one_key(char key, const char *desc);

// Ask a simple yes/no question on the status_bar.
int ask_yes_no(int flags, const char *msg, ...)
{
	int key_lines_save;
	va_list ap;
	char msg_buf[MAX_SCRN_LINE_BUF_LEN+1];
	const char *chars_yes = "Yy";				// Yes(replace)
	const char *chars_no = "Nn";				// No
	const char *chars_all = "Aa";				// All
	const char *chars_backward = "Bb";			// Backward search
	const char *chars_forward = "Ff ";			// Forward search
	const char *chars_cancel = "EeSsXx";		// End/Stop/eXit
	const char *chars_end = "QqRr\x1b" S_C_Q;	// Quit/Return/cancel
	const char *chars_undo = "Uu";				// Undo
	const char *chars_redo = "Oo";				// redO
	key_code_t key_input;
	int answer;
	int byte_idx;
	const char *func_id;

	key_lines_save = GET_APPMD(app_KEY_LINES);		// save KEY_LINES
	SET_APPMD_VAL(app_KEY_LINES, LIM_MIN(1, key_lines_save));	// set lines more than 1

	blank_key_list_lines();
	// First, display key list
	if (get_key_list_lines()) {
		blank_yes_no_line();
		main_win_set_cursor_pos(get_yes_no_line_y(), 0);
		if (flags & ASK_YES) {
			list_one_key(chars_yes[0], _("Yes"));
		}
		if (flags & ASK_NO) {
			list_one_key(chars_no[0], _("No"));
		}
		if (flags & ASK_ALL) {
			list_one_key(chars_all[0], _("All"));
		}
		if (flags & ASK_BACKWARD) {
			list_one_key(chars_backward[0], _("Backward"));
		}
		if (flags & ASK_FORWARD) {
			list_one_key(chars_forward[0], _("Forward"));
		}
		list_one_key(chars_cancel[0], _("Cancel"));
		if (flags & ASK_END || flags & ASK_NO) {
			list_one_key(chars_end[0], _("End"));
		}
		if (flags & ASK_UNDO) {
			list_one_key(chars_undo[0], _("Undo"));
		}
		if (flags & ASK_REDO) {
			list_one_key(chars_redo[0], _("Redo"));
		}
	}

	// Second, display prompt message
	if (*msg) {
		va_start(ap, msg);
		vsnprintf(msg_buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
		va_end(ap);
		strlcat__(msg_buf, MAX_SCRN_LINE_BUF_LEN, " ");
		byte_idx = byte_idx_from_col_idx(msg_buf, MAX_SCRN_LINE_BUF_LEN, CHAR_LEFT, NULL);
		msg_buf[byte_idx] = '\0';
		set_color_by_idx(ITEM_COLOR_IDX_WARNING, 0);
		blank_status_bar();
		main_win_output_string(main_win_get_bottom_win_y() + STATUS_LINE, 0, msg_buf, -1);
	}

	tio_refresh();

	for (answer = ANSWER_NONE; answer == ANSWER_NONE; ) {
		tio_set_cursor_on(1);
		//---------------------------
		key_input = input_key_loop();
		func_id = get_func_id_from_key(key_input);
		//---------------------------
		// Look for the key_input in yes/no/all
		if (strchr__(chars_yes, key_input) != NULL)
			answer = ANSWER_YES;
		else if (strchr__(chars_no, key_input) != NULL)
			answer = ANSWER_NO;
		else if ((flags & ASK_ALL) && strchr__(chars_all, key_input) != NULL)
			answer = ANSWER_ALL;
		else if ((flags & ASK_BACKWARD) && (strchr__(chars_backward, key_input) != NULL)
		 || cmp_func_id(func_id, "do_search_backward_next"))
			answer = ANSWER_BACKWARD;
		else if ((flags & ASK_FORWARD) && (strchr__(chars_forward, key_input) != NULL)
		 || cmp_func_id(func_id, "do_search_forward_next"))
			answer = ANSWER_FORWARD;
		else if (strchr__(chars_cancel, key_input) != NULL)
			answer = ANSWER_CANCEL;
		else if ((flags & ASK_END || flags & ASK_NO) && strchr__(chars_end, key_input) != NULL)
			answer = ANSWER_END;
		else if ((flags & ASK_UNDO) && strchr__(chars_undo, key_input) != NULL)
			answer = ANSWER_UNDO;
		else if ((flags & ASK_REDO) && strchr__(chars_redo, key_input) != NULL)
			answer = ANSWER_REDO;
	}

	// Then blank the status_bar.
	set_color_by_idx(ITEM_COLOR_IDX_STATUS, 0);
	blank_status_bar();
	disp_key_list_editor();

	SET_APPMD_VAL(app_KEY_LINES, key_lines_save);	// recover KEY_LINES
	update_screen_app(1, 1, 1);

///flf_d_printf("ask_yes_no()==>%d\n", answer);
	return answer;
}

PRIVATE void list_one_key(char key, const char *desc)
{
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s", short_key_name_from_key_code(key, key_name));
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
	main_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);

	snprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, "%s ", desc);
	main_win_output_string(-1, -1, buf, strnlen(buf, MAX_SCRN_LINE_BUF_LEN));
}

//-----------------------------------------------------------------------------

void disp_key_list(char *key_lists[])
{
	int idx;

	for (idx = 0; idx < get_key_list_lines(); idx++) {
		display_reverse_text(main_win_get_bottom_win_y() + KEY_LIST_LINE + idx,
		 key_lists[idx]);
	}
}
// text parenthesized by {} are displayed in reversed
void display_reverse_text(int yy, char *text)
{
	char *ptr;
	char *begin;
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	int columns = 1;
	int xx;
	char delimiter = 0;

	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_lines(yy, -1);
	xx = 0;
	for (ptr = text; *ptr && xx < main_win_get_columns(); xx += columns) {
		if (*ptr == '{' || *ptr == '<' || *ptr == '}' || *ptr == '>')
			delimiter = *ptr++;
		for (begin = ptr; *ptr; ptr++) {
			if (*ptr == '{' || *ptr == '<' || *ptr == '}' || *ptr == '>')
				break;
		}
		if (ptr > begin) {
			strlcpy__(buf, begin, ptr - begin);
			if (delimiter == '<') {
				strlcpy__(buf, short_key_name_from_func_id(buf), MAX_SCRN_LINE_BUF_LEN);
			}
			columns = LIM_MAX(main_win_get_columns() - xx, utf8s_columns(buf, INT_MAX));
			truncate_tail_utf8s_columns(buf, columns);
			expand_utf8s_columns(buf, columns);
			if (delimiter == '{' || delimiter == '<') { // }
				set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST, 0);
			}
			main_win_output_string(yy, xx, buf, -1);
			if (delimiter == '{' || delimiter == '<') { // }
				set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
			}
		}
	}
}

// End of winin.c
/**************************************************************************
 *   winout.c - windowed terminal output                                  *
 *                                                                        *
 *   Copyright (C) 2005-2018 PCR                                          *
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

#include "headers.h"

PRIVATE int win_depth = 0;			// 0:root window, 1,2,...:child window
win_rect_t sub_win_rects[SUB_WINS];
win_rect_t *main_win, *cur_win;
int cur_win_idx;

PRIVATE int win_get_lines(win_rect_t *win);
PRIVATE int win_get_columns(win_rect_t *win);
PRIVATE void win_set_cursor_pos(win_rect_t *win, int yy, int xx);
PRIVATE void win_clear_screen(win_rect_t *win);
PRIVATE void win_clear_lines(win_rect_t *win, int line_1, int line_2);
PRIVATE void win_output_string(win_rect_t *win, int yy, int xx, const char *string, int bytes);

//-----------------------------------------------------------------------------
int win_lines = -1;
int win_columns = -1;

void win_init_win_size(void)
{
	win_depth = 0;
	win_reinit_win_size();
}

///int win_show_win_size(void)
///{
////* Minimum editor window rows required to work properly */
///#define MIN_WIN_LINES		(TITLE_LINES+5+STATUS_LINES+MAX_KEY_LINES)	// 10
////* Minimum editor window cols required to work properly */
///#define MIN_WIN_COLS		40
///	if (tio_get_lines() < MIN_WIN_LINES
///	 || tio_get_columns() < MIN_WIN_COLS) {
///		disp_status_bar_done(_("Window size too small: (%d, %d)"),
///		 tio_get_columns(), tio_get_lines());
///		return 1;
///	}
///	disp_status_bar_done(_("Window size: (%d, %d)"),
///	 tio_get_columns(), tio_get_lines());
///	return 0;
///}

//-----------------------------------------------------------------------------

void inc_win_depth(void)
{
	win_depth++;
}
void dec_win_depth(void)
{
	if (win_depth > 0) {
		win_depth--;
	}
}

int win_terminal_resized(void)
{
	return win_lines != tio_get_lines() || win_columns != tio_get_columns();
}

// editor window sectioning
//	[single pane]							[two panes]
//	+path-----------(MAIN)--------------+	+----------------(MAIN)-----------------+
//	|---+--ruler--(SUB_WHOLE)-----------|   |path--(SUB_LEFT)---+path--(SUB_RIGHT)--|
//	|  1+------------text---------------|	|---+----ruler------+---+----ruler------|
//	|  2|                               |	|  1+----text-------|  1+----text-------|
//	|  3|                               |	|  2|               |  2|               |
//	|  4|                               |	|  3|               |  3|               |
//	|  5|                               |	|  4|               |  4|               |
//	|  6|                               |	|  5|               |  5|               |
//	|  7|                               |	|  6|               |  6|               |
//	|  8|                               |	|  7|               |  7|               |
//	|---+-------------------------------|	|---+---------------+---+---------------|
//	|                                   |	|                                       |
//	+-----------------------------------+	+---------------------------------------+

// filer window sectioning
//	[single pane]							[two panes]
//	+---------------(MAIN)--------------+	+----------------(MAIN)-----------------+
//	|------------(SUB_WHOLE)------------|	|----(SUB_LEFT)-----+----(SUB_RIGHT)----|
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|                                   |	|                   |                   |
//	|-----------------------------------|	|-------------------+-------------------|
//	|                                   |	|                                       |
//	+-----------------------------------+	+---------------------------------------+
void win_reinit_win_size(void)
{
	int sub_win_idx;

	win_lines = tio_get_lines();
	win_columns = tio_get_columns();

	win_select_win(WIN_IDX_MAIN);
	main_win = cur_win;		// &sub_win_rects[WIN_IDX_MAIN];
	for (sub_win_idx = 0; sub_win_idx < SUB_WINS; sub_win_idx++) {
		win_select_win(sub_win_idx);
		switch(sub_win_idx) {
		case WIN_IDX_MAIN:
			cur_win->top = 0 + win_depth * 1;
			cur_win->left = 0 + win_depth * 2;
			cur_win->lines = LIM_MIN(0, tio_get_lines() - (win_depth * 1 * 2));
			cur_win->columns = LIM_MIN(0, tio_get_columns() - (win_depth * 2 * 2));
			break;
		case WIN_IDX_SUB_WHOLE:
			cur_win->top = main_win->top + main_win_get_top_win_lines();
			cur_win->left = main_win->left + 0;
			cur_win->lines = LIM_MIN(0, main_win_get_lines()
			 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
			cur_win->columns = main_win->columns;
			break;
		case WIN_IDX_SUB_LEFT:
			cur_win->top = main_win->top + main_win_get_top_win_lines();
			cur_win->left = main_win->left;
			cur_win->lines = LIM_MIN(0, main_win_get_lines()
			 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
			cur_win->columns = main_win->columns / 2;
			break;
		case WIN_IDX_SUB_RIGHT:
			cur_win->top = main_win->top + main_win_get_top_win_lines();
			cur_win->left = main_win->left + main_win->columns / 2;
			cur_win->lines = LIM_MIN(0, main_win_get_lines()
			 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
			cur_win->columns = main_win->columns - main_win->columns / 2;
			break;
		}
	}
}
void win_select_win(int sub_win_idx)
{
	cur_win_idx = MIN_MAX_(0, sub_win_idx, SUB_WINS-1);
	cur_win = &sub_win_rects[cur_win_idx];
}

void dump_cur_win(void)
{
	flf_d_printf("sub_win-%d: top-left(%d, %d), lines-columns(%d, %d)\n",
	 cur_win_idx, cur_win->top, cur_win->left, cur_win->lines, cur_win->columns);
}

//-----------------------------------------------------------------------------
int main_win_get_lines(void)
{
	return main_win->lines;
}
int main_win_get_columns(void)
{
	return main_win->columns;
}
int main_win_get_top_win_lines(void)
{
	return TITLE_LINES;
}
int main_win_get_mid_win_lines(void)
{
	return LIM_MIN(0, main_win_get_lines()
	 - main_win_get_top_win_lines() - main_win_get_bottom_win_lines());
}
int main_win_get_bottom_win_lines(void)
{
	return STATUS_LINES + get_key_list_lines();
}

int main_win_get_top_win_y(void)
{
	return 0;
}
int main_win_get_mid_win_y(void)
{
	return main_win_get_top_win_y() + main_win_get_top_win_lines();
}
int main_win_get_bottom_win_y(void)
{
	return main_win_get_top_win_y() + main_win_get_top_win_lines() + main_win_get_mid_win_lines();
}
//-----------------------------------------------------------------------------
void main_win_set_cursor_pos(int yy, int xx)
{
	win_set_cursor_pos(main_win, yy, xx);
}
void main_win_clear_screen(void)
{
	win_clear_screen(main_win);
}
void main_win_clear_lines(int line_1, int line_2)
{
	win_clear_lines(main_win, line_1, line_2);
}
void main_win_output_string(int yy, int xx, const char *string, int bytes)
{
	win_output_string(main_win, yy, xx, string, bytes);
}
//-----------------------------------------------------------------------------
int sub_win_get_lines(void)
{
	return win_get_lines(cur_win);
}
int sub_win_get_columns(void)
{
	return win_get_columns(cur_win);
}
void sub_win_set_cursor_pos(int yy, int xx)
{
	win_set_cursor_pos(cur_win, yy, xx);
}
void sub_win_clear_screen(void)
{
	win_clear_screen(cur_win);
}
void sub_win_clear_lines(int line_1, int line_2)
{
	win_clear_lines(cur_win, line_1, line_2);
}
void sub_win_output_string(int yy, int xx, const char *string, int bytes)
{
///flf_d_printf("(%d, %d)%d[%s]\n", yy, xx, bytes, string);
	win_output_string(cur_win, yy, xx, string, bytes);
}
//-----------------------------------------------------------------------------
PRIVATE int win_get_lines(win_rect_t *win)
{
	return win->lines;
}
PRIVATE int win_get_columns(win_rect_t *win)
{
	return win->columns;
}
PRIVATE void win_set_cursor_pos(win_rect_t *win, int yy, int xx)
{
	if (yy >= 0 && xx >= 0) {
		yy += win->top;
		xx += win->left;
	}
	tio_set_cursor_pos(yy, xx);
}
PRIVATE void win_clear_screen(win_rect_t *win)
{
	win_clear_lines(win, 0, win_get_lines(win));
}
PRIVATE void win_clear_lines(win_rect_t *win, int line_1, int line_2)
{
	int yy;

	if (line_2 < 0) {
		line_2 = line_1 + 1;
	}
///flf_d_printf("win_clear_lines(%d, %d)\n", line_1, line_2);
	for (yy = line_1; yy < line_2; yy++) {
		win_output_string(win, yy, 0, tio_blank_line(), win_get_columns(win));
	}
}
PRIVATE void win_output_string(win_rect_t *win, int yy, int xx, const char *string, int bytes)
{
	int space_columns;

	space_columns = win->columns;
	if (xx >= 0) {
		space_columns = win->columns - xx;
	}
	if (yy >= 0 && xx >= 0) {
		yy += win->top;
		xx += win->left;
	}
	// limit output length
	if (bytes < 0) {
		bytes = byte_idx_from_col_idx(string, space_columns, CHAR_LEFT, NULL);
	}
///flf_d_printf("(%d, %d)%d[%s]\n", yy, xx, bytes, string);
	tio_output_string(yy, xx, string, bytes);
}

//-----------------------------------------------------------------------------

const char *shrink_str_to_scr_static(const char *str)
{
	static char buf[MAX_PATH_LEN+1];

	return shrink_str_buf(buf, str, main_win_get_columns() / 2, 5);
}

// End of winout.c
} // namespace be_editor
