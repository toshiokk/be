/**************************************************************************
 *   filerrun.c                                                           *
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

#include "headers.h"

#define MAX_EXECV_ARGS		10

PRIVATE int fork_exec_before_after(int flags, const char *command, char * const args[]);

#ifdef ENABLE_FILER

//------------------------------------------------------------------------------

#define STR_TO_BE_REPLACED_WITH_FILE_NAME		"{}"
#define STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN	strlen(STR_TO_BE_REPLACED_WITH_FILE_NAME)

// If two or more files selected, process them one by one iterating execution.
// "command file-1"
// "command file-2"
// "command ..."
void dof_exec_command_with_file()
{
#define MAX_REPLACEMENTS	10
	char command_str[MAX_PATH_LEN+1];
	if (chk_inp_str_ret_val_filer(input_string_pos("", command_str, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Execute: ({} will be replaced with file-name):")))) {
		return;
	}
	int flags = EX_FLAGS_0;
	if (filer_do_next == EF_ENTER_STRING_ADD) {
		flags ^= EX_LOGGING;	// invert logging
	}
	if (is_path_dir(command_str) > 0) {
		filer_chdir(command_str);
		return;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0; file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;

		// replace "{}" with filename
		// e.g. "cp -auv {} dir/{}" ==> "cp -auv filename dir/filename"
		char buffer[MAX_PATH_LEN+1];
		strlcpy__(buffer, command_str, MAX_PATH_LEN);
		for (int cnt = 0; cnt < MAX_REPLACEMENTS; cnt++) {
			char *ptr_replace;
			if ((ptr_replace = strstr(buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME))
			 == NULL)
				break;
			replace_str(buffer, MAX_PATH_LEN,
			 ptr_replace - buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN,
			 quote_file_path_s(get_cur_fv_file_name(file_idx)), -1);
		}
		exit_status = fork_exec_sh_c_repeat(EX_SEPARATE | flags, buffer);

	}
	end_fork_exec_repeat(exit_status);
	SET_filer_do_next(FL_UPDATE_FORCE);
}

PRIVATE int input_command_line_and_execute(char *command_str, int flags);

// If two or more files selected, pass all to command line at once.
// "command file-1 file-2 ..."
void dof_exec_command_with_files()
{
	char command_str[MAX_PATH_LEN+1] = "";
	// "file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_fv_file_name(file_idx));
	}

	input_command_line_and_execute(command_str, EX_FLAGS_0);
}

PRIVATE int _dof_run_command_rel_abs();
PRIVATE int _dof_run_command(int flags);
void dof_run_command_rel()
{
	_dof_run_command_rel_abs(EX_MOD_0 | EX_FLAGS_0);
}
void dof_run_command_abs()
{
	_dof_run_command_rel_abs(EX_MOD_2 | EX_FLAGS_0);
}
PRIVATE int _dof_run_command_rel_abs(int flags)
{
	if (get_files_selected_cfv() == 0) {
		struct stat *st_ptr = &get_cur_fv_cur_file_info()->st;
		if ((st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
			_dof_run_command(flags | EX_MOD_0 | EX_FLAGS_0);
		} else {
			_dof_run_command(flags | EX_MOD_1 | EX_FLAGS_0);
		}
		return 0;
	}

	char command_str[MAX_PATH_LEN+1] = " ";
	// "file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if ((flags & EX_MOD_MASK) == 0) {
			concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
			 get_cur_fv_file_name(file_idx));
		} else {
			concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
			 concat_dir_and_file_s(get_fv_from_cur_pane()->cur_dir,
			  get_cur_fv_file_name(file_idx)));
		}
	}

	return input_command_line_and_execute(command_str, flags);
}
PRIVATE int input_command_line_and_execute(char *command_str, int flags)
{
	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Execute with files:")))) {
		return 0;
	}
	if (filer_do_next == EF_ENTER_STRING_ADD) {
		flags ^= EX_LOGGING;	// invert logging
	}
	fork_exec_sh_c_once(flags | EX_PAUSE, command_str);
	SET_filer_do_next(FL_UPDATE_FORCE);
	return 0;
}
void dof_run_command_shell()
{
	_dof_run_command(EX_MOD_4 | EX_FLAGS_0);
}
void dof_run_command_symlink()
{
	_dof_run_command(EX_MOD_5 | EX_FLAGS_0);
}
void dof_run_command_src_dst_dir()
{
	_dof_run_command(EX_MOD_6 | EX_FLAGS_0);
}
void dof_run_command_src_dst_file()
{
	_dof_run_command(EX_MOD_7 | EX_FLAGS_0);
}
void dof_run_command_soon()
{
	_dof_run_command(EX_MOD_0 | EX_SOON);
}
PRIVATE int _dof_run_command(int flags)
{
	const char *prompt = "";
	char buf1[MAX_PATH_LEN+1];
	char buf2[MAX_PATH_LEN+1];
	char command_str[MAX_PATH_LEN+1] = "";
	int src_pane_idx = get_filer_cur_pane_idx();
	int dst_pane_idx = get_filer_another_pane_idx();

	switch (flags & EX_MOD_MASK) {
	default:
	case EX_MOD_0:
		prompt = _("Run: (current-directory-file)");
		snprintf_(command_str, MAX_PATH_LEN, "./%s ",
		 quote_file_path_s(get_cur_fv_cur_file_name()));
		break;
	case EX_MOD_1:
		prompt = _("Run: (with file)");
		snprintf_(command_str, MAX_PATH_LEN, " %s",
		 quote_file_path_s(get_cur_fv_cur_file_name()));
		break;
	case EX_MOD_2:
	case EX_MOD_3:
		prompt = _("Run: (with abs-path)");
		quote_file_path_buf(command_str, concat_dir_and_file_s(
		 get_fv_from_cur_pane()->cur_dir, get_cur_fv_cur_file_name()));
		break;
	case EX_MOD_4:
		prompt = _("Run: (script)");
		snprintf_(command_str, MAX_PATH_LEN, "sh %s",
		 quote_file_path_s(get_cur_fv_cur_file_name()));
		break;
	case EX_MOD_5:
		prompt = _("Run: (symlink)");
		snprintf_(command_str, MAX_PATH_LEN, "%s",
		 (get_cur_fv_cur_file_info()->symlink != NULL)
		  ? quote_file_path_s(get_cur_fv_cur_file_info()->symlink)
		  : quote_file_path_s(get_cur_fv_cur_file_name()));
		break;
	case EX_MOD_6:
		// " /path/to/dir-A/file-A /path/to/dir-B/file-A"
		prompt = _("Run: (with SRC-dir and DEST-dir)");
		snprintf_(command_str, MAX_PATH_LEN, " %s %s",
		 quote_file_path_buf(buf1, concat_dir_and_file_s1(
		  get_fv_from_pane(src_pane_idx)->cur_dir, get_cur_fv_cur_file_name())),
		 quote_file_path_buf(buf2, concat_dir_and_file_s2(
		  get_fv_from_pane(dst_pane_idx)->cur_dir, get_cur_fv_cur_file_name())));
		break;
	case EX_MOD_7:
		// " /path/to/dir-A/file-A /path/to/dir-B/file-B"
		prompt = _("Run: (with SRC-file and DEST-file)");
		snprintf_(command_str, MAX_PATH_LEN, " %s %s",
		 quote_file_path_buf(buf1, concat_dir_and_file_s1(
		  get_fv_from_pane(src_pane_idx)->cur_dir,
		  get_cur_fv_file_ptr_from_pane(src_pane_idx)->file_name)),
		 quote_file_path_buf(buf2, concat_dir_and_file_s2(
		  get_fv_from_pane(dst_pane_idx)->cur_dir,
		  get_cur_fv_file_ptr_from_pane(dst_pane_idx)->file_name)));
		break;
	}

	if (flags & EX_SOON) {
		// run soon without editing command line
	} else {
		char explanation[MAX_PATH_LEN+1];
		snprintf(explanation, MAX_PATH_LEN, "%s:", prompt);
		struct stat *st_ptr = &get_cur_fv_cur_file_info()->st;
		if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str,
		 (S_ISREG(st_ptr->st_mode) && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
		  ? MAX_PATH_LEN : 0,
		 HISTORY_TYPE_IDX_EXEC,
		 explanation))) {
			return 0;
		}
		if (filer_do_next == EF_ENTER_STRING_ADD) {
			flags ^= EX_LOGGING;	// invert logging
		}
	}

	if (is_path_dir(command_str) > 0) {
		return filer_chdir(command_str);
	}

	fork_exec_sh_c_once(flags | EX_PAUSE, command_str);

	if (is_app_chooser_mode()) {
		SET_filer_do_next(EF_EXECUTED_RET_TO_CALLER);
	} else {
		SET_filer_do_next(FL_UPDATE_FORCE);
	}
	return 0;
}

//------------------------------------------------------------------------------
void begin_fork_exec_repeat()
{
	restore_term_for_shell();
	clear_fork_exec_counter();
	clear_sigint_signaled();
}
void end_fork_exec_repeat(int exit_status)
{
	pause_after_exec(exit_status);
	reinit_term_for_filer();
	// avoid screen flashing at the first key input after execution
	clear_whole_screen_update_timer();
}

//------------------------------------------------------------------------------

PRIVATE int args_from_va_list(char **args, va_list ap);
PRIVATE int fork_exec_args(int flags, char * const args[]);
PRIVATE const char *exec_args_to_str(char * const *args);

int fork_exec_sh_c_once(int flags, const char *command)
{
	return fork_exec_sh_c(flags | EX_SETTERM | EX_SEPARATE | flags, command);
}
int fork_exec_sh_c_repeat(int flags, const char *command)
{
	return fork_exec_sh_c(flags, command);
}

int fork_exec_args_once(int pause_aft_exec, ...)
{
	char *args[MAX_EXECV_ARGS+1];
	va_list ap;
	va_start(ap, pause_aft_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_exec_args(EX_SETTERM | EX_SEPARATE | pause_aft_exec, args);
}
int fork_exec_args_repeat(int flags, ...)
{
	char *args[MAX_EXECV_ARGS+1];
	va_list ap;
	va_start(ap, flags);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_exec_args(flags, args);
}

// convert "va_list" to "char *argv[]"
// Note: "va_list" and "args" are opposite in the order of the arguments.
PRIVATE int args_from_va_list(char **args, va_list ap)
{
	int arg_idx;
	for (arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		args[arg_idx] = va_arg(ap, char *);
		if (args[arg_idx] == NULL)
			break;
	}
	args[arg_idx] = NULL;
	return arg_idx;
}

PRIVATE int fork_exec_args(int flags, char * const args[])
{
	if (flags & EX_SETTERM) {
		clear_fork_exec_counter();
	}
	const char *command = exec_args_to_str(args);
	return fork_exec_sh_c(flags, command);
}

PRIVATE const char *exec_args_to_str(char * const *args)
{
	static char buffer[MAX_PATH_LEN+1];
	strcpy__(buffer, "");
	for (int arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		if (args[arg_idx] == NULL)
			break;
		concat_file_path_separating_by_space(buffer, MAX_PATH_LEN, args[arg_idx]);
	}
	return buffer;
}

#endif // ENABLE_FILER

//------------------------------------------------------------------------------

int fork_exec_sh_c(int flags, const char *command)
{
	char * args[MAX_EXECV_ARGS+1];

	flags |= EX_LOGGING;
	if (flags & EX_SETTERM) {
		clear_fork_exec_counter();
	}

	if (check_availability_of_script() < 0) {
		flags &= ~EX_LOGGING;		// turn off logging
	}
	hmflf_dprintf("command: [%s]\n", command);
	if ((flags & EX_LOGGING) == 0) {
#define SH_PROG			"sh"
		args[0] = SH_PROG;
		args[1] = "-c";
		args[2] = (char *)command;
		args[3] = NULL;
		// "sh -c <command ...>"
		hmflf_dprintf("exec: [%s] [%s] [%s]\n", args[0], args[1], args[2]);
	} else {
#define SCRIPT_PROG		"script"
		args[0] = SCRIPT_PROG;
		args[1] = "-q";
		args[2] = "-O";
		args[3] = (char *)get_exec_log_file_path();
		args[4] = "-a";
		args[5] = "-c";
		args[6] = (char *)command;
		args[7] = NULL;
		// "script -q -O <log_file> -a -c <command ...>"
		hmflf_dprintf("exec: [%s] [%s] [%s] [%s] [%s] [%s] [%s]\n",
		 args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
	}

	return fork_exec_before_after(flags, command, args);
}

PRIVATE int fork_exec_before_after(int flags, const char *command, char * const args[])
{
#ifdef ENABLE_HISTORY
	dir_history_fix();
#endif // ENABLE_HISTORY

	// It does not output "sh -c [command arg1 arg2]"
	//           but output only "command arg1 arg2"
#ifdef ENABLE_HISTORY
	// save to file soon, because a command execution may take long time
	modify_save_history(HISTORY_TYPE_IDX_EXEC, command);
	hmflf_dprintf("exec: [%s]\n", command);
#endif // ENABLE_HISTORY

	if ((flags & EX_SETTERM) && (get_fork_exec_counter() == 0)) {
		restore_term_for_shell();
	}
	if ((flags & EX_SEPARATE) && (get_fork_exec_counter() == 0)) {
		char header[MAX_PATH_LEN+1];
		snprintf(header, MAX_PATH_LEN, /*"== %c%s%c%c %s ==\n",*/ "%c%s%c\n%c %s\n",
		 (flags & EX_LOGGING) ? '<' : '[',
		 get_full_path_of_cur_dir(NULL),
		 (flags & EX_LOGGING) ? '>' : ']',
		 (geteuid() == 0) ? '#' : '$', command);
		// output separator line to log file
		if (flags & EX_LOGGING) {
			write_text_to_file(get_exec_log_file_path(), 1, header);
		}
		// output separator line to console
		printf("\n%s", header);
		fflush(stdout);
	}

	pid_t pid;
	int exit_status = -10000;
	if ((pid = fork()) == 0) {
		signal_clear();
		init_stderr();
		//-----------------------------
		execvp(args[0], args);
		//-----------------------------
		exit(-10001);					// execution error
	} else {
		for ( ; ; ) {
			pid = waitpid(pid, &exit_status, 0);
			hmflf_dprintf("pid: %d, exit_status: %d\n", pid, exit_status);
			if (pid != -1) {
				break;
			}
		}
	}

	if (flags & EX_PAUSE) {
		pause_after_exec(exit_status);
	}
	if ((flags & EX_SETTERM) && (get_fork_exec_counter() == 0)) {
		reinit_term_for_filer();
	}
	inc_fork_exec_counter();
	if (flags & EX_LOGGING) {
		write_text_to_file(get_exec_log_file_path(), 1, "\n");
	}
	return exit_status;
}

