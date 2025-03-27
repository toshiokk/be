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
be_line_t *mark_min_line;		// the line top of the marked area
int mark_min_byte_idx;			// byte_idx in the line top of the marked area
int mark_min_col_idx;			// col_idx left most in the marked area
// cut region (maximum)
be_line_t *mark_max_line;		// the line bottom of the marked area
int mark_max_byte_idx;			// byte_idx in the line bottom of the marked area
int mark_max_col_idx;			// col_idx right most in the marked area

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
PRIVATE void change_cut_mode_on_mark_region_special_cases(void);

void setup_cut_region_after_cursor_move(cursor_horiz_vert_move_t cursor_move)
{
	if (IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set
		return;
	}
	change_cut_mode_after_cursor_horiz_vert_move(cursor_move);
	setup_cut_region();
	change_cut_mode_on_mark_region_special_cases();
	setup_cut_region();	// cut-mode may be changed, setup mark region again
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}

// [cut-mode transition]
// |previous mode      |event / cursor movement |next mode       |comments                |
// |-------------------|------------------------|----------------|------------------------|
// |CUT_MODE_0_LINE    |start regioning         |CUT_MODE_N_LINE |                        |
// | CUT_MODE_N_LINE   | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |enter HV transition mode|
// | CUT_MODE_N_LINE   | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |enter VH transition mode|
// | CUT_MODE_N_LINE   |return to start position|CUT_MODE_N_LINE |                        |
// |HV transition mode-|------------------------|----------------|------------------------|
// |  CUT_MODE_H_CHAR  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |                        |
// |  CUT_MODE_H_CHAR  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_BOX |if HV_IS_BOX defined    |
// |  CUT_MODE_H_CHAR  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_LINE|if VH_IS_BOX defined    |
// |  CUT_MODE_H_CHAR  |return to start position|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_HV_BOX | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |if HV_IS_BOX defined    |
// |   CUT_MODE_HV_BOX | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_BOX |if HV_IS_BOX defined    |
// |   CUT_MODE_HV_BOX |return to start position|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_HV_LINE| ← → (CURS_MOVE_HORIZ)  |CUT_MODE_H_CHAR |if VH_IS_BOX defined    |
// |   CUT_MODE_HV_LINE| ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_HV_LINE|if VH_IS_BOX defined    |
// |   CUT_MODE_HV_LINE|return to start position|CUT_MODE_N_LINE |                        |
// |VH transition mode-|------------------------|----------------|------------------------|
// |  CUT_MODE_V_LINE  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_CHAR|if HV_IS_BOX defined    |
// |  CUT_MODE_V_LINE  | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_BOX |if VH_IS_BOX defined    |
// |  CUT_MODE_V_LINE  | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |                        |
// |  CUT_MODE_V_LINE  |return to start position|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_VH_CHAR| ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_CHAR|if HV_IS_BOX defined    |
// |   CUT_MODE_VH_CHAR| ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_V_LINE |if HV_IS_BOX defined    |
// |   CUT_MODE_VH_CHAR|return to start position|CUT_MODE_N_LINE |                        |
// |   CUT_MODE_VH_BOX | ← → (CURS_MOVE_HORIZ)  |CUT_MODE_VH_CHAR|if VH_IS_BOX defined    |
// |   CUT_MODE_VH_BOX | ↑ ↓ (CURS_MOVE_VERT)   |CUT_MODE_VH_BOX |if VH_IS_BOX defined    |
// |   CUT_MODE_VH_BOX |return to start position|CUT_MODE_N_LINE |                        |

