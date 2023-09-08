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
	int view_idx;				// 0: left pane, 1: right pane
	be_buf_t *bufs[BUF_VIEWS];	// edit-buffers shown in the left and the right pane
} editor_views_t;

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
	BUFS_IDX_SIZE,		// 6
};
#define HEADS_BUFS		BUFS_IDX_SIZE+1		// add 1 for end of list (NULL-ptr)

// collection of buffers
extern be_bufs_t* head_of_bufs[HEADS_BUFS];

// Edit buffers ---------------------------------------------------------------
extern be_bufs_t edit_buffers;
extern editor_views_t editor_views;
extern editor_views_t *cur_editor_views;
extern be_buf_t *c_e_b;
extern be_buf_view_t *c_e_b_v;
#define EDIT_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&edit_buffers)
#define EDIT_BUFS_TOP_BUF		BUFS_TOP_BUF(&edit_buffers)
#define EDIT_BUFS_BOT_BUF		BUFS_BOT_BUF(&edit_buffers)
#define EDIT_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&edit_buffers)
// current edit buffer --------------------------------------------------------
#define CUR_EDIT_BUF_TOP_ANCH	BUFS_TOP_ANCH(c_e_b)
#define CUR_EDIT_BUF_TOP_LINE	BUFS_TOP_BUF(c_e_b)
#define CUR_EDIT_BUF_BOT_LINE	BUFS_BOT_BUF(c_e_b)
#define CUR_EDIT_BUF_BOT_ANCH	BUFS_BOT_ANCH(c_e_b)

#define BUFVX(buf, idx)					(&((buf)->views[idx]))
#define BUFV_CL(b_v)						((b_v)->cur_line)
#define BUFV_CLBI(b_v)						((b_v)->cur_line_byte_idx)
#define BUFV_CURSOR_Y(b_v)					((b_v)->cursor_y)
#define BUFV_CURSOR_X_TO_KEEP(b_v)			((b_v)->cursor_x_to_keep)
#define BUFV_MIN_TEXT_X_TO_BE_DISP(b_v)		((b_v)->min_text_x_to_be_disp)
#define BUFVX_CL(buf, idx)						BUFV_CL(BUFVX((buf), (idx)))
#define BUFVX_CLBI(buf, idx)					BUFV_CLBI(BUFVX((buf), (idx)))
#define BUFVX_CURSOR_Y(buf, idx)				BUFV_CURSOR_Y(BUFVX((buf), (idx)))
#define BUFVX_CURSOR_X_TO_KEEP(buf, idx)		BUFV_CURSOR_X_TO_KEEP(BUFVX((buf), (idx)))
#define BUFVX_MIN_TEXT_X_TO_BE_DISP(buf, idx)	BUFV_MIN_TEXT_X_TO_BE_DISP(BUFVX((buf), (idx)))
#define BUFV0_CL(buf)						BUFVX_CL((buf), 0)
#define BUFV0_CLBI(buf)						BUFVX_CLBI((buf), 0)
#define BUFV0_CURSOR_Y(buf)					BUFVX_CURSOR_Y((buf), 0)
#define BUFV0_CURSOR_X_TO_KEEP(buf)			BUFVX_CURSOR_X_TO_KEEP((buf), 0)
#define BUFV0_MIN_TEXT_X_TO_BE_DISP(buf)	BUFVX_MIN_TEXT_X_TO_BE_DISP((buf), 0)
#define BUFV1_CL(buf)						BUFVX_CL((buf), 1)
#define BUFV1_CLBI(buf)						BUFVX_CLBI((buf), 1)
#define BUFV1_CURSOR_Y(buf)					BUFVX_CURSOR_Y((buf), 1)
#define BUFV1_CURSOR_X_TO_KEEP(buf)			BUFVX_CURSOR_X_TO_KEEP((buf), 1)
#define BUFV1_MIN_TEXT_X_TO_BE_DISP(buf)	BUFVX_MIN_TEXT_X_TO_BE_DISP((buf), 1)
#define CEBV_CL							BUFV_CL(c_e_b_v)
#define CEBV_CLBI						BUFV_CLBI(c_e_b_v)
#define CEBV_CURSOR_Y					BUFV_CURSOR_Y(c_e_b_v)
#define CEBV_CURSOR_X_TO_KEEP			BUFV_CURSOR_X_TO_KEEP(c_e_b_v)
#define CEBV_MIN_TEXT_X_TO_BE_DISP		BUFV_MIN_TEXT_X_TO_BE_DISP(c_e_b_v)
#define CEBV0_CL						BUFVX_CL(c_e_b, 0)
#define CEBV0_CLBI						BUFVX_CLBI(c_e_b, 0)
#define CEBV0_CURSOR_Y					BUFVX_CURSOR_Y(c_e_b, 0)
#define CEBV0_CURSOR_X_TO_KEEP			BUFVX_CURSOR_X_TO_KEEP(c_e_b, 0)
#define CEBV0_MIN_TEXT_X_TO_BE_DISP		BUFVX_MIN_TEXT_X_TO_BE_DISP(c_e_b, 0)
#define CEBV1_CL						BUFVX_CL(c_e_b, 1)
#define CEBV1_CLBI						BUFVX_CLBI(c_e_b, 1)
#define CEBV1_CURSOR_Y					BUFVX_CURSOR_Y(c_e_b, 1)
#define CEBV1_CURSOR_X_TO_KEEP			BUFVX_CURSOR_X_TO_KEEP(c_e_b, 1)
#define CEBV1_MIN_TEXT_X_TO_BE_DISP		BUFVX_MIN_TEXT_X_TO_BE_DISP(c_e_b, 1)
#define CEB_ML				(c_e_b->mark_line)
#define CEB_MLBI			(c_e_b->mark_line_byte_idx)

