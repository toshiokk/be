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

// a command line which would be passed to 'fork_exec_sh_c_once()'
// |command function          | command line format                             |
// |----------------------------------------------------------------------------|
// |dof_exec_for_each_files   | 'cmd file1', 'cmd file2', ...                   |
// |dof_exec_for_each_paths   | 'cmd /path/to/file1', 'cmd /path/to/file2' ...  |
// |dof_run_for_files         | <file-1> <file-2> <file-3> ...                  |
// |dof_run_for_paths         | </path/to/file1> </abs/path/to/file2> ...       |
// |dof_run_command_immediate | ./<file1> <file2> <file3> ...                   |
// |dof_run_command_cur_dir   | ./<file1> <file2> <file3> ...                   |
// |dof_run_command_shell     | sh <file1> <file2> <file3> ...                  |
// |dof_run_for_symlink       | <target-path-of-symlink>                        |
// |dof_run_for_src_dst_files | </src/path/to/file-1> </dest/path/to/file-1>    |
// |dof_run_for_src_dst_paths | </src/path/to/file-1> </dest/path/to/file-2>    |

#define STR_TO_BE_REPLACED_WITH_FILE_NAME		"{}"
#define STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN	strlen(STR_TO_BE_REPLACED_WITH_FILE_NAME)

// If two or more files selected, process them one by one iterating execution.
// "command file-1"
// "command file-2"
// "command ..."
PRIVATE void _dof_exec_for_each_files(int flags);
void dof_exec_for_each_files()
{
	_dof_exec_for_each_files(EX_FLAGS_0 | EX_CL_0);
}
void dof_exec_for_each_paths()
{
	_dof_exec_for_each_files(EX_FLAGS_0 | EX_CL_1);
}
PRIVATE void _dof_exec_for_each_files(int flags)
{
#define MAX_REPLACEMENTS	10
	char command_str[MAX_PATH_LEN+1] = "";
	if (get_cfv_files_selected() == 0) {
		// ""
	} else if (get_cfv_files_selected() == 1) {
		// " file"
		snprintf(command_str, MAX_PATH_LEN, " %s",
		 quote_file_path_s(get_cfv_file_name(get_first_file_idx_selected())));
	} else {
		// " {}"
		snprintf(command_str, MAX_PATH_LEN, " %s", STR_TO_BE_REPLACED_WITH_FILE_NAME);
	}
	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Execute: ({} will be replaced with file-name)")))) {
		return;
	}
	if (is_path_dir(command_str) > 0) {
		filer_chdir(command_str);
		return;
	}
	int exit_status = 0;
	begin_fork_exec_repeat();
	for (int file_idx = get_first_file_idx_selected(); file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		// replace "{}" with filename
		// e.g. "cp -auv {} dir/{}" ==> "cp -auv filename dir/filename"
		char buffer[MAX_PATH_LEN+1];
		strlcpy__(buffer, command_str, MAX_PATH_LEN);
		for (int cnt = 0; cnt < MAX_REPLACEMENTS; cnt++) {
			char *ptr_replace;
			if ((ptr_replace = strstr(buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME))
			 == NULL)
				break;
			const char *file_path;
			switch (flags & EX_CL_MASK) {
			default:
			case EX_CL_0:
				file_path = get_cfv_file_name(file_idx);
				break;
			case EX_CL_1:
				file_path = concat_dir_and_file(NULL, get_fv_from_cur_pane()->cur_dir,
				 get_cfv_file_name(file_idx));
				break;
			}
			replace_str(buffer, MAX_PATH_LEN,
			 ptr_replace - buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN,
			 quote_file_path_s(file_path), -1);
		}
		exit_status = fork_exec_sh_c_repeat(EX_FLAGS_0 | EX_SEPARATE | flags, buffer);
		if (do_check_break_key()) {
			break;
		}
	}
	end_fork_exec_repeat(exit_status);
	SET_app_do_next(FL_UPDATE_FORCE);
}

// If two or more files selected, pass all to command line at once.
// "command file-1 file-2 ..."

PRIVATE void _dof_run_command_once(int flags);
void dof_run_for_files()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_0);
}
void dof_run_for_paths()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_1);
}
void dof_run_for_symlink()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_2);
}
void dof_run_command_immediate()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_3 | EX_IMMEDIATE);
}
void dof_run_command_cur_dir()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_3);
}
void dof_run_command_shell()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_4);
}
void dof_run_for_src_dst_files()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_5);
}
void dof_run_for_src_dst_paths()
{
	_dof_run_command_once(EX_FLAGS_0 | EX_CL_6);
}

PRIVATE void input_command_line_and_execute(const char *prompt, char *command_str,
 __mode_t st_mode, int flags);
