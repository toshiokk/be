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

// collection of edit buffers--------------------------------------------------
be_bufs_t edit_buffers;
editor_views_t editor_views;
editor_views_t *cur_editor_views;
be_buf_t *c_e_b = NULL;			// pointer to the current edit buffer
be_buf_view_t *c_e_b_v = NULL;	// pointer to the current edit buffer view

// collection of Cut-buffers---------------------------------------------------
be_bufs_t cut_buffers;

#ifdef ENABLE_UNDO
// undo buffers ---------------------------------------------------------------
be_bufs_t undo_buffers;
// redo buffers ---------------------------------------------------------------
be_bufs_t redo_buffers;
#endif // ENABLE_UNDO

// history buffers ------------------------------------------------------------
be_bufs_t history_buffers;

//=============================================================================

// Initialize global variables.
void init_buffers(void)
{
	init_edit_bufs();
_FLF_
	init_cut_bufs();
#ifdef ENABLE_UNDO
	init_undo_bufs();
#endif // ENABLE_UNDO
}

void free_all_buffers(void)
{
#ifdef ENABLE_UNDO
_FLF_
	free_all_undo_bufs();
#endif // ENABLE_UNDO
_FLF_
	free_all_cut_bufs();
_FLF_
	free_all_edit_bufs();
}

// Edit-buffer manipulation routines

void init_edit_bufs(void)
{
	buffer_init(EDIT_BUFS_TOP_ANCH, "#Edit-buffer top_anchor");
	buffer_init(EDIT_BUFS_BOT_ANCH, "#Edit-buffer bot_anchor");
	buffer_link(EDIT_BUFS_TOP_ANCH, EDIT_BUFS_BOT_ANCH);
	init_editor_views(&editor_views);
}
// Free all memory associated with all edit buffers
void free_all_edit_bufs(void)
{
	while (IS_NODE_BOT_ANCH(EDIT_BUFS_TOP_BUF) == 0) {
		free_edit_buf(EDIT_BUFS_TOP_BUF);
	}
	set_c_e_b(EDIT_BUFS_TOP_BUF);
}
// free current be_buf_t
// the next or previous buffer will be set to current
int free_cur_edit_buf(void)
{
	return free_edit_buf(get_c_e_b());
}
int free_edit_buf(be_buf_t *edit_buf)
{
	int ret = 1;

#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_CURSPOS, mk_cur_file_pos_str_static());
#endif // ENABLE_HISTORY
	if (edit_buf == get_c_e_b()) {
		// select other buffer
		ret = switch_c_e_b_to_another_buf();
	}
#ifdef ENABLE_UNDO
	delete_undo_redo_buf(edit_buf);
#endif // ENABLE_UNDO
	buffer_unlink_free(edit_buf);
	return ret;		// 0: no buffer remains
}
//-----------------------------------------------------------------------------
void buffer_avoid_wild_ptr_cur(be_buf_t *buf)
{
	// avoid cur_editor_views->bufs[0] becoming wild-pointer 
	buffer_avoid_wild_ptr(buf, &(cur_editor_views->bufs[0]));
	// avoid cur_editor_views->bufs[1] becoming wild-pointer 
	buffer_avoid_wild_ptr(buf, &(cur_editor_views->bufs[1]));
}
void buffer_avoid_wild_ptr(be_buf_t *buf, be_buf_t **buf_ptr)
{
	if (*buf_ptr == buf) {
		if (IS_NODE_BOT_ANCH((*buf_ptr)->next) == 0) {
			*buf_ptr = (*buf_ptr)->next;
		} else {
			*buf_ptr = (*buf_ptr)->prev;
		}
	}
}
//-----------------------------------------------------------------------------
void line_avoid_wild_ptr_cur(be_line_t *line)
{
	// avoid CEBV0_CL becoming wild-pointer
	line_avoid_wild_ptr(line, &CEBV0_CL);
	// avoid CEBV1_CL becoming wild-pointer
	line_avoid_wild_ptr(line, &CEBV1_CL);
}
void line_avoid_wild_ptr(be_line_t *line, be_line_t **line_ptr)
{
	if (*line_ptr == line) {
		if (IS_NODE_TOP(*line_ptr) == 0) {
			*line_ptr = (*line_ptr)->prev;
		} else {
			*line_ptr = (*line_ptr)->next;
		}
	}
}
//-----------------------------------------------------------------------------
// Editor view manegement

void init_editor_views(editor_views_t *editor_views)
{
	int view_idx;

	cur_editor_views = editor_views;
	for (view_idx = 0; view_idx < BUF_VIEWS; view_idx++) {
		set_cur_editor_view_buf(view_idx, EDIT_BUFS_TOP_ANCH);
	}
	set_cur_editor_view_idx(0);
}

