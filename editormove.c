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
PRIVATE char easy_buffer_switching_count = 0;

void clear_easy_buffer_switching()
{
	easy_buffer_switching = EBS_NONE;
	easy_buffer_switching_count = 0;
}
void count_easy_buffer_switching()
{
	if (easy_buffer_switching != EBS_NONE) {
		easy_buffer_switching_count++;
		if (easy_buffer_switching_count >= 2) {
			clear_easy_buffer_switching();
		}
	}
}
int check_easy_buffer_switching(easy_buffer_switching_t top_bottom)
{
	switch (easy_buffer_switching) {
	default:
	case EBS_NONE:
		break;
	case EBS_UP_AT_TOP:
		if (top_bottom == EBS_PAGEUP_AT_TOP) {
			clear_easy_buffer_switching();
			return 1;
		}
		break;
	case EBS_PAGEUP_AT_TOP:
		if (top_bottom == EBS_UP_AT_TOP) {
			clear_easy_buffer_switching();
			return 1;
		}
		break;
	case EBS_DOWN_AT_BOTTOM:
		if (top_bottom == EBS_PAGEDOWN_AT_BOTTOM) {
			clear_easy_buffer_switching();
			return 1;
		}
		break;
	case EBS_PAGEDOWN_AT_BOTTOM:
		if (top_bottom == EBS_DOWN_AT_BOTTOM) {
			clear_easy_buffer_switching();
			return 1;
		}
		break;
	}
	easy_buffer_switching = top_bottom;
	easy_buffer_switching_count = 0;
	return 0;
}

