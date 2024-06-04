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

PRIVATE int fork_execv_before_after(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[]);

#ifdef ENABLE_FILER

PRIVATE int dof_run_command_(int mode);

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"

#define BECMD		"becmd"		// becmd?

//-----------------------------------------------------------------------------

#define STR_TO_BE_REPLACED_WITH_FILE_NAME		"{}"
#define STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN	strlen(STR_TO_BE_REPLACED_WITH_FILE_NAME)

// If two or more files selected, process it by enumerating execution.
int dof_exec_command_with_file(void)
{
	char command_str[MAX_PATH_LEN+1];
	char *ptr_replace;
	char buffer[MAX_PATH_LEN+1];
#define MAX_REPLACEMENTS	10
	int cnt;

	int ret = input_string_tail("", command_str, HISTORY_TYPE_IDX_EXEC,
	 _("Execute({} will be replaced with file-name):"));
	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
	if (get_files_selected_cfv() == 0) {
		fork_exec_sh_c_once(PAUSE1, command_str);
	} else {
		begin_fork_exec_repeat();
		for (int file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_sigint_signaled())
				break;
			// replace "{}" with filename
			// Ex. "cp -auv {} dir/{}" ==> "cp -auv filename dir/filename"
			strlcpy__(buffer, command_str, MAX_PATH_LEN);
			for (cnt = 0; cnt < MAX_REPLACEMENTS; cnt++) {
				if ((ptr_replace = strstr(buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME))
				 == NULL)
					break;
				replace_str(buffer, MAX_PATH_LEN,
				 ptr_replace - buffer, STR_TO_BE_REPLACED_WITH_FILE_NAME_LEN,
				 quote_file_name_static(get_cur_filer_view()->file_list[file_idx].file_name), -1);
			}
			fork_exec_sh_c_repeat(SEPARATE1, buffer);
		}
		end_fork_exec_repeat();
	}
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
// If two or more files selected, pass all to command line at once.
int dof_exec_command_with_files(void)
{
	char command_str[MAX_PATH_LEN+1];
	int ret;

	// "file1 file2 ..."
	command_str[0] = '\0';
	for (int file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_name_separating_by_space(command_str, MAX_PATH_LEN,
		 get_cur_filer_view()->file_list[file_idx].file_name);
	}

	ret = input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC, _("Execute with files:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	fork_exec_sh_c_once(PAUSE1, command_str);
	filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
	return 0;
}
int dof_run_command_rel(void)
{
	struct stat *st_ptr;

	st_ptr = &get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].st;
	if ((st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) == 0)
		dof_run_command_(0);
	else
		dof_run_command_(1);
	return 0;
}
int dof_run_command_abs(void)
{
	dof_run_command_(2);
	return 0;
}
int dof_run_command_src_dst(void)
{
	dof_run_command_(5);
	return 0;
}
int dof_run_command_sh(void)
{
	dof_run_command_(4);
	return 0;
}
int dof_run_command_soon(void)
{
	dof_run_command_(10 + 1);	// rwx: executable file
	return 0;
}
PRIVATE int dof_run_command_(int mode)
{
	const char *explanation;
	char buf_s[MAX_PATH_LEN+1];
	char buf_d[MAX_PATH_LEN+1];
	char buf1[MAX_PATH_LEN+1];
	char buf2[MAX_PATH_LEN+1];
	char command_str[MAX_PATH_LEN+1];
	struct stat *st_ptr;
	int src_fv_idx = 0;
	int dst_fv_idx = 1;
	int ret = 0;

	st_ptr = &get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].st;
	switch (mode % 10) {
	default:
	case 0:
		explanation = _("Run (with file):");
		snprintf_(command_str, MAX_PATH_LEN+1, " %s",
		 quote_file_name_static(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx]
		  .file_name));
		break;
	case 1:
		explanation = _("Run (current-directory-file):");
		snprintf_(command_str, MAX_PATH_LEN+1, "./%s ",
		 quote_file_name_static(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx]
		  .file_name));
		break;
	case 2:
		explanation = _("Run (with real-path):");
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 get_cur_filer_view()->cur_dir,
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		quote_file_name_buf(command_str, buf_s);
		break;
	case 3:
		explanation = _("Run (script):");
		snprintf_(command_str, MAX_PATH_LEN+1, ". %s",
		 quote_file_name_static(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx]
		  .file_name));
		break;
	case 4:
		explanation = _("Run (script by shell):");
		snprintf_(command_str, MAX_PATH_LEN+1, "sh %s",
		 quote_file_name_static(get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx]
		  .file_name));
		break;
	case 5:
		explanation = _("Run (with SRC-path and DEST-path):");
		if (get_filer_cur_pane_idx() == 0) {
			src_fv_idx = 0;
			dst_fv_idx = 1;
		} else {
			src_fv_idx = 1;
			dst_fv_idx = 0;
		}
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_panes->filer_views[src_fv_idx].cur_dir,
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		snprintf_(buf_d, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_panes->filer_views[dst_fv_idx].cur_dir,
		 get_cur_filer_view()->file_list[get_cur_filer_view()->cur_sel_idx].file_name);
		snprintf_(command_str, MAX_PATH_LEN+1, " %s %s",
		 quote_file_name_buf(buf1, buf_s),
		 quote_file_name_buf(buf2, buf_d));
		break;
	}

	if (mode >= 10) {
		ret = 1;	// run soon without editing command line
	} else {
		ret = input_string_pos(command_str, command_str,
		 (S_ISREG(st_ptr->st_mode) && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
		  ? MAX_PATH_LEN : 0,
		 HISTORY_TYPE_IDX_EXEC, explanation);
	}
flf_d_printf("ret: %d\n", ret);

	if (ret < 0) {
		return 0;
	}
	if (ret == 0) {
		return 0;
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
flf_d_printf("command_str [%s]\n", command_str);
	fork_exec_sh_c_once(PAUSE1, command_str);
	if (is_app_list_mode()) {
		filer_do_next = FILER_DO_QUIT;
	} else {
		filer_do_next = FILER_DO_UPDATE_FILE_LIST_FORCE;
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
void end_fork_exec_repeat(void)
{
	pause_after_exec();
	reinit_term_for_filer();
}

//-----------------------------------------------------------------------------

PRIVATE int args_from_va_list(char **args, va_list ap);
PRIVATE int fork_execv_args(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[]);
#ifdef ENABLE_HISTORY
PRIVATE void output_exec_args_to_history(char * const *args);
#endif // ENABLE_HISTORY

int fork_exec_sh_c_once(int pause_aft_exec, const char *command)
{
	return fork_exec_sh_c(SETTERM1, SEPARATE1, pause_aft_exec, command);
}
int fork_exec_sh_c_repeat(int separate_bef_exec, const char *command)
{
	return fork_exec_sh_c(SETTERM0, separate_bef_exec, PAUSE0, command);
}

int fork_exec_args_once(int pause_aft_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];
	va_start(ap, pause_aft_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_execv_args(SETTERM1, SEPARATE1, pause_aft_exec, args);
}
int fork_exec_args_repeat(int separate_bef_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];
	va_start(ap, separate_bef_exec);
	args_from_va_list(args, ap);
	va_end(ap);

	return fork_execv_args(SETTERM0, separate_bef_exec, PAUSE0, args);
}

// convert va_list to "char *argv[]"
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

PRIVATE int fork_execv_args(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[])
{
	if (set_term) {
		clear_fork_exec_counter();
	}
#ifdef ENABLE_HISTORY
	if (get_fork_exec_counter() == 0) {
		output_exec_args_to_history(args);
	}
#endif // ENABLE_HISTORY
	return fork_execv_before_after(set_term, separate_bef_exec, pause_aft_exec, args);
}

#ifdef ENABLE_HISTORY
PRIVATE void output_exec_args_to_history(char * const *args)
{
	char buffer[MAX_PATH_LEN+1] = "";

	for (int arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		if (args[arg_idx] == NULL)
			break;
		concat_file_name_separating_by_space(buffer, MAX_PATH_LEN, args[arg_idx]);
	}
mflf_d_printf("exec: [%s]\n", buffer);
	update_history(HISTORY_TYPE_IDX_EXEC, buffer, 0);
}
#endif // ENABLE_HISTORY

#endif // ENABLE_FILER

//-----------------------------------------------------------------------------

int send_to_system_clipboard()
{
	if (check_wsl()) {
		tio_set_cursor_pos(main_win_get_bottom_win_y() + STATUS_LINE, 0);
		return fork_exec_sh_c(SETTERM0, SEPARATE0, PAUSE0, "update-system-clipboard.sh");
	}
	return 0;
}

//-----------------------------------------------------------------------------

int fork_exec_sh_c(int set_term, int separate_bef_exec, int pause_aft_exec, const char *command)
{
	char *args[MAX_EXECV_ARGS+1];

	if (set_term) {
		clear_fork_exec_counter();
	}
	// sh -c "command arg1 arg2 arg3"
	args[0] = "sh";
	args[1] = "-c";
	args[2] = (char *)command;
	args[3] = NULL;

mflf_d_printf("exec: {{%s} {%s} {%s}}\n", args[0], args[1], args[2]);
#ifdef ENABLE_HISTORY
	if (get_fork_exec_counter() == 0) {
		// It does not output "sh -c [command arg1 arg2 arg3]"
		//           but output only "command arg1 arg2 arg3"
		update_history(HISTORY_TYPE_IDX_EXEC, command, 0);
	}
#endif // ENABLE_HISTORY
	return fork_execv_before_after(set_term, separate_bef_exec, pause_aft_exec, args);
}

PRIVATE int fork_execv_before_after(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[])
{
	if (set_term && get_fork_exec_counter() == 0) {
		restore_term_for_shell();
	}
	if (separate_bef_exec && get_fork_exec_counter() == 0) {
		// output separator line
		printf("\n-------- command execution started here --------\n");
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
			int ret = waitpid(pid, &exit_status, 0);
mflf_d_printf("ret: %d, exit_status: %d\n", ret, exit_status);
			if (ret != -1)
				break;
		}
	}

	if (pause_aft_exec) {
		printf("\n\aStatus:%d ", exit_status);
		pause_after_exec();
	}
	if (set_term && get_fork_exec_counter() == 0) {
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

void pause_after_exec(void)
{
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
	tio_clear_screen();
	tio_set_cursor_on(1);
	tio_suspend();
	signal_fork();
	return 0;
}
int reinit_term_for_filer(void)
{
	signal_init();
	tio_resume();
	tio_clear_flash_screen(1);
	return 0;
}

// End of filerrun.c
