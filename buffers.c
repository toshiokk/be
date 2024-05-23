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

// collection of collections of buffers
be_bufs_t bufss_top_anchor;		//< top buffers
be_bufs_t bufss_bot_anchor;		//< bottom buffers

// collection of edit buffers--------------------------------------------------
be_bufs_t edit_buffers;

// collection of cut-buffers---------------------------------------------------
be_bufs_t cut_buffers;

// history buffers ------------------------------------------------------------
be_bufs_t history_buffers;

// help buffers ---------------------------------------------------------------
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
	bufs_init(&bufss_top_anchor, "##ALL_BUFS-top-anchor");
	bufs_init(&bufss_bot_anchor, "##ALL_BUFS-bot-anchor");
	bufs_link(&bufss_top_anchor, &bufss_bot_anchor);

	bufs_insert_before(&bufss_bot_anchor, bufs_init(&edit_buffers, "##edit-buffers"));
	bufs_insert_before(&bufss_bot_anchor, bufs_init(&cut_buffers, "##cut-buffers"));
	bufs_insert_before(&bufss_bot_anchor, bufs_init(&history_buffers, "##history-buffers"));
	bufs_insert_before(&bufss_bot_anchor, bufs_init(&help_buffers, "##help-buffers"));
#ifdef ENABLE_UNDO
	bufs_insert_before(&bufss_bot_anchor, bufs_init(&undo_buffers, "##undo-buffers"));
	bufs_insert_before(&bufss_bot_anchor, bufs_init(&redo_buffers, "##redo-buffers"));
#endif // ENABLE_UNDO
}

// Initialize global variables.
void init_buffers(void)
{
	init_head_of_bufs();

	init_edit_bufs();
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
	bufs_free_all_bufs(&bufss_top_anchor);
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
	return free_edit_buf(get_epc_buf());
}
int free_edit_buf(be_buf_t *edit_buf)
{
	int ret = 1;

	if (IS_NODE_INT(edit_buf) == 0) {
		return 0;
	}
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_CURSPOS, mk_cur_file_pos_str_static(), 0);
#endif // ENABLE_HISTORY
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
//	editor_panes.bufs[0]->buf_views[0]
//	editor_panes.bufs[0]->buf_views[1] (This pass would be accessed usually)
//	editor_panes.bufs[1]->buf_views[0] (This pass would be accessed usually)
//	editor_panes.bufs[1]->buf_views[1]

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
	// avoid EPCBVX_CL(0) becoming wild-pointer
	line_avoid_wild_ptr(&EPCBVX_CL(0), line);
	// avoid EPCBVX_CL(1) becoming wild-pointer
	line_avoid_wild_ptr(&EPCBVX_CL(1), line);
}
void line_avoid_wild_ptr(be_line_t **line_ptr, be_line_t *line)
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
///		set_epx_buf(pane_idx, EDIT_BUFS_TOP_ANCH);
		set_epx_buf(pane_idx, EDIT_BUFS_TOP_BUF);
	}
	set_editor_cur_pane_idx(0);
}

void set_editor_cur_pane_idx(int pane_idx)
{
	editor_panes.cur_pane_idx = pane_idx;
}
int get_editor_cur_pane_idx(void)
{
	return editor_panes.cur_pane_idx;
}

void set_epc_buf(be_buf_t *buf)	// set edit buffer to current pane
{
	set_epx_buf(-1, buf);
}
// be_buf_t *get_cur_editor_pane_buf(void)
be_buf_t *get_epc_buf(void)
{
	return get_epx_buf(-1);
}
// be_buf_view_t *get_cur_editor_pane_view(void)
be_buf_view_t *get_epc_buf_view(void)
{
	return &(get_epc_buf()->buf_views[get_editor_cur_pane_idx()]);
}

void set_epx_buf(int pane_idx, be_buf_t *buf)
{
	if (pane_idx < 0) {
		pane_idx = editor_panes.cur_pane_idx;
	}
	if (buf) {
		editor_panes.bufs[pane_idx] = buf;
	}
////_D_(dump_editor_panes())

#ifdef ENABLE_SYNTAX
	set_file_type_by_cur_file_path();
#endif // ENABLE_SYNTAX
	update_tab_size();
}
be_buf_t *get_epx_buf(int pane_idx)
{
	if (pane_idx < 0) {
		pane_idx = editor_panes.cur_pane_idx;
	}
	return editor_panes.bufs[pane_idx];
}

