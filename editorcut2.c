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
// |previous mode      |event / cursor movement |next mode       |comments                |
// |-------------------|------------------------|----------------|------------------------|
// |CUT_MODE_0_LINE    |start regioning         |CUT_MODE_N_LINE |                        |
// | CUT_MODE_N_LINE   | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |enter HV transition mode|
// | CUT_MODE_N_LINE   | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |enter VH transition mode|
// | CUT_MODE_N_LINE   |return to start col/line|CUT_MODE_N_LINE |                        |
// |HV transition mode-|------------------------|----------------|------------------------|
// |  CUT_MODE_H_CHAR  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |                        |
// |  CUT_MODE_H_CHAR  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_LINE|                        |
// |  CUT_MODE_H_CHAR  |return to start col/line|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_HV_BOX |return to start col/line|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_HV_LINE| ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |                        |
// |   CUT_MODE_HV_LINE| ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_LINE|                        |
// |   CUT_MODE_HV_LINE|return to start col/line|CUT_MODE_N_LINE |                        |
// |VH transition mode-|------------------------|----------------|------------------------|
// |  CUT_MODE_V_LINE  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_BOX |                        |
// |  CUT_MODE_V_LINE  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |                        |
// |  CUT_MODE_V_LINE  |return to start col/line|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_VH_CHAR|return to start col/line|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_VH_BOX | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_CHAR|                        |
// |   CUT_MODE_VH_BOX | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_VH_BOX |                        |
// |   CUT_MODE_VH_BOX |return to start col/line|CUT_MODE_N_LINE |                        |

//
// CUT_MODE_0_LINE:
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
// CUT_MODE_0_LINE:
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
			case CUT_MODE_0_LINE:
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
			case CUT_MODE_0_LINE:
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
		case CUT_MODE_0_LINE:
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
		case CUT_MODE_0_LINE:
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
	case CUT_MODE_0_LINE:
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
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		// mark_min_col_idx and mark_max_col_idx are not used, but calculate for columns_selected()
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
	flf_d_printf("%d\n", is_there_cut_region());
	flf_d_printf("%p:%d, %p:%d\n",
	 mark_min_line__, mark_min_byte_idx, mark_max_line__, mark_max_byte_idx);
	flf_d_printf("\n[%s]\n[%s]\n",
	 mark_min_line__->data, mark_max_line__->data);
}
#endif // ENABLE_DEBUG

