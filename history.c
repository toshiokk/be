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

PRIVATE int load_history_if_needed(int hist_type_idx);
PRIVATE int save_history_if_modified_and_expired(int hist_type_idx);
PRIVATE int save_history_if_modified(int hist_type_idx);
PRIVATE be_buf_t *get_history_buf(int hist_type_idx);

PRIVATE int save_history_idx(int hist_type_idx);
PRIVATE int load_history_idx(int hist_type_idx);
PRIVATE const char *get_history_file_path(int hist_type_idx);
PRIVATE int get_history_max_lines(int hist_type_idx);

PRIVATE void init_history(int hist_type_idx);
PRIVATE void append_history_buf(int hist_type_idx, const char *str);
PRIVATE void clear_history_buf(int hist_type_idx);

PRIVATE void clear_history_buf_modified(int hist_type_idx);
PRIVATE void set_history_modified(int hist_type_idx);
PRIVATE int is_history_modified(int hist_type_idx);
PRIVATE int is_history_modified_and_expired(int hist_type_idx);

PRIVATE void set_history_oldest(int hist_type_idx);
PRIVATE const char *get_history_newer(int hist_type_idx);

PRIVATE int has_str_registered_in_the_last_line(int hist_type_idx, const char *str);

PRIVATE void remove_all_exact_match(int hist_type_idx, const char *str);
PRIVATE int compare_file_path_str(const char *str, const char *file_path);
PRIVATE void remove_all_file_path_match(int hist_type_idx, const char *str);

PRIVATE be_line_t *search_history_partial_match(int hist_type_idx, const char *str);


PRIVATE const char* dir_history_add_temporal_mark(const char* dir);
PRIVATE const char* dir_history_remove_temporal_mark(const char* str);

// search/replace(directory and execution) history support functions

void init_histories(void)
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
		 buf_create_node(dir_history_add_temporal_mark(get_history_file_path(hist_type_idx)),
		  buf_MODE_LIST));
		init_history(hist_type_idx);
	}
}

//------------------------------------------------------------------------------
// load history files into buffers
void load_histories(void)
{
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		load_history_if_needed(hist_type_idx);
	}
	bufs_renumber_all_bufs_from_top(&history_buffers);
}

// save histories to file
void save_histories(void)
{
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP; hist_type_idx++) {
		save_history_if_modified(hist_type_idx);
	}
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

//------------------------------------------------------------------------------
// update history list (load, modify, save)
void update_history(int hist_type_idx, const char *str)
{
	if (is_strlen_0(str)) {
		return;
	}
	if (has_str_registered_in_the_last_line(hist_type_idx, str)) {
		// 'str' is registered in the last line, no need update
		return;
	}
	// load-modify(free old entry and append new entry)-save
	load_history_if_needed(hist_type_idx);
	if ((hist_type_idx == HISTORY_TYPE_IDX_FILE) || (hist_type_idx == HISTORY_TYPE_IDX_DIR)) {
		remove_all_file_path_match(hist_type_idx, str);
	} else {
		remove_all_exact_match(hist_type_idx, str);
	}
	append_history_buf(hist_type_idx, str);
	// update history files soon so that other BE-editor instances can get recent information
	save_history_if_modified_and_expired(hist_type_idx);
}

