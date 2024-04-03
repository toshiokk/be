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
	be_line_t *cur_line;		// current line (be_line_t)
	int cur_line_byte_idx;		// current line byte index
	int cursor_y;				// display Y in screen to which cur-line displayed
	int cursor_x_to_keep;		// cursor X in text to keep when moving cursor vertically
	int min_text_x_to_keep;		// text X to be displayed on the left edge of the screen
} be_buf_view_t;

#define EDITOR_PANES		MAX_APP_PANES_2

//! buffer, collection of lines
typedef struct be_buf_t {
	struct be_buf_t *prev;		//!< Previous be_buf_t
	struct be_buf_t *next;		//!< Next be_buf_t

	char file_path[MAX_PATH_LEN+1];	//!< relative file path
	char abs_path[MAX_PATH_LEN+1];	//!< absolute file path
	struct stat orig_file_stat;		//!< original file status
	unsigned short orig_file_crc;	//!< file CRC before modification

	be_line_t top_anchor;		//!< top line
	be_line_t bot_anchor;		//!< bottom line
	be_buf_view_t buf_views[EDITOR_PANES];
	be_line_t *mark_line;		//!< 
	int mark_line_byte_idx;		//!< 
	int buf_lines;				//!< line count
	size_t buf_size;			//!< buffer size
	buf_state_t buf_state;		//!< buffer state
} be_buf_t;

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

// Structure of collections of buffers:
//   be_bufs_t bufss_top_anchor
//   be_bufs_t edit_bufs
//     be_buf_t main.c
//          :
//     be_buf_t headers.h
//   be_bufs_t cut_bufs
//     be_buf_t cut-1
//          :
//     be_buf_t cut-n
//   be_bufs_t hist_bufs
//     be_buf_t exec_hist
//     be_buf_t dir_hist
//     be_buf_t key_macro_hist
//     be_buf_t shell_hist
//     be_buf_t file_pos_hist
//   be_bufs_t temp-bufs
//     be_buf_t file_list
//     be_buf_t key_binding_list
//     be_buf_t func_list
//   be_bufs_t undo_bufs
//     be_buf_t undo-1
//          :
//     be_buf_t undo-n
//   be_bufs_t redo_bufs
//     be_buf_t redo-1
//          :
//     be_buf_t redo-n
//   be_bufs_t bufss_bot_anchor

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

be_buf_t *buf_create_node(const char *file_path);
be_buf_t *buf_free_node(be_buf_t *buf);

be_buf_t *buf_init(be_buf_t *buf, const char *file_path);
void buf_view_init(be_buf_view_t *b_v, be_buf_t *buf);
be_buf_t *buf_init_line_anchors(be_buf_t *buf, char *initial_data);
void buf_set_file_path(be_buf_t *buf, const char *file_path);
void buf_get_file_path(be_buf_t *buf, char *file_path);
be_buf_t *buf_insert_before(be_buf_t *buf, be_buf_t *new_buf);
be_buf_t *buf_insert_after(be_buf_t *buf, be_buf_t *new_buf);
be_buf_t *buf_link(be_buf_t *prev, be_buf_t *next);

be_buf_t *buf_create_copy(be_buf_t *src);
be_buf_t *buf_copy(be_buf_t *dest, be_buf_t *src);

be_buf_t *buf_unlink_free(be_buf_t *buf);
be_buf_t *buf_unlink(be_buf_t *buf);
void buf_clear_link(be_buf_t *buf);

void buf_free_lines(be_buf_t *buf);

int buf_compare(be_buf_t *buf1, be_buf_t *buf2);

int buf_renumber_from_top(be_buf_t *buf);
int buf_renumber_from_line(be_buf_t *buf, be_line_t *line);
int buf_guess_tab_size(be_buf_t *buf);

int buf_count_bufs(be_bufs_t *bufs);
int buf_count_lines(be_buf_t *buf);
int buf_is_orig_file_updated(be_buf_t *buf);

const char *buf_eol_str(be_buf_t *buf);
const char *buf_encode_str(be_buf_t *buf);
const char *buf_cut_mode_str(be_buf_t *buf);

//-----------------------------------------------------------------------------

be_line_t *buf_set_cur_line(be_buf_t *buf, be_line_t *line);
be_line_t *buf_cur_line(be_buf_t *buf);
be_line_t *buf_move_cur_line_to_prev(be_buf_t *buf);
be_line_t *buf_move_cur_line_to_next(be_buf_t *buf);

be_line_t *buf_get_line_ptr_from_line_num(be_buf_t *buf, int line_num);

void buf_update_crc(be_buf_t *buf);
int buf_check_crc(be_buf_t *buf);
unsigned short buf_calc_crc(be_buf_t *buf);

const char *buf_get_enc_str(be_buf_t *buf);
const char *buf_get_eol_str(be_buf_t *buf);

//-----------------------------------------------------------------------------

be_bufs_t *bufs_init(be_bufs_t *bufs, const char* buf_name);
be_bufs_t *bufs_link(be_bufs_t *top_anchor, be_bufs_t *bot_anchor);
be_bufs_t *bufs_insert_before(be_bufs_t *bufs, be_bufs_t *other);
be_bufs_t *bufs_insert_between(be_bufs_t *prev, be_bufs_t *mid, be_bufs_t *next);
be_bufs_t *bufs_free_all_bufs(be_bufs_t *bufs);
be_bufs_t *get_bufs_contains_buf(be_bufs_t *bufs, be_buf_t *cur_buf);

//-----------------------------------------------------------------------------

void init_bufs_top_bot_anchor(
 be_buf_t *buf_top, const char *full_path_top,
 be_buf_t *buf_bot, const char *full_path_bot);

be_buf_t *get_buf_from_bufs_by_idx(be_buf_t *buf, int buf_idx);
int get_buf_idx_in_bufs(be_buf_t *bufs, be_buf_t *buf);
be_buf_t *get_buf_from_bufs_by_abs_path(be_buf_t *bufs, const char *abs_path);

void renumber_all_bufs_from_top(be_bufs_t *bufs);

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void buf_dump_bufs(be_buf_t *buf);
void buf_dump_bufs_lines(be_buf_t *buf, const char *label);
void buf_dump_lines(be_buf_t *buf, int lines);
void buf_dump_ptrs(be_buf_t *buf);
void buf_dump_state(be_buf_t *buf);
/////be_line_t *buf_check_line_in_buf(be_buf_t *buf, be_line_t *line_);

void bufs_dump_all_bufs(be_bufs_t *buf);
#endif // ENABLE_DEBUG

#endif // buffer_h

// End of buffer.h
