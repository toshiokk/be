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

#define SETTERM0	0x0000
#define SETTERM1	0x0001		// change terminal settings before executing sub process
#define SEPARATE0	0x0000
#define SEPARATE1	0x0004		// output separator line before executing sub process
#define PAUSE0		0x0000
#define PAUSE1		0x0010		// pause after execution of sub process
#define LOGGING0	0x0000						// no logging
#define LOGGING1	0x0040						// truncate
#define LOGGING2	0x0080						// append
#define LOGGING3	((LOGGING1) | (LOGGING2))	// output

#ifdef ENABLE_FILER

int dof_exec_command_with_file(void);
int dof_exec_command_with_files(void);
int dof_run_command_rel(void);
int dof_run_command_abs(void);
int dof_run_command_src_dst(void);
int dof_run_command_symlink(void);
int dof_run_command_soon(void);
int dof_run_command_soon_w_log(void);

//------------------------------------------------------------------------------

void begin_fork_exec_repeat(void);
void end_fork_exec_repeat(int exit_status);
int fork_exec_sh_c_once(int flags, const char *command);
int fork_exec_sh_c_repeat(int flags, const char *command);
int fork_exec_args_once(int flags, ...);
int fork_exec_args_repeat(int flags, ...);

#endif // ENABLE_FILER

int send_to_system_clipboard();

int fork_exec_sh_c(int flags, const char *command);

void clear_fork_exec_counter(void);
int get_fork_exec_counter(void);
int inc_fork_exec_counter(void);

void pause_after_exec(int exit_status);
int restore_term_for_shell(void);
int reinit_term_for_filer(void);

const char *get_exec_log_file_path();

#endif // filerrun_h

// End of filerrun.h
