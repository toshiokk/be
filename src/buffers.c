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
// Structure of collections of buffers:
//   be_bufs_t all_bufferss.top_anchor
//   be_bufs_t edit_bufs
//     be_buf_t main.c
//          :
//     be_buf_t headers.h
//   be_bufs_t hist_bufs
//     be_buf_t exec_hist
//     be_buf_t dir_hist
//     be_buf_t key_macro_hist
//     be_buf_t shell_hist
//     be_buf_t file_pos_hist
//   be_bufs_t help_bufs
//     be_buf_t file_list
//     be_buf_t key_binding_list
//     be_buf_t func_list
//   be_bufs_t cut_bufs
//     be_buf_t cut-1
//          :
//     be_buf_t cut-n
//   be_bufs_t undo_bufs
//     be_buf_t undo-1
//          :
//     be_buf_t undo-n
//   be_bufs_t redo_bufs
//     be_buf_t redo-1
//          :
//     be_buf_t redo-n
//   be_bufs_t all_bufferss.bot_anchor

// Initialize collection of buffers.
void init_bufferss()
{
	bufss_init(&all_bufferss, "##BUFSS", "##bufss_top_anch", "##bufss_bot_anch");

	bufss_insert_bufs_to_bottom(&all_bufferss, bufs_init(&edit_buffers,
	 "#EDIT", "#edit_bufs_top_anch", "#edit_bufs_bot_anch"));
#ifdef ENABLE_HISTORY
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&history_buffers,
	 "#HIST", "#hist_bufs_top_anch", "#hist_bufs_bot_anch"));
#endif // ENABLE_HISTORY
#ifdef ENABLE_HELP
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&help_buffers,
	 "#HELP", "#help_bufs_top_anch", "#help_bufs_bot_anch"));
#endif // ENABLE_HELP
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&cut_buffers,
	 "#CUT", "#cut_bufs_top_anch", "#cut_bufs_bot_anch"));
#ifdef ENABLE_UNDO
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&undo_buffers,
	 "#UNDO", "#undo_bufs_top_anch", "#undo_bufs_bot_anch"));
	bufs_insert_before(NODES_BOT_ANCH(&all_bufferss), bufs_init(&redo_buffers,
	 "#REDO", "#redo_bufs_top_anch", "#redo_bufs_bot_anch"));
#endif // ENABLE_UNDO

#ifdef ENABLE_HELP
	init_help_bufs();
#endif // ENABLE_HELP
}

void free_all_buffers()
{
	bufs_free_all_bufs(NODES_TOP_NODE(&all_bufferss));
}

// free current edit buffer
// the next or previous buffer will be selected as a current
int free_cur_edit_buf()
{
	unlock_epc_buf_if_file_had_locked_by_myself();
	return free_edit_buf(get_epc_buf());
}
int free_edit_buf(be_buf_t *buf)
{
	if (IS_NODE_INT(buf) == 0) {
		return 0;
	}
	disp_status_bar_ing(_("Freeing edit buffer %s ..."), buf_get_file_path(buf, NULL));
#ifdef ENABLE_HISTORY
	modify_history_w_reloading(HISTORY_TYPE_IDX_FILE, mk_cur_file_pos_str_static());
#endif // ENABLE_HISTORY
	int ret = 1;
	if (buf == get_epc_buf()) {
		// select other buffer
		ret = switch_epc_buf_to_another_buf();
	}
#ifdef ENABLE_UNDO
	// free undo/redo buffers related to this edit_buf
	delete_undo_redo_buf(buf);
#endif // ENABLE_UNDO
	buf_unlink_free(buf);
	return ret;		// 0: no buffer remains
}
//------------------------------------------------------------------------------
// cur_editor_panes->bufs[0]->buf_views[0]
// cur_editor_panes->bufs[1]->buf_views[1]

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
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
// Editor view management

