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
		return doe_switch_to_prev_file();
	}
	move_cursor_left(1);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int doe_right(void)
{
	if (is_app_list_mode()) {
		return doe_switch_to_next_file();
	}
	move_cursor_right();

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

PRIVATE int get_char_type(char chr)
{
/* '\0':0, space:1, alpha-numeric:2, others:3, UTF-8:4 */
#define CHR_TYPE(chr)	((chr) == '\0' ? 0 :								\
						 (((chr) == ' ' || (chr) == '\t') ? 1 :				\
						  ((isalnum(chr) || (chr) == '_') ? 2 :				\
						    (((unsigned char)(chr)) < 0x80 ? 3 : 4)			\
						  )													\
						 )													\
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
	EPCBVC_CLBI = line_data_len(EPCBVC_CL);

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
		doe_switch_editor_pane_();
		doe_up_();
		doe_switch_editor_pane_();
		doe_up_();
	}
	return 1;
}
PRIVATE void doe_up_(void)
{
	if (c_l_up(&EPCBVC_CL, &EPCBVC_CLBI)) {
		EPCBVC_CURSOR_Y--;
	} else {
		if (easy_buffer_switching_check(EBS_UP_AT_TOP)) {
			// already top of buffer, go to the previous buffer's last line
			tio_beep();
			if (doe_switch_to_prev_file() == 0) {
///			if (switch_epc_buf_to_prev(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			}
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
		doe_switch_editor_pane_();
		doe_down_();
		doe_switch_editor_pane_();
		doe_down_();
	}
	return 1;
}
PRIVATE void doe_down_(void)
{
	if (c_l_down(&EPCBVC_CL, &EPCBVC_CLBI)) {
		EPCBVC_CURSOR_Y++;
	} else {
		if (easy_buffer_switching_check(EBS_DOWN_AT_BOTTOM)) {
			// already bottom of buffer, go to the next buffer's top line
			tio_beep();
			if (doe_switch_to_next_file() == 0) {
///			if (switch_epc_buf_to_next(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
			}
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
		doe_switch_editor_pane_();
		doe_page_up_();
		doe_switch_editor_pane_();
		doe_page_up_();
	}
	return 1;
}
PRIVATE void doe_page_up_(void)
{
	int lines;
	int cnt;

	if (c_l_up(&EPCBVC_CL, &EPCBVC_CLBI) == 0) {
		if (easy_buffer_switching_check(EBS_PAGEUP_AT_TOP)) {
			// already top of buffer, go to the previous buffer's last line
			tio_beep();
			if (doe_switch_to_prev_file() == 0) {
///			if (switch_epc_buf_to_prev(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			}
		}
	} else {
		lines = (EPCBVC_CURSOR_Y - TOP_SCROLL_MARGIN_Y) + EDITOR_VERT_SCROLL_LINES - 1;
///		lines = EDITOR_VERT_SCROLL_LINES - 1;	// smaller scroll
		for (cnt = 0; cnt < lines; cnt++) {
			EPCBVC_CURSOR_Y--;
			if (c_l_up(&EPCBVC_CL, &EPCBVC_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
}

PRIVATE int doe_page_down_(void);
int doe_page_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		doe_page_down_();
	} else {
		doe_switch_editor_pane_();
		doe_page_down_();
		doe_switch_editor_pane_();
		doe_page_down_();
	}
	return 1;
}
PRIVATE int doe_page_down_(void)
{
	int lines;
	int cnt;

	if (c_l_down(&EPCBVC_CL, &EPCBVC_CLBI) == 0) {
		if (easy_buffer_switching_check(EBS_PAGEDOWN_AT_BOTTOM)) {
			// already bottom of buffer, go to the next buffer's top line
			tio_beep();
			if (doe_switch_to_next_file() == 0) {
///			if (switch_epc_buf_to_next(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
			}
		}
	} else {
		lines = (BOTTOM_SCROLL_MARGIN_Y - EPCBVC_CURSOR_Y) + EDITOR_VERT_SCROLL_LINES - 1;
///		lines = EDITOR_VERT_SCROLL_LINES - 1;	// smaller scroll
		for (cnt = 0; cnt < lines; cnt++) {
			EPCBVC_CURSOR_Y++;
			if (c_l_down(&EPCBVC_CL, &EPCBVC_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
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
	char string[MAX_PATH_LEN+1];

	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	int ret = input_string_tail("", string, HISTORY_TYPE_IDX_SEARCH,
	 _("Enter Unicode number in hex:"));
	if (ret <= INPUT_LOADED) {
		return 0;
	}
	///if (ret <= 0) {
	///	disp_key_list_editor();
	///	return 0;
	///}
	unsigned int chr;
	char utf8c[MAX_UTF8C_BYTES+1];
	if (sscanf(string, "%x", &chr) == 1) {
		utf8c_encode(chr, utf8c);
		do_enter_utf8s(utf8c);
		return 1;
	}
	return 0;
}

int doe_paste_from_history(void)
{
	char string[MAX_PATH_LEN+1];

	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	int ret = input_string_tail("", string, HISTORY_TYPE_IDX_SEARCH,
	 _("Select history string to paste:"));
	if (ret <= INPUT_LOADED) {
		return 0;
	}
	///if (ret <= 0) {
	///	disp_key_list_editor();
	///	return 0;
	///}
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
	do_clear_mark_();
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

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
	if (line_data_len(EPCBVC_CL) + bytes_insert > MAX_EDIT_LINE_LEN) {
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

	do_clear_mark_();
	if (is_app_list_mode()) {
		editor_quit = EDITOR_INPUT;
		return 0;
	}
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

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

	if (EPCBVC_CURSOR_Y < BOTTOM_SCROLL_MARGIN_Y) {
		EPCBVC_CURSOR_Y++;
		post_cmd_processing(NODE_PREV(EPCBVC_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
		 UPDATE_SCRN_FORWARD);
	} else {
		post_cmd_processing(NODE_PREV(EPCBVC_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
		 UPDATE_SCRN_ALL);
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
	int bytes;

	do_clear_mark_();
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	if (EPCBVC_CLBI <= 0) {
		if (IS_NODE_TOP(EPCBVC_CL)) {
			// top of file, can not backspace
			return 0;
		}
		// line top, concatenate to the previous line
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(NODE_PREV(EPCBVC_CL), EPCBVC_CL, 1);
#endif // ENABLE_UNDO
		EPCBVC_CLBI = line_data_len(NODE_PREV(EPCBVC_CL));
		EPCBVC_CL = line_concat_with_prev(EPCBVC_CL);
		if (EPCBVC_CURSOR_Y > 0) {
			EPCBVC_CURSOR_Y--;
		}
		post_cmd_processing(EPCBVC_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO
		// not line top, delete character
		bytes = utf8c_prev_bytes(EPCBVC_CL->data, &EPCBVC_CL->data[EPCBVC_CLBI]);
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
	int bytes;

	do_clear_mark_();
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	if (EPCBVC_CLBI < line_data_len(EPCBVC_CL)) {
		// not line end
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, EPCBVC_CL, 1);
#endif // ENABLE_UNDO
		bytes = utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
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
			EPCB_MLBI += line_data_len(EPCB_ML);
		}
		line_concat_with_next(EPCBVC_CL);

		get_epc_buf()->buf_lines--;
		get_epc_buf()->buf_size--;
		post_cmd_processing(EPCBVC_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	}
	set_cur_buf_modified();
	return 1;
}

int doe_conv_upp_low_letter(void)
{
	int byte_idx;
	char *data;
	char first_chr;
	char chr;

	do_clear_mark_();
	if (is_editor_view_mode_then_warn_it()) {
		return 0;
	}

	byte_idx = EPCBVC_CLBI;
	data = EPCBVC_CL->data;
	if (isalpha(data[byte_idx])) {
#ifdef ENABLE_UNDO
		undo_set_region__save_before_change(EPCBVC_CL, NODE_NEXT(EPCBVC_CL), 1);
#endif // ENABLE_UNDO
		first_chr = data[byte_idx];
		while ((chr = data[byte_idx]) != '\0') {
			if ((isalpha(chr) || chr == '_' || isalnum(chr)) == 0)
				break;
			if (isalpha(chr)) {
				// Upper ==> Lower, Lower ==> Upper ==> Lower
				data[byte_idx] = isupper(first_chr) ? tolower(chr) : toupper(chr);
				set_cur_buf_modified();
			}
			byte_idx++;
		}
	}
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
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
	int wl_idx;

	if (EPCBVC_CLBI <= 0) {
		// line top
		if (IS_NODE_TOP_MOST(EPCBVC_CL)) {
			return 0;
		}
		EPCBVC_CL = NODE_PREV(EPCBVC_CL);
		EPCBVC_CLBI = line_data_len(EPCBVC_CL);
		if (move_disp_y) {
			if (EPCBVC_CURSOR_Y > EDITOR_VERT_SCROLL_MARGIN_LINES) {
				EPCBVC_CURSOR_Y--;
				set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
			}
		}
	} else {
		wl_idx = start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
		EPCBVC_CLBI -= utf8c_prev_bytes(EPCBVC_CL->data, &EPCBVC_CL->data[EPCBVC_CLBI]);
		if (move_disp_y) {
			if (start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1) < wl_idx) {
				EPCBVC_CURSOR_Y--;
			}
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
	}
	return 1;
}

int move_cursor_right(void)
{
	int wl_idx;

	if (EPCBVC_CLBI < line_data_len(EPCBVC_CL)) {
		wl_idx = start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1);
		EPCBVC_CLBI += utf8c_bytes(&EPCBVC_CL->data[EPCBVC_CLBI]);
		if (start_wl_idx_of_wrap_line(EPCBVC_CL->data, EPCBVC_CLBI, -1) > wl_idx) {
			EPCBVC_CURSOR_Y++;
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
	} else {
		if (IS_NODE_BOT_MOST(EPCBVC_CL)) {
			return 0;
		}
		EPCBVC_CL = NODE_NEXT(EPCBVC_CL);
		EPCBVC_CLBI = 0;
		if (EPCBVC_CURSOR_Y < BOTTOM_SCROLL_MARGIN_Y) {
			EPCBVC_CURSOR_Y++;
			set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
		}
	}
	return 1;
}

int c_l_up(be_line_t **line, int *byte_idx)
{
	int line_byte_idx;
	int wl_idx;
	int col_idx;

	line_byte_idx = *byte_idx;

	te_concat_linefeed((*line)->data);
	wl_idx = start_wl_idx_of_wrap_line(te_concat_linefeed_buf, line_byte_idx, -1);
	col_idx = start_col_idx_of_wrap_line(te_concat_linefeed_buf, line_byte_idx, -1);
	if (wl_idx > 0) {
		wl_idx--;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_concat_linefeed_buf, wl_idx, col_idx, -1);
	} else {
		if (IS_NODE_TOP_MOST(*line)) {
			return 0;	// no move
		}
		*line = NODE_PREV(*line);
		te_concat_linefeed((*line)->data);
		wl_idx = max_wrap_line_idx(te_concat_linefeed_buf, -1);
		line_byte_idx = start_byte_idx_of_wrap_line(te_concat_linefeed_buf, wl_idx, col_idx, -1);
	}

	*byte_idx = line_byte_idx;
	return 1;
}
int c_l_down(be_line_t **line, int *byte_idx)
{
	int line_byte_idx;
	int wl_idx;
	int col_idx;

	line_byte_idx = *byte_idx;

	te_concat_linefeed((*line)->data);
	wl_idx = start_wl_idx_of_wrap_line(te_concat_linefeed_buf, line_byte_idx, -1);
	col_idx = start_col_idx_of_wrap_line(te_concat_linefeed_buf, line_byte_idx, -1);
	if (wl_idx < max_wrap_line_idx(te_concat_linefeed_buf, -1)) {
		wl_idx++;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_concat_linefeed_buf, wl_idx, col_idx, -1);
	} else {
		if (IS_NODE_BOT_MOST(*line)) {
			return 0;	// no move
		}
		*line = NODE_NEXT(*line);
		te_concat_linefeed((*line)->data);
		wl_idx = 0;
		line_byte_idx = end_byte_idx_of_wrap_line_ge(te_concat_linefeed_buf, wl_idx, col_idx, -1);
	}

	*byte_idx = line_byte_idx;
	return 1;
}
int cursor_next_line(void)
{
	if (IS_NODE_BOT_MOST(EPCBVC_CL))
		return 0;
	EPCBVC_CL = NODE_NEXT(EPCBVC_CL);
	EPCBVC_CLBI = 0;
	return IS_NODE_BOT_MOST(EPCBVC_CL) ? 1 : 2;
}

//-----------------------------------------------------------------------------
int first_line(void)
{
	EPCBVC_CL = CUR_EDIT_BUF_TOP_LINE;
	return 1;
}
int last_line(void)
{
	EPCBVC_CL = CUR_EDIT_BUF_BOT_LINE;
	return 1;
}

// End of editormove.c