// Cut buffers ----------------------------------------------------------------
extern be_bufs_t cut_buffers;
#define CUT_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&cut_buffers)
#define CUT_BUFS_TOP_BUF		BUFS_TOP_BUF(&cut_buffers)
#define CUT_BUFS_BOT_BUF		BUFS_BOT_BUF(&cut_buffers)
#define CUT_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&cut_buffers)
// current cut buffer ---------------------------------------------------------
#define CUR_CUT_BUF				CUT_BUFS_TOP_BUF
#define CUR_CUT_BUF_TOP_LINE	BUF_TOP_LINE(CUR_CUT_BUF)
#define CUR_CUT_BUF_BOT_ANCH	BUF_BOT_ANCH(CUR_CUT_BUF)

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
#define CUR_UNDO_BUF			UNDO_BUFS_TOP_BUF
#define CUR_UNDO_BUF_BOT_ANCH	BUF_BOT_ANCH(CUR_UNDO_BUF)
// Redo buffers ---------------------------------------------------------------
extern be_bufs_t redo_buffers;
#define REDO_BUFS_TOP_ANCH		BUFS_TOP_ANCH(&redo_buffers)
#define REDO_BUFS_TOP_BUF		BUFS_TOP_BUF(&redo_buffers)
#define REDO_BUFS_BOT_BUF		BUFS_BOT_BUF(&redo_buffers)
#define REDO_BUFS_BOT_ANCH		BUFS_BOT_ANCH(&redo_buffers)
// current redo buffer --------------------------------------------------------
#define CUR_REDO_BUF			REDO_BUFS_TOP_BUF
#endif // ENABLE_UNDO

//=============================================================================

void init_head_of_bufs(void);
void init_buffers(void);
void free_all_buffers(void);

void init_edit_bufs(void);
void free_all_edit_bufs(void);
int free_cur_edit_buf(void);
int free_edit_buf(be_buf_t *edit_buf);

void buffer_avoid_wild_ptr_cur(be_buf_t *buf);
void buffer_avoid_wild_ptr(be_buf_t *buf, be_buf_t **buf_ptr);

void line_avoid_wild_ptr_cur(be_line_t *line);
void line_avoid_wild_ptr(be_line_t *line, be_line_t **line_ptr);

be_buf_t *get_edit_buf_from_abs_path(const char *abs_path);
int get_edit_buf_idx_from_buf(be_buf_t *edit_buf);

void create_edit_buf(const char *file_path);

