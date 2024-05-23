/**************************************************************************
 *   editorcut.h                                                          *
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

#ifndef editorcut_h
#define editorcut_h

int doe_clear_cut_buf(void);
/////int doe_pop_cut_buf(void);

int doe_select_all_lines(void);
int doe_copy_text(void);
int doe_copy_text_to_system_clipboard(void);
int doe_cut_to_head(void);
int doe_cut_to_tail(void);

int doe_tog_mark(void);
int do_set_mark(void);
void do_set_mark_(void);
int do_clear_mark(void);
void do_clear_mark_(void);

int doe_delete_text(void);
int doe_cut_text(void);
int doe_duplicate_text(void);
int doe_paste_text_with_pop(void);
int doe_paste_text_without_pop(void);

#endif // editorcut_h

// End of editorcut.h
