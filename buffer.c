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
be_buf_t *buffer_create(const char *full_path)
{
	be_buf_t *buf;

	buf = (be_buf_t *)malloc__(sizeof(be_buf_t));
///	get_mem_free_in_kb(1);
	return buffer_init(buf, full_path);
}
// initialize be_buf_t members
be_buf_t *buffer_init(be_buf_t *buf, const char *full_path)
{
	memset(buf, 0x00, sizeof(*buf));

	buffer_clear_link(buf);
	buffer_set_file_path(buf, full_path);
	buf->orig_file_stat.st_uid = geteuid();
	buf->orig_file_stat.st_gid = getegid();
	buf->orig_file_stat.st_mode = RW0RW0R00;		// regular file rw-rw-r--(664)
	buf->orig_file_stat.st_mtime = time(NULL);		// time file was created
	buf->orig_file_crc = 0;

	buffer_init_line_anchors(buf);

	BUFV0_CL(buf) = BUF_TOP_ANCH(buf);
	BUFV0_CLBI(buf) = 0;
	BUFV0_CURSOR_Y(buf) = 0;
	BUFV0_CURSOR_X_TO_KEEP(buf) = 0;
	BUFV0_MIN_TEXT_X_TO_BE_DISP(buf) = 0;

	BUFV1_CL(buf) = BUF_TOP_ANCH(buf);
	BUFV1_CLBI(buf) = 0;
	BUFV1_CURSOR_Y(buf) = 0;
	BUFV1_CURSOR_X_TO_KEEP(buf) = 0;
	BUFV1_MIN_TEXT_X_TO_BE_DISP(buf) = 0;

	buf->mark_line = BUF_TOP_ANCH(buf);
	buf->mark_line_byte_idx = 0;
	buf->buf_lines = 0;
	buf->buf_size = 0;
	return buf;
}
be_buf_t *buffer_init_line_anchors(be_buf_t *buf)
{
	line_init(&buf->top_anchor, "#line_top_anchor");
	line_init(&buf->bot_anchor, "#line_bot_anchor");
	line_link(&buf->top_anchor, &buf->bot_anchor);
	return buf;
}
void buffer_set_file_path(be_buf_t *buf, const char *file_path)
{
	strlcpy__(buf->file_path, file_path, MAX_PATH_LEN);
	if (IS_NODE_TOP_ANCH(buf)) {
		strlcpy__(buf->abs_path, file_path, MAX_PATH_LEN);
		return;
	}
	get_abs_path(file_path, buf->abs_path);
}
void buffer_get_file_path(be_buf_t *buf, char *file_path)
{
	strlcpy__(file_path, buf->file_path, MAX_PATH_LEN);
}

// Splice a buffer into an existing be_buf_t
be_buf_t *buffer_insert(be_buf_t *buf, be_buf_t *new_buf,
 insert_before_after_t before_after)
{
	if (before_after == INSERT_BEFORE) {
		buffer_insert_before(buf, new_buf);
	} else {
		buffer_insert_after(buf, new_buf);
	}
	return new_buf;
}
be_buf_t *buffer_insert_before(be_buf_t *buf, be_buf_t *new_buf)
{
	return buffer_insert_between(buf->prev, new_buf, buf);
}
be_buf_t *buffer_insert_after(be_buf_t *buf, be_buf_t *new_buf)
{
	return buffer_insert_between(buf, new_buf, buf->next);
}
be_buf_t *buffer_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next)
{
	buffer_link(prev, new_buf);
	buffer_link(new_buf, next);
	return new_buf;
}
// Connect two buffers
be_buf_t *buffer_link(be_buf_t *prev, be_buf_t *next)
{
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	return prev;
}

be_buf_t *buffer_create_copy(be_buf_t *src)
{
	return buffer_copy(buffer_create(""), src);
}
be_buf_t *buffer_copy(be_buf_t *dest, be_buf_t *src)
{
	memcpy__(dest, src, sizeof(*src));
	if (dest->prev != NULL && dest->next != NULL) {
		// buffer is linked.
		// clear link and anchors to avoid double linking
		buffer_clear_link(dest);
		buffer_init_line_anchors(dest);
	}
	BUFV0_CL(dest) = BUFV1_CL(dest) = NULL;
	dest->mark_line = NULL;
	return dest;
}

