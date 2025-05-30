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

PRIVATE int copy_text_to_cut_buf();
PRIVATE int delete_text_in_cut_region();
PRIVATE int paste_text_from_cut_buf();

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

PRIVATE int paste_cut_buf_char();
PRIVATE int paste_cut_buf_line();
PRIVATE int paste_cut_buf_rect();

int doe_select_all_lines()
{
	doe_first_line();
	doe_start_of_line();
	do_set_mark();
	doe_last_line();
	doe_end_of_line();
	return 1;
}

PRIVATE int doe_cut_to_head_(int delete0_cut1);
int doe_delete_to_head()
{
	return doe_cut_to_head_(0);
}
int doe_cut_to_head()
{
	return doe_cut_to_head_(1);
}
PRIVATE int doe_cut_to_head_(int delete0_cut1)
{
	disp_status_bar_done(_("Delete/Cut to line head"));
	if (EPCBVC_CLBI == 0) {
		return 0;
	}

	// To restore horizontal cursor pos. after undoing
	int prev_byte_idx = EPCBVC_CLBI;
	doe_start_of_line();
	do_set_mark();
	EPCBVC_CLBI = prev_byte_idx;
	setup_cut_region_after_cursor_move(CURS_MOVE_HORIZ);

	if (delete0_cut1 == 0) {
		doe_delete_text();
		disp_status_bar_done(_("Deleted to line head"));
	} else {
		doe_cut_text();
		disp_status_bar_done(_("Cut to line head"));
	}
	return 1;
}
PRIVATE int doe_cut_to_tail_(int delete0_cut1);
int doe_delete_to_tail()
{
	return doe_cut_to_tail_(0);
}
int doe_cut_to_tail()
{
	return doe_cut_to_tail_(1);
}
PRIVATE int doe_cut_to_tail_(int delete0_cut1)
{
	disp_status_bar_done(_("Delete/Cut to line tail"));
	if (EPCBVC_CLBI == line_strlen(EPCBVC_CL)) {
		return 0;
	}
	set_disabled_update_min_text_x_to_keep();	// avoid contents jump around

	// To restore horizontal cursor pos. after undoing
	int prev_byte_idx = EPCBVC_CLBI;
	EPCBVC_CLBI = line_strlen(EPCBVC_CL);
	doe_end_of_line();
	do_set_mark();
	EPCBVC_CLBI = prev_byte_idx;
	setup_cut_region_after_cursor_move(CURS_MOVE_HORIZ);

	if (delete0_cut1 == 0) {
		doe_delete_text();
		disp_status_bar_done(_("Deleted to line tail"));
	} else {
		doe_cut_text();
		disp_status_bar_done(_("Cut to line tail"));
	}
	return 1;
}

int doe_clear_all_cut_buffers()
{
	clear_cut_bufs();
	disp_status_bar_done(_("Cut buffer cleared"));
	return 1;
}

int doe_tog_mark()
{
	if (IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set ==> set mark
		do_set_mark();
	} else {
		// mark set ==> clear mark
		do_clear_mark();
	}
	return 1;
}
int do_set_mark()
{
	do_set_mark_();

	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Mark Set"));
	return 1;
}
void do_set_mark_()
{
	GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_N_LINE;
	set_mark_pos();
}
int do_clear_mark()
{
	do_clear_mark_();

	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Mark UNset"));
	return 1;
}
void do_clear_mark_()
{
	GET_CUR_EBUF_STATE(buf_CUT_MODE) = CUT_MODE_0_LINE;
	clear_mark_pos();
}

