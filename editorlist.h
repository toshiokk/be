/**************************************************************************
 *   editorlist.h                                                         *
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

#ifndef editorlist_h
#define editorlist_h

int do_switch_to_file_list(void);

void init_help_bufs(void);
be_buf_t *get_help_buf(int help_buf_idx);
void free_help_bufs(void);

#ifdef ENABLE_HELP
int do_switch_to_key_list(void);
int do_switch_to_func_list(void);
#endif // ENABLE_HELP

#endif // editorlist_h

// End of editorlist.h
