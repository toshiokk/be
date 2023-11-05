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

extern filer_panes_t *cur_filer_panes;	// Current Filer ViewS (instance is allocated locally)

#define FILER_DO_NOTHING			0
#define FILER_DO_UPDATE_SCREEN		1
#define FILER_DO_REFRESH_AUTO		2	// periodic file list update
#define FILER_DO_REFRESH_FORCE		3	// force file list update

#define FILER_DO_QUIT				5
#define FILER_DO_ABORT				6
#define FILER_DO_LOADED_FILE		7
#define FILER_DO_ENTERED_FILE		8	// file, dir
#define FILER_DO_ENTERED_FILE_PATH	9	// /home/user/.bashrc
#define FILER_DO_ENTERED_DIR_PATH	10	// /home/user/bin
extern int filer_do_next;

void init_filer_panes(filer_panes_t *fps, const char *cur_dir);
void set_filer_cur_pane_idx(int cur_pane_idx);
int get_filer_cur_pane_idx();
filer_view_t *get_filer_view(int pane_idx);
filer_view_t *get_cur_filer_view(void);
filer_view_t *get_other_filer_view(void);

int call_filer(int push_win, int list_mode,
 char *dir, char *filter, char *file_path, int buf_len);

int update_screen_filer(int title_bar, int status_bar, int refresh);

#endif // ENABLE_FILER

#endif // filer_h

// End of filer.h
