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

typedef enum {
	TBF_NONE,
	TBF_TOP,
	TBF_BOTTOM,
} top_bottom_of_file_t;
PRIVATE top_bottom_of_file_t top_bottom_of_file = 0;

//-----------------------------------------------------------------------------

int do_left(void)
{
	if (is_app_list_mode()) {
		return do_page_up();
	}
	move_cursor_left(1);

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int do_right(void)
{
	if (is_app_list_mode()) {
		return do_page_down();
	}
	move_cursor_right();

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

PRIVATE int get_char_type(char chr)
{
/* '\0':0, space:1, alpha-numeric:2, others:3, UTF-8:4 */
#define CHR_TYPE(chr)	((chr) == '\0' ? 0 :								\
						 (((chr) == ' ' || (chr) == '\t') ? 1 :				\
						  ((isalnum(chr) || (chr) == '_') ? 2 :		\
						    (((unsigned char)(chr)) < 0x80 ? 3 : 4)						\
						  )													\
						 )													\
						)
	return CHR_TYPE(chr);
}

// go to previous word
int do_prev_word(void)
{
	if (CEBV_CLBI > 0) {
		// is head of the word ?
		if (get_char_type(CEBV_CL->data[CEBV_CLBI-1])
		  == get_char_type(CEBV_CL->data[CEBV_CLBI])) {
			// not head of the word
		} else {
			// head of the word
			move_cursor_left(1);
		}
		while (CEBV_CLBI > 0
		 && (get_char_type(CEBV_CL->data[CEBV_CLBI-1])
		  == get_char_type(CEBV_CL->data[CEBV_CLBI])) ) {
			move_cursor_left(1);
		}
	} else {
		// if top of the line, move to the end of the previous line
		move_cursor_left(1);
	}

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 0;
}
// go to next word
int do_next_word(void)
{
	int chr_type;

	if (CEBV_CL->data[CEBV_CLBI]) {
		// skip current block
		chr_type = get_char_type(CEBV_CL->data[CEBV_CLBI]);
		// move to the beginning of the next block
		while (CEBV_CL->data[CEBV_CLBI]
		 && chr_type == get_char_type(CEBV_CL->data[CEBV_CLBI]))
			move_cursor_right();
	} else {
		// if end of the line, move to the top of the next line
		move_cursor_right();
	}

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 0;
}

int do_start_of_line(void)
{
	CEBV_CLBI = 0;

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

int do_end_of_line(void)
{
	CEBV_CLBI = line_data_len(CEBV_CL);

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	return 1;
}

//-----------------------------------------------------------------------------

PRIVATE void do_up_(void);
int do_up(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_up_();
	} else {
		do_switch_editor_pane_();
		do_up_();
		do_switch_editor_pane_();
		do_up_();
	}
	return 1;
}
PRIVATE void do_up_(void)
{
	if (c_l_up(&CEBV_CL, &CEBV_CLBI)) {
		CEBV_CURSOR_Y--;
		top_bottom_of_file = TBF_NONE;
	} else {
		top_bottom_of_file = TBF_TOP;
	}
	post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}
PRIVATE void do_down_(void);
int do_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_down_();
	} else {
		do_switch_editor_pane_();
		do_down_();
		do_switch_editor_pane_();
		do_down_();
	}
	return 1;
}
PRIVATE void do_down_(void)
{
	if (c_l_down(&CEBV_CL, &CEBV_CLBI)) {
		CEBV_CURSOR_Y++;
		top_bottom_of_file = TBF_NONE;
	} else {
		top_bottom_of_file = TBF_BOTTOM;
	}
	post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
}