PRIVATE void _dof_run_command_once(int flags)
{
	char buf1[MAX_PATH_LEN+1];
	char buf2[MAX_PATH_LEN+1];
	char command_str[MAX_PATH_LEN+1] = "";
	int src_pane_idx = get_filer_cur_pane_idx();
	int dst_pane_idx = get_filer_another_pane_idx();
	__mode_t st_mode = get_cfv_file_st_mode(get_first_file_idx_selected());

	switch (flags & EX_CL_MASK) {
	default:
	case EX_CL_0:
	case EX_CL_1:
	case EX_CL_2:
	case EX_CL_3:
	case EX_CL_4:
		// "file1 file2 ..."
		char file_paths[MAX_PATH_LEN+1] = "";
		for (int file_idx = get_first_file_idx_selected(); file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			switch (flags & EX_CL_MASK) {
			default:
			case EX_CL_0:
			case EX_CL_3:
			case EX_CL_4:
				concat_file_path_separating_by_space(file_paths, MAX_PATH_LEN,
				 get_cfv_file_name(file_idx));
				break;
			case EX_CL_1:
				concat_file_path_separating_by_space(file_paths, MAX_PATH_LEN,
				 concat_dir_and_file(NULL, get_fv_from_cur_pane()->cur_dir,
				  get_cfv_file_name(file_idx)));
				break;
			case EX_CL_2:
				if (get_cfv_symlink(file_idx) == NULL) {
					concat_file_path_separating_by_space(file_paths, MAX_PATH_LEN,
					 get_cfv_file_name(file_idx));
				} else {
					concat_file_path_separating_by_space(file_paths, MAX_PATH_LEN,
					 get_cfv_file_name(file_idx));
					concat_file_path_separating_by_space(file_paths, MAX_PATH_LEN,
					 get_cfv_symlink(file_idx));
				}
				break;
			}
		}
		switch (flags & EX_CL_MASK) {
		default:
		case EX_CL_0:
			if (IS_ST_EXECUTABLE(st_mode)) {
				snprintf_(command_str, MAX_PATH_LEN, "./%s ", file_paths);
			} else {
				snprintf_(command_str, MAX_PATH_LEN, " %s", file_paths);
			}
			break;
		case EX_CL_1:
		case EX_CL_2:
			snprintf_(command_str, MAX_PATH_LEN, " %s", file_paths);
			break;
		case EX_CL_3:
			snprintf_(command_str, MAX_PATH_LEN, "./%s ", file_paths);
			break;
		case EX_CL_4:
			snprintf_(command_str, MAX_PATH_LEN, "sh %s ", file_paths);
			break;
		}
		break;
	case EX_CL_5:
		// " /path/to/dir-A/file-A /path/to/dir-B/file-A"
		snprintf_(command_str, MAX_PATH_LEN, " %s %s",
		 quote_file_path_buf(buf1, concat_dir_and_file_s1(
		  get_fv_from_pane(src_pane_idx)->cur_dir, get_cfv_file_name(-1))),
		 quote_file_path_buf(buf2, concat_dir_and_file_s2(
		  get_fv_from_pane(dst_pane_idx)->cur_dir, get_cfv_file_name(-1))));
		break;
	case EX_CL_6:
		// " /path/to/dir-A/file-A /path/to/dir-B/file-B"
		snprintf_(command_str, MAX_PATH_LEN, " %s %s",
		 quote_file_path_buf(buf1, concat_dir_and_file_s1(
		  get_fv_from_pane(src_pane_idx)->cur_dir,
		  get_cfv_file_ptr_from_pane(src_pane_idx)->file_name)),
		 quote_file_path_buf(buf2, concat_dir_and_file_s2(
		  get_fv_from_pane(dst_pane_idx)->cur_dir,
		  get_cfv_file_ptr_from_pane(dst_pane_idx)->file_name)));
		break;
	}
	const char *prompt = "";
	switch (flags & EX_CL_MASK) {
	default:
	case EX_CL_0:	prompt = _("Run: (with file)");					break;
	case EX_CL_1:	prompt = _("Run: (with abs-path)");				break;
	case EX_CL_2:	prompt = _("Run: (symlink)");					break;
	case EX_CL_3:	prompt = _("Run: (current-directory-file)");	break;
	case EX_CL_4:	prompt = _("Run: (script)");					break;
	case EX_CL_5:	prompt = _("Run: (with SRC-dir and DEST-dir)");		break;
	case EX_CL_6:	prompt = _("Run: (with SRC-file and DEST-file)");	break;
	}

	input_command_line_and_execute(prompt, command_str, st_mode, flags);
}
PRIVATE void input_command_line_and_execute(const char *prompt, char *command_str,
 __mode_t st_mode, int flags)
{
	if (flags & EX_IMMEDIATE) {
		// run soon without editing command line
	} else {
		if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str,
		 (IS_ST_EXECUTABLE(st_mode) || ((flags & EX_CL_MASK) == EX_CL_4)) ? MAX_PATH_LEN : 0,
		 HISTORY_TYPE_IDX_EXEC,
		 prompt))) {
			return;
		}
	}

	if (is_path_dir(command_str) > 0) {
		filer_chdir(command_str);
		return;
	}

	fork_exec_sh_c_once(flags | EX_PAUSE, command_str);

	if (is_app_viewer_mode()) {
		SET_app_do_next(EF_EXECUTED_RET_TO_CALLER);
	} else {
		SET_app_do_next(FL_UPDATE_FORCE);
	}
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
	clear_msec_past_after_key_input();
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

	return fork_exec_args(EX_FLAGS_0 | EX_SETTERM | EX_SEPARATE | pause_aft_exec, args);
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
	// It does not output "sh -c [command arg1 arg2]"
	//           but output only "command arg1 arg2"
#ifdef ENABLE_HISTORY
	// save to file soon, because a command execution may take long time
	hmflf_dprintf("exec: [%s]\n", command);
	modify_history_w_reloading(HISTORY_TYPE_IDX_EXEC, command);
	sync_cut_buffers_and_histories(1);
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
	for ( ; ; ) {
		if (getchar() >= 0)
			break;
	}
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
#define MAX_LOG_FILE_SIZE_KB		1000	// 1000 [KB]
void reduce_exec_log_file_size()
{
	reduce_log_file_size(get_exec_log_file_path(), MAX_LOG_FILE_SIZE_KB);
}
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
	if (is_strlen_0(file_path)) {
		concat_dir_and_file(file_path,
		 get_app_dir(), sprintf_s("%s.log", get_tty_name_file_part_2_digits()));
	}
	return file_path;
}

// End of filerrun.c
