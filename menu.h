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

/////key_code_t get_func_key_code(int group_idx, int entry_idx);
/////key_code_t get_func_key_code(int group_idx, int entry_idx)
/////{
/////	func_key_t *func_key;
/////	if ((func_key = get_func_key_group_from_group_idx(group_idx)) == NULL) {
/////		return K_NONE;
/////	}
/////	return func_key[entry_idx].keys[0];
/////}
func_key_t *get_func_key_group_from_group_idx(int group_idx);

#ifdef ENABLE_DEBUG
#ifdef START_UP_TEST
int check_multiple_assignment_of_key();
int check_all_functions_accessible_without_function_key();
#endif // START_UP_TEST
#endif // ENABLE_DEBUG

#endif // menu_h

// End of menu.h
