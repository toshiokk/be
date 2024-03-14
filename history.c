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

PRIVATE int save_history_if_modified(int hist_type_idx);
PRIVATE be_buf_t *get_history_buf(int hist_type_idx);
PRIVATE int check_file_pos_recorded_in_history(int hist_type_idx, const char *str);

PRIVATE int save_history_idx(int hist_type_idx);
PRIVATE int load_history_idx(int hist_type_idx);
PRIVATE char *get_history_file_path(int hist_type_idx);

PRIVATE void init_history(int hist_type_idx);
PRIVATE void append_history(int hist_type_idx, const char *str);
PRIVATE void clear_history(int hist_type_idx);

PRIVATE void clear_history_modified(int hist_type_idx);
PRIVATE void set_history_modified(int hist_type_idx);
PRIVATE int is_history_modified(int hist_type_idx);

PRIVATE void set_history_oldest(int hist_type_idx);
PRIVATE void set_history_newest(int hist_type_idx);
PRIVATE const char *get_history_older(int hist_type_idx);
PRIVATE const char *get_history_newer(int hist_type_idx);

PRIVATE const char *is_the_last_line(int hist_type_idx, const char *str);
PRIVATE be_line_t *search_history_exact_match(int hist_type_idx, const char *str);
PRIVATE int search_history_from_newest(int hist_type_idx, const char *str);
PRIVATE be_line_t *search_history_partial_match(int hist_type_idx, const char *str);

// search/replace(directory and execution) history support functions

void init_hist_bufs(void)
{
	// history_buffers
	init_bufs_top_bot_anchor(
	 HIST_BUFS_TOP_ANCH, "#History-bufs-top-anchor",
	 HIST_BUFS_BOT_ANCH, "#History-bufs-bot-anchor");
}

void init_histories(void)
{
	int hist_type_idx;
	char buf_name[MAX_PATH_LEN+1];

	// prepare ~/.be directory
	if (is_path_exist(get_app_dir()) == 0) {
		if (mkdir(get_app_dir(), S_IRUSR | S_IWUSR | S_IXUSR) < 0) {
			e_printf(_("Unable to create directory %s, %s"),
			 get_app_dir(), strerror(errno));
		}
	} else {
		if (is_path_dir(get_app_dir()) == 0) {
			e_printf(_("No application directory %s"),
			 get_app_dir());
		}
	}
	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		snprintf_(buf_name, MAX_PATH_LEN, "%s",
		 get_history_file_path(hist_type_idx));
		buf_insert_before(HIST_BUFS_BOT_ANCH, buf_create_node(buf_name));
		init_history(hist_type_idx);
	}
}

//-----------------------------------------------------------------------------
// load history files into buffers
void load_histories(void)
{
	int hist_type_idx;

///	_mlc_memorize_count
///	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
///		load_history_idx(hist_type_idx);
///	}
///	_mlc_check_count
///	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
///		clear_history(hist_type_idx);
///	}
///	_mlc_differ_count
	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP_AND_SHELL; hist_type_idx++) {
		load_history_idx(hist_type_idx);
	}
}

// save histories to file
void save_histories(void)
{
	int hist_type_idx;

	for (hist_type_idx = 0; hist_type_idx < HISTORY_TYPES_APP; hist_type_idx++) {
		save_history_if_modified(hist_type_idx);
	}
}

void load_last_searched_needle(void)
{
	// load last_searched_needle
	strlcpy__(last_searched_needle, get_history_newest(HISTORY_TYPE_IDX_SEARCH, 1), MAX_PATH_LEN);
///flf_d_printf("last_searched_needle[%s]\n", last_searched_needle);
}

//-----------------------------------------------------------------------------
PRIVATE be_buf_t *get_history_buf(int hist_type_idx)
{
	be_buf_t *buf;
	int idx;

	if ((0 <= hist_type_idx && hist_type_idx < HISTORY_TYPES_APP_AND_SHELL) == 0) {
		e_printf(_("hist_type_idx out of range: %d"), hist_type_idx);
		hist_type_idx = 0;
	}
	return get_buf_from_bufs_by_idx(HIST_BUFS_TOP_BUF, hist_type_idx);
}

void update_dir_history(const char *prev_dir, const char *cur_dir)
{
	// previous dir
	update_history(HISTORY_TYPE_IDX_DIR, prev_dir, 0);
	// next dir
	update_history(HISTORY_TYPE_IDX_DIR, cur_dir, 0);
}
// update history list (load, modify, save)
void update_history(int hist_type_idx, const char *str, BOOL force_update)
{
	be_line_t *line;

	if ((force_update == FALSE) && (hist_type_idx == HISTORY_TYPE_IDX_CURSPOS)) {
		if (check_file_pos_recorded_in_history(hist_type_idx, str)) {
			return; // registered relatively newer, no need of update
		}
	} else {
		if (is_the_last_line(hist_type_idx, str) != NULL) {
			// str is registered in the last line, no need update
			return;
		}
	}
///
flf_d_printf("hist_type_idx:%d:[%s]\n", hist_type_idx, str);
	// load-modify(free old entry and append new entry)-save
	load_history_idx(hist_type_idx);
	if ((line = search_history_exact_match(hist_type_idx, str)) != NULL) {
		line_unlink_free(line);	// delete older line
	}
	append_history(hist_type_idx, str);
	save_history_idx(hist_type_idx);
}

