/**************************************************************************
 *   buffer.c                                                             *
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

PRIVATE be_buf_t *buf_make_top_anchor(be_buf_t *buf);
PRIVATE be_buf_t *buf_make_top_buf(be_buf_t *buf);
PRIVATE be_bufs_t *bufs_make_top_anchor(be_bufs_t *bufs);
//DDDPRIVATE be_bufs_t *bufs_make_top_bufs(be_bufs_t *bufs);

// be_buf_t manipulation routines ==========================================
// (common to edit-buffer, cut-buffer, undo-redo-buffer and history)

// Create a new buffer be_buf_t
be_buf_t *buf_create_node(const char *full_path, unsigned char buf_mode_)
{
	_mlc_set_caller
	be_buf_t *buf = (be_buf_t *)malloc__(sizeof(be_buf_t));
	return buf_init(buf, full_path, buf_mode_);
}
// Free a buffer be_buf_t
be_buf_t *buf_free_node(be_buf_t *buf)
{
	be_buf_t *next = NODE_NEXT(buf);
	buf_free_lines(buf);
	free__(buf);
	return next;
}

// initialize be_buf_t members
be_buf_t *buf_init(be_buf_t *buf, const char *full_path, unsigned char buf_mode_)
{
	memset(buf, 0x00, sizeof(*buf));

	buf_clear_link(buf);
	buf_set_file_path(buf, full_path);
	buf->orig_file_stat.st_uid = geteuid();
	buf->orig_file_stat.st_gid = getegid();
	buf->orig_file_stat.st_mode = RW0RW0R00;		// regular file rw-rw-r--(664)
	buf->orig_file_stat.st_mtime = 0;
	buf->orig_file_crc = 0;

	buf_init_anchors(buf, buf->file_path_);

	buf_views_init(buf);

	buf->mark_line = NODES_BOT_ANCH(buf);
	buf->mark_line_byte_idx = 0;
	buf->buf_lines = 0;
	buf->buf_size = 0;
	SET_BUF_STATE(buf, buf_MODE, buf_mode_);	// set default buffer mode
	return buf;
}

PRIVATE void buf_view_init(be_buf_view_t *b_v, be_buf_t *buf);
PRIVATE void buf_view_set_cur_line(be_buf_view_t *b_v, be_buf_t *buf);
void buf_views_init(be_buf_t *buf)
{
	buf_view_init(&(buf->buf_views[0]), buf);
	buf_view_init(&(buf->buf_views[1]), buf);
}
PRIVATE void buf_view_init(be_buf_view_t *b_v, be_buf_t *buf)
{
	buf_view_set_cur_line(b_v, buf);
	BUFV_CLBI(b_v) = 0;
	BUFV_CURS_Y(b_v) = MAX_SCRN_LINES;
	BUFV_CURS_X_TO_KEEP(b_v) = 0;
	BUFV_MIN_TEXT_X_TO_KEEP(b_v) = 0;
}
void buf_views_set_cur_line(be_buf_t *buf)
{
	buf_view_set_cur_line(&(buf->buf_views[0]), buf);
	buf_view_set_cur_line(&(buf->buf_views[1]), buf);
}
PRIVATE void buf_view_set_cur_line(be_buf_view_t *b_v, be_buf_t *buf)
{
	BUFV_CL(b_v) = NODES_TOP_NODE(buf);
}

void buf_view_copy(be_buf_view_t *dest, be_buf_view_t *src)
{
	memcpy__(dest, src, sizeof(be_buf_view_t));
}
void buf_set_view_x_cur_line(be_buf_t *buf, int pane_idx, be_line_t *line)
{
#ifdef ENABLE_DEBUG
	if (buf_check_line_in_buf(buf, line) == 0) {
		warning_printf("line:[%s] is not in buf[%s] !!!!\n", line->data, buf->file_path_);
	}
#endif // ENABLE_DEBUG
	BUFVX_CL(buf, pane_idx) = line;
}

be_buf_t *buf_init_anchors(be_buf_t *buf, char *initial_data)
{
	_mlc_set_caller
	line_init(NODES_TOP_ANCH(buf), initial_data);		// set something to data for an anchor
	_mlc_set_caller
	line_init(NODES_BOT_ANCH(buf), initial_data);		// set something to data for an anchor
	line_link(NODES_TOP_ANCH(buf), NODES_BOT_ANCH(buf));
	return buf;
}
void buf_set_file_path(be_buf_t *buf, const char *file_path)
{
	strlcpy__(buf->file_path_, file_path, MAX_PATH_LEN);
}
const char* buf_get_file_path(be_buf_t *buf, char *file_path)
{
	if (file_path == NULL) {
		file_path = buf->file_path_;
	} else {
		strlcpy__(file_path, buf->file_path_, MAX_PATH_LEN);
	}
	return file_path;
}
const char* buf_get_abs_path(be_buf_t *buf, char *abs_path)
{
	static char abs_path_[MAX_PATH_LEN+1];
	if (abs_path == NULL) {
		abs_path = abs_path_;
	}
	if (is_temporal_file_path(buf_get_file_path(buf, NULL)) == 0) {
		return get_abs_path(buf_get_file_path(buf, NULL), abs_path);
	}
	return buf_get_file_path(buf, NULL);
}

void buf_invalidate_file_path(be_buf_t *buf)
{
	char file_path[MAX_PATH_LEN+1];
	// "#/path/to/file" ==> "/path/to/file"
	buf_get_file_path_valid(buf, file_path);
	if (strlcmp__(file_path, PATH_INVALIDATION_TAG) != 0) {
		// "/path/to/file" ==> "#/path/to/file"
		str_prepend(file_path, MAX_PATH_LEN, PATH_INVALIDATION_TAG);
	}
	buf_set_file_path(buf, file_path);
}
const char* buf_get_file_path_valid(be_buf_t *buf, char *file_path)
{
	const char* file_path_ = buf_get_file_path(buf, file_path);
	if (strlcmp__(file_path_, PATH_INVALIDATION_TAG) == 0) {
		// "#/path/to/file" ==> "/path/to/file"
		file_path_ = &(file_path_[strlen_path(PATH_INVALIDATION_TAG)]);
	}
	return file_path_;
}
const char* buf_get_abs_path_valid(be_buf_t *buf, char *abs_path)
{
	static char abs_path_[MAX_PATH_LEN+1];
	if (abs_path == NULL) {
		abs_path = abs_path_;
	}
	return get_abs_path(buf_get_file_path_valid(buf, NULL), abs_path);
}

BOOL buf_is_empty(be_buf_t *buf)
{
	return NODES_TOP_NODE(buf) == NODES_BOT_ANCH(buf);
}

// Splice a buffer into an existing be_buf_t
PRIVATE be_buf_t *buf_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next);
be_buf_t *buf_insert_before(be_buf_t *buf, be_buf_t *new_buf)
{
	return buf_insert_between(NODE_PREV(buf), new_buf, buf);
}
be_buf_t *buf_insert_after(be_buf_t *buf, be_buf_t *new_buf)
{
	return buf_insert_between(buf, new_buf, NODE_NEXT(buf));
}
PRIVATE be_buf_t *buf_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next)
{
	buf_link(prev, new_buf);
	buf_link(new_buf, next);
	return new_buf;
}
// Connect two buffers
be_buf_t *buf_link(be_buf_t *prev, be_buf_t *next)
{
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	return prev;
}

be_buf_t *buf_create_copy_node(be_buf_t *buf)
{
	buf = buf_copy(buf_create_node("", buf_MODE_EDIT), buf);
	SET_BUF_STATE(buf, buf_MODE, buf_MODE_LIST);	// A copy must not be changeable
	SET_BUF_STATE(buf, buf_MODIFIED, 0);			// clear 'modified' flag
	return buf;
}
// This is NOT deep-copy but shallow-copy.
be_buf_t *buf_copy(be_buf_t *dest, be_buf_t *src)
{
	memcpy__(dest, src, sizeof(be_buf_t));
	if (NODE_PREV(dest) != NULL || NODE_NEXT(dest) != NULL) {
		// buffer is linked.
		// clear link and anchors to avoid double linking of the contents
		buf_clear_link(dest);
		buf_init_anchors(dest, "#copied");
	}
	BUFV0_CL(dest) = BUFV1_CL(dest) = NODES_TOP_ANCH(dest);
	dest->mark_line = NODES_TOP_ANCH(dest);
	return dest;
}

be_buf_t *buf_unlink_free(be_buf_t *buf)
{
	be_buf_t *next = NODE_NEXT(buf);
	if (IS_NODE_INT(buf)) {
		buf_unlink(buf);
		buf_free_node(buf);
	} else
	if (IS_NODE_ANCH(buf)) {
		// not unlink and not free frame
		buf_free_lines(buf);
	} else {
		WARN_PTR(buf);
		next = buf;
	}
	return next;	// return next buf
}

// Unlink a buffer from the rest of the be_buf_t
be_buf_t *buf_unlink(be_buf_t *buf)
{
	buf_avoid_wild_ptr_cur(buf);
	if (NODE_PREV(buf)) {
		buf->prev->next = NODE_NEXT(buf);
	}
	if (NODE_NEXT(buf)) {
		buf->next->prev = NODE_PREV(buf);
	}
	buf_clear_link(buf);
	return buf;
}
void buf_clear_link(be_buf_t *buf)
{
	buf->prev = NULL;
	buf->next = NULL;
}
void buf_free_lines(be_buf_t *buf)
{
	// free all lines from top to bottom
	for (be_line_t *line = NODES_TOP_NODE(buf); IS_NODE_INT(line); ) {
		line = line_unlink_free(line);
	}
}

be_line_t *buf_append_line_to_bottom(be_buf_t *buf, be_line_t *line)
{
	return line_insert(NODES_BOT_ANCH(buf), line, INSERT_BEFORE);
}

// compare two buffers in only contents
int buf_compare(be_buf_t *buf1, be_buf_t *buf2)
{
	be_line_t *line1;
	be_line_t *line2;
	int diff;

	for (line1 = NODES_TOP_NODE(buf1), line2 = NODES_TOP_NODE(buf2);
	 IS_NODE_INT(line1) && IS_NODE_INT(line2); 
	 line1 = NODE_NEXT(line1), line2 = NODE_NEXT(line2)) {
		diff = strncmp(line1->data, line2->data, MAX_EDIT_LINE_LEN);
		if (diff)
			return diff;
	}
	return IS_NODE_BOT_ANCH(line1) - IS_NODE_BOT_ANCH(line2);	// 0: exactly the same
}

int buf_renumber_from_top(be_buf_t *buf)
{
	return buf_renumber_from_line(buf, NODES_TOP_NODE(buf));
}

int buf_renumber_from_line(be_buf_t *buf, be_line_t *line)
{
	return buf->buf_lines = line_renumber_from_line(line, &buf->buf_size);
}

int buf_guess_tab_size(be_buf_t *buf)
{
#define LINES_TO_GUESS_TAB_SIZE		1000
	int lines_checked = 0;
	int lines_space4 = 0;

	for (be_line_t *line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (line_strlen(line) > 4) {
			if (strlcmp__(line->data, "    ") == 0 && line->data[4] != ' ')
				lines_space4++;
		}
		lines_checked++;
		if (lines_checked >= LINES_TO_GUESS_TAB_SIZE)
			break;
	}
	if (lines_space4 >= lines_checked / 10)
		return 8;
	return 0;
}
int buf_count_lines(be_buf_t *buf, int max_lines)
{
	int count;
	be_line_t *line;
	for (count = 0, line = NODES_TOP_NODE(buf); count < max_lines && IS_NODE_INT(line);
	 line = NODE_NEXT(line), count++) {
		// NOTHING_TO_DO
	}
	return count;
}
//------------------------------------------------------------------------------
int buf_get_file_stat(be_buf_t *buf, const char* file_path)
{
	return stat(file_path, &(buf->orig_file_stat));
}
int buf_has_orig_file_updated(be_buf_t *buf, const char* file_path)
{
	struct stat st;
	if (stat(file_path, &st)) {
		return -1;										// -1: error
	}
	return st.st_mtime > buf->orig_file_stat.st_mtime;	// >0: updated, 0: not updated
}
//------------------------------------------------------------------------------
const char *buf_mode_str(be_buf_t *buf)
{
	switch (BUF_STATE(buf, buf_MODE)) {
	default:
	case buf_MODE_EDIT:		return _("[EDIT]");
	case buf_MODE_RO:		return _("[RO]");
	case buf_MODE_LIST:		return _("[LIST]");
	}
}
const char *buf_eol_str(be_buf_t *buf)
{
	switch (BUF_STATE(buf, buf_EOL)) {
	default:
	case EOL_NIX:		return "LF(NIX)";		/*"NIX"*/;
	case EOL_MAC:		return "CR(MAC)";		/*"MAC"*/
	case EOL_DOS:		return "CR+LF(DOS)";	/*"DOS"*/
	}
}
const char *buf_enc_str(be_buf_t *buf)
{
	switch (BUF_STATE(buf, buf_ENCODE)) {
	default:
	case ENCODE_ASCII:		return "ASCII";
	case ENCODE_UTF8:		return "UTF8";
#ifdef USE_NKF
	case ENCODE_EUCJP:		return "EUCJP";
	case ENCODE_SJIS:		return "SJIS";
	case ENCODE_JIS:		return "JIS";
#endif // USE_NKF
	case ENCODE_BINARY:		return "BIN";
	}
}
const char *buf_cut_mode_str(be_buf_t *buf)
{
	char *ptr;

	switch (BUF_STATE(buf, buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:	ptr = "--";			break;
	case CUT_MODE_N_LINE:	ptr = "M ";			break;
	case CUT_MODE_H_CHAR:	ptr = "Mc";			break;
	case CUT_MODE_VH_CHAR:	ptr = "MC";			break;
	case CUT_MODE_V_LINE:	ptr = "Ml";			break;
	case CUT_MODE_HV_LINE:	ptr = "ML";			break;
	case CUT_MODE_HV_BOX:	ptr = "Mb";			break;
	case CUT_MODE_VH_BOX:	ptr = "MB";			break;
	}
	return ptr;
}
//------------------------------------------------------------------------------
void buf_fix_cur_line(be_buf_t *buf)
{
	if (IS_NODE_INT(BUFV0_CL(buf)) == 0) {
		BUFV0_CL(buf) = NODES_TOP_NODE(buf);
		BUFV0_CLBI(buf) = MIN_MAX_(0, BUFV0_CLBI(buf), line_strlen(BUFV0_CL(buf)));
	}
	if (IS_NODE_INT(BUFV1_CL(buf)) == 0) {
		BUFV1_CL(buf) = NODES_TOP_NODE(buf);
		BUFV1_CLBI(buf) = MIN_MAX_(0, BUFV1_CLBI(buf), line_strlen(BUFV1_CL(buf)));
	}
}
be_line_t *buf_set_cur_line(be_buf_t *buf, be_line_t *line)
{
	return BUFV0_CL(buf) = BUFV1_CL(buf) = line;
}
be_line_t *buf_move_cur_line_to_prev(be_buf_t *buf)
{
	be_line_t *line = BUFV0_CL(buf);
	if (IS_NODE_INT(line)) {
		BUFV0_CL(buf) = NODE_PREV(line);
		return line;	// return previous current line
	}
	// do not move and return NULL
	return NULL;
}
be_line_t *buf_move_cur_line_to_next(be_buf_t *buf)
{
	be_line_t *line = BUFV0_CL(buf);
	if (IS_NODE_INT(line)) {
		BUFV0_CL(buf) = NODE_NEXT(line);
		return line;	// return previous current line
	}
	// do not move and return NULL
	return NULL;
}

be_line_t *buf_get_line_ptr_from_line_num(be_buf_t *buf, int line_num)
{
	be_line_t *line;

	for (line = NODES_TOP_NODE(buf); line_num > 1 && IS_NODE_INT(line);
	 line_num--, line = NODE_NEXT(line)) {
		// NOTHING_TO_DO
	}
	// If line has gone through to bottom anchor, make it back to a valid line.
	if (IS_NODE_BOT_ANCH(line)) {
		line = NODE_PREV(line);
	}
	return line;
}

//------------------------------------------------------------------------------

PRIVATE long file_size;

void buf_update_crc(be_buf_t *buf)
{
	buf->orig_file_crc = buf_calc_crc(buf);
}
int buf_check_crc(be_buf_t *buf)
{
	return buf->orig_file_crc != buf_calc_crc(buf);
}
UINT16 buf_calc_crc(be_buf_t *buf)
{
	file_size = 0;
	clear_crc16ccitt();
	for (be_line_t *line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		file_size += line_strlen(line) + 1;
		for (char *ptr = line->data; ; ptr++) {
			calc_crc16ccitt(*ptr);
			if (*ptr == 0)	// count including last NUL code
				break;
		}
	}
	return get_crc16ccitt();
}

void buf_set_save_pending_timer(be_buf_t *buf, UINT16 timer)
{
	buf->orig_file_crc = timer;
}
UINT16 buf_get_save_pending_timer(be_buf_t *buf)
{
	return buf->orig_file_crc;
}

int buf_count_buf(be_buf_t *buf)
{
	int cnt = 0;
	buf = buf_make_top_buf(buf);
	for ( ; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		cnt++;
	}
	return cnt;
}

be_buf_t *buf_make_buf_intermediate(be_buf_t *buf)
{
	if (IS_PTR_VALID(buf) && (IS_NODE_INT(buf) == 0)) {
		// valid but not intermediate
		if (IS_NODE_INT(NODE_PREV(buf))) {
			buf = NODE_PREV(buf);
		} else
		if (IS_NODE_INT(NODE_NEXT(buf))) {
			buf = NODE_NEXT(buf);
		}
	}
	return buf;
}

be_buf_t *buf_get_another_buf(be_buf_t *buf)
{
	if (IS_NODE_TOP_MOST(buf) || IS_PTR_VALID(NODE_NEXT(buf))) {
		return NODE_NEXT(buf);
	}
	if (IS_NODE_BOT_MOST(buf) || IS_PTR_VALID(NODE_PREV(buf))) {
		return NODE_PREV(buf);
	}
	// error
	return buf;
}

// "#/path/to/file"
int is_temporal_file_path(const char* str)
{
	return strlcmp__(str, TEMPORAL_HISTORY_MARK) == 0;
}

//------------------------------------------------------------------------------
be_bufs_t *bufs_init(be_bufs_t *bufs, const char* name,
 const char* name_top, const char* name_bottom)
{
	bufs->prev = bufs->next = NULL;
	strlcpy__(bufs->name, name, MAX_NAME_LEN);
	bufs_init_anchors(bufs, name_top, name_bottom);
	bufs_set_cur_buf(bufs, NODES_BOT_ANCH(bufs));
	return bufs;
}
void bufs_init_anchors(be_bufs_t *bufs, const char *full_path_top, const char *full_path_bot)
{
	buf_init(NODES_TOP_ANCH(bufs), full_path_top, buf_MODE_LIST);
	buf_init(NODES_BOT_ANCH(bufs), full_path_bot, buf_MODE_LIST);
	buf_link(NODES_TOP_ANCH(bufs), NODES_BOT_ANCH(bufs));
}
void bufs_link(be_bufs_t *top_anchor, be_bufs_t *bot_anchor)
{
	top_anchor->next = bot_anchor;
	bot_anchor->prev = top_anchor;
}
void bufs_set_cur_buf(be_bufs_t* bufs, be_buf_t* buf)
{
	bufs->cur_buf = buf;
}

void bufs_insert_buf_to_top(be_bufs_t *bufs, be_buf_t *buf)
{
	buf_insert_after(NODES_TOP_ANCH(bufs), buf);
	bufs_set_cur_buf(bufs, buf);
}
void bufs_insert_buf_to_bottom(be_bufs_t *bufs, be_buf_t *buf)
{
	buf_insert_before(NODES_BOT_ANCH(bufs), buf);
	bufs_set_cur_buf(bufs, buf);
}
void bufs_insert_before(be_bufs_t *bufs, be_bufs_t *other)
{
	bufs_insert_between(NODE_PREV(bufs), other, bufs);
}
void bufs_insert_between(be_bufs_t *prev, be_bufs_t *mid, be_bufs_t *next)
{
	bufs_link(prev, mid);
	bufs_link(mid, next);
}

be_bufs_t *bufs_free_all_bufs(be_bufs_t *bufs)
{
	for ( ; IS_NODE_INT(bufs); bufs = NODE_NEXT(bufs)) {
		for (be_buf_t *buf = NODES_TOP_NODE(bufs); IS_NODE_INT(buf); ) {
			if (bufs->cur_buf == buf) {
				bufs->cur_buf = NODE_NEXT(buf);
			}
			buf = buf_unlink_free(buf);
		}
	}
	return bufs;
}

be_bufs_t *bufs_get_bufs_contains_buf(be_bufs_t *bufs, be_buf_t *cur_buf)
{
	if (cur_buf) {
		bufs = bufs_make_top_anchor(bufs);
		for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
			for (be_buf_t *buf = NODES_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
				if (buf == cur_buf) {
					return bufs;
				}
			}
		}
	}
	// not found in all buffers
	return NULL;
}
void bufs_fix_cur_buf(be_bufs_t *bufs)
{
	int count_buf = bufs_count_buf(bufs);
	int buf_idx = buf_get_buf_idx_in_bufs(bufs, bufs->cur_buf);
	if (1 <= buf_idx && buf_idx <= count_buf) {
		// `bufs->cur_buf` is valid
	} else {
		// `bufs->cur_buf` is not valid
		bufs->cur_buf = NODES_TOP_NODE(bufs);	// fix `cur_buf`
	}
	buf_fix_cur_line(bufs->cur_buf);		// fix `cur_line` too
}

