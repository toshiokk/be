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

PRIVATE void make_help_buf(int help_idx);

PRIVATE void make_help_file_list(be_buf_t *cur_edit_buf);
#ifdef ENABLE_HELP
PRIVATE void make_help_func_list(void);
PRIVATE void make_help_key_list(void);
#endif // ENABLE_HELP

void init_help_bufs(void)
{
	bufs_insert_buf_to_bottom(&help_buffers, buf_create_node(_("#List of Files loaded")));
#ifdef ENABLE_HELP
	bufs_insert_buf_to_bottom(&help_buffers, buf_create_node(_("#List of Editor Functions")));
	bufs_insert_buf_to_bottom(&help_buffers, buf_create_node(_("#List of Editor Key Bindings")));
#ifdef ENABLE_FILER
	bufs_insert_buf_to_bottom(&help_buffers, buf_create_node(_("#List of Filer Functions")));
	bufs_insert_buf_to_bottom(&help_buffers, buf_create_node(_("#List of Filer Key Bindings")));
#endif // ENABLE_FILER
#endif // ENABLE_HELP
}
be_buf_t *get_help_buf(int help_buf_idx)
{
	return buf_get_buf_by_idx(HELP_BUFS_TOP_NODE, help_buf_idx);
}

int doe_view_file_list(void)
{
	view_list(HELP_BUF_IDX_EDITOR_FILE_LIST);
	return 1;
}

//-----------------------------------------------------------------------------

// make help text in help-buffer and view by editor
int view_list(int help_idx)
{
	make_help_buf(HELP_BUF_IDX_EDITOR_FILE_LIST);
#ifdef ENABLE_HELP
	make_help_buf(HELP_BUF_IDX_EDITOR_FUNC_LIST);
	make_help_buf(HELP_BUF_IDX_EDITOR_KEY_LIST);
#ifdef ENABLE_FILER
	make_help_buf(HELP_BUF_IDX_FILER_FUNC_LIST);
	make_help_buf(HELP_BUF_IDX_FILER_KEY_LIST);
#endif // ENABLE_FILER
#endif // ENABLE_HELP

	switch (help_idx) {
	default:
	case HELP_BUF_IDX_EDITOR_FILE_LIST:
		disp_status_bar_done(_("File List"));
		break;
#ifdef ENABLE_HELP
	case HELP_BUF_IDX_EDITOR_FUNC_LIST:
		disp_status_bar_done(_("Editor Function List"));
		break;
	case HELP_BUF_IDX_EDITOR_KEY_LIST:
		disp_status_bar_done(_("Editor Key List"));
		break;
#ifdef ENABLE_FILER
	case HELP_BUF_IDX_FILER_FUNC_LIST:
		disp_status_bar_done(_("Filer Function List"));
		break;
	case HELP_BUF_IDX_FILER_KEY_LIST:
		disp_status_bar_done(_("Filer Key List"));
		break;
#endif // ENABLE_FILER
#endif // ENABLE_HELP
	}

	int ret = call_editor(1, APP_MODE_LIST, get_help_buf(help_idx), NULL, 0);

flf_d_printf("ret: %d\n", ret);
	return ret;
}
PRIVATE void make_help_buf(int help_idx)
{
	be_buf_t *cur_edit_buf = get_epc_buf();
	be_buf_t *buf = get_help_buf(help_idx);
	set_epc_buf(buf);
	buf_free_lines(buf);
	EPCBVC_CL = NULL;

	app_mode_t appmode_save;
	appmode_save = app_mode__;
	switch (help_idx) {
	default:
	case HELP_BUF_IDX_EDITOR_FILE_LIST:
		make_help_file_list(cur_edit_buf);
		break;
#ifdef ENABLE_HELP
	case HELP_BUF_IDX_EDITOR_FUNC_LIST:
		SET_APPMD_VAL(app_EDITOR_FILER, EF_EDITOR);
		make_help_func_list();
		break;
	case HELP_BUF_IDX_EDITOR_KEY_LIST:
		SET_APPMD_VAL(app_EDITOR_FILER, EF_EDITOR);
		make_help_key_list();
		break;
#ifdef ENABLE_FILER
	case HELP_BUF_IDX_FILER_FUNC_LIST:
		SET_APPMD_VAL(app_EDITOR_FILER, EF_FILER);
		make_help_func_list();
		break;
	case HELP_BUF_IDX_FILER_KEY_LIST:
		SET_APPMD_VAL(app_EDITOR_FILER, EF_FILER);
		make_help_key_list();
		break;
#endif // ENABLE_FILER
#endif // ENABLE_HELP
	}
	SET_APPMD_VAL(app_EDITOR_FILER, GET_APPMD_PTR(&appmode_save, app_EDITOR_FILER));

	switch (help_idx) {
	default:
	case HELP_BUF_IDX_EDITOR_FILE_LIST:
		if (EPCBVC_CL == NULL) {
			last_line();
		}
		break;
#ifdef ENABLE_HELP
	case HELP_BUF_IDX_EDITOR_FUNC_LIST:
	case HELP_BUF_IDX_EDITOR_KEY_LIST:
#ifdef ENABLE_FILER
	case HELP_BUF_IDX_FILER_FUNC_LIST:
	case HELP_BUF_IDX_FILER_KEY_LIST:
#endif // ENABLE_FILER
		append_magic_line();
		first_line();
		break;
#endif // ENABLE_HELP
	}

	SET_CUR_EBUF_STATE(buf_MODE, buf_MODE_LIST);	// A list must be not saveable
	post_cmd_processing(CUR_EDIT_BUFS_TOP_NODE, CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
	 UPDATE_SCRN_ALL_SOON);
	set_epc_buf(cur_edit_buf);
}

