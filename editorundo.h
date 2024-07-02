/**************************************************************************
 *   editorundo.h                                                         *
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

#ifndef editorundo_h
#define editorundo_h

#ifdef ENABLE_UNDO

void init_undo_redo_bufs(void);
be_buf_t *push_undo_buf(be_buf_t *buf);
be_buf_t *pop_undo_buf(void);
be_buf_t *push_redo_buf(be_buf_t *buf);
be_buf_t *pop_redo_buf(void);
int delete_undo_redo_buf(be_buf_t *edit_buf);
int delete_do_buf(be_buf_t *edit_buf, be_buf_t *do_buf);
int count_undo_bufs(void);
int count_redo_bufs(void);

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
extern char prev_func_id[MAX_PATH_LEN+1];
void memorize_undo_state_before_change(const char *func_id);
void check_undo_state_after_change(void);
#endif // ENABLE_DEBUG

//-----------------------------------------------------------------------------

void undo_set_region_n_save_before_change(be_line_t *min_line, be_line_t *max_line,
 int cut_buf_lines);
void undo_adjust_max_line(void);
void undo_save_before_change(void);
void undo_save_after_change(void);
int doe_undo(void);
int doe_redo(void);

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_undo_bufs_lines(void);
void dump_redo_bufs_lines(void);
#endif // ENABLE_DEBUG

#endif // ENABLE_UNDO

#endif // editorundo_h

// End of editorundo.h
