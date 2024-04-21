/**************************************************************************
 *   utilpath.h                                                           *
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

#ifndef utilpath_h
#define utilpath_h

#define MAX_PATH_LEN		PATH_MAX	// =4096


#ifdef START_UP_TEST
int test_normalize_full_path(void);
#endif // START_UP_TEST
char *normalize_full_path(char *abs_path);

#ifdef START_UP_TEST
void test_cat_dir_and_file();
#endif // START_UP_TEST
char *cat_dir_and_file(char *buf, const char *dir, const char *file);

#ifdef START_UP_TEST
void test_get_full_path(void);
#endif // START_UP_TEST

char *get_abs_path(const char *path, char *buf);
char *get_real_path(const char *path, char *buf, int buf_len);

#if defined(HAVE_REALPATH)
#ifdef START_UP_TEST
void test_realpath(void);
#endif // START_UP_TEST
char *realpath__(const char *path, char *buf, int buf_len);
#else // HAVE_REALPATH
char *my_realpath(const char *path, char *buf, int buf_len);
#endif // HAVE_REALPATH

#ifdef START_UP_TEST
void test_get_file_name_extension(void);
#endif // START_UP_TEST
char *get_file_name_extension(char *file_name);

#ifdef START_UP_TEST
void test_separate_path_to_dir_and_file();
#endif // START_UP_TEST
char *separate_path_to_dir_and_file(char *path, char *buf_dir, char *buf_file);

//-----------------------------------------------------------------------------

int change_cur_dir_in_path(char *path, char *cur_dir, char *prev_dir, char *next_file);

char *remove_last_slash(char *path);
int contain_redundant_slash(char *path);
char *remove_redundant_slash(char *path);
char *get_last_slash(char *path);
/////int is_path_only_file_name(const char *path);

char *separate_dir_part_and_file_part(const char *path,
 char *dir_part, char *file_part);

char *strip_file_from_path(char *path, char *dir);

/////int get_file_type_by_file_path(const char *file_path);
int is_path_exist(const char *path);
int is_path_regular_file(const char *path);
int is_path_dir(const char *path);
int is_file_writable(const char *path);
int is_st_writable(struct stat *st);
#ifdef ENABLE_FILER
int is_path_wildcard(char *path);
#endif // ENABLE_FILER

char *get_home_dir(void);
const char *get_start_dir(void);

int change_cur_dir_by_file_path_after_save(char *dir_save, char *file_path);
int change_cur_dir_by_file_path(char *file_path);
char *strip_file_if_path_is_file(char *path, char *dir);

int change_cur_dir_after_save(char *dir_save, const char *dir);
char *get_cur_dir(char *dir);
int change_cur_dir(const char *dir);
int is_dir_readable(const char *path);

char *getcwd__(char *cwd);
char *getenv_pwd(char *cwd);
char *getenv__(char *env);
char *add_last_slash_to_dir(char *dir);

int readlink__(const char *path, char *buffer, int len);

#endif // utilpath_h

// End of utilpath.h