void append_history(int hist_type_idx, const char *str)
{
	load_history_if_needed(hist_type_idx);
	append_history_buf(hist_type_idx, str);
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

//|--history state----------|--action--------------|--comment--------|
//|updated |modified|write  |       |              |                 |
//|by other|myself  |timer  |save   |load          |comment          |
//|        |        |expired|       |              |                 |
//|--------|--------|-------|-------|--------------|-----------------|
//|   0    |   0    |   0   |--     |--            |--               |
//|   0    |   0    |   1   |--     |--            |--               |
//|   0    |   1    |   0   |pending|not reloadable|write is pending |
//|   0    |   1    |   1   |needed |not reloadable|write needed     |
//|   1    |   0    |   0   |--     |needed        |--               |
//|   1    |   0    |   1   |--     |--            |--               |
//|   1    |   1    |   0   |pending|not reloadable|possible conflict|
//|   1    |   1    |   1   |needed |not reloadable|possible conflict|
// updated : file has updated by other
// modified: local copy(buffer) has modified by myself
//
// before modification of buffer:
// - if file updated: reload
// before referencing buffer:
// - if file updated: reload

PRIVATE int load_history_if_needed(int hist_type_idx)
{
	if (buf_has_orig_file_updated(get_history_buf(hist_type_idx),
	 get_history_file_path(hist_type_idx)) <= 0) {
		return -1;
	}
	return load_history_idx(hist_type_idx);
}
PRIVATE int save_history_if_modified_and_expired(int hist_type_idx)
{
	if (is_history_modified_and_expired(hist_type_idx) == 0) {
		return -1;
	}
	return save_history_idx(hist_type_idx);
}
PRIVATE int save_history_if_modified(int hist_type_idx)
{
	if (is_history_modified(hist_type_idx) == 0) {
		return -1;
	}
	return save_history_idx(hist_type_idx);
}

PRIVATE int save_history_idx(int hist_type_idx)
{
	int error = 0;
	const char *file_path = get_history_file_path(hist_type_idx);
mflf_d_printf("[%s]\n", get_history_file_path(hist_type_idx));
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
	int error = 0;
	clear_history_buf(hist_type_idx);

	const char *file_path = get_history_file_path(hist_type_idx);
mflf_d_printf("[%s]\n", get_history_file_path(hist_type_idx));
	FILE *fp = fopen(file_path, "r");
	if (fp == NULL) {
		e_printf("Unable to open history file: %s, %s",
		 file_path, strerror(errno));
		goto load_history_1;
	}
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
PRIVATE const char *get_history_file_path(int hist_type_idx)
{
	static char file_path[MAX_PATH_LEN+1];
	const char *file;
	const char *dir = get_app_dir();

	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_FILE:
		file = OPENFILE_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_DIR:
		file = DIR_HISTORY_FILE_NAME;
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
		dir = get_home_dir();
		file = BASH_HISTORY_FILE_NAME;
		break;
	}
	snprintf_(file_path, MAX_PATH_LEN+1, "%s/%s", dir, file);
	return file_path;
}
PRIVATE int get_history_max_lines(int hist_type_idx)
{
	int lines = 0;
	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_FILE:
		lines = MAX_HISTORY_LINES_10K;
		break;
	case HISTORY_TYPE_IDX_DIR:
		lines = MAX_HISTORY_LINES;
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
	BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED) = 0;
	buf_set_save_pending_timer(get_history_buf(hist_type_idx), get_msec());
}
PRIVATE void set_history_modified(int hist_type_idx)
{
	if (BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED) == 0) {
		BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED) = 1;
		buf_set_save_pending_timer(get_history_buf(hist_type_idx), get_msec());
	}
}
PRIVATE int is_history_modified(int hist_type_idx)
{
	return BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED);
}