void set_cur_editor_view_idx(int view_idx)
{
	set_cur_editor_view_buf(view_idx, NULL);
}
int get_cur_editor_view_idx(void)
{
	return cur_editor_views->view_idx;
}

void set_cur_edit_buf(be_buf_t *buf)
{
	set_cur_editor_view_buf(-1, buf);
}
void set_cur_editor_view_buf(int view_idx, be_buf_t *buf)
{
///flf_d_printf("view_idx: %d ------------\n", view_idx);
	if (view_idx >= 0) {
		cur_editor_views->view_idx = view_idx;
	}
	if (buf) {
		cur_editor_views->bufs[cur_editor_views->view_idx] = buf;
	}
	set_c_e_b(cur_editor_views->bufs[cur_editor_views->view_idx]);
	set_c_e_b_v(&(get_c_e_b()->views[cur_editor_views->view_idx]));
///_D_(dump_cur_editor_views());

#ifdef ENABLE_SYNTAX
	set_file_type_by_cur_file_path();
#endif // ENABLE_SYNTAX
	update_tab_size();
}

#ifdef ENABLE_DEBUG
void dump_cur_editor_views(void)
{
	flf_d_printf("cur_editor_views->view_idx: %d\n", cur_editor_views->view_idx);

	buffer_dump_state(cur_editor_views->bufs[0]);
	buffer_dump_state(cur_editor_views->bufs[1]);

	flf_d_printf("get_c_e_b(): %p, &(get_c_e_b()->views[0]): %p\n", get_c_e_b(), &(get_c_e_b()->views[0]));
	line_dump_byte_idx(CEBV0_CL, CEBV0_CLBI);
	flf_d_printf("CEBV0_CURSOR_Y: %d, CEBV0_CURSOR_X_TO_KEEP: %d\n",
	 CEBV0_CURSOR_Y, CEBV0_CURSOR_X_TO_KEEP);

	flf_d_printf("get_c_e_b(): %p, &(get_c_e_b()->views[1]): %p\n", get_c_e_b(), &(get_c_e_b()->views[1]));
	line_dump_byte_idx(CEBV1_CL, CEBV1_CLBI);
	flf_d_printf("CEBV1_CURSOR_Y: %d, CEBV1_CURSOR_X_TO_KEEP: %d\n",
	 CEBV1_CURSOR_Y, CEBV1_CURSOR_X_TO_KEEP);

	flf_d_printf("get_c_e_b(): %p, get_c_e_b_v(): %p\n", get_c_e_b(), get_c_e_b_v());
	line_dump_byte_idx(CEBV_CL, CEBV_CLBI);
	flf_d_printf("CEBV_CURSOR_Y: %d, CEBV_CURSOR_X_TO_KEEP: %d\n",
	 CEBV_CURSOR_Y, CEBV_CURSOR_X_TO_KEEP);
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------

be_buf_t *get_edit_buf_from_abs_path(const char *abs_path)
{
	be_buf_t *buf;

	for (buf = EDIT_BUFS_TOP_BUF; IS_NODE_BOT_ANCH(buf) == 0; buf = buf->next) {
		if (strcmp(buf->abs_path, abs_path) == 0) {
			return buf;
		}
	}
	return NULL;		// not found
}
int get_edit_buf_idx_from_buf(be_buf_t *edit_buf)
{
	int buf_idx;
	be_buf_t *buf;

	for (buf_idx = 0, buf = EDIT_BUFS_TOP_BUF;
	 IS_NODE_BOT_ANCH(buf) == 0;
	 buf_idx++, buf = buf->next) {
		if (buf == edit_buf)
			return buf_idx;
	}
	return -1;	// not found
}

//-----------------------------------------------------------------------------
// create new be_buf_t
void create_edit_buf(const char *full_path)
{
	be_buf_t *buf;

	buf = buffer_create(full_path);
	buffer_insert_before(EDIT_BUFS_BOT_ANCH, buf);
///_FLF_
	set_cur_edit_buf(buf);
	if (IS_NODE_VALID(cur_editor_views->bufs[0]) == 0) {
		// make view-0 buffer valid
		cur_editor_views->bufs[0] = buf;
	}
	if (IS_NODE_VALID(cur_editor_views->bufs[1]) == 0) {
		// make view-1 buffer valid
		cur_editor_views->bufs[1] = buf;
	}
}

//-----------------------------------------------------------------------------

// Append a new line to the bottom of the current buffer
be_line_t *append_string_to_cur_edit_buf(const char *string)
{
	CEBV0_CL = CEBV1_CL = line_insert_with_string(CUR_EDIT_BUF_BOT_ANCH, INSERT_BEFORE, string);
	CEBV0_CLBI = CEBV1_CLBI = 0;
	return CEBV_CL;
}

// Append a new magic-line to the bottom of the current buffer
void append_magic_line(void)
{
	if (buffer_count_lines(get_c_e_b()) == 0 || line_data_len(CUR_EDIT_BUF_BOT_LINE)) {
		append_string_to_cur_edit_buf("");
	}
}

int count_edit_bufs(void)
{
	return buffer_count_bufs(EDIT_BUFS_TOP_BUF);
}

int is_c_e_b_valid(void)
{
	return IS_NODE_VALID(get_c_e_b());
}

// Cut-buffers manipulation routines -----------------------------------------

void init_cut_bufs(void)
{
	buffer_init(CUT_BUFS_TOP_ANCH, "#Cut-buffer top_anchor");
	buffer_init(CUT_BUFS_BOT_ANCH, "#Cut-buffer bot_anchor");
	buffer_link(CUT_BUFS_TOP_ANCH, CUT_BUFS_BOT_ANCH);
}
void free_all_cut_bufs(void)
{
	while(IS_NODE_BOT_ANCH(CUR_CUT_BUF) == 0) {
		pop_cut_buf();
	}
}
be_buf_t *push_cut_buf(void)
{
	be_buf_t *cut_buf;
	char buf_name[MAX_PATH_LEN+1];

	snprintf(buf_name, MAX_PATH_LEN, "#cut-buffer-%02d", count_cut_bufs());
	cut_buf = buffer_create(buf_name);
	buffer_insert_after(CUT_BUFS_TOP_ANCH, cut_buf);
	// copy cut-mode to cut-buffer
	SET_CUR_CBUF_STATE(buf_CUT_MODE, CUR_EBUF_STATE(buf_CUT_MODE));
	return cut_buf;
}
int pop_cut_buf(void)
{
	if (IS_NODE_BOT_ANCH(CUR_CUT_BUF))
		return 0;
	buffer_unlink_free(CUR_CUT_BUF);
	return 1;
}
be_line_t *append_string_to_cur_cut_buf(const char *string)
{
	return line_insert_with_string(CUR_CUT_BUF_BOT_ANCH, INSERT_BEFORE, string);
}
int count_cut_bufs(void)
{
	return buffer_count_bufs(CUR_CUT_BUF);
}
int count_cur_cut_buf_lines(void)
{
	return buffer_count_lines(CUR_CUT_BUF);
}

//-----------------------------------------------------------------------------

void renumber_cur_buf_from_top(void)
{
	buffer_renumber_from_line(get_c_e_b(), CUR_EDIT_BUF_TOP_LINE);
}

be_line_t *get_line_ptr_from_cur_buf_line_num(int line_num)
{
	return buffer_get_line_ptr_from_line_num(get_c_e_b(), line_num);
}

//-----------------------------------------------------------------------------

void update_cur_buf_crc(void)
{
	disp_status_bar_ing(_("Calculating CRC..."));
	buffer_update_crc(get_c_e_b());
}
int check_cur_buf_modified(void)
{
	int modified = 0;

	if (CUR_EBUF_STATE(buf_MODIFIED)) {
		disp_status_bar_ing(_("Calculating CRC..."));
		modified = buffer_check_crc(get_c_e_b());
	}
flf_d_printf("modified: %d\n", modified);
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
	// to avoid divided-by-0, check 0 and replace non-zero-value
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
	return buffer_eol_str(get_c_e_b());
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
	return buffer_encode_str(get_c_e_b());
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
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_tog_tab_size(void)
{
	tog_tab_size();
	SHOW_MODE(_("Tab size"), get_str_tab_size());
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
int do_inc_tab_size(void)
{
	inc_tab_size();
	SHOW_MODE(_("Tab size"), get_str_tab_size());
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
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
	buffer_dump_lines(get_c_e_b(), 3);
}
void dump_edit_bufs(void)
{
	buffer_dump_bufs(EDIT_BUFS_TOP_ANCH);
}
void dump_edit_bufs_lines(void)
{
	buffer_dump_bufs_lines(EDIT_BUFS_TOP_ANCH, "edit-bufs");
}
void dump_cut_bufs(void)
{
	buffer_dump_bufs(CUT_BUFS_TOP_ANCH);
}
void dump_cut_bufs_lines(void)
{
	buffer_dump_bufs_lines(CUT_BUFS_TOP_ANCH, "cut-bufs");
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
flf_d_printf("cur_line:%08lx\n", CEBV_CL);
	if (CEBV_CL) {
		flf_d_printf("cur_line->data:%08lx\n", CEBV_CL->data);
	}
	line_dump_lines(CUR_EDIT_BUF_TOP_ANCH, INT_MAX, CEBV_CL);
flf_d_printf(">>>>>>>>>>>>>>>>>>>\n");
}
#endif // ENABLE_DEBUG

// End of buffers.c
