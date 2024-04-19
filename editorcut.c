/**************************************************************************
 *   editorcut.c                                                          *
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

// CDPP(Copy Delete Paste Pop)
#define CDPP_COPY		0x01					// copy
#define CDPP_DELETE		0x02					// delete
#define CDPP_PASTE		0x04					// paste
#define CDPP_POP		0x08					// pop after paste
#define CDPP_NOPOP		0x00					// no pop after paste
#define CDPP_CUT		(CDPP_COPY | CDPP_DELETE)	// cut(copy and delete)
#define CDPP_DUPLICATE	(CDPP_COPY | CDPP_PASTE)	// duplicate(copy and paste)
#define CDPP_REPLACE	(CDPP_DELETE | CDPP_PASTE)	// replace(delete and paste)

PRIVATE int copy_delete_paste_pop(int cp_del_paste_pop);
PRIVATE int copy_delete_paste_pop__(int cp_del_paste_pop);

PRIVATE int copy_text_to_cut_buf(void);
PRIVATE int delete_text_in_cut_region(void);
PRIVATE int paste_text_from_cut_buf(void);

PRIVATE void copy_region_to_cut_buf(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx,
 int char_cut);
PRIVATE void delete_region(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx);

PRIVATE void copy_rect_region_to_cut_buf(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx);
PRIVATE void delete_rect_region(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx);

PRIVATE int paste_cut_buf_char(void);
PRIVATE int paste_cut_buf_line(void);
PRIVATE int paste_cut_buf_rect(void);

int doe_clear_cut_buf(void)
{
	free_all_cut_bufs();
	disp_status_bar_done(_("Cut-buffer cleared"));
	return 1;
}
int doe_pop_cut_buf(void)
{
	if (pop_n_free_from_cut_buf() == 0) {
		disp_status_bar_err(_("Cut-buffer empty !!"));
	} else {
		disp_status_bar_done(_("Cut-buffer popped"));
	}
	return 1;
}

int doe_select_all_lines(void)
{
_FLF_
	doe_first_line();
_FLF_
	doe_start_of_line();
_FLF_
	do_set_mark();
_FLF_
	doe_last_line();
_FLF_
	doe_end_of_line();
_FLF_
	return 1;
}

int doe_cut_to_head(void)
{
	if (CEPBV_CLBI == 0) {
		return 0;
	}
	do_set_mark();
	doe_start_of_line();
	doe_delete_text();

	disp_status_bar_done(_("Cut to line head"));
	return 1;
}
int doe_cut_to_tail(void)
{
	if (CEPBV_CLBI == line_data_len(CEPBV_CL)) {
		return 0;
	}
	set_disabled_update_min_text_x_to_keep();	// avoid contents jump around
	do_set_mark();
	doe_end_of_line();
	doe_delete_text();

	disp_status_bar_done(_("Cut to line tail"));
	return 1;
}

int doe_tog_mark(void)
{
	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set ==> set mark
		do_set_mark();
	} else {
		// mark set ==> clear mark
		do_clear_mark();
	}
	return 1;
}
int do_set_mark(void)
{
	do_set_mark_();

	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Mark Set"));
	return 1;
}
void do_set_mark_(void)
{
	CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
	set_mark_pos();
	setup_cut_region();
}
int do_clear_mark(void)
{
	do_clear_mark_();

	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Mark UNset"));
	return 1;
}
void do_clear_mark_(void)
{
	CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_0_LINE;
	clear_mark_pos();
	setup_cut_region();
}

int doe_copy_text(void)
{
_FLF_
	if (copy_delete_paste_pop(CDPP_COPY) <= 0) {
		return 0;
	}
_FLF_
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
_FLF_
	disp_status_bar_done(_("Text Copied to Cut-buffer"));
_FLF_
	return 1;
}

int doe_delete_text(void)
{
	if (copy_delete_paste_pop(CDPP_DELETE) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Deleted"));
	return 1;
}

int doe_cut_text(void)
{
	if (copy_delete_paste_pop(CDPP_CUT) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Cut to Cut-buffer"));
	return 1;
}

int doe_duplicate_text(void)
{
	if (copy_delete_paste_pop(CDPP_DUPLICATE | CDPP_POP) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Duplicated"));
	return 1;
}

int doe_paste_text_with_pop(void)
{
	if (copy_delete_paste_pop(
	 IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0
	  ? (CDPP_PASTE | CDPP_POP) : (CDPP_REPLACE | CDPP_POP)) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Pasted with popping Cut-buffer"));
	return 1;
}
int doe_paste_text_without_pop(void)
{
	if (copy_delete_paste_pop(
	 IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0
	  ? (CDPP_PASTE | CDPP_NOPOP) : (CDPP_REPLACE | CDPP_NOPOP)) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Pasted without popping Cut-buffer"));
	return 1;
}

PRIVATE int copy_delete_paste_pop(int cp_del_paste_pop)
{
	int ret;

	ret = copy_delete_paste_pop__(cp_del_paste_pop);
	do_clear_mark_();	// always clear mark
	return ret;
}
PRIVATE int copy_delete_paste_pop__(int cp_del_paste_pop)
{
	be_line_t *top_line;

	if (cp_del_paste_pop & CDPP_REPLACE) {
		// in view mode, DELETE and PASTE may not be performed
		if (is_view_mode_then_warn_it())
			return -1;		// error
	}
	if ((cp_del_paste_pop & CDPP_COPY) == 0
	 && (cp_del_paste_pop & CDPP_PASTE)) {
		if (count_cut_bufs() == 0) {
			disp_status_bar_err(_("Cut-buffer empty !!"));
			return -1;		// error
		}
	}

	if (IS_MARK_SET(CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set, setup cut-region
		set_mark_pos();
		setup_cut_region();
	}
	top_line = NODE_PREV(mark_min_line);
///_D_(line_dump_lines(top_line, 10, NULL))
	// ====  COPY  ====
	if (cp_del_paste_pop & CDPP_COPY) {
		copy_text_to_cut_buf();
	}
#ifdef ENABLE_UNDO
	if (((cp_del_paste_pop & CDPP_DELETE) && is_there_cut_region())
	 || ((cp_del_paste_pop & CDPP_PASTE) && count_cut_bufs())) {
		// buffer will be modified
		undo_set_region_save_before_change(mark_min_line, mark_max_line,
		 count_cur_cut_buf_lines());
	}
#endif // ENABLE_UNDO
	// ====  DELETE  ====
	if (cp_del_paste_pop & CDPP_DELETE) {
		delete_text_in_cut_region();
	}
	// ====  PASTE  ====
	if (cp_del_paste_pop & CDPP_PASTE) {
		if (count_cut_bufs() == 0) {
			disp_status_bar_err(_("Cut-buffer empty !!"));
		} else {
			paste_text_from_cut_buf();
#ifdef ENABLE_UNDO
///PPP			undo_adjust_max_line();
#endif // ENABLE_UNDO
		}
	}

	clear_disabled_update_min_text_x_to_keep();

	if (cp_del_paste_pop & CDPP_REPLACE) {
		switch (CUR_CBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
			post_cmd_processing(top_line, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		case CUT_MODE_H_CHAR:
		case CUT_MODE_VH_CHAR:
			post_cmd_processing(top_line, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		case CUT_MODE_N_LINE:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:
			post_cmd_processing(top_line, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:
			post_cmd_processing(top_line, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			break;
		}
	}
	// ====  POP  ====
	if (cp_del_paste_pop & CDPP_POP) {
		pop_n_free_from_cut_buf();
	}
	return 1;		// done
}

//-----------------------------------------------------------------------------
PRIVATE int copy_text_to_cut_buf(void)
{
	int char_cut;

	switch (CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		switch (CUR_EBUF_STATE(buf_CUT_MODE)) {
		default:
		case CUT_MODE_0_LINE:
		case CUT_MODE_N_LINE:
		case CUT_MODE_V_LINE:
		case CUT_MODE_HV_LINE:
			char_cut = 0;
			break;
		case CUT_MODE_VH_CHAR:
		case CUT_MODE_H_CHAR:
			char_cut = 1;
			break;
		case CUT_MODE_HV_BOX:
		case CUT_MODE_VH_BOX:
			char_cut = 0;
			break;
		}
		copy_region_to_cut_buf(
		 mark_min_line, mark_min_byte_idx,
		 mark_max_line, mark_max_byte_idx,
		 char_cut);
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		copy_rect_region_to_cut_buf(
		 mark_min_line, mark_min_col_idx,
		 mark_max_line, mark_max_col_idx);
		break;
	}
	save_cut_buf_to_clipboard_file();
	return 1;
}
PRIVATE int delete_text_in_cut_region(void)
{
	set_cur_buf_modified();
	switch (CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		delete_region(
		 mark_min_line, mark_min_byte_idx,
		 mark_max_line, mark_max_byte_idx);
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		delete_rect_region(
		 mark_min_line, mark_min_col_idx,
		 mark_max_line, mark_max_col_idx);
		break;
	}
	return 1;
}
PRIVATE int paste_text_from_cut_buf(void)
{
	switch (CUR_CBUF_STATE(buf_CUT_MODE)) {
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
		paste_cut_buf_char();
		break;
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		paste_cut_buf_line();
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		paste_cut_buf_rect();
		break;
	}
	return 1;
}

//-------------------------------------------------------------------------------------------------
// Ex.1
//  aaaaAAAAAAAAAAAAAaaaa
// Ex.2
//  aaaaAAAAAAAAAAAAAAAAA
//  BBBBbbbbbbbbbbbbbbbbb
PRIVATE void copy_region_to_cut_buf(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx,
 int char_cut)
{
	be_line_t *line;
	char buf[MAX_EDIT_LINE_LEN+1];

	if (min_line == max_line && min_byte_idx == max_byte_idx)
		return;

	push_cut_buf();
	for (line = min_line; IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (line != max_line) {
			// first and intermediate line
			append_string_to_cur_cut_buf(
			 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, line_data_len(line)));
		} else {
			// last line
			if (char_cut ? (min_byte_idx <= max_byte_idx) : (min_byte_idx < max_byte_idx)) {
				append_string_to_cur_cut_buf(
				 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, max_byte_idx));
			}
			break;
		}
		min_byte_idx = 0;
	}
}

// Ex.1
//  [Before]
//  aaaa<BBBBBBBBBBBBB>cccccc
//  [After]
//  aaaacccccc
// Ex.2
//  [Before]
//  aaaaaaaaaaaaaaaaaaaaa
//  <BBBBBBBBBBBBBBBBBBBBB>
//  ccccccccccccccccccccc
//  [After]
//  aaaaaaaaaaaaaaaaaaaaa
//  
//  ccccccccccccccccccccc
// Ex.3
//  [Before]
//  aaaaaaaaaaaaaaaaaaaaa
//  <BBBBBBBBBBBBBBBBBBBBB
//  >ccccccccccccccccccccc
//  [After]
//  aaaaaaaaaaaaaaaaaaaaa
//  ccccccccccccccccccccc
// Ex.4
//  [Before]
//  aaaa<AAAAAAAAAAAAAAAAAAAAA
//  BBBBBBBBBBBBBBBBBBBBBBBBBB
//  CCCCCCCCCCCCCCCCCCCCCCCCCC
//  DDDDDDDDDDDDDDDDDDD>dddddd
//  [After]
//  aaaadddddd
PRIVATE void delete_region(
 be_line_t *min_line, int min_byte_idx,
 be_line_t *max_line, int max_byte_idx)
{
	be_line_t *line, *next;

	if (min_line == max_line && min_byte_idx == max_byte_idx)
		return;

	// adjust current line
	CEPBV_CL = min_line;
	CEPBV_CLBI = min_byte_idx;
	for (line = min_line; ; line = next) {
		next = NODE_NEXT(line);
		if (line == min_line) {
			if (line == max_line) {
				// first and last line
				// >aaaaDDDDbbbb
				line_delete_string(line, min_byte_idx, max_byte_idx - min_byte_idx);
				// >aaaabbbb
				break;
			} else {
				// first line
				// >aaaaDDDD
				//  DDDDDDDD
				//  DDDDbbbb
				line_delete_string(line, min_byte_idx, line_data_len(line) - min_byte_idx);
				// >aaaa
				//  DDDDDDDD
				//  DDDDbbbb
			}
		} else {
			if (line != max_line) {
				// intermediate line ==> delete whole line
				//  aaaa
				// >DDDDDDDD
				//  DDDDbbbb
				line_unlink_free(line);
				//  aaaa
				// >DDDDbbbb
			} else {
				// last line
				//  aaaa
				// >DDDDbbbb
				line_delete_string(line, min_byte_idx, max_byte_idx - min_byte_idx);
				//  aaaa
				// >bbbb
				line_concat_with_next(min_line);
				// >aaaabbbb
				break;
			}
		}
		min_byte_idx = 0;
	}
}

PRIVATE void copy_rect_region_to_cut_buf(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx)
{
	be_line_t *line;
	int min_byte_idx, max_byte_idx;
	char buf[MAX_EDIT_LINE_LEN+1];

	if (min_col_idx == max_col_idx)
		return;

	push_cut_buf();
	for (line = min_line; ; line = NODE_NEXT(line)) {
		min_byte_idx = byte_idx_from_col_idx(line->data, min_col_idx, CHAR_RIGHT, NULL);
		max_byte_idx = byte_idx_from_col_idx(line->data, max_col_idx, CHAR_LEFT, NULL);
		append_string_to_cur_cut_buf(
		 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, max_byte_idx));
		if (line == max_line)
			break;		// copy till the selection bottom ?
	}
}

PRIVATE void delete_rect_region(
 be_line_t *min_line, int min_col_idx,
 be_line_t *max_line, int max_col_idx)
{
	be_line_t *line;
	int min_byte_idx, max_byte_idx;

	if (min_col_idx == max_col_idx)
		return;

	for (line = min_line; ; line = NODE_NEXT(line)) {
		min_byte_idx = byte_idx_from_col_idx(line->data, min_col_idx, CHAR_RIGHT, NULL);
		max_byte_idx = byte_idx_from_col_idx(line->data, max_col_idx, CHAR_LEFT, NULL);
		if (CEPBV_CL == line) {
			CEPBV_CLBI = min_byte_idx;
		}
		get_cep_buf()->buf_size -= (max_byte_idx - min_byte_idx);
		line_delete_string(line, min_byte_idx, max_byte_idx - min_byte_idx);
		if (line == max_line) {
			break;		// delete till the selection bottom ?
		}
	}
}

//-----------------------------------------------------------------------------
// paste data into edit buffer streamlike
// Ex.
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//   CCCC
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   aaaaAAAA
//   BBBB
//   CCCC^bbbb
PRIVATE int paste_cut_buf_char(void)
{
	be_line_t *inserted_line;
	int cur_byte_idx;
	be_line_t *cut_line;

	set_cur_buf_modified();

	cur_byte_idx = CEPBV_CLBI;
	cut_line = CUR_CUT_BUF_TOP_LINE;
	// Paste the first line of the cut-buffer
	// >aaaa^bbbb
	inserted_line = line_separate(CEPBV_CL, cur_byte_idx, INSERT_BEFORE);
	//  aaaa
	// >bbbb
	line_insert_with_string(CEPBV_CL, INSERT_BEFORE, cut_line->data);
	//  aaaa
	//  AAAA
	// >bbbb
	line_concat_with_next(inserted_line);
	//  aaaaAAAA
	// >bbbb
	for ( ; ; ) {
		cut_line = NODE_NEXT(cut_line);
		if (IS_NODE_BOT_ANCH(cut_line))
			break;
		inserted_line = line_insert_with_string(CEPBV_CL, INSERT_BEFORE, cut_line->data);
		//  aaaaAAAA
		//  BBBB
		// >bbbb
		CEPBV_CURSOR_Y++;
	}
	//  aaaaAAAA
	//  BBBB
	//  CCCC
	// >bbbb
	CEPBV_CLBI = line_data_len(inserted_line);
	line_concat_with_prev(CEPBV_CL);
	//  aaaaAAAA
	//  BBBB
	// >CCCC^bbbb
	return 1;		// pasted
}
// paste cut-buffer data line by line
// Ex.
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   AAAA
//   BBBB
//   aaaa^bbbb
PRIVATE int paste_cut_buf_line(void)
{
	be_line_t *cut_line;

	set_cur_buf_modified();

	for (cut_line = CUR_CUT_BUF_TOP_LINE; IS_NODE_INT(cut_line); ) {
		line_insert_with_string(CEPBV_CL, INSERT_BEFORE, cut_line->data);
		cut_line = NODE_NEXT(cut_line);
		if (IS_MARK_SET(CUR_CBUF_STATE(buf_CUT_MODE))) {
			// marked cut/copy
			CEPBV_CURSOR_Y++;
		}
	}
	if (IS_MARK_SET(CUR_CBUF_STATE(buf_CUT_MODE)) == 0) {
		// unmarked cut/copy
		CEPBV_CL = NODE_PREV(CEPBV_CL);
		CEPBV_CLBI = LIM_MAX(CEPBV_CLBI, line_data_len(CEPBV_CL));	// limit cursor pos
	}
	return 1;		// pasted
}
// paste data into edit buffer rectangular
// Ex.1
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//   ccccdddd
//  [AFTER]
//   aaaaAAAAbbbb
//   ccccBBBB^dddd
// Ex.2
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   aaaaAAAAbbbb
//   BBBB^
PRIVATE int paste_cut_buf_rect(void)
{
	int cur_line_col_idx;
	be_line_t *cut_line;

	set_cur_buf_modified();

	cur_line_col_idx = col_idx_from_byte_idx(CEPBV_CL->data, 0, CEPBV_CLBI);
	for (cut_line = CUR_CUT_BUF_TOP_LINE; IS_NODE_INT(cut_line); ) {
		if (IS_NODE_BOT_ANCH(CEPBV_CL)) {
			// if no more lines in edit buffer, append line automatically
			CEPBV_CL = line_insert_with_string(CEPBV_CL, INSERT_BEFORE, "");
		}
		CEPBV_CLBI = byte_idx_from_col_idx(CEPBV_CL->data, cur_line_col_idx,
		 CHAR_LEFT, NULL);
		line_replace_string(CEPBV_CL, CEPBV_CLBI, 0, cut_line->data, -1);
		CEPBV_CLBI += line_data_len(cut_line);
		cut_line = NODE_NEXT(cut_line);
		if (IS_NODE_BOT_ANCH(cut_line))
			break;
		CEPBV_CL = NODE_NEXT(CEPBV_CL);
	}
	return 1;		// pasted
}

// End of editorcut.c
