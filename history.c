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

time_t abs_msec_history_modified[HISTORY_TYPES_APP_AND_SHELL];
time_t abs_msec_history_loaded[HISTORY_TYPES_APP_AND_SHELL];

PRIVATE int load_history_if_reloadable(int hist_type_idx);
PRIVATE int load_history_if_needed_and_reloadable(int hist_type_idx);
PRIVATE int save_history_if_modified_and_expired(int hist_type_idx);
PRIVATE int save_history_if_modified(int hist_type_idx);
PRIVATE be_buf_t *get_history_buf(int hist_type_idx);

PRIVATE int save_history_idx(int hist_type_idx);
PRIVATE int load_history_idx(int hist_type_idx);
PRIVATE char *get_history_file_path(int hist_type_idx);
PRIVATE int get_history_max_lines(int hist_type_idx);

PRIVATE void init_history(int hist_type_idx);
PRIVATE void append_history(int hist_type_idx, const char *str);
PRIVATE void clear_history(int hist_type_idx);

PRIVATE void clear_history_modified(int hist_type_idx);
PRIVATE void set_history_modified(int hist_type_idx);
PRIVATE int is_history_modified(int hist_type_idx);
PRIVATE int is_history_modified_and_expired(int hist_type_idx);
PRIVATE int is_history_reload_needed_and_not_modified(int hist_type_idx);

PRIVATE void set_history_oldest(int hist_type_idx);
PRIVATE const char *get_history_newer(int hist_type_idx);

PRIVATE const char *the_same_str_registered_in_the_last_line(int hist_type_idx, const char *str);

PRIVATE void remove_all_exact_match(int hist_type_idx, const char *str);
PRIVATE void remove_all_file_path_match(int hist_type_idx, const char *str);

PRIVATE int compare_file_path_str(const char *str, const char *file_path);

PRIVATE be_line_t *search_history_partial_match(int hist_type_idx, const char *str);

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
		 buf_create_node(get_history_file_path(hist_type_idx), buf_MODE_LIST));
		init_history(hist_type_idx);
	}
}

