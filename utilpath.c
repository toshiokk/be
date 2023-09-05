/**************************************************************************
 *   utilpath.c                                                           *
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

/* Strip one dir from the end of the string */
// /dir1/dir2/file ==> /dir1/dir2
// /dir1/dir2/ ==> /dir1/dir2
// /dir1 ==> /
///char *strip_one_dir(char *path, char *dir)
///{
///	char *ptr;
///
//////flf_d_printf("strip_one_dir(%s)\n", path);
///	if (strcmp(path, "/") != 0 && strlen(path)) {
///		if (path[strlen(path)-1] == '/')
///			// "/dir1/dir2/" ==> "/dir1/dir2"
///			path[strlen(path)-1] = '\0';
///	}
///	ptr = separate_dir_and_file(path, dir);
//////flf_d_printf(" ==> return:[%s], path:[%s], dir:[%s]\n", ptr, path, dir);
///	return ptr;							// return stripped dir or file
///}
// /dir1/dir2/file ==> /dir1/dir2
char *strip_file_from_path(const char *path)
{
	static char dir[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];

	strlcpy__(dir, path, MAX_PATH_LEN);
	separate_dir_and_file(dir, file);
	return dir;
}
// /dir1/dir2/file ==> /dir1/dir2  file
// /dir1/dir2 ==> /dir1  dir2
// /file      ==> /  file
// file       ==> /  file
// /          ==> /  ""
// ""         ==> /  ""
char *separate_dir_and_file(char *path, char *file)
{
	char *ptr;

	ptr = get_last_slash(path);
	if (*ptr == '/') {
		*ptr++ = '\0';	// NUL terminate ("/dir/file" ==> "/dir")
		strlcpy__(file, ptr, MAX_PATH_LEN);	// "file"
		normalize_root_dir(path);			// "" ==> "/"
	} else {
		strcpy__(path, "/");				// "file" ==> "/"
		strlcpy__(file, ptr, MAX_PATH_LEN);	// "file" ==> "file"
	}
///flf_d_printf("separate_dir_and_file(%s)\n", path);
///flf_d_printf(" ==> dir/file:[%s]/[%s]\n", dir, file);
	return file;		// return file
}

// "" ==> "/"
char *normalize_root_dir(char *dir)
{
	if (dir[0] == '\0')
		strcpy__(dir, "/");
	return dir;
}

///// point to the last part of a path
///// /dir1/dir2/file ==> file
///char *get_last_part_of_path(char *path);
///char *get_last_part_of_path(char *path)
///{
///	char *ptr;
///
///	ptr = get_last_slash(path);
///	if (*ptr == '/')
///		ptr++;
///	return ptr;
///}

// /dir1/dir2/file ==> /dir1/dir2
// /file           ==> /file
// file            ==> file
char *remove_last_slash(char *path)
{
	char *ptr;

	ptr = get_last_slash(path);
	if (ptr) {
		if (path < ptr && ptr[0] == '/' && ptr[1] == '\0')
			*ptr = '\0';
	}
	return path;
}

// point to the last '/'
// /dir1/dir2/file    file
//           ^        ^
char *get_last_slash(char *path)
{
	char *ptr;

	ptr = strrchr(path, '/');
	if (ptr == NULL)
		ptr = path;
	return ptr;
}
////int is_path_only_file_name(const char *path)
////{
////	return strchr(path, '/') == NULL;
////}

int is_path_exist(const char *path)
{
	struct stat st;

	return stat(path, &st) == 0;
}
int is_path_regular_file(const char *path)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return -1;				// no such file nor directory
	return S_ISREG(st.st_mode);	// 1:file, 0:non-file
}
int is_path_dir(const char *path)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return -1;				// no such file nor directory
	return S_ISDIR(st.st_mode);	// 1:directory, 0:non-directory
}
int is_file_writable(const char *path)
{
	struct stat st;

	if (stat(path, &st) < 0)
		return -1;
	return is_st_writable(&st);
}
int is_st_writable(struct stat *st)
{
	if (st->st_uid == geteuid()) {
		return (st->st_mode & S_IWUSR) != 0;
	} else if (st->st_gid == getegid()) {
		return (st->st_mode & S_IWGRP) != 0;
	}
	return (st->st_mode & S_IWOTH) != 0;
}

