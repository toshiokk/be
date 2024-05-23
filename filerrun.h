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

#define SETTERM0	0
#define SETTERM1	1		// change terminal settings before executing sub process
#define SEPARATE0	0
#define SEPARATE1	1		// output separator line before executing sub process
#define PAUSE0		0
#define PAUSE1		1		// pause after execution of sub process

#ifdef ENABLE_FILER

int dof_exec_command_with_file(void);
int dof_exec_command_with_files(void);
int dof_run_command_rel(void);
int dof_run_command_abs(void);
int dof_run_command_src_dst(void);
int dof_run_command_sh(void);
int dof_run_command_soon(void);

//-----------------------------------------------------------------------------

void begin_fork_exec_repeat(void);
void end_fork_exec_repeat(void);
int fork_exec_sh_c_once(int pause_aft_exec, const char *command);
int fork_exec_sh_c_repeat(int separate_bef_exec, const char *command);
int fork_exec_args_once(int pause_aft_exec, ...);
int fork_exec_args_repeat(int separate_bef_exec, ...);

#endif // ENABLE_FILER

int send_to_system_clipboard();

int fork_exec_sh_c(int set_term, int separate_bef_exec, int pause_aft_exec, const char *command);

void clear_fork_exec_counter(void);
int get_fork_exec_counter(void);
int inc_fork_exec_counter(void);

void pause_after_exec(void);
int restore_term_for_shell(void);
int reinit_term_for_filer(void);

#endif // filerrun_h

// End of filerrun.h
