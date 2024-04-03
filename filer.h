/**************************************************************************
 *   filer.h                                                              *
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

#ifndef filer_h
#define filer_h

#ifdef ENABLE_FILER

#define FILER_PANES		MAX_APP_PANES_2
typedef struct {
	char org_cur_dir[MAX_PATH_LEN+1];	// original current directory
	int cur_pane_idx;					// 0: Left, 1: Right
	filer_view_t filer_views[FILER_PANES];
} filer_panes_t;
extern filer_panes_t *cur_filer_panes;	// Current Filer Panes (instance is allocated locally)

typedef enum {
	FILER_DO_NOTHING = 					0,
	FILER_DO_UPDATE_FILE_LIST_AUTO = 	1,	// periodic file list update
	FILER_DO_UPDATE_FILE_LIST_FORCE = 	2,	// force file list update
											// quit:
	FILER_DO_QUIT = 					5,	//   quit        and return from filer
	FILER_DO_ABORT = 					6,	//   abort       and return from filer
	FILER_DO_FILE_LOADED = 				7,	//   file loaded and return from filer
											// list mode:
	FILER_DO_ENTER_FILE_NAME =			8,	//   return file name (file, dir)
	FILER_DO_ENTER_FILE_PATH = 			9,	//   return file path (/path/to/file, /path/to/dir)
	FILER_DO_ENTER_CUR_DIR_PATH = 		10,	//   return current directory path (/path/to/dir)
} filer_do_next_t;
extern filer_do_next_t filer_do_next;

void init_filer_panes(filer_panes_t *fps, const char *cur_dir);
filer_panes_t *inherit_filer_panes(filer_panes_t *next_fps);
void free_filer_panes(filer_panes_t *fps, filer_panes_t *prev_fps);
void set_filer_cur_pane_idx(int cur_pane_idx);
int get_filer_cur_pane_idx();
filer_view_t *get_cur_filer_view(void);
filer_view_t *get_other_filer_view(void);

int call_filer(int push_win, int list_mode,
 const char *dir, const char *filter, char *path_buf, int buf_len);

int update_screen_filer(int title_bar, int status_bar, int refresh);

int filer_win_get_file_path_lines(void);
int filer_win_get_file_list_lines(void);
int filer_win_get_file_path_y(void);
int filer_win_get_file_list_y(void);

#endif // ENABLE_FILER

#endif // filer_h

// End of filer.h