#ifdef ENABLE_FILER
int is_dir_readable(const char *path)
{
	DIR *dir;

////flf_d_printf("path: %s\n", path);
	dir = opendir(path);
	// If dir is NULL, don't call closedir()
	if (dir)
		closedir(dir);
	return dir != NULL;
}

int is_path_wildcard(char *path)
{
#ifdef ENABLE_DEBUG
	char *ptr;

	ptr = strchr(path, '*');
	if (ptr)
		flf_d_printf("strchr(path, '*') ==> [%s]\n", ptr);
	ptr = strchr(path, '?');
	if (ptr)
		flf_d_printf("strchr(path, '?') ==> [%s]\n", ptr);
#endif // ENABLE_DEBUG
	return strchr(path, '*') || strchr(path, '?');
}
#endif // ENABLE_FILER

char *separate_dir_part_and_file_part(const char *path,
 char *dir_part, char *file_part)
{
	struct stat fileinfo;
	char *last_slash;

	if (stat(path, &fileinfo) == 0 && S_ISDIR(fileinfo.st_mode)) {
		// /dir1/dir2
		strlcpy__(dir_part, path, MAX_PATH_LEN);
		strcpy__(file_part, "");
	} else {
		// /dir1/dir2/filename or filename only
		last_slash = strrchr(path, '/');
		if (last_slash != NULL) {
			// /dir1/dir2/filename
			strlcpy__(dir_part, path, last_slash - path);
			strlcpy__(file_part, last_slash+1, MAX_PATH_LEN);
		} else {
			// filename only
			strcpy__(dir_part, "");
			strlcpy__(file_part, path, MAX_PATH_LEN);
		}
	}
	return dir_part;
}

char *get_home_dir(void)
{
	char *env_home;
	const struct passwd *userdata = 0;
	static char buffer[MAX_PATH_LEN+1];

	if (strlen(env_home = getenv__("HOME"))) {
		strlcpy__(buffer, env_home, MAX_PATH_LEN);
		return buffer;
	}
	if ((userdata = getpwuid(geteuid())) != NULL) {
		strlcpy__(buffer, userdata->pw_dir, MAX_PATH_LEN);
		return buffer;
	}
	strcpy__(buffer, "/");
	return buffer;
}

//------------------------------------------------------------------------------
const char *get_start_dir(void)
{
	static char start_dir[MAX_PATH_LEN+1] = "";

	if (strcmp(start_dir, "") == 0) {
		if (strlen(get_cwd(start_dir)) == 0) {
			getenv_pwd(start_dir);
		}
	}
	return start_dir;
}
int save_change_cur_dir(char *dir_save, const char *dir)
{
	get_cur_dir(dir_save);
	return change_cur_dir(dir);
}
PRIVATE char current_directory[MAX_PATH_LEN+1] = "";
char *get_cur_dir(char *dir)
{
	if (strcmp(current_directory, "") == 0) {
		if (strlen(get_cwd(current_directory)) == 0) {
			getenv_pwd(current_directory);
		}
	}
	strlcpy__(dir, current_directory, MAX_PATH_LEN);
	return dir;
}
int change_cur_dir(const char *dir)
{
	int ret;

	if ((ret = chdir(dir)) == 0) {
		strlcpy__(current_directory, dir, MAX_PATH_LEN);
	}
	return ret;
}
//------------------------------------------------------------------------------

// get real current directory(symbolic link is expanded to absolute path)
char *get_cwd(char *cwd)
{
	if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
		strcpy__(cwd, "");
	}
	return cwd;
}

char *getenv_pwd(char *cwd)
{
	strlcpy__(cwd, getenv__("PWD"), MAX_PATH_LEN);
	return cwd;
}

char *getenv__(char *env)
{
	char *ptr;

	if ((ptr = getenv(env)) == NULL) {
		ptr = "";
	}
	return ptr;
}

char *add_last_slash_to_dir(char *dir)
{
	if (strcmp(dir, "/") != 0 && strlen(dir) && dir[strlen(dir) - 1] != '/') {
		strlcat__(dir, MAX_PATH_LEN, "/");
	}
	return dir;
}