//
// CUT_MODE_0_LINE:
//  (no move)
// CUT_MODE_N_LINE:
//
// CUT_MODE_N_LINE: (CURS_MOVE_HORIZ)
//   -->
//   CUT_MODE_H_CHAR: (CURS_MOVE_VERT)
//     -->
//       |
//       v
//     CUT_MODE_HV_BOX: (CURS_MOVE_HORIZ)(if HV_IS_BOX defined)
//     CUT_MODE_HV_LINE:(CURS_MOVE_HORIZ)(if VH_IS_BOX defined)
//       -->
//         |
//         v
//         -->
//       CUT_MODE_H_CHAR:
//
// CUT_MODE_N_LINE: (CURS_MOVE_VERT)
//   |
//   v
//   CUT_MODE_V_LINE: (CURS_MOVE_HORIZ)
//     |
//     v
//     -->
//     CUT_MODE_VH_CHAR:(CURS_MOVE_VERT)(if HV_IS_BOX defined)
//     CUT_MODE_VH_BOX: (CURS_MOVE_VERT)(if VH_IS_BOX defined)
//       |
//       v
//       -->
//         |
//         v
//       CUT_MODE_V_LINE:
//

PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t cursor_move)
{
flf_d_printf("cut_mode: %d:%s -->\n",
 GET_CUR_EBUF_STATE(buf_CUT_MODE), buf_cut_mode_str(get_epc_buf()));
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
#ifdef HV_IS_BOX
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_CHAR;
#endif
#ifdef VH_IS_BOX
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
#endif
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
#ifdef HV_IS_BOX
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_BOX;
#endif
#ifdef VH_IS_BOX
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
#endif
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
#ifdef HV_IS_BOX
		GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
#endif
#ifdef VH_IS_BOX
		GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
#endif
		break;
	}
flf_d_printf("  cut_mode: %d:%s\n",
 GET_CUR_EBUF_STATE(buf_CUT_MODE), buf_cut_mode_str(get_epc_buf()));
}
PRIVATE void change_cut_mode_on_mark_region_special_cases(void)
{
	if (EPCBVC_CL == EPCB_ML) {
		// current line == beginning line
		mark_min_line = EPCBVC_CL;
		mark_max_line = EPCBVC_CL;
		if (EPCBVC_CLBI == EPCB_MLBI) {
			// the same line and the same column
			// change cut-mode to initial mode
			GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
		} else {
			// the same line, different column
			switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_LINE:
			case CUT_MODE_N_LINE:
			case CUT_MODE_H_CHAR:
			case CUT_MODE_VH_CHAR:
				break;
			case CUT_MODE_V_LINE:
			case CUT_MODE_HV_LINE:
				// change cut-mode
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
				break;
			case CUT_MODE_HV_BOX:
				// change cut-mode
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
				break;
			case CUT_MODE_VH_BOX:
				// change cut-mode
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
				break;
			}
		}
	}
	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		// different line, the same column
		if (col_idx_from_byte_idx(mark_min_line->data, 0, mark_min_byte_idx)
		 == col_idx_from_byte_idx(mark_max_line->data, 0, mark_max_byte_idx)) {
			switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_LINE:
			case CUT_MODE_N_LINE:
			case CUT_MODE_H_CHAR:
			case CUT_MODE_HV_LINE:
			case CUT_MODE_V_LINE:
			case CUT_MODE_VH_CHAR:
				break;
			case CUT_MODE_HV_BOX:
				// change cut-mode
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
				break;
			case CUT_MODE_VH_BOX:
				// change cut-mode
				GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
				break;
			}
			break;
		}
	}
}
void setup_cut_region(void)
{
	int mark_min_line_col_idx;
	int mark_max_line_col_idx;

	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
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
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		if (EPCBVC_CL == EPCB_ML) {
			// current == begin
			mark_min_line = EPCBVC_CL;
			mark_max_line = EPCBVC_CL;
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
		}
		mark_min_byte_idx = 0;
		mark_max_byte_idx = 0;
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
		mark_min_col_idx = 0;	// not used
		mark_max_col_idx = 0;	// not used
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		mark_min_line_col_idx = col_idx_from_byte_idx(mark_min_line->data, 0, mark_min_byte_idx);
		mark_max_line_col_idx = col_idx_from_byte_idx(mark_max_line->data, 0, mark_max_byte_idx);
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
#if 0
flf_d_printf("EPCB_ML: [%s]\n", EPCB_ML->data);
flf_d_printf("EPCB_MLBI: %d\n", EPCB_MLBI);
flf_d_printf("EPCBVC_CL: [%s]\n", EPCBVC_CL->data);
flf_d_printf("EPCBVC_CLBI: %d\n", EPCBVC_CLBI);
flf_d_printf("mark_min_line: [%s]\n", mark_min_line->data);
flf_d_printf("mark_min_byte_idx: %d\n", mark_min_byte_idx);
flf_d_printf("mark_max_line: [%s]\n", mark_max_line->data);
flf_d_printf("mark_max_byte_idx: %d\n", mark_max_byte_idx);
flf_d_printf("mark_min_col_idx: %d\n", mark_min_col_idx);
flf_d_printf("mark_max_col_idx: %d\n", mark_max_col_idx);
#endif
}

