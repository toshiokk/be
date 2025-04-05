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

//------------------------------------------------------------------------------

typedef struct /*buf_state*/ {
#define buf_MODE_EDIT		0				// Normal buffer (modifiable)
#define buf_MODE_RO			1				// Read Only open file (unmodifiable)
#define buf_MODE_LIST		2				// List (unmodifiable)
	unsigned char buf_MODE:2;				// bit 0,1
	unsigned char buf_LOCKED:1;				// bit 2

	unsigned char buf_MAGIC_LINE_ADDED:1;	// bit 3

	unsigned char buf_MODIFIED:1;			// bit 4
	unsigned char buf_LINE_WRAP_MODE:1;		// bit 5
#if 0 // 0
#define HV_IS_BOX	// HV is BOX-selection-mode, VH is CHAR-selection-mode
#else
#define VH_IS_BOX	// HV is LINE-selection-mode, VH is BOX-selection-mode
#endif
#define CUT_MODE_0_LINE			0	// no mark                                    (one line cut)
#define CUT_MODE_N_LINE			1	// marking started but cursor not moved           (line cut)
#define CUT_MODE_H_CHAR			2	//  and cursor moved horizontally                 (char cut)
#define CUT_MODE_HV_LINE		3	//  and cursor moved horizontally then vertically (line cut)
#define CUT_MODE_HV_BOX			4	//  and cursor moved horizontally then vertically (box cut)
#define CUT_MODE_V_LINE			5	//  and cursor moved vertically                   (line cut)
#define CUT_MODE_VH_CHAR		6	//  and cursor moved vertically then horizontally (char cut)
#define CUT_MODE_VH_BOX			7	//  and cursor moved vertically then horizontally (box cut)
#define IS_MARK_SET(cut_mode)	((cut_mode) != CUT_MODE_0_LINE)
	unsigned char buf_CUT_MODE:3;			// bit 6-8
#define TAB_SIZE_MIN			1
#define TAB_SIZE_0				0			// DEFAULT_TAB_SIZE
#define TAB_SIZE_1				1
#define TAB_SIZE_2				2
#define TAB_SIZE_3				3
#define TAB_SIZE_4				4
#define TAB_SIZE_5				5
#define TAB_SIZE_6				6
#define TAB_SIZE_7				7
#define TAB_SIZE_8				8
#define TAB_SIZE_MAX			TAB_SIZE_8
	unsigned char buf_TAB_SIZE:4;			// bit 9-12
#define EOL_NIX					0
#define EOL_MAC					1
#define EOL_DOS					2
#define EOL_MAX					EOL_DOS
	unsigned char buf_EOL:2;				// bit 13-14
#define ENCODE_ASCII			0
#define ENCODE_UTF8				1
#ifdef USE_NKF
#define ENCODE_EUCJP			2
#define ENCODE_SJIS				3
#define ENCODE_JIS				4
#endif // USE_NKF
#define ENCODE_BINARY			5
#define ENCODE_MAX				ENCODE_BINARY
	unsigned char buf_ENCODE:3;				// bit 15-17
} buf_state_t;

#define EDITOR_PANES		MAX_APP_PANES_2