int bufs_count_buf(be_bufs_t *bufs)
{
	return buf_count_buf(NODES_TOP_NODE(bufs));
}

//------------------------------------------------------------------------------

be_buf_t *buf_get_buf_by_idx(be_buf_t *buf, int buf_idx)
{
	// making sure that bufs is TOP_BUF
	buf = buf_make_top_buf(buf);
	for ( ; buf_idx > 0 && IS_NODE_INT(buf); buf = NODE_NEXT(buf), buf_idx--) {
		// NOTHING_TO_DO
	}
	return buf;	// buf may(possibly) be top/bottom anchor
}

// top-anchor:0, buf1:1, buf2:2, ..., bufN:n, bottom-anchor:(n+1)
int buf_get_buf_idx(be_buf_t *buf)
{
	int buf_idx = 0;
	for ( ; IS_NODE_TOP_OOL(buf) == 0; buf = NODE_PREV(buf), buf_idx++) {
		// NOTHING_TO_DO
	}
	return buf_idx;		// 0: anchor, 1~: intermediate buffer
}
PRIVATE int buf_get_buf_idx_from_top_anch(be_buf_t *buff, be_buf_t *buf);
int buf_get_buf_idx_in_bufs(be_bufs_t *bufs, be_buf_t *buf)
{
	// -1:not found in the `bufs`
	return buf_get_buf_idx_from_top_anch(NODES_TOP_ANCH(bufs), buf);
}
PRIVATE int buf_get_buf_idx_from_top_anch(be_buf_t *buff, be_buf_t *buf)
{
	for (int buf_idx = 0; IS_PTR_VALID(buff); buff = NODE_NEXT(buff), buf_idx++) {
		if (buff == buf)
			return buf_idx;	// found
	}
	return -1;	// not found
}

