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

editor_panes_t *cur_editor_panes = NULL;	// Current Editor Panes

// collection of collections of buffers
be_bufss_t all_bufferss;

// collection of edit buffers--------------------------------------------------
be_bufs_t edit_buffers;

// collection of cut-buffers---------------------------------------------------
be_bufs_t cut_buffers;

#ifdef ENABLE_HISTORY
// history buffers ------------------------------------------------------------
be_bufs_t history_buffers;
#endif // ENABLE_HISTORY

// help buffers ---------------------------------------------------------------
be_bufs_t help_buffers;

#ifdef ENABLE_UNDO
// undo buffers ---------------------------------------------------------------
be_bufs_t undo_buffers;
// redo buffers ---------------------------------------------------------------
be_bufs_t redo_buffers;
#endif // ENABLE_UNDO

//=============================================================================

// Initialize collection of buffers.
void init_bufferss(void)
{
	bufss_init(&all_bufferss, "###ALL_BUFSS", "##ALL_BUFSS_top_anch", "##ALL_BUFSS_bot_anch");

	bufss_insert_bufs_to_bottom(&all_bufferss, bufs_init(&edit_buffers,
	 "##edit_bufs", "#edit_bufs_top_anch", "#edit_bufs_bot_anch"));
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&cut_buffers,
	 "##cut_bufs", "#cut_bufs_top_anch", "#cut_bufs_bot_anch"));
#ifdef ENABLE_HISTORY
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&history_buffers,
	 "##hist_bufs", "#hist_bufs_top_anch", "#hist_bufs_bot_anch"));
#endif // ENABLE_HISTORY
#ifdef ENABLE_HELP
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&help_buffers,
	 "##help_bufs", "#help_bufs_top_anch", "#help_bufs_bot_anch"));
#endif // ENABLE_HELP
#ifdef ENABLE_UNDO
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&undo_buffers,
	 "##undo_bufs", "#undo_bufs_top_anch", "#undo_bufs_bot_anch"));
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&redo_buffers,
	 "##redo_bufs", "#redo_bufs_top_anch", "#redo_bufs_bot_anch"));
#endif // ENABLE_UNDO

#ifdef ENABLE_HELP
	init_help_bufs();
#endif // ENABLE_HELP
}

void free_all_buffers(void)
{
	bufs_free_all_bufs(NODES_TOP_ANCH(&all_bufferss));
}

// Edit-buffer manipulation routines

// free current be_buf_t
// the next or previous buffer will be set to current
int free_cur_edit_buf(void)
{
	return free_edit_buf(get_epc_buf());
}
int free_edit_buf(be_buf_t *edit_buf)
{
	if (IS_NODE_INT(edit_buf) == 0) {
		return 0;
	}
	disp_status_bar_ing(_("Freeing edit buffer %s ..."), get_epc_buf()->file_path_);
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_FILE, mk_cur_file_pos_str_static());
#endif // ENABLE_HISTORY
	int ret = 1;
	if (edit_buf == get_epc_buf()) {
		// select other buffer
		ret = switch_epc_buf_to_another_buf();
	}
#ifdef ENABLE_UNDO
	// free undo/redo buffers related to this edit_buf
	delete_undo_redo_buf(edit_buf);
#endif // ENABLE_UNDO
	buf_unlink_free(edit_buf);
	return ret;		// 0: no buffer remains
}
//-----------------------------------------------------------------------------

// 4 line pointers are referenced from one editor_panes
//	cur_editor_panes->bufs[0]->buf_views[0]
//	cur_editor_panes->bufs[0]->buf_views[1] (This pass would be accessed usually) ????
//	cur_editor_panes->bufs[1]->buf_views[0] (This pass would be accessed usually) ????
//	cur_editor_panes->bufs[1]->buf_views[1]

