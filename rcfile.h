/**************************************************************************
 *   rcfile.h                                                             *
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

#ifndef rcfile_h
#define rcfile_h

#ifdef ENABLE_RC

int read_rc_file(const char *rc_file_name);
#ifdef ENABLE_SYNTAX
int register_default_color_syntax(void);
#endif // ENABLE_SYNTAX
void free_file_types(void);

#ifdef ENABLE_DEBUG
void dump_app_mode(void);
#endif // ENABLE_DEBUG

#endif // ENABLE_RC

#ifdef ENABLE_SYNTAX
#ifdef ENABLE_DEBUG
void dump_file_types(void);
void dump_file_type(const file_type_t *file_type, int syntax);
void dump_color_syntax(const color_syntax_t *clr_syntax);
#endif // ENABLE_DEBUG
#endif // ENABLE_SYNTAX

#endif // rcfile_h

// End of rcfile.h