// Concatenate path and file
// /dir1/dir2  file ==> /dir1/dir2/file
// /dir1/dir2/  file ==> /dir1/dir2/file
// dir1/dir2  file ==> dir1/dir2/file
// ""  file ==> file
char *cat_dir_and_file(char *buf, int buf_len, const char *dir, const char *file)
{
	int last;

	last = LIM_MIN(0, strnlen(dir, MAX_PATH_LEN) - 1);
	if (file[0] == '/') {
		// "/file" ==> "file"
		file++;
	}
	if (is_strlen_0(dir) || dir[last] == '/') {
///flf_d_printf("dir file [%s %s]\n", dir, file);
		snprintf(buf, buf_len+1, "%s%s", dir, file);
	} else {
///flf_d_printf("dir / file [%s / %s]\n", dir, file);
		snprintf(buf, buf_len+1, "%s/%s", dir, file);
	}
	return buf;
}

// read link and null terminate string
int readlink__(const char *path, char *buffer, int len)
{
	int ret;

	if ((ret = readlink(path, buffer, len)) > 0)
		buffer[ret] = '\0';
	return ret;
}

//-----------------------------------------------------------------------------------
// NOTE: Meaning of file_name, file_path, full_path and abs_path(real_path)
//  file_name          : filename.ext
//  file_path          : ../src/filename.ext
//  full_path          : /home/user/symlink/../src/filename.ext
//						 start by '/' and may contain symlinks and ".."
//  abs_path(real_path): /home/user/tools/src/filename.ext
//						 start by '/' and may not contain symlinks and ".."
//-----------------------------------------------------------------------------------

#ifdef START_UP_TEST
// /aaa/bbb/.. ==> /aaa
// /aaa/bbb/../ccc ==> /aaa/ccc
// /aaa/bbb/../../ccc ==> /ccc
// /../ccc ==> error
// /aaa/bbb/. ==> /aaa/bbb
// /aaa/bbb/./ccc ==> /aaa/bbb/ccc
// /. ==> /
PRIVATE void test_normalize_full_path_(const char *abs_path);
PRIVATE void test_normalize_full_path__(const char *abs_path);
int test_normalize_full_path(void)
{
	flf_d_printf("-----------------------\n");
	test_normalize_full_path_(".");
	test_normalize_full_path_("././.");
	test_normalize_full_path_("..");
	test_normalize_full_path_("../../..");
	test_normalize_full_path_("aaa/bbb/..");
	test_normalize_full_path_("aaa/bbb/../ccc");
	test_normalize_full_path_("aaa/bbb/../../ccc");
	test_normalize_full_path_("aaa/bbb/ccc/../../ddd");
	test_normalize_full_path_("aaa/bbb/ccc/../../../ddd");
	test_normalize_full_path_("../ccc");
	test_normalize_full_path_("../../../ccc");
	test_normalize_full_path_("../aaa/bbb/..");
	test_normalize_full_path_("aaa/bbb/.");
	test_normalize_full_path_("aaa/bbb/./ccc");
	test_normalize_full_path_(".");
	test_normalize_full_path_("./aaa/bbb/.");
	return 0;
}
PRIVATE void test_normalize_full_path_(const char *abs_path)
{
	char buffer[MAX_PATH_LEN+1];

	snprintf(buffer, MAX_PATH_LEN, "%s", abs_path);
	test_normalize_full_path__(buffer);
	snprintf(buffer, MAX_PATH_LEN, "/%s", abs_path);
	test_normalize_full_path__(buffer);
	snprintf(buffer, MAX_PATH_LEN, "%s/", abs_path);
	test_normalize_full_path__(buffer);
	snprintf(buffer, MAX_PATH_LEN, "/%s/", abs_path);
	test_normalize_full_path__(buffer);
}
PRIVATE void test_normalize_full_path__(const char *abs_path)
{
	char buffer[MAX_PATH_LEN+1];

	strlcpy__(buffer, abs_path, MAX_PATH_LEN);
	normalize_full_path(buffer);
	flf_d_printf("[%s] ==> [%s]\n", abs_path, buffer);
}
#endif // START_UP_TEST