void buf_avoid_wild_ptr_cur(be_buf_t *buf)
{
	// avoid cur_editor_panes->bufs[0] becoming wild-pointer 
	buf_avoid_wild_ptr(buf, &(cur_editor_panes->bufs[0]));
	// avoid cur_editor_panes->bufs[1] becoming wild-pointer 
	buf_avoid_wild_ptr(buf, &(cur_editor_panes->bufs[1]));
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
	// avoid EPCBVX_CL(0) becoming wild-pointer
	line_avoid_wild_ptr(&EPCBVX_CL(0), line);
	// avoid EPCBVX_CL(1) becoming wild-pointer
	line_avoid_wild_ptr(&EPCBVX_CL(1), line);
}
void line_avoid_wild_ptr(be_line_t **line_ptr, be_line_t *line)
{
	if (*line_ptr == line) {
		if (IS_NODE_TOP_MOST(*line_ptr) == 0) {
			*line_ptr = NODE_PREV(*line_ptr);
		} else
		if (IS_NODE_BOT_MOST(*line_ptr) == 0) {
			*line_ptr = NODE_NEXT(*line_ptr);
		}
	}
}
//-----------------------------------------------------------------------------
// Editor view management

void set_cur_editor_panes(editor_panes_t *editor_panes)
{
	cur_editor_panes = editor_panes;
}
void init_cur_editor_panes(editor_panes_t *eps, be_buf_t *buf)
{
	editor_panes_t *prev_eps = cur_editor_panes;	// save pointer
	set_cur_editor_panes(eps);
	if (prev_eps == NULL) {
		// No prev_eps, this means 'eps' is the root view. set default initial values.
		for (int pane_idx = 0; pane_idx < EDITOR_PANES; pane_idx++) {
			set_epx_buf(pane_idx, EDIT_BUFS_TOP_NODE);
		}
		set_editor_cur_pane_idx(0);
	} else {
		// There is prev_eps, copy it.
		copy_editor_panes(cur_editor_panes, prev_eps);
	}
	if (buf) {
///		for (int pane_idx = 0; pane_idx < EDITOR_PANES; pane_idx++) {
			set_epx_buf(-1, buf);	// set only to the current pane
///		}
	}
}
void destroy_editor_panes()
{
	// nothing to do
}
void copy_editor_panes(editor_panes_t *dest, editor_panes_t *src)
{
	memcpy__(dest, src, sizeof(*src));
}

void set_epc_buf(be_buf_t *buf)	// set edit buffer to current pane
{
	set_epx_buf(-1, buf);
}
be_buf_t *get_epc_buf(void)
{
	return get_epx_buf(-1);
}
be_buf_view_t *get_epc_buf_view(void)
{
	return &(get_epc_buf()->buf_views[get_editor_cur_pane_idx()]);
}

void set_epx_buf(int pane_idx, be_buf_t *buf)
{
	if (pane_idx < 0) {
		pane_idx = get_editor_cur_pane_idx();
	}
	if (buf) {
_D_(buf_dump_name(buf))
		cur_editor_panes->bufs[pane_idx] = buf;
	}
}

be_buf_t *get_epx_buf(int pane_idx)
{
	if (pane_idx < 0) {
		pane_idx = get_editor_cur_pane_idx();
	}
	return cur_editor_panes->bufs[pane_idx];
}

be_bufs_t *set_cur_buf_to_bufs(be_buf_t *buf)
{
	buf = buf_make_buf_intermediate(buf);
	be_bufs_t *bufs = bufs_get_bufs_contains_buf(NODES_TOP_ANCH(&all_bufferss), buf);
	if (IS_NODE_INT(bufs)) {
		bufs->cur_buf = buf;	// set as a current
	}
	return bufs;
}

