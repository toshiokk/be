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

// be_buf_t manipulation routines ==========================================
// (common to edit-buffer, cut-buffer, undo-redo-buffer and history)

// Create a new be_buf_t
be_buf_t *buf_create(const char *full_path)
{
	be_buf_t *buf;

	buf = (be_buf_t *)malloc__(sizeof(be_buf_t));
///	get_mem_free_in_kb(1);
	return buf_init(buf, full_path);
}
// initialize be_buf_t members
be_buf_t *buf_init(be_buf_t *buf, const char *full_path)
{
	memset(buf, 0x00, sizeof(*buf));

	buf_clear_link(buf);
	buf_set_file_path(buf, full_path);
	buf->orig_file_stat.st_uid = geteuid();
	buf->orig_file_stat.st_gid = getegid();
	buf->orig_file_stat.st_mode = RW0RW0R00;		// regular file rw-rw-r--(664)
	buf->orig_file_stat.st_mtime = time(NULL);		// time file was created
	buf->orig_file_crc = 0;

	buf_init_line_anchors(buf);

	buf_view_init(&(buf->buf_views[0]), buf);
	buf_view_init(&(buf->buf_views[1]), buf);

	buf->mark_line = BUF_TOP_ANCH(buf);
	buf->mark_line_byte_idx = 0;
	buf->buf_lines = 0;
	buf->buf_size = 0;
	return buf;
}
void buf_view_init(be_buf_view_t *b_v, be_buf_t *buf)
{
	BUF_V_CL(b_v) = BUF_TOP_ANCH(buf);
	BUF_V_CLBI(b_v) = 0;
	BUF_V_CURSOR_Y(b_v) = 0;
	BUF_V_CURSOR_X_TO_KEEP(b_v) = 0;
	BUF_V_MIN_TEXT_X_TO_KEEP(b_v) = 0;
}
be_buf_t *buf_init_line_anchors(be_buf_t *buf)
{
	line_init(&buf->top_anchor, "#LINE-top-anchor");
	line_init(&buf->bot_anchor, "#LINE-bot-anchor");
	line_link(&buf->top_anchor, &buf->bot_anchor);
	return buf;
}
void buf_set_file_path(be_buf_t *buf, const char *file_path)
{
	strlcpy__(buf->file_path, file_path, MAX_PATH_LEN);
	if (IS_NODE_TOP_ANCH(buf)) {
		strlcpy__(buf->abs_path, file_path, MAX_PATH_LEN);
		return;
	}
	get_abs_path(file_path, buf->abs_path);
}
void buf_get_file_path(be_buf_t *buf, char *file_path)
{
	strlcpy__(file_path, buf->file_path, MAX_PATH_LEN);
}

// Splice a buffer into an existing be_buf_t
be_buf_t *buf_insert(be_buf_t *buf, be_buf_t *new_buf,
 insert_before_after_t before_after)
{
	if (before_after == INSERT_BEFORE) {
		buf_insert_before(buf, new_buf);
	} else {
		buf_insert_after(buf, new_buf);
	}
	return new_buf;
}
be_buf_t *buf_insert_before(be_buf_t *buf, be_buf_t *new_buf)
{
	return buf_insert_between(PREV_NODE(buf), new_buf, buf);
}
be_buf_t *buf_insert_after(be_buf_t *buf, be_buf_t *new_buf)
{
	return buf_insert_between(buf, new_buf, NEXT_NODE(buf));
}
be_buf_t *buf_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next)
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

be_buf_t *buf_create_copy(be_buf_t *src)
{
	return buf_copy(buf_create(""), src);
}
be_buf_t *buf_copy(be_buf_t *dest, be_buf_t *src)
{
	memcpy__(dest, src, sizeof(*src));
	if (dest->prev != NULL && dest->next != NULL) {
		// buffer is linked.
		// clear link and anchors to avoid double linking
		buf_clear_link(dest);
		buf_init_line_anchors(dest);
	}
	BUF_V0_CL(dest) = BUF_V1_CL(dest) = &(dest->top_anchor);
	dest->mark_line = &(dest->top_anchor);
	return dest;
}

be_buf_t *buf_unlink_free(be_buf_t *buf)
{
	be_buf_t *prev_buf;

///_D_(buf_dump_state(buf));
	if (IS_NODE_VALID(buf) == 0) {
		progerr_printf("CANNOT free this buffer %s\n", buf->file_path);
		return NULL;
	}
	prev_buf = buf->prev;
	buf_unlink(buf);
	buf_free(buf);
	return prev_buf;
}

