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

#define IS_EQ_STR(gotten, expected)		(strcmp(gotten, expected) == 0)
#define EQU_STR(gotten, expected)		(IS_EQ_STR(gotten, expected) ? '=' : '!')

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
const char *get_home_dir(void)
{
	static char home_dir[MAX_PATH_LEN+1] = "";
	if (strlen_path(home_dir) == 0) {
		char *env_home;
		const struct passwd *userdata = 0;
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
const char *get_tty_name(void)
{
	static char tty_name[MAX_PATH_LEN+1] = "";
	if (strlen_path(tty_name) == 0) {
		strlcpy__(tty_name, ttyname(0), MAX_PATH_LEN);	// /dev/pts/99
		if (strlen_path(tty_name) == 0) {
			strlcpy__(tty_name, cur_hhmmss(), MAX_PATH_LEN);
		}
flf_d_printf("tty_name: [%s]\n", tty_name);
	}
	return tty_name;
}

int check_wsl()
{
	static int checked = 0;
	if (! checked) {
		checked = (is_path_exist("/mnt/c") ? 1 : -1);
		if (checked > 0) {
			flf_d_printf("WSL(Linux running on Windows)\n");
		} else {
			flf_d_printf("Native Linux\n");
		}
	}
	return checked > 0;
}
int check_availability_of_script()
{
	static int checked = 0;
	if (! checked) {
		checked = (is_path_exist("/usr/bin/script") ? 1 : -1);
		if (checked > 0) {
			flf_d_printf("'script' is available\n");
		} else {
			flf_d_printf("'script' is unavailable\n");
		}
	}
	return checked > 0;
}

// change direcotry independently from filer view
//   cur_path:      current directory before changing directory and after changing directory
//   prev_path:     current directory before changing directory
//   next_dir_sel:  directory to be selected in filer after changing directory
int change_cur_dir_saving_prev_next_dir(const char *path,
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
		// We can't open this dir for some reason.
		return 0;	// Error
	}
	strlcpy__(prev_path, cur_path, MAX_PATH_LEN);
	change_cur_dir(dir);
	get_full_path_of_cur_dir(cur_path);
	return 1;	// OK
}

//------------------------------------------------------------------------------
// comparison of change current dir functions
// | func                                   |save before change|try parent|
// |----------------------------------------|------------------|----------|
// | change_cur_dir_by_file_path_after_save | Yes              | Yes      |
// | change_cur_dir_by_file_path            | No               | Yes      |
// | change_cur_dir_after_save              | Yes              | No       |
// | change_cur_dir                         | No               | No       |

int change_cur_dir_by_file_path_after_save(char *dir_save, const char *file_path)
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
int change_cur_dir_after_save(char *dir_save, const char *dir)
{
	get_full_path_of_cur_dir(dir_save);
	return change_cur_dir(dir);
}

char *strip_file_if_path_is_file(const char *path, char *dir)
{
	if (is_path_regular_file(path) > 0) {
		strip_file_from_path(path, dir);
	} else {
		strlcpy__(dir, path, MAX_PATH_LEN);
	}
	return dir;
}

/* Strip one dir from the end of the string */
// /dir1/dir2/file ==> /dir1/dir2
// /dir1/dir2/     ==> /dir1/dir2
// /dir1           ==> /
// ""              ==> /
// path and dir can be the same address
char *strip_file_from_path(const char *path, char *dir)
{
	static char dir_[MAX_PATH_LEN+1];
	char file[MAX_PATH_LEN+1];
	if (dir == NULL) {
		dir = dir_;
	}
	separate_path_to_dir_and_file(path, dir, file);
	return dir;
}

// "filename.ext" ==> "ext"
// "filename" ==> ""
// "." ==> ""
// "filename." ==> ""
// ".filename" ==> "filename"
// ".filename.ext" ==> "ext"
char *get_file_name_extension(char *file_name)
{
	char *last_period = NULL;
	char *ptr = file_name;
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
char *separate_path_to_dir_and_file(const char *path, char *buf_dir, char *buf_file)
{
	char buf_path[MAX_PATH_LEN+1];
	strlcpy__(buf_path, path, MAX_PATH_LEN);
	const char *ptr = get_last_slash(buf_path);
	if (*ptr != '/') {	// '/' not found
		if ((strcmp(buf_path, ".") == 0) || (strcmp(buf_path, "..") == 0)) {
			// "." ==> ".", ""
			//   ^
			// ".." ==> "..", ""
			//    ^
			ptr = skip_string_const(ptr);
			strlcpy__(buf_dir, buf_path, ptr - buf_path);	// "." or ".."
		} else {
			// "file" ==> ".", "file"
			//  ^
			// ".file" ==> ".", ".file"
			//  ^
			strcpy__(buf_dir, ".");						// "."
		}
		strlcpy__(buf_file, ptr, MAX_PATH_LEN);			// "" or "file"
	} else {
		// '/' found
		if (ptr == buf_path) {
			// "/"     ==> "/", ""
			//  ^
			// "/file" ==> "/", "file"
			//  ^
			ptr++;
			strlcpy__(buf_dir, buf_path, ptr - buf_path);	// "/"
			strlcpy__(buf_file, ptr, MAX_PATH_LEN);	// "" or "file"
		} else {
			// "...dir1/dir2/" ==> "...dir1/dir2", ""
			//             ^
			// "...dir/file"  ==> "...dir", "file"
			//        ^
			strlcpy__(buf_dir, buf_path, ptr - buf_path);	// "...dir"
			ptr++;
			strlcpy__(buf_file, ptr, MAX_PATH_LEN);	// "" or "file"
		}
	}
	if (is_strlen_0(buf_dir)) {
		strcpy__(buf_dir, ".");
	}
	remove_redundant_slash(buf_dir);
	return buf_file;		// return file_name
}

// "/dir1/dir2/" ==> "/dir1/dir2"
// "/"           ==> "/"
char *remove_last_slash(char *path)
{
	char *ptr = get_last_slash(path);
	if (path < ptr && ptr[0] == '/' && ptr[1] == '\0') {
		*ptr = '\0';
	}
	return path;
}

char *add_last_slash_to_dir(char *dir)
{
	if (strcmp(dir, "/") != 0 && strlen(dir) && dir[strlen(dir) - 1] != '/') {
		strlcat__(dir, MAX_PATH_LEN, "/");
	}
	return dir;
}

int contain_redundant_slash(char *path)
{
	return strstr(path, "//") != NULL;
}

// "//" ==> "/"
// "///" ==> "/"
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
	char *ptr = strrchr(path, '/');
	if (ptr == NULL)
		ptr = path;
	return ptr;
}

int is_path_exist(const char *path)
{
	struct stat st;
	return stat(path, &st) == 0;	// 1: exists, 0: does not exist
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
		return (st->st_mode & S_IWUSR) != 0;	// 1: writable, 0: non-writable
	} else if (st->st_gid == getegid()) {
		return (st->st_mode & S_IWGRP) != 0;
	}
	return (st->st_mode & S_IWOTH) != 0;
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
	if (dir)
		closedir(dir);
	return dir != NULL;		// 1: readable, 0: non-readable
}

