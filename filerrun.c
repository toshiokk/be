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

PRIVATE int dof_run_command_(int mode);

//------------------------------------------------------------------------------

#define STR_TO_BE_REPLACED_WITH_FILE_NAME		"{}"
#define STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN	strlen(STR_TO_BE_REPLACED_WITH_FILE_NAME)

// If two or more files selected, process them one by one iterating execution.
// "command file-1"
// "command file-2"
// "command ..."
int dof_exec_command_with_file(void)
{
	char command_str[MAX_PATH_LEN+1];
	char *ptr_replace;
	char buffer[MAX_PATH_LEN+1];
#define MAX_REPLACEMENTS	10
	int cnt;
	int exit_status = 0;
	int logging = LOGGING1;

	if (chk_inp_str_ret_val_filer(input_string_pos("", command_str, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Execute({} will be replaced with file-name):")))) {
		return 0;
	}
	if (filer_do_next == EF_INPUT_W_ALT_ENTER) {
		logging ^= LOGGING1;	// invert logging
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
	begin_fork_exec_repeat();
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		if (is_sigint_signaled())
			break;
		// replace "{}" with filename
		// e.g. "cp -auv {} dir/{}" ==> "cp -auv filename dir/filename"
		strlcpy__(buffer, command_str, MAX_PATH_LEN);
		for (cnt = 0; cnt < MAX_REPLACEMENTS; cnt++) {
			if ((ptr_replace = strstr(buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME))
			 == NULL)
				break;
			replace_str(buffer, MAX_PATH_LEN,
			 ptr_replace - buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN,
			 quote_file_path_static(get_cur_fv_file_ptr(file_idx)->file_name), -1);
		}
		exit_status = fork_exec_sh_c_repeat(SEPARATE1 | logging, buffer);
	}
	end_fork_exec_repeat(exit_status);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}
// If two or more files selected, pass all to command line at once.
// "command file-1 file-2 ..."
int dof_exec_command_with_files(void)
{
	char command_str[MAX_PATH_LEN+1] = "";
	int logging = LOGGING1;

	// "file1 file2 ..."
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_path_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_fv_file_ptr(file_idx)->file_name);
	}

	if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Execute with files%s:"), (logging == 0) ? "" : _("(WITH LOG)")))) {
		return 0;
	}
	if (filer_do_next == EF_INPUT_W_ALT_ENTER) {
		logging ^= LOGGING1;	// invert logging
	}
	fork_exec_sh_c_once(logging | PAUSE1 | logging, command_str);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}

