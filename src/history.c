/**************************************************************************
 *   history.c                                                            *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#ifdef ENABLE_HISTORY

PRIVATE int load_history_if_file_newer(int hist_type_idx);
PRIVATE void save_history_if_modified_newer__expired(int hist_type_idx);
PRIVATE void save_history_if_modified(int hist_type_idx);
PRIVATE be_buf_t *get_history_buf(int hist_type_idx);

PRIVATE int save_history_idx(int hist_type_idx);
PRIVATE int load_history_idx(int hist_type_idx);
const char *get_history_file_path(int hist_type_idx);
const char *get_history_file_name(int hist_type_idx);
PRIVATE int get_history_max_lines(int hist_type_idx);

PRIVATE void init_history(int hist_type_idx);
PRIVATE void append_history_buf(int hist_type_idx, const char *str);
PRIVATE void clear_history_buf(int hist_type_idx);

PRIVATE void clear_history_buf_modified(int hist_type_idx);
PRIVATE void set_history_modified(int hist_type_idx);
PRIVATE int is_history_modified(int hist_type_idx);
PRIVATE int is_history_file_newer(int hist_type_idx);
PRIVATE int is_history_buffer_modified_newer__expired(int hist_type_idx);

PRIVATE void set_history_oldest(int hist_type_idx);
PRIVATE const char *get_history_newer(int hist_type_idx);

PRIVATE int has_str_registered_in_the_last_line(int hist_type_idx, const char *str);

PRIVATE void remove_all_exact_match(int hist_type_idx, const char *str);
PRIVATE void remove_all_file_path_match(int hist_type_idx, const char *str);
PRIVATE be_line_t *search_history_partial_match(int hist_type_idx, const char *str);
PRIVATE int compare_file_path_str(const char *str, const char *file_path);

// search/replace(directory and execution) history support functions

void init_histories()
{
	// prepare ~/.be directory
	if (is_path_exist(get_app_dir()) == 0) {
		if (mkdir(get_app_dir(), S_IRUSR | S_IWUSR | S_IXUSR) < 0) {
			e_printf("Unable to create directory %s, %s",
			 get_app_dir(), strerror(errno));
		}
	} else {
		if (is_path_dir(get_app_dir()) == 0) {
			e_printf("No application directory %s",
			 get_app_dir());
		}
	}
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		// Create a buffer of LIST mode
		bufs_insert_buf_to_bottom(&history_buffers,
		 buf_create(add_temporal_file_path_prefix(get_history_file_path(hist_type_idx)),
		  BUF_MODE_LIST));
		init_history(hist_type_idx);
	}
}

//------------------------------------------------------------------------------
void sync_histories_if_necessary()
{
	save_histories_if_modified_newer__expired();
	int hist_type_idx = load_histories_if_file_newer();
	if (hist_type_idx >= 0) {
		disp_status_bar_async(_("%s %s buffer re-loaded"),
		 cur_hhcmmcss(), get_history_file_name(hist_type_idx));
	}
}

// save history buffers to file
void save_histories_if_modified()
{
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP; hist_type_idx++) {
		save_history_if_modified(hist_type_idx);
	}
}
// save history buffers to files
void save_histories_if_modified_newer__expired()
{
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP; hist_type_idx++) {
		save_history_if_modified_newer__expired(hist_type_idx);
	}
}

// load history files into buffers
int load_histories_if_file_newer()
{
	int hist_type_idx_loaded = -1;
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		if (load_history_if_file_newer(hist_type_idx) >= 0) {
			hist_type_idx_loaded = hist_type_idx;
		}
	}
	return hist_type_idx_loaded;
}

//------------------------------------------------------------------------------
PRIVATE be_buf_t *get_history_buf(int hist_type_idx)
{
	if ((0 <= hist_type_idx && hist_type_idx < HISTORY_TYPES_APP_AND_SHELL) == 0) {
		e_printf("hist_type_idx out of range: %d", hist_type_idx);
		hist_type_idx = 0;
	}
	return buf_get_buf_by_idx(HIST_BUFS_TOP_BUF, hist_type_idx);
}

void modify_save_history(int hist_type_idx, const char *str)
{
	modify_history_w_sync(hist_type_idx, str);
	save_history_if_modified(hist_type_idx);
}

//------------------------------------------------------------------------------
// update history list (load, modify, save)
void modify_history_w_sync(int hist_type_idx, const char *str)
{
	if (is_strlen_0(str)) {
		return;
	}
	if (has_str_registered_in_the_last_line(hist_type_idx, str)) {
		// 'str' is registered in the last line, no need update
		return;
	}
	// load-modify(free old entry and append new entry)-save
	load_history_if_file_newer(hist_type_idx);
	if ((hist_type_idx == HISTORY_TYPE_IDX_FILE) || (hist_type_idx == HISTORY_TYPE_IDX_DIR)) {
		remove_all_file_path_match(hist_type_idx, str);
	} else {
		remove_all_exact_match(hist_type_idx, str);
	}
	append_history_buf(hist_type_idx, str);
	// update history files soon so that other BE-editor instances can get recent information
	save_history_if_modified_newer__expired(hist_type_idx);
}

// last_n: 1, 2, 3, ....
const char *get_history_newest(int hist_type_idx, int last_n)
{
	const char *history = "";
	set_history_newest(hist_type_idx);
	for (int cnt = 0; cnt < last_n; cnt++) {
		history = get_history_older(hist_type_idx);
	}
	return history;
}

// get a completion
const char *get_history_completion(int hist_type_idx, const char *str)
{
	be_line_t *line;
	if ((line = search_history_partial_match(hist_type_idx, str)) != NULL) {
		return line->data;
	}
	return str;		// return original string
}

//------------------------------------------------------------------------------
PRIVATE int load_history_if_file_newer(int hist_type_idx)
{
	if (is_history_file_newer(hist_type_idx)) {
		load_history_idx(hist_type_idx);
		return hist_type_idx;
	}
	return -1;
}
PRIVATE void save_history_if_modified_newer__expired(int hist_type_idx)
{
	if (is_history_buffer_modified_newer__expired(hist_type_idx)) {
		save_history_idx(hist_type_idx);
	}
}
PRIVATE void save_history_if_modified(int hist_type_idx)
{
	if (is_history_modified(hist_type_idx)) {
		save_history_idx(hist_type_idx);
	}
}

//------------------------------------------------------------------------------
PRIVATE int save_history_idx(int hist_type_idx)
{
	const char *file_path = get_history_file_path(hist_type_idx);
hmtflf_d_printf("ZZZZSSSS[%s]\n", file_path);
	make_backup_files(file_path, get_backup_files());
	int error = 0;
	FILE *fp = fopen(file_path, "w");
	if (fp == NULL) {
		e_printf("Unable to create history file: %s, %s", file_path, strerror(errno));
		error = 1;
		goto save_history_2;
	}
	const char *str;
	int lines = 0;
	for (set_history_oldest(hist_type_idx); *(str = get_history_newer(hist_type_idx)); ) {
		// count lines
		lines++;
	}
	lines = LIM_MIN(0, lines - get_history_max_lines(hist_type_idx)); // lines to skip
	// write oldest first
	for (set_history_oldest(hist_type_idx); *(str = get_history_newer(hist_type_idx)); ) {
		if (lines-- > 0)
			continue;
		char buf[MAX_EDIT_LINE_LEN+1];
		snprintf_(buf, MAX_EDIT_LINE_LEN+1, "%s\n", str);
		if (fputs(buf, fp) == EOF) {
			e_printf("Unable to write history file: %s, %s", file_path, strerror(errno));
			error = 1;
			goto save_history_1;
		}
	}
save_history_1:;
	if (fclose(fp) != 0) {
		error = 1;
	}
	// set rw only to owner for security
	chmod(file_path, S_IRUSR | S_IWUSR);
	buf_get_file_stat(get_history_buf(hist_type_idx), file_path);
save_history_2:;
	clear_history_buf_modified(hist_type_idx);
	set_history_newest(hist_type_idx);
	return error;
}

PRIVATE int load_history_idx(int hist_type_idx)
{
	const char *file_path = get_history_file_path(hist_type_idx);
hmtflf_d_printf("ZZZZLLLL[%s]\n", file_path);
	int error = 0;
	FILE *fp = fopen(file_path, "r");
	if (fp == NULL) {
		e_printf("Unable to open history file: %s, %s",
		 file_path, strerror(errno));
		error = 1;
		goto load_history_1;
	}
	clear_history_buf(hist_type_idx);
	int lines;
	char str[MAX_EDIT_LINE_LEN+1];
	// count total lines
	for (lines = 0; fgets(str, MAX_EDIT_LINE_LEN, fp) != NULL; ) {
		// count lines
		lines++;
	}
	lines = LIM_MIN(0, lines - get_history_max_lines(hist_type_idx));
	fseek(fp, 0, SEEK_SET);
	// skip lines
	for ( ; fgets(str, MAX_EDIT_LINE_LEN, fp) != NULL; ) {
		if (lines-- > 0)
			continue;
		// load the last get_history_max_lines(hist_type_idx) lines
		str[MAX_EDIT_LINE_LEN] = '\0';
		append_history_buf(hist_type_idx, remove_line_tail_lf(str));
	}
	if (fclose(fp) != 0) {
		error = 1;
	}
	buf_get_file_stat(get_history_buf(hist_type_idx), file_path);
load_history_1:;
	clear_history_buf_modified(hist_type_idx);
	return error;
}
//------------------------------------------------------------------------------
const char *get_history_file_path(int hist_type_idx)
{
	static char file_path[MAX_PATH_LEN+1];
	const char *dir = get_app_dir();
	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_DIR:
	case HISTORY_TYPE_IDX_FILE:
	case HISTORY_TYPE_IDX_SEARCH:
	case HISTORY_TYPE_IDX_EXEC:
	case HISTORY_TYPE_IDX_KEYMACRO:
		break;
	case HISTORY_TYPE_IDX_SHELL:
		dir = get_home_dir();
		break;
	}
	cat_dir_and_file(file_path, dir, get_history_file_name(hist_type_idx));
	return file_path;
}
const char *get_history_file_name(int hist_type_idx)
{
	const char *file;
	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_DIR:
		file = DIR_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_FILE:
		file = FILE_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_SEARCH:
		file = SEARCH_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_EXEC:
		file = EXEC_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_KEYMACRO:
		file = KEYMACRO_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_SHELL:
		file = BASH_HISTORY_FILE_NAME;
		break;
	}
	return file;
}
PRIVATE int get_history_max_lines(int hist_type_idx)
{
	int lines = 0;
	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_DIR:
		lines = MAX_HISTORY_LINES;
		break;
	case HISTORY_TYPE_IDX_FILE:
		lines = MAX_HISTORY_LINES_10K;
		break;
	case HISTORY_TYPE_IDX_SEARCH:
		lines = MAX_HISTORY_LINES;
		break;
	case HISTORY_TYPE_IDX_EXEC:
		lines = MAX_HISTORY_LINES_10K;
		break;
	case HISTORY_TYPE_IDX_KEYMACRO:
		lines = MAX_HISTORY_LINES;
		break;
	case HISTORY_TYPE_IDX_SHELL:
		lines = MAX_HISTORY_LINES;
		break;
	}
	return lines;
}

//------------------------------------------------------------------------------
// initialize search and replace history lists
PRIVATE void init_history(int hist_type_idx)
{
	set_history_newest(hist_type_idx);
	clear_history_buf_modified(hist_type_idx);
}
// add string to the bottom of history list
PRIVATE void append_history_buf(int hist_type_idx, const char *str)
{
	if (is_strlen_0(str)) {
		return;
	}
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, line_insert_with_string(NODES_BOT_ANCH(buf), INSERT_BEFORE, str));
	set_history_modified(hist_type_idx);
}
PRIVATE void clear_history_buf(int hist_type_idx)
{
	buf_free_lines(get_history_buf(hist_type_idx));
}
PRIVATE void clear_history_buf_modified(int hist_type_idx)
{
	buf_clear_modified(get_history_buf(hist_type_idx));
}
PRIVATE void set_history_modified(int hist_type_idx)
{
	buf_set_modified(get_history_buf(hist_type_idx));
}
PRIVATE int is_history_modified(int hist_type_idx)
{
	return buf_get_modified(get_history_buf(hist_type_idx));
}

PRIVATE int is_history_file_newer(int hist_type_idx)
{
	return buf_compare_mtime_to_cur_file(get_history_buf(hist_type_idx),
	 get_history_file_path(hist_type_idx)) < 0;
}
PRIVATE int is_history_buffer_modified_newer__expired(int hist_type_idx)
{
	return buf_is_modified_newer__expired(get_history_buf(hist_type_idx),
	 get_history_file_path(hist_type_idx), BUFFER_EXPIRATION_MSEC);
}

PRIVATE void set_history_oldest(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, NODES_TOP_NODE(buf));
}
PRIVATE const char *get_history_newer(int hist_type_idx)
{
	be_line_t *line;
	if ((line = buf_move_cur_line_to_next(get_history_buf(hist_type_idx))) != NULL) {
		if (line->data) {
			return line->data;
		}
	}
	return "";			// end of list
}
void set_history_newest(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, NODES_BOT_NODE(buf));
}
const char *get_history_older(int hist_type_idx)
{
	be_line_t *line = buf_move_cur_line_to_prev(get_history_buf(hist_type_idx));
	if (line != NULL) {
		if (line->data) {
			return line->data;
		}
	}
	return "";			// end of list
}

//------------------------------------------------------------------------------
PRIVATE int has_str_registered_in_the_last_line(int hist_type_idx, const char *str)
{
	return strcmp(history_last_line_str(hist_type_idx), str) == 0;	// exact match
}
const char* history_last_line_str(int hist_type_idx)
{
	be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx));
	if (IS_NODE_INT(line) == 0) {
		return "";
	}
	return line->data;
}

//------------------------------------------------------------------------------
// remove all line the same to str
PRIVATE void remove_all_exact_match(int hist_type_idx, const char *str)
{
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line); ) {
		be_line_t *prev = NODE_PREV(line);
		if (strcmp(line->data, str) == 0) {	// exact match
			line_unlink_free(line);		// delete older line
			set_history_modified(hist_type_idx);
		}
		line = prev;
	}
}
PRIVATE void remove_all_file_path_match(int hist_type_idx, const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	// "/dir/file.txt|100,40" ==> "/dir/file.txt"
	get_file_line_col_from_str(str, file_path, NULL, NULL);
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line); ) {
		be_line_t *prev = NODE_PREV(line);
		if (compare_file_path_str(line->data, file_path) == 0) {	// file path match
			line_unlink_free(line);		// delete older line
			set_history_modified(hist_type_idx);
		}
		line = prev;
	}
}

// find first line containing string str in history list
PRIVATE be_line_t *search_history_partial_match(int hist_type_idx, const char *str)
{
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line);
	 line = NODE_PREV(line)) {
		if (strlcmp__(line->data, str) == 0)	// partial match
			return line;
	}
	return NULL;
}
const char *search_history_file_path(int hist_type_idx, const char *file_path)
{
	// search from the newest to the oldest
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line);
	 line = NODE_PREV(line)) {
		if (compare_file_path_str(line->data, file_path) == 0) {
			return line->data;	// return matched string
		}
	}
	return file_path;			// return original string
}
PRIVATE int compare_file_path_str(const char *str, const char *file_path)
{
	//     "#/home/user/filename.exp|1234"
	// ==> "/home/user/filename.exp"
	//     "#'/home/user/ filename.exp '|1234"
	// ==> "'/home/user/ filename.exp '"
	str = remove_temporal_file_path_prefix(str);
	int len = strlen_path(str);
	const char *ptr = strstr(str, FILE_PATH_SEPARATOR);
	if (ptr != NULL) {
		len = ptr - str;
	}
	const char *quoted_file_path = quote_file_path_s(file_path);
	return strn_strn_cmp__(str, len, quoted_file_path, strlen_path(quoted_file_path));
}

//------------------------------------------------------------------------------
// Record only the directory in which some operation actually done in the directory.
// Do not record the directory in which nothing done and it has just visited the directory.
PRIVATE char dir_history_temporal[MAX_PATH_LEN+1] = "";
// current directory has changed
void dir_history_append_temporarily(const char *dir)
{
	// fix previously registered
	dir_history_remove();
	if ((has_str_registered_in_the_last_line(HISTORY_TYPE_IDX_DIR, dir) == 0)
	 && (has_str_registered_in_the_last_line(HISTORY_TYPE_IDX_DIR,
	  add_temporal_file_path_prefix(dir)) == 0)) {
		load_history_if_file_newer(HISTORY_TYPE_IDX_DIR);
		// temporarily register dir
		append_history_buf(HISTORY_TYPE_IDX_DIR, add_temporal_file_path_prefix(dir));
		save_history_if_modified_newer__expired(HISTORY_TYPE_IDX_DIR);
		strlcpy__(dir_history_temporal, dir, MAX_PATH_LEN);
	}
}
// no operation has done before leaving the directory which is memorized in `dir_history_temporal`
int dir_history_remove()
{
	if (dir_history_temporal[0]) {
		// nothing operated in the dir, remove temporal entry
		load_history_if_file_newer(HISTORY_TYPE_IDX_DIR);
		remove_all_file_path_match(HISTORY_TYPE_IDX_DIR, dir_history_temporal);
///		remove_all_exact_match(HISTORY_TYPE_IDX_DIR,
///		 add_temporal_file_path_prefix(dir_history_temporal));
		save_history_if_modified_newer__expired(HISTORY_TYPE_IDX_DIR);
		strlcpy__(dir_history_temporal, "", MAX_PATH_LEN);
		return 1;
	}
	return 0;
}
// some operation is about to do or has done in the directory
int dir_history_fix()
{
	if (dir_history_temporal[0]) {
		// some operation has done in the dir, remove temporal and append dir
///		load_history_if_file_newer(HISTORY_TYPE_IDX_DIR);
///		remove_all_exact_match(HISTORY_TYPE_IDX_DIR,
///		 add_temporal_file_path_prefix(dir_history_temporal));
		modify_history_w_sync(HISTORY_TYPE_IDX_DIR, dir_history_temporal);
///		save_history_if_modified_newer__expired(HISTORY_TYPE_IDX_DIR);
		strlcpy__(dir_history_temporal, "", MAX_PATH_LEN);
		return 1;	// temporal entry has resolved
	}
	return 0;	// nothing done
}

//------------------------------------------------------------------------------
int select_from_history_list(int hist_type_idx, char *buffer)
{
	load_histories_if_file_newer();
	bufs_renumber_all_bufs_from_top(&history_buffers);
	load_cut_buffers_if_file_newer();
	bufs_renumber_all_bufs_from_top(&cut_buffers);

	//----------------------------------------------------
	int ret = do_call_editor(1, APP_MODE_CHOOSER, get_history_buf(hist_type_idx), buffer);
	//----------------------------------------------------
	if (hist_type_idx == HISTORY_TYPE_IDX_DIR) {
		strlcpy__(buffer, remove_temporal_file_path_prefix(buffer), MAX_PATH_LEN);
	}
	return ret;
}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_history_idx(int hist_type_idx)
{
	buf_dump_lines(get_history_buf(hist_type_idx), INT_MAX);
}
void dump_hist_bufs()
{
	buf_dump_bufs(HIST_BUFS_TOP_BUF);
}
void dump_hist_bufs_lines()
{
	buf_dump_bufs_lines(HIST_BUFS_TOP_BUF, "hist-bufs");
}
#endif // ENABLE_DEBUG

#endif // ENABLE_HISTORY

// End of history.c