// dest: "/home/user/tools/be/file_name.ext"
// src : "/home/user/tools/be/file_name.ext" --> match
// src :  "home/user/tools/be/file_name.ext" --> match
// src :            "tools/be/file_name.ext" --> match
// src :             "ools/be/file_name.ext" --> not match
// src :                     "file_name.ext" --> match
// src :                          "name.ext" --> not match
// src :                              ".ext" --> not match
int compare_file_path_from_tail(const char *full_path, const char *file_path) {
	if ((strcmp_from_tail(full_path, file_path) == 0)
	 && (strlen_path(full_path) > strlen_path(file_path))
	 && (full_path[strlen_path(full_path) - strlen_path(file_path) - 1] == '/')) {
		return 0;	// match
	}
	return -1;		// not match
}

#ifdef ENABLE_FILER
int is_path_wildcard(char *path)
{
#ifdef ENABLE_DEBUG
	char *ptr = strchr(path, '*');
	if (ptr)
		flf_d_printf("strchr(path, '*') ==> [%s]\n", ptr);
	ptr = strchr(path, '?');
	if (ptr)
		flf_d_printf("strchr(path, '?') ==> [%s]\n", ptr);
#endif // ENABLE_DEBUG
	return strchr(path, '*') || strchr(path, '?');
}
#endif // ENABLE_FILER

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
	getcwd__(real_path_of_cur_dir);
	return 1;			// 1: changed
}
char *get_full_path_of_cur_dir(char *dir)
{
	static char dir_[MAX_PATH_LEN+1];
	if (dir == NULL) {
		dir = dir_;
	}
	strlcpy__(dir, full_path_of_cur_dir, MAX_PATH_LEN);
	return dir;
}
const char *full_path_of_cur_dir_s()
{
	return full_path_of_cur_dir;
}
char *get_real_path_of_cur_dir(char *dir)
{
	strlcpy__(dir, real_path_of_cur_dir, MAX_PATH_LEN);
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
	return cwd;
}

