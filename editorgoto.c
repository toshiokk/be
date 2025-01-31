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

PRIVATE int load_files_in_string_(const char *string, int flags);
PRIVATE int load_file_in_string_(const char *string, int flags);

PRIVATE int load_file_name_recurs_(const char *file_name, int flags);
PRIVATE int load_files_in_cur_buf_(int flags);
PRIVATE int load_file_name__(const char *file_name, int flags);

#ifdef ENABLE_HISTORY
PRIVATE int load_file_from_history(const char *file_name);
PRIVATE void goto_pos_by_history(const char *full_path);
#endif // ENABLE_HISTORY

PRIVATE const char *skip_n_file_names(const char *line, int field_idx);

// "100" ==> column number
int doe_goto_column(void)
{
	char buf[MAX_PATH_LEN+1] = "";
	if (chk_inp_str_ret_val_editor(input_string_pos(buf, buf, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_SEARCH,
	 _("Enter column number:")))) {
		return 0;
	}
	int col_num = 0;
	if (sscanf(buf, "%d", &col_num) > 0) {
		EPCBVC_CLBI = byte_idx_from_col_idx(EPCBVC_CL->data, col_num - 1, CHAR_LEFT, NULL);
		return 1;
	}
	disp_status_bar_done(_("Cancelled"));
	return 0;
}
// "123" ==> line number
// "file.ext:123:45"
int doe_goto_line(void)
{
	char buf[MAX_PATH_LEN+1] = "";
	if (chk_inp_str_ret_val_editor(input_string_pos(buf, buf, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_FILE,
	 _("Enter line number:")))) {
		return 0;
	}
	int line_num;
	if (sscanf(buf, "%d", &line_num) > 0) {
		// go to line
		goto_line_col_in_cur_buf(line_num, 1);
		post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
		return 1;
	}
	// go to file
	// CURDIR: changed in editor
	// file.ext:123:45
	load_files_in_string(buf, TUL0 | OOE0 | MOE1 | LFH0 | WRP0 | FOL0 | RECURS0);
	post_cmd_processing(NULL, CURS_MOVE_VERT, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
	return 1;
}

// |function name                         |difference|
// |--------------------------------------|----------|
// |doe_goto_file_or_dir_in_cur_line      |jump to the file or directory gotten from line top  |
// |doe_goto_file_or_dir_in_cur_cursor_pos|jump to the file or directory gotten from cursor pos|

PRIVATE int goto_file_in_cur_line_byte_idx(int line_byte_idx);

// TAG JUMP (file_path is taken from the head of current line)
int doe_goto_file_or_dir_in_cur_line(void)
{
	if (goto_file_in_cur_line_byte_idx(0) >= 0) {
		// files opened
		return 1;
	}
#ifdef ENABLE_FILER
	// going to change directory
	return try_to_open_dir_in_cur_line_with_filer(0);
#else // ENABLE_FILER
	return 0;
#endif // ENABLE_FILER
}
// TAG JUMP (file_path is taken from the current cursor position)
int doe_goto_file_or_dir_in_cur_cursor_pos(void)
{
	if (goto_file_in_cur_line_byte_idx(EPCBVC_CLBI) >= 0) {
		// files opened
		return 1;
	}
#ifdef ENABLE_FILER
	// going to change directory
	return try_to_open_dir_in_cur_line_with_filer(EPCBVC_CLBI);
#else // ENABLE_FILER
	return 0;
#endif // ENABLE_FILER
}

#ifdef ENABLE_FILER
int doe_goto_directory_in_cur_line()
{
	return try_to_open_dir_in_cur_line_with_filer(0);
}

int doe_goto_directory_in_cur_cursor_pos()
{
	// going to change directory
	return try_to_open_dir_in_cur_line_with_filer(EPCBVC_CLBI);
}
#endif // ENABLE_FILER

PRIVATE int goto_file_in_cur_line_byte_idx(int line_byte_idx)
{
	char dir_save[MAX_PATH_LEN+1];
	clear_files_loaded();

	memorize_cur_file_pos_before_jump();
	// CURDIR: changed to cur-file's abs-dir
	change_cur_dir_by_file_path_after_save(dir_save, get_epc_buf()->file_path_);
	// file_path is taken from the line_byte_idx of current line
	int files = load_files_in_string(&(EPCBVC_CL->data[line_byte_idx]),
	 TUL0 | OOE0 | MOE1 | LFH1 | WRP0 | FOL0 | RECURS1);
	change_cur_dir(dir_save);

	disp_files_loaded_if_ge_0();
	if (files >= 0) {
		post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_CENTER, UPDATE_SCRN_ALL);
		editor_do_next = EF_LOADED;
	}
	return files;
}

