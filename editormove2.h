/**************************************************************************
 *   editormove2.h                                                        *
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

#ifndef editormove2_h
#define editormove2_h

//           +-----------------------------------+
// 0         |                                   |
// 1         |                                   |
// 2         |                                   |
// 3         | - - - - - - - - - - - - - - - - - | TOP_SCROLL_MARGIN_IDX
// :         |                                   |
// :         |                                   |
// :         |                                   |
// :         |                                   |
// LINES-1-3 | - - - - - - - - - - - - - - - - - | BOTTOM_SCROLL_MARGIN_IDX
// LINES-1-2 |                                   |
// LINES-1-1 |                                   |
// LINES-1-0 |                                   |
// LINES     +-----------------------------------+
#define EDITOR_VERT_SCROLL_MARGIN_LINES		\
	LIM_MAX(3, edit_win_get_text_lines()/3)
#define TOP_SCROLL_MARGIN_IDX		EDITOR_VERT_SCROLL_MARGIN_LINES
#define BOTTOM_SCROLL_MARGIN_IDX	\
	(edit_win_get_text_lines()-1 - EDITOR_VERT_SCROLL_MARGIN_LINES)

#define EDITOR_VERT_SCROLL_LINES						\
	MIN_MAX_(1, edit_win_get_text_lines()-1 - EDITOR_VERT_SCROLL_MARGIN_LINES, 50)

#define HORIZ_SCROLL_MARGIN		5

typedef enum {
	LOCATE_CURS_NONE,	// not adjust cursor pos
	LOCATE_CURS_JUMP_BACKWARD,	// keep contents pos if possible after jumping cursor pos,
								//  otherwise locate center-1
	LOCATE_CURS_JUMP_CENTER,	// keep contents pos if possible after jumping cursor pos,
								//  otherwise locate center
	LOCATE_CURS_JUMP_FORWARD,	// keep contents pos if possible after jumping cursor pos,
								//  otherwise locate center+1
	LOCATE_CURS_TOP,	// locate top    of screen
	LOCATE_CURS_CENTER,	// locate center of screen
	LOCATE_CURS_BOTTOM	// locate bottom of screen
} locate_cursor_to_t;

typedef enum {
	NO_MOVE, HORIZ_MOVE, VERT_MOVE,
} cursor_horiz_vert_move_t;

int post_cmd_processing(be_line_t *renum_from, cursor_horiz_vert_move_t cursor_move,
 locate_cursor_to_t locate_cursor, int update_needed);

void locate_cursor_in_edit_win(locate_cursor_to_t location);
void fix_cursor_y_keeping_vert_scroll_margin(void);

void memorize_cursor_pos_before_move(void);
int get_disp_y_after_cursor_move(void);

int get_cur_screen_top(be_line_t **line, int *byte_idx);
int get_screen_top(be_line_t *_cl_, int _clbi_, int yy,
 be_line_t **line, int *byte_idx);

void fix_buf_state_after_cursor_move(cursor_horiz_vert_move_t cursor_move);
void fix_buf_state_after_cursor_horiz_move(void);
void fix_buf_state_after_cursor_vert_move(void);

void update_min_text_x_to_keep(int text_x);
int get_c_b_v_min_text_x_to_keep(void);

void set_disable_update_min_x_to_keep();
void clear_disable_update_min_x_to_keep();
char is_disable_update_min_x_to_keep();

#endif // editormove2_h

// End of editormove2.h
