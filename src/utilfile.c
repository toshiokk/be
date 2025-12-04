/**************************************************************************
 *   utilfile.c                                                           *
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

#include "utilincs.h"

int is_path_exist(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0;	// 1: exists, 0: does not exist
}
int is_path_regular_file(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0)
		return -1;				// -1: no such file nor directory
	return S_ISREG(st.st_mode);	// 1: file, 0: non-file
}
int is_path_executable_file(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0)
		return -1;				// -1: no such file nor directory
	return S_ISREG(st.st_mode) && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
								// 1: executable file, 0: non-file
}
int is_path_dir(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0)
		return -1;				// -1: no such file nor directory
	return S_ISDIR(st.st_mode);	// 1: directory, 0: non-directory
}
int is_file_writable(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0)
		return -1;				// -1: no such file
	return is_st_writable(&st);	// 1: writable, 0: not writable
}
int is_st_writable(struct stat *st)
{
	if (st->st_uid == geteuid()) {
		return (st->st_mode & S_IWUSR) != 0;	// 1: writable, 0: not-writable
	} else if (st->st_gid == getegid()) {
		return (st->st_mode & S_IWGRP) != 0;	// 1: writable, 0: not-writable
	}
	return (st->st_mode & S_IWOTH) != 0;		// 1: writable, 0: not-writable
}
ssize_t get_file_size(const char *path)
{
	struct stat st;
	if (stat(path, &st) < 0) {
		return -1;
	}
	return st.st_size;
}
int is_dir_readable(const char *path)
{
	DIR *dir = opendir(path);
	// If dir is NULL, don't call closedir()
	if (dir) {
		closedir(dir);
		return 1;		// 1: readable
	}
	return 0;			// 0: not-readable
}
time_t get_file_mtime(const char *file_path)
{
	struct stat cur_file_stat;
	if (get_file_stat(file_path, &cur_file_stat)) {
		cur_file_stat.st_mtime = 0;		// no such file
	}
	return cur_file_stat.st_mtime;
}
int get_file_stat(const char *file_path, struct stat *stat_)
{
	int ret = stat(file_path, stat_);
	if (ret) {
		stat_->st_mtime = 0;		// no such file
	}
	return ret;
}

//------------------------------------------------------------------------------
// change direcotry independently from filer view
//   cur_path:      current directory before changing directory and after changing directory
//   prev_path:     current directory before changing directory
//   next_dir_sel:  directory to be selected in filer after changing directory
int chdir_saving_prev_next_dir(const char *path,
 char *cur_path, char *prev_path, char *next_dir_sel)
{
	char dir[MAX_PATH_LEN+1];
	if (is_path_regular_file(path) > 0) {
		// If path is pointing a file, change to the directory containing it.
		separate_path_to_dir_and_file(path, dir, next_dir_sel);
	} else {
		if (strcmp(path, ".") == 0) {
			return 1;	// OK
		} else if (strcmp(path, "..") == 0) {
			separate_path_to_dir_and_file(cur_path, dir, next_dir_sel);
		} else if (strcmp(path, "~") == 0) {
			strcpy__(next_dir_sel, "..");
			get_abs_dir_path("~", dir);
		} else if (path[0] == '/') {
			// absolute path "/some/dir"
			strcpy__(next_dir_sel, "..");
			strlcpy__(dir, path, MAX_PATH_LEN);
		} else {
			// relative path "some/dir" or "./some/dir"
			strcpy__(next_dir_sel, "..");
			// "/cur/dir" + "some/dir" ==> "/cur/dir/some/dir"
			concat_dir_and_dir(dir, cur_path, path);
		}
	}
	normalize_file_path(dir);
	if (is_dir_readable(dir) == 0) {
		// We can't open this dir for some reason.
		return 0;	// Error
	}
	strlcpy__(prev_path, cur_path, MAX_PATH_LEN);
	change_cur_dir(dir);
	get_full_path_of_cur_dir(cur_path);
	return 1;	// OK
}

//------------------------------------------------------------------------------
// comparison of chdir_ functions
// | func                          |save before change|try parent|
// |-------------------------------|------------------|----------|
// | chdir_by_file_path_after_save | Yes              | Yes      |
// | chdir_by_file_path            | No               | Yes      |
// | chdir_after_save              | Yes              | No       |

int chdir_by_file_path_after_save(char *dir_save, const char *file_path)
{
	char dir[MAX_PATH_LEN+1];
	strip_file_if_path_is_file(file_path, dir);
	return chdir_after_save(dir_save, dir);
}
int chdir_by_file_path(char *file_path)
{
	char dir[MAX_PATH_LEN+1];
	strip_file_if_path_is_file(file_path, dir);
	return change_cur_dir(dir);
}
int chdir_after_save(char *dir_save, const char *dir)
{
	get_full_path_of_cur_dir(dir_save);
	return change_cur_dir(dir);
}

//------------------------------------------------------------------------------
// change process's current directory
PRIVATE char full_path_of_cur_dir[MAX_PATH_LEN+1] = "";
PRIVATE char real_path_of_cur_dir[MAX_PATH_LEN+1] = "";
int change_cur_dir(const char *dir)
{
	if (chdir(dir) < 0) {
		return 0;		// 0: error
	}
	// update "full_path" and "real_path"
	strlcpy__(full_path_of_cur_dir, dir, MAX_PATH_LEN);
	add_trailing_slash_in_handling(full_path_of_cur_dir);
	getcwd__(real_path_of_cur_dir);
	return 1;			// 1: changed
}
const char *get_full_path_of_cur_dir(char *dir)
{
	if (dir == NULL) {
		dir = full_path_of_cur_dir;
	} else {
		strlcpy__(dir, full_path_of_cur_dir, MAX_PATH_LEN);
	}
	return dir;
}
const char *get_real_path_of_cur_dir(char *dir)
{
	if (dir == NULL) {
		dir = real_path_of_cur_dir;
	} else {
		strlcpy__(dir, real_path_of_cur_dir, MAX_PATH_LEN);
	}
	return dir;
}

//------------------------------------------------------------------------------
// get real current directory(symbolic link is expanded to absolute path)
// NOTE: getcwd() returns real_path of the current directory
char *getcwd__(char *cwd)
{
	if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
		strcpy__(cwd, "");
	}
	add_trailing_slash_in_handling(cwd);
	return cwd;
}

// NOTE: "PWD" environment is not automatically updated after changing current directory
//       so you can use this only for getting application startup directory
char *getenv_pwd(char *pwd)
{
	strlcpy__(pwd, getenv__("PWD"), MAX_PATH_LEN);
	add_trailing_slash_in_handling(pwd);
	return pwd;
}

char *getenv__(char *env)
{
	char *ptr;
	if ((ptr = getenv(env)) == NULL) {
		ptr = "";
	}
	return ptr;
}

//------------------------------------------------------------------------------
int write_text_to_file(const char *file_path, char append, const char *text)
{
	FILE *fp = fopen(file_path, append ? "a" : "w");
	if (fp == NULL) {
		return -1;
	}
	fputs(text, fp);
	if (fclose(fp) != 0) {
		return -1;
	}
	return 0;
}

int remove_file(const char *file_path)
{
	return remove(file_path);
}

// End of utilfile.c