void set_cur_editor_panes(editor_panes_t *editor_panes)
{
	cur_editor_panes = editor_panes;
}
editor_panes_t* get_cur_editor_panes()
{
	return cur_editor_panes;
}
void init_editor_panes(editor_panes_t *eps)
{
	set_cur_editor_panes(eps);
	set_editor_cur_pane_idx(0);
	for (int pane_idx = 0; pane_idx < EDITOR_PANES; pane_idx++) {
		set_epx_buf(pane_idx, EDIT_BUFS_TOP_BUF);
	}
}
void inherit_editor_panes(editor_panes_t *eps, be_buf_t *buf)
{
	editor_panes_t *prev_eps = cur_editor_panes;
	set_cur_editor_panes(eps);
	if (prev_eps == NULL) {
		// No prev_eps, this means 'eps' is the root view. set default initial values.
		for (int pane_idx = 0; pane_idx < EDITOR_PANES; pane_idx++) {
			set_epx_buf(pane_idx, EDIT_BUFS_TOP_BUF);
		}
	} else {
		// There is prev_eps, copy it.
		copy_editor_panes(cur_editor_panes, prev_eps);
	}
	if (buf) {
		set_epx_buf(-1, buf);	// set a buffer only to the current pane
	}
}
void free_editor_panes()
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
be_buf_t *get_epc_buf()
{
	return get_epx_buf(-1);
}
be_buf_view_t *get_epc_buf_view()
{
	return &(get_epc_buf()->buf_views[get_editor_cur_pane_idx()]);
}

void set_epx_buf(int pane_idx, be_buf_t *buf)
{
	if (pane_idx < 0) {
		pane_idx = get_editor_cur_pane_idx();
	}
	if (buf) {
		cur_editor_panes->bufs[pane_idx] = buf;
	}
	buf_fix_cur_line(buf);
}

be_buf_t *get_epx_buf(int pane_idx)
{
	if (pane_idx < 0) {
		pane_idx = get_editor_cur_pane_idx();
	}
	return cur_editor_panes->bufs[pane_idx];
}

//--------------------------------------
void clear_cur_ebuf_modified()
{
	SET_CUR_EBUF_STATE(buf_MODIFIED, 0);
}
void set_cur_ebuf_modified()
{
	SET_CUR_EBUF_STATE(buf_MODIFIED, 1);
}
int is_epc_buf_modified()
{
	return GET_CUR_EBUF_STATE(buf_MODIFIED);
}

int is_epc_buf_edit_buf()
{
	return get_bufs_contains_buf(get_epc_buf()) == &edit_buffers;
}

be_bufs_t* get_bufs_contains_buf(be_buf_t* buf)
{
	return bufs_get_bufs_contains_buf(NODES_TOP_ANCH(&all_bufferss), buf);
}
const char *get_bufs_name_contains_buf(be_buf_t* buf)
{
	return bufs_get_bufs_contains_buf(NODES_TOP_ANCH(&all_bufferss), buf)->name;
}

