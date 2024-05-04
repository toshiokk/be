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

int doe_switch_to_file_list(void)
{
	be_buf_t *prev_cur_edit_buf;
	be_line_t *line_to_go = NULL;
	be_buf_t *edit_buf;
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	char buf[MAX_PATH_LEN+1];

	prev_cur_edit_buf = get_cep_buf();
	set_cep_buf(EDIT_BUFS_TOP_ANCH);
	buf_free_lines(EDIT_BUFS_TOP_ANCH);
	buf_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Files currently loaded"));
	for (edit_buf = EDIT_BUFS_TOP_BUF; IS_NODE_INT(edit_buf);
	 edit_buf = NODE_NEXT(edit_buf)) {
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, "%-60s %-5s %s %s %s",
		 quote_file_name_static(edit_buf->abs_path),
		 buf_encode_str(edit_buf), buf_eol_str(edit_buf),
		 BUF_STATE(edit_buf, buf_MODIFIED) ? "Mo" : "--",
		 quote_file_name_buf(buf, edit_buf->abs_path));
		append_string_to_cur_edit_buf(buffer);
		if (edit_buf == prev_cur_edit_buf)
			line_to_go = CUR_EDIT_BUF_BOT_LINE;
	}
	append_magic_line();
	if (line_to_go) {
		CEPBV_CL = line_to_go;
	} else {
		CEPBV_CL = CUR_EDIT_BUF_TOP_LINE;
	}
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);

	post_cmd_processing(CUR_EDIT_BUF_TOP_LINE, CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
	 UPDATE_SCRN_ALL_SOON);
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
	buf_insert_before(HELP_BUFS_BOT_ANCH, buf_create_node(_("#List of Editor Key Bindings")));
	buf_insert_before(HELP_BUFS_BOT_ANCH, buf_create_node(_("#List of Editor Functions")));
}
be_buf_t *get_help_buf(int help_buf_idx)
{
	return get_buf_from_bufs_by_idx(HELP_BUFS_TOP_BUF, help_buf_idx);
}

//-----------------------------------------------------------------------------
int make_help_buf_call_editor(int help_idx);
void make_help_buf(int help_idx);
void make_help_key_list(void);
void make_help_func_list(void);

int doe_switch_to_key_list(void)
{
	make_help_buf_call_editor(HELP_BUF_IDX_KEY_LIST);
	return 1;
}
int doe_switch_to_func_list(void)
{
	make_help_buf_call_editor(HELP_BUF_IDX_FUNC_LIST);
	return 1;
}

int make_help_buf_call_editor(int help_idx)
{
	make_help_buf(HELP_BUF_IDX_KEY_LIST);
	make_help_buf(HELP_BUF_IDX_FUNC_LIST);

	be_buf_t *edit_buf_save = get_cep_buf();
	set_cep_buf(get_help_buf(help_idx));

	switch (help_idx) {
	default:
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
	switch (help_idx) {
	default:
	case HELP_BUF_IDX_KEY_LIST:
		make_help_key_list();
		break;
	case HELP_BUF_IDX_FUNC_LIST:
		make_help_func_list();
		break;
	}
	append_magic_line();
	CEPBV_CL = CUR_EDIT_BUF_TOP_LINE;
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

// End of editorlist.c