be_buf_t *buf_get_buf_by_file_path(be_buf_t *buf, const char *file_path)
{
	buf = buf_make_top_buf(buf);
	for ( ; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		// compare in file_path
		if (strcmp(buf_get_file_path(buf, NULL), file_path) == 0) {
			return buf;	// found
		}
		// compare in abs_path
		if (compare_file_path_in_abs_path(buf_get_file_path(buf, NULL), file_path) == 0) {
			return buf;	// found
		}
	}
	return NULL;		// not found
}
be_buf_t *buf_get_buf_by_file_name(be_buf_t *buf, const char *file_name)
{
	buf = buf_make_top_buf(buf);
	for ( ; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		if (compare_file_path_from_tail(buf_get_file_path(buf, NULL), file_name) == 0) {
			return buf;	// found by file_path
		}
		if (compare_file_path_from_tail(buf_get_abs_path(buf, NULL), file_name) == 0) {
			return buf;	// found by abs_path
		}
	}
	return NULL;		// not found
}

//------------------------------------------------------------------------------
void bufs_renumber_all_bufs_from_top(be_bufs_t *bufs)
{
	for (be_buf_t *buf = NODES_TOP_NODE(bufs); IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		buf_renumber_from_top(buf);
	}
}

//------------------------------------------------------------------------------
void bufss_init(be_bufss_t *bufss, const char *name,
 const char *name_top, const char *name_bottom)
{
	strlcpy__(bufss->name, name, MAX_NAME_LEN);
	bufs_init(NODES_TOP_ANCH(bufss), name_top, "##BUFSS_top_top_anch", "##BUFSS_top_bot_anch");
	bufs_init(NODES_BOT_ANCH(bufss), name_bottom, "##BUFSS_bot_top_anch", "##BUFSS_bot_bot_anch");
	bufs_link(NODES_TOP_ANCH(bufss), NODES_BOT_ANCH(bufss));
	bufss->cur_bufs = NODES_BOT_ANCH(bufss);
}
void bufss_insert_bufs_to_bottom(be_bufss_t *bufss, be_bufs_t *bufs)
{
	bufs_insert_before(NODES_BOT_ANCH(bufss), bufs);
	bufss->cur_bufs = bufs;
}

