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

// directory

const char *get_starting_dir(void);
const char *get_home_dir(void);
const char *get_tty_name(void);
int check_wsl();
int check_availability_of_script();

// change current directory -----------

int change_cur_dir_saving_prev_next_dir(const char *path,
 char *cur_path, char *prev_path, char *next_dir_sel);

int change_cur_dir_by_file_path_after_save(char *dir_save, const char *file_path);
int change_cur_dir_by_file_path(char *file_path);
int change_cur_dir_after_save(char *dir_save, const char *dir);

char *strip_file_if_path_is_file(const char *path, char *dir);
char *strip_file_from_path(const char *path, char *dir);

int change_cur_dir(const char *dir);
char *get_full_path_of_cur_dir(char *dir);
const char *full_path_of_cur_dir_s();
char *get_real_path_of_cur_dir(char *dir);

// file path string -------------------

#define PATH_INVALIDATION_TAG		"#"

char *normalize_full_path(char *full_path);

char *cat_dir_and_file(char *buf, const char *dir, const char *file);

int compare_file_path_in_abs_path(const char *file_path_a, const char *file_path_b);

char *get_abs_path(const char *path, char *abs_path);
char *get_full_path(const char *path, char *buf);
char *get_real_path(const char *path, char *buf);

char *get_file_name_extension(char *file_name);

char *separate_path_to_dir_and_file(const char *path, char *buf_dir, char *buf_file);

char *remove_last_slash(char *path);
char *add_last_slash_to_dir(char *dir);

int contain_redundant_slash(char *path);
char *remove_redundant_slash(char *path);
char *get_last_slash(char *path);

int compare_file_path_from_tail(const char *full_path, const char *file_path);

#ifdef ENABLE_FILER
int is_path_wildcard(char *path);
#endif // ENABLE_FILER

// file permission --------------------

int is_path_exist(const char *path);
int is_path_regular_file(const char *path);
int is_path_dir(const char *path);
int is_file_writable(const char *path);
int is_st_writable(struct stat *st);
ssize_t get_file_size(const char *path);
int is_dir_readable(const char *path);

char *getcwd__(char *cwd);
char *getenv_pwd(char *cwd);
char *getenv__(char *env);

// realpath() -------------------------
#if defined(HAVE_REALPATH)
char *realpath__(const char *path, char *buf, int buf_len);
#endif // HAVE_REALPATH

// readlink() -------------------------
int readlink__(const char *path, char *buffer, int len);

BOOL is_abs_path(const char *path);

int write_text_to_file(const char *file_path, char append, const char *text);
int remove_file(const char* file_path);

// tests ======================================================================

#ifdef START_UP_TEST

void test_cwd_PWD();

int test_normalize_path(void);

void test_cat_dir_and_file();

void test_get_full_path(void);

#if defined(HAVE_REALPATH)
void test_realpath(void);
#endif // HAVE_REALPATH

void test_get_file_name_extension(void);

void test_separate_path_to_dir_and_file();

#endif // START_UP_TEST

#endif // utilpath_h

// End of utilpath.h
