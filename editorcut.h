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

int doe_select_all_lines();

int doe_delete_to_head();
int doe_cut_to_head();
int doe_copy_text();
int doe_cut_text_to_sys_clipboard();
int doe_copy_text_to_sys_clipboard();
int send_to_sys_clipboard();
int doe_delete_to_tail();
int doe_cut_to_tail();
int doe_clear_all_cut_buffers();

int doe_tog_mark();
int do_set_mark();
void do_set_mark_();
int do_clear_mark();
void do_clear_mark_();

int doe_delete_text();
int doe_cut_text();
int doe_duplicate_text();
int doe_paste_text_with_pop();
int doe_paste_text_without_pop();

#endif // editorcut_h

// End of editorcut.h