//-----------------------------------------------------------------------------
int doe_left(void)
{
	if (is_app_list_mode()) {
		return doe_page_up();
	}
	move_cursor_left(1);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int doe_right(void)
{
	if (is_app_list_mode()) {
		return doe_page_down();
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
	if (CEPBV_CLBI > 0) {
		// is head of the word ?
		if (get_char_type(CEPBV_CL->data[CEPBV_CLBI-1])
		  == get_char_type(CEPBV_CL->data[CEPBV_CLBI])) {
			// not head of the word
		} else {
			// head of the word
			move_cursor_left(1);
		}
		while (CEPBV_CLBI > 0
		 && (get_char_type(CEPBV_CL->data[CEPBV_CLBI-1])
		  == get_char_type(CEPBV_CL->data[CEPBV_CLBI])) ) {
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

	if (CEPBV_CL->data[CEPBV_CLBI]) {
		// skip current block
		chr_type = get_char_type(CEPBV_CL->data[CEPBV_CLBI]);
		// move to the beginning of the next block
		while (CEPBV_CL->data[CEPBV_CLBI]
		 && chr_type == get_char_type(CEPBV_CL->data[CEPBV_CLBI]))
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
	CEPBV_CLBI = 0;

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int doe_end_of_line(void)
{
	CEPBV_CLBI = line_data_len(CEPBV_CL);

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
	if (c_l_up(&CEPBV_CL, &CEPBV_CLBI)) {
		CEPBV_CURSOR_Y--;
	} else {
		if (check_easy_buffer_switching(EBS_UP_AT_TOP)) {
			// already top of buffer, go to the previous buffer's last line
			tio_beep();
			if (switch_cep_buf_to_prev(1, 1)) {
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
	if (c_l_down(&CEPBV_CL, &CEPBV_CLBI)) {
		CEPBV_CURSOR_Y++;
	} else {
		if (check_easy_buffer_switching(EBS_DOWN_AT_BOTTOM)) {
			// already bottom of buffer, go to the next buffer's top line
			tio_beep();
			if (switch_cep_buf_to_next(1, 1)) {
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

	if (c_l_up(&CEPBV_CL, &CEPBV_CLBI) == 0) {
		if (check_easy_buffer_switching(EBS_PAGEUP_AT_TOP)) {
			// already top of buffer, go to the previous buffer's last line
			tio_beep();
			if (switch_cep_buf_to_prev(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			}
		}
	} else {
		lines = (CEPBV_CURSOR_Y - TOP_SCROLL_MARGIN_Y) + EDITOR_VERT_SCROLL_LINES - 1;
///		lines = EDITOR_VERT_SCROLL_LINES - 1;	// smaller scroll
		for (cnt = 0; cnt < lines; cnt++) {
			CEPBV_CURSOR_Y--;
			if (c_l_up(&CEPBV_CL, &CEPBV_CLBI) == 0) {
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

	if (c_l_down(&CEPBV_CL, &CEPBV_CLBI) == 0) {
		if (check_easy_buffer_switching(EBS_PAGEDOWN_AT_BOTTOM)) {
			// already bottom of buffer, go to the next buffer's top line
			tio_beep();
			if (switch_cep_buf_to_next(1, 1)) {
				post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
			}
		}
	} else {
		lines = (BOTTOM_SCROLL_MARGIN_Y - CEPBV_CURSOR_Y) + EDITOR_VERT_SCROLL_LINES - 1;
///		lines = EDITOR_VERT_SCROLL_LINES - 1;	// smaller scroll
		for (cnt = 0; cnt < lines; cnt++) {
			CEPBV_CURSOR_Y++;
			if (c_l_down(&CEPBV_CL, &CEPBV_CLBI) == 0) {
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
	CEPBV_CL = CUR_EDIT_BUF_BOT_LINE;
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_BOTTOM, UPDATE_SCRN_ALL);
	return 1;
}

PRIVATE int do_enter_utf8s(const char *utf8s);
PRIVATE int do_enter_utf8c(const char *utf8c);
int doe_control_code(void)
{
	key_code_t key;

	if (is_view_mode_then_warn_it())
		return 0;
	disp_status_bar_ing(_("Input control character [^A-^Z,^[,^\\,^],^^,^_,\x7f]"));
	key = input_key_loop();
	disp_status_bar_ing(_("Key code: %04x"), key);
	if ((0x01 <= key && key < 0x20) || key == CHAR_DEL) {
		doe_enter_char(key);
	}
	return 1;
}
int doe_charcode(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;
	unsigned int chr;
#ifdef ENABLE_UTF8
	char utf8c[MAX_UTF8C_BYTES+1];
#endif // ENABLE_UTF8

	if (is_view_mode_then_warn_it())
		return 0;

	ret = input_string_tail("", string,
	 HISTORY_TYPE_IDX_SEARCH, _("Enter Unicode number in hex:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		disp_key_list_editor();
		return 0;
	}
	if (sscanf(string, "%x", &chr) == 1) {
#ifdef ENABLE_UTF8
		utf8c_encode(chr, utf8c);
		do_enter_utf8s(utf8c);
#else // ENABLE_UTF8
		doe_enter_char(chr);
#endif // ENABLE_UTF8
		return 1;
	}
	return 0;
}

int doe_paste_history(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;

	if (is_view_mode_then_warn_it())
		return 0;

	ret = input_string_tail("", string,
	 HISTORY_TYPE_IDX_SEARCH, _("Select history string to paste:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		disp_key_list_editor();
		return 0;
	}
	do_enter_utf8s(string);
	return 0;
}

int doe_tab(void)
{
	return doe_enter_char('\t');
}

#define UTF8S_SEND_BUF_LEN			(MAX_UTF8C_BYTES * 2)
PRIVATE int utf8s_send_buf_bytes = 0;
PRIVATE char utf8s_send_buf[UTF8S_SEND_BUF_LEN+1] = "";
int doe_enter_char(char chr)
{
	static char utf8c_state = 0;

	utf8c_state = utf8c_len(utf8c_state, chr);
flf_d_printf("<%02x>\n", (unsigned char)chr);
	// put to send buffer
	if (utf8s_send_buf_bytes < UTF8S_SEND_BUF_LEN) {
		utf8s_send_buf[utf8s_send_buf_bytes++] = chr;
		utf8s_send_buf[utf8s_send_buf_bytes] = '\0';
	}
	if (utf8c_state == 0) {
		doe_enter_char_send();
	}
	return 1;
}
int doe_enter_char_send(void)
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
PRIVATE int do_enter_utf8s(const char *utf8s)
{
	int bytes_str;
	int byte_idx;
#ifdef ENABLE_UTF8
	int bytes_chr;
#endif // ENABLE_UTF8
	char utf8c[MAX_UTF8C_BYTES+1];

flf_d_printf("[%s]\n", utf8s);
	do_clear_mark_();
	if (is_view_mode_then_warn_it()) {
		return 0;
	}
#ifdef ENABLE_UNDO
	undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO

	bytes_str = strnlen(utf8s, MAX_PATH_LEN);
#ifdef ENABLE_UTF8
	for (byte_idx = 0; byte_idx < bytes_str; byte_idx += bytes_chr) {
		bytes_chr = utf8c_bytes(&utf8s[byte_idx]);
		strlcpy__(utf8c, &utf8s[byte_idx], bytes_chr);
		do_enter_utf8c(utf8c);
	}
#else // ENABLE_UTF8
	for (byte_idx = 0; byte_idx < bytes_str; byte_idx++) {
		utf8c[0] = utf8s[byte_idx];
		utf8c[1] = '\0';
		do_enter_utf8c(utf8c);
	}
#endif // ENABLE_UTF8
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}
PRIVATE int do_enter_utf8c(const char *utf8c)
{
	int bytes_insert;

flf_d_printf("[%s]\n", utf8c);
	bytes_insert = strnlen(utf8c, MAX_UTF8C_BYTES);
	if (line_data_len(CEPBV_CL) + bytes_insert > MAX_EDIT_LINE_LEN) {
		// exceeds MAX_EDIT_LINE_LEN, do not enter
		return 0;
	}
	line_insert_string(CEPBV_CL, CEPBV_CLBI, utf8c, bytes_insert);
	CEPBV_CLBI += bytes_insert;
	get_cep_buf()->buf_size += bytes_insert;

	set_cur_buf_modified();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}

int doe_carriage_return(void)
{
	const char *ptr_s, *ptr_d;
	int len_s, len_d;

	do_clear_mark_();
	if (is_app_list_mode()) {
		editor_quit = EDITOR_ENTERED;
		return 0;
	}
	if (is_view_mode_then_warn_it())
		return 0;

#ifdef ENABLE_UNDO
	undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO

	set_cur_buf_modified();
	// >aaaa^bbbb
	line_separate(CEPBV_CL, CEPBV_CLBI, INSERT_BEFORE);
	CEPBV_CLBI = 0;
	//  aaaa
	// >^bbbb

	if (GET_APPMD(ed_AUTO_INDENT)) {
		// autoindent: auto insert the previous lines preceeding spaces to the next line
		ptr_s = NODE_PREV(CEPBV_CL)->data;
		SKIP_SPACE(ptr_s);
		ptr_d = CEPBV_CL->data; 
		SKIP_SPACE(ptr_d);
		len_s = ptr_s - NODE_PREV(CEPBV_CL)->data;
		len_d = ptr_d - CEPBV_CL->data;
		line_replace_string(CEPBV_CL, 0, len_d, NODE_PREV(CEPBV_CL)->data, len_s);
		CEPBV_CLBI = len_s;
	}

	if (CEPBV_CURSOR_Y < BOTTOM_SCROLL_MARGIN_Y) {
		CEPBV_CURSOR_Y++;
		post_cmd_processing(NODE_PREV(CEPBV_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
		 UPDATE_SCRN_FORWARD);
	} else {
		post_cmd_processing(NODE_PREV(CEPBV_CL), CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
		 UPDATE_SCRN_ALL);
	}
	return 1;
}

int doe_backspace(void)
{
	int bytes;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;
///_D_(dump_editor_panes())
	if (CEPBV_CLBI <= 0) {
		if (IS_NODE_TOP(CEPBV_CL)) {
			// top of file, can not backspace
			return 0;
		}
		// line top, concatenate to the previous line
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(NODE_PREV(CEPBV_CL), CEPBV_CL, 1);
#endif // ENABLE_UNDO
		CEPBV_CLBI = line_data_len(NODE_PREV(CEPBV_CL));
		CEPBV_CL = line_concat_with_prev(CEPBV_CL);
		if (CEPBV_CURSOR_Y > 0) {
			CEPBV_CURSOR_Y--;
		}
		post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO
		// not line top, delete character
		bytes = utf8c_prev_bytes(CEPBV_CL->data, &CEPBV_CL->data[CEPBV_CLBI]);
		CEPBV_CLBI -= bytes;
		line_delete_string(CEPBV_CL, CEPBV_CLBI, bytes);
		get_cep_buf()->buf_size -= bytes;
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	}
	set_cur_buf_modified();
	return 1;
}

int doe_delete_char(void)
{
	int bytes;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;

	if (CEPBV_CLBI < line_data_len(CEPBV_CL)) {
		// not line end
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, CEPBV_CL, 1);
#endif // ENABLE_UNDO
		bytes = utf8c_bytes(&CEPBV_CL->data[CEPBV_CLBI]);
		line_delete_string(CEPBV_CL, CEPBV_CLBI, bytes);
		if (CEPB_ML == CEPBV_CL && CEPBV_CLBI < CEPB_MLBI) {
			// adjust mark position
			CEPB_MLBI -= bytes;
		}

		get_cep_buf()->buf_size -= bytes;
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	} else {
		// line end
		if (IS_NODE_BOT_MOST(CEPBV_CL)) {
			// line end and the last line
			return 0;
		}
		// line end, concatenate with the next line
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, NODE_NEXT(CEPBV_CL), 1);
#endif // ENABLE_UNDO
		if (CEPB_ML == NODE_NEXT(CEPBV_CL)) {
			// next line will be freed, adjust mark position
			CEPB_ML = CEPBV_CL;
			CEPB_MLBI += line_data_len(CEPB_ML);
		}
		line_concat_with_next(CEPBV_CL);

		get_cep_buf()->buf_lines--;
		get_cep_buf()->buf_size--;
		post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
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
	if (is_view_mode_then_warn_it())
		return 0;
	byte_idx = CEPBV_CLBI;
	data = CEPBV_CL->data;
	if (isalpha(data[byte_idx])) {
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEPBV_CL, NODE_NEXT(CEPBV_CL), 1);
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

	if (CEPBV_CLBI <= 0) {
		// line top
		if (IS_NODE_TOP_MOST(CEPBV_CL)) {
			return 0;
		}
		CEPBV_CL = NODE_PREV(CEPBV_CL);
		CEPBV_CLBI = line_data_len(CEPBV_CL);
		if (move_disp_y) {
			if (CEPBV_CURSOR_Y > EDITOR_VERT_SCROLL_MARGIN_LINES) {
				CEPBV_CURSOR_Y--;
				set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
			}
		}
	} else {
		wl_idx = start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
		CEPBV_CLBI -= utf8c_prev_bytes(CEPBV_CL->data, &CEPBV_CL->data[CEPBV_CLBI]);
		if (move_disp_y) {
			if (start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1) < wl_idx) {
				CEPBV_CURSOR_Y--;
			}
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
	}
	return 1;
}

int move_cursor_right(void)
{
	int wl_idx;

	if (CEPBV_CLBI < line_data_len(CEPBV_CL)) {
		wl_idx = start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1);
		CEPBV_CLBI += utf8c_bytes(&CEPBV_CL->data[CEPBV_CLBI]);
		if (start_wl_idx_of_wrap_line(CEPBV_CL->data, CEPBV_CLBI, -1) > wl_idx) {
			CEPBV_CURSOR_Y++;
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
	} else {
		if (IS_NODE_BOT_MOST(CEPBV_CL)) {
			return 0;
		}
		CEPBV_CL = NODE_NEXT(CEPBV_CL);
		CEPBV_CLBI = 0;
		if (CEPBV_CURSOR_Y < BOTTOM_SCROLL_MARGIN_Y) {
			CEPBV_CURSOR_Y++;
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
	if (IS_NODE_BOT_MOST(CEPBV_CL))
		return 0;
	CEPBV_CL = NODE_NEXT(CEPBV_CL);
	CEPBV_CLBI = 0;
	return IS_NODE_BOT_MOST(CEPBV_CL) ? 1 : 2;
}

//-----------------------------------------------------------------------------
int first_line(void)
{
	CEPBV_CL = CUR_EDIT_BUF_TOP_LINE;
	return 1;
}

// End of editormove.c
