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

#ifdef ENABLE_FILER

PRIVATE int do_run_command_(int mode);
PRIVATE int do_filer_cmd_(char *cmd_file);

#define BEPAGER		"bepager"
#define BETAIL		"betail"
#define BETRASH		"betrash"
#define BEMARKDEL	"bemarkdel"
#define BESIZE0		"besize0"

#define BECMD		"becmd"		// becmd?

//-----------------------------------------------------------------------------

int do_run_line(void)
{
	fork_exec_once_sh_c(SEPARATE1, PAUSE1, CEBV_CL->data);
	do_refresh_editor();
	return 0;
}

#define MARK_TO_BE_REPLACED_WITH_FILE_NAME		"{}"
#define MARK_TO_BE_REPLACED_WITH_FILE_NAME_LEN	strlen(MARK_TO_BE_REPLACED_WITH_FILE_NAME)
int do_exec_command_with_file(void)
{
	char command_str[MAX_PATH_LEN+1];
	int ret;
	int file_idx;
	char *ptr_replace;
	char buffer[MAX_PATH_LEN+1];
#define MAX_REPLACEMENTS	10
	int cnt;

	ret = input_string("", command_str,
	 HISTORY_TYPE_IDX_EXEC, _("Execute({} will be replaced with file-name):"));

	if (ret < 0) {
		// do_exec_command_with_file -> FILER_ENTERED_FILE_PATH
		filer_do_next = FILER_ENTERED_FILE_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
	if (get_files_selected_cfv() == 0) {
		fork_exec_once_sh_c(SEPARATE1, PAUSE1, command_str);
		filer_do_next = FILER_REFRESH_FORCED;
	} else {
		begin_fork_exec_repeat();
		for (file_idx = select_and_get_first_file_idx_selected();
		 file_idx >= 0;
		 file_idx = get_next_file_idx_selected(file_idx)) {
			if (is_handler_sigint_called())
				break;
			// replace "{}" with filename
			// Ex. "cp -auv {} dir/{}" ==> "cp -auv filename dir/filename"
			strlcpy__(buffer, command_str, MAX_PATH_LEN);
			for (cnt = 0; cnt < MAX_REPLACEMENTS; cnt++) {
				if ((ptr_replace = strstr(buffer, MARK_TO_BE_REPLACED_WITH_FILE_NAME))
				 == NULL)
					break;
				replace_str(buffer, MAX_PATH_LEN,
				 ptr_replace - buffer, MARK_TO_BE_REPLACED_WITH_FILE_NAME_LEN,
				 quote_file_name(cur_fv->file_list[file_idx].file_name), -1);
			}
			fork_exec_repeat_sh_c(SEPARATE1, buffer);
		}
		end_fork_exec_repeat();
		filer_do_next = FILER_REFRESH_FORCED;
	}
	return 0;
}
int do_exec_command_with_files(void)
{
	char command_str[MAX_PATH_LEN+1];
	int ret;
	int file_idx;

	command_str[0] = '\0';
	for (file_idx = select_and_get_first_file_idx_selected();
	 file_idx >= 0;
	 file_idx = get_next_file_idx_selected(file_idx)) {
		concat_file_name_separating_by_space(command_str, MAX_PATH_LEN,
		 cur_fv->file_list[file_idx].file_name, '\'');
	}

	ret = input_string_pos(command_str, command_str, 0,
	 HISTORY_TYPE_IDX_EXEC, _("Execute with files:"));

	if (ret < 0) {
		// do_exec_command_with_files -> FILER_ENTERED_FILE_PATH
		filer_do_next = FILER_ENTERED_FILE_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	fork_exec_once_sh_c(SEPARATE1, PAUSE1, command_str);
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}
int do_run_command_rel(void)
{
	struct stat *st_ptr;

	st_ptr = &cur_fv->file_list[cur_fv->cur_sel_idx].st;
	if ((st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) == 0)
		do_run_command_(0);
	else
		do_run_command_(1);
	return 0;
}
int do_run_command_abs(void)
{
	do_run_command_(2);
	return 0;
}
int do_run_command_src_dst(void)
{
	do_run_command_(5);
	return 0;
}
int do_run_command_sh(void)
{
	do_run_command_(4);
	return 0;
}
PRIVATE int do_run_command_(int mode)
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
	int ret;

	st_ptr = &cur_fv->file_list[cur_fv->cur_sel_idx].st;
	switch(mode) {
	default:
	case 0:
		explanation = _("Run (with file):");
		snprintf_(command_str, MAX_PATH_LEN+1, " %s",
		 quote_file_name(cur_fv->file_list[cur_fv->cur_sel_idx].file_name));
		break;
	case 1:
		explanation = _("Run (current-directory-file):");
		snprintf_(command_str, MAX_PATH_LEN+1, "./%s ",
		 quote_file_name(cur_fv->file_list[cur_fv->cur_sel_idx].file_name));
		break;
	case 2:
		explanation = _("Run (with real-path):");
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 cur_fv->cur_dir,
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
		quote_file_name_buf(command_str, buf_s);
		break;
	case 3:
		explanation = _("Run (script):");
		snprintf_(command_str, MAX_PATH_LEN+1, ". %s",
		 quote_file_name(cur_fv->file_list[cur_fv->cur_sel_idx].file_name));
		break;
	case 4:
		explanation = _("Run (script by shell):");
		snprintf_(command_str, MAX_PATH_LEN+1, "sh %s",
		 quote_file_name(cur_fv->file_list[cur_fv->cur_sel_idx].file_name));
		break;
	case 5:
		explanation = _("Run (with SRC-path and DEST-path):");
		if (cur_filer_views->view_idx == 0) {
			src_fv_idx = 0;
			dst_fv_idx = 1;
		} else {
			src_fv_idx = 1;
			dst_fv_idx = 0;
		}
		snprintf_(buf_s, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_views->views[src_fv_idx].cur_dir,
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
		snprintf_(buf_d, MAX_PATH_LEN+1, "%s/%s",
		 cur_filer_views->views[dst_fv_idx].cur_dir,
		 cur_fv->file_list[cur_fv->cur_sel_idx].file_name);
		snprintf_(command_str, MAX_PATH_LEN+1, " %s %s",
		 quote_file_name_buf(buf1, buf_s),
		 quote_file_name_buf(buf2, buf_d));
		break;
	}

	ret = input_string_pos(command_str, command_str,
	 (S_ISREG(st_ptr->st_mode) && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
	  ? MAX_PATH_LEN : 0,
	 HISTORY_TYPE_IDX_EXEC, explanation);

	if (ret < 0) {
		// do_run_command_ -> FILER_ENTERED_FILE_PATH
		filer_do_next = FILER_ENTERED_FILE_PATH;
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (is_path_dir(command_str) > 0) {
		return filer_change_dir(command_str);
	}
	fork_exec_once_sh_c(SEPARATE1, PAUSE1, command_str);
	filer_do_next = FILER_REFRESH_FORCED;
	return 0;
}

int do_filer_cmd_1(void)
{
	return do_filer_cmd_(BECMD "1");
}
int do_filer_cmd_2(void)
{
	return do_filer_cmd_(BECMD "2");
}
int do_filer_cmd_3(void)
{
	return do_filer_cmd_(BECMD "3");
}
int do_filer_cmd_4(void)
{
	return do_filer_cmd_(BECMD "4");
}

PRIVATE int do_filer_cmd_(char *cmd_file)
{
	int file_idx;
	char *file_name;

	file_idx = cur_fv->cur_sel_idx;
	file_name = cur_fv->file_list[file_idx].file_name;
	if (is_app_list_mode()) {
		// do_filer_cmd -> FILER_ENTERED_FILE_PATH
		filer_do_next = FILER_ENTERED_FILE_PATH;
		return 0;
	}
	if (S_ISREG(cur_fv->file_list[file_idx].st.st_mode)) {
		fork_exec_once(SEPARATE1, PAUSE0, cmd_file, file_name, 0);
		return 0;
	}
	return 0;
}

#endif // ENABLE_FILER

// End of filerrun.c