//--------------------------------------
int is_epc_buf_modifiable()
{
	return 1
	 && is_epc_buf_valid()

	 && is_epc_buf_mode_edit()

	 && (is_epc_buf_file_wp() == 0)
	 && (is_epc_buf_file_locked() == 0)
	;
}
int is_epc_buf_saveable()
{
	return is_epc_buf_modifiable();
}
int is_epc_buf_closeable()	// closeable by user's intention
{
	return 1
	 && is_epc_buf_valid()
	 && is_epc_buf_mode_edit()
	;
}
//--------------------------------------
int is_epc_buf_mode_edit()
{
	return GET_CUR_EBUF_STATE(buf_MODE) == BUF_MODE_EDIT;
}
int is_epc_buf_mode_list()
{
	return GET_CUR_EBUF_STATE(buf_MODE) == BUF_MODE_LIST;
}
int is_epc_buf_mode_ro()
{
	return GET_CUR_EBUF_STATE(buf_MODE) == BUF_MODE_RO;
}
//--------------------------------------
int is_epc_buf_valid()
{
	// avoid crash on the modification
	return IS_NODE_INT(get_epc_buf());		// not NULL, not anchor but intermediate
}
int is_epc_buf_empty()
{
	// avoid crash on the modification
	return IS_NODES_EMPTY(get_epc_buf());	// has No line
}
//--------------------------------------
int is_epc_buf_file_wp()	// write protected in the file system
{
	return is_st_writable(&get_epc_buf()->orig_file_stat) == 0;
}
int is_epc_buf_file_locked()
{
	return GET_CUR_EBUF_STATE(buf_LOCKED);
}
//--------------------------------------
const char *get_all_buf_state_str()
{
	static char all_buf_state_str[MAX_PATH_LEN+1] = "";
	all_buf_state_str[0] = '\0';
	if (is_epc_buf_modified()) {
		strlcat__(all_buf_state_str, MAX_SCRN_LINE_BUF_LEN, _("[MOD]"));
	}
	strlcat__(all_buf_state_str, MAX_PATH_LEN, get_all_buf_unmodifiable_str());
	if (is_epc_buf_empty()) {
		strlcat__(all_buf_state_str, MAX_PATH_LEN, _("[EMPTY]"));
	}
	return all_buf_state_str;
}
const char *get_all_buf_unmodifiable_str()
{
	static char all_buf_state_str[MAX_PATH_LEN+1] = "";
	all_buf_state_str[0] = '\0';
	if (is_epc_buf_valid() == 0) {
		strlcat__(all_buf_state_str, MAX_PATH_LEN, _("[ANCH]"));
	}
	if (is_epc_buf_file_wp()) {
		strlcat__(all_buf_state_str, MAX_PATH_LEN, _("[WP]"));
	}
	if (is_epc_buf_file_locked()) {
		strlcat__(all_buf_state_str, MAX_PATH_LEN, _("[LOCKED]"));
	}
	strlcat__(all_buf_state_str, MAX_PATH_LEN, get_epc_buf_mode_str());
	return all_buf_state_str;
}
const char *get_epc_buf_mode_str()
{
	if (is_epc_buf_mode_edit() == 0) {
		return get_str_buf_mode();
	}
	return "";
}
//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_cur_editor_panes()
{
	dump_editor_panes(get_cur_editor_panes());
}
void dump_editor_panes(editor_panes_t *eps)
{
	flf_dprintf("{{ %p\n", eps);
	if (eps == NULL) {
		eps = cur_editor_panes;
	}
	flf_dprintf("cur_pane_idx: %d\n", get_editor_cur_pane_idx());
	dump_buf_views(eps->bufs[get_editor_cur_pane_idx()]);
	////flf_dprintf("pane_idx:0 ---------------------------------------------\n");
	////dump_editor_pane_x(eps, 0);
	////flf_dprintf("pane_idx:1 ---------------------------------------------\n");
	////dump_editor_pane_x(eps, 1);
	flf_dprintf("}}\n");
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
	flf_dprintf("pane_idx: %d ------------------\n", pane_idx);
	if ((buf_check_line_is_in_buf(buf, BVX_CL(buf, pane_idx)) == NULL)
	 && (buf_check_line_is_buf_anchs(buf, BVX_CL(buf, pane_idx)) == NULL)) {
		warning_printf("pane[%d].cur_line is not in cur_buf!!!!\n", pane_idx);
	}
	buf_dump_name(buf);
	////line_dump_byte_idx(BVX_CL(buf, pane_idx), BVX_CLBI(buf, pane_idx));
	////flf_dprintf(
	//// "BVX_CURS_Y(buf, pane_idx): %d, BVX_CURS_X_TO_KEEP(buf, pane_idx): %d,"
	//// " BVX_MIN_TEXT_X_TO_KEEP(buf, pane_idx): %d\n",
	//// BVX_CURS_Y(buf, pane_idx), BVX_CURS_X_TO_KEEP(buf, pane_idx),
	//// BVX_MIN_TEXT_X_TO_KEEP(buf, pane_idx));
}
#endif // ENABLE_DEBUG
//------------------------------------------------------------------------------
be_buf_t *get_any_buf_by_full_path(const char *file_path)
{
	if (is_internal_buf_file_path(file_path) == 0) {
		// from edit buffers
		return buf_get_buf_by_full_path(EDIT_BUFS_TOP_BUF, file_path);
	} else {
		// from all buffers
		for (be_bufs_t *bufs = NODES_TOP_NODE(&all_bufferss); IS_NODE_INT(bufs);
		 bufs = NODE_NEXT(bufs)) {
			be_buf_t *buf = buf_get_buf_by_full_path(NODES_TOP_NODE(bufs), file_path);
			if (buf) {
				return buf;
			}
		}
		return NULL;		// not found
	}
}
be_buf_t *get_edit_buf_by_file_path(const char *file_path)
{
	return buf_get_buf_by_file_path(EDIT_BUFS_TOP_BUF, file_path);
}

