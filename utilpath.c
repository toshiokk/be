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

// change direcotry independently from filer view
//   cur_path:      current directory before changing directory and after changing directory
//   prev_path:     current directory before changing directory
//   next_dir_sel:  directory to be pointed in filer after changing directory
int change_cur_dir_saving_prev_next_dir(char *path,
 char *cur_path, char *prev_path, char *next_dir_sel)
{
flf_d_printf("path: [%s]\n", path);
	char dir[MAX_PATH_LEN+1];

	if (is_path_regular_file(path) > 0) {
		// If path is pointing a file, change to the directory containing it.
		separate_path_to_dir_and_file(path, dir, next_dir_sel);
flf_d_printf("path: [%s]\n", path);
	} else {
		if (strcmp(path, ".") == 0) {
			return 1;	// OK
		} else if (strcmp(path, "..") == 0) {
			separate_path_to_dir_and_file(cur_path, dir, next_dir_sel);
		} else if (strcmp(path, "~") == 0) {
			strcpy__(next_dir_sel, "..");
			get_abs_path("~", dir);
		} else if (path[0] == '/') {
			// absolute path "/some/dir"
			strcpy__(next_dir_sel, "..");
			strlcpy__(dir, path, MAX_PATH_LEN);
		} else {
			// relative path "some/dir" or "./some/dir"
			strcpy__(next_dir_sel, "..");
			// "/cur/dir" + "some/dir" ==> "/cur/dir/some/dir"
flf_d_printf("cur_path: [%s], path: [%s]\n", cur_path, path);
			cat_dir_and_file(dir, cur_path, path);
		}
	}
flf_d_printf("dir: [%s]\n", dir);
	normalize_full_path(dir);
flf_d_printf("dir: [%s]\n", dir);
	if (is_dir_readable(dir) == 0) {
		// We can't open this dir for some reason. Complain.
		return 0;	// Error
	}
	strlcpy__(prev_path, cur_path, MAX_PATH_LEN);
	change_cur_dir(dir);
	get_full_path_of_cur_dir(cur_path);
	return 1;	// OK
}

//------------------------------------------------------------------------------

/* Strip one dir from the end of the string */
// /dir1/dir2/file ==> /dir1/dir2
// /dir1/dir2/     ==> /dir1/dir2
// /dir1           ==> /
// ""              ==> /
char *strip_file_from_path(char *path, char *dir)
{
	char file[MAX_PATH_LEN+1];
	if (dir == NULL) {
		separate_path_to_dir_and_file(path, path, file);
		return path;
	} else {
		separate_path_to_dir_and_file(path, dir, file);
		return dir;
	}
}

PRIVATE void test_separate_path_to_dir_and_file__(char *path, char *buf_dir, char *buf_file,
 const char *exp_dir, const char *exp_file);
void test_separate_path_to_dir_and_file()
{
	flf_d_printf("-----------------------\n");
	char buf_dir[MAX_PATH_LEN+1];
	char buf_file[MAX_PATH_LEN+1];
	test_separate_path_to_dir_and_file__("/dir/to/file", buf_dir, buf_file,
	 "/dir/to", "file");
	strcpy(buf_dir, "/dir/to/file");
	test_separate_path_to_dir_and_file__(buf_dir, buf_dir, buf_file,
	 "/dir/to", "file");
	strcpy(buf_dir, "dir/to/file");
	test_separate_path_to_dir_and_file__(buf_dir, buf_dir, buf_file,
	 "dir/to", "file");
	test_separate_path_to_dir_and_file__("", buf_dir, buf_file, ".", "");
	test_separate_path_to_dir_and_file__(".", buf_dir, buf_file, ".", "");
	test_separate_path_to_dir_and_file__("..", buf_dir, buf_file, "..", "");
	test_separate_path_to_dir_and_file__("...", buf_dir, buf_file, ".", "...");
	test_separate_path_to_dir_and_file__("/", buf_dir, buf_file, "/", "");
	test_separate_path_to_dir_and_file__("file", buf_dir, buf_file, ".", "file");
	test_separate_path_to_dir_and_file__("/file", buf_dir, buf_file, "/", "file");
	test_separate_path_to_dir_and_file__("./file", buf_dir, buf_file, ".", "file");
	test_separate_path_to_dir_and_file__("../file", buf_dir, buf_file, "..", "file");
	test_separate_path_to_dir_and_file__(".../file", buf_dir, buf_file, "...", "file");
}
PRIVATE void test_separate_path_to_dir_and_file__(char *path, char *buf_dir, char *buf_file,
 const char *exp_dir, const char *exp_file)
{
#define IS_EQ_STR(gotten, expected)		(strcmp(gotten, expected) == 0)
#define EQU_STR(gotten, expected)		(IS_EQ_STR(gotten, expected) ? '=' : '!')
	separate_path_to_dir_and_file(path, buf_dir, buf_file);
	if ((! IS_EQ_STR(buf_dir, exp_dir)) || (! IS_EQ_STR(buf_file, exp_file))) {
		flf_d_printf("path[%s]\n", path);
		flf_d_printf("  dir[%s]%c[%s], file[%s]%c[%s]\n",
		 buf_dir, EQU_STR(buf_dir, exp_dir), exp_dir,
		 buf_file, EQU_STR(buf_file, exp_file), exp_file);
	}
}

