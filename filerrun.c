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

PRIVATE int fork_exec_before_after(int set_term, int separate_bef_exec, int logging,
 int pause_aft_exec, const char *command, char * const args[]);

#ifdef ENABLE_FILER

PRIVATE int dof_run_command_(int mode);

//-----------------------------------------------------------------------------

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
	int logging = LOGGING0;

	if (chk_inp_str_ret_val_filer(input_string_pos("", command_str, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_EXEC,
	 _("Execute({} will be replaced with file-name):")))) {
		return 0;
	}
	if (filer_do_next == EF_INPUT_W_ALT) {
		logging = LOGGING2;	// set logging ON
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
		exit_status = fork_exec_sh_c_repeat(SEPARATE1, logging, buffer);
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
	int logging = LOGGING0;

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
	if (filer_do_next == EF_INPUT_W_ALT) {
		logging = LOGGING2;	// set logging ON
	}
	fork_exec_sh_c_once(logging, PAUSE1, command_str);
	filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	return 0;
}

#define RUN_MOD_MASK	0x0f
#define RUN_WITH_LOG	0x10
#define RUN_SOON		0x20
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
int dof_run_command_src_dst(void)
{
	return dof_run_command_(5);
}
int dof_run_command_sh(void)
{
	return dof_run_command_(4);
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
	int src_fv_idx = 0;
	int dst_fv_idx = 1;

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
		 get_cur_filer_cur_pane_view()->cur_dir,
		 get_cur_fv_cur_file_ptr()->file_name);
		quote_file_path_buf(command_str, buf_s);
		break;
	case 3:
		expl = _("Run (script)");
		snprintf_(command_str, MAX_PATH_LEN+1, ". %s",
		 quote_file_path_static(get_cur_fv_cur_file_ptr()->file_name));
		break;
	case 4:
		expl = _("Run (script by shell)");
		snprintf_(command_str, MAX_PATH_LEN+1, "sh %s",
		 quote_file_path_static(get_cur_fv_cur_file_ptr()->file_name));
		break;
	case 5:
		expl = _("Run (with SRC-path and DEST-path)");
		if (get_filer_cur_pane_idx() == 0) {
			src_fv_idx = 0;
			dst_fv_idx = 1;
		} else {
			src_fv_idx = 1;
			dst_fv_idx = 0;
		}
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_panes->filer_views[src_fv_idx].cur_dir,
		 get_cur_fv_cur_file_ptr()->file_name);
		snprintf_(buf_d, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_panes->filer_views[dst_fv_idx].cur_dir,
		 get_cur_fv_cur_file_ptr()->file_name);
		snprintf_(command_str, MAX_PATH_LEN+1, " %s %s",
		 quote_file_path_buf(buf1, buf_s),
		 quote_file_path_buf(buf2, buf_d));
		break;
	}

	int logging = LOGGING0;
	if (mode & RUN_WITH_LOG) {
		logging = LOGGING2;
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
		if (filer_do_next == EF_INPUT_W_ALT) {
			logging = LOGGING2;	// set logging ON
		}
	}

	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}

flf_d_printf("command_str [%s]\n", command_str);
	fork_exec_sh_c_once(logging, PAUSE1, command_str);

	if (is_app_list_mode()) {
		filer_do_next = EF_EXECUTED;
	} else {
		filer_do_next = FL_UPDATE_FILE_LIST_FORCE;
	}
	return 0;
}

//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------

PRIVATE int args_from_va_list(char **args, va_list ap);
PRIVATE int fork_exec_args(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[]);
PRIVATE const char *exec_args_to_str(char * const *args);

int fork_exec_sh_c_once(int logging, int pause_aft_exec, const char *command)
{
	return fork_exec_sh_c(SETTERM1, SEPARATE1, logging, pause_aft_exec, command);
}
int fork_exec_sh_c_repeat(int separate_bef_exec, int logging, const char *command)
{
	return fork_exec_sh_c(SETTERM0, separate_bef_exec, logging, PAUSE0, command);
}

int fork_exec_args_once(int pause_aft_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];
	va_start(ap, pause_aft_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_exec_args(SETTERM1, SEPARATE1, pause_aft_exec, args);
}
int fork_exec_args_repeat(int separate_bef_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];
	va_start(ap, separate_bef_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_exec_args(SETTERM0, separate_bef_exec, PAUSE0, args);
}

// convert va_list to "char *argv[]"
// Note: va_list and args are opposite in the order.
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

