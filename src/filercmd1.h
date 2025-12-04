/**************************************************************************
 *   filercmd1.h                                                           *
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

#ifndef filercmd1_h
#define filercmd1_h

#ifdef ENABLE_FILER

int chk_inp_str_ret_val_filer(int ret);

// "dof_..."  DO Editor function
void dof_up();
void dof_down();
void dof_page_up();
void dof_page_down();
void dof_top_of_list();
void dof_bottom_of_list();

void dof_refresh_filer();

void dof_set_filter();
void dof_select_file();
void dof_select_no_file();
void dof_select_all_files();
void dof_select_all_regular_files();
void disp_files_selected();
void dof_find_file();

void dof_tog_show_dot_file();
void dof_inc_file_view_mode();
void dof_clear_file_sort_mode();
void dof_inc_file_sort_mode();
void dof_tog_show_zebra_striping();
void dof_tog_panes();
void dof_tog_panex();
void dof_inc_key_list_lines();
#ifdef ENABLE_HELP
void dof_splash();
void dof_view_file_list();
void dof_view_func_list();
#endif // ENABLE_HELP

void dof_menu_0();

#endif // ENABLE_FILER

#endif // filercmd1_h

// End of filercmd1.h