void set_editor_app_mode_on_cur_buf_mode()
{
	// select editor-app-mode depending on the mode of current buffer
	switch (CUR_EBUF_STATE(buf_MODE)) {
	default:
	case buf_MODE_EDIT:
		SET_APPMD_VAL(app_LIST_MODE, APP_MODE_NORMAL);		break;
	case buf_MODE_VIEW:
		SET_APPMD_VAL(app_LIST_MODE, APP_MODE_VIEWER);		break;
	case buf_MODE_ANCH:
	case buf_MODE_LIST:
		SET_APPMD_VAL(app_LIST_MODE, APP_MODE_LIST);		break;
	}
}
int is_epc_buf_view_mode(void)
{
	// avoid crash on the modification
	if (IS_NODES_EMPTY(get_epc_buf())) {
		return 1;
	}

	switch (CUR_EBUF_STATE(buf_MODE)) {
	default:
	case buf_MODE_EDIT:
		return 0;		// modifiable
	case buf_MODE_VIEW:
	case buf_MODE_LIST:
	case buf_MODE_ANCH:
		return 1;		// view only
	}
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_editor_panes(editor_panes_t *eps)
{
flf_d_printf("{{ %p\n", eps);
	if (eps == NULL) {
		eps = cur_editor_panes;
	}
flf_d_printf("cur_pane_idx: %d\n", get_editor_cur_pane_idx());
	dump_buf_views(eps->bufs[get_editor_cur_pane_idx()]);
///	flf_d_printf("pane_idx:0 ---------------------------------------------\n");
///	dump_editor_pane_x(eps, 0);
///	flf_d_printf("pane_idx:1 ---------------------------------------------\n");
///	dump_editor_pane_x(eps, 1);
	flf_d_printf("}}\n");
}
void dump_editor_pane_x(editor_panes_t *eps, int pane_idx)
{
	dump_buf_view_x(eps->bufs[pane_idx], pane_idx);
}
void dump_buf_views(be_buf_t *buf)
{
	dump_buf_view_x(buf, 0);
	dump_buf_view_x(buf, 1);
}
void dump_buf_view_x(be_buf_t *buf, int pane_idx)
{
	flf_d_printf("pane_idx: %d ------------------\n", pane_idx);
	if ((buf_check_line_in_buf(buf, BUFVX_CL(buf, pane_idx)) == NULL)
	 && (buf_check_line_in_buf_anchs(buf, BUFVX_CL(buf, pane_idx)) == NULL)) {
		warning_printf("pane[%d].cur_line is not in cur_buf!!!!\n", pane_idx);
	}
	buf_dump_name(buf);
	line_dump_byte_idx(BUFVX_CL(buf, pane_idx), BUFVX_CLBI(buf, pane_idx));
	flf_d_printf(
	 "BUFVX_CURS_Y(buf, pane_idx): %d, BUFVX_CURS_X_TO_KEEP(buf, pane_idx): %d,"
	 " BUFVX_MIN_TEXT_X_TO_KEEP(buf, pane_idx): %d\n",
	 BUFVX_CURS_Y(buf, pane_idx), BUFVX_CURS_X_TO_KEEP(buf, pane_idx),
	 BUFVX_MIN_TEXT_X_TO_KEEP(buf, pane_idx));
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------

be_buf_t *get_edit_buf_by_file_path(const char *abs_path)
{
	return buf_get_buf_by_file_path(EDIT_BUFS_TOP_NODE, abs_path);
}
be_buf_t *get_edit_buf_by_file_name(const char *file_name)
{
	return buf_get_buf_by_file_name(EDIT_BUFS_TOP_NODE, file_name);
}

//-----------------------------------------------------------------------------
// create new be_buf_t
void create_edit_buf(const char *full_path)
{
	be_buf_t *buf = buf_create_node(full_path);
	// copy default encoding from EDIT_BUFS_BOT_ANCH
#ifdef USE_NKF
	SET_BUF_STATE(buf, buf_ENCODE, BUF_STATE(EDIT_BUFS_BOT_ANCH, buf_ENCODE));
#endif // USE_NKF
	bufs_insert_buf_to_bottom(&edit_buffers, buf);
	set_epc_buf(buf);
	if (IS_NODE_INT(cur_editor_panes->bufs[0]) == 0) {
		// make view-0 buffer valid
		cur_editor_panes->bufs[0] = buf;
	}
	if (IS_NODE_INT(cur_editor_panes->bufs[1]) == 0) {
		// make view-1 buffer valid
		cur_editor_panes->bufs[1] = buf;
	}
}

//-----------------------------------------------------------------------------
// Append a new line to the bottom of the current buffer
be_line_t *append_string_to_cur_edit_buf(const char *string)
{
	EPCBVX_CL(0) = EPCBVX_CL(1) = line_insert_with_string(CUR_EDIT_BUFS_BOT_ANCH, INSERT_BEFORE,
	 string);
	EPCBVX_CLBI(0) = EPCBVX_CLBI(1) = 0;
	return EPCBVC_CL;
}

// Append a new magic-line to the bottom of the current buffer
void append_magic_line(void)
{
	if (buf_is_empty(get_epc_buf())
	 || ((buf_is_empty(get_epc_buf()) == 0) && line_data_strlen(CUR_EDIT_BUFS_BOT_NODE))) {
		append_string_to_cur_edit_buf("");
	}
}

int edit_bufs_count_bufs(void)
{
	return bufs_count_bufs(&edit_buffers);
}
int epc_buf_count_bufs(void)
{
	return buf_count_bufs(get_epc_buf());
}

int is_epc_buf_valid(void)
{
	return IS_NODE_INT(get_epc_buf());
}

// Cut-buffers manipulation routines -----------------------------------------

void free_all_cut_bufs(void)
{
	while (IS_NODE_INT(TOP_BUF_OF_CUT_BUFS)) {
		pop__free_from_cut_buf();
	}
}
be_buf_t *push_cut_buf(void)
{
	be_buf_t *buf;
	char buf_name[MAX_PATH_LEN+1];

	snprintf(buf_name, MAX_PATH_LEN, "#cut-buffer-%02d", count_cut_bufs());
	buf = buf_create_node(buf_name);
	buf_insert_after(CUT_BUFS_TOP_ANCH, buf);
	// copy cut-mode to cut-buffer
	SET_CUR_CBUF_STATE(buf_CUT_MODE, CUR_EBUF_STATE(buf_CUT_MODE));
	return buf;
}
int pop__free_from_cut_buf(void)
{
	if (IS_NODE_BOT_ANCH(TOP_BUF_OF_CUT_BUFS))
		return 0;
	buf_unlink_free(TOP_BUF_OF_CUT_BUFS);
	return 1;
}
be_line_t *append_string_to_cur_cut_buf(const char *string)
{
	return line_insert_with_string(CUR_CUT_BUFS_BOT_ANCH, INSERT_BEFORE, string);
}
int count_cut_bufs(void)
{
	return bufs_count_bufs(&cut_buffers);
}
int count_cur_cut_buf_lines(void)
{
	return buf_count_lines(TOP_BUF_OF_CUT_BUFS);
}

//-----------------------------------------------------------------------------

void renumber_cur_buf_from_top(void)
{
	buf_renumber_from_top(get_epc_buf());
}

be_line_t *get_line_ptr_in_cur_buf_by_line_num(int line_num)
{
	return buf_get_line_ptr_from_line_num(get_epc_buf(), line_num);
}

//-----------------------------------------------------------------------------

void update_cur_buf_crc(void)
{
	disp_status_bar_ing(_("Calculating CRC..."));
	buf_update_crc(get_epc_buf());
}
int check_cur_buf_modified(void)
{
	int modified = 0;

	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		disp_status_bar_ing(_("Calculating CRC..."));
		modified = buf_check_crc(get_epc_buf());
		if (modified == 0) {
			// clear "modified" flag if it's actually not modified
			SET_CUR_EBUF_STATE(buf_MODIFIED, 0);
		}
	}
	return modified;
}
// If "modified" is not set, set it and update titlebar.
void set_cur_buf_modified(void)
{
	if (CUR_EBUF_STATE(buf_MODIFIED) == 0) {
		SET_CUR_EBUF_STATE(buf_MODIFIED, 1);
	}
}
int is_any_edit_buf_modified(void)
{
	for (be_buf_t *edit_buf = EDIT_BUFS_TOP_NODE; IS_NODE_INT(edit_buf);
	 edit_buf = NODE_NEXT(edit_buf)) {
		if (BUF_STATE(edit_buf, buf_MODIFIED)) {
			return 1;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------

int tog_buf_view_mode(void)
{
	INC_CUR_EBUF_STATE(buf_MODE, buf_MODE_EDIT, buf_MODE_LIST);
	return 0;
}
const char *get_str_buf_view_mode(void)
{
	return buf_mode_str(get_epc_buf());
}

int tog_buf_line_wrap_mode(void)
{
	TOGGLE_CUR_EBUF_STATE(buf_LINE_WRAP_MODE);
	return 0;
}
const char *get_str_buf_line_wrap_mode(void)
{
	return BOOL_TO_ON_OFF(CUR_EBUF_STATE(buf_LINE_WRAP_MODE));
}

int tog_buf_tab_size(void)
{
	CUR_EBUF_STATE(buf_TAB_SIZE)
	 = (CUR_EBUF_STATE(buf_TAB_SIZE) == 8) ? 4 : 8;
	set_wrap_line_tab_size_from_cur_buf();
	return 0;
}
int inc_buf_tab_size(void)
{
	INC_CUR_EBUF_STATE(buf_TAB_SIZE, TAB_SIZE_MIN, TAB_SIZE_MAX);
	set_wrap_line_tab_size_from_cur_buf();
	return 0;
}
const char *get_str_buf_tab_size(void)
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
// update tab-size in wrap-line from the current buffer
void set_wrap_line_tab_size_from_cur_buf(void)
{
	set_wrap_line_tab_size(get_cur_buf_tab_size());
}

int set_buf_nix_file(void)
{
	set_buf_eol(EOL_NIX);
	return 0;
}
const char *get_str_buf_nix_file(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_EOL, EOL_NIX));
}
int set_buf_mac_file(void)
{
	set_buf_eol(EOL_MAC);
	return 0;
}
const char *get_str_buf_mac_file(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_EOL, EOL_MAC));
}
int set_buf_dos_file(void)
{
	set_buf_eol(EOL_DOS);
	return 0;
}
const char *get_str_buf_dos_file(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_EOL, EOL_DOS));
}
int set_buf_eol(int eol)
{
	CUR_EBUF_STATE(buf_EOL) = eol;
	return 0;
}
const char *get_str_buf_eol(void)
{
	return buf_eol_str(get_epc_buf());
}