PRIVATE int fork_exec_args(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[])
{
	if (set_term) {
		clear_fork_exec_counter();
	}
	const char *command = exec_args_to_str(args);
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_EXEC, command);
mflf_d_printf("exec: [%s]\n", command);
#endif // ENABLE_HISTORY
	return fork_exec_before_after(set_term, separate_bef_exec, LOGGING0, pause_aft_exec,
	 command, args);
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

//-----------------------------------------------------------------------------

#define UP_SYS_CLIPBOARD_CMD	"update-system-clipboard.sh"
int send_to_system_clipboard()
{
	if (check_wsl()) {
		tio_set_cursor_pos(get_status_line_y(), 0);
		return fork_exec_sh_c(SETTERM0, SEPARATE0, PAUSE0, LOGGING0, UP_SYS_CLIPBOARD_CMD);
	}
	return 0;
}

//-----------------------------------------------------------------------------

int fork_exec_sh_c(int set_term, int separate_bef_exec, int logging, int pause_aft_exec,
 const char *command)
{
	char *args[MAX_EXECV_ARGS+1];
	char buffer[MAX_PATH_LEN+1] = "";

	if (set_term) {
		clear_fork_exec_counter();
	}
	// sh -c "command ..."
#define SH_PROG			"sh"
/////#define TEE_PROG		"tee"
#define TEE_PROG_APPEND	"tee -a"
	args[0] = SH_PROG;
	args[1] = "-c";
	if (logging == LOGGING0) {
		args[2] = (char *)command;
	} else {
		// sh -c "command ... 2>&1 | tee    /home/user/.be/1.long"
		// sh -c "command ... 2>&1 | tee -a /home/user/.be/1.long"
		snprintf_(buffer, MAX_PATH_LEN, "%s 2>&1 | %s %s",
		 command, TEE_PROG_APPEND, get_exec_log_file_path());
		args[2] = (char *)buffer;
	}
	args[3] = NULL;

mflf_d_printf("logging: %d, exec: {{%s} {%s} {%s}}\n", logging, args[0], args[1], args[2]);
	// It does not output "sh -c [command arg1 arg2]"
	//           but output only "command arg1 arg2"
#ifdef ENABLE_HISTORY
	update_history(HISTORY_TYPE_IDX_EXEC, command);
mflf_d_printf("exec: [%s]\n", command);
#endif // ENABLE_HISTORY
	return fork_exec_before_after(set_term, separate_bef_exec, logging, pause_aft_exec,
	 command, args);
}

PRIVATE int fork_exec_before_after(int set_term, int separate_bef_exec, int logging,
 int pause_aft_exec, const char *command, char * const args[])
{
	if (set_term && get_fork_exec_counter() == 0) {
		restore_term_for_shell();
	}
	if (separate_bef_exec && (get_fork_exec_counter() == 0)) {
		char header[MAX_PATH_LEN+1];
		snprintf(header, MAX_PATH_LEN, "== %c%s%c%c %s ==\n",
		 logging ? '<' : '[', full_path_of_cur_dir_static(), logging ? '>' : ']',
		 (geteuid() == 0) ? '#' : '$', command);
		// output separator line to log file
		if (logging) {
			write_text_to_file(get_exec_log_file_path(), logging == LOGGING2, header);
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

	if (pause_aft_exec) {
		pause_after_exec(exit_status);
	}
	if (set_term && (get_fork_exec_counter() == 0)) {
		reinit_term_for_filer();
	}
	inc_fork_exec_counter();
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
	printf(
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
 "======== Hit any key to return to %s ======== ", APP_NAME);
	fflush(stdout);
	getchar();
	printf("\n");
}

//-----------------------------------------------------------------------------
int restore_term_for_shell(void)
{
	tio_fill_screen(0);
	tio_set_cursor_on(1);
	tio_suspend();
	signal_fork();
	return 0;
}
int reinit_term_for_filer(void)
{
	signal_init();
	tio_resume();
	tio_fill_screen(0);
	return 0;
}
//-----------------------------------------------------------------------------
const char *get_exec_log_file_path()
{
	// /dev/tty1 => "tty1.log", /dev/pts/1 => "1.log"
	static char file_path[MAX_PATH_LEN+1] = "";
	char dir[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];
	if (is_strlen_0(file_path)) {
		separate_path_to_dir_and_file(get_tty_name(), dir, file);
		snprintf_(file_path, MAX_PATH_LEN, "%s/%s.log", get_app_dir(), file);
flf_d_printf("file_path: [%s]\n", file_path);
	}
	return file_path;
}

// End of filerrun.c
