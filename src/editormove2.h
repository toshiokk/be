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

#define VERT_SCROLL_MERGIN				3
// define this to limit the number of vertical scroll lines in large screen
#define LIMIT_MAX_VERT_SCROLL_LINES		25

//           +-----------------------------------+
// 0         |                                   |
// 1         |                                   | top_scroll_margin_y()
// 2         |                                   |
// 3         | - - - - - - - - - - - - - - - - - |
// :         |                                   |
// :         |                                   |
// :         |                                   |
// :         |                                   |
// LINES-1-3 | - - - - - - - - - - - - - - - - - |
// LINES-1-2 |                                   |
// LINES-1-1 |                                   | bottom_scroll_margin_y()
// LINES-1-0 |                                   |
// LINES     +-----------------------------------+
#define EDITOR_VERT_SCROLL_MERGIN	VERT_SCROLL_MERGIN
#define HORIZ_SCROLL_MARGIN			5
int editor_vert_scroll_margin_lines();
int top_scroll_margin_y();
int bottom_scroll_margin_y();
int editor_vert_scroll_lines();

typedef enum {
	LOCATE_CURS_NONE,			// not adjust cursor pos
	LOCATE_CURS_JUMP_BACKWARD,	// keep contents pos if possible after jumping cursor pos,
								//  otherwise locate center - 2 lines
	LOCATE_CURS_JUMP_CENTER,	// keep contents pos if possible after jumping cursor pos,
								//  otherwise locate center line
	LOCATE_CURS_JUMP_FORWARD,	// keep contents pos if possible after jumping cursor pos,
								//  otherwise locate center + 2 lines
	LOCATE_CURS_TOP,			// locate top    of screen
	LOCATE_CURS_CENTER,			// locate center of screen
	LOCATE_CURS_BOTTOM			// locate bottom of screen
} locate_cursor_to_t;

typedef enum {
	CURS_MOVE_NONE, CURS_MOVE_HORIZ, CURS_MOVE_VERT, CURS_MOVE_JUMP,
} cursor_horiz_vert_move_t;

void post_cmd_processing(be_line_t *renum_from, cursor_horiz_vert_move_t cursor_move,
 locate_cursor_to_t locate_cursor, int update_needed);

void locate_curs_y_in_edit_win(locate_cursor_to_t locate_curs);
void fix_cursor_y_keeping_vert_scroll_margin();

void memorize_cursor_pos_before_move();
int get_disp_y_after_cursor_move();

int get_edit_win_screen_top(be_line_t *_cl_, int _clbi_, int yy, be_line_t **line, int *byte_idx);

void update_text_x_to_keep_after_cursor_move(cursor_horiz_vert_move_t cursor_move);

void update_min_text_x_to_keep(int text_x);

void set_disabled_update_min_text_x_to_keep();
void clear_disabled_update_min_text_x_to_keep();
char is_disabled_update_min_text_x_to_keep();

#endif // editormove2_h

// End of editormove2.h
