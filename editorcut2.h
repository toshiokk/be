/**************************************************************************
 *   editorcut2.h                                                         *
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

#ifndef editorcut2_h
#define editorcut2_h

extern be_line_t *mark_min_line;
extern int mark_min_byte_idx;
extern int mark_min_col_idx;
extern be_line_t *mark_max_line;
extern int mark_max_byte_idx;
extern int mark_max_col_idx;

void clear_mark_pos(void);
void set_mark_pos(void);
void setup_cut_region_after_cursor_move(cursor_horiz_vert_move_t cursor_move);
void setup_cut_region(void);
int is_there_cut_region(void);
int lines_selected(void);

#endif // editorcut2_h

// End of editorcut2.h
