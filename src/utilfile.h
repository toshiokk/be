/**************************************************************************
 *   utilfile.h                                                           *
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

#ifndef utilfile_h
#define utilfile_h

// file status and permission --------------------

int is_path_exist(const char *path);
int is_path_regular_file(const char *path);
int is_path_executable_file(const char *path);
int is_path_dir(const char *path);
int is_file_writable(const char *path);
int is_st_writable(struct stat *st);
ssize_t get_file_size(const char *path);
int is_dir_readable(const char *path);

// change current directory -----------

int change_cur_dir_saving_prev_next_dir(const char *path,
 char *cur_path, char *prev_path, char *next_dir_sel);

int change_cur_dir_by_file_path_after_save(char *dir_save, const char *file_path);
int change_cur_dir_by_file_path(char *file_path);
int change_cur_dir_after_save(char *dir_save, const char *dir);

char *strip_file_if_path_is_file(const char *path, char *dir);
char *strip_file_from_path(const char *path, char *dir);

int change_cur_dir(const char *dir);
const char *get_full_path_of_cur_dir(char *dir);
const char *get_real_path_of_cur_dir(char *dir);

int write_text_to_file(const char *file_path, char append, const char *text);
int remove_file(const char* file_path);

#endif // utilfile_h

// End of utilfile.h