int lines_selected()
{
	int lines = abs(EPCB_ML->line_num - EPCBVC_CL->line_num);
	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
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
int columns_selected()
{
	return abs(mark_max_col_idx - mark_min_col_idx);
}

//------------------------------------------------------------------------------
#define CUT_BUF_SEPARATOR_L		(const char*)("Ｌ\n")
#define CUT_BUF_SEPARATOR_C		(const char*)("Ｃ\n")
#define CUT_BUF_SEPARATOR_B		(const char*)("Ｂ\n")
// "Ｌ^L"
// "line cut strings"
// "Ｃ^C"
// "character cut strings"
// "Ｂ^B"
// "box cut strings"

PRIVATE char *get_cut_buffer_file_path();

int save_cut_buffers()
{
	int ret = 0;
	FILE *fp = fopen(get_cut_buffer_file_path(), "w");
	if (fp == NULL) {
		return EOF;
	}
	for (be_buf_t* buf = NODES_BOT_NODE(&cut_buffers); IS_NODE_INT(buf); buf = NODE_PREV(buf)) {
		const char* buf_sep_str = CUT_BUF_SEPARATOR_L;
		switch (GET_BUF_STATE(buf, buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
		case CUT_MODE_N_LINE:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:	buf_sep_str = CUT_BUF_SEPARATOR_L;	break;
		case CUT_MODE_H_CHAR:
		case CUT_MODE_VH_CHAR:	buf_sep_str = CUT_BUF_SEPARATOR_C;	break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:	buf_sep_str = CUT_BUF_SEPARATOR_B;	break;
		}
		if (fwrite(buf_sep_str, 1, line_data_strlen(buf_sep_str), fp) < 0) {
			ret = EOF;
			break;
		}
		for (be_line_t* line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
			char buffer[MAX_EDIT_LINE_LEN+1+1];
			snprintf(buffer, MAX_EDIT_LINE_LEN+1+1, "%s\n", line->data);
			if (fwrite(buffer, 1, strnlen(buffer, MAX_EDIT_LINE_LEN+1), fp) < 0) {
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
	buf_get_file_stat(NODES_BOT_ANCH(&cut_buffers), get_cut_buffer_file_path());
	buf_clear_buf_modified(NODES_BOT_ANCH(&cut_buffers));
	return ret;
}
int load_cut_buffers()
{
	int ret = 0;
	FILE *fp = fopen(get_cut_buffer_file_path(), "r");
	if (fp == NULL) {
		return EOF;
	}
	int buf_mode = CUT_MODE_N_LINE;		// at the first loop, push_cut_buf() shall be called
	for ( ; ; ) {
		char buffer[MAX_EDIT_LINE_LEN+1];
		if (fgets(buffer, MAX_EDIT_LINE_LEN+1, fp) == NULL) {
			break;
		}
		if ((buf_mode == CUT_MODE_N_LINE) || (buf_mode == CUT_MODE_0_LINE)) {
			if (strcmp(buffer, CUT_BUF_SEPARATOR_L) == 0) { buf_mode = CUT_MODE_HV_LINE; }
			else if (strcmp(buffer, CUT_BUF_SEPARATOR_C) == 0) { buf_mode = CUT_MODE_VH_CHAR; }
			else if (strcmp(buffer, CUT_BUF_SEPARATOR_B) == 0) { buf_mode = CUT_MODE_VH_BOX; }
		}
		if (buf_mode != CUT_MODE_0_LINE) {
			if (buf_mode != CUT_MODE_N_LINE) {
				strcpy(buffer, "");
			}
			push_cut_buf();
			SET_BUF_STATE(CUT_BUFS_TOP_BUF, buf_CUT_MODE, buf_mode);
			buf_mode = CUT_MODE_0_LINE;
		}
		if (is_strlen_not_0(buffer)) {
			append_string_to_cur_cut_buf(remove_line_tail_lf(buffer));
		}
	}
	if (fclose(fp)) {
		ret = EOF;
	}
	buf_get_file_stat(NODES_BOT_ANCH(&cut_buffers), get_cut_buffer_file_path());
	buf_clear_buf_modified(NODES_BOT_ANCH(&cut_buffers));

	bufs_renumber_all_bufs_from_top(&cut_buffers);

	return ret;
}

void set_cut_buffers_modified()
{
	buf_set_modified(NODES_BOT_ANCH(&cut_buffers));
}
void load_cut_buffers_if_updated()
{
	if (is_cut_buffers_updated()) {
		load_cut_buffers();
	}
}
void save_cut_buffers_if_modified()
{
	if (is_cut_buffers_modified()) {
		save_cut_buffers();
	}
}
void save_cut_buffers_if_modified_or_limited()
{
	// | updated | modified | limited || saving |
	// |---------|----------|---------||--------|
	// |    1    |     *    |    *    ||  No    | keep other process's change
	// |    0    |     0    |    0    ||  No    | no need of saving
	// |    0    |     0    |    1    ||  Yes   | save reduced buffers
	// |    1    |     1    |    0    ||  Yes   | save modified buffers
	// |    1    |     1    |    1    ||  Yes   | save
	if ((is_cut_buffers_updated() == 0)
	 && (is_cut_buffers_modified() || limit_cut_buffers_in_size())) {
		save_cut_buffers();
	}
}
int is_cut_buffers_updated()
{
	return buf_has_orig_file_updated(NODES_BOT_ANCH(&cut_buffers), get_cut_buffer_file_path()) > 0;
}
int is_cut_buffers_modified()
{
	return buf_get_modified(NODES_BOT_ANCH(&cut_buffers));
}
int limit_cut_buffers_in_size()
{
#define MAX_CUT_BUFFERS			100
#define MAX_CUT_BUFFER_LINES	1000
	int buf_cnt = 0;
	int line_cnt = 0;
	for (be_buf_t* buf = NODES_TOP_NODE(&cut_buffers); IS_NODE_INT(buf); ) {
/////flf_d_printf("buf_cnt: %d, line_cnt: %d\n", buf_cnt, line_cnt);
		if ((buf_cnt < MAX_CUT_BUFFERS) && (line_cnt < MAX_CUT_BUFFER_LINES)) {
			buf_cnt++;
			line_cnt += buf_count_lines(buf, MAX_CUT_BUFFER_LINES);
			buf = NODE_NEXT(buf);
		} else {
			buf = buf_unlink_free(buf);
			set_cut_buffers_modified();
		}
	}
flf_d_printf("buf_cnt: %d\n", count_cut_bufs());
	return buf_get_modified(NODES_BOT_ANCH(&cut_buffers));
}
PRIVATE char *get_cut_buffer_file_path()
{
	static char file_path[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(file_path)) {
		cat_dir_and_file(file_path, get_app_dir(), CUT_BUFFER_FILE_NAME);
	}
	return file_path;
}

// End of editorcut2.c