//! buffer, collection of lines
typedef struct be_buf_t {
	struct be_buf_t *prev;		//!< Previous be_buf_t
	struct be_buf_t *next;		//!< Next be_buf_t

	char file_path_[MAX_PATH_LEN+1];	//!< relative file path (use this as a file accessor)
	struct stat orig_file_stat;			//!< original file status
	UINT16 orig_file_crc;		//!< file CRC before modification / expiration timer

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

//! bufferss, collection of buffers
typedef struct be_bufss_t {
	char name[MAX_NAME_LEN+1];	//! name
	be_bufs_t top_anchor;		//!< top    buffer
	be_bufs_t bot_anchor;		//!< bottom buffer
	be_bufs_t *cur_bufs;		//!< current buffer
} be_bufss_t;

// Structure of collections of buffers:
//   be_bufs_t all_bufferss.top_anchor
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
//   be_bufs_t help_bufs
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
//   be_bufs_t all_bufferss.bot_anchor

be_buf_t *buf_create_node(const char *full_path, unsigned char buf_mode_);
be_buf_t *buf_free_node(be_buf_t *buf);

be_buf_t *buf_init(be_buf_t *buf, const char *full_path, unsigned char buf_mode_);
void buf_views_init(be_buf_t *buf);
void buf_views_set_cur_line(be_buf_t *buf);
void buf_view_copy(be_buf_view_t *dest, be_buf_view_t *src);
void buf_set_view_x_cur_line(be_buf_t *buf, int pane_idx, be_line_t *line);
be_buf_t *buf_init_anchors(be_buf_t *buf, char *initial_data);
void buf_set_file_path(be_buf_t *buf, const char *file_path);
const char* buf_get_file_path(be_buf_t *buf, char *file_path);
const char* buf_get_abs_path(be_buf_t *buf, char *abs_path);
void buf_invalidate_file_path(be_buf_t *buf);
const char* buf_get_file_path_valid(be_buf_t *buf, char *file_path);
const char* buf_get_abs_path_valid(be_buf_t *buf, char *abs_path);

BOOL buf_is_empty(be_buf_t *buf);
be_buf_t *buf_insert_before(be_buf_t *buf, be_buf_t *new_buf);
be_buf_t *buf_insert_after(be_buf_t *buf, be_buf_t *new_buf);
be_buf_t *buf_link(be_buf_t *prev, be_buf_t *next);

be_buf_t *buf_create_copy_node(be_buf_t *buf);
be_buf_t *buf_copy(be_buf_t *dest, be_buf_t *src);

be_buf_t *buf_unlink_free(be_buf_t *buf);
be_buf_t *buf_unlink(be_buf_t *buf);
void buf_clear_link(be_buf_t *buf);

void buf_free_lines(be_buf_t *buf);

be_line_t *buf_append_line_to_bottom(be_buf_t *buf, be_line_t *line);

int buf_compare(be_buf_t *buf1, be_buf_t *buf2);

int buf_renumber_from_top(be_buf_t *buf);
int buf_renumber_from_line(be_buf_t *buf, be_line_t *line);
int buf_guess_tab_size(be_buf_t *buf);

int buf_count_lines(be_buf_t *buf, int max_lines);

int buf_get_file_stat(be_buf_t *buf, const char* file_path);
int buf_has_orig_file_updated(be_buf_t *buf, const char* file_path);

const char *buf_mode_str(be_buf_t *buf);
const char *buf_eol_str(be_buf_t *buf);
const char *buf_enc_str(be_buf_t *buf);
const char *buf_cut_mode_str(be_buf_t *buf);

//------------------------------------------------------------------------------
void buf_fix_cur_line(be_buf_t *buf);
be_line_t *buf_set_cur_line(be_buf_t *buf, be_line_t *line);
be_line_t *buf_move_cur_line_to_prev(be_buf_t *buf);
be_line_t *buf_move_cur_line_to_next(be_buf_t *buf);

be_line_t *buf_get_line_ptr_from_line_num(be_buf_t *buf, int line_num);

void buf_update_crc(be_buf_t *buf);
int buf_check_crc(be_buf_t *buf);
UINT16 buf_calc_crc(be_buf_t *buf);

void buf_set_save_pending_timer(be_buf_t *buf, UINT16 timer);
UINT16 buf_get_save_pending_timer(be_buf_t *buf);

int buf_count_buf(be_buf_t *buf);
be_buf_t *buf_make_buf_intermediate(be_buf_t *buf);
be_buf_t *buf_get_another_buf(be_buf_t *buf);

#define TEMPORAL_HISTORY_MARK		"#"
int is_temporal_file_path(const char* str);

//------------------------------------------------------------------------------
be_bufs_t *bufs_init(be_bufs_t *bufs, const char* name,
 const char* name_top, const char* name_bottom);
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
int bufs_count_buf(be_bufs_t *bufs);

//------------------------------------------------------------------------------
be_buf_t *buf_get_buf_by_idx(be_buf_t *buf, int buf_idx);
int buf_get_buf_idx(be_buf_t *buf);
int buf_get_buf_idx_in_bufs(be_bufs_t *bufs, be_buf_t *buf);

be_buf_t *buf_get_buf_by_file_path(be_buf_t *buf, const char *abs_path);
be_buf_t *buf_get_buf_by_file_name(be_buf_t *buf, const char *file_name);

void bufs_renumber_all_bufs_from_top(be_bufs_t *bufs);

//------------------------------------------------------------------------------
void bufss_init(be_bufss_t *bufss, const char *name,
 const char *name_top, const char *name_bottom);
void bufss_insert_bufs_to_bottom(be_bufss_t *bufss, be_bufs_t *bufs);

int bufs_get_bufs_idx_in_bufss(be_bufs_t *bufs, be_buf_t *buf);

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
be_line_t *buf_check_line_in_buf(be_buf_t *buf, be_line_t *line_);
be_line_t *buf_check_line_in_buf_anchs(be_buf_t *buf, be_line_t *line_);
void buf_dump_bufs(be_buf_t *buf);
void buf_dump_bufs_lines(be_buf_t *buf, const char *label);
void buf_dump_lines(be_buf_t *buf, int lines);
void buf_dump_ptrs(be_buf_t *buf);
void buf_dump_name(be_buf_t *buf);
const char* buf_dump_buf_state(be_buf_t *buf);

void bufs_dump_all_bufs(be_bufs_t *buf);
void bufs_dump_name(be_bufs_t *bufs);

#endif // ENABLE_DEBUG

#endif // buffer_h

// End of buffer.h
