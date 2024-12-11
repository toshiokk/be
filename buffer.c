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
///PRIVATE be_bufs_t *bufs_make_top_bufs(be_bufs_t *bufs);

// be_buf_t manipulation routines ==========================================
// (common to edit-buffer, cut-buffer, undo-redo-buffer and history)

// Create a new buffer be_buf_t
be_buf_t *buf_create_node(const char *full_path)
{
	_mlc_set_caller
	be_buf_t *buf = (be_buf_t *)malloc__(sizeof(be_buf_t));
	return buf_init(buf, full_path);
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
be_buf_t *buf_init(be_buf_t *buf, const char *full_path)
{
	memset(buf, 0x00, sizeof(*buf));

	buf_clear_link(buf);
	buf_set_file_abs_path(buf, full_path);
	buf->orig_file_stat.st_uid = geteuid();
	buf->orig_file_stat.st_gid = getegid();
	buf->orig_file_stat.st_mode = RW0RW0R00;		// regular file rw-rw-r--(664)
	buf->orig_file_stat.st_mtime = time(NULL);		// time file was created
	buf->orig_file_crc = 0;

	buf_init_anchors(buf, buf->file_path_);

	buf_views_init(buf);

	buf->mark_line = NODES_BOT_ANCH(buf);
	buf->mark_line_byte_idx = 0;
	buf->buf_lines = 0;
	buf->buf_size = 0;
	SET_BUF_STATE(buf, buf_MODE, buf_MODE_EDIT);
	return buf;
}
void buf_views_init(be_buf_t *buf)
{
	buf_view_init(&(buf->buf_views[0]), buf);
	buf_view_init(&(buf->buf_views[1]), buf);
}
void buf_view_init(be_buf_view_t *b_v, be_buf_t *buf)
{
	BUFV_CL(b_v) = NODES_BOT_ANCH(buf);
	BUFV_CLBI(b_v) = 0;
	BUFV_CURS_Y(b_v) = MAX_SCRN_LINES;
	BUFV_CURS_X_TO_KEEP(b_v) = 0;
	BUFV_MIN_TEXT_X_TO_KEEP(b_v) = 0;
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
void buf_set_file_abs_path(be_buf_t *buf, const char *file_path)
{
	buf_set_file_path(buf, file_path);
	buf_set_abs_path(buf, file_path);
}
void buf_set_file_path(be_buf_t *buf, const char *file_path)
{
	strlcpy__(buf->file_path_, file_path, MAX_PATH_LEN);
}
void buf_set_abs_path(be_buf_t *buf, const char *file_path)
{
	get_abs_path(file_path, buf->abs_path_);
}
void buf_get_file_path(be_buf_t *buf, char *file_path)
{
	strlcpy__(file_path, buf->file_path_, MAX_PATH_LEN);
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

be_buf_t *buf_create_copy(be_buf_t *buf)
{
	return buf_copy(buf_create_node(""), buf);
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
		SET_BUF_STATE(dest, buf_MODE, buf_MODE_VIEW);	// A copy must be not changeable
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
		progerr_printf("This buffer is NULL\n");
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
int buf_count_lines(be_buf_t *buf)
{
	int count = 0;

	for (be_line_t *line = NODES_TOP_NODE(buf); IS_NODE_INT(line);
	 line = NODE_NEXT(line), count++) {
		// NOTHING_TO_DO
	}
	return count;
}

int buf_is_orig_file_updated(be_buf_t *buf)
{
	struct stat st;

	if (stat(buf->file_path_, &st)) {
		return -1;
	}
	return st.st_mtime > buf->orig_file_stat.st_mtime;
}
//-----------------------------------------------------------------------------
const char *buf_mode_str(be_buf_t *buf)
{
	switch (BUF_STATE(buf, buf_MODE)) {
	default:
	case buf_MODE_EDIT:		return "[EDIT]";
	case buf_MODE_VIEW:		return "[VIEW]";
	case buf_MODE_LIST:		return "[LIST]";
	case buf_MODE_ANCH:		return "[ANCH]";
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
//-----------------------------------------------------------------------------
be_line_t *buf_set_cur_line(be_buf_t *buf, be_line_t *line)
{
	return BUFV0_CL(buf) = BUFV1_CL(buf) = line;
}
be_line_t *buf_cur_line(be_buf_t *buf)
{
	return BUFV0_CL(buf);
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

//-----------------------------------------------------------------------------

PRIVATE unsigned short file_crc;
PRIVATE long file_size;

void buf_update_crc(be_buf_t *buf)
{
	buf->orig_file_crc = buf_calc_crc(buf);
}
int buf_check_crc(be_buf_t *buf)
{
	return buf->orig_file_crc != buf_calc_crc(buf);
}
unsigned short buf_calc_crc(be_buf_t *buf)
{
	file_size = 0;
	clear_crc16ccitt();
	for (be_line_t *line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
		file_size += line_strlen(line) + 1;
		for (char *ptr = line->data; ; ptr++) {
			file_crc = calc_crc16ccitt(*ptr);
			if (*ptr == 0)	// count including last NUL code
				break;
		}
	}
	return file_crc;
}

int buf_count_bufs(be_buf_t *buf)
{
	buf = buf_make_top_buf(buf);
	int cnt;
	for (cnt = 0; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		cnt++;
	}
	return cnt;
}

be_buf_t *buf_make_buf_intermediate(be_buf_t *buf)
{
	if (IS_PTR_VALID(buf) && (IS_NODE_INT(buf) == 0)) {
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

//-----------------------------------------------------------------------------
be_bufs_t *bufs_init(be_bufs_t *bufs, const char* name,
 const char* name_top, const char* name_bottom)
{
	bufs->prev = bufs->next = NULL;
	strlcpy__(bufs->name, name, MAX_NAME_LEN);
	bufs_init_anchors(bufs, name_top, name_bottom);
	bufs->cur_buf = NODES_BOT_ANCH(bufs);
	return bufs;
}
void bufs_init_anchors(be_bufs_t *bufs, const char *full_path_top, const char *full_path_bot)
{
	buf_init(NODES_TOP_ANCH(bufs), full_path_top);
	SET_BUF_STATE(NODES_TOP_ANCH(bufs), buf_MODE, buf_MODE_ANCH);
	buf_init(NODES_BOT_ANCH(bufs), full_path_bot);
	SET_BUF_STATE(NODES_BOT_ANCH(bufs), buf_MODE, buf_MODE_ANCH);
	buf_link(NODES_TOP_ANCH(bufs), NODES_BOT_ANCH(bufs));
}
void bufs_link(be_bufs_t *top_anchor, be_bufs_t *bot_anchor)
{
	top_anchor->next = bot_anchor;
	bot_anchor->prev = top_anchor;
}
void bufs_insert_buf_to_bottom(be_bufs_t *bufs, be_buf_t *buf)
{
	buf_insert_before(NODES_BOT_ANCH(bufs), buf);
	bufs->cur_buf = buf;
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
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		for (be_buf_t *buf = NODES_TOP_ANCH(bufs); IS_PTR_VALID(buf); ) {
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
	bufs = bufs_make_top_anchor(bufs);
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		for (be_buf_t *buf = NODES_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
			if (buf == cur_buf) {
				return bufs;
			}
		}
	}
	return NULL;
}
void bufs_fix_cur_buf(be_bufs_t *bufs)
{
	be_bufs_t *buffs = bufs_get_bufs_contains_buf(bufs, bufs->cur_buf);
	if (IS_PTR_NULL(buffs) || (IS_NODE_INT(buffs) == 0)) {
		// 'bufs->cur_buf' is not valid
		bufs->cur_buf = NODES_TOP_NODE(bufs);	// fix
	}
}

int bufs_count_bufs(be_bufs_t *bufs)
{
	return buf_count_bufs(NODES_TOP_NODE(bufs));
}

//-----------------------------------------------------------------------------

be_buf_t *buf_get_buf_by_idx(be_buf_t *buf, int buf_idx)
{
	// making sure that bufs is TOP_BUF
	buf = buf_make_top_buf(buf);
	for ( ; buf_idx > 0 && IS_NODE_INT(buf); buf = NODE_NEXT(buf), buf_idx--) {
		// NOTHING_TO_DO
	}
	return buf;	// buf may(possibly) be top/bottom anchor
}

PRIVATE int buf_get_buf_idx_from_top_anch(be_buf_t *buff, be_buf_t *buf);
// top-anchor: 0, buf-1: 1, buf-2: 2, ..., buf-n: n, bottom-anchor: (n+1)
int buf_get_buf_idx(be_buf_t *buf)
{
/////_D_(buf_dump_name(buf))
	return buf_get_buf_idx_from_top_anch(buf_make_top_anchor(buf), buf);
}
int buf_get_buf_idx_in_bufs(be_bufs_t *bufs, be_buf_t *buf)
{
/////_D_(buf_dump_name(buf))
	return buf_get_buf_idx_from_top_anch(NODES_TOP_ANCH(bufs), buf);
}
PRIVATE int buf_get_buf_idx_from_top_anch(be_buf_t *buff, be_buf_t *buf)
{
/////_D_(buf_dump_name(buff))
/////_D_(buf_dump_name(buf))
/////_D_(buf_dump_name(buff))
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
		if (strcmp(buf->file_path_, file_path) == 0) {
			return buf;	// found
		}
		char abs_path[MAX_PATH_LEN+1];
		get_abs_path(file_path, abs_path);
		if (strcmp(buf->abs_path_, abs_path) == 0) {
			return buf;	// found
		}
	}
	return NULL;		// not found
}
be_buf_t *buf_get_buf_by_file_name(be_buf_t *buf, const char *file_name)
{
	buf = buf_make_top_buf(buf);
	for ( ; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		if (compare_file_path_from_tail(buf->file_path_, file_name) == 0) {
			return buf;	// found by file_path
		}
		if (compare_file_path_from_tail(buf->abs_path_, file_name) == 0) {
			return buf;	// found by abs_path
		}
	}
	return NULL;		// not found
}

//-----------------------------------------------------------------------------
void bufs_renumber_all_bufs_from_top(be_bufs_t *bufs)
{
	for (be_buf_t *buf = NODES_TOP_NODE(bufs); IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		buf_renumber_from_top(buf);
	}
}

//-----------------------------------------------------------------------------
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

// top-anchor: 0, bufs-1: 1, bufs-2: 2, ..., bufs-n: n, bottom-anchor: (n+1)
int bufs_get_bufs_idx_in_bufss(be_bufs_t *bufs, be_buf_t *buf)
{
/////_D_(buf_dump_name(buf))
	bufs = bufs_make_top_anchor(bufs);
	for (int bufs_idx = 0; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs), bufs_idx++) {
/////_D_(bufs_dump_name(bufs))
		if (buf_get_buf_idx_in_bufs(bufs, buf) >= 0) {
/////_D_(bufs_dump_name(bufs))
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
	if (IS_NODE_TOP_ANCH(buf)) {
		buf = NODE_NEXT(buf);
	}
	for ( ; IS_PTR_VALID(buf) && (IS_NODE_TOP_MOST(buf) == 0); buf = NODE_PREV(buf)) {
		// NOTHING_TO_DO
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
///PRIVATE be_bufs_t *bufs_make_top_bufs(be_bufs_t *bufs)
///{
///	if (IS_NODE_TOP_ANCH(bufs)) {
///		bufs = NODE_NEXT(bufs);
///	}
///	for ( ; IS_PTR_VALID(bufs) && IS_NODE_TOP_MOST(bufs) == 0; bufs = NODE_PREV(bufs)) {
///		// NOTHING_TO_DO
///	}
///	return bufs;
///}

//-----------------------------------------------------------------------------
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
///		buf_dump_ptrs(buf);
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
		if (buf_count_lines(buf)) {
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
///	flf_d_printf("file_path: [%s]\n", buf->file_path_);
	flf_d_printf("abs_path_: [%s]\n", buf->abs_path_);
}

void bufs_dump_all_bufs(be_bufs_t *bufs)
{
	flf_d_printf("00============================================\n");
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		flf_d_printf("bufs: [%s]\n", bufs->name);
		for (be_buf_t *buf = NODES_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
			flf_d_printf(" %cbuf: [%s]\n", (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path_);
			flf_d_printf("    buf->v0_str: [%s]\n", buf->buf_views[0].cur_line->data);
		///	flf_d_printf("    buf->v1_str: [%s]\n", buf->buf_views[1].cur_line->data);
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