PRIVATE int check_file_pos_recorded_in_history(int hist_type_idx, const char *str)
{
	char file_path[MAX_PATH_LEN+1];
	// "/dir/file.txt|100,40" ==> "/dir/file.txt"
	get_file_line_col_from_str_null(str, file_path, NULL, NULL);
	const char *line = search_history_file_path(hist_type_idx, file_path);
	// check if exact line registered
	if (strcmp(line, str) == 0) {
		int lines = search_history_from_newest(hist_type_idx, line);
		if (0 < lines && lines <= MAX_HISTORY_LINES / 2) {
			return 1; // registered relatively newer, no need of update
		}
	}
	return 0;
}

// last_n: 1, 2, 3, ....
const char *get_history_newest(int hist_type_idx, int last_n)
{
	int cnt;
	const char *history = "";

	set_history_newest(hist_type_idx);
	for (cnt = 0; cnt < last_n; cnt++) {
		history = get_history_older(hist_type_idx);
	}
///flf_d_printf("[%s]\n", history);
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

//-----------------------------------------------------------------------------
PRIVATE int save_history_if_modified(int hist_type_idx)
{
	if (is_history_modified(hist_type_idx) == 0)
		return -1;
	return save_history_idx(hist_type_idx);
}

PRIVATE int save_history_idx(int hist_type_idx)
{
	const char *file_path;
	FILE *fp;
	int lines;
	const char *str;
	char buf[MAX_EDIT_LINE_LEN+1];
	int error = 0;

	if (GET_APPMD(app_HISTORY) == 0)
		return 0;
	file_path = get_history_file_path(hist_type_idx);
flf_d_printf("hist_type_idx:%d[%s]\n", hist_type_idx, file_path);
	if ((fp = fopen(file_path, "w")) == NULL) {
		e_printf(_("Unable to create history file: %s, %s"),
		 file_path, strerror(errno));
		error = 1;
		goto save_history_2;
	}
	lines = 0;
	for (set_history_oldest(hist_type_idx); *(str = get_history_newer(hist_type_idx)); ) {
		// count lines
		lines++;
	}
	lines = LIM_MIN(0, lines - MAX_HISTORY_LINES); // lines to skip
	// write oldest first
	for (set_history_oldest(hist_type_idx); *(str = get_history_newer(hist_type_idx)); ) {
		if (lines-- > 0)
			continue;
		snprintf_(buf, MAX_EDIT_LINE_LEN+1, "%s\n", str);
		if (fputs(buf, fp) == EOF) {
			e_printf(_("Unable to write history file: %s, %s"),
			 file_path, strerror(errno));
			error = 1;
			goto save_history_1;
		}
	}
save_history_1:;
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
	const char *file_path;
	FILE *fp;
	int lines;
	char str[MAX_EDIT_LINE_LEN+1];
	int error = 0;

	if (GET_APPMD(app_HISTORY) == 0)
		return 0;
	file_path = get_history_file_path(hist_type_idx);
flf_d_printf("hist_type_idx:%d[%s]\n", hist_type_idx, file_path);
	clear_history(hist_type_idx);

	if ((fp = fopen(file_path, "r")) == NULL) {
		if (errno != ENOENT) {
			e_printf(_("Unable to open history file: %s, %s"),
			 file_path, strerror(errno));
		}
	} else {
		// count total lines
		for (lines = 0; fgets(str, MAX_EDIT_LINE_LEN, fp) != NULL; ) {
			// count lines
			lines++;
		}
		lines = LIM_MIN(0, lines - MAX_HISTORY_LINES);
		fseek(fp, 0, SEEK_SET);
		// skip lines
		for ( ; fgets(str, MAX_EDIT_LINE_LEN, fp) != NULL; ) {
			if (lines-- > 0)
				continue;
			// load the last MAX_HISTORY_LINES lines
			str[MAX_EDIT_LINE_LEN] = '\0';
			remove_line_tail_lf(str);
			if (strlen(str) > 0) {
				append_history(hist_type_idx, str);
			}
		}
		if (fclose(fp) != 0) {
			error = 1;
		}
	}
	clear_history_modified(hist_type_idx);
///_D_(dump_history_ix(hist_type_idx))
	return error;
}
PRIVATE char *get_history_file_path(int hist_type_idx)
{
	const char *dir;
	const char *file;
	static char file_path[MAX_PATH_LEN+1];

	dir = get_app_dir();
	switch (hist_type_idx) {
	default:
	case HISTORY_TYPE_IDX_KEYMACRO:
		file = KEYMACRO_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_SEARCH:
		file = SEARCH_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_CURSPOS:
		file = OPENFILE_HISTORY_FILE_NAME;
		break;
	case HISTORY_TYPE_IDX_DIR:
		file = DIR_HISTORY_FILE_NAME;
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

//-----------------------------------------------------------------------------
// initialize search and replace history lists
PRIVATE void init_history(int hist_type_idx)
{
	set_history_newest(hist_type_idx);
	clear_history_modified(hist_type_idx);
}
// add string to the bottom of history list
PRIVATE void append_history(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
///flf_d_printf("[%s]\n", str);
	buf_set_cur_line(buf, line_insert_with_string(BUF_BOT_ANCH(buf), INSERT_BEFORE, str));
}
PRIVATE void clear_history(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_free_lines(buf);
}
PRIVATE void clear_history_modified(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	BUF_STATE(buf, buf_MODIFIED) = 0;
}
PRIVATE void set_history_modified(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	BUF_STATE(buf, buf_MODIFIED) = 1;
}
PRIVATE int is_history_modified(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	return BUF_STATE(buf, buf_MODIFIED);
}

PRIVATE void set_history_oldest(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, BUF_TOP_LINE(buf));
///_D_(buf_dump_lines(buf, INT_MAX))
}
PRIVATE void set_history_newest(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	buf_set_cur_line(buf, BUF_BOT_LINE(buf));
///_D_(buf_dump_lines(buf, INT_MAX))
}
PRIVATE const char *get_history_older(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	if ((line = buf_move_cur_line_to_prev(buf)) != NULL) {
		if (line->data) {
			return line->data;
		}
	}
	return "";			// end of list
}
PRIVATE const char *get_history_newer(int hist_type_idx)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	if ((line = buf_move_cur_line_to_next(buf)) != NULL) {
		if (line->data) {
			return line->data;
		}
	}
	return "";			// end of list
}

PRIVATE const char *is_the_last_line(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	line = BUF_BOT_LINE(buf);
	if (IS_NODE_INT(line) && (strcmp(line->data, str) == 0))	// exact match
		return line->data;
	return NULL;
}
// find first line containing string str in history list
PRIVATE be_line_t *search_history_exact_match(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	for (line = BUF_BOT_LINE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		if (strcmp(line->data, str) == 0)	// exact match
			return line;	// return line
	}
	return NULL;
}
PRIVATE int search_history_from_newest(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;
	int lines;

	lines = 0;
	for (line = BUF_BOT_LINE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		lines++;
		if (strcmp(line->data, str) == 0)	// exact match
			return lines;	// return line count from the newest
	}
	return 0; // not found
}
PRIVATE be_line_t *search_history_partial_match(int hist_type_idx, const char *str)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;

	for (line = BUF_BOT_LINE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		if (strlcmp__(line->data, str) == 0)	// partial match
			return line;
	}
	return NULL;
}
const char *search_history_file_path(int hist_type_idx, const char *path)
{
	be_buf_t *buf = get_history_buf(hist_type_idx);
	be_line_t *line;
	const char *ptr;
	size_t len;

	path = quote_file_name(path);
	// search from the newest to the oldest
	for (line = BUF_BOT_LINE(buf); IS_NODE_INT(line); line = NODE_PREV(line)) {
		// /home/user/filename.exp|1234
		// '/home/user/ filename.exp '|1234
		if ((ptr = strstr(line->data, FILE_PATH_SEPARATOR)) != NULL) {
			len = ptr - line->data;
		} else {
			len = line_data_len(line);
		}
		if (strncmp(line->data, path, len) == 0) {
			return line->data;
		}
	}
	return path;		// return original string
}