// Unlink a line from the rest of the be_buf_t
be_buf_t *buf_unlink(be_buf_t *buf)
{
	buf_avoid_wild_ptr_cur(buf);
	if (buf->prev) {
		buf->prev->next = NEXT_NODE(buf);
	}
	if (NEXT_NODE(buf)) {
		buf->next->prev = PREV_NODE(buf);
	}
	buf_clear_link(buf);
	return buf;
}
void buf_clear_link(be_buf_t *buf)
{
	buf->prev = NULL;
	buf->next = NULL;
}

///be_buf_t *goto_top_buf(be_buf_t *buf)
///{
///	for ( ; IS_NODE_TOP_ANCH(buf) == 0; buf = PREV_NODE(buf)) {
///	}
///	return buf;
///}
///be_buf_t *goto_bottom_buf(be_buf_t *buf)
///{
///	for ( ; IS_NODE_BOT_ANCH(buf) == 0; buf = NEXT_NODE(buf)) {
///	}
///	return buf;
///}

// Free a buffer
void buf_free(be_buf_t *buf)
{
	buf_free_lines(buf);
	free__(buf);
///	get_mem_free_in_kb(1);
}
void buf_free_lines(be_buf_t *buf)
{
	be_line_t *line;

	// free all lines from top to bottom
	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0; ) {
		line = line_unlink_free(line);
	}
}

// compare two buffers
int buf_compare(be_buf_t *buf1, be_buf_t *buf2)
{
	be_line_t *line1;
	be_line_t *line2;
	int diff;

	for (line1 = BUF_TOP_LINE(buf1), line2 = BUF_TOP_LINE(buf2);
	 IS_NODE_BOT_ANCH(line1) == 0 && IS_NODE_BOT_ANCH(line2) == 0; 
	 line1 = NEXT_NODE(line1), line2 = NEXT_NODE(line2)) {
		diff = strncmp(line1->data, line2->data, MAX_EDIT_LINE_LEN);
		if (diff)
			return diff;
	}
	return IS_NODE_BOT_ANCH(line1) - IS_NODE_BOT_ANCH(line2);	// 0: exactly the same
}

