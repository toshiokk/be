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
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create_node(_("#List of Files loaded"), buf_MODE_LIST));
#ifdef ENABLE_HELP
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create_node(_("#List of Editor Functions"), buf_MODE_LIST));
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create_node(_("#List of Editor Key Bindings"), buf_MODE_LIST));
#ifdef ENABLE_FILER
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create_node(_("#List of Filer Functions"), buf_MODE_LIST));
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create_node(_("#List of Filer Key Bindings"), buf_MODE_LIST));
#endif // ENABLE_FILER
#endif // ENABLE_HELP
}
be_buf_t *get_help_buf(int help_buf_idx)
{
	return buf_get_buf_by_idx(HELP_BUFS_TOP_BUF, help_buf_idx);
}

int doe_view_file_list(void)
{
	view_list(HELP_BUF_IDX_EDITOR_FILE_LIST);
	return 1;
}

//------------------------------------------------------------------------------

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
	bufs_renumber_all_bufs_from_top(&help_buffers);

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

	int ret = do_call_editor(1, APP_MODE_CHOOSER, get_help_buf(help_idx), NULL, 0);

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

	SET_CUR_EBUF_STATE(buf_MODE, buf_MODE_LIST);	// A list
	set_epc_buf(cur_edit_buf);
}

PRIVATE void make_help_file_list(be_buf_t *cur_edit_buf)
{
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	be_line_t *line_to_go = NULL;

	buf_set_file_path(get_epc_buf(), _("#List of Files currently loaded"));

	for (be_buf_t *edit_buf = EDIT_BUFS_TOP_BUF; IS_NODE_INT(edit_buf);
	 edit_buf = NODE_NEXT(edit_buf)) {
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, "%-60s %-5s %s %s %04x",
		 quote_file_path_static(buf_get_abs_path(edit_buf, NULL)),
		 buf_enc_str(edit_buf), buf_eol_str(edit_buf),
		 BUF_STATE(edit_buf, buf_MODIFIED) ? "Mo" : "--",
		 edit_buf->orig_file_crc);
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
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char buf3[MAX_KEY_NAME_LEN+1];
	char *template_ = "%-32s  %-*s %-*s %-*s  %-32s";
	//			   12345678901234567890123456789012
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	func_key_list_t *fkey_list = get_func_key_group_from_group_idx(0);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, "Function",
	 MAX_KEY_NAME_LEN, "Key1",
	 MAX_KEY_NAME_LEN, "Key2",
	 MAX_KEY_NAME_LEN, "Key3",
	 "func_id");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, func_,
	 MAX_KEY_NAME_LEN, long_key_none_str(),
	 MAX_KEY_NAME_LEN, long_key_none_str(),
	 MAX_KEY_NAME_LEN, long_key_none_str(),
	 func_);
	append_string_to_cur_edit_buf(buffer);
	for (int f_idx = 0; fkey_list[f_idx].explanation[0]; f_idx++) {
		if (f_idx != 0 && fkey_list[f_idx].desc[0] == 0) {
			append_string_to_cur_edit_buf("");
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, fkey_list[f_idx].explanation,
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(fkey_list[f_idx].keys[0], buf1),
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(fkey_list[f_idx].keys[1], buf2),
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(fkey_list[f_idx].keys[2], buf3),
		 fkey_list[f_idx].func_id);
		append_string_to_cur_edit_buf(buffer);
	}
}
PRIVATE void make_help_key_list(void)
{
	char *template_ = "%-*s  %-32s  %-*s %-*s  %-32s";
	//			   12345678901234567890123456789012
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
	 MAX_KEY_NAME_LEN, "Key",
	 "Function",
	 MAX_KEY_NAME_LEN, "Key2",
	 MAX_KEY_NAME_LEN, "Key3",
	 "func_id");
	append_string_to_cur_edit_buf(buffer);
	snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
	 MAX_KEY_NAME_LEN, long_key_none_str(),
	 func_,
	 MAX_KEY_NAME_LEN, long_key_none_str(),
	 MAX_KEY_NAME_LEN, long_key_none_str(),
     func_);
	append_string_to_cur_edit_buf(buffer);
	int key_idx;
	for (key_idx = -0x60; key_idx < 0; key_idx++) {
		key_code_t key = key_idx + 0x60 + ' ';
		// key = [' ', 0x7f]
		func_key_list_t *fkey_list = get_fkey_entry_table_from_key(NULL, key, 0, 2);
		if (fkey_list) {
			break;
		}
	}
	for ( ; key_idx < get_key_name_table_entries(); key_idx++) {
		key_code_t key;
		char buf1[MAX_KEY_NAME_LEN+1];
		char buf2[MAX_KEY_NAME_LEN+1];
		char buf3[MAX_KEY_NAME_LEN+1];
		if (key_idx < 0) {
			key = key_idx + 0x60 + ' ';
			// key = [' ', 0x7f]
		} else {
			key = key_name_table[key_idx].key_code;
		}
		func_key_list_t *fkey_list = get_fkey_entry_table_from_key(NULL, key, 0, 2);
		key_code_t keys[MAX_KEYS_BIND];
		for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
			keys[key_idx] = K_NONE;
		}
		int keys_set = 0;
		keys[keys_set++] = key;
		if (fkey_list) {
			for (int key_idx_from = 0; key_idx_from < MAX_KEYS_BIND; key_idx_from++) {
				if (IS_KEY_VALID(fkey_list->keys[key_idx_from])) {
					for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
						if (keys[key_idx] == fkey_list->keys[key_idx_from]) {
							// already set
							break;
						}
					}
					if (key_idx >= MAX_KEYS_BIND) {
						// not yet set
						keys[keys_set++] = fkey_list->keys[key_idx_from];
					}
				}
			}
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(keys[0], buf1),
		 fkey_list ? fkey_list->explanation : "-- No function assigned --",
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(keys[1], buf2),
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(keys[2], buf3),
		 fkey_list ? fkey_list->func_id : "-- None --");
		append_string_to_cur_edit_buf(buffer);
	}
}
#endif // ENABLE_HELP

// End of editorlist.c
