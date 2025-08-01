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

const char *get_starting_dir();
const char *get_home_dir();
const char *get_tty_name();
const char *get_tty_name_file_part();
int check_wsl();
int check_availability_of_script();

// file path string -------------------

char *normalize_full_path(char *full_path);

const char *cat_dir_and_file_s(const char *dir, const char *file);
const char *cat_dir_and_file_s1(const char *dir, const char *file);
const char *cat_dir_and_file_s2(const char *dir, const char *file);
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

BOOL is_abs_path(const char *path);

#ifdef ENABLE_FILER
int is_path_wildcard(char *path);
#endif // ENABLE_FILER

char *getcwd__(char *cwd);
char *getenv_pwd(char *cwd);
char *getenv__(char *env);

// realpath() -------------------------
#if defined(HAVE_REALPATH)
char *realpath__(const char *path, char *buf, int buf_len);
#endif // HAVE_REALPATH

// readlink() -------------------------
int readlink__(const char *path, char *buffer, int len);

const char* add_temporal_file_path_prefix(const char* file_path);
const char* remove_temporal_file_path_prefix(const char* file_path);
int is_temporal_file_path(const char* file_path);

// tests ======================================================================

#ifdef START_UP_TEST

void test_cwd_PWD();
void test_normalize_path();
void test_cat_dir_and_file();
void test_get_full_path();

#if defined(HAVE_REALPATH)
void test_realpath();
#endif // HAVE_REALPATH

void test_get_file_name_extension();
void test_separate_path_to_dir_and_file();

#endif // START_UP_TEST

#endif // utilpath_h

// End of utilpath.h
