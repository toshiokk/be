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

void set_cur_filer_panes(filer_panes_t *fps);
filer_panes_t* get_cur_filer_panes();
void init_filer_panes(filer_panes_t *fps, const char *cur_dir);
void inherit_filer_panes(filer_panes_t *fps);
void free_filer_panes();
void copy_filer_panes_cur_dir(filer_panes_t *dest, filer_panes_t *src);

#ifdef ENABLE_DEBUG
void dump_filer_view(filer_view_t *fv);
void dump_filer_view_cur_dir(filer_view_t *fv);
#endif // ENABLE_DEBUG

filer_view_t *get_fv_from_pane(int pane_idx);
filer_view_t *get_fv_from_cur_pane();
filer_view_t *get_fv_from_another_pane();
#define FV_ORG_CUR_DIR(fv)				((fv)->org_cur_dir)
#define FV_CUR_DIR(fv)					((fv)->cur_dir)
#define FV_FILTER(fv)					((fv)->filter)
#define FV_SEL_F_IDX(fv)				((fv)->selecting_file_idx)
#define FV_CUR_F_IDX(fv)				((fv)->cur_file_idx)
#define FV_CURS_Y(fv)					((fv)->cursor_y)
#define FV_LISTED_DIR(fv)				((fv)->listed_dir)
#define FV_F_INFO_ENTRIES(fv)			((fv)->file_info_entries)
#define FV_F_INFO_ARRAY(fv)				((fv)->file_info_array)
#define FV_F_INFO(fv, idx)				(FV_F_INFO_ARRAY(fv)[idx])
#define FV_F_INFO_F_NAME(fv, idx)		(FV_F_INFO(fv, idx).file_name)
#define FV_F_INFO_ST(fv, idx)			(FV_F_INFO(fv, idx).st)
#define FV_F_INFO_LST(fv, idx)			(FV_F_INFO(fv, idx).lst)
#define FV_F_INFO_SLINK(fv, idx)		(FV_F_INFO(fv, idx).symlink)
#define FV_F_INFO_SELECTED(fv, idx)		(FV_F_INFO(fv, idx).selected)
file_info_t *get_cfv_file_info_array();
int get_cfv_file_info_entries();
file_info_t *get_fv_file_info_array(filer_view_t *fv);
int get_fv_file_info_entries(filer_view_t *fv);
file_info_t *get_fv_file_info(filer_view_t *fv, int file_idx);

file_info_t *get_cfv_file_info(int file_idx);
const char *get_cfv_file_name(int file_idx);
const char *get_cfv_symlink(int file_idx);
struct stat get_cfv_file_st(int file_idx);
__mode_t get_cfv_file_st_mode(int file_idx);
void set_cfv_file_selected(int file_idx, char selected);
char get_cfv_file_selected(int file_idx);

const char *get_fv_file_name(filer_view_t *fv, int file_idx);
const char *get_fv_symlink(filer_view_t *fv, int file_idx);
struct stat get_fv_file_st(filer_view_t *fv, int file_idx);
__mode_t get_fv_file_st_mode(filer_view_t *fv, int file_idx);
void set_fv_file_selected(filer_view_t *fv, int file_idx, char selected);
char get_fv_file_selected(filer_view_t *fv, int file_idx);

file_info_t *get_cfv_file_ptr_from_pane(int pane_idx);
void set_cfv_file_idx(int file_idx);
int get_cfv_file_idx();
int get_fv_file_idx(filer_view_t *fv);

do_next_t do_call_filer(int push_win, int list_mode, const char *dir, const char *filter,
 char *path_buf);
void set_text_to_output_buf_filer(char *text);

int update_screen_filer(s_b_d_t status_bar, int refresh);

int filer_vert_scroll_margin_lines();
int filer_vert_scroll_lines();

int filer_win_get_file_path_lines();
int filer_win_get_file_list_lines();
int filer_win_get_file_path_y();
int filer_win_get_file_list_y();

#endif // ENABLE_FILER

#endif // filer_h

// End of filer.h
