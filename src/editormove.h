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

void doe_left();
void doe_right();

void doe_start_of_line();
void doe_end_of_line();

void set_column_idx_at_which_curs_vert_moved(int col_idx);
int get_column_idx_at_which_curs_vert_moved();

void doe_up();
void doe_down();
void doe_page_up();
void doe_page_down();

void doe_prev_word();
void doe_next_word();
void doe_first_line();
void doe_last_line();
void doe_control_code();
void doe_charcode();
void doe_paste_from_history();

void doe_tab();
void doe_buffer_utf8c_bytes(char chr);
void doe_enter_buffered_utf8c_bytes();
void doe_carriage_return();
void doe_carriage_return_indent();
void doe_backspace_limited();
void doe_backspace();
void doe_delete_char();
void doe_conv_upp_low_letter();
void doe_conv_upp_letter();
void doe_conv_low_letter();

void doe_memorize_columns();
void doe_fill_spaces_to_columns();

void doe_enter_text();
void doe_enter_text_add();

void doe_refresh_editor();

int move_cursor_left(int move_disp_y);
int move_cursor_right();

int cur_line_up(be_line_t **line, int *byte_idx);
int cur_line_down(be_line_t **line, int *byte_idx);
int next_line();
void first_line();
void last_line();

#endif // editormove_h

// End of editormove.h