PRIVATE void do_page_up_(void);
int do_page_up(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_page_up_();
	} else {
		do_switch_editor_pane_();
		do_page_up_();
		do_switch_editor_pane_();
		do_page_up_();
	}
	return 1;
}
PRIVATE void do_page_up_(void)
{
	int lines;
	int cnt;

	if (c_l_up(&CEBV_CL, &CEBV_CLBI) == 0) {
		if (top_bottom_of_file == TBF_TOP) {
			// already top of buffer, go to the prevous buffer's last line
			top_bottom_of_file = TBF_NONE;
			tio_beep();
			if (switch_c_e_b_to_prev(1, 1)) {
				post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
			}
		}
	} else {
///		lines = (CEBV_CURSOR_Y - TOP_SCROLL_MARGIN_IDX) + EDITOR_VERT_SCROLL_LINES - 1;
		lines = EDITOR_VERT_SCROLL_LINES - 1;
		for (cnt = 0; cnt < lines; cnt++) {
			CEBV_CURSOR_Y--;
			if (c_l_up(&CEBV_CL, &CEBV_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
}

PRIVATE int do_page_down_(void);
int do_page_down(void)
{
	if (GET_APPMD(ed_DUAL_SCROLL) == 0) {
		do_page_down_();
	} else {
		do_switch_editor_pane_();
		do_page_down_();
		do_switch_editor_pane_();
		do_page_down_();
	}
	return 1;
}
PRIVATE int do_page_down_(void)
{
	int lines;
	int cnt;

	if (c_l_down(&CEBV_CL, &CEBV_CLBI) == 0) {
		if (top_bottom_of_file == TBF_BOTTOM) {
			// already bottom of buffer, go to the next buffer's top line
			top_bottom_of_file = TBF_NONE;
			tio_beep();
			if (switch_c_e_b_to_next(1, 1)) {
				post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
			}
		}
	} else {
///		lines = (BOTTOM_SCROLL_MARGIN_IDX - CEBV_CURSOR_Y) + EDITOR_VERT_SCROLL_LINES - 1;
		lines = EDITOR_VERT_SCROLL_LINES - 1;
		for (cnt = 0; cnt < lines; cnt++) {
			CEBV_CURSOR_Y++;
			if (c_l_down(&CEBV_CL, &CEBV_CLBI) == 0) {
				break;
			}
		}
		post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
	return 1;
}

int do_first_line(void)
{
	first_line();
	post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_TOP, UPDATE_SCRN_ALL);
	return 1;
}
int do_last_line(void)
{
	CEBV_CL = CUR_EDIT_BUF_BOT_LINE;
	post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_BOTTOM, UPDATE_SCRN_ALL);
	return 1;
}

PRIVATE int do_enter_utf8s(const char *utf8s);
PRIVATE int do_enter_utf8c(const char *utf8c);
int do_control_code(void)
{
	key_code_t key;

	if (is_view_mode_then_warn_it())
		return 0;
	disp_status_bar_ing(_("Input control character [^A-^Z,^[,^\\,^],^^,^_,\x7f]"));
	key = input_key_loop();
	disp_status_bar_ing(_("Key code: %04x"), key);
	if ((0x01 <= key && key < 0x20) || key == 0x7f) {
		do_enter_char(key);
	}
	return 1;
}
int do_charcode(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;
	unsigned int chr;
#ifdef ENABLE_UTF8
	char utf8c[MAX_UTF8C_BYTES+1];
#endif // ENABLE_UTF8

	if (is_view_mode_then_warn_it())
		return 0;

	ret = input_string("", string,
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
		do_enter_char(chr);
#endif // ENABLE_UTF8
		return 1;
	}
	return 0;
}

int do_paste_history(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;

	if (is_view_mode_then_warn_it())
		return 0;

	ret = input_string("", string,
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

int do_tab(void)
{
	return do_enter_char('\t');
}

#define UTF8S_SEND_BUF_LEN			(MAX_UTF8C_BYTES * 2)
PRIVATE int utf8s_send_buf_bytes = 0;
PRIVATE char utf8s_send_buf[UTF8S_SEND_BUF_LEN+1] = "";
int do_enter_char(char chr)
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
		do_enter_char_send();
	}
	return 1;
}
int do_enter_char_send(void)
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
///_FLF_
		return 0;
	}
///_FLF_
#ifdef ENABLE_UNDO
	undo_set_region_save_before_change(CEBV_CL, CEBV_CL, 1);
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
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}
PRIVATE int do_enter_utf8c(const char *utf8c)
{
	int bytes_insert;

flf_d_printf("[%s]\n", utf8c);
	bytes_insert = strnlen(utf8c, MAX_UTF8C_BYTES);
	if (line_data_len(CEBV_CL) + bytes_insert > MAX_EDIT_LINE_LEN) {
		// exceeds MAX_EDIT_LINE_LEN, do not enter
		return 0;
	}
	line_insert_string(CEBV_CL, CEBV_CLBI, utf8c, bytes_insert);
	CEBV_CLBI += bytes_insert;
	get_c_e_b()->buf_size += bytes_insert;

	set_cur_buf_modified();
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}

int do_carriage_return(void)
{
	const char *ptr_s, *ptr_d;
	int len_s, len_d;

///_FLF_
	do_clear_mark_();
	if (is_app_list_mode()) {
		editor_quit = EDITOR_QUIT;
		return 0;
	}
	if (is_view_mode_then_warn_it())
		return 0;

#ifdef ENABLE_UNDO
	undo_set_region_save_before_change(CEBV_CL, CEBV_CL, 1);
#endif // ENABLE_UNDO

	set_cur_buf_modified();
	// >aaaa^bbbb
	line_separate(CEBV_CL, CEBV_CLBI, INSERT_BEFORE);
	CEBV_CLBI = 0;
	//  aaaa
	// >^bbbb

	if (GET_APPMD(ed_AUTO_INDENT)) {
		// autoindent: auto insert the previous lines preceeding spaces to the next line
		ptr_s = CEBV_CL->prev->data;
		SKIP_SPACE(ptr_s);
		ptr_d = CEBV_CL->data; 
		SKIP_SPACE(ptr_d);
		len_s = ptr_s - CEBV_CL->prev->data;
		len_d = ptr_d - CEBV_CL->data;
		line_replace_string(CEBV_CL, 0, len_d, CEBV_CL->prev->data, len_s);
		CEBV_CLBI = len_s;
	}

	if (CEBV_CURSOR_Y < BOTTOM_SCROLL_MARGIN_IDX) {
		CEBV_CURSOR_Y++;
		post_cmd_processing(CEBV_CL->prev, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
		post_cmd_processing(CEBV_CL->prev, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	}
	return 1;
}

int do_backspace(void)
{
	int bytes;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;
///_D_(dump_cur_editor_views());
	if (CEBV_CLBI <= 0) {
		if (IS_NODE_TOP(CEBV_CL)) {
			// top of file, can not backspace
			return 0;
		}
		// line top, concatenate to the previous line
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEBV_CL->prev, CEBV_CL, 1);
#endif // ENABLE_UNDO
		CEBV_CLBI = line_data_len(CEBV_CL->prev);
		CEBV_CL = line_concat_with_prev(CEBV_CL);
		if (CEBV_CURSOR_Y > 0) {
			CEBV_CURSOR_Y--;
		}
		post_cmd_processing(CEBV_CL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	} else {
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEBV_CL, CEBV_CL, 1);
#endif // ENABLE_UNDO
		// not line top, delete character
		bytes = utf8c_prev_bytes(CEBV_CL->data, &CEBV_CL->data[CEBV_CLBI]);
		CEBV_CLBI -= bytes;
		line_delete_string(CEBV_CL, CEBV_CLBI, bytes);
		get_c_e_b()->buf_size -= bytes;
		post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	}
	set_cur_buf_modified();
	return 1;
}

int do_delete_char(void)
{
	int bytes;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;

	if (CEBV_CLBI < line_data_len(CEBV_CL)) {
		// not line end
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEBV_CL, CEBV_CL, 1);
#endif // ENABLE_UNDO
		bytes = utf8c_bytes(&CEBV_CL->data[CEBV_CLBI]);
		line_delete_string(CEBV_CL, CEBV_CLBI, bytes);
		if (CEB_ML == CEBV_CL && CEBV_CLBI < CEB_MLBI) {
			// adjust mark position
			CEB_MLBI -= bytes;
		}

		get_c_e_b()->buf_size -= bytes;
		post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_CUR);
	} else {
		if (IS_NODE_BOT(CEBV_CL)) {
			// line end and the last line
			return 0;
		}
		// line end, concatenate with the next line
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEBV_CL, CEBV_CL->next, 1);
#endif // ENABLE_UNDO
		if (CEB_ML == CEBV_CL->next) {
			// next line will be freed, adjust mark position
			CEB_ML = CEBV_CL;
			CEB_MLBI += line_data_len(CEB_ML);
		}
		line_concat_with_next(CEBV_CL);

		get_c_e_b()->buf_lines--;
		get_c_e_b()->buf_size--;
		post_cmd_processing(CEBV_CL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_FORWARD);
	}
	set_cur_buf_modified();
	return 1;
}