#ifdef ENABLE_DEBUG
void dump_editor_panes(void)
{
	flf_d_printf("editor_panes.cur_pane_idx: %d {{{{{\n", editor_panes.cur_pane_idx);
	flf_d_printf("pane_idx-0 ---------------------------------------------\n");
	dump_editor_pane_x(0);
	flf_d_printf("pane_idx-1 ---------------------------------------------\n");
	dump_editor_pane_x(1);
	flf_d_printf("}}}}}\n");
}
void dump_editor_pane_x(int pane_idx)
{
	flf_d_printf("get_epc_buf(): %p, &(get_epc_buf()->buf_views[pane_idx]): %p\n",
	 get_epc_buf(), &(get_epc_buf()->buf_views[pane_idx]));
	dump_buf_view_x(editor_panes.bufs[pane_idx], pane_idx);
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
	buf_dump_state(buf);
	line_dump_byte_idx(BUFVX_CL(buf, pane_idx), BUFVX_CLBI(buf, pane_idx));
	flf_d_printf(
	 "BUFVX_CURSOR_Y(buf, pane_idx): %d, BUFVX_CURSOR_X_TO_KEEP(buf, pane_idx): %d,"
	 " BUFVX_MIN_TEXT_X_TO_KEEP(buf, pane_idx): %d\n",
	 BUFVX_CURSOR_Y(buf, pane_idx), BUFVX_CURSOR_X_TO_KEEP(buf, pane_idx),
	 BUFVX_MIN_TEXT_X_TO_KEEP(buf, pane_idx));
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------

be_buf_t *get_edit_buf_by_file_path(const char *abs_path)
{
	return get_buf_from_bufs_by_file_path(EDIT_BUFS_TOP_BUF, abs_path);
}
be_buf_t *get_edit_buf_by_file_name(const char *file_name)
{
	return get_buf_from_bufs_by_file_name(EDIT_BUFS_TOP_BUF, file_name);
}
int get_edit_buf_idx_from_buf(be_buf_t *edit_buf)
{
	return get_buf_idx_in_bufs(EDIT_BUFS_TOP_BUF, edit_buf);
}

//-----------------------------------------------------------------------------
// create new be_buf_t
void create_edit_buf(const char *full_path)
{
	be_buf_t *buf;

	buf = buf_create_node(full_path);
	buf_insert_before(EDIT_BUFS_BOT_ANCH, buf);
	set_epc_buf(buf);
	if (IS_NODE_INT(editor_panes.bufs[0]) == 0) {
		// make view-0 buffer valid
		editor_panes.bufs[0] = buf;
	}
	if (IS_NODE_INT(editor_panes.bufs[1]) == 0) {
		// make view-1 buffer valid
		editor_panes.bufs[1] = buf;
	}
/////_D_(buf_dump_state(buf))
}

//-----------------------------------------------------------------------------
// Append a new line to the bottom of the current buffer
be_line_t *append_string_to_cur_edit_buf(const char *string)
{
	EPCBVX_CL(0) = EPCBVX_CL(1) = line_insert_with_string(CUR_EDIT_BUF_BOT_ANCH, INSERT_BEFORE,
	 string);
	EPCBVX_CLBI(0) = EPCBVX_CLBI(1) = 0;
	return EPCBVC_CL;
}

// Append a new magic-line to the bottom of the current buffer
void append_magic_line(void)
{
	int lines = buf_count_lines(get_epc_buf());
	if ((lines == 0)
	 || (lines && line_data_len(CUR_EDIT_BUF_BOT_LINE))) {
		append_string_to_cur_edit_buf("");
	}
}

int count_edit_bufs(void)
{
	return buf_count_bufs(&edit_buffers);
}

int is_epc_buf_valid(void)
{
	return IS_NODE_INT(get_epc_buf());
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
	while (IS_NODE_INT(TOP_BUF_OF_CUT_BUFS)) {
		pop_n_free_from_cut_buf();
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
int pop_n_free_from_cut_buf(void)
{
	if (IS_NODE_BOT_ANCH(TOP_BUF_OF_CUT_BUFS))
		return 0;
	buf_unlink_free(TOP_BUF_OF_CUT_BUFS);
	return 1;
}
be_line_t *append_string_to_cur_cut_buf(const char *string)
{
	return line_insert_with_string(CUR_CUT_BUF_BOT_ANCH, INSERT_BEFORE, string);
}
int count_cut_bufs(void)
{
	return buf_count_bufs(&cut_buffers);
}
int count_cur_cut_buf_lines(void)
{
	return buf_count_lines(TOP_BUF_OF_CUT_BUFS);
}

//-----------------------------------------------------------------------------

void renumber_cur_buf_from_top(void)
{
	buf_renumber_from_line(get_epc_buf(), CUR_EDIT_BUF_TOP_LINE);
}

be_line_t *get_line_ptr_from_cur_buf_line_num(int line_num)
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
			editor_disp_title_bar();
		}
	}
	return modified;
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
	return buf_eol_str(get_epc_buf());
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
	return buf_encode_str(get_epc_buf());
}
#endif // USE_NKF

