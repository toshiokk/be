/**************************************************************************
 *   buffer.h                                                             *
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

#ifndef buffer_h
#define buffer_h

#define RW0000RW0		\
 (S_IRUSR | S_IWUSR | 0 | 0 | 0 | 0 | S_IROTH | S_IWOTH | 0)
#define RW0RW0R00		\
 (S_IRUSR | S_IWUSR | 0 | S_IRGRP | S_IWGRP | 0 | S_IROTH | 0 | 0)
#define RWXRWXRWX		\
 (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)

typedef struct be_buf_view_t {
	be_line_t *cur_line;			//!< pointer to current line (be_line_t)
	int cur_line_byte_idx;			//!< current line byte index
	int cursor_y;					//! display y to which cur-line to be displayed
	int cursor_x_to_keep;			//!< cursor X to keep when moving cursor vertically
	int min_text_x_to_be_disp;		// text x to be displayed on the left edge of the screen
} be_buf_view_t;

#define BUF_VIEWS		2			// NOTE: must be the same as MAX_APP_PANES

//! buffer, correction of lines
typedef struct be_buf_t {
	struct be_buf_t *prev;		//!< Previous be_buf_t
	struct be_buf_t *next;		//!< Next be_buf_t

	char file_path[MAX_PATH_LEN+1];	//!< relative file path
	char abs_path[MAX_PATH_LEN+1];	//!< absolute file path
	struct stat orig_file_stat;		//!< original file status
	unsigned short orig_file_crc;	//!< file CRC before modification

	be_line_t top_anchor;			//!< top line
	be_line_t bot_anchor;			//!< bottom line
	be_buf_view_t views[BUF_VIEWS];
	be_line_t *mark_line;			//!< 
	int mark_line_byte_idx;			//!< 
	int buf_lines;					//!< line count
	size_t buf_size;				//!< buffer size
	buf_state_t buf_state;			//!< buffer state
} be_buf_t;

typedef struct /*be_bufs_t*/ {
	be_buf_t top_anchor;	//< top buffer
	be_buf_t bot_anchor;	//< bottom buffer
} be_bufs_t;

// buffers ==> buffer
#define BUFS_TOP_ANCH(bufs)		NODES_TOP_ANCH(bufs)
#define BUFS_TOP_BUF(bufs)		NODES_TOP_NODE(bufs)
#define BUFS_BOT_BUF(bufs)		NODES_BOT_NODE(bufs)
#define BUFS_BOT_ANCH(bufs)		NODES_BOT_ANCH(bufs)

// buffer ==> line
#define BUF_TOP_ANCH(buf)		NODES_TOP_ANCH(buf)
#define BUF_TOP_LINE(buf)		NODES_TOP_NODE(buf)
#define BUF_BOT_LINE(buf)		NODES_BOT_NODE(buf)
#define BUF_BOT_ANCH(buf)		NODES_BOT_ANCH(buf)

be_buf_t *buffer_create(const char *file_path);
be_buf_t *buffer_init(be_buf_t *buf, const char *file_path);
be_buf_t *buffer_init_line_anchors(be_buf_t *buf);
void buffer_set_file_path(be_buf_t *buf, const char *file_path);
void buffer_get_file_path(be_buf_t *buf, char *file_path);
be_buf_t *buffer_insert(be_buf_t *buf, be_buf_t *new_buf,
 insert_before_after_t before_after);
be_buf_t *buffer_insert_before(be_buf_t *buf, be_buf_t *new_buf);
be_buf_t *buffer_insert_after(be_buf_t *buf, be_buf_t *new_buf);
be_buf_t *buffer_insert_between(be_buf_t *prev, be_buf_t *new_buf, be_buf_t *next);
be_buf_t *buffer_link(be_buf_t *prev, be_buf_t *next);

be_buf_t *buffer_create_copy(be_buf_t *src);
be_buf_t *buffer_copy(be_buf_t *dest, be_buf_t *src);

be_buf_t *buffer_unlink_free(be_buf_t *buf);
be_buf_t *buffer_unlink(be_buf_t *buf);
void buffer_clear_link(be_buf_t *buf);
void buffer_free(be_buf_t *buf);
void buffer_free_lines(be_buf_t *buf);

int buffer_compare(be_buf_t *buf1, be_buf_t *buf2);

int buffer_renumber_from_line(be_buf_t *buf, be_line_t *line);
int buffer_guess_tab_size(be_buf_t *buf);

int buffer_count_bufs(be_buf_t *buf);
int buffer_count_lines(be_buf_t *buf);
int buffer_is_orig_file_updated(be_buf_t *buf);

const char *buffer_eol_str(be_buf_t *buf);
const char *buffer_encode_str(be_buf_t *buf);
const char *buffer_cut_mode_str(be_buf_t *buf);

//-----------------------------------------------------------------------------

be_line_t *buffer_set_cur_line(be_buf_t *buf, be_line_t *line);
be_line_t *buffer_cur_line(be_buf_t *buf);
be_line_t *buffer_get_cur_line__move_prev(be_buf_t *buf);
be_line_t *buffer_get_cur_line__move_next(be_buf_t *buf);

be_line_t *buffer_get_line_ptr_from_line_num(be_buf_t *buf, int line_num);

void buffer_update_crc(be_buf_t *buf);
int buffer_check_crc(be_buf_t *buf);
unsigned short buffer_calc_crc(be_buf_t *buf);

const char *buffer_get_enc_str(be_buf_t *buf);
const char *buffer_get_eol_str(be_buf_t *buf);

//-----------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void buffer_dump_bufs(be_buf_t *buf);
void buffer_dump_bufs_lines(be_buf_t *buf, const char *label);
void buffer_dump_lines(be_buf_t *buf, int lines);
void buffer_dump_ptrs(be_buf_t *buf);
void buffer_dump_state(be_buf_t *buf);
be_line_t *buffer_check_line_in_buf(be_buf_t *buf, be_line_t *line_);
#endif // ENABLE_DEBUG

#endif // buffer_h

// End of buffer.h
