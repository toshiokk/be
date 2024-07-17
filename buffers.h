/**************************************************************************
 *   buffers.h                                                            *
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

#ifndef buffers_h
#define buffers_h

typedef struct {
	int cur_pane_idx;				// 0: left pane, 1: right pane
	be_buf_t *bufs[EDITOR_PANES];	// edit-buffers shown in the left and the right pane
} editor_panes_t;
extern editor_panes_t *cur_editor_panes;	// Current Editor Panes

// two panes of editor(editor_panes)
// - each pane reference a `buffer`
// two panes of filer(filer_panes)
// - each pane reference a `directory-view`
//
// `buffers`
// - buffer has two views and accessed from each pane of editor
// `filer_views`
// - two directory views referenced from each pane.

//-----------------------------------------------------------------------------

enum BUFS_IDX {
	BUFS_IDX_EDIT,		// 0
	BUFS_IDX_CUT,		// 1
	BUFS_IDX_HISTORY,	// 2
	BUFS_IDX_HELP,		// 3
#ifdef ENABLE_UNDO
	BUFS_IDX_UNDO,		// 4
	BUFS_IDX_REDO,		// 5
#endif // ENABLE_UNDO
};

// collection of collections of buffers (Root of all buffers)
extern be_bufs_t bufss_top_anchor;		//< top buffers
extern be_bufs_t bufss_bot_anchor;		//< bottom buffers

// Edit buffers ---------------------------------------------------------------
extern be_bufs_t edit_buffers;
#define EDIT_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&edit_buffers)
#define EDIT_BUFS_TOP_BUF		BUFS_TOP_BUF(&edit_buffers)
#define EDIT_BUFS_BOT_BUF		BUFS_BOT_BUF(&edit_buffers)
#define EDIT_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&edit_buffers)
// current edit buffer --------------------------------------------------------
#define CUR_EDIT_BUF_TOP_ANCH	BUF_TOP_ANCH(get_epc_buf())
#define CUR_EDIT_BUF_TOP_LINE	BUF_TOP_LINE(get_epc_buf())
#define CUR_EDIT_BUF_BOT_LINE	BUF_BOT_LINE(get_epc_buf())
#define CUR_EDIT_BUF_BOT_ANCH	BUF_BOT_ANCH(get_epc_buf())

// EPC/epc : Editor Pane Current
// EPX/epx : Editor Pane indeXed
// EPBV  : Editor Pane -> Buffer View
// BUF_V : Buffer View
// BUF_VX: Buffer View indeXed
// BUF_V0: Buffer View index-0
// BUF_V1: Buffer View index-1

#define BUF_VX(buf, idx)					(&((buf)->buf_views[idx]))

#define BUFV_CL(b_v)						((b_v)->cur_line)
#define BUFV_CL_DATA(b_v)					((b_v)->cur_line->data)
#define BUFV_CLBI(b_v)						((b_v)->cur_line_byte_idx)
#define BUFV_CURSOR_Y(b_v)					((b_v)->cursor_y)
#define BUFV_CURSOR_X_TO_KEEP(b_v)			((b_v)->cursor_x_to_keep)
#define BUFV_MIN_TEXT_X_TO_KEEP(b_v)		((b_v)->min_text_x_to_keep)

#define BUFVX_CL(buf, idx)					BUFV_CL(BUF_VX((buf), (idx)))
#define BUFVX_CLBI(buf, idx)				BUFV_CLBI(BUF_VX((buf), (idx)))
#define BUFVX_CURSOR_Y(buf, idx)			BUFV_CURSOR_Y(BUF_VX((buf), (idx)))
#define BUFVX_CURSOR_X_TO_KEEP(buf, idx)	BUFV_CURSOR_X_TO_KEEP(BUF_VX((buf), (idx)))
#define BUFVX_MIN_TEXT_X_TO_KEEP(buf, idx)	BUFV_MIN_TEXT_X_TO_KEEP(BUF_VX((buf), (idx)))
#define BUFV0_CL(buf)						BUFVX_CL((buf), 0)
#define BUFV1_CL(buf)						BUFVX_CL((buf), 1)

#define EPCBVC_CL							BUFV_CL(get_epc_buf_view())
#define EPCBVC_CLBI							BUFV_CLBI(get_epc_buf_view())
#define EPCBVC_CURSOR_Y						BUFV_CURSOR_Y(get_epc_buf_view())
#define EPCBVC_CURSOR_X_TO_KEEP				BUFV_CURSOR_X_TO_KEEP(get_epc_buf_view())
#define EPCBVC_MIN_TEXT_X_TO_KEEP			BUFV_MIN_TEXT_X_TO_KEEP(get_epc_buf_view())
#define EPCBVC_CL_EPCBVC_CLBI				(&(EPCBVC_CL->data[EPCBVC_CLBI]))

#define EPCBVX_CL(idx)						BUFVX_CL(get_epc_buf(), (idx))
#define EPCBVX_CLBI(idx)					BUFVX_CLBI(get_epc_buf(), (idx))

#define EPCB_ML								(get_epc_buf()->mark_line)
#define EPCB_MLBI							(get_epc_buf()->mark_line_byte_idx)

// Cut buffers ----------------------------------------------------------------
extern be_bufs_t cut_buffers;
#define CUT_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&cut_buffers)
#define CUT_BUFS_TOP_BUF		BUFS_TOP_BUF(&cut_buffers)
#define CUT_BUFS_BOT_BUF		BUFS_BOT_BUF(&cut_buffers)
#define CUT_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&cut_buffers)
// current cut buffer ---------------------------------------------------------
#define TOP_BUF_OF_CUT_BUFS		CUT_BUFS_TOP_BUF
#define CUR_CUT_BUF_TOP_LINE	BUF_TOP_LINE(CUT_BUFS_TOP_BUF)
#define CUR_CUT_BUF_BOT_ANCH	BUF_BOT_ANCH(CUT_BUFS_TOP_BUF)

// History buffers ------------------------------------------------------------
extern be_bufs_t history_buffers;
#define HIST_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&history_buffers)
#define HIST_BUFS_TOP_BUF		BUFS_TOP_BUF(&history_buffers)
#define HIST_BUFS_BOT_BUF		BUFS_BOT_BUF(&history_buffers)
#define HIST_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&history_buffers)

// Help buffers ------------------------------------------------------------
extern be_bufs_t help_buffers;
#define HELP_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&help_buffers)
#define HELP_BUFS_TOP_BUF		BUFS_TOP_BUF(&help_buffers)
#define HELP_BUFS_BOT_BUF		BUFS_BOT_BUF(&help_buffers)
#define HELP_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&help_buffers)

#ifdef ENABLE_UNDO
// Undo buffers ---------------------------------------------------------------
extern be_bufs_t undo_buffers;
#define UNDO_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&undo_buffers)
#define UNDO_BUFS_TOP_BUF		BUFS_TOP_BUF(&undo_buffers)
#define UNDO_BUFS_BOT_BUF		BUFS_BOT_BUF(&undo_buffers)
#define UNDO_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&undo_buffers)
// current undo buffer --------------------------------------------------------
#define TOP_BUF_OF_UNDO_BUFS	UNDO_BUFS_TOP_BUF
#define CUR_UNDO_BUF_BOT_ANCH	BUF_BOT_ANCH(UNDO_BUFS_TOP_BUF)
// Redo buffers ---------------------------------------------------------------
extern be_bufs_t redo_buffers;
#define REDO_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&redo_buffers)
#define REDO_BUFS_TOP_BUF		BUFS_TOP_BUF(&redo_buffers)
#define REDO_BUFS_BOT_BUF		BUFS_BOT_BUF(&redo_buffers)
#define REDO_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&redo_buffers)
// current redo buffer --------------------------------------------------------
#define TOP_BUF_OF_REDO_BUFS	REDO_BUFS_TOP_BUF
#endif // ENABLE_UNDO

//=============================================================================

void init_head_of_bufs(void);
void init_buffers(void);
void free_all_buffers(void);

void init_edit_bufs(void);
int free_cur_edit_buf(void);
int free_edit_buf(be_buf_t *edit_buf);

void buf_avoid_wild_ptr_cur(be_buf_t *buf);
void buf_avoid_wild_ptr(be_buf_t *buf, be_buf_t **buf_ptr);

void line_avoid_wild_ptr_cur(be_line_t *line);
void line_avoid_wild_ptr(be_line_t **line_ptr, be_line_t *line);

be_buf_t *get_edit_buf_by_file_path(const char *abs_path);
be_buf_t *get_edit_buf_by_file_name(const char *file_name);

void create_edit_buf(const char *file_path);

void init_cur_editor_panes(editor_panes_t *eps);
void copy_editor_panes(editor_panes_t *dest, editor_panes_t *src);
editor_panes_t *push_editor_panes(editor_panes_t *next_eps);
void pop_editor_panes(editor_panes_t *prev_eps, editor_panes_t *eps, BOOL copy_back);

void set_editor_cur_pane_idx(int pane_idx);
int get_editor_cur_pane_idx(void);

void set_epc_buf(be_buf_t *buf);
be_buf_t *get_epc_buf(void);
be_buf_view_t *get_epc_buf_view(void);

void set_epx_buf(int pane_idx, be_buf_t *buf);
be_buf_t *get_epx_buf(int pane_idx);

// Some compiler needs "inline static" for inline functions

#ifdef ENABLE_DEBUG
void dump_editor_panes(void);
void dump_editor_pane_x(int pane_idx);
void dump_buf_views(be_buf_t *buf);
void dump_buf_view_x(be_buf_t *buf, int pane_idx);
#endif // ENABLE_DEBUG

be_line_t *append_string_to_cur_edit_buf(const char *string);
void append_magic_line(void);
int count_edit_bufs(void);
int is_epc_buf_valid(void);

//-----------------------------------------------------------------------------

void init_cut_bufs(void);
void free_all_cut_bufs(void);
be_buf_t *push_cut_buf(void);
int pop_n_free_from_cut_buf(void);
be_line_t *append_string_to_cur_cut_buf(const char *string);
int count_cut_bufs(void);
int count_cur_cut_buf_lines(void);

void renumber_cur_buf_from_top(void);
struct be_line_t *get_line_ptr_from_cur_buf_line_num(int line_num);

void update_cur_buf_crc(void);
int check_cur_buf_modified(void);

//-----------------------------------------------------------------------------

#define BUF_STATE(buf, var)					(&(buf)->buf_state)->var
#define SET_BUF_STATE(buf, var, val)		(BUF_STATE(buf, var) = val)
#define TOGGLE_BUF_STATE(buf, var)			(BUF_STATE(buf, var)++)
#define INC_BUF_STATE(buf, var, min, max)	((++BUF_STATE(buf, var) <= (max)) \
 ? (BUF_STATE(buf, var)) : (BUF_STATE(buf, var) = (min)))
#define CMP_BUF_STATE(buf, var, val)		(BUF_STATE(buf, var) == val)

// current edit buffer state
#define CUR_EBUF_STATE(var)					BUF_STATE(get_epc_buf(), var)
#define SET_CUR_EBUF_STATE(var, val)		SET_BUF_STATE(get_epc_buf(), var, val)
#define TOGGLE_CUR_EBUF_STATE(var)			TOGGLE_BUF_STATE(get_epc_buf(), var)
#define INC_CUR_EBUF_STATE(var, min, max)	INC_BUF_STATE(get_epc_buf(), var, min, max)
#define CMP_CUR_EBUF_STATE(var, val)		CMP_BUF_STATE(get_epc_buf(), var, val)

// current cut buffer state
#define CUR_CBUF_STATE(var)					BUF_STATE(TOP_BUF_OF_CUT_BUFS, var)
#define SET_CUR_CBUF_STATE(var, val)		SET_BUF_STATE(TOP_BUF_OF_CUT_BUFS, var, val)

int tog_view_mode(void);
const char *get_str_view_mode(void);

int tog_line_wrap_mode(void);
const char *get_str_line_wrap_mode(void);

int tog_tab_size(void);
int inc_tab_size(void);
const char *get_str_tab_size(void);

int get_cur_buf_tab_size(void);
void update_tab_size(void);

int set_nix_file(void);
const char *get_str_nix_file(void);
int set_mac_file(void);
const char *get_str_mac_file(void);
int set_dos_file(void);
const char *get_str_dos_file(void);
int set_eol(int eol);
const char *get_str_eol(void);

int set_encode_ascii(void);
const char *get_str_encode_ascii(void);

int set_encode_utf8(void);
const char *get_str_encode_utf8(void);

#ifdef USE_NKF
int set_encode_eucjp(void);
const char *get_str_encode_eucjp(void);

int set_encode_sjis(void);
const char *get_str_encode_sjis(void);

int set_encode_jis(void);
const char *get_str_encode_jis(void);
#endif // USE_NKF

int set_encode_binary(void);
const char *get_str_encode_binary(void);

int set_encode(int encode);
const char *get_str_encode(void);

void set_cur_buf_modified(void);
int is_any_edit_buf_modified(void);

int doe_tog_view_mode(void);
int doe_tog_line_wrap_mode(void);
int doe_tog_tab_size(void);
int doe_inc_tab_size(void);
int doe_set_nix_file(void);
int doe_set_mac_file(void);
int doe_set_dos_file(void);

int doe_set_encode_ascii(void);
int doe_set_encode_utf8(void);
#ifdef USE_NKF
int doe_set_encode_eucjp(void);
int doe_set_encode_sjis(void);
int doe_set_encode_jis(void);
#endif // USE_NKF
int doe_set_encode_binary(void);

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_cur_edit_buf_lines(void);
void dump_edit_bufs(void);
void dump_edit_bufs_lines(void);
void dump_cut_bufs(void);
void dump_cut_bufs_lines(void);

void dump_cur_edit_buf(void);
#endif // ENABLE_DEBUG

#endif // buffers_h

// End of buffers.h
