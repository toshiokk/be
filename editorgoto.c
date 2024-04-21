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

PRIVATE int load_file_in_string(const char *string,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive);

#ifdef ENABLE_HISTORY
PRIVATE void goto_pos_by_history(const char *full_path);
#endif // ENABLE_HISTORY

PRIVATE const char *skip_n_file_names(const char *line, int field_idx);

// 123
// file.ext:123:45
int doe_goto_input_line(void)
{
	char string[MAX_PATH_LEN+1];
	int ret;
	int line_num;

	ret = input_string_tail("", string, HISTORY_TYPE_IDX_CURSPOS, _("Enter line number:"));

	if (ret < 0) {
		return 0;
	}
	if (ret <= 0) {
		return 0;
	}
	if (sscanf(string, "%d", &line_num) > 0) {
		// go to line
		goto_line_col_in_cur_buf(line_num, 1);
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
		return 1;
	}
	// go to file
	// CURDIR: changed in editor
	load_files_in_string(string, 1, TUL0, OOE0, MOE1, RECURSIVE0);	// file.ext:123:45
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return 1;
}

/////PRIVATE int check_cur_line_file_or_directory(void);
/////PRIVATE int check_cur_line_file_or_directory(void)
/////{
/////	char file_path[MAX_PATH_LEN+1];
/////	if (get_file_line_col_from_str_null(CEPBV_CL->data, file_path, NULL, NULL) == 0) {
/////		return 0;
/////	}
/////	return get_file_type_by_file_path(file_path);
/////}

int doe_goto_file_or_dir_in_cur_line(void)
{
	if (doe_goto_file_in_cur_line() > 0) {
_FLF_
		// files opened
		return 1;
	}
_FLF_
	// going to change directory
	return doe_goto_directory_in_cur_line();
}
int doe_goto_file_or_dir_in_cur_cursor_pos(void)
{
	if (doe_goto_file_in_cur_cursor_pos() > 0) {
_FLF_
		// files opened
		return 1;
	}
_FLF_
	// going to change directory
	return doe_goto_directory_in_cur_line();
}

// TAG JUMP
int doe_goto_file_in_cur_line(void)
{
	char dir_save[MAX_PATH_LEN+1];
	clear_files_loaded();

	// CURDIR: changed to cur-file's abs-dir
	change_cur_dir_by_file_path_after_save(dir_save, get_cep_buf()->abs_path);
	// file_path is taken from the head of current line
	int files = load_files_in_string(CEPBV_CL->data, 10, TUL1, OOE0, MOE1, RECURSIVE1);
	change_cur_dir(dir_save);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return files;
}
int doe_goto_file_in_cur_cursor_pos(void)
{
	char dir_save[MAX_PATH_LEN+1];
	clear_files_loaded();

	// CURDIR: changed to cur-file's abs-dir
	change_cur_dir_by_file_path_after_save(dir_save, get_cep_buf()->abs_path);
	// file_path is taken from the current cursor position
	int files = load_files_in_string(&(CEPBV_CL->data[CEPBV_CLBI]), 1, TUL1, OOE0, MOE1, RECURSIVE1);
	change_cur_dir(dir_save);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return files;
}

int doe_goto_directory_in_cur_line(void)
{
#ifdef ENABLE_FILER
	char file_path[MAX_PATH_LEN+1];
	change_cur_dir_by_file_path(get_cep_buf()->abs_path);
	if (change_dirs_in_string(CEPBV_CL->data, get_cur_filer_view()->cur_dir,
	 get_cur_filer_view()->prev_dir, get_cur_filer_view()->next_file) == 0) {
		// directory can not be changed
		return 0;
	}
#ifdef ENABLE_HISTORY
	update_dir_history(get_cur_filer_view()->prev_dir, get_cur_filer_view()->cur_dir);
#endif // ENABLE_HISTORY
	call_filer(1, 0, get_cur_filer_view()->cur_dir, "", file_path, MAX_PATH_LEN);
#endif // ENABLE_FILER
	editor_quit = EDITOR_CANCELLED;
	return 1;
}

