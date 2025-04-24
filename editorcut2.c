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

// mark beginning position
// cut region (minimum)
be_line_t *mark_min_line = NULL;	// the line top of the marked area
int mark_min_byte_idx;				// byte_idx in the line top of the marked area
int mark_min_col_idx;				// col_idx left most in the marked area
// cut region (maximum)
be_line_t *mark_max_line = NULL;	// the line bottom of the marked area
int mark_max_byte_idx;				// byte_idx in the line bottom of the marked area
int mark_max_col_idx;				// col_idx right most in the marked area

//------------------------------------------------------------------------------
void clear_mark_pos(void)
{
	EPCB_ML = NODES_TOP_ANCH(get_epc_buf());
	EPCB_MLBI = 0;
}
void set_mark_pos(void)
{
	EPCB_ML = EPCBVC_CL;
	EPCB_MLBI = EPCBVC_CLBI;
}

PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t cursor_move);

void setup_cut_region_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	if ((is_epc_buf_modifiable() == 0)
	 || (IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE)) == 0)) {
		// no mark set
		return;
	}
	change_cut_mode_after_cursor_horiz_vert_move(cursor_move);
	setup_cut_region();
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
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
		int epcbvc_col_idx = col_idx_from_byte_idx(EPCBVC_CL->data, 0, EPCBVC_CLBI);
		int epcb_col_idx = col_idx_from_byte_idx(EPCB_ML->data, 0, EPCB_MLBI);
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
void setup_cut_region(void)
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
				mark_min_line = EPCBVC_CL;
				mark_max_line = EPCBVC_CL;
				mark_min_byte_idx = 0;
				mark_max_byte_idx = line_strlen(EPCBVC_CL);
			} else {
				// There is next line
				// {aaaaaaaaaaaaaaaaaaa
				// }bbbbbbbbbbbbbbbbbbb
				mark_min_line = EPCBVC_CL;
				mark_max_line = NODE_NEXT(EPCBVC_CL);
				mark_min_byte_idx = 0;
				mark_max_byte_idx = 0;
			}
		} else {
			// different line
			if (EPCB_ML->line_num < EPCBVC_CL->line_num) {
				// begin < current
				mark_min_line = EPCB_ML;
				mark_max_line = EPCBVC_CL;
			} else {
				// current < begin
				mark_min_line = EPCBVC_CL;
				mark_max_line = EPCB_ML;
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
			mark_min_line = EPCBVC_CL;
			mark_max_line = EPCBVC_CL;
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
				mark_min_line = EPCB_ML;
				mark_min_byte_idx = EPCB_MLBI;
				mark_max_line = EPCBVC_CL;
				mark_max_byte_idx = EPCBVC_CLBI;
			} else {
				// current < begin
				mark_min_line = EPCBVC_CL;
				mark_min_byte_idx = EPCBVC_CLBI;
				mark_max_line = EPCB_ML;
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
		 = col_idx_from_byte_idx(mark_min_line->data, 0, mark_min_byte_idx);
		int mark_max_line_col_idx
		 = col_idx_from_byte_idx(mark_max_line->data, 0, mark_max_byte_idx);
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

int is_there_cut_region(void)
{
	return (mark_min_line != NULL) && (mark_max_line != NULL)
	 && ((mark_min_line != mark_max_line) | (mark_min_byte_idx != mark_max_byte_idx));
}

int lines_selected(void)
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
int columns_selected(void)
{
	return abs(mark_max_col_idx - mark_min_col_idx);
}

//------------------------------------------------------------------------------
#define CUT_BUF_SEPARATOR_L		(const char*)(S_C_L "\n")
#define CUT_BUF_SEPARATOR_C		(const char*)(S_C_C "\n")
#define CUT_BUF_SEPARATOR_B		(const char*)(S_C_B "\n")
#define MAX_CUT_BUFFERS			1000
// "^L"
// "line cut strings"
// "^C"
// "character cut strings"
// "^B"
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
			if (strcmp(buffer, CUT_BUF_SEPARATOR_C) == 0) { buf_mode = CUT_MODE_VH_CHAR; }
			if (strcmp(buffer, CUT_BUF_SEPARATOR_B) == 0) { buf_mode = CUT_MODE_VH_BOX; }
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
	if (buf_has_orig_file_updated(NODES_BOT_ANCH(&cut_buffers), get_cut_buffer_file_path())) {
		load_cut_buffers();
	}
}
/////void save_cut_buffers_if_modified_and_expired()
/////{
/////	if (buf_get_modified(NODES_BOT_ANCH(&cut_buffers))
/////	 && buf_check_save_pending_timer(NODES_BOT_ANCH(&cut_buffers), BUFFER_EXPIRATION_MSEC)) {
/////		save_cut_buffers();
/////_MFLF_
/////	}
/////}
void save_cut_buffers_if_modified()
{
	if (buf_get_modified(NODES_BOT_ANCH(&cut_buffers))) {
		save_cut_buffers();
	}
}
int limit_cut_buffers()
{
	int buf_cnt = 0;
	for (be_buf_t* buf = NODES_TOP_NODE(&cut_buffers); IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		if (buf_cnt >= MAX_CUT_BUFFERS) {
			buf = buf_unlink_free(buf);
		}
		buf_cnt++;
	}
	return buf_cnt;
}
PRIVATE char *get_cut_buffer_file_path()
{
	static char file_path[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(file_path)) {
		snprintf_(file_path, MAX_PATH_LEN+1, "%s/%s", get_app_dir(), CUT_BUFFER_FILE_NAME);
	}
	return file_path;
}

// End of editorcut2.c