// NOTE: "PWD" environment is not automatically updated after changing current directory
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
	char tmp_buf[MAX_PATH_LEN+1];
	if (file[0] == '/') {
		// "/file" ==> "file"
		file++;
	}
	int last = LIM_MIN(0, strlen_path(dir) - 1);
	if (is_strlen_0(dir) || dir[last] == '/') {
		// ""            + "file" ==> "file"
		// "/dir1/dir2/" + "file" ==> "/dir1/dir2/file"
		snprintf(tmp_buf, MAX_PATH_LEN+1, "%s%s", dir, file);
	} else {
		// "/dir1/dir2" + "/" + "file" ==> "/dir1/dir2/file"
		snprintf(tmp_buf, MAX_PATH_LEN+1, "%s/%s", dir, file);
	}
	strlcpy__(buf, tmp_buf, MAX_PATH_LEN);
	return buf;
}

//------------------------------------------------------------------------------
// NOTE: Meaning of file_name, file_path, full_path, normalized_path and abs_path(real_path)
//  file_name          : e.g. "filename.ext"
//						 not contain directory
//  file_path          : e.g. "../src/filename.ext"
//						 may be relative_path and contain symlinks and ".."
//  full_path          : e.g. "/home/user/symlink/../src/filename.ext"
//						 start by '/' and may contain symlinks and ".."
//  normalized_path    : e.g. "/home/user/symlink/src/filename.ext"
//						 start by '/' and not contain ".." and may contain symlinks
//  abs_path(real_path): e.g. "/home/user/tools/src/filename.ext"
//						 start by '/' and not contain symlinks and ".."

// TODO: FULL_PATH can be converted to ABS_PATH
//       but ABS_PATH can not be converted to FULL_PATH
//       so you may keep FULL_PATH as long as possible, not replacing with ABS_PATH
// TODO: switch_epc_buf...() shall compare file path in FULL_PATH first and in ABS_PATH next
// TODO: memorize_file_pos() shall memorize file path as it is in FULL_PATH not in ABS_PATH
//       recall_file_pos() shall open file path as it is
//       goto_file_pos() shall open file path as it is

//------------------------------------------------------------------------------

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
	for ( ; is_char_file_path(child); ) {
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
			if (is_char_file_path(grandchild) == 0) {
				child = grandchild;
				break;
			}
			if (*grandchild == '/') {
				// "/dir1/../????"
				//   ^   ^
				// recursive call
				child = normalize_full_path__(full_path, child, grandchild);
			}
		}
	}
	return child;
}

//------------------------------------------------------------------------------

int compare_file_path_in_abs_path(const char *file_path_a, const char *file_path_b)
{
	char abs_path_a[MAX_PATH_LEN+1];
	char abs_path_b[MAX_PATH_LEN+1];
	return strcmp(get_abs_path(file_path_a, abs_path_a), get_abs_path(file_path_b, abs_path_b));
}

// get absolute path (not include symlinks)
char *get_abs_path(const char *path, char *buf)
{
	char full_path[MAX_PATH_LEN+1];
	get_full_path(path, full_path);			// --> full_path
	get_real_path(full_path, buf);			// --> abs path (real path)
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
	if (is_abs_path(path)) {
		// "/..." already full path
		strlcpy__(buf, path, MAX_PATH_LEN);
	} else if (path[0] == '~') {
		// "~", "~user", "~/..." or "~user/..."
		size_t len;
		for (len = 0; path[len]; len++) {
			if (path[len] == '/') {
				break;
			}
		}
		// Determine home directory using getpwuid() or getpwent(), don't rely on $HOME
		const char *user_dir;
		if (len == 1) {		// "~"
			const struct passwd *user_data = 0;
			user_data = getpwuid(geteuid());
			user_dir = user_data->pw_dir;
		} else {			// "~user"
			char user_name[MAX_PATH_LEN+1];
			strlcpy__(user_name, &path[1], len);
			user_dir = get_user_home_dir(user_name);
		}
		if (path[len] == '/') {
			// "~/..."   "~/..."
			//   ^    ==>   ^
			len++;
		}
		cat_dir_and_file(buf, user_dir, &path[len]);
	} else {
		// "filename..."   ==> "/home/user/tools/..."
		// "./filename..." ==> "/home/user/tools/..."
		cat_dir_and_file(buf, full_path_of_cur_dir_s(), path);
	}
	normalize_full_path(buf);
	return buf;
}

char *get_real_path(const char *path, char *buf)
{
#if defined(HAVE_REALPATH)
	return realpath__(path, buf, MAX_PATH_LEN);
#else // HAVE_REALPATH
#error "HAVE_REALPATH not defined"
#endif // HAVE_REALPATH
}