be_buf_t *buffer_unlink_free(be_buf_t *buf)
{
	be_buf_t *prev_buf;

///_D_(buffer_dump_state(buf));
	if (IS_NODE_VALID(buf) == 0) {
		progerr_printf("CANNOT free this buffer %s\n", buf->file_path);
		return NULL;
	}
	prev_buf = buf->prev;
	buffer_unlink(buf);
	buffer_free(buf);
	return prev_buf;
}

// Unlink a line from the rest of the be_buf_t
be_buf_t *buffer_unlink(be_buf_t *buf)
{
	buffer_avoid_wild_ptr_cur(buf);
	if (buf->prev) {
		buf->prev->next = buf->next;
	}
	if (buf->next) {
		buf->next->prev = buf->prev;
	}
	buffer_clear_link(buf);
	return buf;
}
void buffer_clear_link(be_buf_t *buf)
{
	buf->prev = NULL;
	buf->next = NULL;
}

// Free a buffer
void buffer_free(be_buf_t *buf)
{
	buffer_free_lines(buf);
	free__(buf);
///	get_mem_free_in_kb(1);
}
void buffer_free_lines(be_buf_t *buf)
{
	be_line_t *line;

	// free all lines from top to bottom
	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0; ) {
		line = line_unlink_free(line);
	}
}

// compare two buffers
int buffer_compare(be_buf_t *buf1, be_buf_t *buf2)
{
	be_line_t *line1;
	be_line_t *line2;
	int diff;

	for (line1 = BUF_TOP_LINE(buf1), line2 = BUF_TOP_LINE(buf2);
	 IS_NODE_BOT_ANCH(line1) == 0 && IS_NODE_BOT_ANCH(line2) == 0; 
	 line1 = line1->next, line2 = line2->next) {
		diff = strncmp(line1->data, line2->data, MAX_EDIT_LINE_LEN);
		if (diff)
			return diff;
	}
	return IS_NODE_BOT_ANCH(line1) - IS_NODE_BOT_ANCH(line2);	// 0: exactly the same
}

int buffer_renumber_from_line(be_buf_t *buf, be_line_t *line)
{
	return buf->buf_lines = line_renumber(line, &buf->buf_size);
}

int buffer_guess_tab_size(be_buf_t *buf)
{
#define LINES_TO_GUESS_TAB_SIZE		1000
	int lines_checked = 0;
	int lines_space4 = 0;
	be_line_t *line;

	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0;
	 line = line->next) {
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

int buffer_count_bufs(be_buf_t *buf)
{
	int count;

	for (count = 0; IS_NODE_BOT_ANCH(buf) == 0; count++) {
		buf = buf->next;
	}
	return count;
}
int buffer_count_lines(be_buf_t *buf)
{
	return line_count_lines(BUF_TOP_LINE(buf));
}

int buffer_is_orig_file_updated(be_buf_t *buf)
{
	struct stat st;

	if (stat(buf->abs_path, &st)) {
		return -1;
	}
	return st.st_mtime > buf->orig_file_stat.st_mtime;
}
//-----------------------------------------------------------------------------
const char *buffer_eol_str(be_buf_t *buf)
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
const char *buffer_encode_str(be_buf_t *buf)
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
const char *buffer_cut_mode_str(be_buf_t *buf)
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
be_line_t *buffer_set_cur_line(be_buf_t *buf, be_line_t *line)
{
	return BUFV0_CL(buf) = line;
}
be_line_t *buffer_cur_line(be_buf_t *buf)
{
	return BUFV0_CL(buf);
}
be_line_t *buffer_get_cur_line__move_prev(be_buf_t *buf)
{
	be_line_t *line;

	if (IS_NODE_TOP_ANCH(BUFV0_CL(buf)) == 0) {
		line = BUFV0_CL(buf);
		BUFV0_CL(buf) = BUFV0_CL(buf)->prev;
		return line;
	}
	// do not move and return NULL
	return NULL;
}
be_line_t *buffer_get_cur_line__move_next(be_buf_t *buf)
{
	be_line_t *line;

	if (IS_NODE_BOT_ANCH(BUFV0_CL(buf)) == 0) {
		line = BUFV0_CL(buf);
		BUFV0_CL(buf) = BUFV0_CL(buf)->next;
		return line;
	}
	// do not move and return NULL
	return NULL;
}

be_line_t *buffer_get_line_ptr_from_line_num(be_buf_t *buf, int line_num)
{
	be_line_t *line;

	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0 && line_num > 1;
	 line_num--) {
		if (IS_NODE_BOT(line))
			break;
		line = line->next;
	}
	return line;
}