//-----------------------------------------------------------------------------
int select_from_history_list(int hist_type_idx, char *buffer)
{
	be_buf_t *edit_buf_save;

	edit_buf_save = get_cep_buf();
	load_histories();
	renumber_all_bufs_from_top(&history_buffers);
	set_cep_buf(get_history_buf(hist_type_idx));
	CEPBV_CL = CUR_EDIT_BUF_BOT_LINE;
	post_cmd_processing(CUR_EDIT_BUF_TOP_LINE, CURS_MOVE_HORIZ, LOCATE_CURS_NONE,
	 UPDATE_SCRN_ALL_SOON);

	int ret = call_editor(1, 1);

	if (ret > 0) {
		strlcpy__(buffer, CEPBV_CL->data, MAX_EDIT_LINE_LEN);
	} else {
		strcpy__(buffer, "");
	}
	set_cep_buf(edit_buf_save);

	return ret; // 1: selected, 0: done in editor, -1: cancelled
}

//------------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void dump_history_ix(int hist_type_idx)
{
	buf_dump_lines(get_history_buf(hist_type_idx), INT_MAX);
}
void dump_hist_bufs(void)
{
	buf_dump_bufs(HIST_BUFS_TOP_ANCH);
}
void dump_hist_bufs_lines(void)
{
	buf_dump_bufs_lines(HIST_BUFS_TOP_ANCH, "hist-bufs");
}
#endif // ENABLE_DEBUG

#endif // ENABLE_HISTORY

// End of history.c