int set_buf_enc_ascii(void)
{
	set_buf_encode(ENCODE_ASCII);
	return 0;
}
const char *get_str_buf_enc_ascii(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_ASCII));
}
int set_buf_enc_utf8(void)
{
	set_buf_encode(ENCODE_UTF8);
	return 0;
}
const char *get_str_buf_enc_utf8(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8));
}
#ifdef USE_NKF
int set_buf_enc_eucjp(void)
{
	set_buf_encode(ENCODE_EUCJP);
	return 0;
}
const char *get_str_buf_enc_eucjp(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_EUCJP));
}
int set_buf_enc_sjis(void)
{
	set_buf_encode(ENCODE_SJIS);
	return 0;
}
const char *get_str_buf_enc_sjis(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_SJIS));
}
int set_buf_enc_jis(void)
{
	set_buf_encode(ENCODE_JIS);
	return 0;
}
const char *get_str_buf_enc_jis(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_JIS));
}
#endif // USE_NKF
int set_buf_enc_binary(void)
{
	set_buf_encode(ENCODE_BINARY);
	return 0;
}
const char *get_str_buf_enc_binary(void)
{
	return BOOL_TO_ON_OFF(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_BINARY));
}

int set_buf_encode(int encode)
{
	CUR_EBUF_STATE(buf_ENCODE) = encode;
	return 0;
}
const char *get_str_buf_encode(void)
{
	return buf_enc_str(get_epc_buf());
}

