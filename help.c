/**************************************************************************
 *   help.c                                                               *
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

PRIVATE void make_help_file_list(be_buf_t *cur_buf);
#ifdef ENABLE_HELP
PRIVATE void make_help_func_list();
PRIVATE void make_help_key_list();
#endif // ENABLE_HELP

void init_help_bufs()
{
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create(INTERNAL_FILE_NAME_PREFIX "Editor-Files-loaded", buf_MODE_LIST));
#ifdef ENABLE_HELP
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create(INTERNAL_FILE_NAME_PREFIX "Editor-Functions", buf_MODE_LIST));
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create(INTERNAL_FILE_NAME_PREFIX "Editor-Key-Bindings", buf_MODE_LIST));
#ifdef ENABLE_FILER
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create(INTERNAL_FILE_NAME_PREFIX "Filer-Functions", buf_MODE_LIST));
	bufs_insert_buf_to_bottom(&help_buffers,
	 buf_create(INTERNAL_FILE_NAME_PREFIX "Filer-Key-Bindings", buf_MODE_LIST));
#endif // ENABLE_FILER
#endif // ENABLE_HELP
}
be_buf_t *get_help_buf(int help_buf_idx)
{
	return buf_get_buf_by_idx(HELP_BUFS_TOP_BUF, help_buf_idx);
}

//------------------------------------------------------------------------------
// make help text in help-buffer and view by editor
int view_list(int help_idx)
{
#ifdef ENABLE_HISTORY
	load_histories_if_needed();
#endif // ENABLE_HISTORY
	load_cut_buffers_if_updated();

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
		disp_status_bar_done(_("Editor File List"));
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

	return do_call_editor(1, APP_MODE_CHOOSER, get_help_buf(help_idx), NULL);

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
		append_magic_line_if_necessary();
		first_line();
		break;
#endif // ENABLE_HELP
	}
	set_epc_buf(cur_edit_buf);
}

PRIVATE void make_help_file_list(be_buf_t *cur_buf)
{
	be_line_t *line_to_go = NULL;
	for (be_bufs_t *bufs = NODES_TOP_NODE(&all_bufferss); IS_NODE_INT(bufs);
	 bufs = NODE_NEXT(bufs)) {
		append_string_to_cur_edit_buf(sprintf_s("%-s", bufs->name));
		if (IS_NODES_EMPTY(bufs)) {
			append_string_to_cur_edit_buf(sprintf_s("  == no buffer =="));
			continue;
		}
		for (be_buf_t *buf = NODES_TOP_NODE(bufs); IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
			append_string_to_cur_edit_buf(
			 sprintf_s("  %s  %-5s-%s-%s",
			  expand_str_columns(quote_file_path_s(buf_get_file_path(buf, NULL)),
			   central_win_get_columns() - win_size_shrink_columns() - (4 + 2 + 20)),
			  buf_enc_str(buf), buf_eol_str(buf),
			  GET_BUF_STATE(buf, buf_MODIFIED) ? "Mo" : "--"));
			if (buf == cur_buf) {
				line_to_go = EPCBVC_CL;
			}
		}
	}
	if (line_to_go) {
		EPCBVC_CL = line_to_go;
	} else {
		EPCBVC_CL = NODES_TOP_NODE(get_epc_buf());
	}
}

#ifdef ENABLE_HELP
PRIVATE void make_help_func_list()
{
	char buf1[MAX_KEY_NAME_LEN+1];
	char buf2[MAX_KEY_NAME_LEN+1];
	char buf3[MAX_KEY_NAME_LEN+1];
	char *template_ = "%-32s  %-*s %-*s %-*s  %-32s";
	//			   12345678901234567890123456789012
	char *func_ = "--------------------------------";
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];

	func_key_t *func_key = get_func_key_group_from_group_idx(0);
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
	for (int f_idx = 0; func_key[f_idx].explanation[0]; f_idx++) {
		if (f_idx != 0 && func_key[f_idx].desc[0] == 0) {
			append_string_to_cur_edit_buf("");
		}
		snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_, func_key[f_idx].explanation,
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(func_key[f_idx].keys[0], buf1),
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(func_key[f_idx].keys[1], buf2),
		 MAX_KEY_NAME_LEN, short_key_name_from_key_code(func_key[f_idx].keys[2], buf3),
		 func_key[f_idx].func_id);
		append_string_to_cur_edit_buf(buffer);
	}
}

struct key_code_range {
	key_code_t start;
	key_code_t end;
} key_code_ranges[] = {
	{ 0x0060, 0x007f },		// "`"  ~ "z"
	{ 0x0040, 0x005f },		// "@"  ~ "Z"
	{ 0x0030, 0x004f },		// "0"  ~ "9"
	{ 0x0020, 0x002f },		// " "  ~ "/"
	{ 0x0000, 0x001f },		// "^@" ~ "^Z"
	{ 0x1b60, 0x1b7f },		// "@`"  ~ "@z"
	{ 0x1b40, 0x1b5f },		// "@@"  ~ "@Z"
	{ 0x1b30, 0x1b4f },		// "@0"  ~ "@9"
	{ 0x1b20, 0x1b2f },		// "@ "  ~ "@/"
	{ 0x1b00, 0x1b1f },		// "@^@"  ~ "@^Z"
	{ 0x0100, 0x077f },		// other function keys
};
PRIVATE void make_help_key_list()
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
	for (int range_idx = 0; range_idx < ARRAY_SIZE_OF(key_code_ranges); range_idx++) {
		func_key_t *func_key = NULL;
		key_code_t key = K_NONE;
		for (key = key_code_ranges[range_idx].start; key < key_code_ranges[range_idx].end; key++) {
			if (get_fkey_entry_from_key(NULL, key, -1))
				break;
		}
		if (key >= key_code_ranges[range_idx].end)
			continue;		// no key assignment for the range
		for (key = key_code_ranges[range_idx].start; key < key_code_ranges[range_idx].end; key++) {
			if (is_strlen_0(long_key_name_from_key_code_null(key, NULL)))
				continue;
			key_code_t keys[MAX_KEYS_BIND];
			for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
				keys[key_idx] = K_NONE;
			}
			int keys_set = 0;
			keys[keys_set++] = key;
			func_key = get_fkey_entry_from_key(NULL, key, -1);
			for (int key_idx2 = 0; key_idx2 < MAX_KEYS_BIND; key_idx2++) {
				if (IS_KEY_VALID(func_key->keys[key_idx2])) {
					for (int key_idx = 0; key_idx < MAX_KEYS_BIND; key_idx++) {
						if (keys[key_idx] == func_key->keys[key_idx2]) {
							// already set
							break;
						}
					}
					if (key_idx2 >= MAX_KEYS_BIND) {
						// not yet set
						keys[keys_set++] = func_key->keys[key_idx2];
					}
				}
			}
			char buf1[MAX_KEY_NAME_LEN+1];
			char buf2[MAX_KEY_NAME_LEN+1];
			char buf3[MAX_KEY_NAME_LEN+1];
			snprintf_(buffer, MAX_SCRN_LINE_BUF_LEN+1, template_,
			 MAX_KEY_NAME_LEN, short_key_name_from_key_code(keys[0], buf1),
			 func_key ? func_key->explanation : "-- No function assigned --",
			 MAX_KEY_NAME_LEN, short_key_name_from_key_code(keys[1], buf2),
			 MAX_KEY_NAME_LEN, short_key_name_from_key_code(keys[2], buf3),
			 func_key ? func_key->func_id : "-- None --");
			append_string_to_cur_edit_buf(buffer);
		}
	}
}
#endif // ENABLE_HELP

//------------------------------------------------------------------------------
#ifdef ENABLE_HELP
const char *splash_text_b[] = {
//012345678901234567890123456789
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
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
		tio_flash_screen(delay);
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
			msg5_x = 27;
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
			msg5_x = 19;
			break;
		default:
		case 0:
			splash_text = splash_text_b;
			msg1_y = 1;
			msg2_y = 2;
			msg3_y = 3;
			msg4_y = 4;
			msg5_y = 5;
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
			switch (splash_text[yy][xx]) {
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
		tio_output_string(top_y + msg5_y, left_x + msg5_x,
		 _("We're invincible with this !!"), -1);
	}
	tio_refresh();
}
#endif // ENABLE_HELP

// End of help.c