int do_conv_upp_low_letter(void)
{
	int byte_idx;
	char *data;
	char first_chr;
	char chr;

	do_clear_mark_();
	if (is_view_mode_then_warn_it())
		return 0;
	byte_idx = CEBV_CLBI;
	data = CEBV_CL->data;
	if (isalpha(data[byte_idx])) {
#ifdef ENABLE_UNDO
		undo_set_region_save_before_change(CEBV_CL, CEBV_CL->next, 1);
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
	post_cmd_processing(NULL, NO_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	return 1;
}

//-----------------------------------------------------------------------------

int do_refresh_editor(void)
{
	post_cmd_processing(NULL, NO_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}

//-----------------------------------------------------------------------------

int move_cursor_left(int move_disp_y)
{
	int wl_idx;

	if (CEBV_CLBI <= 0) {
		// line top
		if (IS_NODE_TOP(CEBV_CL)) {
			return 0;
		}
		CEBV_CL = CEBV_CL->prev;
		CEBV_CLBI = line_data_len(CEBV_CL);
		if (move_disp_y) {
			if (CEBV_CURSOR_Y > EDITOR_VERT_SCROLL_MARGIN_LINES) {
				CEBV_CURSOR_Y--;
				set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
			}
		}
	} else {
		wl_idx = start_wl_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1);
		CEBV_CLBI -= utf8c_prev_bytes(CEBV_CL->data, &CEBV_CL->data[CEBV_CLBI]);
		if (move_disp_y) {
			if (start_wl_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1) < wl_idx) {
				CEBV_CURSOR_Y--;
			}
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_NEXT);
	}
	return 1;
}

int move_cursor_right(void)
{
	int wl_idx;

	if (CEBV_CLBI < line_data_len(CEBV_CL)) {
		wl_idx = start_wl_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1);
		CEBV_CLBI += utf8c_bytes(&CEBV_CL->data[CEBV_CLBI]);
		if (start_wl_idx_of_wrap_line(CEBV_CL->data, CEBV_CLBI, -1) > wl_idx) {
			CEBV_CURSOR_Y++;
		}
		set_edit_win_update_needed(UPDATE_SCRN_CUR_PREV);
	} else {
		if (IS_NODE_BOT(CEBV_CL)) {
			return 0;
		}
		CEBV_CL = CEBV_CL->next;
		CEBV_CLBI = 0;
		if (CEBV_CURSOR_Y < BOTTOM_SCROLL_MARGIN_IDX) {
			CEBV_CURSOR_Y++;
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
	wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	col_idx = start_col_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	if (wl_idx > 0) {
		wl_idx--;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_line_concat_linefeed, wl_idx, col_idx, -1);
	} else {
		if (IS_NODE_TOP(*line)) {
			return 0;
		}
		*line = (*line)->prev;
		te_concat_linefeed((*line)->data);
		wl_idx = max_wrap_line_idx(te_line_concat_linefeed, -1);
		line_byte_idx = start_byte_idx_of_wrap_line(te_line_concat_linefeed, wl_idx, col_idx, -1);
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
	wl_idx = start_wl_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	col_idx = start_col_idx_of_wrap_line(te_line_concat_linefeed, line_byte_idx, -1);
	if (wl_idx < max_wrap_line_idx(te_line_concat_linefeed, -1)) {
		wl_idx++;
		line_byte_idx = end_byte_idx_of_wrap_line_le(te_line_concat_linefeed, wl_idx, col_idx, -1);
	} else {
		if (IS_NODE_BOT(*line)) {
			return 0;
		}
		*line = (*line)->next;
		te_concat_linefeed((*line)->data);
		wl_idx = 0;
		line_byte_idx = end_byte_idx_of_wrap_line_ge(te_line_concat_linefeed, wl_idx, col_idx, -1);
	}

	*byte_idx = line_byte_idx;
	return 1;
}
int cursor_next_line(void)
{
	if (IS_NODE_BOT(CEBV_CL))
		return 0;
	CEBV_CL = CEBV_CL->next;
	CEBV_CLBI = 0;
	return IS_NODE_BOT(CEBV_CL) ? 1 : 2;
}

//-----------------------------------------------------------------------------

int first_line(void)
{
	CEBV_CL = CUR_EDIT_BUF_TOP_LINE;
	return 1;
}

// End of editormove.c
