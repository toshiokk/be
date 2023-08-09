/**************************************************************************
 *   editorgoto.c                                                         *
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

PRIVATE int load_files_in_cur_buf(void);
PRIVATE int load_files_in_string(const char *string, int files_to_load, int try_upp_low,
 int open_on_err, int msg_on_err, int recursive);

PRIVATE int load_file_name__(const char *file_name, int open_on_err, int msg_on_err);

PRIVATE int is_file_name_proj_file(const char *file_name);
PRIVATE const char *skip_n_file_names(const char *line, int field_idx);
#ifdef START_UP_TEST
void test_get_n_th_file_name(void);
#endif // START_UP_TEST
#ifdef ENABLE_HISTORY
PRIVATE void goto_pos_by_history(const char *full_path);
#endif // ENABLE_HISTORY

// 123
// file.ext:123:45
int do_goto_input_line(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;
	int line_num;

	ret = input_string("", string, HISTORY_TYPE_IDX_CURSPOS, _("Enter line number:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (sscanf(string, "%d", &line_num) > 0) {
		// go to line
		goto_line_col_in_cur_buf(line_num, 1);
		post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
		return 1;
	}
	// go to file
	// CURDIR: changed in editor
	load_files_in_string(string, 1, TUL0, OOE0, WOE1, RECURSIVE0);	// file.ext:123:45
	post_cmd_processing(NULL, VERT_MOVE, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return 1;
}
// do tag jump
int do_goto_file_in_cur_line(void)
{
	char dir_save[MAX_PATH_LEN+1];
	int ret;

////flf_d_printf("[%s]\n", CEBV_CL->data);
	memorize_cur_file_pos_null(NULL);
	clear_files_loaded();
	save_change_cur_dir(dir_save, strip_file_from_path(get_c_e_b()->abs_path));
	// CURDIR: changed to cur-file's abs-dir
	ret = load_files_in_string(CEBV_CL->data, 10, TUL1, OOE0, WOE1, RECURSIVE1);
	change_cur_dir(dir_save);
	if (ret <= 0) {
		// error
		return 0;
	}
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return 1;
}
int do_open_files_in_buf(void)
{
	clear_files_loaded();
	load_files_in_cur_buf();
	disp_files_loaded();
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
int do_switch_to_top_file(void)
{
	if (switch_c_e_b_to_top() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int do_switch_to_bot_file(void)
{
	if (switch_c_e_b_to_bot() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
//-----------------------------------------------------------------------------
int do_switch_to_prev_file(void)
{
	int ret;

	if ((ret = switch_c_e_b_to_prev(1, 0)) == 0) {
		disp_status_bar_err(_("No previous open files"));
		return ret;
	}
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Previous file"));
	return ret;
}
int do_switch_to_next_file(void)
{
	int ret;

	if ((ret = switch_c_e_b_to_next(1, 0)) == 0) {
		disp_status_bar_err(_("No next open files"));
		return ret;
	}
	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Next file"));
	return ret;
}
//-----------------------------------------------------------------------------
int do_return_to_prev_file_pos(void)
{
	recall_cur_file_pos_null(NULL);

	post_cmd_processing(NULL, HORIZ_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	do_refresh_editor();
	disp_status_bar_done(_("Returned to previous pos"));
	return 1;
}
//-----------------------------------------------------------------------------
int do_switch_editor_pane(void)
{
	do_switch_editor_pane_();
///_FLF_
	post_cmd_processing(NULL, NO_MOVE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
///_FLF_
	return 1;
}
void do_switch_editor_pane_(void)
{
	int pane_idx;			// pane index

	pane_idx = get_cur_editor_view_idx() ? 0 : 1;
flf_d_printf("pane_idx: %d\n", pane_idx);
	set_cur_editor_view_idx(pane_idx);
}

//-----------------------------------------------------------------------------

int load_files_in_cur_buf(void)
{
	char file_pos_str[MAX_PATH_LEN+1];
	int lines;
	char dir_save[MAX_PATH_LEN+1];
	int files = 0;

	memorize_cur_file_pos_null(file_pos_str);
	first_line();
	clear_handler_sigint_called();
#define MAX_LINES_TO_TRY_TO_LOAD		1000
	for (lines = 0; lines < MAX_LINES_TO_TRY_TO_LOAD; lines++) {
		if (is_handler_sigint_called())
			break;
		if (line_data_len(CEBV_CL)) {
			memorize_cur_file_pos_null(NULL);
			save_change_cur_dir(dir_save, strip_file_from_path(get_c_e_b()->abs_path));
			// CURDIR: changed to cur-file's abs-dir
			files += load_files_in_string(CEBV_CL->data, 10, TUL1, OOE0, WOE0, RECURSIVE0);
			change_cur_dir(dir_save);
			disp_editor_title_bar();
			tio_refresh();
			recall_cur_file_pos_null(NULL);
		}
		if (cursor_next_line() == 0)
			break;
	}
	recall_cur_file_pos_null(file_pos_str);
	return files;
}

//=============================================================================

// supported formats:
//  ##						// ignore any string
//  #fileio.h				// file name in comment
//  #include <fileio.h>		// C include directive
//  fileio.h:10				// GCC error message
//  fileio.h,10				// C compiler error message
//  fileio.h(10)			// M$ C compiler error message
//  #:fileio.h,10			// gettext *.po file
//  fileio.h 10:10			// file-name line-num:col-num
//  11/04/13 16:40:00, fileio.h:10:10,	// event log file
//  <location filename="fileio.h" line="10"/>	// Qt-lupdate
//  diff fileio.h fileio.h~
//  SOURCES += fileio.h
PRIVATE int load_files_in_string(const char *string,
 int files_to_load, int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	int field_idx;
	int files_loaded = 0;
	int files;
	const char *ptr;

////
flf_d_printf("[%s]\n", string);
	for (field_idx = 0; files_loaded < files_to_load; field_idx++) {
		ptr = skip_n_file_names(string, field_idx);
		if (*ptr == '\0')
			break;
		files = load_file_in_string(ptr, try_upp_low, open_on_err, msg_on_err, recursive);
		if (files > 0) {
			files_loaded++;
			open_on_err = 0;
			msg_on_err = 0;
		}
////flf_d_printf("files_loaded: %d\n", files_loaded);
	}
////flf_d_printf("files_loaded: %d\n", files_loaded);
	return files_loaded;
}
int load_file_in_string(const char *string,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	char file_name[MAX_PATH_LEN+1];
	int line_num, col_num;
	int files;

///
flf_d_printf("string:[%s]\n", string);
	if (get_file_line_col_from_str_null(string, file_name, &line_num, &col_num) == 0) {
_FLF_
		return 0;
	}
///
flf_d_printf("file_name:[%s]\n", file_name);
	if ((files = load_file_name_upp_low(file_name,
	 try_upp_low, open_on_err, msg_on_err, recursive)) > 0) {
///
flf_d_printf("loaded:[%s]\n", file_name);
		goto_line_col_in_cur_buf(line_num, col_num);
	}
	return files;
}
int load_file_name_upp_low(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	char file_name_buf[MAX_PATH_LEN+1];
	int files;

	// try to open specified file name (FileName.Ext)
	if ((files = load_file_name_recursive(file_name, open_on_err, msg_on_err, recursive)) > 0)
		goto load_file_name_upp_low_goto;
	if (try_upp_low) {
		strlcpy__(file_name_buf, file_name, MAX_PATH_LEN);
		// try to open in upper case file name (FILENAME.EXT)
		strupper(file_name_buf);
		if ((files = load_file_name_recursive(file_name_buf, open_on_err, msg_on_err, recursive)) > 0)
			goto load_file_name_upp_low_goto;
		// try to open in lower case file name (filename.ext)
		strlower(file_name_buf);
		if ((files = load_file_name_recursive(file_name_buf, open_on_err, msg_on_err, recursive)) > 0)
			goto load_file_name_upp_low_goto;
	}
load_file_name_upp_low_goto:;
	return files;
}
// Open file. If it is a project file, open file(s) described in it.
int load_file_name_recursive(const char *file_name, int open_on_err, int msg_on_err, int recursive)
{
	static int recursive_call_count = 0;
	int files;

///
flf_d_printf("[%s], %d, %d, %d\n", file_name, open_on_err, msg_on_err, recursive);
	if (load_file_name__(file_name, open_on_err, msg_on_err) <= 0) {
		return 0;
	}
///_FLF_
	files = 1;
	if (recursive && recursive_call_count == 0 && is_file_name_proj_file(file_name)) {
///flf_d_printf("recursive_call_count:%d\n", recursive_call_count);
		recursive_call_count++;
		files += load_files_in_cur_buf();		// recursive call
		recursive_call_count--;
	}
	add_files_loaded(1);
	return files;
}
PRIVATE int load_file_name__(const char *file_name, int open_on_err, int msg_on_err)
{
	char abs_path[MAX_PATH_LEN+1];

///
flf_d_printf("[%s]\n", file_name);
	get_abs_path(file_name, abs_path);
///
flf_d_printf("[%s]\n", abs_path);
	if (switch_c_e_b_to_file_name(abs_path)) {
flf_d_printf("already loaded:[%s]\n", abs_path);
		// already loaded
		return 1;
	}
	if (load_file_into_new_buf(abs_path, open_on_err, msg_on_err) < 0) {
		return 0;
	}
flf_d_printf("loaded:[%s]\n", abs_path);
#ifdef ENABLE_HISTORY
	goto_pos_by_history(abs_path);
#endif // ENABLE_HISTORY
	return 1;
}
//-----------------------------------------------------------------------------
int switch_c_e_b_to_file_name(const char *file_name)
{
	char abs_path[MAX_PATH_LEN+1];

	get_abs_path(file_name, abs_path);
	return switch_c_e_b_to_abs_path(abs_path);
}
int switch_c_e_b_to_abs_path(const char *abs_path)
{
	be_buf_t *buf;

	buf = get_edit_buf_from_abs_path(abs_path);
	if (buf) {
		set_cur_edit_buf(buf);
		return 1;	// switched
	}
	return 0;		// not found
}
//-----------------------------------------------------------------------------
int switch_c_e_b_to_top(void)
{
	if (IS_NODE_BOT_ANCH(EDIT_BUFS_TOP_BUF))
		return 0;
	set_cur_edit_buf(EDIT_BUFS_TOP_BUF);
	return 1;
}
int switch_c_e_b_to_bot(void)
{
	if (IS_NODE_TOP_ANCH(EDIT_BUFS_BOT_BUF))
		return 0;
	set_cur_edit_buf(EDIT_BUFS_BOT_BUF);
	return 1;
}
int switch_c_e_b_to_prev(int beep_at_end, int goto_bottom)
{
	if (IS_NODE_TOP(get_c_e_b())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_cur_edit_buf(get_c_e_b()->prev);
	if (goto_bottom)
		CEBV_CL = CUR_EDIT_BUF_BOT_LINE;
	return 1;
}
int switch_c_e_b_to_next(int beep_at_end, int goto_top)
{
	if (IS_NODE_BOT(get_c_e_b())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_cur_edit_buf(get_c_e_b()->next);
	if (goto_top)
		CEBV_CL = CUR_EDIT_BUF_TOP_LINE;
	return 1;
}
int switch_c_e_b_to_valid_buf(void)
{
	if (IS_NODE_VALID(get_c_e_b()) == 0) {
		return switch_c_e_b_to_another_buf();
	}
	return 1;
}
int switch_c_e_b_to_another_buf(void)
{
	if (switch_c_e_b_to_next(0, 0) == 0) {
		if (switch_c_e_b_to_prev(0, 0) == 0) {
			set_cur_edit_buf(EDIT_BUFS_TOP_ANCH);
			return 0;
		}
	}
	return 1;
}
//-----------------------------------------------------------------------------
PRIVATE int is_file_name_proj_file(const char *file_name)
{
	return (strcasecmp(&file_name[
	   LIM_MIN(0, strnlen(file_name, MAX_PATH_LEN) - strlen(PROJ_FILE_EXTENSION1))],
	  PROJ_FILE_EXTENSION1) == 0)
	 || (strcasecmp(&file_name[
	   LIM_MIN(0, strnlen(file_name, MAX_PATH_LEN) - strlen(PROJ_FILE_EXTENSION2))],
	  PROJ_FILE_EXTENSION2) == 0);	// 1: project file
}
PRIVATE const char *skip_n_file_names(const char *line, int field_idx)
{
	const char *ptr;
	int field_cnt;

////
flf_d_printf("[%s], %d\n", line, field_idx);
	ptr = line;
	for (field_cnt = 0; ; field_cnt++) {
		ptr = skip_to_file_path(ptr);
		if (*ptr == '\0')
			break;		// EOL
		if (field_cnt >= field_idx)
			break;
		const char *prev_ptr = ptr;
		ptr = skip_file_path(ptr);
		if (ptr == prev_ptr)
			// not progressed
			break;
	}
////
flf_d_printf("file: [%s]\n", ptr);
	return ptr;
}

#ifdef START_UP_TEST
void test_get_n_th_file_name(void)
{
	char test_str[] =
	 "history.c 345 hist_type_idx:3:['/home/user/ filename including space .txt '|1:1]";
	int field_idx;
	const char *ptr;
	char file_name[MAX_PATH_LEN+1];
	int line_num, col_num;

_FLF_
	for (field_idx = 0; field_idx < 10; field_idx++) {
		ptr = skip_n_file_names(test_str, field_idx);
		if (*ptr == '\0')
			break;
		if (get_file_line_col_from_str_null(ptr, file_name, &line_num, &col_num)) {
			flf_d_printf("%d: file_name:[%s],%d,%d\n", field_idx, file_name, line_num, col_num);
		}
	}
}
#endif // START_UP_TEST

//-----------------------------------------------------------------------------
#ifdef ENABLE_HISTORY
PRIVATE void goto_pos_by_history(const char *full_path)
{
	const char *str;

	// search in history
	str = search_history_file_path(HISTORY_TYPE_IDX_CURSPOS, full_path);
	// get line-num and col-num
	if (goto_str_line_col_in_cur_buf(str)) {
		CEBV0_CL = CEBV1_CL = CEBV_CL;
		CEBV0_CLBI = CEBV1_CLBI = CEBV_CLBI;
	}
}
#endif // ENABLE_HISTORY
//-----------------------------------------------------------------------------
PRIVATE char memorized_file_pos_str[MAX_PATH_LEN+1];
char *memorize_cur_file_pos_null(char *buffer)
{
	buffer = buffer ? buffer : memorized_file_pos_str;
	mk_cur_file_pos_str(buffer);
////
flf_d_printf("buffer: [%s]\n", buffer);
	return buffer;
}

int recall_cur_file_pos_null(const char *str)
{
	char file_path[MAX_PATH_LEN+1];

	if (switch_c_e_b_to_file_name(
	 get_file_line_col_from_str_null(str, file_path, NULL, NULL)) == 0) {
		return 0;
	}
	goto_str_line_col_in_cur_buf(str);
	return 1;
}
int goto_str_line_col_in_cur_buf(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	get_file_line_col_from_str_null(str, file_path, &line_num, &col_num);
	return goto_line_col_in_cur_buf(line_num, col_num);
}
int goto_line_col_in_cur_buf(int line_num, int col_num)
{
////flf_d_printf("%d, %d\n", line_num, col_num);
	if (line_num <= 0) {
		return 0;
	}
	CEBV_CL = get_line_ptr_from_cur_buf_line_num(line_num);
	if (col_num <= 0) {
		return 1;
	}
#ifdef CURSOR_POS_BYTE
	// col_num is byte count
	CEBV_CLBI = byte_idx_from_byte_idx(CEBV_CL->data, col_num-1);
#endif // CURSOR_POS_BYTE
#ifdef CURSOR_POS_COLUMN
	// col_num is column in view
	CEBV_CLBI = byte_idx_from_col_idx(CEBV_CL->data, col_num-1, CHAR_LEFT, NULL);
#endif // CURSOR_POS_COLUMN
	return 2;
}
//-----------------------------------------------------------------------------
char *mk_cur_file_pos_str_static(void)
{
	static char buffer[MAX_PATH_LEN+1];
	return mk_cur_file_pos_str(buffer);
}
char *mk_cur_file_pos_str(char *buffer)
{
#ifdef CURSOR_POS_BYTE
	// memorize byte number
	return mk_file_pos_str(buffer, get_c_e_b()->file_path, CEBV_CL->line_num,
	 CEBV_CL->data ? byte_idx_from_byte_idx(CEBV_CL->data, CEBV_CLBI)+1 : 0);
#endif // CURSOR_POS_BYTE
#ifdef CURSOR_POS_COLUMN
	// memorize column number
	return mk_file_pos_str(buffer, get_c_e_b()->file_path, CEBV_CL->line_num,
	 CEBV_CL->data ? col_idx_from_byte_idx(CEBV_CL->data, 0, CEBV_CLBI)+1 : 0);
#endif // CURSOR_POS_COLUMN
}
char *mk_file_pos_str(char *buffer, const char *file_path, int line_num, int col_num)
{
	file_path = quote_file_name(file_path);
	if (col_num <= 0) {
		if (line_num <= 0) {
			// /path/to/file.ext
			snprintf_(buffer, MAX_PATH_LEN+1, "%s", file_path);
		} else {
			// /path/to/file.ext|999
			snprintf_(buffer, MAX_PATH_LEN+1, "%s%s%d", file_path, FILE_PATH_SEPARATOR, line_num);
		}
	} else {
		// /path/to/file.ext|999:99
		snprintf_(buffer, MAX_PATH_LEN+1, "%s%s%d:%d",
		 file_path, FILE_PATH_SEPARATOR, line_num, col_num);
	}
	return buffer;
}
//-----------------------------------------------------------------------------
PRIVATE int get_file_line_col_from_str(const char *str, char *file_path,
 int *line_num_, int *col_num_);
const char *get_file_line_col_from_str_null(const char *str, char *file_path,
 int *line_num, int *col_num)
{
	str = str ? str : memorized_file_pos_str;
	get_file_line_col_from_str(str, file_path, line_num, col_num);
	return file_path;
}
// /home/user/tools/be/src/editorgoto.c|400:10
//  => "/home/user/tools/be/src/editorgoto.c", 400, 10
// '/home/user/tools/be/src/ file name.txt '|400:10
//  => "/home/user/tools/be/src/ file name.txt ", 400, 10
PRIVATE int get_file_line_col_from_str(const char *str, char *file_path,
 int *line_num_, int *col_num_)
{
	const char *ptr;
	const char *fn_begin;
	const char *fn_end;
	int line_num;
	int col_num;

	strcpy__(file_path, "");
	line_num = 0;
	col_num = 0;

	ptr = skip_to_file_path(str);
	if (is_file_path_char(ptr) == 0)
		goto no_file_path;
	fn_begin = ptr;
	ptr = skip_file_path(ptr);
	fn_end = ptr;
	strlcpy__(file_path, fn_begin, LIM_MAX(MAX_PATH_LEN, fn_end - fn_begin));
	unquote_string(file_path);
	// skip to beginning of a line number
	ptr = skip_to_digit(ptr);
	if (isdigit(*ptr)) {
		line_num = atoi(ptr);
	}
	ptr = skip_digits(ptr);
	// skip to beginning of a column number
	ptr = skip_to_digit(ptr);
	if (isdigit(*ptr)) {
		col_num = atoi(ptr);
	}
no_file_path:;
	if (line_num_)
		*line_num_ = line_num;
	if (col_num_)
		*col_num_ = col_num;
///
flf_d_printf("str:[%s] ==> path:[%s] line:%d col:%d\n",
///
 str, file_path, line_num, col_num);
	return strnlen(file_path, MAX_PATH_LEN);
}

// supported file names:
//  |No.| file type                                                | command line | file list |
//  |---|----------------------------------------------------------|--------------|-----------|
//  | 1 |" file name.txt "(includes spaces in head, middle or tail)| supported    | supported |
//  | 2 |'"filename".txt' (includes '"')                           | supported    | supported |
//  | 3 |"file|name.txt"  (includes special chars [|'])            | supported    | supported |
// workaround:
//  |No.| command line      | file list                 | project file             |
//  |---|-------------------|---------------------------|--------------------------|
//  | 1 | " file name.txt " | " file name.txt ",100,10  | 100,10," file name.txt " |
//  | 2 | "filename".txt    | "\"filename.txt\"",100,10 | 100,10,"\"filename.txt\""|
//  | 2 | "file|name.txt"   | "file|name.txt",100,10    | 100,10,"file|name.txt"   |
//  | 2 | "file'name.txt"   | "file'name.txt",100,10    | 100,10,"file|name.txt"   |

// End of editorgoto.c
