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
	prev_cur_edit_buf = get_c_e_b();
	set_cur_edit_buf(EDIT_BUFS_TOP_ANCH);
	buffer_free_lines(EDIT_BUFS_TOP_ANCH);
	buffer_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Files currently loaded"));
	for (edit_buf = EDIT_BUFS_TOP_BUF; IS_NODE_BOT_ANCH(edit_buf) == 0;
	 edit_buf = edit_buf->next) {
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, "%-40s %-5s %s %s %s",
		 edit_buf->file_path,
		 buffer_encode_str(edit_buf), buffer_eol_str(edit_buf),
		 BUF_STATE(edit_buf, buf_MODIFIED) ? "Mo" : "--",
		 edit_buf->abs_path);
		append_string_to_cur_edit_buf(buffer);
		if (edit_buf == prev_cur_edit_buf)
			line_to_go = CUR_EDIT_BUF_BOT_LINE;
	}
	append_magic_line();
	if (line_to_go) {
		CEBV_CL = line_to_go;
	} else {
		CEBV_CL = CUR_EDIT_BUF_TOP_LINE;
	}
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);

	post_cmd_processing(CUR_EDIT_BUF_TOP_LINE, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("File List"));
	return 1;
}
#ifdef ENABLE_HELP
int do_switch_to_key_list(void)
{
	int key_idx;
	key_code_t key;
	func_key_table_t *func_key_table;
	char *template_ = "%-5s  %-32s  %-32s";
	//			   12345678901234567890123456789012
	char *key_  = "-----";
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

_FLF_
	set_cur_edit_buf(EDIT_BUFS_TOP_ANCH);
	buffer_free_lines(EDIT_BUFS_TOP_ANCH);
	buffer_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Editor Key Bindings"));
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
	append_magic_line();
	CEBV_CL = CUR_EDIT_BUF_TOP_LINE;
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);

	post_cmd_processing(CEBV_CL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Key List"));
	return 1;
}
int do_switch_to_func_list(void)
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
	set_cur_edit_buf(EDIT_BUFS_TOP_ANCH);
	buffer_free_lines(EDIT_BUFS_TOP_ANCH);
	buffer_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Editor Functions"));
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
	append_magic_line();
	CEBV_CL = CUR_EDIT_BUF_TOP_LINE;
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);

	post_cmd_processing(CEBV_CL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Function List"));
	return 1;
}
int do_switch_to_key_bindings_list(void)
{
	func_key_table_t *table;
	int idx;
	static char buf1[MAX_KEY_NAME_LEN+1];
	char *template_ = "%-32s  %s";
	//			   12345678901234567890123456789012
	char *func_ = "--------------------------------";
	char *key_  = "-----";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

_FLF_
	set_cur_edit_buf(EDIT_BUFS_TOP_ANCH);
	buffer_free_lines(EDIT_BUFS_TOP_ANCH);
	buffer_set_file_path(EDIT_BUFS_TOP_ANCH, _("#List of Key bindings"));
	table = get_func_key_table_from_key_group(0);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, "func_id", "key");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, func_, key_);
	append_string_to_cur_edit_buf(buffer);
	for (idx = 0; table[idx].help[0]; idx++) {
		if (idx != 0 && table[idx].desc[0] == 0) {
			append_string_to_cur_edit_buf("");
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
		 table[idx].func_id,
		 short_key_name_from_key_code(table[idx].key1, buf1));
		append_string_to_cur_edit_buf(buffer);
	}
	append_magic_line();
	CEBV_CL = CUR_EDIT_BUF_TOP_LINE;
	SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);

	post_cmd_processing(CEBV_CL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Key bindings List"));
	return 1;
}
#endif // ENABLE_HELP

// End of list.c