// top-anchor:0, bufs1:1, bufs2:2, ..., bufsN:n, bottom-anchor:(n+1)
int bufs_get_bufs_idx_in_bufss(be_bufs_t *bufs, be_buf_t *buf)
{
	bufs = bufs_make_top_anchor(bufs);
	for (int bufs_idx = 0; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs), bufs_idx++) {
		if (buf_get_buf_idx_in_bufs(bufs, buf) >= 0) {
			// buf was found in the bufs
			return bufs_idx;	// return bufs_idx of the bufs
		}
	}
	return -1;	// not found
}

PRIVATE be_buf_t *buf_make_top_anchor(be_buf_t *buf)
{
	for ( ; IS_PTR_VALID(buf) && IS_NODE_TOP_ANCH(buf) == 0; buf = NODE_PREV(buf)) {
		// NOTHING_TO_DO
	}
	return buf;
}
PRIVATE be_buf_t *buf_make_top_buf(be_buf_t *buf)
{
	buf = buf_make_top_anchor(buf);
	if (IS_NODE_TOP_ANCH(buf)) {
		buf = NODE_NEXT(buf);
	}
	return buf;
}

PRIVATE be_bufs_t *bufs_make_top_anchor(be_bufs_t *bufs)
{
	for ( ; IS_PTR_VALID(bufs) && IS_NODE_TOP_ANCH(bufs) == 0; bufs = NODE_PREV(bufs)) {
		// NOTHING_TO_DO
	}
	return bufs;
}
//DDDPRIVATE be_bufs_t *bufs_make_top_bufs(be_bufs_t *bufs)
//DDD{
//DDD	if (IS_NODE_TOP_ANCH(bufs)) {
//DDD		bufs = NODE_NEXT(bufs);
//DDD	}
//DDD	for ( ; IS_PTR_VALID(bufs) && IS_NODE_TOP_MOST(bufs) == 0; bufs = NODE_PREV(bufs)) {
//DDD		// NOTHING_TO_DO
//DDD	}
//DDD	return bufs;
//DDD}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG

