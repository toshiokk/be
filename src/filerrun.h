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

#define EX_CL_MASK		0x000f		// EX_MOD: Execution Mode
#define EX_CL_0			0x0000		// "file"
#define EX_CL_1			0x0001		// "/full/path/to/file"
#define EX_CL_2			0x0002		// "<real file of symlink>
#define EX_CL_3			0x0003		// "./command"
#define EX_CL_4			0x0004		// "sh command"
#define EX_CL_5			0x0006		// "/path/to/dir-A/file-A /path/to/dir-B/file-A"
#define EX_CL_6			0x0007		// "/path/to/dir-A/file-A /path/to/dir-B/file-B"
#define EX_SETTERM		0x0010		// change terminal settings before executing sub process
#define EX_SEPARATE		0x0040		// output separator line before executing sub process
#define EX_LOGGING		0x0100		// append
#define EX_IMMEDIATE	0x0400		// run soon without confirmation
#define EX_PAUSE		0x1000		// pause after execution of sub process
#define EX_FLAGS_0		EX_LOGGING

#ifdef ENABLE_FILER

void dof_exec_for_each_files();
void dof_exec_for_each_paths();
void dof_run_for_files();
void dof_run_for_paths();
void dof_run_for_symlink();
void dof_run_command_immediate();
void dof_run_command_cur_dir();
void dof_run_command_shell();
void dof_run_for_src_dst_files();
void dof_run_for_src_dst_paths();

//------------------------------------------------------------------------------

void begin_fork_exec_repeat();
void end_fork_exec_repeat(int exit_status);
int fork_exec_sh_c_once(int flags, const char *command);
int fork_exec_sh_c_repeat(int flags, const char *command);
int fork_exec_args_once(int flags, ...);
int fork_exec_args_repeat(int flags, ...);

#endif // ENABLE_FILER

int fork_exec_sh_c(int flags, const char *command);

void clear_fork_exec_counter();
int get_fork_exec_counter();
int inc_fork_exec_counter();

void pause_after_exec(int exit_status);
int restore_term_for_shell();
int reinit_term_for_filer();

void reduce_exec_log_file_size();
int reduce_log_file_size(const char *file_path, int size_in_kb);
const char *get_exec_log_file_path();

#endif // filerrun_h

// End of filerrun.h
