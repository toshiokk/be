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

#define EX_MOD_MASK		0x000f
#define EX_MOD_0		0x0000		// "./command"
#define EX_MOD_1		0x0001		// "command"
#define EX_MOD_2		0x0002		// "/full/path/to/command"
#define EX_MOD_3		0x0003		// "/full/path/to/command"
#define EX_MOD_4		0x0004		// "sh command"
#define EX_MOD_5		0x0005		// "<real file of symlink>
#define EX_MOD_6		0x0006		// "/path/to/dir-A/file-A /path/to/dir-B/file-A"
#define EX_MOD_7		0x0007		// "/path/to/dir-A/file-A /path/to/dir-B/file-B"
#define EX_SETTERM		0x0010		// change terminal settings before executing sub process
#define EX_SEPARATE		0x0040		// output separator line before executing sub process
#define EX_LOGGING		0x0100		// append
#define EX_SOON			0x0400		// run soon without confirmation
#define EX_PAUSE		0x1000		// pause after execution of sub process
#define EX_FLAGS_0		EX_LOGGING

#ifdef ENABLE_FILER

void dof_exec_command_with_file();
void dof_exec_command_with_files();
void dof_run_command_rel();
void dof_run_command_abs();
void dof_run_command_shell();
void dof_run_command_symlink();
void dof_run_command_src_dst_dir();
void dof_run_command_src_dst_file();
void dof_run_command_soon();

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

#define MAX_LOG_FILE_SIZE_KB		1000	// 1000 [KB]
int reduce_log_file_size(const char *file_path, int size_in_kb);
const char *get_exec_log_file_path();

#endif // filerrun_h

// End of filerrun.h
