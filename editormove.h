/**************************************************************************
 *   editormove.h                                                         *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#ifndef editormove_h
#define editormove_h

typedef enum {
	EBS_NONE,
	EBS_UP_AT_TOP,			// cursor UP requested at top of buffer
	EBS_PAGEUP_AT_TOP,		// page   UP requested at top of buffer
	EBS_DOWN_AT_BOTTOM,		// cursor down requested at bottom of buffer
	EBS_PAGEDOWN_AT_BOTTOM,	// page   down requested at bottom of buffer
} easy_buffer_switching_t;
void easy_buffer_switching_clear();
void easy_buffer_switching_count();
int easy_buffer_switching_check(easy_buffer_switching_t top_bottom);

int doe_left();
int doe_right();

int doe_start_of_line();
int doe_end_of_line();

int doe_up();
int doe_down();
int doe_page_up();
int doe_page_down();

int doe_prev_word();
int doe_next_word();
int doe_first_line();
int doe_last_line();
int doe_control_code();
int doe_charcode();
int doe_paste_from_history();

int doe_tab();
int doe_buffer_utf8c_bytes(char chr);
int doe_enter_buffered_utf8c_bytes();
int doe_carriage_return();
int doe_carriage_return_indent();
int doe_backspace_limited();
int doe_backspace();
int doe_delete_char();
int doe_conv_upp_low_letter();
int doe_conv_upp_letter();
int doe_conv_low_letter();

int doe_memorize_columns();
int doe_fill_spaces_to_columns();

int doe_enter_text();
int doe_enter_text_append();

int doe_refresh_editor();

int move_cursor_left(int move_disp_y);
int move_cursor_right();

int cur_line_up(be_line_t **line, int *byte_idx);
int cur_line_down(be_line_t **line, int *byte_idx);
int next_line();
void first_line();
void last_line();

#endif // editormove_h

// End of editormove.h
