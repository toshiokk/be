/**************************************************************************
 *   editorundo.c                                                         *
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

#ifdef ENABLE_UNDO

// Undo-buffers manipulation routines -----------------------------------------

void init_undo_bufs(void)
{
	init_bufs_top_bot_anchor(
	 UNDO_BUFS_TOP_ANCH, "#Undo-bufs-top_anchor",
	 UNDO_BUFS_BOT_ANCH, "#Undo-bufs-bot_anchor");

	init_bufs_top_bot_anchor(
	 REDO_BUFS_TOP_ANCH, "#Redo-bufs-top_anchor",
	 REDO_BUFS_BOT_ANCH, "#Redo-bufs-bot_anchor");
}
void free_all_undo_bufs(void)
{
	while (IS_NODE_BOT_ANCH(CUR_REDO_BUF) == 0) {
		buffer_free(pop_redo_buf());
	}
	while (IS_NODE_BOT_ANCH(CUR_UNDO_BUF) == 0) {
		buffer_free(pop_undo_buf());
	}
}

be_buf_t *push_undo_buf(be_buf_t *buf)
{
	buf = buffer_create_copy(buf);
	snprintf(buf->file_path, MAX_PATH_LEN, "undo-buf-%02d", count_undo_bufs());
	return buffer_insert_after(UNDO_BUFS_TOP_ANCH, buf);
}
be_buf_t *pop_undo_buf(void)
{
	if (IS_NODE_BOT_ANCH(CUR_UNDO_BUF))
		return NULL;
	return buffer_unlink(CUR_UNDO_BUF);
}
be_buf_t *push_redo_buf(be_buf_t *buf)
{
	buf = buffer_create_copy(buf);
	snprintf(buf->file_path, MAX_PATH_LEN, "redo-buf-%02d", count_redo_bufs());
	return buffer_insert_after(REDO_BUFS_TOP_ANCH, buf);
}
be_buf_t *pop_redo_buf(void)
{
	if (IS_NODE_BOT_ANCH(CUR_REDO_BUF))
		return NULL;
	return buffer_unlink(CUR_REDO_BUF);
}
int delete_undo_redo_buf(be_buf_t *edit_buf)
{
	// delete undo, redo buffers related to edit_buf
	return delete_do_buf(edit_buf, UNDO_BUFS_TOP_BUF)
	 + delete_do_buf(edit_buf, REDO_BUFS_TOP_BUF);
}
int delete_do_buf(be_buf_t *edit_buf, be_buf_t *do_buf)
{
	int deleted = 0;

	for ( ; IS_NODE_BOT_ANCH(do_buf) == 0; do_buf = do_buf->next) {
		if (strcmp(do_buf->abs_path, edit_buf->abs_path) == 0) {
			do_buf = buffer_unlink_free(do_buf);
			deleted++;
		}
	}
	return deleted;
}
int count_undo_bufs(void)
{
	return buffer_count_bufs(CUR_UNDO_BUF);
}
int count_redo_bufs(void)
{
	return buffer_count_bufs(CUR_REDO_BUF);
}

#ifdef ENABLE_DEBUG
PRIVATE be_buf_t *prev_c_e_b;
PRIVATE size_t prev_buf_size;
PRIVATE int prev_count_undo_bufs;
void memorize_undo_state_before_change(void)
{
	prev_c_e_b = get_c_e_b();
	prev_buf_size = get_c_e_b()->buf_size;
	prev_count_undo_bufs = count_undo_bufs();
}
void check_undo_state_after_change(void)
{
	if (get_c_e_b() != EDIT_BUFS_TOP_ANCH
	 && get_c_e_b() == prev_c_e_b && get_c_e_b()->buf_size != prev_buf_size
		// edit buffer has been modified
	 && count_undo_bufs() == prev_count_undo_bufs) {
		// but no undo info pushed
		// warn it by setting unusual application color
		set_work_space_color_low();
		disp_status_bar_err(_("!!!! No UNDO info pushed !!!!"));
		progerr_printf("No UNDO info pushed for %s\n", prev_func_id);
	}
}
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------
// aaaa				<== undo_min_line (line not-modified)
// BBBB				first line which will be modified
// CCCC				last line which will be modified
// dddd				<== undo_max_line (line not-modified)
PRIVATE be_line_t *undo_min_line = NULL;
PRIVATE be_line_t *undo_max_line = NULL;
PRIVATE int undo_lines = 0;
PRIVATE void save_region_to_undo_buf(void);
PRIVATE be_line_t *append_line_to_cur_undo_buf(be_line_t *line);

void undo_set_region_save_before_change(be_line_t *min_line, be_line_t *max_line,
 int cut_buf_lines)
{
	undo_set_region(min_line, max_line, cut_buf_lines);
	undo_save_before_change();
}
void undo_set_region(be_line_t *min_line, be_line_t *max_line, int cut_buf_lines)
{
	undo_min_line = min_line->prev;
	undo_max_line = max_line->next;
	undo_lines = cut_buf_lines;

	undo_adjust_max_line();
}
// save undo info before change
void undo_save_before_change(void)
{
///_FLF_
	save_region_to_undo_buf();
}
void undo_adjust_max_line(void)
{
	be_line_t *line;
	int lines;
	int past_max_line = 0;

	line = undo_min_line->next;
	for (lines = 0 ; IS_NODE_VALID(line); lines++) {
		if (line == undo_max_line) {
			past_max_line = 1;
		}
		if (lines >= undo_lines && past_max_line) {
			break;
		}
		line = line->next;
	}
	undo_max_line = line;	// adjust max line
}
// save undo info after change
void undo_save_after_change(void)
{
///_FLF_
	if (count_undo_bufs() % 2) {
		// count_undo_bufs() is odd.
		// The state before change was saved.
		// Save the state after change.
		save_region_to_undo_buf();	// save the state after change
		if (count_undo_bufs() >= 2) {
			// compare before and after
			if (buffer_compare(CUR_UNDO_BUF, CUR_UNDO_BUF->next) == 0) {
				// not changed, pop two buffer (after and before)
				buffer_free(pop_undo_buf());
				buffer_free(pop_undo_buf());
			}
		}
	}
}
PRIVATE void save_region_to_undo_buf(void)
{
	be_line_t *line;

	push_undo_buf(get_c_e_b());
	for (line = undo_min_line->next; line != undo_max_line; line = line->next) {
		append_line_to_cur_undo_buf(line_create_copy(line));
	}
}
PRIVATE be_line_t *append_line_to_cur_undo_buf(be_line_t *line)
{
	return line_insert(CUR_UNDO_BUF_BOT_ANCH, line, INSERT_BEFORE);
}

typedef enum /*undo0_redo1*/ {
	UNDO0,	// undo
	REDO1	// redo
} undo0_redo1_t;

