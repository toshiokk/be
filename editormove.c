/**************************************************************************
 *   editormove.c                                                         *
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

PRIVATE easy_buffer_switching_t easy_buffer_switching = 0;
PRIVATE char easy_buffer_switching_count_ = 0;

void easy_buffer_switching_clear()
{
	easy_buffer_switching = EBS_NONE;
	easy_buffer_switching_count_ = 0;
}
void easy_buffer_switching_count()
{
	if (easy_buffer_switching != EBS_NONE) {
		easy_buffer_switching_count_++;
		if (easy_buffer_switching_count_ >= 2) {
			easy_buffer_switching_clear();
		}
	}
}
int easy_buffer_switching_check(easy_buffer_switching_t top_bottom)
{
	switch (easy_buffer_switching) {
	default:
	case EBS_NONE:
		break;
	case EBS_UP_AT_TOP:
		if (top_bottom == EBS_PAGEUP_AT_TOP) {
			easy_buffer_switching_clear();
			return 1;
		}
		break;
	case EBS_PAGEUP_AT_TOP:
		if (top_bottom == EBS_UP_AT_TOP) {
			easy_buffer_switching_clear();
			return 1;
		}
		break;
	case EBS_DOWN_AT_BOTTOM:
		if (top_bottom == EBS_PAGEDOWN_AT_BOTTOM) {
			easy_buffer_switching_clear();
			return 1;
		}
		break;
	case EBS_PAGEDOWN_AT_BOTTOM:
		if (top_bottom == EBS_DOWN_AT_BOTTOM) {
			easy_buffer_switching_clear();
			return 1;
		}
		break;
	}
	easy_buffer_switching = top_bottom;
	easy_buffer_switching_count_ = 0;
	return 0;
}

//-----------------------------------------------------------------------------
int doe_left(void)
{
	if (is_app_list_mode()) {
		return doe_switch_to_prev_buffer();
	}
	move_cursor_left(1);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int doe_right(void)
{
	if (is_app_list_mode()) {
		return doe_switch_to_next_buffer();
	}
	move_cursor_right();

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

PRIVATE int get_char_type(char chr)
{
/* '\0':0, space:1, alpha-numeric:2, others:3, UTF-8:4 */
#define CHR_TYPE(chr)	((chr) == '\0' ? 0 :							\
						 (is_char_white_space(chr) ? 1 :				\
						  (is_char_id(chr) ? 2 :						\
						    (ispunct(chr) ? (chr) :						\
						     (((unsigned char)(chr)) < 0x80 ? 3 : 4)	\
						    )											\
						  )												\
						 )												\
						)
	return CHR_TYPE(chr);
}

// go to previous word
int doe_prev_word(void)
{
	if (EPCBVC_CLBI > 0) {
		// is head of the word ?
		if (get_char_type(EPCBVC_CL->data[EPCBVC_CLBI-1])
		  == get_char_type(EPCBVC_CL->data[EPCBVC_CLBI])) {
			// not head of the word
		} else {
			// head of the word
			move_cursor_left(1);
		}
		while (EPCBVC_CLBI > 0
		 && (get_char_type(EPCBVC_CL->data[EPCBVC_CLBI-1])
		  == get_char_type(EPCBVC_CL->data[EPCBVC_CLBI])) ) {
			move_cursor_left(1);
		}
	} else {
		// if top of the line, move to the end of the previous line
		move_cursor_left(1);
	}

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 0;
}
// go to next word
int doe_next_word(void)
{
	int chr_type;

	if (EPCBVC_CL->data[EPCBVC_CLBI]) {
		// skip current block
		chr_type = get_char_type(EPCBVC_CL->data[EPCBVC_CLBI]);
		// move to the beginning of the next block
		while (EPCBVC_CL->data[EPCBVC_CLBI]
		 && chr_type == get_char_type(EPCBVC_CL->data[EPCBVC_CLBI]))
			move_cursor_right();
	} else {
		// if end of the line, move to the top of the next line
		move_cursor_right();
	}

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 0;
}

