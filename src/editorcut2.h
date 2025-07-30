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
#define _CUT_BUFFER_FILE_NAME		"cut_buffer"
#if defined(APP_DIR)
#define CUT_BUFFER_FILE_NAME		_CUT_BUFFER_FILE_NAME
#else // APP_DIR
#define CUT_BUFFER_FILE_NAME		"." _CUT_BUFFER_FILE_NAME
#endif // APP_DIR

extern be_line_t *mark_min_line__;
extern int mark_min_byte_idx;
extern int mark_min_col_idx;
extern be_line_t *mark_max_line__;
extern int mark_max_byte_idx;
extern int mark_max_col_idx;

void clear_mark_pos();
void set_mark_pos();

void setup_cut_region_after_cursor_move(cursor_horiz_vert_move_t cursor_move);
void clear_cut_region();
void setup_cut_region();
int is_there_cut_region();
#ifdef ENABLE_DEBUG
void dump_cut_region();
#endif // ENABLE_DEBUG
int get_lines_selected_in_cut_region();
int get_columns_selected_in_cut_region();

int load_cut_buffers();
int save_cut_buffers();
void set_cut_buffers_modified();
void sync_cut_buffers_if_necessary();
int save_cut_buffers_if_local_newer();
int load_cut_buffers_if_file_newer();

#endif // editorcut2_h

// End of editorcut2.h
