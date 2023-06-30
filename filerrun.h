/**************************************************************************
 *   filerrun.h                                                           *
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

#ifndef filerrun_h
#define filerrun_h

#ifdef ENABLE_FILER

int do_run_line(void);
int do_exec_command_with_file(void);
int do_exec_command_with_files(void);
int do_run_command_rel(void);
int do_run_command_abs(void);
int do_run_command_src_dst(void);
int do_run_command_sh(void);

int do_filer_cmd_1(void);
int do_filer_cmd_2(void);
int do_filer_cmd_3(void);
int do_filer_cmd_4(void);

#endif // ENABLE_FILER

#endif // filerrun_h

// End of filerrun.h