int buf_renumber_from_top(be_buf_t *buf)
{
	return buf->buf_lines = buf_renumber_from_line(buf, NODES_TOP_NODE(buf));
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
	be_line_t *line;

	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0; line = NEXT_NODE(line)) {
		if (line_data_len(line) > 4) {
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

int buf_count_bufs(be_buf_t *buf)
{
	int count;

	for (count = 0; IS_NODE_BOT_ANCH(buf) == 0; count++) {
		buf = NEXT_NODE(buf);
	}
	return count;
}
int buf_count_lines(be_buf_t *buf)
{
	return line_count_lines(BUF_TOP_LINE(buf));
}

int buf_is_orig_file_updated(be_buf_t *buf)
{
	struct stat st;

	if (stat(buf->abs_path, &st)) {
		return -1;
	}
	return st.st_mtime > buf->orig_file_stat.st_mtime;
}
//-----------------------------------------------------------------------------
const char *buf_eol_str(be_buf_t *buf)
{
	switch(BUF_STATE(buf, buf_EOL)) {
	default:
	case EOL_NIX:
		return "LF(NIX)";		/*"NIX"*/;
	case EOL_MAC:
		return "CR(MAC)";		/*"MAC"*/
	case EOL_DOS:
		return "CR+LF(DOS)";	/*"DOS"*/
	}
}
const char *buf_encode_str(be_buf_t *buf)
{
#ifdef USE_NKF
	switch(BUF_STATE(buf, buf_ENCODE)) {
	default:
	case ENCODE_ASCII:		return "ASCII";
	case ENCODE_UTF8:		return "UTF8";
	case ENCODE_EUCJP:		return "EUCJP";
	case ENCODE_SJIS:		return "SJIS";
	case ENCODE_JIS:		return "JIS";
	case ENCODE_BINARY:		return "BIN";
	}
#else // USE_NKF
	return "ASCII";
#endif // USE_NKF
}
const char *buf_cut_mode_str(be_buf_t *buf)
{
	char *ptr;

	switch(BUF_STATE(buf, buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:	ptr = "  ";			break;
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
	return BUF_V0_CL(buf) = line;
}
be_line_t *buf_cur_line(be_buf_t *buf)
{
	return BUF_V0_CL(buf);
}
be_line_t *buf_move_cur_line_to_prev(be_buf_t *buf)
{
	be_line_t *line = BUF_V0_CL(buf);
	if (IS_NODE_TOP_ANCH(line) == 0) {
		BUF_V0_CL(buf) = line->prev;
		return line;
	}
	// do not move and return NULL
	return NULL;
}
be_line_t *buf_move_cur_line_to_next(be_buf_t *buf)
{
	be_line_t *line;

	if (IS_NODE_BOT_ANCH(BUF_V0_CL(buf)) == 0) {
		line = BUF_V0_CL(buf);
		BUF_V0_CL(buf) = BUF_V0_CL(buf)->next;
		return line;
	}
	// do not move and return NULL
	return NULL;
}

be_line_t *buf_get_line_ptr_from_line_num(be_buf_t *buf, int line_num)
{
	be_line_t *line;

	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0 && line_num > 1; line_num--) {
		if (IS_NODE_BOT(line))
			break;
		line = NEXT_NODE(line);
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
	be_line_t *line;
	char *ptr;

	file_size = 0;
	clear_crc16ccitt();
	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0; line = NEXT_NODE(line)) {
		file_size += line_data_len(line) + 1;
		for (ptr = line->data; ; ptr++) {
			file_crc = calc_crc16ccitt(*ptr);
			if (*ptr == 0)	// count in last NUL code
				break;
		}
	}
flf_d_printf("crc: %04lx, size: %d\n", file_crc, file_size);
	return file_crc;
}
//-----------------------------------------------------------------------------

be_bufs_t *bufs_init(be_bufs_t *bufs, const char* buf_name)
{
	bufs->prev = bufs->next = NULL;
	strlcpy__(bufs->name, buf_name, MAX_NAME_LEN);
	init_bufs_top_bot_anchor(
	 &(bufs->top_anchor), "#BUF-top-anchor",
	 &(bufs->bot_anchor), "#BUF-bot-anchor");
	bufs->cur_buf = &(bufs->bot_anchor);
	return bufs;
}
be_bufs_t *bufs_link(be_bufs_t *top_anchor, be_bufs_t *bot_anchor)
{
	top_anchor->next = bot_anchor;
	return bot_anchor->prev = top_anchor;
}
be_bufs_t *bufs_insert_before(be_bufs_t *bufs, be_bufs_t *other)
{
	return bufs_insert_between(bufs->prev, other, bufs);
}
be_bufs_t *bufs_insert_between(be_bufs_t *prev, be_bufs_t *mid, be_bufs_t *next)
{
	bufs_link(prev, mid);
	return bufs_link(mid, next);
}

be_bufs_t *bufs_free_all_bufss(be_bufs_t *bufs)
{
	flf_d_printf("bufs: %s\n", bufs->name);
	for ( ; IS_PTR_VALID(bufs); bufs = NEXT_NODE(bufs)) {
		flf_d_printf("bufs: %s\n", bufs->name);
		for ( ; ; ) {
			be_buf_t *buf = bufs->top_anchor.next;
			flf_d_printf(" %cbuf:[%s]\n",
			 (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path);
			if (IS_NODE_VALID(buf) == 0) {
				break;
			}
			flf_d_printf("    unlink_free %cbuf:[%s]\n",
			 (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path);
			if (bufs->cur_buf == buf) {
				bufs->cur_buf = buf->next;
			}
			buf_unlink_free(buf);
		}
	}
	return bufs;
}

be_bufs_t *get_bufs_contains_buf(be_bufs_t *bufs, be_buf_t *cur_buf)
{
	for ( ; IS_PTR_VALID(bufs); bufs = NEXT_NODE(bufs)) {
		for (be_buf_t *buf = &(bufs->top_anchor); IS_PTR_VALID(buf); buf = NEXT_NODE(buf)) {
			if (buf == cur_buf) {
				return bufs;
			}
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------

void init_bufs_top_bot_anchor(
 be_buf_t *buf_top, const char *full_path_top,
 be_buf_t *buf_bot, const char *full_path_bot)
{
	buf_init(buf_top, full_path_top);
	buf_init(buf_bot, full_path_bot);
	buf_link(buf_top, buf_bot);
}

PRIVATE be_buf_t *make_sure_buf_is_top_buf(be_buf_t *bufs)
{
	if (IS_NODE_TOP_ANCH(bufs)) {
		bufs = NEXT_NODE(bufs);
	}
	while (IS_NODE_TOP(bufs) == 0) {
		bufs = PREV_NODE(bufs);
	}
	return bufs;
}

be_buf_t *get_buf_from_bufs_by_idx(be_buf_t *bufs, int buf_idx)
{
	// making sure that bufs is TOP_BUF
	bufs = make_sure_buf_is_top_buf(bufs);
	for ( ; IS_NODE_BOT_ANCH(bufs) == 0 && buf_idx > 0; buf_idx--) {
		bufs = NEXT_NODE(bufs);
	}
	return bufs;	// bufs may be top/bottom anchor
}
int get_buf_idx_in_bufs(be_buf_t *bufs, be_buf_t *buf)
{
	bufs = make_sure_buf_is_top_buf(bufs);
	for (int buf_idx = 0; IS_NODE_BOT_ANCH(bufs) == 0; buf_idx++, bufs = NEXT_NODE(bufs)) {
		if (bufs == buf)
			return buf_idx;	// found
	}
	return -1;	// not found
}
be_buf_t *get_buf_from_bufs_by_abs_path(be_buf_t *bufs, const char *abs_path)
{
	bufs = make_sure_buf_is_top_buf(bufs);
	for ( ; IS_NODE_BOT_ANCH(bufs) == 0; bufs = NEXT_NODE(bufs)) {
		if (strcmp(bufs->abs_path, abs_path) == 0) {
			return bufs;	// found
		}
	}
	return NULL;		// not found
}

//-----------------------------------------------------------------------------
void renumber_all_bufs_from_top(be_bufs_t *bufs)
{
	for (be_buf_t *buf = BUFS_TOP_BUF(bufs); IS_NODE_BOT_ANCH(buf) == 0; buf = NEXT_NODE(buf)) {
		buf_renumber_from_top(buf);
	}
}

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void buf_dump_bufs(be_buf_t *buf)
{
	int cnt;

flf_d_printf("0============================================\n");
	for (cnt = 0; cnt < 100 && IS_PTR_VALID(buf); cnt++, buf = NEXT_NODE(buf)) {
		buf_dump_ptrs(buf);
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
flf_d_printf("9============================================\n");
}
void buf_dump_bufs_lines(be_buf_t *buf, const char *label)
{
	int cnt;

flf_d_printf("%s {{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n", label);
	for (cnt = 0; cnt < 100 && IS_PTR_VALID(buf); cnt++, buf = NEXT_NODE(buf)) {
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
	buf_dump_state(buf);
	if (buf == NULL) {
		return;
	}
	line_dump_lines(BUF_TOP_ANCH(buf), lines, BUF_V0_CL(buf));
}
void buf_dump_ptrs(be_buf_t *buf)
{
	flf_d_printf("%saddr:%08lx,prev:%08lx,next:%08lx,line:%08lx\n",
	 buf == get_c_e_b() ? ">" : " ",
	 buf, buf->prev, buf->next, NODES_TOP_NODE(buf));
	line_dump_lines(BUF_TOP_ANCH(buf), 3, BUF_V0_CL(buf));
}
void buf_dump_state(be_buf_t *buf)
{
	if (buf == NULL) {
		return;
	}
flf_d_printf("file_path: [%s], abs_path: [%s]\n", buf->file_path, buf->abs_path);
///	buf_dump_ptrs(buf);
///flf_d_printf("byte_idx: %d\n", buf->cur_line_byte_idx);
///flf_d_printf("buf->cursor_y: %d\n", buf->cursor_y);
///flf_d_printf("cursor_x_to_keep: %d\n", buf->cursor_x_to_keep);
///flf_d_printf("buf_lines: %d\n", buf->buf_lines);
///flf_d_printf("buf_size: %d\n", buf->buf_size);
///flf_d_printf("mark_line_byte_idx: %d\n", buf->mark_line_byte_idx);
///flf_d_printf("enc: %s, eol: %s\n", buf_encode_str(buf), buf_eol_str(buf));
}
be_line_t *buf_check_line_in_buf(be_buf_t *buf, be_line_t *line_)
{
	be_line_t *line;

	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0; line = NEXT_NODE(line)) {
		if (line == line_)
			return line_;
	}
	return NULL;
}

void bufs_dump_all_bufs(be_bufs_t *bufs)
{
flf_d_printf("00============================================\n");
	for ( ; IS_PTR_VALID(bufs); bufs = NEXT_NODE(bufs)) {
		flf_d_printf("bufs: %s\n", bufs->name);
		for (be_buf_t *buf = &(bufs->top_anchor); IS_PTR_VALID(buf); buf = NEXT_NODE(buf)) {
			flf_d_printf(" %cbuf: %s\n", (bufs->cur_buf == buf) ? '>' : ' ', buf->file_path);
			flf_d_printf("   buf->v0_str: %s\n", buf->buf_views[0].cur_line->data);
			flf_d_printf("   buf->v1_str: %s\n", buf->buf_views[1].cur_line->data);
		}
	}
flf_d_printf("99============================================\n");
}

#endif // ENABLE_DEBUG

// End of buffer.c
