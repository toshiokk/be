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

extern ef_do_next_t filer_do_next;

void set_cur_filer_panes(filer_panes_t *fps);
filer_panes_t* get_cur_filer_panes();
void init_cur_filer_panes(filer_panes_t *fps, const char *cur_dir);	// TODO: rename
void destroy_filer_panes();
void copy_filer_panes_cur_dir(filer_panes_t *dest, filer_panes_t *src);

filer_view_t *get_cur_filer_view(int pane_idx);
filer_view_t *get_cur_filer_pane_view(void);
filer_view_t *get_another_filer_pane_view(void);

file_info_t *get_cur_fv_file_infos();
file_info_t *get_cur_fv_cur_file_ptr();
file_info_t *get_cur_fv_file_ptr(int file_idx);
file_info_t *get_fv_file_ptr(int pane_idx);
int get_cur_fv_file_idx();
void set_cur_fv_file_idx(int file_idx);

int do_call_filer(int push_win, int list_mode, const char *dir, const char *filter,
 char *path_buf);

int update_screen_filer(int status_bar, int refresh);

int filer_vert_scroll_margin_lines();
int filer_vert_scroll_lines();

int filer_win_get_file_path_lines(void);
int filer_win_get_file_list_lines(void);
int filer_win_get_file_path_y(void);
int filer_win_get_file_list_y(void);

#endif // ENABLE_FILER

#endif // filer_h

// End of filer.h