//------------------------------------------------------------------------------
// load history files into buffers
void load_histories(void)
{
	for (int hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		load_history_if_reloadable(hist_type_idx);
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

void load_last_searched_needle(void)
{
	// load last_searched_needle
	strlcpy__(last_searched_needle, get_history_newest(HISTORY_TYPE_IDX_SEARCH, 1), MAX_PATH_LEN);
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

// record "dir-history" on the only occasion when some operation actually done in the directory
// does not record to history on the occasion that it just visit the directory
char update_history_prev_dir[MAX_PATH_LEN+1] = "";
char update_history_next_dir[MAX_PATH_LEN+1] = "";
void update_history_dir_change(const char *prev_dir, const char *next_dir)
{
	strlcpy__(update_history_prev_dir, prev_dir, MAX_PATH_LEN);
	strlcpy__(update_history_next_dir, next_dir, MAX_PATH_LEN);
}
int update_history_dir_operate(void)
{
	if (update_history_prev_dir[0] || update_history_next_dir[0]) {
		update_history_dir_sync(update_history_prev_dir, update_history_next_dir);
		update_history_prev_dir[0] = '\0';
		update_history_next_dir[0] = '\0';
		return 1;
	}
	return 0;
}
void update_history_dir_sync(const char *prev_dir, const char *next_dir)
{
	// previous dir
	update_history(HISTORY_TYPE_IDX_DIR, prev_dir);
	// next dir
	update_history(HISTORY_TYPE_IDX_DIR, next_dir);
}
//--------------------------------------

// update history list (load, modify, save)
void update_history(int hist_type_idx, const char *str)
{
	if (is_strlen_0(str)) {
		return;
	}
#ifdef NOT_DELETE_OLD_IDENTICAL_ENTRY
	if ((hist_type_idx != HISTORY_TYPE_IDX_EXEC)
	 && the_same_str_registered_in_the_last_line(hist_type_idx, str) != NULL)
#else // NOT_DELETE_OLD_IDENTICAL_ENTRY
	if (the_same_str_registered_in_the_last_line(hist_type_idx, str) != NULL)
#endif // NOT_DELETE_OLD_IDENTICAL_ENTRY
	{
		// 'str' is registered in the last line, no need update
		return;
	}
	// load-modify(free old entry and append new entry)-save
	load_history_if_needed_and_reloadable(hist_type_idx);
#ifdef NOT_DELETE_OLD_IDENTICAL_ENTRY
	if (hist_type_idx == HISTORY_TYPE_IDX_EXEC) {
		// not delete old entry
	} else
#endif // NOT_DELETE_OLD_IDENTICAL_ENTRY
	if (hist_type_idx == HISTORY_TYPE_IDX_FILE) {
		remove_all_file_path_match(hist_type_idx, str);
	} else {
		remove_all_exact_match(hist_type_idx, str);
	}
	append_history(hist_type_idx, str);
	// update history files soon so that other BE-editor instances can get recent information
	save_history_if_modified_and_expired(hist_type_idx);
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
//|        | write  | reload |       |              |                |
//|modified| timer  | timer  |save   |load          |comment         |
//|        | expired| expired|       |              |                |
//|--------|--------|--------|-------|--------------|----------------|
//|   0    |   0    |   0    |   -   |   -          |--              |
//|   0    |   0    |   1    |   -   |needed        |reload needed   |
//|   0    |   1    |   1    |   -   |   -          |N/A             |
//|   1    |   0    |   1    |pending|not reloadable|write           |
//|   1    |   1    |   1    |needed |needed        |write needed    |
//|   0    |   1    |   0    |   -   |   -          |N/A             |
//|   1    |   0    |   0    |pending|not reloadable|write is pending|
//|   1    |   1    |   0    |needed |not reloadable|write needed    |

PRIVATE int load_history_if_reloadable(int hist_type_idx)
{
	if (is_history_modified(hist_type_idx)) {
		return -1;
	}
	return load_history_idx(hist_type_idx);
}
PRIVATE int load_history_if_needed_and_reloadable(int hist_type_idx)
{
	if (is_history_reload_needed_and_not_modified(hist_type_idx) == 0) {
		return -1;
	}
	return load_history_idx(hist_type_idx);
}
PRIVATE int save_history_if_modified_and_expired(int hist_type_idx)
{
	if ((hist_type_idx == HISTORY_TYPE_IDX_FILE)
	 && (is_history_modified_and_expired(hist_type_idx) == 0)) {
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

	if (GET_APPMD(app_HISTORY) == 0)
		return 0;
	const char *file_path = get_history_file_path(hist_type_idx);
	FILE *fp;
	if ((fp = fopen(file_path, "w")) == NULL) {
		e_printf("Unable to create history file: %s, %s",
		 file_path, strerror(errno));
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
			e_printf("Unable to write history file: %s, %s",
			 file_path, strerror(errno));
			error = 1;
			goto save_history_1;
		}
	}
save_history_1:;
	set_history_newest(hist_type_idx);
	if (fclose(fp) != 0) {
		error = 1;
	}
	// set rw only by owner for security
	chmod(file_path, S_IRUSR | S_IWUSR);
save_history_2:;
	clear_history_modified(hist_type_idx);
	return error;
}

PRIVATE int load_history_idx(int hist_type_idx)
{
	int error = 0;

	if (GET_APPMD(app_HISTORY) == 0)
		return 0;
	const char *file_path = get_history_file_path(hist_type_idx);
	clear_history(hist_type_idx);

	FILE *fp;
	if ((fp = fopen(file_path, "r")) == NULL) {
		if (errno != ENOENT) {
			e_printf("Unable to open history file: %s, %s",
			 file_path, strerror(errno));
		}
	} else {
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
			// load the last MAX_HISTORY_LINES lines
			str[MAX_EDIT_LINE_LEN] = '\0';
			append_history(hist_type_idx, remove_line_tail_lf(str));
		}
		if (fclose(fp) != 0) {
			error = 1;
		}
	}
	clear_history_modified(hist_type_idx);
	return error;
}
PRIVATE char *get_history_file_path(int hist_type_idx)
{
	static char file_path[MAX_PATH_LEN+1];
	const char *file;
	const char *dir = get_app_dir();

	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_KEYMACRO:
		file = KEYMACRO_HISTORY_FILE_NAME;
		break;
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
	case HISTORY_TYPE_IDX_KEYMACRO:
		lines = MAX_HISTORY_LINES;
		break;
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
	clear_history_modified(hist_type_idx);
}
// add string to the bottom of history list
PRIVATE void append_history(int hist_type_idx, const char *str)
{
	if (is_strlen_0(str)) {
		return;
	}
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, line_insert_with_string(NODES_BOT_ANCH(buf), INSERT_BEFORE, str));
	set_history_modified(hist_type_idx);
}
PRIVATE void clear_history(int hist_type_idx)
{
	buf_free_lines(get_history_buf(hist_type_idx));
}
PRIVATE void clear_history_modified(int hist_type_idx)
{
	BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED) = 0;
	abs_msec_history_modified[hist_type_idx] = get_msec();
	abs_msec_history_loaded[hist_type_idx] = get_msec();
}
PRIVATE void set_history_modified(int hist_type_idx)
{
	if (BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED) == 0) {
		BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED) = 1;
		abs_msec_history_modified[hist_type_idx] = get_msec();
	}
}
PRIVATE int is_history_modified(int hist_type_idx)
{
	return BUF_STATE(get_history_buf(hist_type_idx), buf_MODIFIED);
}
#define HISTORY_EXPIRATION_MSEC		100		// 1000
PRIVATE int is_history_modified_and_expired(int hist_type_idx)
{
	return is_history_modified(hist_type_idx)
	 && ((get_msec() - abs_msec_history_modified[hist_type_idx]) >= HISTORY_EXPIRATION_MSEC);
}
PRIVATE int is_history_reload_needed_and_not_modified(int hist_type_idx)
{
	return ((get_msec() - abs_msec_history_loaded[hist_type_idx]) >= HISTORY_EXPIRATION_MSEC)
	 && (is_history_modified(hist_type_idx) == 0);
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

PRIVATE const char *the_same_str_registered_in_the_last_line(int hist_type_idx, const char *str)
{
	be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx));
	if (IS_NODE_INT(line) && (strcmp(line->data, str) == 0))	// exact match
		return line->data;
	return NULL;
}

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
	const char *path = quote_file_path_static(file_path);
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line); ) {
		be_line_t *prev = NODE_PREV(line);
		if (compare_file_path_str(line->data, path) == 0) {	// file path match
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
const char *search_history_file_path(int hist_type_idx, const char *path)
{
	path = quote_file_path_static(path);
	// search from the newest to the oldest
	for (be_line_t *line = NODES_BOT_NODE(get_history_buf(hist_type_idx)); IS_NODE_INT(line);
	 line = NODE_PREV(line)) {
		// /home/user/filename.exp|1234
		// '/home/user/ filename.exp '|1234
		const char *ptr;
		size_t len;
		if ((ptr = strstr(line->data, FILE_PATH_SEPARATOR)) != NULL) {
			len = ptr - line->data;
		} else {
			len = line_strlen(line);
		}
		if (strncmp(line->data, path, len) == 0) {
			return line->data;
		}
	}
	return path;		// return original string
}
PRIVATE int compare_file_path_str(const char *str, const char *file_path)
{
	// /home/user/filename.exp|1234
	// '/home/user/ filename.exp '
	const char *ptr;
	int len;
	if ((ptr = strstr(str, FILE_PATH_SEPARATOR)) != NULL) {
		len = ptr - str;
	} else {
		len = strlen_path(str);
	}
	return strncmp(str, file_path, len);
}

//------------------------------------------------------------------------------
int select_from_history_list(int hist_type_idx, char *buffer)
{
	load_histories();

	int ret = do_call_editor(1, APP_MODE_CHOOSER, get_history_buf(hist_type_idx),
	 buffer, MAX_PATH_LEN);

flf_d_printf("ret: %d, buffer: [%s]\n", ret, buffer);
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