//-----------------------------------------------------------------------------

PRIVATE unsigned short file_crc;
PRIVATE long file_size;

void buffer_update_crc(be_buf_t *buf)
{
	buf->orig_file_crc = buffer_calc_crc(buf);
}
int buffer_check_crc(be_buf_t *buf)
{
	return buf->orig_file_crc != buffer_calc_crc(buf);
}
unsigned short buffer_calc_crc(be_buf_t *buf)
{
	be_line_t *line;
	char *ptr;

	file_size = 0;
	clear_crc16ccitt();
	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0;
	 line = line->next) {
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
#ifdef ENABLE_DEBUG
void buffer_dump_bufs(be_buf_t *buf)
{
	int cnt;

flf_d_printf("0============================================\n");
	for (cnt = 0; cnt < 100 && buf != NULL; cnt++, buf = buf->next) {
		buffer_dump_ptrs(buf);
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
flf_d_printf("9============================================\n");
}
void buffer_dump_bufs_lines(be_buf_t *buf, const char *label)
{
	int cnt;

flf_d_printf("%s {{{{{{{{{{{{{{{{{{{{{{{{{{{{{\n", label);
	for (cnt = 0; cnt < 100 && buf != NULL; cnt++, buf = buf->next) {
		if (buffer_count_lines(buf)) {
			buffer_dump_lines(buf, 3);
		}
		if (IS_NODE_BOT_ANCH(buf))
			break;
	}
flf_d_printf("%s }}}}}}}}}}}}}}}}}}}}}}}}}}}}}\n", label);
}
void buffer_dump_lines(be_buf_t *buf, int lines)
{
	buffer_dump_state(buf);
	if (buf == NULL) {
		return;
	}
	line_dump_lines(BUF_TOP_ANCH(buf), lines, BUFV0_CL(buf));
}
void buffer_dump_ptrs(be_buf_t *buf)
{
	flf_d_printf("%saddr:%08lx,prev:%08lx,next:%08lx,line:%08lx\n",
	 buf == get_c_e_b() ? ">" : " ",
	 buf, buf->prev, buf->next, buf->top_anchor.next);
	line_dump_lines(BUF_TOP_ANCH(buf), 3, BUFV0_CL(buf));
}
void buffer_dump_state(be_buf_t *buf)
{
	if (buf == NULL) {
		return;
	}
flf_d_printf("file_path: [%s], abs_path: [%s]\n", buf->file_path, buf->abs_path);
///	buffer_dump_ptrs(buf);
///flf_d_printf("byte_idx: %d\n", buf->cur_line_byte_idx);
///flf_d_printf("buf->cursor_y: %d\n", buf->cursor_y);
///flf_d_printf("cursor_x_to_keep: %d\n", buf->cursor_x_to_keep);
///flf_d_printf("buf_lines: %d\n", buf->buf_lines);
///flf_d_printf("buf_size: %d\n", buf->buf_size);
///flf_d_printf("mark_line_byte_idx: %d\n", buf->mark_line_byte_idx);
///flf_d_printf("enc: %s, eol: %s\n", buffer_encode_str(buf), buffer_eol_str(buf));
}
be_line_t *buffer_check_line_in_buf(be_buf_t *buf, be_line_t *line_)
{
	be_line_t *line;

	for (line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(line) == 0;
	 line = line->next) {
		if (line == line_)
			return line_;
	}
	return NULL;
}
#endif // ENABLE_DEBUG

// End of buffer.c
