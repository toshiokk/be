/**************************************************************************
 *   help.h                                                               *
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

#ifndef help_h
#define help_h

#ifndef ENABLE_HELP

#define HELP_BUF_IDX_EDITOR_FILE_LIST		0
#define HELP_BUFS							1

#else // ENABLE_HELP

#ifndef ENABLE_FILER

#define HELP_BUF_IDX_EDITOR_FILE_LIST		0
#define HELP_BUF_IDX_EDITOR_KEY_LIST		1
#define HELP_BUF_IDX_EDITOR_FUNC_LIST		2
#define HELP_BUFS							3

#else // ENABLE_FILER

#define HELP_BUF_IDX_EDITOR_FILE_LIST		0
#define HELP_BUF_IDX_EDITOR_FUNC_LIST		1
#define HELP_BUF_IDX_EDITOR_KEY_LIST		2
#define HELP_BUF_IDX_FILER_FUNC_LIST		3
#define HELP_BUF_IDX_FILER_KEY_LIST			4
#define HELP_BUFS							5

#endif // ENABLE_FILER

#endif // ENABLE_HELP

void init_help_bufs();
be_buf_t *get_help_buf(int help_buf_idx);

int view_list(int help_idx);

#ifdef ENABLE_HELP
void disp_splash(int delay);
#endif // ENABLE_HELP

#endif // help_h

// End of help.h
