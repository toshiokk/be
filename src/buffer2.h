/**************************************************************************
 *   buffer2.h                                                             *
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

#ifndef buffer2_h
#define buffer2_h

#define MAX_NAME_LEN	80
//! buffers, collection of buffers
typedef struct be_bufs_t {
	struct be_bufs_t *prev;		//!< Previous be_bufs_t
	struct be_bufs_t *next;		//!< Next     be_bufs_t
	char name[MAX_NAME_LEN+1];	//! name
	be_buf_t top_anchor;		//!< top    buffer
	be_buf_t bot_anchor;		//!< bottom buffer
	be_buf_t *cur_buf;			//!< current buffer
} be_bufs_t;

//! bufferss, collection of buffers
typedef struct be_bufss_t {
	char name[MAX_NAME_LEN+1];	//! name
	be_bufs_t top_anchor;		//!< top    buffer
	be_bufs_t bot_anchor;		//!< bottom buffer
} be_bufss_t;

//------------------------------------------------------------------------------
be_bufs_t *bufs_init(be_bufs_t *bufs, const char *name,
 const char *name_top, const char *name_bottom);
void bufs_init_anchors(be_bufs_t *bufs, const char *full_path_top, const char *full_path_bot);
void bufs_link(be_bufs_t *top_anchor, be_bufs_t *bot_anchor);
void bufs_set_cur_buf(be_bufs_t* bufs, be_buf_t* buf);
void bufs_insert_buf_to_top(be_bufs_t *bufs, be_buf_t *buf);
void bufs_insert_buf_to_bottom(be_bufs_t *bufs, be_buf_t *buf);
void bufs_insert_before(be_bufs_t *bufs, be_bufs_t *other);
void bufs_insert_between(be_bufs_t *prev, be_bufs_t *mid, be_bufs_t *next);
be_bufs_t *bufs_free_all_bufs(be_bufs_t *bufs);
be_bufs_t *bufs_get_bufs_contains_buf(be_bufs_t *bufs, be_buf_t *cur_buf);
void bufs_fix_cur_buf(be_bufs_t *bufs);

int bufs_count_bufs(be_bufs_t *bufs);
int bufs_get_file_stat(be_bufs_t *bufs, const char *file_path);

void bufs_set_modified__pending_timer(be_bufs_t *bufs);
int bufs_is_modified_newer__expired(be_bufs_t *bufs, const char *file_path, UINT16 dsec);

void bufs_clear_modified(be_bufs_t *bufs);
void bufs_set_modified(be_bufs_t *bufs);
int bufs_get_modified(be_bufs_t *bufs);
int bufs_compare_mtime_to_cur_file(be_bufs_t *bufs, const char *file_path);

void bufs_renumber_all_bufs_from_top(be_bufs_t *bufs);

//------------------------------------------------------------------------------
void bufss_init(be_bufss_t *bufss, const char *name,
 const char *name_top, const char *name_bottom);
void bufss_insert_bufs_to_bottom(be_bufss_t *bufss, be_bufs_t *bufs);

int bufs_get_bufs_idx_in_bufss(be_bufs_t *bufs, be_buf_t *buf);
int bufs_get_buf_idx(be_bufs_t *bufs, be_buf_t *buf);

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void bufs_dump_all_bufs(be_bufs_t *bufs);
void bufs_dump_name(be_bufs_t *bufs);
#endif // ENABLE_DEBUG

#endif // buffer2_h

// End of buffer2.h