//------------------------------------------------------------------------------
// create new be_buf_t
void create_edit_buf(const char *full_path)
{
	be_buf_t *buf = buf_create(full_path, BUF_MODE_EDIT);
	bufs_insert_buf_to_bottom(&edit_buffers, buf);
	set_epc_buf(buf);
	// copy default encoding from EDIT_BUFS_BOT_ANCH
#ifdef USE_NKF
	SET_CUR_EBUF_STATE(buf_ENCODE, GET_BUF_STATE(EDIT_BUFS_BOT_ANCH, buf_ENCODE));
#endif // USE_NKF
	if (IS_NODE_INT(cur_editor_panes->bufs[0]) == 0) {
		// If view-0 does not point a valid buffer, set valid buf to view-0 buffer pointer
		cur_editor_panes->bufs[0] = buf;
	}
	if (IS_NODE_INT(cur_editor_panes->bufs[1]) == 0) {
		// If view-0 does not point a valid buffer, set valid buf to view-0 buffer pointer
		cur_editor_panes->bufs[1] = buf;
	}
}

//------------------------------------------------------------------------------
// Append a new line to the bottom of the current buffer
be_line_t *append_string_to_cur_edit_buf(const char *string)
{
	EPCBVX_CL(0) = EPCBVX_CL(1) = line_insert_with_string(CUR_EDIT_BUF_BOT_ANCH, INSERT_BEFORE,
	 string);
	EPCBVX_CLBI(0) = EPCBVX_CLBI(1) = 0;
	return EPCBVC_CL;
}

// Append a new magic-line to the bottom of the current buffer
int append_magic_line_if_necessary()
{
	if (buf_is_empty(get_epc_buf())
	 || ((buf_is_empty(get_epc_buf()) == 0) && line_strlen(CUR_EDIT_BUF_BOT_LINE))) {
		append_string_to_cur_edit_buf("");
		return 1;
	}
	return 0;
}

int has_bufs_to_edit()
{
	return (count_edit_bufs() > 0) || (epc_buf_count() > 0);
}
int count_edit_bufs()
{
	return bufs_count_bufs(&edit_buffers);
}
int epc_buf_count()
{
	return buf_count(get_epc_buf());
}

// Cut-buffers manipulation routines -----------------------------------------