#define HISTORY_EXPIRATION_MSEC		1000		// 1000
PRIVATE int is_history_modified_and_expired(int hist_type_idx)
{
	return is_history_modified(hist_type_idx)
	 && ((((UINT16)get_msec()) - buf_get_save_pending_timer(get_history_buf(hist_type_idx)))
	  >= HISTORY_EXPIRATION_MSEC);
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
flf_d_printf("[%s]\n", str);
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line); ) {
		be_line_t *prev = NODE_PREV(line);
		if (strcmp(line->data, str) == 0) {	// exact match
flf_d_printf("delete line [%s]\n", line->data);
			line_unlink_free(line);		// delete older line
			set_history_modified(hist_type_idx);
		}
		line = prev;
	}
}
PRIVATE void remove_all_file_path_match(int hist_type_idx, const char *str)
{
flf_d_printf("[%s]\n", str);
	char file_path[MAX_PATH_LEN+1];
	// "/dir/file.txt|100,40" ==> "/dir/file.txt"
	get_file_line_col_from_str(str, file_path, NULL, NULL);
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line); ) {
		be_line_t *prev = NODE_PREV(line);
		if (compare_file_path_str(line->data, file_path) == 0) {	// file path match
flf_d_printf("delete line [%s]\n", line->data);
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
	str = dir_history_remove_temporal_mark(str);
	const char *ptr;
	int len;
	if ((ptr = strstr(str, FILE_PATH_SEPARATOR)) != NULL) {
		len = ptr - str;
	} else {
		len = strlen_path(str);
	}
	const char *quoted_file_path = quote_file_path_static(file_path);
	return strn_strn_cmp__(str, len, quoted_file_path, strlen_path(quoted_file_path));
}

//------------------------------------------------------------------------------
// record "dir-history" on the only occasion when some operation actually done in the directory.
// does not record to history on the case that it just visit the directory.
char dir_history_temporal[MAX_PATH_LEN+1] = "";
// current directory has changed
void dir_history_append_temporarily(const char *dir)
{
	// fix previously registered
	dir_history_remove();
	load_history_if_needed(HISTORY_TYPE_IDX_DIR);
	if ((has_str_registered_in_the_last_line(HISTORY_TYPE_IDX_DIR, dir) == 0)
	 && (has_str_registered_in_the_last_line(HISTORY_TYPE_IDX_DIR,
	  dir_history_add_temporal_mark(dir)) == 0)) {
		// temporarily register dir
		append_history_buf(HISTORY_TYPE_IDX_DIR, dir_history_add_temporal_mark(dir));
		save_history_if_modified(HISTORY_TYPE_IDX_DIR);
		strlcpy__(dir_history_temporal, dir, MAX_PATH_LEN);
	}
}
// no operation has done before leaving the directory which is memorized in `dir_history_temporal`
int dir_history_remove()
{
	if (dir_history_temporal[0]) {
		// nothing operated in the dir, remove temporal entry
		load_history_if_needed(HISTORY_TYPE_IDX_DIR);
		remove_all_exact_match(HISTORY_TYPE_IDX_DIR,
		 dir_history_add_temporal_mark(dir_history_temporal));
		save_history_if_modified(HISTORY_TYPE_IDX_DIR);

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
		load_history_if_needed(HISTORY_TYPE_IDX_DIR);
		remove_all_exact_match(HISTORY_TYPE_IDX_DIR,
		 dir_history_add_temporal_mark(dir_history_temporal));
		update_history(HISTORY_TYPE_IDX_DIR, dir_history_temporal);
		save_history_if_modified(HISTORY_TYPE_IDX_DIR);

		strlcpy__(dir_history_temporal, "", MAX_PATH_LEN);
		return 1;	// temporal entry has resolved
	}
	return 0;	// nothing done
}

//     "/path/to/file"
// ==> "#/path/to/file"
PRIVATE const char* dir_history_add_temporal_mark(const char* dir)
{
	static char temporal_dir_path_s[MAX_PATH_LEN+1];
	strlcpy__(temporal_dir_path_s, dir, MAX_PATH_LEN);
	str_prepend(temporal_dir_path_s, MAX_PATH_LEN, TEMPORAL_HISTORY_MARK);
	return temporal_dir_path_s;
}
//     "#/path/to/file"
// ==> "/path/to/file"
PRIVATE const char* dir_history_remove_temporal_mark(const char* str)
{
	if (is_temporal_file_path(str)) {
		str = &str[strlen_path(TEMPORAL_HISTORY_MARK)];	// skip "#"
	}
	return str;
}

//------------------------------------------------------------------------------
int select_from_history_list(int hist_type_idx, char *buffer)
{
	load_histories();
	//----------------------------------------------------
	int ret = do_call_editor(1, APP_MODE_CHOOSER, get_history_buf(hist_type_idx),
	 buffer, MAX_PATH_LEN);
	//----------------------------------------------------
	if (hist_type_idx == HISTORY_TYPE_IDX_DIR) {
		strlcpy__(buffer, dir_history_remove_temporal_mark(buffer), MAX_PATH_LEN);
	}
	return ret;
}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_history_ix(int hist_type_idx)
{
	buf_dump_lines(get_history_buf(hist_type_idx), INT_MAX);
}
void dump_hist_bufs(void)
{
	buf_dump_bufs(HIST_BUFS_TOP_BUF);
}
void dump_hist_bufs_lines(void)
{
	buf_dump_bufs_lines(HIST_BUFS_TOP_BUF, "hist-bufs");
}
#endif // ENABLE_DEBUG

#endif // ENABLE_HISTORY

// End of history.c