//-----------------------------------------------------------------------------

int doe_tog_buf_view_mode(void)
{
	tog_buf_view_mode();
	SHOW_MODE("View mode", get_str_buf_view_mode());
	return 0;
}
int doe_tog_buf_line_wrap_mode(void)
{
	tog_buf_line_wrap_mode();
	SHOW_MODE("Line-wrap mode", get_str_buf_line_wrap_mode());

	EPCBVC_MIN_TEXT_X_TO_KEEP = 0;
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_tog_buf_tab_size(void)
{
	tog_buf_tab_size();
	SHOW_MODE(_("Tab size"), get_str_buf_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_inc_buf_tab_size(void)
{
	inc_buf_tab_size();
	SHOW_MODE(_("Tab size"), get_str_buf_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_set_buf_nix_file(void)
{
	set_buf_nix_file();
	SHOW_MODE("File format", get_str_buf_eol());
	return 0;
}
int doe_set_buf_mac_file(void)
{
	set_buf_mac_file();
	SHOW_MODE("File format", get_str_buf_eol());
	return 0;
}
int doe_set_buf_dos_file(void)
{
	set_buf_dos_file();
	SHOW_MODE("File format", get_str_buf_eol());
	return 0;
}
//-----------------------------------------------------------------------------
int doe_set_buf_enc_ascii(void)
{
	set_buf_enc_ascii();
	SHOW_MODE("ASCII format", get_str_buf_enc_ascii());
	return 0;
}
int doe_set_buf_enc_utf8(void)
{
	set_buf_enc_utf8();
	SHOW_MODE("UTF-8 format", get_str_buf_enc_utf8());
	return 0;
}
#ifdef USE_NKF
int doe_set_buf_enc_eucjp(void)
{
	set_buf_enc_eucjp();
	SHOW_MODE("EUC-JP format", get_str_buf_enc_eucjp());
	return 0;
}
int doe_set_buf_enc_sjis(void)
{
	set_buf_enc_sjis();
	SHOW_MODE("SJIS format", get_str_buf_enc_sjis());
	return 0;
}
int doe_set_buf_enc_jis(void)
{
	set_buf_enc_jis();
	SHOW_MODE("JIS format", get_str_buf_enc_jis());
	return 0;
}
#endif // USE_NKF
int doe_set_buf_enc_binary(void)
{
	set_buf_enc_binary();
	SHOW_MODE("BINARY format", get_str_buf_enc_binary());
	return 0;
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_cur_edit_buf_lines(void)
{
	buf_dump_lines(get_epc_buf(), 3);
}
void dump_edit_bufs(void)
{
	buf_dump_bufs(EDIT_BUFS_TOP_NODE);
}
void dump_edit_bufs_lines(void)
{
	buf_dump_bufs_lines(EDIT_BUFS_TOP_NODE, "edit-bufs");
}
void dump_cut_bufs(void)
{
	buf_dump_bufs(CUT_BUFS_TOP_NODE);
}
void dump_cut_bufs_lines(void)
{
	buf_dump_bufs_lines(CUT_BUFS_TOP_NODE, "cut-bufs");
}

// dump current buffer
void dump_cur_edit_buf(void)
{
	flf_d_printf("<<<\n");
	flf_d_printf("CUR_EDIT_BUFS_TOP_NODE:%08lx\n", CUR_EDIT_BUFS_TOP_NODE);
	if (CUR_EDIT_BUFS_TOP_NODE) {
		flf_d_printf("CUR_EDIT_BUFS_TOP_NODE->data:%08lx\n", CUR_EDIT_BUFS_TOP_NODE->data);
	}
	flf_d_printf("CUR_EDIT_BUFS_BOT_NODE:%08lx\n", CUR_EDIT_BUFS_BOT_NODE);
	if (CUR_EDIT_BUFS_BOT_NODE) {
		flf_d_printf("CUR_EDIT_BUFS_BOT_NODE->data:%08lx\n", CUR_EDIT_BUFS_BOT_NODE->data);
	}
	flf_d_printf("cur_line:%08lx\n", EPCBVC_CL);
	if (EPCBVC_CL) {
		flf_d_printf("cur_line->data:%08lx\n", EPCBVC_CL->data);
	}
	line_dump_lines(CUR_EDIT_BUFS_TOP_ANCH, INT_MAX, EPCBVC_CL);
	flf_d_printf(">>>\n");
}
#endif // ENABLE_DEBUG

/////_D_(dump_editor_panes())
/////_D_(dump_editor_panes())
/////_D_(dump_editor_panes())
/////_D_(buf_dump_name(buf))
/////_D_(dump_file_type(cur_file_type, 0))
/////_D_(dump_editor_panes())
/////_D_(dump_editor_panes())
/////_D_(dump_editor_panes())
/////_D_(dump_editor_panes())
/////_D_(line_dump_byte_idx(EPCBVC_CL, EPCBVC_CLBI))
/////_D_(dump_editor_panes())
/////_D_(dump_editor_panes())
/////_D_(dump_buf_views(edit_buf_save))
/////_D_(dump_buf_views(edit_buf_save))
/////_D_(line_dump_byte_idx(EPCBVC_CL, 0))
/////_D_(line_dump_byte_idx(EPCBVC_CL, 0))
/////_D_(buf_dump_name(cur_edit_buf))
/////_D_(buf_dump_name(edit_buf))
/////_D_(line_dump_byte_idx(EPCBVC_CL, 0))
/////_D_(line_dump_byte_idx(EPCBVC_CL, 0))
/////_D_(line_dump_byte_idx(EPCBVC_CL, 0))

// End of buffers.c