be_buf_t *add_one_cut_buf()
{
	be_buf_t *buf = buf_create(
	 make_internal_buf_file_path(sprintf_s("cut-buffer-%03d", count_cut_bufs())), BUF_MODE_LIST);
	bufs_insert_buf_to_bottom(&cut_buffers, buf);
	// copy cut-mode to cut-buffer
	SET_CUR_CBUF_STATE(buf_CUT_MODE, GET_CUR_EBUF_STATE(buf_CUT_MODE));
	SET_CUR_CBUF_STATE(buf_MODE, BUF_MODE_LIST);
	set_cut_buffers_modified();
	return buf;
}
int remove__free_one_cut_buf()
{
	if (IS_NODE_INT(CUT_BUFS_NEWEST_BUF) == 0)
		return 0;
	buf_unlink_free(CUT_BUFS_NEWEST_BUF);
	set_cut_buffers_modified();
	return 1;
}
be_line_t *append_string_to_newest_cut_buf(const char *string)
{
	BVX_CL(CUT_BUFS_NEWEST_BUF, 0) = BVX_CL(CUT_BUFS_NEWEST_BUF, 1)
	 = line_insert_with_string(NEWEST_CUT_BUF_BOT_ANCH, INSERT_BEFORE, string);
	BVX_CLBI(CUT_BUFS_NEWEST_BUF, 0) = BVX_CLBI(CUT_BUFS_NEWEST_BUF, 1) = 0;
	return BVX_CL(CUT_BUFS_NEWEST_BUF, 0);
}
int count_cut_bufs()
{
	return bufs_count_bufs(&cut_buffers);
}
int count_cur_cut_buf_lines()
{
	return buf_count_lines(CUT_BUFS_NEWEST_BUF, MAX_LINES_LOADABLE);
}
void clear_cut_bufs()
{
	while (IS_NODE_INT(CUT_BUFS_NEWEST_BUF)) {
		remove__free_one_cut_buf();
	}
}
//------------------------------------------------------------------------------
void renumber_cur_buf_from_top()
{
	buf_renumber_from_top(get_epc_buf());
}

be_line_t *get_line_ptr_in_cur_buf_by_line_num(int line_num)
{
	return buf_get_line_ptr_from_line_num(get_epc_buf(), line_num);
}
//------------------------------------------------------------------------------
void update_cur_ebuf_crc()
{
	disp_status_bar_ing(_("Calculating CRC..."));
	buf_update_crc(get_epc_buf());
}
int check_cur_ebuf_modified()
{
	int modified = 0;
	if (is_epc_buf_modified()) {
		disp_status_bar_ing(_("Checking CRC..."));
		modified = buf_check_crc(get_epc_buf());
		if (modified == 0) {
			// clear "modified" flag if it's actually not modified
			clear_cur_ebuf_modified();
		}
	}
	return modified;
}
int is_any_ebuf_modified()
{
	for (be_buf_t *edit_buf = EDIT_BUFS_TOP_BUF; IS_NODE_INT(edit_buf);
	 edit_buf = NODE_NEXT(edit_buf)) {
		if (GET_BUF_STATE(edit_buf, buf_MODIFIED)) {
			return 1;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------
int inc_buf_mode()
{
	INC_CUR_EBUF_STATE(buf_MODE, BUF_MODE_EDIT, BUF_MODE_LIST);
	return 0;
}
const char *get_str_buf_mode()
{
	return buf_mode_str(get_epc_buf());
}

int tog_buf_line_wrap_mode()
{
	TOGGLE_CUR_EBUF_STATE(buf_LINE_WRAP_MODE);
	return 0;
}
const char *get_str_buf_line_wrap_mode()
{
	return BOOL_TO_ON_OFF(GET_CUR_EBUF_STATE(buf_LINE_WRAP_MODE));
}

int tog_buf_tab_size()
{
	GET_CUR_EBUF_STATE(buf_TAB_SIZE)
	 = (GET_CUR_EBUF_STATE(buf_TAB_SIZE) == 8) ? 4 : 8;
	set_wrap_line_tab_size_from_cur_buf();
	return 0;
}
int inc_buf_tab_size()
{
	INC_CUR_EBUF_STATE(buf_TAB_SIZE, TAB_SIZE_MIN, TAB_SIZE_MAX);
	set_wrap_line_tab_size_from_cur_buf();
	return 0;
}
const char *get_str_buf_tab_size()
{
	static char buf[2+1];
	snprintf(buf, 2+1, "%d", get_cur_buf_tab_size());
	return buf;
}
int get_cur_buf_tab_size()
{
	// to avoid divided-by-0, check 0 and replace with non-zero-value
	return GET_CUR_EBUF_STATE(buf_TAB_SIZE) == 0
	 ? DEFAULT_TAB_SIZE : GET_CUR_EBUF_STATE(buf_TAB_SIZE);
}
// update tab-size in wrap-line from the current buffer
void set_wrap_line_tab_size_from_cur_buf()
{
	set_wrap_line_tab_size(get_cur_buf_tab_size());
}

int set_buf_nix_file()
{
	set_buf_eol(EOL_NIX);
	return 0;
}
const char *get_str_buf_nix_file()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_EOL, EOL_NIX));
}
int set_buf_mac_file()
{
	set_buf_eol(EOL_MAC);
	return 0;
}
const char *get_str_buf_mac_file()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_EOL, EOL_MAC));
}
int set_buf_dos_file()
{
	set_buf_eol(EOL_DOS);
	return 0;
}
const char *get_str_buf_dos_file()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_EOL, EOL_DOS));
}
int set_buf_eol(int eol)
{
	GET_CUR_EBUF_STATE(buf_EOL) = eol;
	return 0;
}
const char *get_str_buf_eol()
{
	return buf_eol_str(get_epc_buf());
}