#define RUN_MOD_MASK	0x0f
#define RUN_SOON		0x10
#define RUN_WITH_LOG	0x20
int dof_run_command_rel()
{
	struct stat *st_ptr = &get_cur_fv_cur_file_ptr()->st;
	if ((st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
		dof_run_command_(0);
	} else {
		dof_run_command_(1);
	}
	return 0;
}
int dof_run_command_abs(void)
{
	return dof_run_command_(2);
}
int dof_run_command_symlink(void)
{
	return dof_run_command_(4);
}
int dof_run_command_src_dst_dir(void)
{
	return dof_run_command_(5);
}
int dof_run_command_src_dst_file(void)
{
	return dof_run_command_(6);
}
int dof_run_command_soon(void)
{
	return dof_run_command_(0 | RUN_SOON);
}
int dof_run_command_soon_w_log(void)
{
	return dof_run_command_(0 | RUN_SOON | RUN_WITH_LOG);
}
PRIVATE int dof_run_command_(int mode)
{
	const char *expl;
	char buf_s[MAX_PATH_LEN+1];
	char buf_d[MAX_PATH_LEN+1];
	char buf1[MAX_PATH_LEN+1];
	char buf2[MAX_PATH_LEN+1];
	char explanation[MAX_PATH_LEN+1];
	char command_str[MAX_PATH_LEN+1] = "";
	int src_fv_idx = get_filer_cur_pane_idx();
	int dst_fv_idx = get_filer_another_pane_idx();

	switch (mode & RUN_MOD_MASK) {
	default:
	case 0:
		expl = _("Run (current-directory-file)");
		snprintf_(command_str, MAX_PATH_LEN+1, "./%s ",
		 quote_file_path_static(get_cur_fv_cur_file_ptr()->file_name));
		break;
	case 1:
		expl = _("Run (with file)");
		snprintf_(command_str, MAX_PATH_LEN+1, " %s",
		 quote_file_path_static(get_cur_fv_cur_file_ptr()->file_name));
		break;
	case 2:
		expl = _("Run (with real-path)");
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_pane_view()->cur_dir, get_cur_fv_cur_file_ptr()->file_name);
		quote_file_path_buf(command_str, buf_s);
		break;
	case 3:
		expl = _("Run (script)");
		snprintf_(command_str, MAX_PATH_LEN+1, ". %s",
		 quote_file_path_static(get_cur_fv_cur_file_ptr()->file_name));
		break;
	case 4:
		expl = _("Run (symlink)");
		snprintf_(command_str, MAX_PATH_LEN+1, "%s",
		 (get_cur_fv_cur_file_ptr()->symlink != NULL)
		  ? quote_file_path_static(get_cur_fv_cur_file_ptr()->symlink)
		  : quote_file_path_static(get_cur_fv_cur_file_ptr()->file_name));
		break;
	case 5:
		// " /path/to/dir-A/file-A /path/to/dir-B/file-A"
		expl = _("Run (with SRC-dir and DEST-dir)");
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_view(src_fv_idx)->cur_dir, get_cur_fv_cur_file_ptr()->file_name);
		snprintf_(buf_d, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_view(dst_fv_idx)->cur_dir, get_cur_fv_cur_file_ptr()->file_name);
		snprintf_(command_str, MAX_PATH_LEN+1, " %s %s",
		 quote_file_path_buf(buf1, buf_s),
		 quote_file_path_buf(buf2, buf_d));
		break;
	case 6:
		// " /path/to/dir-A/file-A /path/to/dir-B/file-B"
		expl = _("Run (with SRC-file and DEST-file)");
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_view(src_fv_idx)->cur_dir, get_fv_file_ptr(src_fv_idx)->file_name);
		snprintf_(buf_d, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_view(dst_fv_idx)->cur_dir, get_fv_file_ptr(dst_fv_idx)->file_name);
		snprintf_(command_str, MAX_PATH_LEN+1, " %s %s",
		 quote_file_path_buf(buf1, buf_s),
		 quote_file_path_buf(buf2, buf_d));
		break;
	}

	int logging = LOGGING0;
	if (mode & RUN_WITH_LOG) {
		logging = LOGGING1;
	}
	if (mode & RUN_SOON) {
		// run soon without editing command line
	} else {
		snprintf(explanation, MAX_PATH_LEN, "%s%s:",
		 expl, (logging == 0) ? "" : _("(WITH LOG)"));
		struct stat *st_ptr = &get_cur_fv_cur_file_ptr()->st;
		if (chk_inp_str_ret_val_filer(input_string_pos(command_str, command_str,
		 (S_ISREG(st_ptr->st_mode) && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
		  ? MAX_PATH_LEN : 0,
		 HISTORY_TYPE_IDX_EXEC,
		 explanation))) {
			return 0;
		}
		if (filer_do_next == EF_INPUT_W_ALT_ENTER) {
			logging ^= LOGGING1;	// invert logging
		}
	}

	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}

flf_d_printf("command_str [%s]\n", command_str);
	fork_exec_sh_c_once(logging | PAUSE1, command_str);

	if (is_app_chooser_mode()) {
		filer_do_next = EF_EXECUTED;
	} else {
		filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	}
	return 0;
}

//------------------------------------------------------------------------------
void begin_fork_exec_repeat(void)
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
	return fork_exec_sh_c(SETTERM1 | SEPARATE1 | flags, command);
}
int fork_exec_sh_c_repeat(int flags, const char *command)
{
	return fork_exec_sh_c(SETTERM0 | flags | PAUSE0, command);
}

int fork_exec_args_once(int pause_aft_exec, ...)
{
	char *args[MAX_EXECV_ARGS+1];
	va_list ap;
	va_start(ap, pause_aft_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_exec_args(SETTERM1 | SEPARATE1 | pause_aft_exec, args);
}
int fork_exec_args_repeat(int flags, ...)
{
	char *args[MAX_EXECV_ARGS+1];
	va_list ap;
	va_start(ap, flags);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_exec_args(SETTERM0 | flags | PAUSE0, args);
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
	if (flags & SETTERM1) {
		clear_fork_exec_counter();
	}
	const char *command = exec_args_to_str(args);
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_EXEC, command);
	mflf_d_printf("exec: [%s]\n", command);
#endif // ENABLE_HISTORY
	return fork_exec_before_after(flags | LOGGING0, command, args);
}

PRIVATE const char *exec_args_to_str(char * const *args)
{
	static char buffer[MAX_PATH_LEN+1];
	strcpy(buffer, "");
	for (int arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		if (args[arg_idx] == NULL)
			break;
		concat_file_path_separating_by_space(buffer, MAX_PATH_LEN, args[arg_idx]);
	}
	return buffer;
}