// path                buf_dir     buf_file
// /dir1/dir2/file ==> /dir1/dir2  file
// /dir1/dir2      ==> /dir1       dir2
// dir/file        ==> dir         file
// /file           ==> /           file
// file            ==> .           file
// /               ==> /           ""
// ""              ==> .           ""
// "."             ==> .           ""
// ".."            ==> ..          ""
// "..."           ==> .           "..."
// path and buf_dir can be the same address
char *separate_path_to_dir_and_file(char *path, char *buf_dir, char *buf_file)
{
	char *ptr = get_last_slash(path);
	if (*ptr != '/') {	// '/' not found
		if ((strcmp(path, ".") == 0) || (strcmp(path, "..") == 0)) {
			// "." ==> ".", ""
			//   ^
			// ".." ==> "..", ""
			//    ^
			ptr = &ptr[strlen(ptr)];
		} else {
			// "file" ==> ".", "file"
			//  ^
			// ".file" ==> ".", ".file"
			//  ^
		}
		strlcpy__(buf_dir, path, ptr - path);	// "." or ".." or ""
		strlcpy__(buf_file, ptr, MAX_PATH_LEN);	// "" or "file"
	} else {
		// '/' found
		if (ptr != path) {
			// "...dir/dir2/" ==> "...dir/dir2", ""
			//             ^
			// "...dir/file"  ==> "...dir", "file"
			//        ^
			strlcpy__(buf_dir, path, ptr - path);	// "...dir"
			ptr++;
			strlcpy__(buf_file, ptr, MAX_PATH_LEN);	// "" or "file"
		} else {
			// "/"     ==> "/", ""
			//  ^
			// "/file" ==> "/", "file"
			//  ^
			ptr++;
			strlcpy__(buf_dir, path, ptr - path);	// "/"
			strlcpy__(buf_file, ptr, MAX_PATH_LEN);	// "" or "file"
		}
	}
	if (is_strlen_0(buf_dir)) {
		strcpy__(buf_dir, ".");
	}
	remove_redundant_slash(buf_dir);
	return buf_file;		// return file_name
}

// /dir1/dir2/ ==> /dir1/dir2
// /           ==> ""
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

int contain_redundant_slash(char *path)
{
	return strstr(path, "//") != NULL;
}