int doe_start_of_line(void)
{
	EPCBVC_CLBI = 0;

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}
int doe_end_of_line(void)
{
	EPCBVC_CLBI = line_data_strlen(EPCBVC_CL);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

//-----------------------------------------------------------------------------
PRIVATE void doe_up_(void);
int doe_up(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		doe_up_();
	} else {
		tog_editor_panex();
		doe_up_();
		tog_editor_panex();
		doe_up_();
	}
	return 1;
}
PRIVATE void doe_up_(void)
{
	if (cur_line_up(&EPCBVC_CL, &EPCBVC_CLBI)) {
		EPCBVC_CURS_Y--;
	} else {
		if (easy_buffer_switching_check(EBS_UP_AT_TOP) == 0) {
			disp_status_bar_done(_("No previous lines"));
		} else {
			// already top of buffer, go to the previous buffer's last line
			doe_switch_to_prev_buffer();
		}
	}
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
PRIVATE void doe_down_(void);
int doe_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		doe_down_();
	} else {
		tog_editor_panex();
		doe_down_();
		tog_editor_panex();
		doe_down_();
	}
	return 1;
}
PRIVATE void doe_down_(void)
{
	if (cur_line_down(&EPCBVC_CL, &EPCBVC_CLBI)) {
		EPCBVC_CURS_Y++;
	} else {
		if (easy_buffer_switching_check(EBS_DOWN_AT_BOTTOM) == 0) {
			disp_status_bar_done(_("No next lines"));
		} else {
			// already bottom of buffer, go to the next buffer's top line
			doe_switch_to_next_buffer();
		}
	}
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}