// /dir1/.  ==> /dir1/
// /dir1/.. ==> /
// /..      ==> /
// /dir1/./dir2  ==> /dir1/dir2
// /dir1/../dir2 ==> /dir2
// /dir1/dir2/.. ==> /dir1/
// /dir1/dir2/dir3/../../.. ==> /
PRIVATE char *normalize_full_path__(char *abs_path, char *parent, char *child);
char *normalize_full_path(char *abs_path)
{
	// "/dir1/../????"
	//  ^
	normalize_full_path__(abs_path, abs_path, abs_path);
	remove_last_slash(abs_path);
	return abs_path;
}
// "/dir1/../????"
//   ^   ^
PRIVATE char *normalize_full_path__(char *abs_path, char *parent, char *child)
{
	char *grandchild;

////flf_d_printf(">[%s], [%s], [%s]\n", abs_path, parent, child);
	if (*parent == '/')
		parent++;
	if (*child == '/')
		child++;
	for ( ; is_file_path_char(child); ) {
////flf_d_printf("=[%s], [%s], [%s]\n", abs_path, parent, child);
		if (strcmp(child, ".") == 0) {
			// "/dir1/." ==> "/dir1/"
			// "/."      ==> "/"
			strlcpy__(child, child+1, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
		} else
		if (strlcmp__(child, "./") == 0) {
			// "/dir1/./????" ==> "/dir1/????"
			// "/./????"      ==> "/????"
			strlcpy__(child, child+2, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
			child = parent;
		} else
		if (strcmp(child, "..") == 0) {
			// "/dir1/.." ==> "/"
			// "/.."      ==> "/"
			strlcpy__(parent, child+2, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
			child = parent;
		} else
		if (strlcmp__(child, "../") == 0) {
			// "/dir1/../????" ==> "/????"
			// "/../????"      ==> "/????"
			strlcpy__(parent, child+3, MAX_PATH_LEN);
////flf_d_printf("*[%s]\n", parent);
			child = parent;
			if (abs_path+1 < child) {
				// not string top
				break;
			}
		} else {
			grandchild = skip_file_name(child);
			if (is_file_path_char(grandchild) == 0) {
				child = grandchild;
				break;
			}
			if (*grandchild == '/') {
				// "/dir1/../????"
				//   ^   ^
				child = normalize_full_path__(abs_path, child, grandchild);	// recursive call
////flf_d_printf("-[%s], [%s], [%s]\n", abs_path, parent, child);
			}
		}
	}
////flf_d_printf("<[%s]\n", child);
	return child;
}

//-----------------------------------------------------------------------------------

// get absolute path (not include symlinks)
PRIVATE char *get_full_path(const char *path, char *buf);

#ifdef START_UP_TEST
PRIVATE void test_get_full_path_(const char *path);
void test_get_full_path(void)
{
	flf_d_printf("-----------------------\n");
	test_get_full_path_("~");
	test_get_full_path_("~user");
	test_get_full_path_("~root");

	test_get_full_path_("~/abc");
	test_get_full_path_("~user/abc");
	test_get_full_path_("~root/abc");

	test_get_full_path_("abc");
	test_get_full_path_("./abc");
	test_get_full_path_("../abc");

	test_get_full_path_("aaa/./bbb");
	test_get_full_path_("./aaa/./bbb");
	test_get_full_path_("../aaa/./bbb");

	test_get_full_path_("aaa/../bbb");
	test_get_full_path_("./aaa/../bbb");
	test_get_full_path_("../aaa/../bbb");

	test_get_full_path_(" abc ");	// filename includes space in head or tail
}
PRIVATE void test_get_full_path_(const char *path)
{
	char full_path[MAX_PATH_LEN+1];

	get_full_path(path, full_path);
	flf_d_printf("path:[%s] ==> full_path:[%s]\n", path, full_path);
}
#endif // START_UP_TEST

//-----------------------------------------------------------------------------------

char *get_abs_path(const char *path, char *buf)
{
	char full_path[MAX_PATH_LEN+1];

	get_full_path(path, full_path);
	get_real_path(full_path, buf, MAX_PATH_LEN);
	return buf;
}

char *get_real_path(const char *path, char *buf, int buf_len)
{
#if defined(HAVE_REALPATH)
	return realpath__(path, buf, buf_len);
#else // HAVE_REALPATH
	return my_realpath(path, buf, buf_len);
#endif // HAVE_REALPATH
}

#if defined(HAVE_REALPATH)
#ifdef START_UP_TEST
PRIVATE void test_realpath_(const char *path);
void test_realpath(void)
{
	flf_d_printf("-----------------------\n");
	test_realpath_("~");
	test_realpath_("~user");
	test_realpath_("~root");

	test_realpath_("/home/user/tools/be/src");
	test_realpath_("/home/user/tools/./be/src");
	test_realpath_("/home/user/../user/tools/be/src");

	test_realpath_("/dev/stdin");
	test_realpath_("/dev/fd");
}
PRIVATE void test_realpath_(const char *path)
{
	char buf[MAX_PATH_LEN+1];

	realpath__(path, buf, MAX_PATH_LEN);
	flf_d_printf("path:[%s] ==> buf:[%s]\n", path, buf);
}
#endif // START_UP_TEST
// return normalized(canonicalized) absolute file path
char *realpath__(const char *path, char *buf, int buf_len)
{
	char buffer[MAX_PATH_LEN+1];

	if (realpath(path, buffer) == NULL) {
		strlcpy__(buf, path, buf_len);	// error, return original path
	} else {
		strlcpy__(buf, buffer, buf_len);
	}
////flf_d_printf("realpath__([%s]) ==> [%s]\n", path, buf);
	return buf;
}
#else // HAVE_REALPATH
char *my_realpath(const char *path, char *buf, int buf_len)
{
	strlcpy__(buf, path, buf_len);
	return normalize_full_path(buf);
}
#endif // HAVE_REALPATH

// get full path (path begins from "/") but do not resolve symlink
//                                            (may contain symlink)
// ~     ==> /home/user
// ~user ==> /home/user
// ~/tools/src/filename.ext     ==> /home/user/tools/src/filename.ext
// ~user/tools/src/filename.ext ==> /home/user/tools/src/filename.ext
// ./filename.ext               ==> /home/user/tools/src/filename.ext
PRIVATE char *get_full_path(const char *path, char *buf)
{
	char username[MAX_PATH_LEN+1];
	const char *userdir;
	char cur_dir[MAX_PATH_LEN+1];
	size_t len;
	const struct passwd *userdata = 0;

	strlcpy__(buf, path, MAX_PATH_LEN);
	if (path[0] == '/') {
		// "/..." already full path
	} else if (path[0] == '~') {
		// "~", "~user", "~/..." or "~user/..."
		for (len = 0; path[len]; len++) {
			if (path[len] == '/') {
				break;
			}
		}
		// Determine home directory using getpwuid() or getpwent(), don't rely on $HOME
		if (len == 1) {		// "~"
			userdata = getpwuid(geteuid());
			userdir = userdata->pw_dir;
		} else {			// "~user"
			strlcpy__(username, &path[1], len);
			userdir = get_user_home_dir(username);
///flf_d_printf("[%s] ==> [%s]\n", username, userdir);
		}
		if (path[len] == '/') {
			// "~/..."   "~/..."
			//   ^    ==>   ^
			len++;
		}
		cat_dir_and_file(buf, MAX_PATH_LEN, userdir, &path[len]);
	} else {
		// "./..." ==> "/home/user/tools/..."
		get_cur_dir(cur_dir);
		cat_dir_and_file(buf, MAX_PATH_LEN, cur_dir, path);
	}
///flf_d_printf("[%s] ==> [%s]\n", path, buf);
	normalize_full_path(buf);
///flf_d_printf("[%s] ==> [%s]\n", path, buf);
	return buf;
}

#ifdef START_UP_TEST
PRIVATE void get_file_name_extension_(char *file_name);
void test_get_file_name_extension(void)
{
	flf_d_printf("-----------------------\n");
	get_file_name_extension_("filename");
	get_file_name_extension_(".filename");
	get_file_name_extension_(".");
	get_file_name_extension_("filename.");
	get_file_name_extension_("filename.ext");
	get_file_name_extension_(".filename.ext");
}
PRIVATE void get_file_name_extension_(char *file_name)
{
	flf_d_printf("[%s] ==> [%s]\n", file_name, get_file_name_extension(file_name));
}
#endif // START_UP_TEST
// "filename.ext" ==> "ext"
// "filename" ==> ""
// "." ==> ""
// "filename." ==> ""
// ".filename" ==> "filename"
// ".filename.ext" ==> "ext"
char *get_file_name_extension(char *file_name)
{
	char *last_period = NULL;
	char *ptr;

	ptr = file_name;
	if (*ptr == '.') {
		// "." ==> ""
		// ".filename" ==> "filename"
		ptr++;
	}
	for ( ; *ptr; ptr++) {
		if (*ptr == '.') {
			last_period = ptr;
		}
	}
	if (last_period == NULL) {
		last_period = ptr;
	}
	if (*last_period == '.') {
		last_period++;
	}
	return last_period;
}

// End of utilpath.c
