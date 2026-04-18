/**************************************************************************
 *   filerlist.h                                                          *
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

#ifndef filerlist_h
#define filerlist_h

#ifdef ENABLE_FILER

#define _FILE_SEL_NONE_		0x00	// file un-selected
#define _FILE_SEL_MAN_		0x01	// file selected manually
typedef struct {
	char *file_name;
						// | non symlink | symlink                |
						// |-------------|------------------------|
	struct stat lst;	// | file or dir | symlink                |
	struct stat st;		// | file or dir | target file or dir     |
	char *symlink;		// | NULL        | target path of symlink |
	char selected;
} file_info_t;

typedef struct {
	char org_cur_dir[MAX_PATH_LEN+1];	// original current directory
	char cur_dir[MAX_PATH_LEN+1];		// current directory
	char filter[MAX_PATH_LEN+1];		// e.g. "*.h", "*.c", "*.[hc], "": no filter
	char listed_dir[MAX_PATH_LEN+1];	// directory from which file list gotten
	char prev_dir[MAX_PATH_LEN+1];		// previous current directory
	char next_file[MAX_PATH_LEN+1];		// next file to be selected after changing dir
	int selecting_file_idx;				// starting file index into array(-1: not selecting)
	int cur_file_idx;					// current file index into array
	int cursor_y;						// cursor y position in file view
	int file_info_entries;				// array size
	file_info_t *file_info_array;		// array
} filer_view_t;

#define FILER_PANES		MAX_APP_PANES_2
typedef struct {
	filer_view_t filer_views[FILER_PANES];
} filer_panes_t;

extern filer_panes_t *cur_filer_panes;	// Current Filer Panes (instance is allocated locally)

//------------------------------------------------------------------------------

int read_into_file_info_array(filer_view_t *fv);
void free_file_info_array(filer_view_t *fv);

#ifdef START_UP_TEST
void test_get_file_size_str();
#endif // START_UP_TEST

const char *file_info_str(file_info_t *file_info,
 int show_link, int trunc_file_name, int selected);
const char *get_file_size_str(char *buf_size, loff_t size);

void sort_file_info_array(filer_view_t *fv);
int get_file_type_num(file_info_t *info);

int get_cfv_files_selected();
int get_cfv_files_selected_size(size_t *selected, size_t *total);
int get_fv_files_selected(filer_view_t *fv);
int get_fv_files_selected_size(filer_view_t *fv, size_t *selected, size_t *total);

int get_first_file_idx_selected();
int get_next_file_idx_selected(int file_idx);
#define MY_ISREG		0x01
#define MY_ISDIR		0x02
#define MY_ISLNK		0x04
int is_cfv_file_selectable(int file_idx, mode_t type);

int research_file_name_in_file_info_array(filer_view_t *fv, const char *file_name,
 int cur_dir_changed);
int search_file_name_in_file_info_array(filer_view_t *fv, const char *file_name);

int search_file_from_list(const char *file_name, int first0_next1, int direction);

void normalize_filer_cur_file_idx(filer_view_t *fv);
void normalize_filer_cursor_y(filer_view_t *fv);

#endif // ENABLE_FILER

#endif // filerlist_h

// End of filerlist.h
