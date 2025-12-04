/**************************************************************************
 *   buffer2.c                                                             *
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

PRIVATE be_bufs_t *bufs_make_top_anchor(be_bufs_t *bufs);

be_bufs_t *bufs_init(be_bufs_t *bufs, const char *name,
 const char *name_top, const char *name_bottom)
{
	bufs->prev = bufs->next = NULL;
	strlcpy__(bufs->name, name, MAX_NAME_LEN);
	bufs_init_anchors(bufs, name_top, name_bottom);
	bufs_set_cur_buf(bufs, NODES_BOT_ANCH(bufs));
	return bufs;
}
void bufs_init_anchors(be_bufs_t *bufs, const char *full_path_top, const char *full_path_bot)
{
	buf_init(NODES_TOP_ANCH(bufs), full_path_top, BUF_MODE_LIST);
	buf_init(NODES_BOT_ANCH(bufs), full_path_bot, BUF_MODE_LIST);
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
	int count_bufs = bufs_count_bufs(bufs);
	int buf_idx = bufs_get_buf_idx(bufs, bufs->cur_buf);
	if (1 <= buf_idx && buf_idx <= count_bufs) {
		// `bufs->cur_buf` is valid
	} else {
		// `bufs->cur_buf` is not valid
		bufs->cur_buf = NODES_TOP_NODE(bufs);	// fix `cur_buf`
	}
	buf_fix_cur_line(bufs->cur_buf);		// fix `cur_line` too
}

int bufs_count_bufs(be_bufs_t *bufs)
{
	return buf_count(NODES_TOP_NODE(bufs));
}

int bufs_get_file_stat(be_bufs_t *bufs, const char *file_path)
{
	return buf_get_file_stat(BUFS_INFO_BUF(bufs), file_path);
}

void bufs_set_modified__pending_timer(be_bufs_t *bufs)
{
	buf_set_modified__pending_timer(BUFS_INFO_BUF(bufs));
}
int bufs_is_modified_newer__expired(be_bufs_t *bufs, const char *file_path, UINT16 dsec)
{
	return buf_is_modified_newer__expired(BUFS_INFO_BUF(bufs), file_path, dsec);
}

void bufs_clear_modified(be_bufs_t *bufs)
{
	buf_clear_modified__pending_timer(BUFS_INFO_BUF(bufs));
}
void bufs_set_modified(be_bufs_t *bufs)
{
	buf_set_modified(BUFS_INFO_BUF(bufs));
}
int bufs_get_modified(be_bufs_t *bufs)
{
	return buf_get_modified(BUFS_INFO_BUF(bufs));
}
int bufs_compare_mtime_to_cur_file(be_bufs_t *bufs, const char *file_path)
{
	return buf_compare_mtime_to_cur_file(BUFS_INFO_BUF(bufs), file_path);
}

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
}
void bufss_insert_bufs_to_bottom(be_bufss_t *bufss, be_bufs_t *bufs)
{
	bufs_insert_before(NODES_BOT_ANCH(bufss), bufs);
}

// top-anchor:0, bufs1:1, bufs2:2, ..., bufsN:n, bottom-anchor:(n+1)
int bufs_get_bufs_idx_in_bufss(be_bufs_t *bufs, be_buf_t *buf)
{
	bufs = bufs_make_top_anchor(bufs);
	for (int bufs_idx = 0; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs), bufs_idx++) {
		if (bufs_get_buf_idx(bufs, buf) >= 0) {
			// buf was found in the bufs
			return bufs_idx;	// return bufs_idx of the bufs
		}
	}
	return -1;	// not found
}
int bufs_get_buf_idx(be_bufs_t *bufs, be_buf_t *buf)
{
	// -1:not found in the `bufs`
	return buf_get_buf_idx_from_top_anch(NODES_TOP_ANCH(bufs), buf);
}

PRIVATE be_bufs_t *bufs_make_top_anchor(be_bufs_t *bufs)
{
	for ( ; IS_NODE_TOP_OOL(bufs) == 0; bufs = NODE_PREV(bufs)) {
		// NOTHING_TO_DO
	}
	return bufs;
}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG

void bufs_dump_all_bufs(be_bufs_t *bufs)
{
	flf_dprintf("00============================================\n");
	for ( ; IS_PTR_VALID(bufs); bufs = NODE_NEXT(bufs)) {
		flf_dprintf("bufs: [%s]\n", bufs->name);
		for (be_buf_t *buf = NODES_TOP_ANCH(bufs); IS_PTR_VALID(buf); buf = NODE_NEXT(buf)) {
			flf_dprintf(" %cbuf: [%s]\n",
			 (bufs->cur_buf == buf) ? '>' : ' ', buf_get_file_path(buf, NULL));
			flf_dprintf("    buf->v0_str: [%s]\n", buf->buf_views[0].cur_line->data);
			flf_dprintf("    buf->v1_str: [%s]\n", buf->buf_views[1].cur_line->data);
		}
	}
	flf_dprintf("99============================================\n");
}
void bufs_dump_name(be_bufs_t *bufs)
{
	if (bufs == NULL) {
		flf_dprintf("bufs: NULL\n");
		return;
	}
	flf_dprintf("name: [%s]\n", bufs->name);
}

#endif // ENABLE_DEBUG

// End of buffer2.c