//-----------------------------------------------------------------------------

// If "modified" is not set, set it and update titlebar.
void set_cur_buf_modified(void)
{
	if (CUR_EBUF_STATE(buf_MODIFIED) == 0) {
		SET_CUR_EBUF_STATE(buf_MODIFIED, 1);
		editor_disp_title_bar();
	}
}

//-----------------------------------------------------------------------------

int doe_tog_view_mode(void)
{
	tog_view_mode();
	SHOW_MODE("View mode", get_str_view_mode());
	return 0;
}
int doe_tog_line_wrap_mode(void)
{
	tog_line_wrap_mode();
	SHOW_MODE("Line-wrap mode", get_str_line_wrap_mode());

	EPCBVC_MIN_TEXT_X_TO_KEEP = 0;
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_tog_tab_size(void)
{
	tog_tab_size();
	SHOW_MODE(_("Tab size"), get_str_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_inc_tab_size(void)
{
	inc_tab_size();
	SHOW_MODE(_("Tab size"), get_str_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int doe_set_nix_file(void)
{
	set_nix_file();
	SHOW_MODE("File format", get_str_eol());
	return 0;
}
int doe_set_mac_file(void)
{
	set_mac_file();
	SHOW_MODE("File format", get_str_eol());
	return 0;
}
int doe_set_dos_file(void)
{
	set_dos_file();
	SHOW_MODE("File format", get_str_eol());
	return 0;
}
//-----------------------------------------------------------------------------
#ifdef USE_NKF
int doe_set_encode_ascii(void)
{
	set_encode_ascii();
	SHOW_MODE("ASCII format", get_str_encode_ascii());
	return 0;
}
int doe_set_encode_utf8(void)
{
	set_encode_utf8();
	SHOW_MODE("UTF-8 format", get_str_encode_utf8());
	return 0;
}
int doe_set_encode_eucjp(void)
{
	set_encode_eucjp();
	SHOW_MODE("EUC-JP format", get_str_encode_eucjp());
	return 0;
}
int doe_set_encode_sjis(void)
{
	set_encode_sjis();
	SHOW_MODE("SJIS format", get_str_encode_sjis());
	return 0;
}
int doe_set_encode_jis(void)
{
	set_encode_jis();
	SHOW_MODE("JIS format", get_str_encode_jis());
	return 0;
}
int doe_set_encode_binary(void)
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
	buf_dump_lines(get_epc_buf(), 3);
}
void dump_edit_bufs(void)
{
///	buf_dump_bufs(EDIT_BUFS_TOP_ANCH);
	buf_dump_bufs(EDIT_BUFS_TOP_BUF);
}
void dump_edit_bufs_lines(void)
{
///	buf_dump_bufs_lines(EDIT_BUFS_TOP_ANCH, "edit-bufs");
	buf_dump_bufs_lines(EDIT_BUFS_TOP_BUF, "edit-bufs");
}
void dump_cut_bufs(void)
{
///	buf_dump_bufs(CUT_BUFS_TOP_ANCH);
	buf_dump_bufs(CUT_BUFS_TOP_BUF);
}
void dump_cut_bufs_lines(void)
{
///	buf_dump_bufs_lines(CUT_BUFS_TOP_ANCH, "cut-bufs");
	buf_dump_bufs_lines(CUT_BUFS_TOP_BUF, "cut-bufs");
}

// dump current buffer
void dump_cur_edit_buf(void)
{
flf_d_printf("<<<<<<<<<<<<<<<<<<<\n");
flf_d_printf("CUR_EDIT_BUF_TOP_LINE:%08lx\n", CUR_EDIT_BUF_TOP_LINE);
	if (CUR_EDIT_BUF_TOP_LINE) {
		flf_d_printf("CUR_EDIT_BUF_TOP_LINE->data:%08lx\n", CUR_EDIT_BUF_TOP_LINE->data);
	}
flf_d_printf("CUR_EDIT_BUF_BOT_LINE:%08lx\n", CUR_EDIT_BUF_BOT_LINE);
	if (CUR_EDIT_BUF_BOT_LINE) {
		flf_d_printf("CUR_EDIT_BUF_BOT_LINE->data:%08lx\n", CUR_EDIT_BUF_BOT_LINE->data);
	}
flf_d_printf("cur_line:%08lx\n", EPCBVC_CL);
	if (EPCBVC_CL) {
		flf_d_printf("cur_line->data:%08lx\n", EPCBVC_CL->data);
	}
	line_dump_lines(CUR_EDIT_BUF_TOP_ANCH, INT_MAX, EPCBVC_CL);
flf_d_printf(">>>>>>>>>>>>>>>>>>>\n");
}
#endif // ENABLE_DEBUG

// End of buffers.c
