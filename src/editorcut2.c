/**************************************************************************
 *   editorcut2.c                                                         *
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

// The accutual cut region
// cut region (minimum)
be_line_t *mark_min_line__ = NULL;	// the line top of the marked area
int mark_min_byte_idx;				// byte_idx in the line top of the marked area
int mark_min_col_idx;				// col_idx left most in the marked area
// cut region (maximum)
be_line_t *mark_max_line__ = NULL;	// the line bottom of the marked area
int mark_max_byte_idx;				// byte_idx in the line bottom of the marked area
int mark_max_col_idx;				// col_idx right most in the marked area

//------------------------------------------------------------------------------
void clear_mark_pos()
{
	EPCB_ML = NODES_TOP_ANCH(get_epc_buf());
	EPCB_MLBI = 0;
	clear_cut_region();
}
void set_mark_pos()
{
	EPCB_ML = EPCBVC_CL;
	EPCB_MLBI = EPCBVC_CLBI;
	setup_cut_region();
}

PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t cursor_move);

void setup_cut_region_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	if (IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE))) {
		change_cut_mode_after_cursor_horiz_vert_move(cursor_move);
		setup_cut_region();
		set_edit_win_update_needed(UPDATE_SCRN_ALL);
	}
}

// [cut-mode transition]
// |previous mode      |event / cursor movement |next mode       |comments                     |
// |-------------------|------------------------|----------------|-----------------------------|
// |CUT_MODE_0_NONE    |start regioning         |CUT_MODE_N_LINE |                             |
// | CUT_MODE_N_LINE   | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |enter HV transition group    |
// | CUT_MODE_N_LINE   | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |enter VH transition group    |
// |HV transition group|------------------------|----------------|-----------------------------|
// |  CUT_MODE_H_CHAR  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |                             |
// |  CUT_MODE_H_CHAR  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_LINE|                             |
// |  CUT_MODE_H_CHAR  |return to start col/line|CUT_MODE_N_LINE |return to NO transition group|
// |   CUT_MODE_HV_BOX |return to start col/line|CUT_MODE_N_LINE |return to NO transition group|
// |   CUT_MODE_HV_LINE| ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |                             |
// |   CUT_MODE_HV_LINE| ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_LINE|                             |
// |   CUT_MODE_HV_LINE|return to start col/line|CUT_MODE_N_LINE |return to NO transition group|
// |VH transition group|------------------------|----------------|-----------------------------|
// |  CUT_MODE_V_LINE  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_BOX |                             |
// |  CUT_MODE_V_LINE  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |                             |
// |  CUT_MODE_V_LINE  |return to start col/line|CUT_MODE_N_LINE |return to NO transition group|
// |   CUT_MODE_VH_CHAR|return to start col/line|CUT_MODE_N_LINE |return to NO transition group|
// |   CUT_MODE_VH_BOX | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_CHAR|                             |
// |   CUT_MODE_VH_BOX | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_VH_BOX |                             |
// |   CUT_MODE_VH_BOX |return to start col/line|CUT_MODE_N_LINE |return to NO transition group|

//
// CUT_MODE_0_NONE:
//   |  (start regioning)
//   v
// CUT_MODE_N_LINE:
//   -->  (CURS_MOVE_HORIZ)
//   CUT_MODE_H_CHAR:
//       |  (CURS_MOVE_VERT)
//       v
//     CUT_MODE_HV_LINE:
//       -->
//       CUT_MODE_H_CHAR:
//
// CUT_MODE_0_NONE:
//   |  (start regioning)
//   v
// CUT_MODE_N_LINE:
//   |  (CURS_MOVE_VERT)
//   v
//   CUT_MODE_V_LINE:
//     -->  (CURS_MOVE_HORIZ)
//     CUT_MODE_VH_BOX:
//       |
//       v
//       CUT_MODE_V_LINE:
//

PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t cursor_move)
{
	switch (cursor_move) {
	default:
	case CURS_MOVE_NONE:
		break;
	case CURS_MOVE_HORIZ:
		int epcbvc_col_idx = col_idx_from_byte_idx(EPCBVC_CL->data, EPCBVC_CLBI);
		int epcb_col_idx = col_idx_from_byte_idx(EPCB_ML->data, EPCB_MLBI);
		if (epcbvc_col_idx == epcb_col_idx) {
			// current column == beginning column
			switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_NONE:
			case CUT_MODE_N_LINE:
				break;
			case CUT_MODE_H_CHAR:
			case CUT_MODE_HV_LINE:
			case CUT_MODE_V_LINE:
			case CUT_MODE_VH_CHAR:
			case CUT_MODE_HV_BOX:
			case CUT_MODE_VH_BOX:
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
				return;
			}
		}
		break;
	case CURS_MOVE_VERT:
	case CURS_MOVE_JUMP:
		if (EPCBVC_CL == EPCB_ML) {
			// current line == beginning line
			switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_NONE:
			case CUT_MODE_N_LINE:
				break;
			case CUT_MODE_H_CHAR:
			case CUT_MODE_HV_LINE:
			case CUT_MODE_V_LINE:
			case CUT_MODE_VH_CHAR:
			case CUT_MODE_HV_BOX:
			case CUT_MODE_VH_BOX:
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
				return;
			}
		}
		break;
	}

	switch (cursor_move) {
	default:
	case CURS_MOVE_NONE:
		break;
	case CURS_MOVE_HORIZ:
		switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_NONE:
			break;
		case CUT_MODE_N_LINE:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
			break;
		case CUT_MODE_H_CHAR:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
			break;
		case CUT_MODE_HV_LINE:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
			break;
		case CUT_MODE_V_LINE:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
			break;
		case CUT_MODE_VH_CHAR:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_CHAR;
			break;
		case CUT_MODE_HV_BOX:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
			break;
		case CUT_MODE_VH_BOX:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
			break;
		}
		break;
	case CURS_MOVE_VERT:
		switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_NONE:
			break;
		case CUT_MODE_N_LINE:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
			break;
		case CUT_MODE_H_CHAR:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
			break;
		case CUT_MODE_HV_LINE:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
			break;
		case CUT_MODE_V_LINE:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
			break;
		case CUT_MODE_VH_CHAR:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
			break;
		case CUT_MODE_HV_BOX:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_BOX;
			break;
		case CUT_MODE_VH_BOX:
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
			break;
		}
		break;
	case CURS_MOVE_JUMP:
		GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
		break;
	}
}

// setup below variables:
//  be_line_t *mark_min_line__;
//  int mark_min_byte_idx;
//  int mark_min_col_idx;
//  be_line_t *mark_max_line__;
//  int mark_max_byte_idx;
//  int mark_max_col_idx;

void clear_cut_region()
{
	mark_min_line__ = NULL;
	mark_min_byte_idx = 0;
	mark_min_col_idx = 0;
	mark_max_line__ = NULL;
	mark_max_byte_idx = 0;
	mark_max_col_idx = 0;
}
void setup_cut_region()
{
	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_NONE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		if (EPCBVC_CL == EPCB_ML) {
			if (IS_NODE_BOT_MOST(EPCBVC_CL)) {
				// No next line
				// {aaaaaaaaaaaaaaaaaa}
				mark_min_line__ = EPCBVC_CL;
				mark_max_line__ = EPCBVC_CL;
				mark_min_byte_idx = 0;
				mark_max_byte_idx = line_strlen(EPCBVC_CL);
			} else {
				// There is next line
				// {aaaaaaaaaaaaaaaaaaa
				// }bbbbbbbbbbbbbbbbbbb
				mark_min_line__ = EPCBVC_CL;
				mark_max_line__ = NODE_NEXT(EPCBVC_CL);
				mark_min_byte_idx = 0;
				mark_max_byte_idx = 0;
			}
		} else {
			// different line
			if (EPCB_ML->line_num < EPCBVC_CL->line_num) {
				// begin < current
				mark_min_line__ = EPCB_ML;
				mark_max_line__ = EPCBVC_CL;
			} else {
				// current < begin
				mark_min_line__ = EPCBVC_CL;
				mark_max_line__ = EPCB_ML;
			}
			mark_min_byte_idx = 0;
			mark_max_byte_idx = 0;
		}
		break;

	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		if (EPCBVC_CL == EPCB_ML) {
			// current == begin
			mark_min_line__ = EPCBVC_CL;
			mark_max_line__ = EPCBVC_CL;
			if (EPCBVC_CLBI == EPCB_MLBI) {
				// the same line, the same column
				mark_min_byte_idx = EPCBVC_CLBI;
				mark_max_byte_idx = EPCBVC_CLBI;
			} else {
				// the same line, different column
				if (EPCBVC_CLBI < EPCB_MLBI) {
					mark_min_byte_idx = EPCBVC_CLBI;
					mark_max_byte_idx = EPCB_MLBI;
				} else {
					mark_min_byte_idx = EPCB_MLBI;
					mark_max_byte_idx = EPCBVC_CLBI;
				}
			}
		} else {
			// different line
			if (EPCB_ML->line_num < EPCBVC_CL->line_num) {
				// begin < current
				mark_min_line__ = EPCB_ML;
				mark_min_byte_idx = EPCB_MLBI;
				mark_max_line__ = EPCBVC_CL;
				mark_max_byte_idx = EPCBVC_CLBI;
			} else {
				// current < begin
				mark_min_line__ = EPCBVC_CL;
				mark_min_byte_idx = EPCBVC_CLBI;
				mark_max_line__ = EPCB_ML;
				mark_max_byte_idx = EPCB_MLBI;
			}
		}
		break;
	}

	// setup mark_min_col_idx, mark_max_col_idx
	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_NONE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		// mark_min_col_idx and mark_max_col_idx are not used,
		// but calculate for get_columns_selected_in_cut_region()
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		int mark_min_line_col_idx
		 = col_idx_from_byte_idx(mark_min_line__->data, mark_min_byte_idx);
		int mark_max_line_col_idx
		 = col_idx_from_byte_idx(mark_max_line__->data, mark_max_byte_idx);
		if (mark_min_line_col_idx <= mark_max_line_col_idx) {
			mark_min_col_idx = mark_min_line_col_idx;
			mark_max_col_idx = mark_max_line_col_idx;
		} else {
			// swap left and right
			mark_min_col_idx = mark_max_line_col_idx;
			mark_max_col_idx = mark_min_line_col_idx;
		}
		break;
	}
}

int is_there_cut_region()
{
	return (mark_min_line__ != NULL) && (mark_max_line__ != NULL)
	 && ((mark_min_line__ != mark_max_line__) | (mark_min_byte_idx != mark_max_byte_idx));
}
#ifdef ENABLE_DEBUG
void dump_cut_region()
{
	flf_dprintf("%d\n", is_there_cut_region());
	flf_dprintf("%p:%d, %p:%d\n",
	 mark_min_line__, mark_min_byte_idx, mark_max_line__, mark_max_byte_idx);
	flf_dprintf("\n[%s]\n[%s]\n",
	 mark_min_line__->data, mark_max_line__->data);
}
#endif // ENABLE_DEBUG

int get_lines_selected_in_cut_region()
{
	int lines = abs(EPCB_ML->line_num - EPCBVC_CL->line_num);
	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_NONE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		lines++;
		break;
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		break;
	}
	return lines;
}
int get_columns_selected_in_cut_region()
{
	return abs(mark_max_col_idx - mark_min_col_idx);
}

//------------------------------------------------------------------------------

PRIVATE int is_cut_buffers_file_newer();
PRIVATE int is_cut_buffers_modified_newer__expired();
PRIVATE int limit_cut_buffers_in_size();

// Note:
// - older is earlier
// - newer is later in file and buffer
#define CUT_BUF_SEPARATOR_L		(const char*)("{Ｌ}")
#define CUT_BUF_SEPARATOR_C		(const char*)("{Ｃ}")
#define CUT_BUF_SEPARATOR_B		(const char*)("{Ｂ}")
// "{Ｌ}<line cut string>"
// "{Ｃ}<character cut string>"
// "<character cut string>"
//            :
// "{Ｂ}<box cut string>"
// "<box cut string>"
//            :

PRIVATE const char *get_cut_buffer_file_path();

int save_cut_buffers()
{
	const char *file_path = get_cut_buffer_file_path();
/////hmtflf_dprintf("ZZZZSSSS[%s]\n", file_path);
	make_backup_files(file_path, get_backup_files());
	FILE *fp = fopen(file_path, "w");
	if (fp == NULL) {
		return EOF;
	}
	int ret = 0;
	for (be_buf_t* buf = CUT_BUFS_OLDEST_BUF; IS_NODE_INT(buf); buf = CUT_BUF_NEWER(buf)) {
		const char *buf_sep_str = CUT_BUF_SEPARATOR_L;
		switch (GET_BUF_STATE(buf, buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_NONE:
		case CUT_MODE_N_LINE:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:	buf_sep_str = CUT_BUF_SEPARATOR_L;	break;
		case CUT_MODE_H_CHAR:
		case CUT_MODE_VH_CHAR:	buf_sep_str = CUT_BUF_SEPARATOR_C;	break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:	buf_sep_str = CUT_BUF_SEPARATOR_B;	break;
		}
		for (be_line_t* line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
			char buffer[MAX_EDIT_LINE_LEN+1+1];
			snprintf(buffer, MAX_EDIT_LINE_LEN+1+1, "%s%s\n", buf_sep_str, line->data);
			buf_sep_str = "";
			if (fputs(buffer, fp) < 0) {
				ret = EOF;
				break;
			}
		}
		if (ret == EOF)
			break;
	}
	if (fclose(fp)) {
		ret = EOF;
	}
	bufs_get_file_stat(&cut_buffers, file_path);
	bufs_clear_modified(&cut_buffers);
	return ret;
}

int load_cut_buffers()
{
	const char *file_path = get_cut_buffer_file_path();
/////hmtflf_dprintf("ZZZZLLLL[%s]\n", file_path);
	FILE *fp = fopen(file_path, "r");
	if (fp == NULL) {
		return EOF;
	}
	clear_cut_bufs();
	int ret = 0;
	// at the first loop, do add_one_cut_buf() even though no separator line found
	int buf_mode = CUT_MODE_N_LINE;
	for ( ; ; ) {
		char buffer[MAX_EDIT_LINE_LEN+1+1];
		if (fgets(buffer, MAX_EDIT_LINE_LEN+1+1, fp) == NULL) {
			break;
		}
		char *buf_ptr = buffer;
		if ((buf_mode == CUT_MODE_N_LINE) || (buf_mode == CUT_MODE_0_NONE)) {
			if (strlcmp__(buffer, CUT_BUF_SEPARATOR_L) == 0) { buf_mode = CUT_MODE_V_LINE; }
			else if (strlcmp__(buffer, CUT_BUF_SEPARATOR_C) == 0) { buf_mode = CUT_MODE_H_CHAR; }
			else if (strlcmp__(buffer, CUT_BUF_SEPARATOR_B) == 0) { buf_mode = CUT_MODE_HV_BOX; }
		}
		if (buf_mode != CUT_MODE_0_NONE) {
			if ((buf_mode == CUT_MODE_V_LINE)
			 || (buf_mode = CUT_MODE_H_CHAR)
			 || (buf_mode = CUT_MODE_HV_BOX)) {
				// skip buffer separation mark
				buf_ptr += strlen(CUT_BUF_SEPARATOR_L);
			}
			add_one_cut_buf();
			SET_CUR_CBUF_STATE(buf_CUT_MODE, buf_mode);
			buf_mode = CUT_MODE_0_NONE;
		}
		append_string_to_newest_cut_buf(remove_line_tail_lf(buf_ptr));
	}
	if (fclose(fp)) {
		ret = EOF;
	}
	bufs_get_file_stat(&cut_buffers, file_path);
	bufs_clear_modified(&cut_buffers);
	bufs_renumber_all_bufs_from_top(&cut_buffers);

	return ret;
}

void set_cut_buffers_modified()
{
	bufs_set_modified__pending_timer(&cut_buffers);
}
void sync_cut_buffers_if_necessary()
{
	save_cut_buffers_if_local_newer();
	if (load_cut_buffers_if_file_newer()) {
		disp_status_bar_async(_("%s cut-buffer re-loaded"), cur_hhcmmcss());
	}
}
int save_cut_buffers_if_local_newer()
{
	if (is_cut_buffers_modified_newer__expired()) {
		limit_cut_buffers_in_size();
		save_cut_buffers();
		return 1;
	}
	return 0;
}
int load_cut_buffers_if_file_newer()
{
	if (is_cut_buffers_file_newer()) {
		load_cut_buffers();
		return 1;
	}
	return 0;
}

// compare local-cut_buffer and cut_buffer-file
PRIVATE int is_cut_buffers_file_newer()
{
	return bufs_compare_mtime_to_cur_file(&cut_buffers, get_cut_buffer_file_path()) < 0;
}
PRIVATE int is_cut_buffers_modified_newer__expired()
{
	return bufs_is_modified_newer__expired(&cut_buffers,
	 get_cut_buffer_file_path(), BUFFER_EXPIRATION_DSEC);
}
PRIVATE int limit_cut_buffers_in_size()
{
#define MAX_CUT_BUFFERS			100
#define MAX_CUT_BUFFER_LINES	10000
	int buf_cnt = 0;
	int line_cnt = 0;
	for (be_buf_t* buf = CUT_BUFS_NEWEST_BUF; IS_NODE_INT(buf); ) {
		if ((buf_cnt < MAX_CUT_BUFFERS) && (line_cnt < MAX_CUT_BUFFER_LINES)) {
			buf_cnt++;
			line_cnt += buf_count_lines(buf, MAX_CUT_BUFFER_LINES);
			buf = CUT_BUF_OLDER(buf);
		} else {
			buf = buf_unlink_free_ret_prev(buf);
			set_cut_buffers_modified();
		}
	}
flf_dprintf("buf_cnt: %d\n", count_cut_bufs());
	return bufs_get_modified(&cut_buffers);
}
PRIVATE const char *get_cut_buffer_file_path()
{
	static char file_path[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(file_path)) {
		concat_dir_and_file(file_path, get_app_dir(), CUT_BUFFER_FILE_NAME);
	}
	return file_path;
}

// End of editorcut2.c