void init_editor_views(editor_views_t *editor_views);
void set_cur_editor_view_idx(int view_idx);
int get_cur_editor_view_idx(void);
void set_cur_edit_buf(be_buf_t *buf);
void set_cur_editor_view_buf(int view_idx, be_buf_t *buf);

// Some compiler needs "static"
inline static void set_c_e_b(be_buf_t *ceb);
inline static be_buf_t *get_c_e_b(void);
inline static void set_c_e_b_v(be_buf_view_t *cebv);
inline static be_buf_view_t *get_c_e_b_v(void);
// current edit buffer
inline static void set_c_e_b(be_buf_t *ceb)
{
	c_e_b = ceb;
}
inline static be_buf_t *get_c_e_b(void)
{
	return c_e_b;
}
// current line
inline static void set_c_e_b_v(be_buf_view_t *cebv)
{
	c_e_b_v = cebv;
}
inline static be_buf_view_t *get_c_e_b_v(void)
{
	return c_e_b_v;
}

#ifdef ENABLE_DEBUG
void dump_cur_editor_views(void);
#endif // ENABLE_DEBUG

be_line_t *append_string_to_cur_edit_buf(const char *string);
void append_magic_line(void);
int count_edit_bufs(void);
int is_c_e_b_valid(void);

//-----------------------------------------------------------------------------
void init_cut_bufs(void);
void free_all_cut_bufs(void);
be_buf_t *push_cut_buf(void);
int pop_cut_buf(void);
be_line_t *append_string_to_cur_cut_buf(const char *string);
int count_cut_bufs(void);
int count_cur_cut_buf_lines(void);

//-----------------------------------------------------------------------------

void init_bufs_top_bot_anchor(
 be_buf_t *buf_top, const char *full_path_top,
 be_buf_t *buf_bot, const char *full_path_bot);

be_buf_t *get_buf_from_bufs_by_idx(be_buf_t *bufs, int buf_idx);
int get_buf_idx_in_bufs(be_buf_t *bufs, be_buf_t *buf);
be_buf_t *get_buf_from_bufs_by_abs_path(be_buf_t *bufs, const char *abs_path);

//-----------------------------------------------------------------------------

void renumber_all_bufs_from_top(be_bufs_t *bufs);

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
#define CUR_EBUF_STATE(var)					BUF_STATE(c_e_b, var)
#define SET_CUR_EBUF_STATE(var, val)		SET_BUF_STATE(c_e_b, var, val)
#define TOGGLE_CUR_EBUF_STATE(var)			TOGGLE_BUF_STATE(c_e_b, var)
#define INC_CUR_EBUF_STATE(var, min, max)	INC_BUF_STATE(c_e_b, var, min, max)
#define CMP_CUR_EBUF_STATE(var, val)		CMP_BUF_STATE(c_e_b, var, val)

// current cut buffer state
#define CUR_CBUF_STATE(var)					BUF_STATE(CUR_CUT_BUF, var)
#define SET_CUR_CBUF_STATE(var, val)		SET_BUF_STATE(CUR_CUT_BUF, var, val)

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

#ifdef USE_NKF
int set_encode_ascii(void);
const char *get_str_encode_ascii(void);

int set_encode_utf8(void);
const char *get_str_encode_utf8(void);

int set_encode_eucjp(void);
const char *get_str_encode_eucjp(void);

int set_encode_sjis(void);
const char *get_str_encode_sjis(void);

int set_encode_jis(void);
const char *get_str_encode_jis(void);

int set_encode_binary(void);
const char *get_str_encode_binary(void);

int set_encode(int encode);
const char *get_str_encode(void);
#endif // USE_NKF

void set_cur_buf_modified(void);

int do_tog_view_mode(void);
int do_tog_line_wrap_mode(void);
int do_tog_tab_size(void);
int do_inc_tab_size(void);
int do_set_nix_file(void);
int do_set_mac_file(void);
int do_set_dos_file(void);

#ifdef USE_NKF
int do_set_encode_ascii(void);
int do_set_encode_utf8(void);
int do_set_encode_eucjp(void);
int do_set_encode_sjis(void);
int do_set_encode_jis(void);
int do_set_encode_binary(void);
#endif // USE_NKF

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