PRIVATE int fork_exec_counter = 0;
void clear_fork_exec_counter()
{
	fork_exec_counter = 0;
}
int get_fork_exec_counter()
{
	return fork_exec_counter;
}
int inc_fork_exec_counter()
{
	return fork_exec_counter++;
}

void pause_after_exec(int exit_status)
{
	set_term_raw();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	for (int chars = 0; chars < 1000; chars++) {
		if (getchar() == EOF)
			break;
	}
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	//      12345678901234567890123456789012345678901234567890123456789012345678901234567890
	printf("\r======== Hit any key to return to %s ======== ", APP_NAME);
	fflush(stdout);
	getchar();
	o_printf("\r\nBE_EDITOR_A{\r\n");
}

//------------------------------------------------------------------------------
int restore_term_for_shell()
{
	o_printf("\r\nBE_EDITOR_Z}\r\n");
	tio_suspend();
	signal_fork();
	return 0;
}
int reinit_term_for_filer()
{
	signal_init();
	tio_resume();
	tio_fill_screen();
	return 0;
}

//------------------------------------------------------------------------------
int reduce_log_file_size(const char *file_path, int size_in_kb)
{
	if (get_file_size(file_path) <= ((ssize_t)size_in_kb * 1024)) {
		return 0;	// no need to reduce
	}

	char command_str[MAX_PATH_LEN+1] = "";
	// "tail -c 500K 1.log >1.log~; mv -vf 1.log~ 1.log"
	snprintf_(command_str, MAX_PATH_LEN, "tail -c %dK %s >%s%s ; mv -vf %s%s %s",
	 size_in_kb / 2,
	 file_path, file_path, BACKUP_FILE_SUFFIX,
	 file_path, BACKUP_FILE_SUFFIX, file_path);

	return fork_exec_sh_c(EX_FLAGS_0, command_str);
}

const char *get_exec_log_file_path()
{
	// /dev/tty1  => "/home/user/.be/tty1.log"
	// /dev/pts/1 => "/home/user/.be/1.log"
	static char file_path[MAX_PATH_LEN+1] = "";
	char dir[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];
	if (is_strlen_0(file_path)) {
		separate_path_to_dir_and_file(get_tty_name(), dir, file);
		concat_dir_and_file(file_path, get_app_dir(), sprintf_s("%s.log", file));
	}
	return file_path;
}

// End of filerrun.c