#if defined(HAVE_REALPATH)
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

// read link and null terminate string
int readlink__(const char *path, char *buffer, int len)
{
	int ret;
	if ((ret = readlink(path, buffer, len)) > 0)
		buffer[ret] = '\0';
	return ret;
}

BOOL is_abs_path(const char *path)
{
	return path[0] == '/';
}

//------------------------------------------------------------------------------
int write_text_to_file(const char *file_path, char append, const char *text)
{
	FILE *fp;
	if ((fp = fopen(file_path, append ? "a" : "w")) != NULL) {
		fputs(text, fp);
		if (fclose(fp) != 0) {
			return -1;
		}
		return 0;
	}
	return -1;
}

int remove_file(const char* file_path)
{
	return remove(file_path);
}

// tests ==============================

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
	flf_d_printf("getenv(PWD): [%s]\n", getenv_pwd(buf));
}

PRIVATE void test_cat_dir_and_file_(char *buf, const char *dir, const char *file,
 const char *expected);
void test_cat_dir_and_file()
{
	flf_d_printf("-----------------------\n");
	char buf[MAX_PATH_LEN+1];
	test_cat_dir_and_file_(buf, "/dir1/dir2", "/file", "/dir1/dir2/file");
	test_cat_dir_and_file_(buf, "/dir1/dir2/", "/file", "/dir1/dir2/file");
	strcpy__(buf, "/dir1/dir2");
	test_cat_dir_and_file_(buf, buf, "/file", "/dir1/dir2/file");
	strcpy__(buf, "/dir1/dir2/");
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
// /aaa/bbb/.. ==> /aaa
// /aaa/bbb/../ccc ==> /aaa/ccc
// /aaa/bbb/../../ccc ==> /ccc
// /../ccc ==> error
// /aaa/bbb/. ==> /aaa/bbb
// /aaa/bbb/./ccc ==> /aaa/bbb/ccc
// /. ==> /
PRIVATE const char *test_normalize_path_(const char *templ, const char *path);
int test_normalize_path(void)
{
	flf_d_printf("-----------------------\n");
	MY_UT_STR(test_normalize_path_("%s", "///"), "/");
	MY_UT_STR(test_normalize_path_("/%s", "///"), "/");
	MY_UT_STR(test_normalize_path_("%s/", "///"), "/");
	MY_UT_STR(test_normalize_path_("/%s/", "///"), "/");
	MY_UT_STR(test_normalize_path_("%s", "///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_("/%s", "///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_("%s/", "///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_("/%s/", "///dir///file///"), "/dir/file");
	MY_UT_STR(test_normalize_path_("%s", "."), "");
	MY_UT_STR(test_normalize_path_("/%s", "."), "/");
	MY_UT_STR(test_normalize_path_("%s/", "."), "");
	MY_UT_STR(test_normalize_path_("/%s/", "."), "/");
	MY_UT_STR(test_normalize_path_("%s", "././."), "");
	MY_UT_STR(test_normalize_path_("/%s", "././."), "/");
	MY_UT_STR(test_normalize_path_("%s/", "././."), "");
	MY_UT_STR(test_normalize_path_("/%s/", "././."), "/");
	MY_UT_STR(test_normalize_path_("%s", ".."), "");
	MY_UT_STR(test_normalize_path_("/%s", ".."), "/");
	MY_UT_STR(test_normalize_path_("%s/", ".."), "");
	MY_UT_STR(test_normalize_path_("/%s/", ".."), "/");
	MY_UT_STR(test_normalize_path_("%s", "../../.."), "");
	MY_UT_STR(test_normalize_path_("/%s", "../../.."), "/");
	MY_UT_STR(test_normalize_path_("%s/", "../../.."), "");
	MY_UT_STR(test_normalize_path_("/%s/", "../../.."), "/");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/../ccc"), "aaa/ccc");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/../ccc"), "/aaa/ccc");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/../ccc"), "aaa/ccc");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/../ccc"), "/aaa/ccc");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/ccc/../../ddd"), "aaa/ddd");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/ccc/../../ddd"), "/aaa/ddd");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/ccc/../../ddd"), "aaa/ddd");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/ccc/../../ddd"), "/aaa/ddd");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/ccc/../../../ddd"), "ddd");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/ccc/../../../ddd"), "/ddd");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/ccc/../../../ddd"), "ddd");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/ccc/../../../ddd"), "/ddd");
	MY_UT_STR(test_normalize_path_("%s", "../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_("/%s", "../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_("%s/", "../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_("/%s/", "../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_("%s", "../../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_("/%s", "../../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_("%s/", "../../../ccc"), "ccc");
	MY_UT_STR(test_normalize_path_("/%s/", "../../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_path_("%s", "../aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_("/%s", "../aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_("%s/", "../aaa/bbb/.."), "aaa");
	MY_UT_STR(test_normalize_path_("/%s/", "../aaa/bbb/.."), "/aaa");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/."), "/aaa/bbb");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/."), "/aaa/bbb");
	MY_UT_STR(test_normalize_path_("%s", "aaa/bbb/./ccc"), "aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_("/%s", "aaa/bbb/./ccc"), "/aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_("%s/", "aaa/bbb/./ccc"), "aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_("/%s/", "aaa/bbb/./ccc"), "/aaa/bbb/ccc");
	MY_UT_STR(test_normalize_path_("%s", "."), "");
	MY_UT_STR(test_normalize_path_("/%s", "."), "/");
	MY_UT_STR(test_normalize_path_("%s/", "."), "");
	MY_UT_STR(test_normalize_path_("/%s/", "."), "/");
	MY_UT_STR(test_normalize_path_("%s", "./aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_("/%s", "./aaa/bbb/."), "/aaa/bbb");
	MY_UT_STR(test_normalize_path_("%s/", "./aaa/bbb/."), "aaa/bbb");
	MY_UT_STR(test_normalize_path_("/%s/", "./aaa/bbb/."), "/aaa/bbb");
	return 0;
}
PRIVATE const char *test_normalize_path_(const char *templ, const char *path)
{
	static char buffer[MAX_PATH_LEN+1];
	snprintf(buffer, MAX_PATH_LEN, templ, path);
	normalize_full_path(buffer);
	return buffer;
}

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
}

#if defined(HAVE_REALPATH)
PRIVATE const char *test_realpath_(const char *path);
void test_realpath(void)
{
	flf_d_printf("-----------------------\n");
	MY_UT_STR(test_realpath_("~"), "~");
	MY_UT_STR(test_realpath_("~user"), "~user");
	MY_UT_STR(test_realpath_("~root"), "~root");

	test_realpath_("/home/user/tools/be/be");
	test_realpath_("/home/user/tools/./be/be");
	test_realpath_("/home/user/../user/tools/be/be");

	test_realpath_("/home/user/tools/be/be/testfiles/symlinkd");
	test_realpath_("/home/user/tools/be/be/testfiles/symlinkf");

	////MY_UT_STR(test_realpath_("/dev/stdin"), "/proc/self/fd/0");
	////MY_UT_STR(test_realpath_("/dev/fd"), "/proc/self/fd");
}
PRIVATE const char *test_realpath_(const char *path)
{
	static char buf[MAX_PATH_LEN+1];

	realpath__(path, buf, MAX_PATH_LEN);
	return buf;
}
#endif // HAVE_REALPATH

PRIVATE const char *get_file_name_extension_(char *file_name);
void test_get_file_name_extension(void)
{
	flf_d_printf("-----------------------\n");
	MY_UT_STR(get_file_name_extension_("filename"), "");
	MY_UT_STR(get_file_name_extension_(".filename"), "");
	MY_UT_STR(get_file_name_extension_("."), "");
	MY_UT_STR(get_file_name_extension_("filename."), "");
	MY_UT_STR(get_file_name_extension_("filename.ext"), "ext");
	MY_UT_STR(get_file_name_extension_(".filename.ext"), "ext");
}
PRIVATE const char *get_file_name_extension_(char *file_name)
{
	return get_file_name_extension(file_name);
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
	strcpy__(buf_dir, "/dir/to/file");
	test_separate_path_to_dir_and_file__(buf_dir, buf_dir, buf_file,
	 "/dir/to", "file");
	strcpy__(buf_dir, "dir/to/file");
	test_separate_path_to_dir_and_file__(buf_dir, buf_dir, buf_file, "dir/to", "file");
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
	separate_path_to_dir_and_file(path, buf_dir, buf_file);
	if ((! IS_EQ_STR(buf_dir, exp_dir)) || (! IS_EQ_STR(buf_file, exp_file))) {
		flf_d_printf("path[%s]\n", path);
		flf_d_printf("  dir[%s]%c[%s], file[%s]%c[%s]\n",
		 buf_dir, EQU_STR(buf_dir, exp_dir), exp_dir,
		 buf_file, EQU_STR(buf_file, exp_file), exp_file);
	}
}

#endif // START_UP_TEST

// End of utilpath.c