be_line_t *buf_check_line_in_buf(be_buf_t *buf, be_line_t *line_)
{
	for (be_line_t *line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (line == line_)
			return line_;
	}
	return NULL;
}
be_line_t *buf_check_line_in_buf_anchs(be_buf_t *buf, be_line_t *line_)
{
	if ((line_ == NODES_TOP_ANCH(buf)) || (line_ == NODES_BOT_ANCH(buf))) {
		return line_;
	}
	return NULL;
}
void buf_dump_bufs(be_buf_t *buf)
{
	int cnt;

	flf_d_printf("0============================================\n");
	for (cnt = 0; cnt < 100 && IS_NODE_INT(buf); cnt++, buf = NODE_NEXT(buf)) {
		dump_buf_view_x(buf, 0);
		dump_buf_view_x(buf, 1);
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
	flf_d_printf("9============================================\n");
}
void buf_dump_bufs_lines(be_buf_t *buf, const char *label)
{
	flf_d_printf("%s {{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n", label);
	for (int cnt = 0; cnt < 100 && IS_PTR_VALID(buf); cnt++, buf = NODE_NEXT(buf)) {
		if (buf_count_lines(buf, 3)) {
			buf_dump_lines(buf, 3);
		}
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
	flf_d_printf("%s }}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n", label);
}
void buf_dump_lines(be_buf_t *buf, int lines)
{
	buf_dump_name(buf);
	if (buf == NULL) {
		return;
	}
	line_dump_lines(NODES_TOP_ANCH(buf), lines, BUFV0_CL(buf));
}
void buf_dump_ptrs(be_buf_t *buf)
{
	flf_d_printf("%saddr:%08lx,prev:%08lx,next:%08lx,line:%08lx\n",
	 buf == get_epc_buf() ? ">" : " ",
	 buf, NODE_PREV(buf), NODE_NEXT(buf), NODES_TOP_NODE(buf));
	line_dump_lines(NODES_TOP_ANCH(buf), 3, BUFV0_CL(buf));
	line_dump_lines(NODES_TOP_ANCH(buf), 3, BUFV1_CL(buf));
}
void buf_dump_name(be_buf_t *buf)
{
	if (buf == NULL) {
		flf_d_printf("buf: NULL\n");
		return;
	}
	flf_d_printf("file_path: [%s]\n", buf_get_file_path(buf, NULL));
}
const char* buf_dump_buf_state(be_buf_t *buf)
{
	return sprintf_s("buf_state:%08lx", buf->buf_state);
}

void bufs_dump_all_bufs(be_bufs_t *bufs)
{
	flf_d_printf("00============================================\n");
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		flf_d_printf("bufs: [%s]\n", bufs->name);
		for (be_buf_t *buf = NODES_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
			flf_d_printf(" %cbuf: [%s]\n",
			 (bufs->cur_buf == buf) ? '>' : ' ', buf_get_file_path(buf, NULL));
			flf_d_printf("    buf->v0_str: [%s]\n", buf->buf_views[0].cur_line->data);
			flf_d_printf("    buf->v1_str: [%s]\n", buf->buf_views[1].cur_line->data);
		}
	}
	flf_d_printf("99============================================\n");
}
void bufs_dump_name(be_bufs_t *bufs)
{
	if (bufs == NULL) {
		flf_d_printf("bufs: NULL\n");
		return;
	}
	flf_d_printf("name: [%s]\n", bufs->name);
}

#endif // ENABLE_DEBUG

// End of buffer.c