int is_there_cut_region(void)
{
	return (mark_max_line - mark_min_line) | (mark_max_byte_idx - mark_min_byte_idx);
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

//------------------------------------------------------------------------------
#define CUT_BUFFER_SEPARATOR	(const char*)(S_C_L "\n")
#define MAX_CUT_BUFFERS			1000

PRIVATE char *get_cut_buffer_file_path();

int save_cut_buffers()
{
_MFLF_
	int ret = 0;
	FILE *fp = fopen(get_cut_buffer_file_path(), "w");
	if (fp == NULL) {
		return EOF;
	}
	int line_cnt = 0;
	for (be_buf_t* buf = NODES_BOT_NODE(&cut_buffers); IS_NODE_INT(buf); buf = NODE_PREV(buf)) {
		if (line_cnt) {
			if (fwrite(CUT_BUFFER_SEPARATOR, 1, line_data_strlen(CUT_BUFFER_SEPARATOR), fp) < 0) {
				ret = EOF;
				break;
			}
		}
		for (be_line_t* line = NODES_TOP_NODE(buf); IS_NODE_INT(line); line = NODE_NEXT(line)) {
			char buffer[MAX_EDIT_LINE_LEN+1+1];
			snprintf(buffer, MAX_EDIT_LINE_LEN+1+1, "%s\n", line->data);
			if (fwrite(buffer, 1, strnlen(buffer, MAX_EDIT_LINE_LEN+1), fp) < 0) {
				ret = EOF;
				break;
			}
			line_cnt++;
		}
	}
	if (fclose(fp)) {
		ret = EOF;
	}
_MFLF_
	return ret;
}
int load_cut_buffers()
{
_MFLF_
	int ret = 0;
	FILE *fp = fopen(get_cut_buffer_file_path(), "r");
	if (fp == NULL) {
		return EOF;
	}
	int line_cnt = 0;
	for ( ; ; ) {
		char buffer[MAX_EDIT_LINE_LEN+1];
		if (fgets(buffer, MAX_EDIT_LINE_LEN+1, fp) == NULL) {
			break;
		}
		if (strcmp(buffer, CUT_BUFFER_SEPARATOR) == 0) {
			line_cnt = 0;
		} else {
			if (line_cnt == 0) {
				push_cut_buf();
			}
			append_string_to_cur_cut_buf(remove_line_tail_lf(buffer));
			line_cnt++;
		}
	}
	if (fclose(fp)) {
		ret = EOF;
	}

	bufs_renumber_all_bufs_from_top(&cut_buffers);

_MFLF_
	return ret;
}
int limit_cut_buffers()
{
	int buf_cnt = 0;
	for (be_buf_t* buf = NODES_TOP_NODE(&cut_buffers); IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		if (buf_cnt < MAX_CUT_BUFFERS) {
		} else {
			buf = buf_unlink_free(buf);
		}
		buf_cnt++;
	}
flf_d_printf("buf_cnt: %d\n", buf_cnt);
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