PRIVATE int do_undo_redo_(undo0_redo1_t undo0_redo1);
PRIVATE be_line_t *restore_region_from_buffer(undo0_redo1_t undo0_redo1);
PRIVATE be_line_t *delete_region_in_buf(be_buf_t *buf);
PRIVATE be_line_t *insert_region_from_buf(be_line_t *edit_line, be_buf_t *buf);

int do_undo(void)
{
	do_clear_mark_();
	if (count_undo_bufs() < 2) {
		disp_status_bar_err(_("Undo-buffer empty !!"));
		return 0;
	}
	return do_undo_redo_(UNDO0);
}
int do_redo(void)
{
	do_clear_mark_();
	if (count_redo_bufs() < 2) {
		disp_status_bar_err(_("Redo-buffer empty !!"));
		return 0;
	}
	return do_undo_redo_(REDO1);
}
PRIVATE int do_undo_redo_(undo0_redo1_t undo0_redo1)
{
	be_line_t *top_line;

	set_cur_buf_modified();
	top_line = restore_region_from_buffer(undo0_redo1);
	post_cmd_processing(top_line, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

// modification:
//	[edit-buf(before)]	[edit-buf(after)]	[undo-stack]	[redo-stack]
//	aaaa				aaaa				BBBB
//	bbbb				BBBB				bbbb
//	cccc				cccc
//
// undo:
//	[edit-buf(before)]	[edit-buf(after)]	[undo-stack]	[redo-stack]
//	aaaa				aaaa								bbbb
//	BBBB				bbbb								BBBB
//	cccc				cccc
//
// redo:
//	[edit-buf(before)]	[edit-buf(after)]	[undo-stack]	[redo-stack]
//	aaaa				aaaa				BBBB
//	bbbb				BBBB				bbbb
//	cccc				cccc
//
PRIVATE be_line_t *restore_region_from_buffer(undo0_redo1_t undo0_redo1)
{
	be_buf_t *buf_after;
	be_buf_t *buf_before;
	be_line_t *edit_line;
	be_line_t *top_line;

	if (undo0_redo1 == UNDO0) {
		// undo (move undo information from undo-buf to redo-buf)
		buf_after = pop_undo_buf();
		push_redo_buf(buf_after);
		buf_before = pop_undo_buf();
		push_redo_buf(buf_before);
	} else {
		// redo (move undo information from redo-buf to undo-buf)
		buf_after = pop_redo_buf();
		push_undo_buf(buf_after);
		buf_before = pop_redo_buf();
		push_undo_buf(buf_before);
	}

	// delete modified lines
	edit_line = delete_region_in_buf(buf_after);
	top_line = edit_line->prev;
	// insert unmodified lines
	insert_region_from_buf(edit_line, buf_before);
	return top_line;
}
PRIVATE be_line_t *delete_region_in_buf(be_buf_t *buf)
{
	be_line_t *edit_line;
	be_line_t *undo_line;

	if (switch_c_e_b_to_abs_path(buf->abs_path) == 0) {
		progerr_printf("No such buffer: %s\n", buf->abs_path);
		return CUR_EDIT_BUF_BOT_LINE;
	}
	edit_line = get_line_ptr_from_cur_buf_line_num(BUF_TOP_LINE(buf)->line_num);
	for (undo_line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(undo_line) == 0;
	 undo_line = undo_line->next) {
		edit_line = line_unlink_free(edit_line);	// delete line
	}
	return edit_line;
}
PRIVATE be_line_t *insert_region_from_buf(be_line_t *edit_line, be_buf_t *buf)
{
	be_line_t *undo_line;

	if (switch_c_e_b_to_abs_path(buf->abs_path) == 0) {
		progerr_printf("No such buffer: %s\n", buf->abs_path);
		return CUR_EDIT_BUF_BOT_LINE;
	}
	for (undo_line = BUF_TOP_LINE(buf); IS_NODE_BOT_ANCH(undo_line) == 0; ) {
#if 1
		line_insert(edit_line, line_create_copy(undo_line), INSERT_BEFORE);
		undo_line = undo_line->next;
#else
		undo_line = undo_line->next;
		line_insert(edit_line, line_create_copy(undo_line->prev), INSERT_BEFORE);
#endif
	}
	// restore pointers
	CEBV_CLBI = buf->views[0].cur_line_byte_idx;
	CEBV_CL = get_line_ptr_from_cur_buf_line_num(BUF_TOP_LINE(buf)->line_num);
	return edit_line;
}

#ifdef ENABLE_DEBUG
void dump_undo_bufs_lines(void)
{
	buffer_dump_bufs_lines(UNDO_BUFS_TOP_ANCH, "undo-bufs");
}
void dump_redo_bufs_lines(void)
{
	buffer_dump_bufs_lines(REDO_BUFS_TOP_ANCH, "redo-bufs");
}
#endif // ENABLE_DEBUG

#endif // ENABLE_UNDO

// End of editorundo.c