int doe_delete_text()
{
	if (copy_delete_paste_pop(CDPP_DELETE) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Deleted"));
	return 1;
}

int doe_cut_text()
{
	if (copy_delete_paste_pop(CDPP_CUT) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Cut to Cut-buffer"));
	return 1;
}

int doe_copy_text()
{
	if (copy_delete_paste_pop(CDPP_COPY) <= 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Text Copied to Cut-buffer"));
	return 1;
}

int doe_cut_text_to_sys_clipboard()
{
	doe_cut_text();
	// copy cut text to clip board file
	save_top_cut_buf_to_clipboard_file();
	send_to_sys_clipboard();
	return 1;
}
int doe_copy_text_to_sys_clipboard()
{
	doe_copy_text();
	// copy cut text to clip board file
	save_top_cut_buf_to_clipboard_file();
	send_to_sys_clipboard();
	return 1;
}
int send_to_sys_clipboard()
{
#define UPDATE_SYS_CLIPBOARD_CMD	"update-system-clipboard.sh"
	if (check_wsl()) {
		tio_set_cursor_pos(central_win_get_status_line_y(), 0);
		return fork_exec_sh_c(EX_FLAGS_0, UPDATE_SYS_CLIPBOARD_CMD);
	}
	return 0;
}

int doe_duplicate_text()
{
	if (copy_delete_paste_pop(CDPP_DUPLICATE | CDPP_POP) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Duplicated"));
	return 1;
}

int doe_paste_text_with_pop()
{
	if (copy_delete_paste_pop(
	 IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE)) == 0
	  ? (CDPP_PASTE | CDPP_POP) : (CDPP_REPLACE | CDPP_POP)) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Pasted with popping Cut-buffer"));
	return 1;
}
int doe_paste_text_without_pop()
{
	if (copy_delete_paste_pop(
	 IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE)) == 0
	  ? (CDPP_PASTE | CDPP_NOPOP) : (CDPP_REPLACE | CDPP_NOPOP)) <= 0) {
		return 0;
	}
	disp_status_bar_done(_("Text Pasted without popping Cut-buffer"));
	return 1;
}

PRIVATE int copy_delete_paste_pop(int cp_del_paste_pop)
{
	int ret = copy_delete_paste_pop__(cp_del_paste_pop);
	do_clear_mark_();	// always clear mark
	return ret;
}
PRIVATE int copy_delete_paste_pop__(int cp_del_paste_pop)
{
	if (cp_del_paste_pop & CDPP_REPLACE) {
		// in view mode, DELETE and PASTE may not be performed
		if (is_editor_unmodifiable_then_warn_it()) {
			return -1;		// error
		}
	}
	if ((cp_del_paste_pop & CDPP_COPY) == 0
	 && (cp_del_paste_pop & CDPP_PASTE)) {
		load_cut_buffers_if_updated();
		if (count_cut_bufs() == 0) {
			disp_status_bar_err(_("Cut-buffer empty !!"));
			return -1;		// error
		}
	}

	if (IS_MARK_SET(GET_CUR_EBUF_STATE(buf_CUT_MODE)) == 0) {
		// no mark set, setup cut-region
		set_mark_pos();
	}
	be_line_t *top_line = NODE_PREV(mark_min_line__);
	// ====  COPY  ====
	if (cp_del_paste_pop & CDPP_COPY) {
		copy_text_to_cut_buf();
	}
#ifdef ENABLE_UNDO
	if (((cp_del_paste_pop & CDPP_DELETE) && is_there_cut_region())
	 || ((cp_del_paste_pop & CDPP_PASTE) && count_cut_bufs())) {
		// buffer will be modified
		undo_set_region__save_before_change(mark_min_line__, mark_max_line__,
		 count_cur_cut_buf_lines());
	}
#endif // ENABLE_UNDO

	UINT8 delete_cut_mode = CUT_MODE_0_LINE;
	UINT8 paste_cut_mode = CUT_MODE_0_LINE;

	// ====  DELETE  ====
	if (cp_del_paste_pop & CDPP_DELETE) {
		delete_cut_mode = GET_CUR_EBUF_STATE(buf_CUT_MODE);
		delete_text_in_cut_region();
	}
	// ====  PASTE  ====
	if (cp_del_paste_pop & CDPP_PASTE) {
		if (count_cut_bufs() == 0) {
			disp_status_bar_err(_("Cut-buffer empty !!"));
		} else {
			paste_cut_mode = GET_CUR_CBUF_STATE(buf_CUT_MODE);
			paste_text_from_cut_buf();
		}
	}
	// ====  POP  ====
	if (cp_del_paste_pop & CDPP_POP) {
		pop__free_from_cut_bufs();
	}

	do_clear_mark_();	// always clear mark
	clear_disabled_update_min_text_x_to_keep();

	// | delete_cut_mode | paste_cut_mode || desirable horizontal cursor positioning |
	// |-----------------|----------------||-----------------|
	// | char            | char           || horizontal move |
	// | char            | line           || vertical move   |
	// | char            | box            || horizontal move |
	// | line            | char           || horizontal move |
	// | line            | line           || vertical move   |
	// | line            | box            || horizontal move |
	// | box             | char           || horizontal move |
	// | box             | line           || vertical move   |
	// | box             | box            || horizontal move |
	if (cp_del_paste_pop & CDPP_REPLACE) {
		if (paste_cut_mode == CUT_MODE_0_LINE) {
			paste_cut_mode = delete_cut_mode;
		}
		switch (paste_cut_mode) {
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
	return 1;		// done
}
//------------------------------------------------------------------------------
PRIVATE int copy_text_to_cut_buf()
{
	int char_cut;

	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
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
		 mark_min_line__, mark_min_byte_idx,
		 mark_max_line__, mark_max_byte_idx,
		 char_cut);
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		copy_rect_region_to_cut_buf(
		 mark_min_line__, mark_min_col_idx,
		 mark_max_line__, mark_max_col_idx);
		break;
	}
	return 1;
}
PRIVATE int delete_text_in_cut_region()
{
	set_cur_buf_modified();
	switch (GET_CUR_EBUF_STATE(buf_CUT_MODE)) {
	default:
	case CUT_MODE_0_LINE:
	case CUT_MODE_N_LINE:
	case CUT_MODE_H_CHAR:
	case CUT_MODE_VH_CHAR:
	case CUT_MODE_V_LINE:
	case CUT_MODE_HV_LINE:
		delete_region(
		 mark_min_line__, mark_min_byte_idx,
		 mark_max_line__, mark_max_byte_idx);
		break;
	case CUT_MODE_HV_BOX:
	case CUT_MODE_VH_BOX:
		delete_rect_region(
		 mark_min_line__, mark_min_col_idx,
		 mark_max_line__, mark_max_col_idx);
		break;
	}
	return 1;
}
PRIVATE int paste_text_from_cut_buf()
{
	switch (GET_CUR_CBUF_STATE(buf_CUT_MODE)) {
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
//------------------------------------------------------------------------------
// e.g.1
//  aaaaAAAAAAAAAAAAAaaaa
// e.g.2
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
			 strcut__(buf, MAX_EDIT_LINE_LEN, line->data, min_byte_idx, line_strlen(line)));
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

// e.g.1
//  [Before]
//  aaaa<BBBBBBBBBBBBB>cccccc
//  [After]
//  aaaacccccc
// e.g.2
//  [Before]
//  aaaaaaaaaaaaaaaaaaaaa
//  <BBBBBBBBBBBBBBBBBBBBB>
//  ccccccccccccccccccccc
//  [After]
//  aaaaaaaaaaaaaaaaaaaaa
//  
//  ccccccccccccccccccccc
// e.g.3
//  [Before]
//  aaaaaaaaaaaaaaaaaaaaa
//  <BBBBBBBBBBBBBBBBBBBBB
//  >ccccccccccccccccccccc
//  [After]
//  aaaaaaaaaaaaaaaaaaaaa
//  ccccccccccccccccccccc
// e.g.4
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
	EPCBVC_CL = min_line;
	EPCBVC_CLBI = min_byte_idx;
	for (line = min_line; ; line = next) {
		next = NODE_NEXT(line);
		if (line == min_line) {
			if (line == max_line) {
				// first and last line
				// >aaaaDDDDbbbb
				line_string_delete(line, min_byte_idx, max_byte_idx - min_byte_idx);
				// >aaaabbbb
				break;
			} else {
				// first line
				// >aaaaDDDD
				//  DDDDDDDD
				//  DDDDbbbb
				line_string_delete(line, min_byte_idx, line_strlen(line) - min_byte_idx);
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
				line_string_delete(line, min_byte_idx, max_byte_idx - min_byte_idx);
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
		if (EPCBVC_CL == line) {
			EPCBVC_CLBI = min_byte_idx;
		}
		get_epc_buf()->buf_size -= (max_byte_idx - min_byte_idx);
		line_string_delete(line, min_byte_idx, max_byte_idx - min_byte_idx);
		if (line == max_line) {
			break;		// delete till the selection bottom ?
		}
	}
}
//------------------------------------------------------------------------------
// paste data into edit buffer streamlike
// e.g.
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
PRIVATE int paste_cut_buf_char()
{
	set_cur_buf_modified();

	be_line_t *cut_line = CUR_CUT_BUF_TOP_LINE;
	// Paste the first line of the cut-buffer
	// >aaaa^bbbb
	be_line_t *new_line = line_separate(EPCBVC_CL, EPCBVC_CLBI, INSERT_BEFORE);
	//  aaaa  <== new_line
	// >bbbb
	line_insert_with_string(EPCBVC_CL, INSERT_BEFORE, cut_line->data);
	//  aaaa  <== new_line
	//  AAAA
	// >bbbb
	line_concat_with_next(new_line);
	//  aaaaAAAA  <== new_line
	// >bbbb
	while (cut_line = NODE_NEXT(cut_line), IS_NODE_INT(cut_line)) {
		new_line = line_insert_with_string(EPCBVC_CL, INSERT_BEFORE, cut_line->data);
		//  aaaaAAAA
		//  BBBB  <== new_line
		// >bbbb
		EPCBVC_CURS_Y++;
	}
	EPCBVC_CLBI = line_strlen(new_line);
	//  aaaaAAAA
	//  BBBB
	//  CCCC^  <== new_line
	// >bbbb
	line_concat_with_prev(EPCBVC_CL);
	//  aaaaAAAA
	//  BBBB
	// >CCCC^bbbb
	return 1;		// pasted
}
// paste cut-buffer data line by line
// e.g.
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   AAAA
//   BBBB
//   aaaa^bbbb
PRIVATE int paste_cut_buf_line()
{
	set_cur_buf_modified();

	for (be_line_t *cut_line = CUR_CUT_BUF_TOP_LINE; IS_NODE_INT(cut_line); ) {
		line_insert_with_string(EPCBVC_CL, INSERT_BEFORE, cut_line->data);
		cut_line = NODE_NEXT(cut_line);
		if (IS_MARK_SET(GET_CUR_CBUF_STATE(buf_CUT_MODE))) {
			// marked cut/copy
			EPCBVC_CURS_Y++;
		}
	}
	if (IS_MARK_SET(GET_CUR_CBUF_STATE(buf_CUT_MODE)) == 0) {
		// unmarked cut/copy
		EPCBVC_CL = NODE_PREV(EPCBVC_CL);
		EPCBVC_CLBI = LIM_MAX(EPCBVC_CLBI, line_strlen(EPCBVC_CL));	// limit cursor pos
	}
	return 1;		// pasted
}
// paste data into edit buffer rectangular
// e.g.1
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//   ccccdddd
//  [AFTER]
//   aaaaAAAAbbbb
//   ccccBBBB^dddd
// e.g.2
//  [CUT-BUFFER]
//   AAAA
//   BBBB
//  [BEFORE]
//   aaaa^bbbb
//  [AFTER]
//   aaaaAAAAbbbb
//   BBBB^
PRIVATE int paste_cut_buf_rect()
{
	set_cur_buf_modified();

	int cur_line_col_idx = col_idx_from_byte_idx(EPCBVC_CL->data, EPCBVC_CLBI);
	for (be_line_t *cut_line = CUR_CUT_BUF_TOP_LINE; IS_NODE_INT(cut_line); ) {
		if (IS_NODE_BOT_ANCH(EPCBVC_CL)) {
			// if no more lines in edit buffer, append line automatically
			EPCBVC_CL = line_insert_with_string(EPCBVC_CL, INSERT_BEFORE, "");
		}
		EPCBVC_CLBI = byte_idx_from_col_idx(EPCBVC_CL->data, cur_line_col_idx,
		 CHAR_LEFT, NULL);
		line_string_replace(EPCBVC_CL, EPCBVC_CLBI, 0, cut_line->data, -1);
		EPCBVC_CLBI += line_strlen(cut_line);
		cut_line = NODE_NEXT(cut_line);
		if (IS_NODE_BOT_ANCH(cut_line))
			break;
		EPCBVC_CL = NODE_NEXT(EPCBVC_CL);
	}
	return 1;		// pasted
}

// End of editorcut.c
