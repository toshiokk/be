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

int doe_left(void);
int doe_right(void);

int doe_start_of_line(void);
int doe_end_of_line(void);

int doe_up(void);
int doe_down(void);
int doe_page_up(void);
int doe_page_down(void);

int doe_prev_word(void);
int doe_next_word(void);
int doe_first_line(void);
int doe_last_line(void);
int doe_control_code(void);
int doe_charcode(void);
int doe_paste_history(void);

int doe_tab(void);
int doe_enter_char(char chr);
int doe_enter_char_send(void);
int doe_carriage_return(void);
int doe_backspace(void);
int doe_delete_char(void);
int doe_conv_upp_low_letter(void);

int doe_refresh_editor(void);

int move_cursor_left(int move_disp_y);
int move_cursor_right(void);

int c_l_up(be_line_t **line, int *byte_idx);
int c_l_down(be_line_t **line, int *byte_idx);
int cursor_next_line(void);
int first_line(void);

#endif // editormove_h

// End of editormove.h