// "////" ==> "/"
// "//dir//file" ==> "/dir/file"
char *remove_redundant_slash(char *path)
{
	char *ptr;
	for ( ; ; ) {
		if ((ptr = strstr(path, "//")) == NULL) {
			break;
		}
		memmove(ptr, ptr+1, strlen_path(ptr+1)+1);
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

/////int get_file_type_by_file_path(const char *file_path)
/////{
/////	if (is_path_exist(file_path) == 0) {
/////		return 0;	// not exist
/////	}
/////	if (is_path_regular_file(file_path) > 0) {
/////		return 1;	// regular file
/////	} else
/////	if (is_path_dir(file_path) > 0) {
/////		return 2;	// directory
/////	}
/////	return 3;		// not directory and regular file
/////}
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
	struct stat st;
	char *last_slash;

	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
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
	static char home_dir[MAX_PATH_LEN+1] = "";

	if (strlen_path(home_dir) == 0) {
		if (strlen(env_home = getenv__("HOME"))) {
			strlcpy__(home_dir, env_home, MAX_PATH_LEN);
		} else if ((userdata = getpwuid(geteuid())) != NULL) {
			strlcpy__(home_dir, userdata->pw_dir, MAX_PATH_LEN);
		} else {
			strcpy__(home_dir, "/");
		}
flf_d_printf("home_dir: [%s]\n", home_dir);
	}
	return home_dir;
}

const char *get_starting_dir(void)
{
	static char starting_dir[MAX_PATH_LEN+1] = "";

	if (strlen_path(starting_dir) == 0) {
		if (strlen_path(getenv_pwd(starting_dir)) == 0) {
			getcwd__(starting_dir);
		}
flf_d_printf("starting_dir: [%s]\n", starting_dir);
	}
	return starting_dir;
}

int check_wsl()
{
	static int checked = 0;
	if (! checked) {
		checked = (is_path_exist("/mnt/c") ? 1 : -1);
	}
	if (checked > 0) {
		flf_d_printf("WSL(Linux running on Windows)\n");
	} else {
		flf_d_printf("Native Linux\n");
	}
	return checked > 0;
}

//------------------------------------------------------------------------------
// comparison of change current dir functions
// | func                                     |save before change|may change to parent|
// |------------------------------------------|------------------|--------------------|
// | change_cur_dir_by_file_path_after_save   | Yes              | Yes                |
// | change_cur_dir_by_file_path              | No               | Yes                |
// | change_cur_dir_after_save                | Yes              | No                 |
// | change_cur_dir                           | No               | No                 |

int change_cur_dir_by_file_path_after_save(char *dir_save, char *file_path)
{
	char dir[MAX_PATH_LEN+1];
	strip_file_if_path_is_file(file_path, dir);
	return change_cur_dir_after_save(dir_save, dir);
}
int change_cur_dir_by_file_path(char *file_path)
{
	char dir[MAX_PATH_LEN+1];
	strip_file_if_path_is_file(file_path, dir);
	return change_cur_dir(dir);
}
char *strip_file_if_path_is_file(char *path, char *dir)
{
	if (is_path_regular_file(path) > 0) {
		strip_file_from_path(path, dir);
	} else {
		strlcpy__(dir, path, MAX_PATH_LEN);
	}
	return dir;
}
int change_cur_dir_after_save(char *dir_save, const char *dir)
{
	get_full_path_of_cur_dir(dir_save);
	return change_cur_dir(dir);
}

PRIVATE char full_path_of_cur_dir[MAX_PATH_LEN+1] = "";
PRIVATE char real_path_of_cur_dir[MAX_PATH_LEN+1] = "";
int change_cur_dir(const char *dir)
{
	int ret;

/////flf_d_printf("dir: [%s]\n", dir);
	if ((ret = chdir(dir)) == 0) {
		// update "full_path" and "real_path"
		strlcpy__(full_path_of_cur_dir, dir, MAX_PATH_LEN);
		getcwd__(real_path_of_cur_dir);
/////flf_d_printf("full_path_of_cur_dir: [%s]\n", full_path_of_cur_dir);
/////flf_d_printf("real_path_of_cur_dir: [%s]\n", real_path_of_cur_dir);
	}
	return ret;
}
char *get_full_path_of_cur_dir(char *dir)
{
	strlcpy__(dir, full_path_of_cur_dir, MAX_PATH_LEN);
	return dir;
}
char *get_real_path_of_cur_dir(char *dir)
{
	strlcpy__(dir, real_path_of_cur_dir, MAX_PATH_LEN);
	return dir;
}

int is_dir_readable(const char *path)
{
	DIR *dir;

	dir = opendir(path);
	// If dir is NULL, don't call closedir()
	if (dir)
		closedir(dir);
	return dir != NULL;
}

//------------------------------------------------------------------------------

// get real current directory(symbolic link is expanded to absolute path)
// NOTE: getcwd() returns real_path of the current directory
char *getcwd__(char *cwd)
{
	if (getcwd(cwd, MAX_PATH_LEN) == NULL) {
		strcpy__(cwd, "");
	}
/////flf_d_printf("getcwd: [%s]\n", cwd);
	return cwd;
}

// NOTE: "PWD" environment not automatically updated after changing current directory
//       so you can use this only for getting application startup directory
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

#ifdef START_UP_TEST
PRIVATE void test_cat_dir_and_file_(char *buf, const char *dir, const char *file,
 const char *expected);
void test_cat_dir_and_file()
{
	flf_d_printf("-----------------------\n");
	char buf[MAX_PATH_LEN+1];
	test_cat_dir_and_file_(buf, "/dir1/dir2", "/file", "/dir1/dir2/file");
	test_cat_dir_and_file_(buf, "/dir1/dir2/", "/file", "/dir1/dir2/file");
	strcpy(buf, "/dir1/dir2");
	test_cat_dir_and_file_(buf, buf, "/file", "/dir1/dir2/file");
	strcpy(buf, "/dir1/dir2/");
	test_cat_dir_and_file_(buf, buf, "/file", "/dir1/dir2/file");
}
PRIVATE void test_cat_dir_and_file_(char *buf, const char *dir, const char *file,
 const char *expected)
{
	cat_dir_and_file(buf, dir, file);
	if (! IS_EQ_STR(buf, expected)) {
		flf_d_printf("dir: [%s], file: [%s]\n", dir, file);
		flf_d_printf("  buf: [%s]%c[%s]\n", buf, EQU_STR(buf, expected), expected);
	}
}
#endif // START_UP_TEST

// Concatenate path and file
// "/dir1/dir2"   "file"       ==> "/dir1/dir2/file"
// "/dir1/dir2/"  "file"       ==> "/dir1/dir2/file"
// "/dir1/dir2/"  "./dir/file" ==> "/dir1/dir2/./dir/file"
// "dir1/dir2"    "file"       ==> "dir1/dir2/file"
// "/"            "file"       ==> "/file"
// ""             "file"       ==> "file"
// buf and dir can be the same address
char *cat_dir_and_file(char *buf, const char *dir, const char *file)
{
////flf_d_printf("dir: [%s], file: [%s]\n", dir, file);
	char tmp_buf[MAX_PATH_LEN+1];
	if (file[0] == '/') {
		// "/file" ==> "file"
		file++;
	}
	int last = LIM_MIN(0, strlen_path(dir) - 1);
	if (is_strlen_0(dir) || dir[last] == '/') {
///flf_d_printf("dir file [%s %s]\n", dir, file);
		// "/dir1/dir2/" + "file" ==> "/dir1/dir2/file"
		snprintf(tmp_buf, MAX_PATH_LEN+1, "%s%s", dir, file);
	} else {
///flf_d_printf("dir / file [%s / %s]\n", dir, file);
		// "/dir1/dir2" + "/" + "file" ==> "/dir1/dir2/file"
		snprintf(tmp_buf, MAX_PATH_LEN+1, "%s/%s", dir, file);
	}
////flf_d_printf("buf: [%s]\n", buf);
	strlcpy__(buf, tmp_buf, MAX_PATH_LEN);
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
// NOTE: Meaning of file_name, file_path, full_path, normalized_path and abs_path(real_path)
//  file_name          : Ex. "filename.ext"
//						 not contain directory
//  file_path          : Ex. "../src/filename.ext"
//						 may contain symlinks and ".."
//  full_path          : Ex. "/home/user/symlink/../src/filename.ext"
//						 start by '/' and may contain symlinks and ".."
//  normalized_path    : Ex. "/home/user/symlink/src/filename.ext"
//						 start by '/' and not contain ".." and may contain symlinks
//  abs_path(real_path): Ex. "/home/user/tools/src/filename.ext"
//						 start by '/' and not contain symlinks and ".."

// TODO: FULL_PATH can be converted to ABS_PATH
//       but ABS_PATH can not be converted to FULL_PATH
//       so you may keep FULL_PATH as long as possible, not replacing with ABS_PATH
// TODO: switch_epc_buf...() shall compare file path in FULL_PATH first and then comare in ABS_PATH
// TODO: memorize_file_pos() shall memorize file path as it is in FULL_PATH not in ABS_PATH
//       recall_file_pos() shall open file path as it is
//       goto_file_pos() shall open file path as it is

//-----------------------------------------------------------------------------------

#ifdef START_UP_TEST
void test_cwd_PWD()
{
	char buf[MAX_PATH_LEN+1];

	flf_d_printf("getcwd: [%s]\n", getcwd__(buf));
	flf_d_printf("getenv(PWD): [%s]\n", getenv_pwd(buf));

	change_cur_dir("..");
	flf_d_printf("getcwd: [%s]\n", getcwd__(buf));
	flf_d_printf("getenv(PWD): [%s]\n", getenv_pwd(buf));

	change_cur_dir("/home/user/tools/be/be/testfiles/symlinkd");
	flf_d_printf("getcwd: [%s]\n", getcwd__(buf));
	flf_d_printf("getenv(PWD): [%s]\n", getenv_pwd(buf));

	change_cur_dir(get_starting_dir());
	flf_d_printf("getcwd: [%s]\n", getcwd__(buf));
}
// /aaa/bbb/.. ==> /aaa
// /aaa/bbb/../ccc ==> /aaa/ccc
// /aaa/bbb/../../ccc ==> /ccc
// /../ccc ==> error
// /aaa/bbb/. ==> /aaa/bbb
// /aaa/bbb/./ccc ==> /aaa/bbb/ccc
// /. ==> /
PRIVATE const char *test_normalize_path_1(const char *path);
PRIVATE const char *test_normalize_path_2(const char *path);
PRIVATE const char *test_normalize_path_3(const char *path);
PRIVATE const char *test_normalize_path_4(const char *path);
PRIVATE const char *test_normalize_path__(const char *path);
int test_normalize_path(void)
{
	flf_d_printf("-----------------------\n");
	MY_UT_STR(test_normalize_path_1("///"), "/");
	MY_UT_STR(test_normalize_path_2("///"), "/");
	MY_UT_STR(test_normalize_path_3("///"), "/");
	MY_UT_STR(test_normalize_path_4("///"), "/");
	MY_UT_STR(test_normalize_path_1("///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_2("///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_3("///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_4("///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_1("."), "");
	MY_UT_STR(test_normalize_path_2("."), "/");
	MY_UT_STR(test_normalize_path_3("."), "");
	MY_UT_STR(test_normalize_path_4("."), "/");
	MY_UT_STR(test_normalize_path_1("././."), "");
	MY_UT_STR(test_normalize_path_2("././."), "/");
	MY_UT_STR(test_normalize_path_3("././."), "");
	MY_UT_STR(test_normalize_path_4("././."), "/");
	MY_UT_STR(test_normalize_path_1(".."), "");
	MY_UT_STR(test_normalize_path_2(".."), "/");
	MY_UT_STR(test_normalize_path_3(".."), "");
	MY_UT_STR(test_normalize_path_4(".."), "/");
	MY_UT_STR(test_normalize_path_1("../../.."), "");
	MY_UT_STR(test_normalize_path_2("../../.."), "/");
	MY_UT_STR(test_normalize_path_3("../../.."), "");
	MY_UT_STR(test_normalize_path_4("../../.."), "/");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/../ccc"), "aaa/ccc");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/../ccc"), "/aaa/ccc");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/../ccc"), "aaa/ccc");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/../ccc"), "/aaa/ccc");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/ccc/../../ddd"), "aaa/ddd");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/ccc/../../ddd"), "/aaa/ddd");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/ccc/../../ddd"), "aaa/ddd");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/ccc/../../ddd"), "/aaa/ddd");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/ccc/../../../ddd"), "ddd");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/ccc/../../../ddd"), "/ddd");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/ccc/../../../ddd"), "ddd");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/ccc/../../../ddd"), "/ddd");
	MY_UT_STR(test_normalize_path_1("../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_2("../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_3("../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_4("../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_1("../../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_2("../../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_3("../../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_4("../../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_1("../aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_2("../aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_3("../aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_4("../aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/."), "/aaa/bbb");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/."), "/aaa/bbb");
	MY_UT_STR(test_normalize_path_1("aaa/bbb/./ccc"), "aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_2("aaa/bbb/./ccc"), "/aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_3("aaa/bbb/./ccc"), "aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_4("aaa/bbb/./ccc"), "/aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_1("."), "");
	MY_UT_STR(test_normalize_path_2("."), "/");
	MY_UT_STR(test_normalize_path_3("."), "");
	MY_UT_STR(test_normalize_path_4("."), "/");
	MY_UT_STR(test_normalize_path_1("./aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_2("./aaa/bbb/."), "/aaa/bbb");
	MY_UT_STR(test_normalize_path_3("./aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_4("./aaa/bbb/."), "/aaa/bbb");
	return 0;
}
PRIVATE const char *test_normalize_path_1(const char *path)
{
	char buffer[MAX_PATH_LEN+1];
	snprintf(buffer, MAX_PATH_LEN, "%s", path);
	return test_normalize_path__(buffer);
}
PRIVATE const char *test_normalize_path_2(const char *path)
{
	char buffer[MAX_PATH_LEN+1];
	snprintf(buffer, MAX_PATH_LEN, "/%s", path);
	return test_normalize_path__(buffer);
}
PRIVATE const char *test_normalize_path_3(const char *path)
{
	char buffer[MAX_PATH_LEN+1];
	snprintf(buffer, MAX_PATH_LEN, "%s/", path);
	return test_normalize_path__(buffer);
}
PRIVATE const char *test_normalize_path_4(const char *path)
{
	char buffer[MAX_PATH_LEN+1];
	snprintf(buffer, MAX_PATH_LEN, "/%s/", path);
	return test_normalize_path__(buffer);
}
PRIVATE const char *test_normalize_path__(const char *path)
{
	static char buffer[MAX_PATH_LEN+1];
	strlcpy__(buffer, path, MAX_PATH_LEN);
	normalize_full_path(buffer);
	////flf_d_printf("[%s] ==> [%s]\n", path, buffer);
	return buffer;
}
#endif // START_UP_TEST

// /dir1/.  ==> /dir1/
// /dir1/.. ==> /
// /..      ==> /
// /dir1/./dir2  ==> /dir1/dir2
// /dir1/../dir2 ==> /dir2
// /dir1/dir2/.. ==> /dir1/
// /dir1/dir2/dir3/../../.. ==> /
PRIVATE char *normalize_full_path__(char *full_path, char *parent, char *child);
char *normalize_full_path(char *full_path)
{
	// "/dir1/../????"
	//  ^
	normalize_full_path__(full_path, full_path, full_path);
	remove_redundant_slash(full_path);
	remove_last_slash(full_path);
	return full_path;
}
// "/dir1/../????"
//   ^   ^
PRIVATE char *normalize_full_path__(char *full_path, char *parent, char *child)
{
	char *grandchild;

	if (*parent == '/')
		parent++;
	if (*child == '/')
		child++;
	for ( ; is_file_path_char(child); ) {
		if (strcmp(child, ".") == 0) {
			// "/dir1/." ==> "/dir1/"
			// "/."      ==> "/"
			strlcpy__(child, child+1, MAX_PATH_LEN);
		} else
		if (strlcmp__(child, "./") == 0) {
			// "/dir1/./????" ==> "/dir1/????"
			// "/./????"      ==> "/????"
			strlcpy__(child, child+2, MAX_PATH_LEN);
			child = parent;
		} else
		if (strcmp(child, "..") == 0) {
			// "/dir1/.." ==> "/"
			// "/.."      ==> "/"
			strlcpy__(parent, child+2, MAX_PATH_LEN);
			child = parent;
		} else
		if (strlcmp__(child, "../") == 0) {
			// "/dir1/../????" ==> "/????"
			// "/../????"      ==> "/????"
			strlcpy__(parent, child+3, MAX_PATH_LEN);
			child = parent;
			if (full_path+1 < child) {
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
				child = normalize_full_path__(full_path, child, grandchild);	// recursive call
			}
		}
	}
	return child;
}

//-----------------------------------------------------------------------------------

#ifdef START_UP_TEST
PRIVATE void test_get_full_path_(const char *path);
void test_get_full_path(void)
{
	flf_d_printf("-----------------------\n");
	test_get_full_path_("~");
	test_get_full_path_("~user");
	test_get_full_path_("~root");
	test_get_full_path_("~hoge");
	test_get_full_path_("~hoge/abc");

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

// get absolute path (not include symlinks)
char *get_abs_path(const char *path, char *buf)
{
	char full_path[MAX_PATH_LEN+1];

	get_full_path(path, full_path);					// --> full_path
	get_real_path(full_path, buf, MAX_PATH_LEN);	// --> abs path (real path)
///flf_d_printf("\n[%s]\n ==> [%s]\n", full_path, buf);
	return buf;
}

// get full path (path begins from "/") but do not resolve symlink
//                                            (may contain symlink)
// ~     ==> /home/user
// ~user ==> /home/user
// ~/tools/src/filename.ext     ==> /home/user/tools/src/filename.ext
// ~user/tools/src/filename.ext ==> /home/user/tools/src/filename.ext
// ./filename.ext               ==> /home/user/tools/src/filename.ext
char *get_full_path(const char *path, char *buf)
{
	char user_name[MAX_PATH_LEN+1];
	const char *user_dir;
	char cur_dir[MAX_PATH_LEN+1];
	size_t len;
	const struct passwd *user_data = 0;

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
			user_data = getpwuid(geteuid());
			user_dir = user_data->pw_dir;
		} else {			// "~user"
			strlcpy__(user_name, &path[1], len);
			user_dir = get_user_home_dir(user_name);
///flf_d_printf("[%s] ==> [%s]\n", user_name, user_dir);
		}
		if (path[len] == '/') {
			// "~/..."   "~/..."
			//   ^    ==>   ^
			len++;
		}
		cat_dir_and_file(buf, user_dir, &path[len]);
	} else {
		// "./..." ==> "/home/user/tools/..."
		get_full_path_of_cur_dir(cur_dir);
		cat_dir_and_file(buf, cur_dir, path);
	}
///flf_d_printf("[%s] ==> [%s]\n", path, buf);
	normalize_full_path(buf);
///flf_d_printf("[%s] ==> [%s]\n", path, buf);
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

	test_realpath_("/home/user/tools/be/be");
	test_realpath_("/home/user/tools/./be/be");
	test_realpath_("/home/user/../user/tools/be/be");

	test_realpath_("/home/user/tools/be/be/testfiles/symlinkd");
	test_realpath_("/home/user/tools/be/be/testfiles/symlinkf");

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
		strlcpy__(buffer, path, MAX_PATH_LEN);	// error, return original path
	}
	strlcpy__(buf, buffer, buf_len);
	return buf;
}
#endif // HAVE_REALPATH

#if !defined(HAVE_REALPATH) || defined(START_UP_TEST)
#warning "my_realpath() is only partially implemented"

PRIVATE void test_my_realpath_(const char *path);
void test_my_realpath(void)
{
	flf_d_printf("-----------------------\n");
	test_my_realpath_("~");
	test_my_realpath_("~user");
	test_my_realpath_("~root");

	test_my_realpath_("/home/user/tools/be/be");
	test_my_realpath_("/home/user/tools/./be/be");
	test_my_realpath_("/home/user/../user/tools/be/be");

	test_my_realpath_("/home/user/tools/be/be/testfiles/symlinkd");
	test_my_realpath_("/home/user/tools/be/be/testfiles/symlinkf");

	test_my_realpath_("/dev/stdin");
	test_my_realpath_("/dev/fd");
}
PRIVATE void test_my_realpath_(const char *path)
{
	char buf[MAX_PATH_LEN+1];

	my_realpath(path, buf, MAX_PATH_LEN);
	flf_d_printf("path:[%s] ==> buf:[%s]\n", path, buf);
}
char *my_realpath(const char *path, char *buf, int buf_len)
{
	strlcpy__(buf, path, buf_len);
	return normalize_full_path(buf);
}
#endif // !defined(HAVE_REALPATH) || defined(START_UP_TEST)

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