int change_dirs_in_string(const char *string, char *cur_dir, char *prev_dir, char *next_file)
{
	const char *ptr;
	char dir[MAX_PATH_LEN+1];

	for (int field_idx = 0; ; field_idx++) {
		ptr = skip_n_file_names(string, field_idx);
		if (*ptr == '\0')
			break;
		if (get_file_line_col_from_str_null(ptr, dir, NULL, NULL)) {
flf_d_printf("dir: [%s]\n", dir);
			// directory gotten
			if ((contain_redundant_slash(dir) == 0)
			 && change_cur_dir_in_path(dir, cur_dir, prev_dir, next_file)) {
				return 1;	// changed
			}
			// not changed, try the next directory
		}
	}
	return 0;
}

int doe_open_files_in_buf(void)
{
	clear_files_loaded();
	load_files_in_cur_buf();
	disp_files_loaded();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//-----------------------------------------------------------------------------
int doe_switch_to_top_file(void)
{
	if (switch_cep_buf_to_top() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int doe_switch_to_bot_file(void)
{
	if (switch_cep_buf_to_bot() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
//-----------------------------------------------------------------------------
int doe_switch_to_prev_file(void)
{
	int ret;

	if ((ret = switch_cep_buf_to_prev(1, 0)) == 0) {
		disp_status_bar_err(_("No previous open files"));
		return ret;
	}
_D_(dump_editor_panes())
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Previous file"));
	return ret;
}
int doe_switch_to_next_file(void)
{
	int ret;

	if ((ret = switch_cep_buf_to_next(1, 0)) == 0) {
		disp_status_bar_err(_("No next open files"));
		return ret;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Next file"));
	return ret;
}
#if APP_REL_LVL == APP_REL_LVL_EXPERIMENTAL
int doe_switch_to_prev_buffers(void)
{
	be_bufs_t *bufs = get_bufs_contains_buf(&bufss_top_anchor, get_cep_buf());
	if (IS_NODE_TOP(bufs))
		return 0;
flf_d_printf("bufs: %s\n", bufs->name);
flf_d_printf("NODE_PREV(bufs)->name: %s\n", NODE_PREV(bufs)->name);
flf_d_printf("NODE_PREV(bufs)->cur_buf->name: %s\n", NODE_PREV(bufs)->cur_buf->file_path);
	set_cep_buf(NODE_PREV(bufs)->cur_buf);
	post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
int doe_switch_to_next_buffers(void)
{
	be_bufs_t *bufs = get_bufs_contains_buf(&bufss_top_anchor, get_cep_buf());
	if (IS_NODE_BOT(bufs))
		return 0;
flf_d_printf("bufs: %s\n", bufs->name);
flf_d_printf("NODE_NEXT(bufs)->name: %s\n", NODE_NEXT(bufs)->name);
flf_d_printf("NODE_NEXT(bufs)->cur_buf->name: %s\n", NODE_NEXT(bufs)->cur_buf->file_path);
	set_cep_buf(NODE_NEXT(bufs)->cur_buf);
_D_(bufs_dump_all_bufs(&bufss_top_anchor))
	post_cmd_processing(CEPBV_CL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
#endif // APP_REL_LVL
//-----------------------------------------------------------------------------
int doe_return_to_prev_file_pos(void)
{
	recall_cur_file_pos_null(NULL);

	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	doe_refresh_editor();
	disp_status_bar_done(_("Returned to previous pos"));
	return 1;
}
//-----------------------------------------------------------------------------
int doe_switch_editor_pane(void)
{
	doe_switch_editor_pane_();
	post_cmd_processing(NULL, CURS_MOVE_NONE, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 1;
}
void doe_switch_editor_pane_(void)
{
	int pane_idx;			// pane index

	pane_idx = get_editor_cur_pane_idx() ? 0 : 1;
flf_d_printf("pane_idx: %d\n", pane_idx);
	set_editor_cur_pane_idx(pane_idx);
}

//-----------------------------------------------------------------------------
PRIVATE int load_file_name_recurs_(const char *file_name, int open_on_err, int msg_on_err,
 int recursive);
PRIVATE int load_files_in_cur_buf_(void);
PRIVATE int load_files_in_string_(const char *string,
 int files_to_load, int try_upp_low, int open_on_err, int msg_on_err, int recursive);

// Top level functions - never called recursively
// | function name            | actions                                                            |
// |--------------------------|--------------------------------------------------------------------|
// | load_files_in_cur_buf()  | Load files listed in current buffer								   |
// | load_files_in_string()   | Load files listed in one line of string							   |
// | load_file_in_string()    | Load one file in one line of string and goto pos (appdefs.h|100,8) |
// | load_file_name_upp_low() | Try to open file with name in as-it-is, UPPER and lower case	   |
// | load_file_name_recurs()  | Load file and if it's project file, load files described in it	   |
// | load_file_name__         | Load file and goto pos (memorized in 'openfile_history')		   |
//
// Top level functions must:
//  1. call begin_check_break_key()
//  2. call equivalent sub-function (sub-function can be called recursively)
//  3. call end_check_break_key()

int load_file_name_upp_low(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	begin_check_break_key();
	int ret = load_file_name_upp_low_(file_name, try_upp_low, open_on_err, msg_on_err, recursive);
	end_check_break_key();
	return ret;
}
int load_file_name_recurs(const char *file_name, int open_on_err, int msg_on_err, int recursive)
{
	begin_check_break_key();
	int ret = load_file_name_recurs_(file_name, open_on_err, msg_on_err, recursive);
	end_check_break_key();
	return ret;
}
int load_files_in_cur_buf(void)
{
	begin_check_break_key();
	int ret = load_files_in_cur_buf_();
	end_check_break_key();
	return ret;
}
int load_files_in_string(const char *string,
 int files_to_load, int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
flf_d_printf("string: [%s]\n", string);
	begin_check_break_key();
	int files = load_files_in_string_(string, files_to_load, try_upp_low, open_on_err, msg_on_err,
	 recursive);
	end_check_break_key();
	return files;
}

//-----------------------------------------------------------------------------

// sub-function of load_files_in_cur_buf()
PRIVATE int load_files_in_cur_buf_(void)
{
	char file_pos_str1[MAX_PATH_LEN+1];
	int lines;
	int files = 0;

#define MAX_LINES_TO_TRY_TO_LOAD		10000
#define MAX_FILES_TO_LOAD				2000
#define MIN_FREE_MEM_KB					(100 * 1000)	// 100 MB
	memorize_cur_file_pos_null(file_pos_str1);
	first_line();
	for (lines = 0; lines < MAX_LINES_TO_TRY_TO_LOAD; lines++) {
		if (line_data_len(CEPBV_CL)) {
			if (CEPBV_CL->data[0] != '#') {
				char file_pos_str2[MAX_PATH_LEN+1];
				char dir_save[MAX_PATH_LEN+1];

				memorize_cur_file_pos_null(file_pos_str2);
				// CURDIR: changed to cur-file's abs-dir
				change_cur_dir_by_file_path_after_save(dir_save, get_cep_buf()->abs_path);
				files += load_files_in_string_(CEPBV_CL->data, 10, TUL1, OOE0, MOE0, RECURSIVE0);
				change_cur_dir(dir_save);

				disp_editor_title_bar();
				tio_refresh();
				recall_cur_file_pos_null(file_pos_str2);
			}
		}
		if (cursor_next_line() == 0)
			break;
		if (files >= MAX_FILES_TO_LOAD)
			break;
		if (get_mem_free_in_kb(1) <= MIN_FREE_MEM_KB)
			break;
		if (check_break_key()) {
flf_d_printf("sigint_signaled\n");
			break;
		}
	}
	recall_cur_file_pos_null(file_pos_str1);
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

// sub-function of load_files_in_string()
PRIVATE int load_files_in_string_(const char *string,
 int files_to_load, int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	int files_loaded = 0;

	for (int field_idx = 0; files_loaded < files_to_load; field_idx++) {
		const char *ptr = skip_n_file_names(string, field_idx);
		if (*ptr == '\0')
			break;
		int files = load_file_in_string(ptr, try_upp_low, open_on_err, msg_on_err, recursive);
		if (files > 0) {
			// once any file has loaded, show no more error message
 			files_loaded++;
			open_on_err = 0;
			msg_on_err = 0;
		}
	}
	return files_loaded;
}
PRIVATE int load_file_in_string(const char *string,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;
	int files;

	if (get_file_line_col_from_str_null(string, file_path, &line_num, &col_num) == 0) {
		return 0;
	}
	if ((files = load_file_name_upp_low_(file_path,
	 try_upp_low, open_on_err, msg_on_err, recursive)) > 0) {
		if (recursive) {
			// tagged jump to line, col
			goto_line_col_in_cur_buf(line_num, col_num);	// appdefs.h|100,8
		}
	}
	return files;
}

// sub-function of load_file_name_upp_low()
int load_file_name_upp_low_(const char *file_name,
 int try_upp_low, int open_on_err, int msg_on_err, int recursive)
{
	char file_name_buf[MAX_PATH_LEN+1];
	int files;

	for (int name_type_idx = 0; name_type_idx < (try_upp_low ? 3 : 1); name_type_idx++) {
		strlcpy__(file_name_buf, file_name, MAX_PATH_LEN);
		switch (name_type_idx) {
		case 0:
			// try to open specified file name (FileName.Ext)
			break;
		case 1:
			// try to open in upper case file name (FILENAME.EXT)
			strupper(file_name_buf);
			break;
		case 2:
			// try to open in lower case file name (filename.ext)
			strlower(file_name_buf);
			break;
		}
		if ((files = load_file_name_recurs_(file_name_buf, open_on_err, msg_on_err,
		 recursive)) > 0)
			return files;
		if (check_break_key()) {
			break;
		}
	}
	return 0;	// 0
}

PRIVATE int load_file_name__(const char *file_name, int open_on_err, int msg_on_err);

// Open file. If it is a project file, open file(s) described in it.
// sub-function of load_file_name_recurs()
PRIVATE int load_file_name_recurs_(const char *file_name, int open_on_err, int msg_on_err,
 int recursive)
{
	static int recursive_call_count = 0;
	int files = 0;

	if (load_file_name__(file_name, open_on_err, msg_on_err) > 0) {
		files = 1;
		if (recursive && recursive_call_count == 0 && is_file_name_proj_file(file_name, 0)) {
			recursive_call_count++;
			files += load_files_in_cur_buf_();		// recursive call
			recursive_call_count--;
		}
		add_files_loaded(1);
	}
	return files;
}
PRIVATE int load_file_name__(const char *file_name, int open_on_err, int msg_on_err)
{
	char abs_path[MAX_PATH_LEN+1];

	get_abs_path(file_name, abs_path);
	if (switch_cep_buf_by_abs_path(abs_path)) {
		// already loaded
		return 1;
	}
	if (load_file_into_new_buf(abs_path, open_on_err, msg_on_err) < 0) {
		return 0;
	}
#ifdef ENABLE_HISTORY
	goto_pos_by_history(abs_path);
#endif // ENABLE_HISTORY
	return 1;
}
#ifdef ENABLE_HISTORY
PRIVATE void goto_pos_by_history(const char *full_path)
{
	const char *str;

	// search in history
	str = search_history_file_path(HISTORY_TYPE_IDX_CURSPOS, full_path);
	// get line-num and col-num
	if (goto_str_line_col_in_cur_buf(str)) {
		EPBVX_CL(0) = EPBVX_CL(1) = CEPBV_CL;
		EPBVX_CLBI(0) = EPBVX_CLBI(1) = CEPBV_CLBI;
	}
}
#endif // ENABLE_HISTORY
//-----------------------------------------------------------------------------
int is_file_name_proj_file(const char *file_name, int type)
{
	if ((type == 0 || type == 1)
	 && strnlen(file_name, MAX_PATH_LEN) >= strlen(PROJ_FILE_EXTENSION1)) {
		if (strcasecmp(
		 &file_name[strnlen(file_name, MAX_PATH_LEN) - strlen(PROJ_FILE_EXTENSION1)],
	     PROJ_FILE_EXTENSION1) == 0) {
			return 1;
		}
	}
	if ((type == 0 || type == 2)
	 && strnlen(file_name, MAX_PATH_LEN) >= strlen(PROJ_FILE_EXTENSION2)) {
		if (strcasecmp(
		 &file_name[strnlen(file_name, MAX_PATH_LEN) - strlen(PROJ_FILE_EXTENSION2)],
	     PROJ_FILE_EXTENSION2) == 0) {
			return 1;
		}
	}
	return 0;	// not project file
}

#ifdef START_UP_TEST
void test_get_n_th_file_name(void)
{
	char test_str[] =
	 "history.c 345 hist_type_idx:3:['/home/user/ filename including space .txt '|1:1]";
	const char *ptr;
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	for (int field_idx = 0; field_idx < 10; field_idx++) {
		ptr = skip_n_file_names(test_str, field_idx);
		if (*ptr == '\0')
			break;
		if (get_file_line_col_from_str_null(ptr, file_path, &line_num, &col_num)) {
			flf_d_printf("%d: file_path:[%s],%d,%d\n", field_idx, file_path, line_num, col_num);
		}
	}
}
#endif // START_UP_TEST

PRIVATE const char *skip_n_file_names(const char *line, int field_idx)
{
	const char *ptr;
	int field_cnt;

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
	return ptr;
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_HISTORY
int goto_last_file_line_col_in_loaded()
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	if (get_file_line_col_from_str_null(get_history_newest(HISTORY_TYPE_IDX_CURSPOS, 1),
	 file_path, &line_num, &col_num) == 0) {
		return 0;
	}
	if (switch_cep_buf_by_file_name(file_path) == 0) {
		return 0;
	}
	return goto_line_col_in_cur_buf(line_num, col_num);
}
#endif // ENABLE_HISTORY
//-----------------------------------------------------------------------------
PRIVATE char memorized_file_pos_buf[MAX_PATH_LEN+1] = "";
char *get_memorized_file_pos_buf(char *buffer)
{
	return buffer ? buffer : memorized_file_pos_buf;
}
const char *get_memorized_file_pos_str(const char *str)
{
	return str ? str : memorized_file_pos_buf;
}
char *memorize_cur_file_pos_null(char *buffer)
{
	buffer = get_memorized_file_pos_buf(buffer);
	mk_cur_file_pos_str(buffer);
///flf_d_printf("[%s]\n", buffer);
	return buffer;
}

int recall_cur_file_pos_null(const char *str)
{
	char file_path[MAX_PATH_LEN+1];

	if (get_file_line_col_from_str_null(str, file_path, NULL, NULL)) {
///flf_d_printf("[%s]\n", file_path);
		if (switch_cep_buf_by_file_name(file_path) == 0) {
			return 0;
		}
	}
	return goto_str_line_col_in_cur_buf(str);
}
int goto_str_line_col_in_cur_buf(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	get_file_line_col_from_str_null(str, file_path, &line_num, &col_num);
flf_d_printf("[%s],%d,%d\n", str, line_num, col_num);
	int ret = goto_line_col_in_cur_buf(line_num, col_num);
flf_d_printf("%d\n", ret);
	return ret;
}
int goto_line_col_in_cur_buf(int line_num, int col_num)
{
	if (line_num <= 0) {
		return 0;
	}
	CEPBV_CL = get_line_ptr_from_cur_buf_line_num(line_num);
	CEPBV_CLBI = 0;
	if (col_num <= 0) {
		return 1;
	}
#ifdef CURSOR_POS_BYTE
	// col_num is byte count
	CEPBV_CLBI = byte_idx_from_byte_idx(CEPBV_CL->data, col_num-1);
#endif // CURSOR_POS_BYTE
#ifdef CURSOR_POS_COLUMN
	// col_num is column in view
	CEPBV_CLBI = byte_idx_from_col_idx(CEPBV_CL->data, col_num-1, CHAR_LEFT, NULL);
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
	return mk_file_pos_str(buffer, get_cep_buf()->file_path, CEPBV_CL->line_num,
	 CEPBV_CL->data ? byte_idx_from_byte_idx(CEPBV_CL->data, CEPBV_CLBI)+1 : 0);
#endif // CURSOR_POS_BYTE
#ifdef CURSOR_POS_COLUMN
	// memorize column number
	return mk_file_pos_str(buffer, get_cep_buf()->file_path, CEPBV_CL->line_num,
	 CEPBV_CL->data ? col_idx_from_byte_idx(CEPBV_CL->data, 0, CEPBV_CLBI)+1 : 0);
#endif // CURSOR_POS_COLUMN
}
char *mk_file_pos_str(char *buffer, const char *file_path, int line_num, int col_num)
{
	file_path = quote_file_name_static(file_path);
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

int get_file_line_col_from_str_null(const char *str, char *file_path,
 int *line_num, int *col_num)
{
	str = get_memorized_file_pos_str(str);
	return get_file_line_col_from_str(str, file_path, line_num, col_num);
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
	return strnlen(file_path, MAX_PATH_LEN);
}
//-----------------------------------------------------------------------------
int switch_cep_buf_by_file_name(const char *file_name)
{
	char abs_path[MAX_PATH_LEN+1];

	get_abs_path(file_name, abs_path);
	return switch_cep_buf_by_abs_path(abs_path);
}
int switch_cep_buf_by_abs_path(const char *abs_path)
{
	be_buf_t *buf;

	buf = get_edit_buf_by_abs_path(abs_path);
	if (buf) {
		set_cep_buf(buf);
		return 1;	// switched
	}
	return 0;		// not found
}

int switch_cep_buf_to_top(void)
{
	if (IS_NODE_ANCH(EDIT_BUFS_TOP_BUF))
		return 0;
	set_cep_buf(EDIT_BUFS_TOP_BUF);
	return 1;
}
int switch_cep_buf_to_bot(void)
{
	if (IS_NODE_ANCH(EDIT_BUFS_BOT_BUF))
		return 0;
	set_cep_buf(EDIT_BUFS_BOT_BUF);
	return 1;
}
int switch_cep_buf_to_prev(int beep_at_end, int goto_bottom)
{
	if (IS_NODE_TOP(get_cep_buf())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_cep_buf(NODE_PREV(get_cep_buf()));
	if (goto_bottom) {
		CEPBV_CL = CUR_EDIT_BUF_BOT_LINE;
	}
	return 1;
}
int switch_cep_buf_to_next(int beep_at_end, int goto_top)
{
	if (IS_NODE_BOT(get_cep_buf())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_cep_buf(NODE_NEXT(get_cep_buf()));
	if (goto_top) {
		CEPBV_CL = CUR_EDIT_BUF_TOP_LINE;
	}
	return 1;
}
int switch_cep_buf_to_valid_buf(void)
{
	if (IS_NODE_INT(get_cep_buf()) == 0) {
		return switch_cep_buf_to_another_buf();
	}
	return 1;
}
int switch_cep_buf_to_another_buf(void)
{
	if (switch_cep_buf_to_next(0, 0) == 0) {
		if (switch_cep_buf_to_prev(0, 0) == 0) {
			set_cep_buf(EDIT_BUFS_TOP_ANCH);
			return 0;
		}
	}
	return 1;
}
//-----------------------------------------------------------------------------
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