PRIVATE void doe_page_up_(void);
int doe_page_up(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		doe_page_up_();
	} else {
		tog_editor_panex();
		doe_page_up_();
		tog_editor_panex();
		doe_page_up_();
	}
	return 1;
}
PRIVATE void doe_page_up_(void)
{
	int lines;
	int cnt;

	if (cur_line_up(&EPCBVC_CL, &EPCBVC_CLBI)) {
#define GO_CONST_LINES
#ifdef GO_CONST_LINES
		lines = editor_vert_scroll_lines() - 1;
#else
		lines = (EPCBVC_CURS_Y - top_scroll_margin_y()) + editor_vert_scroll_lines() - 1;
#endif
		for (cnt = 0; cnt < lines; cnt++) {
			EPCBVC_CURS_Y--;
			if (cur_line_up(&EPCBVC_CL, &EPCBVC_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	} else {
		if (easy_buffer_switching_check(EBS_PAGEUP_AT_TOP) == 0) {
			disp_status_bar_done(_("No previous lines"));
		} else {
			// already top of buffer, go to the previous buffer's last line
			doe_switch_to_prev_buffer();
		}
	}
}

PRIVATE int doe_page_down_(void);
int doe_page_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		doe_page_down_();
	} else {
		tog_editor_panex();
		doe_page_down_();
		tog_editor_panex();
		doe_page_down_();
	}
	return 1;
}
PRIVATE int doe_page_down_(void)
{
	int lines;
	int cnt;

	if (cur_line_down(&EPCBVC_CL, &EPCBVC_CLBI)) {
#ifdef GO_CONST_LINES
		lines = editor_vert_scroll_lines() - 1;
#else
		lines = (bottom_scroll_margin_y() - EPCBVC_CURS_Y) + editor_vert_scroll_lines() - 1;
#endif
		for (cnt = 0; cnt < lines; cnt++) {
			EPCBVC_CURS_Y++;
			if (cur_line_down(&EPCBVC_CL, &EPCBVC_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	} else {
		if (easy_buffer_switching_check(EBS_PAGEDOWN_AT_BOTTOM) == 0) {
			disp_status_bar_done(_("No next lines"));
		} else {
			// already bottom of buffer, go to the next buffer's top line
			doe_switch_to_next_buffer();
		}
	}
	return 1;
}

int doe_first_line(void)
{
	first_line();
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
	return 1;
}
int doe_last_line(void)
{
	last_line();
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_BOTTOM, UPDATE_SCRN_ALL);
	return 1;
}

PRIVATE int do_enter_char(char chr);
PRIVATE int do_enter_utf8s(const char *utf8s);
PRIVATE int do_enter_utf8c(const char *utf8c);
int doe_control_code(void)
{
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	disp_status_bar_ing(_("Input control character [^A-^Z,^[,^\\,^],^^,^_,\x7f]"));
	key_code_t key = input_key_loop();
	disp_status_bar_ing(_("Key code: %04x"), key);
	if ((0x01 <= key && key < 0x20) || key == CHAR_DEL) {
		do_enter_char(key);
	}
	return 1;
}
int doe_charcode(void)
{
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	char string[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", string, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 _("Enter Unicode number in hex:")))) {
		return 0;
	}
	unsigned int chr;
	if (sscanf(string, "%x", &chr) == 1) {
		char utf8c[MAX_UTF8C_BYTES+1];
		utf8c_encode(chr, utf8c);
		do_enter_utf8s(utf8c);
		return 1;
	}
	return 0;
}

int doe_paste_from_history(void)
{
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	char string[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_editor(input_string_pos("", string, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 _("Select history string to paste:")))) {
		return 0;
	}
	do_enter_utf8s(string);
	return 0;
}

int doe_tab(void)
{
	return do_enter_char('\t');
}

#define UTF8S_SEND_LEN				(MAX_UTF8C_BYTES * 100)
#define UTF8S_SEND_BUF_LEN			(UTF8S_SEND_LEN + MAX_UTF8C_BYTES * 1)
PRIVATE int utf8s_send_buf_bytes = 0;
PRIVATE char utf8s_send_buf[UTF8S_SEND_BUF_LEN+1] = "";
int doe_buffer_utf8c_bytes(char chr)
{
	static char utf8c_state = 0;

	utf8c_state = utf8c_len(utf8c_state, chr);
flf_d_printf("<%02x>\n", (unsigned char)chr);
	// put to send buffer
	utf8s_send_buf[utf8s_send_buf_bytes++] = chr;
	utf8s_send_buf[utf8s_send_buf_bytes] = '\0';
	if ((utf8c_state == 0) && (utf8s_send_buf_bytes >= UTF8S_SEND_LEN)) {
		doe_enter_buffered_utf8c_bytes();
	}
	return 1;
}
int doe_enter_buffered_utf8c_bytes(void)
{
	if (utf8s_send_buf_bytes == 0) {
		return 0;	// no character sent
	}
	do_enter_utf8s(utf8s_send_buf);
	// clear send buffer
	utf8s_send_buf_bytes = 0;
	utf8s_send_buf[utf8s_send_buf_bytes] = '\0';
	return 1;	// sent
}
PRIVATE int do_enter_char(char chr)
{
	char utf8s[1+1];
	utf8s[0] = chr;
	utf8s[1] = '\0';
	return do_enter_utf8s(utf8s);
}
PRIVATE int do_enter_utf8s(const char *utf8s)
{
	int bytes_str;
	int bytes_chr;
	char utf8c[MAX_UTF8C_BYTES+1];

flf_d_printf("[%s]\n", utf8s);
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	do_clear_mark_();

#ifdef ENABLE_UNDO
	undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO

	bytes_str = strlen_path(utf8s);
	for (int byte_idx = 0; byte_idx < bytes_str; byte_idx += bytes_chr) {
		bytes_chr = utf8c_bytes(&utf8s[byte_idx]);
		strlcpy__(utf8c, &utf8s[byte_idx], bytes_chr);
		do_enter_utf8c(utf8c);
	}

	set_cur_buf_modified();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}
PRIVATE int do_enter_utf8c(const char *utf8c)
{
flf_d_printf("[%s]\n", utf8c);
	int bytes_insert = strnlen(utf8c, MAX_UTF8C_BYTES);
	if (line_data_strlen(EPCBVC_CL) + bytes_insert > MAX_EDIT_LINE_LEN) {
		// exceeds MAX_EDIT_LINE_LEN, do not enter
		return 0;
	}
	line_string_insert(EPCBVC_CL, EPCBVC_CLBI, utf8c, bytes_insert);
	EPCBVC_CLBI += bytes_insert;
	get_epc_buf()->buf_size += bytes_insert;
	return 1;
}

int doe_carriage_return(void)
{
	const char *ptr_s, *ptr_d;
	int len_s, len_d;

	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	do_clear_mark_();

#ifdef ENABLE_UNDO
	undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO

	set_cur_buf_modified();
	// >aaaa^bbbb
	line_separate(EPCBVC_CL, EPCBVC_CLBI, INSERT_BEFORE);
	EPCBVC_CLBI = 0;
	//  aaaa
	// >^bbbb

	if (GET_APPMD(ed_AUTO_INDENT)) {
		// autoindent: auto insert the previous lines preceeding spaces to the next line
		ptr_s = NODE_PREV(EPCBVC_CL)->data;
		SKIP_SPACE(ptr_s);
		ptr_d = EPCBVC_CL->data; 
		SKIP_SPACE(ptr_d);
		len_s = ptr_s - NODE_PREV(EPCBVC_CL)->data;
		len_d = ptr_d - EPCBVC_CL->data;
		line_string_replace(EPCBVC_CL, 0, len_d, NODE_PREV(EPCBVC_CL)->data, len_s);
		EPCBVC_CLBI = len_s;
	}

	if (EPCBVC_CURS_Y < bottom_scroll_margin_y()) {
		EPCBVC_CURS_Y++;
		post_cmd_processing(NODE_PREV(EPCBVC_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
		 UPDATE_SCRN_FORWARD);
	} else {
		post_cmd_processing(NODE_PREV(EPCBVC_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
		 UPDATE_SCRN_ALL);
	}
	return 1;
}
int doe_carriage_return_with_auto_indent(void)
{
	if (doe_carriage_return() == 0) {
		return 0;
	}
	// TODO: auto indent if not already indented
	for (int tab = 0; tab < CUR_EBUF_STATE(buf_TAB_SIZE); tab++) {
		do_enter_char(' ');
	}
	return 1;
}

int doe_backspace_limited(void)
{
	if (EPCBVC_CLBI <= 0) {
		return 0;
	}
	return doe_backspace();
}
int doe_backspace(void)
{
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	do_clear_mark_();

	if (EPCBVC_CLBI <= 0) {
		if (IS_NODE_TOP(EPCBVC_CL)) {
			// top of file, can not backspace
			return 0;
		}
		// line top, concatenate to the previous line
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(NODE_PREV(EPCBVC_CL), EPCBVC_CL, 1);
#endif // ENABLE_UNDO
		EPCBVC_CLBI = line_data_strlen(NODE_PREV(EPCBVC_CL));
		EPCBVC_CL = line_concat_with_prev(EPCBVC_CL);
		if (EPCBVC_CURS_Y > 0) {
			EPCBVC_CURS_Y--;
		}
		post_cmd_processing(EPCBVC_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO
		// not line top, delete character
		int bytes = utf8c_prev_bytes(EPCBVC_CL->data, &EPCBVC_CL->data[EPCBVC_CLBI]);
		EPCBVC_CLBI -= bytes;
		line_string_delete(EPCBVC_CL, EPCBVC_CLBI, bytes);
		get_epc_buf()->buf_size -= bytes;
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	}
	set_cur_buf_modified();
	return 1;
}

int doe_delete_char(void)
{
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	do_clear_mark_();

	if (EPCBVC_CLBI < line_data_strlen(EPCBVC_CL)) {
		// not line end
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO
		int bytes = utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
		line_string_delete(EPCBVC_CL, EPCBVC_CLBI, bytes);
		if (EPCB_ML == EPCBVC_CL && EPCBVC_CLBI < EPCB_MLBI) {
			// adjust mark position
			EPCB_MLBI -= bytes;
		}

		get_epc_buf()->buf_size -= bytes;
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	} else {
		// line end
		if (IS_NODE_BOT_MOST(EPCBVC_CL)) {
			// line end and the last line
			return 0;
		}
		// line end, concatenate with the next line
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, NODE_NEXT(EPCBVC_CL), 1);
#endif // ENABLE_UNDO
		if (EPCB_ML == NODE_NEXT(EPCBVC_CL)) {
			// next line will be freed, adjust mark position
			EPCB_ML = EPCBVC_CL;
			EPCB_MLBI += line_data_strlen(EPCB_ML);
		}
		line_concat_with_next(EPCBVC_CL);

		get_epc_buf()->buf_lines--;
		get_epc_buf()->buf_size--;
		post_cmd_processing(EPCBVC_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	}
	set_cur_buf_modified();
	return 1;
}

#define	LETTER_BIDIR		0	// camel -> Camel -> CAMEL -> cAMEL
#define	LETTER_TO_UPP		1	// -> CAMEL
#define	LETTER_TO_LOW		2	// -> camel
#define	LETTER_TO_UPPLOW	3	// -> Camel
#define	LETTER_TO_LOWUPP	4	// -> cAMEL
PRIVATE int doe_conv_upp_low_letter_(char mode);
#define CAMEL_CASE	// "Camel" / "cAMEL"
#ifdef CAMEL_CASE
PRIVATE void change_str_letters(char *str, size_t len, char mode);
#endif // CAMEL_CASE

int doe_conv_upp_low_letter(void)
{
	return doe_conv_upp_low_letter_(LETTER_BIDIR);
}
int doe_conv_upp_letter(void)
{
	return doe_conv_upp_low_letter_(LETTER_TO_UPP);
}
int doe_conv_low_letter(void)
{
	return doe_conv_upp_low_letter_(LETTER_TO_LOW);
}
// "_camel" --> "_CAMEL"
PRIVATE int doe_conv_upp_low_letter_(char mode)
{
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	do_clear_mark_();

	int byte_idx = EPCBVC_CLBI;
	char *data = EPCBVC_CL->data;
	if (is_char_id(data[byte_idx])) {
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, NODE_NEXT(EPCBVC_CL), 1);
#endif // ENABLE_UNDO
		char chr;
		while (chr = data[byte_idx], is_char_id(chr)) {
			if (isalpha(chr)) {
				break;
			}
			byte_idx++;
		}
#ifndef CAMEL_CASE
		if (mode == LETTER_BIDIR) {
			mode = islower(data[byte_idx]) ? LETTER_TO_UPP : LETTER_TO_LOW;
		}
		while (chr = data[byte_idx], is_char_id(chr)) {
			if (isalpha(chr)) {
				// Lower ==> Upper, Upper ==> Lower
				data[byte_idx] = (mode == LETTER_TO_UPP) ? toupper(chr) : tolower(chr);
			}
			byte_idx++;
		}
#else // CAMEL_CASE
		int len;
		for (len = 0; is_char_id(data[byte_idx + len]); len++) {
			// nothing to do
		}
		change_str_letters(&data[byte_idx], len, mode);
#endif // CAMEL_CASE
		set_cur_buf_modified();
	}
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}

PRIVATE void change_str_letters(char *str, size_t len, char mode)
{
	if (mode == LETTER_BIDIR) {
		if (len <= 0) {
			mode = LETTER_TO_UPP;
		} else if (len <= 1) {
			if (islower(str[0])) {
				mode = LETTER_TO_UPP;
			} else
			if (isupper(str[0])) {
				mode = LETTER_TO_LOW;
			} else {
				mode = LETTER_TO_UPP;
			}
		} else {
			if (islower(str[0]) && islower(str[1])) {	// camel -> Camel
				mode = LETTER_TO_UPPLOW;
			} else
			if (isupper(str[0]) && isupper(str[1])) {	// Camel -> CAMEL
				mode = LETTER_TO_LOWUPP;
			} else
			if (isupper(str[0]) && islower(str[1])) {	// CAMEL -> cAMEL
				mode = LETTER_TO_UPP;
			} else
			if (islower(str[0]) && isupper(str[1])) {	// cAMEL -> camel
				mode = LETTER_TO_LOW;
			} else
			if (isupper(str[0])) {						// C_AMEL -> c_amel
				mode = LETTER_TO_LOW;
			} else {									// c_amel -> C_AMEL
				mode = LETTER_TO_UPP;
			}
		}
	}
	for (int off = 0; off < len; off++) {
		if (isalpha(str[off])) {
			if (off == 0) {
				switch(mode) {
				case LETTER_TO_UPP:
				case LETTER_TO_UPPLOW:
				default:
					str[off] = toupper(str[off]);
					break;
				case LETTER_TO_LOW:
				case LETTER_TO_LOWUPP:
					str[off] = tolower(str[off]);
					break;
				}
			} else {
				switch(mode) {
				case LETTER_TO_UPP:
				case LETTER_TO_LOWUPP:
				default:
					str[off] = toupper(str[off]);
					break;
				case LETTER_TO_LOW:
				case LETTER_TO_UPPLOW:
					str[off] = tolower(str[off]);
					break;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
PRIVATE int memorized_columns = 0;
PRIVATE int doe_fill_spaces_to_columns__(int column_idx);
int doe_memorize_columns()
{
	memorized_columns = start_col_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
	disp_status_bar_done(_("Column position %d was memorized"), FROM_IDX(memorized_columns));
	return 0;
}
int doe_fill_spaces_to_columns()
{
	int col_idx = start_col_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
	doe_fill_spaces_to_columns__(memorized_columns);
	disp_status_bar_done(_("Space characters are filled from column %d to %d"),
	 FROM_IDX(col_idx), FROM_IDX(memorized_columns));
	return 0;
}
PRIVATE int doe_fill_spaces_to_columns__(int column_idx)
{
	int col_idx = start_col_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
	for (int loop = 0; col_idx < column_idx && loop < column_idx; loop++) {
		do_enter_utf8s(" ");
		col_idx = start_col_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
	}
	return 0;
}

//-----------------------------------------------------------------------------
int doe_refresh_editor(void)
{
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

//-----------------------------------------------------------------------------
int move_cursor_left(int move_disp_y)
{
	if (EPCBVC_CLBI <= 0) {
		// line top
		if (IS_NODE_TOP_MOST(EPCBVC_CL)) {
			return 0;
		}
		EPCBVC_CL = NODE_PREV(EPCBVC_CL);
		EPCBVC_CLBI = line_data_strlen(EPCBVC_CL);
		if (move_disp_y) {
			if (EPCBVC_CURS_Y > editor_vert_scroll_margin_lines()) {
				EPCBVC_CURS_Y--;
				set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
			}
		}
	} else {
		int wl_idx = start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
		EPCBVC_CLBI -= utf8c_prev_bytes(EPCBVC_CL->data, &EPCBVC_CL->data[EPCBVC_CLBI]);
		if (move_disp_y) {
			if (start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1) < wl_idx) {
				EPCBVC_CURS_Y--;
			}
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
	}
	return 1;
}

int move_cursor_right(void)
{
	if (EPCBVC_CLBI < line_data_strlen(EPCBVC_CL)) {
		int wl_idx = start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
		EPCBVC_CLBI += utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
		if (start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1) > wl_idx) {
			EPCBVC_CURS_Y++;
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
	} else {
		if (IS_NODE_BOT_MOST(EPCBVC_CL)) {
			return 0;
		}
		EPCBVC_CL = NODE_NEXT(EPCBVC_CL);
		EPCBVC_CLBI = 0;
		if (EPCBVC_CURS_Y < bottom_scroll_margin_y()) {
			EPCBVC_CURS_Y++;
			set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
		}
	}
	return 1;
}

int cur_line_up(be_line_t **line, int *byte_idx)
{
	int line_byte_idx = *byte_idx;
	te_concat_linefeed((*line)->data);
	int wl_idx = start_wl_idx_of_wrap_line(te_concat_lf_buf, line_byte_idx, -1);
	int col_idx = start_col_idx_of_wrap_line(te_concat_lf_buf, line_byte_idx, -1);
	if (wl_idx > 0) {
		wl_idx--;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_concat_lf_buf, wl_idx, col_idx, -1);
	} else {
		if (IS_PREV_NODE_INT(*line) == 0) {
			return 0;	// no move
		}
		*line = NODE_PREV(*line);
		te_concat_linefeed((*line)->data);
		wl_idx = max_wrap_line_idx(te_concat_lf_buf, -1);
		line_byte_idx = start_byte_idx_of_wrap_line(te_concat_lf_buf, wl_idx, col_idx, -1);
	}
	*byte_idx = line_byte_idx;
	return 1;
}
int cur_line_down(be_line_t **line, int *byte_idx)
{
	int line_byte_idx = *byte_idx;
	te_concat_linefeed((*line)->data);
	int wl_idx = start_wl_idx_of_wrap_line(te_concat_lf_buf, line_byte_idx, -1);
	int col_idx = start_col_idx_of_wrap_line(te_concat_lf_buf, line_byte_idx, -1);
	if (wl_idx < max_wrap_line_idx(te_concat_lf_buf, -1)) {
		wl_idx++;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_concat_lf_buf, wl_idx, col_idx, -1);
	} else {
		if (IS_NEXT_NODE_INT(*line) == 0) {
			return 0;	// no move
		}
		*line = NODE_NEXT(*line);
		te_concat_linefeed((*line)->data);
		wl_idx = 0;
		line_byte_idx = end_byte_idx_of_wrap_line_ge(te_concat_lf_buf, wl_idx, col_idx, -1);
	}
	*byte_idx = line_byte_idx;
	return 1;
}
int next_line(void)
{
	if (IS_NODE_BOT_MOST(EPCBVC_CL)) {
		return 0;
	}
	EPCBVC_CL = NODE_NEXT(EPCBVC_CL);
	EPCBVC_CLBI = 0;
	return IS_NODE_BOT_MOST(EPCBVC_CL) ? 1 : 2;
}

//-----------------------------------------------------------------------------
void first_line(void)
{
	EPCBVC_CL = CUR_EDIT_BUFS_TOP_NODE;
}
void last_line(void)
{
	EPCBVC_CL = CUR_EDIT_BUFS_BOT_NODE;
}

// End of editormove.c
