/**************************************************************************
 *   menu.h                                                               *
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

#ifndef menu_h
#define menu_h

int editor_menu_n(int group_idx);
int filer_menu_n(int group_idx);

int disp_drop_down_menu(int group_idx, int sel_idx, int yy, int xx);
int get_groups_in_func_key_table(void);
int get_func_key_group_entries(int group_idx);
short get_func_key_code(int group_idx, int entry_idx);
func_key_list_t *get_func_key_group_from_group_idx(int group_idx);
int get_menu_items_from_group_idx(int group_idx);

#endif // menu_h

// End of menu.h