int set_buf_enc_ascii()
{
	set_buf_encode(ENCODE_ASCII);
	return 0;
}
const char *get_str_buf_enc_ascii()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_ASCII));
}
int set_buf_enc_utf8()
{
	set_buf_encode(ENCODE_UTF8);
	return 0;
}
const char *get_str_buf_enc_utf8()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8));
}
#ifdef USE_NKF
int set_buf_enc_eucjp()
{
	set_buf_encode(ENCODE_EUCJP);
	return 0;
}
const char *get_str_buf_enc_eucjp()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_EUCJP));
}
int set_buf_enc_sjis()
{
	set_buf_encode(ENCODE_SJIS);
	return 0;
}
const char *get_str_buf_enc_sjis()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_SJIS));
}
int set_buf_enc_jis()
{
	set_buf_encode(ENCODE_JIS);
	return 0;
}
const char *get_str_buf_enc_jis()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_JIS));
}
#endif // USE_NKF
int set_buf_enc_binary()
{
	set_buf_encode(ENCODE_BINARY);
	return 0;
}
const char *get_str_buf_enc_binary()
{
	return SELECTED_OR_NOT(CMP_CUR_EBUF_STATE(buf_ENCODE, ENCODE_BINARY));
}

int set_buf_encode(int encode)
{
	GET_CUR_EBUF_STATE(buf_ENCODE) = encode;
	return 0;
}
const char *get_str_buf_encode()
{
	return buf_enc_str(get_epc_buf());
}
//------------------------------------------------------------------------------
void doe_inc_buf_mode()
{
	inc_buf_mode();
	SHOW_MODE("View mode", get_str_buf_mode());
}
void doe_tog_buf_line_wrap_mode()
{
	tog_buf_line_wrap_mode();
	SHOW_MODE("Line-wrap mode", get_str_buf_line_wrap_mode());

	EPCBVC_MIN_TEXT_X_TO_KEEP = 0;
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
void doe_tog_buf_tab_size()
{
	tog_buf_tab_size();
	SHOW_MODE(_("Tab size"), get_str_buf_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
void doe_inc_buf_tab_size()
{
	inc_buf_tab_size();
	SHOW_MODE(_("Tab size"), get_str_buf_tab_size());
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
void doe_set_buf_nix_file()
{
	set_buf_nix_file();
	SHOW_MODE("File format", get_str_buf_eol());
}
void doe_set_buf_mac_file()
{
	set_buf_mac_file();
	SHOW_MODE("File format", get_str_buf_eol());
}
void doe_set_buf_dos_file()
{
	set_buf_dos_file();
	SHOW_MODE("File format", get_str_buf_eol());
}
//------------------------------------------------------------------------------
void doe_set_buf_enc_ascii()
{
	set_buf_enc_ascii();
	SHOW_MODE("ASCII format", get_str_buf_enc_ascii());
}
void doe_set_buf_enc_utf8()
{
	set_buf_enc_utf8();
	SHOW_MODE("UTF-8 format", get_str_buf_enc_utf8());
}
#ifdef USE_NKF
void doe_set_buf_enc_eucjp()
{
	set_buf_enc_eucjp();
	SHOW_MODE("EUC-JP format", get_str_buf_enc_eucjp());
}
void doe_set_buf_enc_sjis()
{
	set_buf_enc_sjis();
	SHOW_MODE("SJIS format", get_str_buf_enc_sjis());
}
void doe_set_buf_enc_jis()
{
	set_buf_enc_jis();
	SHOW_MODE("JIS format", get_str_buf_enc_jis());
}
#endif // USE_NKF
void doe_set_buf_enc_binary()
{
	set_buf_enc_binary();
	SHOW_MODE("BINARY format", get_str_buf_enc_binary());
}
//------------------------------------------------------------------------------
#define INTERNAL_BUFFER_FILE_PATH_PREFIX		"#"
// internal_buffer_file_path: "#..."
const char *make_internal_buf_file_path(const char *file_path)
{
	static char file_path_s[MAX_PATH_LEN+1];
	if (is_internal_buf_file_path(file_path) == 0) {
		strlcpy__(file_path_s, file_path, MAX_PATH_LEN);
		str_prepend(file_path_s, MAX_PATH_LEN, INTERNAL_BUFFER_FILE_PATH_PREFIX);
		file_path = file_path_s;
	}
	return file_path;
}
int is_internal_buf_file_path(const char *file_path)
{
	return strlcmp__(file_path, INTERNAL_BUFFER_FILE_PATH_PREFIX) == 0;
}
//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_cur_edit_buf_lines()
{
	buf_dump_lines(get_epc_buf(), 3);
}
void dump_edit_bufs()
{
	buf_dump_bufs(EDIT_BUFS_TOP_BUF);
}
void dump_edit_bufs_lines()
{
	buf_dump_bufs_lines(EDIT_BUFS_TOP_BUF, "edit-bufs");
}
void dump_cut_bufs()
{
	buf_dump_bufs(CUT_BUFS_TOP_BUF);
}
void dump_cut_bufs_lines()
{
	buf_dump_bufs_lines(CUT_BUFS_TOP_BUF, "cut-bufs");
}

// dump current buffer
void dump_cur_edit_buf()
{
	flf_dprintf("<<<\n");
	flf_dprintf("CUR_EDIT_BUF_TOP_LINE:%08lx\n", CUR_EDIT_BUF_TOP_LINE);
	if (CUR_EDIT_BUF_TOP_LINE) {
		flf_dprintf("CUR_EDIT_BUF_TOP_LINE->data:%08lx\n", CUR_EDIT_BUF_TOP_LINE->data);
	}
	flf_dprintf("CUR_EDIT_BUF_BOT_LINE:%08lx\n", CUR_EDIT_BUF_BOT_LINE);
	if (CUR_EDIT_BUF_BOT_LINE) {
		flf_dprintf("CUR_EDIT_BUF_BOT_LINE->data:%08lx\n", CUR_EDIT_BUF_BOT_LINE->data);
	}
	flf_dprintf("cur_line:%08lx\n", EPCBVC_CL);
	if (EPCBVC_CL) {
		flf_dprintf("cur_line->data:%08lx\n", EPCBVC_CL->data);
	}
	line_dump_lines(CUR_EDIT_BUF_TOP_ANCH, INT_MAX, EPCBVC_CL);
	flf_dprintf(">>>\n");
}
#endif // ENABLE_DEBUG

// End of buffers.c
