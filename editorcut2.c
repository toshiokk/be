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

//-----------------------------------------------------------------------------
void clear_mark_pos(void)
{
	CEB_ML = BUF_TOP_ANCH(get_c_e_b());
	CEB_MLBI = 0;
}
void set_mark_pos(void)
{
	CEB_ML = CEBV_CL;
	CEB_MLBI = CEBV_CLBI;
}
// CEB_MLBI <==> CEBV_CLBI
void swap_m_l_b_i_with_c_l_b_i(void)
{
	int c_l_b_i;

	if (CEB_ML == CEBV_CL) {
		c_l_b_i = CEB_MLBI;
		CEB_MLBI = CEBV_CLBI;
		CEBV_CLBI = c_l_b_i;
	}
}

PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t move);
PRIVATE void change_cut_mode_on_mark_region_special_cases(void);

void setup_cut_region_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t move)
{
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set
		return;
	}
	change_cut_mode_after_cursor_horiz_vert_move(move);
	setup_cut_region();
	change_cut_mode_on_mark_region_special_cases();
	setup_cut_region();	// cut-mode may be changed, setup mark region again
	set_edit_win_update_needed(UPDATE_SCRN_ALL);
}
// [cut-mode transition]
//
// CUT_MODE_0_LINE:
// CUT_MODE_N_LINE:
//  (no move)
//
// CUT_MODE_H_CHAR:
//  ====>
//
// CUT_MODE_HV_LINE:
// CUT_MODE_HV_BOX:
//  ====>
//      |
//      v
//
// CUT_MODE_H_CHAR:
//  ====>
//      |
//      v
//      ====>
//
// CUT_MODE_V_LINE:
//  |
//  v
//
// CUT_MODE_VH_CHAR:
// CUT_MODE_VH_BOX:
//  |
//  v
//  ====>
//
// CUT_MODE_V_LINE:
//  |
//  v
//  ====>
//      |
//      v
//
PRIVATE void change_cut_mode_after_cursor_horiz_vert_move(cursor_horiz_vert_move_t move)
{
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
		break;
	case CUT_MODE_N_LINE:
		if (move == HORIZ_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (move == VERT_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	case CUT_MODE_H_CHAR:
		if (move == HORIZ_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (move == VERT_MOVE) {
#ifdef HV_IS_BOX_VH_IS_CHAR
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_BOX;
#else
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
#endif
		}
		break;
	case CUT_MODE_HV_LINE:
		if (move == HORIZ_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (move == VERT_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_LINE;
		}
		break;
	case CUT_MODE_V_LINE:
		if (move == HORIZ_MOVE) {
#ifdef HV_IS_BOX_VH_IS_CHAR
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_CHAR;
#else
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
#endif
		} else if (move == VERT_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	case CUT_MODE_VH_CHAR:
		if (move == HORIZ_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_CHAR;
		} else if (move == VERT_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	case CUT_MODE_HV_BOX:
		if (move == HORIZ_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
		} else if (move == VERT_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_HV_BOX;
		}
		break;
	case CUT_MODE_VH_BOX:
		if (move == HORIZ_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_VH_BOX;
		} else if (move == VERT_MOVE) {
			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
		}
		break;
	}
}
PRIVATE void change_cut_mode_on_mark_region_special_cases(void)
{
	if (CEBV_CL == CEB_ML) {
		// current == begin
		mark_min_line = CEBV_CL;
		mark_max_line = CEBV_CL;
		if (CEBV_CLBI == CEB_MLBI) {
			// the same line, the same column
			// change cut-mode to initial mode
///			// cut-mode is not initial mode, set to initial mode
///			CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
		} else {
			// the same line, different column
			switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
			default:
			case CUT_MODE_0_LINE:
			case CUT_MODE_N_LINE:
			case CUT_MODE_H_CHAR:
			case CUT_MODE_VH_CHAR:
				break;
			case CUT_MODE_V_LINE:
			case CUT_MODE_HV_LINE:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
				break;
			case CUT_MODE_HV_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
				break;
			case CUT_MODE_VH_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
				break;
			}
		}
	}
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
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
			switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
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
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_H_CHAR;
				break;
			case CUT_MODE_VH_BOX:
				// change cut-mode
				CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_V_LINE;
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

	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
		if (IS_NODE_BOT(CEBV_CL)) {
			// No next line
			// {aaaaaaaaaaaaaaaaaa}
			mark_min_line = CEBV_CL;
			mark_max_line = CEBV_CL;
			mark_min_byte_idx = 0;
			mark_max_byte_idx = line_data_len(CEBV_CL);
		} else {
			// There is next line
			// {aaaaaaaaaaaaaaaaaaa
			// }bbbbbbbbbbbbbbbbbbb
			mark_min_line = CEBV_CL;
			mark_max_line = CEBV_CL->next;
			mark_min_byte_idx = 0;
			mark_max_byte_idx = 0;
		}
		break;
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		if (CEBV_CL == CEB_ML) {
			// current == begin
			mark_min_line = CEBV_CL;
			mark_max_line = CEBV_CL;
			if (CEBV_CLBI == CEB_MLBI) {
				// the same line, the same column
				mark_min_byte_idx = CEBV_CLBI;
				mark_max_byte_idx = CEBV_CLBI;
			} else {
				// the same line, different column
				if (CEBV_CLBI < CEB_MLBI) {
					mark_min_byte_idx = CEBV_CLBI;
					mark_max_byte_idx = CEB_MLBI;
				} else {
					mark_min_byte_idx = CEB_MLBI;
					mark_max_byte_idx = CEBV_CLBI;
				}
			}
		} else {
			// different line
			if (CEB_ML->line_num < CEBV_CL->line_num) {
				// begin < current
				mark_min_line = CEB_ML;
				mark_min_byte_idx = CEB_MLBI;
				mark_max_line = CEBV_CL;
				mark_max_byte_idx = CEBV_CLBI;
			} else {
				// current < begin
				mark_min_line = CEBV_CL;
				mark_min_byte_idx = CEBV_CLBI;
				mark_max_line = CEB_ML;
				mark_max_byte_idx = CEB_MLBI;
			}
		}
		break;
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		if (CEBV_CL == CEB_ML) {
			// current == begin
			mark_min_line = CEBV_CL;
			mark_max_line = CEBV_CL;
		} else {
			// different line
			if (CEB_ML->line_num < CEBV_CL->line_num) {
				// begin < current
				mark_min_line = CEB_ML;
				mark_max_line = CEBV_CL;
			} else {
				// current < begin
				mark_min_line = CEBV_CL;
				mark_max_line = CEB_ML;
			}
		}
		mark_min_byte_idx = 0;
		mark_max_byte_idx = 0;
		break;
	}

	// setup mark_min_col_idx, mark_max_col_idx
	switch(CUR_EBUF_STATE(buf_CUT_MODE)) {
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
flf_d_printf("CEB_ML: [%s]\n", CEB_ML->data);
flf_d_printf("CEB_MLBI: %d\n", CEB_MLBI);
flf_d_printf("CEBV_CL: [%s]\n", CEBV_CL->data);
flf_d_printf("CEBV_CLBI: %d\n", CEBV_CLBI);
flf_d_printf("mark_min_line: [%s]\n", mark_min_line->data);
flf_d_printf("mark_min_byte_idx: %d\n", mark_min_byte_idx);
flf_d_printf("mark_max_line: [%s]\n", mark_max_line->data);
flf_d_printf("mark_max_byte_idx: %d\n", mark_max_byte_idx);
flf_d_printf("mark_min_col_idx: %d\n", mark_min_col_idx);
flf_d_printf("mark_max_col_idx: %d\n", mark_max_col_idx);
}

int is_there_cut_region(void)
{
	return (mark_max_line - mark_min_line) | (mark_max_byte_idx - mark_min_byte_idx);
}

// End of editorcut2.c