int doe_open_files_in_buf(void)
{
	load_files_in_cur_buf();
	disp_files_loaded_if_ge_0();
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	return 0;
}
//------------------------------------------------------------------------------
int doe_switch_to_top_buffer(void)
{
	if (switch_epc_buf_to_top() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Switched to the first buffer"));
	return 1;
}
int doe_switch_to_bot_buffer(void)
{
	if (switch_epc_buf_to_bot() == 0) {
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL_SOON);
	disp_status_bar_done(_("Switched to the last buffer"));
	return 1;
}
int doe_switch_to_prev_buffer(void)
{
	if (switch_epc_buf_to_prev(1, 0) == 0) {
		disp_status_bar_warn(_("No previous buffer"));
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Switched to the previous buffer"));
	return 1;
}
int doe_switch_to_next_buffer(void)
{
	if (switch_epc_buf_to_next(1, 0) == 0) {
		disp_status_bar_warn(_("No next buffer"));
		return 0;
	}
	post_cmd_processing(NULL, CURS_MOVE_HORIZ, LOCATE_CURS_NONE, UPDATE_SCRN_ALL);
	disp_status_bar_done(_("Switched to the next buffer"));
	return 1;
}
//------------------------------------------------------------------------------
int doe_switch_to_prev_buffers(void)
{
	be_bufs_t *bufs = set_cur_buf_to_bufs(get_epc_buf());
	if (IS_PREV_NODE_INT(bufs) == 0) {
		disp_status_bar_warn(_("No previous buffers"));
		return 0;
	}
	bufs = NODE_PREV(bufs);
	bufs_fix_cur_buf(bufs);
	set_epc_buf(bufs->cur_buf);
	disp_status_bar_done(_("Switched to the previous buffers"));
	return 1;
}
int doe_switch_to_next_buffers(void)
{
	be_bufs_t *bufs = set_cur_buf_to_bufs(get_epc_buf());
	if (IS_NEXT_NODE_INT(bufs) == 0) {
		disp_status_bar_warn(_("No next buffers"));
		return 0;
	}
	bufs = NODE_NEXT(bufs);
	bufs_fix_cur_buf(bufs);
	set_epc_buf(bufs->cur_buf);
	disp_status_bar_done(_("Switched to the next buffers"));
	return 1;
}
//------------------------------------------------------------------------------
PRIVATE char cur_file_pos_before_jump[MAX_PATH_LEN+1] = "";
void memorize_cur_file_pos_before_jump()
{
	memorize_cur_file_pos_null(cur_file_pos_before_jump);
}
int doe_return_to_prev_file_pos(void)
{
	recall_file_pos_null(cur_file_pos_before_jump);

	doe_refresh_editor();
	disp_status_bar_done(_("Returned to previous pos"));
	return 1;
}
//------------------------------------------------------------------------------

// Top level functions:
int load_file_name_upp_low(const char *file_name, int flags)
{
	clear_files_loaded();
	begin_check_break_key();
	int ret = load_file_name_upp_low_(file_name, flags);
	end_check_break_key();
	return ret;
}
int load_files_in_cur_buf(void)
{
	clear_files_loaded();
	begin_check_break_key();
	int ret = load_files_in_cur_buf_(LFH0);
	end_check_break_key();
	return ret;
}
int load_files_in_string(const char *string, int flags)
{
	clear_files_loaded();
	begin_check_break_key();
	int files = load_files_in_string_(string, flags);
	end_check_break_key();
	return files;
}

//=============================================================================

// sub-function of load_files_in_string()
PRIVATE int load_files_in_string_(const char *string, int flags)
{
	int files_loaded = -1;

	for (int field_idx = 0; field_idx < MAX_FILES_TO_TRY_TO_LOAD_IN_A_LINE; field_idx++) {
		char file_path[MAX_PATH_LEN+1];
		if (get_n_th_file_line_col_from_str(string, field_idx,
		 file_path, NULL, NULL) > 0) {
			int files = load_file_in_string_(file_path, flags);
			if (files >= 0) {
				// once any file has loaded, show no more error message
				files_loaded = LIM_MIN(0, files_loaded);
				files_loaded += files;
				flags &= ~OOE1;
				flags &= ~MOE1;
			}
		}
	}
	return files_loaded;
}
PRIVATE int load_file_in_string_(const char *string, int flags)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

flf_d_printf("try to open file in string [%s]\n", string);
	if (get_file_line_col_from_str(string, file_path, &line_num, &col_num) == 0) {
		return -1;	// nothing loaded nor selected
	}
	int files = load_file_name_upp_low_(file_path, flags);
	if (files >= 0) {
		// Tag-jump to line, col
		goto_line_col_in_cur_buf(line_num, col_num);	// appdefs.h|100,8
	}
	return files;
}

// sub-function of load_file_name_upp_low()
int load_file_name_upp_low_(const char *file_name, int flags)
{
	char file_name_buf[MAX_PATH_LEN+1];
	char file_name_prev[MAX_PATH_LEN+1];

	for (int name_type_idx = 0; name_type_idx < ((flags & TUL1) ? 3 : 1); name_type_idx++) {
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
		if (strcmp(file_name_buf, file_name_prev) != 0) {
			int files = load_file_name_recurs_(file_name_buf, flags);
			if (files >= 0) {
				return files;
			}
		}
		if (check_break_key()) {
			break;
		}
		strlcpy__(file_name_prev, file_name_buf, MAX_PATH_LEN);
	}
	return -1;	// not loaded
}

// Open file. If it is a project file, open file(s) described in it.
PRIVATE int load_file_name_recurs_(const char *file_name, int flags)
{
	static int recursive_call_count = 0;

	int files = load_file_name__(file_name, flags);

	if (files >= 0 && (recursive_call_count == 0) && (flags & RECURS1)
	 && is_file_name_proj_file(file_name, 0)) {
		recursive_call_count++;
		files += load_files_in_cur_buf_(flags);
		recursive_call_count--;
	}
	return files;
}

// sub-function of load_files_in_cur_buf()
PRIVATE int load_files_in_cur_buf_(int flags)
{
#define MAX_LINES_TO_TRY_TO_LOAD		10000
#define MAX_FILES_TO_LOAD				3844
#define MIN_FREE_MEM_KB					(100 * 1000)	// 100 MB
	char file_pos_str1[MAX_PATH_LEN+1];
	memorize_cur_file_pos_null(file_pos_str1);
	first_line();
	int files = 0;
	int ret = -1;
	for (int lines = 0; lines < MAX_LINES_TO_TRY_TO_LOAD; lines++) {
		if (line_strlen(EPCBVC_CL) && (EPCBVC_CL->data[0] != '#')) {
			char file_pos_str2[MAX_PATH_LEN+1];
			char dir_save[MAX_PATH_LEN+1];

			memorize_cur_file_pos_null(file_pos_str2);
			// CURDIR: changed to cur-file's abs-dir
			change_cur_dir_by_file_path_after_save(dir_save, get_epc_buf()->file_path_);
			ret = load_files_in_string_(EPCBVC_CL->data, flags);
			change_cur_dir(dir_save);

			disp_title_bar_editor();
			tio_refresh();
			recall_file_pos_null(file_pos_str2);
		}
		if (ret >= 0) {
			files += ret;
		}
		if (next_line() == 0) {
			break;
		}
		if (files >= MAX_FILES_TO_LOAD) {
			break;
		}
		if (get_mem_free_in_kb(1) <= MIN_FREE_MEM_KB) {
			break;
		}
		if (check_break_key()) {
flf_d_printf("sigint_signaled\n");
			break;
		}
	}
	recall_file_pos_null(file_pos_str1);
	return files;	// 0--
}

PRIVATE int load_file_name__(const char *file_name, int flags)
{
flf_d_printf("[%s]\n", file_name);
	char full_path[MAX_PATH_LEN+1];
	char abs_path[MAX_PATH_LEN+1];
	int files_loaded = -1;

	get_full_path(file_name, full_path);
	get_abs_path(file_name, abs_path);
	// switch to the file of "full_path" if it already loaded
	if (switch_epc_buf_by_file_path(full_path)) {
		add_files_loaded(0);		// switched
		// already loaded and select it
		goto not_goto_line;
	}
	// try to load the file
	int lines = load_file_into_new_buf(full_path, flags);
	if (lines >= 0) {
		if (lines <= MAX_LINES_LOADABLE) {
			add_files_loaded(1);
		}
		files_loaded = LIM_MIN(0, files_loaded);
		files_loaded++;
		goto goto_line;
	}
	// switch to if the file of the "file-name" already loaded
	if (switch_epc_buf_by_file_name(file_name)) {
		add_files_loaded(0);		// switched
		// already loaded and select it
		goto not_goto_line;
	}
#ifdef ENABLE_HISTORY
	if (flags & LFH1) {
		// try to load a file of the same "file-name" memorized in history
		if (load_file_from_history(file_name) >= 0) {
			add_files_loaded(0);	// switched
			goto goto_line;
		}
	}
#endif // ENABLE_HISTORY
	return -1;				// loading failed

goto_line:
#ifdef ENABLE_HISTORY
	goto_pos_by_history(full_path);
#endif // ENABLE_HISTORY
not_goto_line:
	files_loaded = LIM_MIN(0, files_loaded);
#ifdef ENABLE_SYNTAX
	set_file_type_and_tab_size_by_cur_file_path();
#endif // ENABLE_SYNTAX
	return files_loaded;	// x > 0: files newly loaded, x == 0: file selected
}

#ifdef ENABLE_HISTORY
// may load one of below files
// /path/to/filename.ext
// /path/to/dir1/filename.ext
// /path/to/dir1/dir2/filename.ext
PRIVATE int load_file_from_history(const char *file_name)
{
	set_history_newest(HISTORY_TYPE_IDX_FILE);
	for ( ; ; ) {
		const char *history = get_history_older(HISTORY_TYPE_IDX_FILE);
		if (strlen_path(history) == 0) {
			break;
		}
		char file_path[MAX_PATH_LEN+1];
		if (get_file_line_col_from_str(history, file_path, NULL, NULL)) {
			if (compare_file_path_from_tail(file_path, file_name) == 0) {
				return load_file_in_string_(history,
				 TUL0 | OOE0 | MOE0 | LFH0 | WRP0 | FOL0 | RECURS0);
			}
		}
	}
	return -1;		// nothing loaded nor selected
}

PRIVATE void goto_pos_by_history(const char *full_path)
{
	// search in history
	const char *str = search_history_file_path(HISTORY_TYPE_IDX_FILE, full_path);
	// get line-num and col-num
	if (goto_str_line_col_in_cur_buf(str)) {
		EPCBVX_CL(0) = EPCBVX_CL(1) = EPCBVC_CL;
		EPCBVX_CLBI(0) = EPCBVX_CLBI(1) = EPCBVC_CLBI;
	}
}
#endif // ENABLE_HISTORY
//------------------------------------------------------------------------------
int is_file_name_proj_file(const char *file_name, int type)
{
	if ((type == 0 || type == 1)
	 && strlen_path(file_name) >= strlen(PROJ_FILE_EXTENSION1)) {
		if (strcasecmp(
		 &file_name[strlen_path(file_name) - strlen(PROJ_FILE_EXTENSION1)],
	     PROJ_FILE_EXTENSION1) == 0) {
			return 1;
		}
	}
	if ((type == 0 || type == 2)
	 && strlen_path(file_name) >= strlen(PROJ_FILE_EXTENSION2)) {
		if (strcasecmp(
		 &file_name[strlen_path(file_name) - strlen(PROJ_FILE_EXTENSION2)],
	     PROJ_FILE_EXTENSION2) == 0) {
			return 1;
		}
	}
	return 0;	// not project file
}

#ifdef ENABLE_FILER
#ifdef START_UP_TEST
void test_get_n_th_file(void)
{
	char test_str[] =
	 "history.c 345 hist_type_idx:3:['/home/user/ filename including space .txt '|1:1]";
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	int len;
	len = get_n_th_file_line_col_from_str(test_str, 0, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "history.c");		MY_UT_INT(line_num, 345);	MY_UT_INT(col_num, 3);

	len = get_n_th_file_line_col_from_str(test_str, 1, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "345");			MY_UT_INT(line_num, 0);	MY_UT_INT(col_num, 0);

	len = get_n_th_file_line_col_from_str(test_str, 2, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "hist_type_idx");	MY_UT_INT(line_num, 3);	MY_UT_INT(col_num, 1);

	len = get_n_th_file_line_col_from_str(test_str, 3, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "3");				MY_UT_INT(line_num, 0);	MY_UT_INT(col_num, 0);

	len = get_n_th_file_line_col_from_str(test_str, 4, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "/home/user/ filename including space .txt ");
											MY_UT_INT(line_num, 1);	MY_UT_INT(col_num, 1);

	len = get_n_th_file_line_col_from_str(test_str, 5, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "1");				MY_UT_INT(line_num, 1);	MY_UT_INT(col_num, 0);

	len = get_n_th_file_line_col_from_str(test_str, 6, file_path, &line_num, &col_num);
	MY_UT_INT(len, strlen(file_path));
	MY_UT_STR(file_path, "1");				MY_UT_INT(line_num, 0);	MY_UT_INT(col_num, 0);

	len = get_n_th_file_line_col_from_str(test_str, 7, file_path, &line_num, &col_num);
	MY_UT_INT(len, -1);
	MY_UT_STR(file_path, "");				MY_UT_INT(line_num, 0);	MY_UT_INT(col_num, 0);
	flf_d_printf("---------------------------------------------------------\n");
}
#endif // START_UP_TEST

int get_n_th_file_line_col_from_str(const char *str, int field_idx,
 char *file_path, int *line_num, int *col_num)
{
	strcpy__(file_path, "");
	const char *ptr = skip_n_file_names(str, field_idx);
	if (*ptr == '\0') {
		return -1;		// end of line
	}
	return get_file_line_col_from_str(ptr, file_path, line_num, col_num);
}
#endif // ENABLE_FILER

PRIVATE const char *skip_n_file_names(const char *line, int field_idx)
{
	const char *ptr = line;
	for (int field_cnt = 0; ; field_cnt++) {
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

//------------------------------------------------------------------------------
#ifdef ENABLE_HISTORY
int goto_last_file_line_col_in_history()
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	if (get_file_line_col_from_str(get_history_newest(HISTORY_TYPE_IDX_FILE, 1),
	 file_path, &line_num, &col_num) == 0) {
		return 0;
	}
	if (switch_epc_buf_by_file_path(file_path) == 0) {
		return 0;
	}
	return goto_line_col_in_cur_buf(line_num, col_num);
}
#endif // ENABLE_HISTORY
//------------------------------------------------------------------------------
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
	mk_cur_file_pos_str_buf(buffer);
	return buffer;
}

int recall_file_pos_null(const char *str)
{
	char file_path[MAX_PATH_LEN+1];

	if (get_file_line_col_from_str(str, file_path, NULL, NULL)) {
		if (switch_epc_buf_by_file_path(file_path) == 0) {
			return 0;
		}
	}
	return goto_str_line_col_in_cur_buf(str);
}
int goto_str_line_col_in_cur_buf(const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	int line_num, col_num;

	get_file_line_col_from_str(str, file_path, &line_num, &col_num);
	return goto_line_col_in_cur_buf(line_num, col_num);
}
int goto_line_col_in_cur_buf(int line_num, int col_num)
{
	if (line_num <= 0) {
		return 0;
	}
	EPCBVC_CL = get_line_ptr_in_cur_buf_by_line_num(line_num);
	EPCBVC_CLBI = 0;
	if (col_num < 0) {	// if colnum == -1, updata both pane
		buf_set_view_x_cur_line(get_epc_buf(), 0, EPCBVC_CL);
		BUFV0_CLBI(get_epc_buf()) = EPCBVC_CLBI;
		buf_set_view_x_cur_line(get_epc_buf(), 1, EPCBVC_CL);
		BUFV1_CLBI(get_epc_buf()) = EPCBVC_CLBI;
	}
	if (col_num <= 0) {
		return 1;
	}
	// col_num is byte count
	EPCBVC_CLBI = byte_idx_from_byte_idx(EPCBVC_CL->data, col_num-1);
	return 2;
}
//------------------------------------------------------------------------------
char *mk_cur_file_pos_str_static(void)
{
	static char buffer[MAX_PATH_LEN+1];
	return mk_cur_file_pos_str_buf(buffer);
}
char *mk_cur_file_pos_str_buf(char *buffer)
{
	// memorize byte number
	return mk_file_pos_str(buffer, get_epc_buf()->file_path_, EPCBVC_CL->line_num,
	 EPCBVC_CL->data ? byte_idx_from_byte_idx(EPCBVC_CL->data, EPCBVC_CLBI)+1 : 0);
}
char *mk_file_pos_str(char *buffer, const char *file_path, int line_num, int col_num)
{
	file_path = quote_file_path_static(file_path);
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
//------------------------------------------------------------------------------
PRIVATE int get_file_line_col_from_str__(const char *str, char *file_path,
 int *line_num_, int *col_num_);

int get_file_line_col_from_str(const char *str, char *file_path,
 int *line_num, int *col_num)
{
	str = get_memorized_file_pos_str(str);
	return get_file_line_col_from_str__(str, file_path, line_num, col_num);
}

// supported formats:
//  ##						// ignore any string
//  #fileio.h				// file name in comment
//  #include <fileio.h>		// C include directive
//  fileio.h:20:10			// GCC error message
//  fileio.h,10				// C compiler error message
//  fileio.h(20:10)			// M$ C compiler error message
//  #:fileio.h,10			// gettext *.po file
//  fileio.h 20:10			// file-name line-num:col-num
//  fileio.h   20 10		// (separate by less than 4 spaces -> goto line-N)
//  11/04/13 16:40:00, fileio.h:20:10,	// event log file
//  <location filename="fileio.h" line="10"/>	// Qt-lupdate
//  diff fileio.h fileio.h~	// command line
//  SOURCES += fileio.h		// Qt project file
//  /home/user/tools/be/be/editorgoto.c|400:10 // BE file pos format
//	  => "/home/user/tools/be/src/editorgoto.c", 400, 10
//  '/home/user/tools/be/src/ file name.txt '|400:10 // BE file pos format (quoted)
//	  => "/home/user/tools/be/src/ file name.txt ", 400, 10
// un-supported or avoided formats:
//  fileio.h    20 10		// (separate by more than 4 spaces -> does not goto line-N)

PRIVATE int get_file_line_col_from_str__(const char *str, char *file_path,
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
	if (! is_char_file_path(ptr)) {
		goto no_file_path;
	}
	fn_begin = ptr;
	ptr = skip_file_path(ptr);
	fn_end = ptr;
	strlcpy__(file_path, fn_begin, LIM_MAX(MAX_PATH_LEN, fn_end - fn_begin));
	unquote_string(file_path);
	// skip to beginning of a line number
	ptr = skip_one_separator(ptr);
	ptr = skip_two_spaces(ptr);
	if (! isdigit(*ptr)) {
		goto no_file_path;
	}
	line_num = atoi(ptr);
	ptr = skip_digits(ptr);
	// skip to beginning of a column number
	ptr = skip_to_digit(ptr);
	if (! isdigit(*ptr)) {
		goto no_file_path;
	}
	col_num = atoi(ptr);
no_file_path:;
	if (line_num_)
		*line_num_ = line_num;
	if (col_num_)
		*col_num_ = col_num;
	return strlen_path(file_path);
}
//------------------------------------------------------------------------------
int switch_epc_buf_by_file_path(const char *file_path)
{
	be_buf_t *buf = get_edit_buf_by_file_path(file_path);
	if (buf) {
		set_epc_buf(buf);
		return 1;	// switched
	}
	return 0;		// not found
}
int switch_epc_buf_by_file_name(const char *file_name)
{
	be_buf_t *buf = get_edit_buf_by_file_name(file_name);
	if (buf) {
		set_epc_buf(buf);
		return 1;	// switched
	}
	return 0;		// not found
}

int switch_epc_buf_to_top(void)
{
	if (IS_NODE_ANCH(EDIT_BUFS_TOP_NODE))
		return 0;
	set_epc_buf(EDIT_BUFS_TOP_NODE);
	return 1;
}
int switch_epc_buf_to_bot(void)
{
	if (IS_NODE_ANCH(EDIT_BUFS_BOT_NODE))
		return 0;
	set_epc_buf(EDIT_BUFS_BOT_NODE);
	return 1;
}
int switch_epc_buf_to_prev(int beep_at_end, int goto_bottom)
{
	if (IS_NODE_TOP_MOST(get_epc_buf())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_epc_buf(NODE_PREV(get_epc_buf()));
	if (goto_bottom) {
		last_line();
	}
	return 1;
}
int switch_epc_buf_to_next(int beep_at_end, int goto_top)
{
	if (IS_NODE_BOT_MOST(get_epc_buf())) {
		if (beep_at_end)
			tio_beep();
		return 0;
	}
	set_epc_buf(NODE_NEXT(get_epc_buf()));
	if (goto_top) {
		first_line();
	}
	return 1;
}
int switch_epc_buf_to_another_buf(void)
{
	if (switch_epc_buf_to_next(0, 0) == 0) {
		if (switch_epc_buf_to_prev(0, 0) == 0) {
			set_epc_buf(buf_get_another_buf(get_epc_buf()));
			return 0;
		}
	}
	return 1;
}

//------------------------------------------------------------------------------
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
//  | 2 | "filename.txt"    | "\"filename.txt\"",100,10 | 100,10,"\"filename.txt\""|
//  | 2 | "file|name.txt"   | "file|name.txt",100,10    | 100,10,"file|name.txt"   |
//  | 2 | "file'name.txt"   | "file'name.txt",100,10    | 100,10,"file|name.txt"   |

#ifdef ENABLE_FILER
int doe_filer(void)
{
	return try_to_open_dir_in_cur_line_with_filer(0);
}
int try_to_open_dir_in_cur_line_with_filer(int line_byte_idx)
{
	return try_to_open_dir_in_str_with_filer(&(EPCBVC_CL->data[line_byte_idx]));
}
int try_to_open_dir_in_str_with_filer(const char *str)
{
	char buf_dir[MAX_PATH_LEN+1];
	if (check_to_change_dir_in_string(str, buf_dir) == 0) {
		disp_status_bar_err(_("No valid directory in the current line"));
		strlcpy__(buf_dir, ".", MAX_PATH_LEN);
		return 0;
	}
	char file_path[MAX_PATH_LEN+1];
	do_call_filer(1, APP_MODE_NORMAL, buf_dir, "", file_path, MAX_PATH_LEN);
	disp_status_bar_done(_("Filer quited"));
	return 1;
}
#endif // ENABLE_FILER

// End of editorgoto.c