PRIVATE void make_help_file_list(be_buf_t *cur_edit_buf)
{
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	be_line_t *line_to_go = NULL;

	buf_set_file_abs_path(get_epc_buf(), _("#List of Files currently loaded"));

	for (be_buf_t *edit_buf = EDIT_BUFS_TOP_NODE; IS_NODE_INT(edit_buf);
	 edit_buf = NODE_NEXT(edit_buf)) {
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, "%-60s %-5s %s %s",
		 quote_file_path_static(edit_buf->abs_path_),
		 buf_enc_str(edit_buf), buf_eol_str(edit_buf),
		 BUF_STATE(edit_buf, buf_MODIFIED) ? "Mo" : "--");
		append_string_to_cur_edit_buf(buffer);
		if (edit_buf == cur_edit_buf) {
			line_to_go = EPCBVC_CL;
		}
	}
	EPCBVC_CL = line_to_go;
}

#ifdef ENABLE_HELP
PRIVATE void make_help_func_list(void)
{
	char buf2[MAX_KEY_NAME_LEN+1];
	char buf3[MAX_KEY_NAME_LEN+1];
	char *template_ = "%-32s  %-8s %-8s %-8s  %-32s";
	//			   12345678901234567890123456789012
	char *key_  = "--------";
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	func_key_table_t *fkey_table = get_func_key_table_from_key_group(0);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
	 "Function", "Key1", "Key2", "Key3", "func_id");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, func_, key_, key_, key_, func_);
	append_string_to_cur_edit_buf(buffer);
	for (int idx = 0; fkey_table[idx].help[0]; idx++) {
		if (idx != 0 && fkey_table[idx].desc[0] == 0) {
			append_string_to_cur_edit_buf("");
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
		 fkey_table[idx].help,
		 short_key_name_from_key_code(fkey_table[idx].key1, NULL),
		 short_key_name_from_key_code(fkey_table[idx].key2, buf2),
		 short_key_name_from_key_code(fkey_table[idx].key3, buf3),
		 fkey_table[idx].func_id);
		append_string_to_cur_edit_buf(buffer);
	}
}
PRIVATE void make_help_key_list(void)
{
	char *template_ = "%-8s  %-32s  %-8s %-8s  %-32s";
	//			   12345678901234567890123456789012
	char *key_  = "--------";
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
	 "Key", "Function", "Key2", "Key3", "func_id");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
	 key_, func_, key_, key_, func_);
	append_string_to_cur_edit_buf(buffer);
	for (int key_idx = -0x7f; key_idx < get_key_name_table_entries(); key_idx++) {
		key_code_t key;
		key_code_t key1 = K_NONE;
		key_code_t key2 = K_NONE;
		key_code_t key3 = K_NONE;
		char buf2[MAX_KEY_NAME_LEN+1];
		char buf3[MAX_KEY_NAME_LEN+1];
		if (key_idx <  0) {
			key = key_idx + 0x7f;	// key: 0x00 - 0x7e
		} else {
			key = key_name_table[key_idx].key_code;
		}
		func_key_table_t *fkey_table = get_func_key_table_from_key(
		 get_app_func_key_table(), key);
		key1 = key;
		if (fkey_table) {
			key = fkey_table->key1;
			if ((key != key1) && (key != key2) && (key != key3)) {
				if (IS_KEY_VALID(key1) == 0) { key1 = key; }
				 else if (IS_KEY_VALID(key2) == 0) { key2 = key; }
				  else if (IS_KEY_VALID(key3) == 0) { key3 = key; }
			}
			key = fkey_table->key2;
			if ((key != key1) && (key != key2) && (key != key3)) {
				if (IS_KEY_VALID(key1) == 0) { key1 = key; }
				 else if (IS_KEY_VALID(key2) == 0) { key2 = key; }
				  else if (IS_KEY_VALID(key3) == 0) { key3 = key; }
			}
			key = fkey_table->key3;
			if ((key != key1) && (key != key2) && (key != key3)) {
				if (IS_KEY_VALID(key1) == 0) { key1 = key; }
				 else if (IS_KEY_VALID(key2) == 0) { key2 = key; }
				  else if (IS_KEY_VALID(key3) == 0) { key3 = key; }
			}
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
		 short_key_name_from_key_code(key1, NULL),
		 fkey_table ? fkey_table->help : "-- No function assigned --",
		 short_key_name_from_key_code(key2, buf2),
		 short_key_name_from_key_code(key3, buf3),
		 fkey_table ? fkey_table->func_id : "-- None --");
		append_string_to_cur_edit_buf(buffer);
	}
}
#endif // ENABLE_HELP

// End of editorlist.c