#endif // ENABLE_FILER

//------------------------------------------------------------------------------

int send_to_system_clipboard()
{
#define UP_SYS_CLIPBOARD_CMD	"update-system-clipboard.sh"
	if (check_wsl()) {
		tio_set_cursor_pos(central_win_get_status_line_y(), 0);
		return fork_exec_sh_c(SETTERM0 | SEPARATE0 | LOGGING0 | PAUSE0, UP_SYS_CLIPBOARD_CMD);
	}
	return 0;
}

//------------------------------------------------------------------------------

int fork_exec_sh_c(int flags, const char *command)
{
	char * args[MAX_EXECV_ARGS+1];

	if (flags & SETTERM1) {
		clear_fork_exec_counter();
	}

	if (check_availability_of_script() == 0) {
		flags &= ~LOGGING1;		// turn off logging
	}
	if ((flags & LOGGING1) == 0) {
		// "sh -c <command ...>"
#define SH_PROG			"sh"
		args[0] = SH_PROG;
		args[1] = "-c";
		args[2] = (char *)command;
		args[3] = NULL;
		mflf_d_printf("logging: %d, exec: {{%s} {%s} {%s}}\n",
		 (flags & LOGGING1) != 0, args[0], args[1], args[2]);
	} else {
		// "script -q -O <log_file> -a -c <command ...>"
#define SCRIPT_PROG		"script"
		args[0] = SCRIPT_PROG;
		args[1] = "-q";
		args[2] = "-O";
		args[3] = (char *)get_exec_log_file_path();
		args[4] = "-a";
		args[5] = "-c";
		args[6] = (char *)command;
		args[7] = NULL;
		mflf_d_printf("logging: %d, exec: {{%s} {%s} {%s} {%s} {%s} {%s} {%s}}\n",
		 (flags & LOGGING1) != 0, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
	}

	// It does not output "sh -c [command arg1 arg2]"
	//           but output only "command arg1 arg2"
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_EXEC, command);
	mflf_d_printf("exec: [%s]\n", command);
#endif // ENABLE_HISTORY
	return fork_exec_before_after(flags, command, args);
}

PRIVATE int fork_exec_before_after(int flags, const char *command, char * const args[])
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	if ((flags & SETTERM1) && get_fork_exec_counter() == 0) {
		restore_term_for_shell();
	}
	if ((flags & SEPARATE1) && (get_fork_exec_counter() == 0)) {
		char header[MAX_PATH_LEN+1];
		snprintf(header, MAX_PATH_LEN, "== %c%s%c%c %s ==\n",
		 (flags & LOGGING1) ? '<' : '[',
		 full_path_of_cur_dir_s(),
		 (flags & LOGGING1) ? '>' : ']',
		 (geteuid() == 0) ? '#' : '$', command);
		// output separator line to log file
		if (flags & LOGGING1) {
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
			mflf_d_printf("pid: %d, exit_status: %d\n", pid, exit_status);
			if (pid != -1) {
				break;
			}
		}
	}

	if (flags & PAUSE1) {
		pause_after_exec(exit_status);
	}
	if ((flags & SETTERM1) && (get_fork_exec_counter() == 0)) {
		reinit_term_for_filer();
	}
	inc_fork_exec_counter();
	if (flags & LOGGING1) {
		write_text_to_file(get_exec_log_file_path(), 1, "\n");
	}
	return exit_status;
}

PRIVATE int fork_exec_counter = 0;
void clear_fork_exec_counter(void)
{
	fork_exec_counter = 0;
}
int get_fork_exec_counter(void)
{
	return fork_exec_counter;
}
int inc_fork_exec_counter(void)
{
	return fork_exec_counter++;
}

void pause_after_exec(int exit_status)
{
	printf("\n\aStatus:%d ", exit_status);
	set_term_raw();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	for (int chars = 0; chars < 1000; chars++) {
		if (getchar() == EOF)
			break;
	}
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	//      12345678901234567890123456789012345678901234567890123456789012345678901234567890
	printf("======== Hit any key to return to %s ======== ", APP_NAME);
	fflush(stdout);
	getchar();
	printf("\n");
}

//------------------------------------------------------------------------------
int restore_term_for_shell(void)
{
	tio_set_cursor_on(1);
	tio_suspend();
	signal_fork();
	return 0;
}
int reinit_term_for_filer(void)
{
	signal_init();
	tio_resume();
	tio_fill_screen();
	return 0;
}

// End of filerrun.c
