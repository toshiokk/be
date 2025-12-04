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
#define EQU_STR(gotten, expected)		(IS_EQ_STR(gotten, expected) ? "===" : "!!!")

PRIVATE const char *get_trailing_slash_in_dir_path(const char *path);

const char *get_starting_dir()
{
	static char starting_dir[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(starting_dir)) {		// prepare on-demand
		if (is_strlen_0(getenv_pwd(starting_dir))) {
			getcwd__(starting_dir);
		}
		add_trailing_slash_in_handling(starting_dir);
flf_dprintf("starting_dir: [%s]\n", starting_dir);
	}
	return starting_dir;
}
const char *get_home_dir()
{
	static char home_dir[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(home_dir)) {		// prepare on-demand
		char *env_home;
		const struct passwd *userdata = 0;
		if (strlen(env_home = getenv__("HOME"))) {
			strlcpy__(home_dir, env_home, MAX_PATH_LEN);
		} else if ((userdata = getpwuid(geteuid())) != NULL) {
			strlcpy__(home_dir, userdata->pw_dir, MAX_PATH_LEN);
		} else {
			strcpy__(home_dir, "/");
		}
		add_trailing_slash_in_handling(home_dir);
flf_dprintf("home_dir: [%s]\n", home_dir);
	}
	return home_dir;
}
const char *get_tty_name()
{
	static char tty_name[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(tty_name)) {		// prepare on-demand
		strlcpy__(tty_name, ttyname(0), MAX_PATH_LEN);	// /dev/pts/99
		if (is_strlen_0(tty_name)) {
			strlcpy__(tty_name, cur_hhmmss(), MAX_PATH_LEN);
		}
flf_dprintf("tty_name: [%s]\n", tty_name);
	}
	return tty_name;
}
const char *get_tty_name_file_part()
{
	char dir[MAX_PATH_LEN+1];
	static char file[MAX_PATH_LEN+1];
	separate_path_to_dir_and_file(get_tty_name(), dir, file);
	return file;
}

int check_wsl()
{
	static int checked = 0;		// 0: not yet checked
	if (! checked) {		// prepare on-demand
		checked = (is_path_exist("/mnt/c") ? 1 : -1);
		if (checked > 0) {
			flf_dprintf("WSL(Linux running on Windows)\n");
		} else {
			flf_dprintf("Native Linux\n");
		}
	}
	return checked > 0;
}
int check_availability_of_script()
{
	static int checked = 0;		// 0: not yet checked
	if (! checked) {		// prepare on-demand
		checked = ((is_path_executable_file("/usr/bin/script") > 0) ? 1 : -1);
		if (checked > 0) {
			flf_dprintf("'script' is available\n");
		} else {
			flf_dprintf("'script' is unavailable\n");
		}
	}
	return checked > 0;
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
	if (dir == NULL) {
		dir = dir_;
	}
	separate_path_to_dir_and_file(path, dir, NULL);
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
// file            ==> ""          file
// /               ==> /           ""
// ""              ==> ""          ""
// "."             ==> .           ""
// ".."            ==> ..          ""
// "..."           ==> ""          "..."
// path and buf_dir can be the same address
// buf_file can be a NULL
char *separate_path_to_dir_and_file(const char *path, char *buf_dir, char *buf_file)
{
	char buf_path[MAX_PATH_LEN+1];
	remove_trailing_slash(path, buf_path);
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
			strcpy__(buf_dir, "");						// "."
		}
	} else {
		// '/' found
		if (ptr == buf_path) {
			ptr++;
			// "/"     ==> "/", ""
			//   ^
			// "/file" ==> "/", "file"
			//   ^
			strlcpy__(buf_dir, buf_path, ptr - buf_path);	// "/"
		} else {
			ptr++;
			// "...dir/dir2" ==> "...dir/", "dir2"
			//         ^
			// "...dir/file" ==> "...dir/", "file"
			//         ^
			strlcpy__(buf_dir, buf_path, ptr - buf_path);	// "...dir/"
		}
	}
	add_trailing_slash_in_handling(buf_dir);		// "." ==> "./", ".." ==> "../"
	remove_redundant_slash(buf_dir);
	if (buf_file) {
		strlcpy__(buf_file, ptr, MAX_PATH_LEN);		// "" or "file"
	}
	return buf_file;		// return file_name
}

// point to the last '/'
// /dir1/dir2/file    /dir1/dir2/    file
//           ^                  ^        ^
char *get_last_slash(char *path)
{
	char *ptr = strrchr(path, '/');
	if (ptr == NULL)
		ptr = &path[strlen_path(path)];
	return ptr;
}

int contain_redundant_slash(char *path)
{
	return strstr(path, "//") != NULL;
}

// "//" ==> "/"
// "///" ==> "/"
// "////" ==> "/"
// "//dir//file" ==> "/dir/file"
// "//dir//dir2//" ==> "/dir/dir2/"
char *remove_redundant_slash(char *path)
{
	for ( ; ; ) {
		char *ptr;
		if ((ptr = strstr(path, "//")) == NULL) {
			break;
		}
		memmove(ptr, ptr+1, strlen_path(ptr+1)+1);	// "//" ==> "/"
	}
	return path;
}

// compare directory path with or without a last slash
// /path/to/dir
// /path/to/dir/
int compare_dir_path_w_or_wo_trailing_slash(const char *dir1, const char *dir2)
{
	// compare with zero copy
	size_t len1 = get_trailing_slash_in_dir_path(dir1) - dir1;
	size_t len2 = get_trailing_slash_in_dir_path(dir2) - dir2;
	return strn_strn_cmp__(dir1, len1, dir2, len2);
}

// full_path: "/home/user/tools/be/file_name.ext"
// file_path: "/home/user/tools/be/file_name.ext" --> match
// file_path:  "home/user/tools/be/file_name.ext" --> match
// file_path:            "tools/be/file_name.ext" --> match
// file_path:             "ools/be/file_name.ext" --> not match
// file_path:                     "file_name.ext" --> match
// file_path:                          "name.ext" --> not match
// file_path:                              ".ext" --> not match
int compare_file_path_from_tail(const char *full_path, const char *file_path)
{
	if ((strcmp_from_tail(full_path, file_path) == 0)
	 && (strlen_path(full_path) > strlen_path(file_path))
	 && (full_path[strlen_path(full_path) - strlen_path(file_path) - 1] == '/')) {
		return 0;	// match
	}
	return -1;		// not match
}

char *add_trailing_slash_in_handling(char *dir)
{
#ifdef HANDLE_DIR_PATH_WITH_TRAINLING_SLASH
	add_trailing_slash(dir, dir);
#endif // HANDLE_DIR_PATH_WITH_TRAINLING_SLASH
	return dir;
}
// "/"      ==> "/"
// "."      ==> "./"
// "...dir" ==> "...dir/"
// ""       ==> "" (No change)
char *add_trailing_slash(const char *dir, char *buf)
{
	static char buf_s[MAX_PATH_LEN];
	if (buf == NULL) {
		buf = buf_s;
	}
	if (buf && (buf != dir)) {
		strlcpy__(buf, dir, MAX_PATH_LEN);
	}
	if (strlen(buf) && (buf[strlen(buf) - 1] != '/')) {
		strlcat__(buf, MAX_PATH_LEN, "/");
	}
	return buf;
}
// "/"       ==> "/" (No change)
// "...dir/" ==> "...dir"
char *remove_trailing_slash(const char *str, char *buf)
{
	static char buf_s[MAX_PATH_LEN];
	if (buf == NULL) {
		buf = buf_s;
	}
	if (buf && (buf != str)) {
		strlcpy__(buf, str, MAX_PATH_LEN);
	}
	char *ptr = get_last_slash(buf);
	if (buf < ptr && ptr[0] == '/' && ptr[1] == '\0') {
		*ptr = '\0';
	}
	return buf;
}
PRIVATE const char *get_trailing_slash_in_dir_path(const char *path)
{
	if (strlen_path(path) == 0) {
		return path;
	}
	const char *ptr = &path[strlen_path(path) - 1];
	if (*ptr != '/') {
		ptr++;
	}
	return ptr;
}

//------------------------------------------------------------------------------
BOOL is_abs_path(const char *path)
{
	return path[0] == '/';
}

#ifdef ENABLE_FILER
int is_path_wildcard(char *path)
{
#ifdef ENABLE_DEBUG
	char *ptr = strchr(path, '*');
	if (ptr)
		flf_dprintf("strchr(path, '*') ==> [%s]\n", ptr);
	ptr = strchr(path, '?');
	if (ptr)
		flf_dprintf("strchr(path, '?') ==> [%s]\n", ptr);
#endif // ENABLE_DEBUG
	return strchr(path, '*') || strchr(path, '?');
}
#endif // ENABLE_FILER

const char *concat_dir_and_file_s(const char *dir, const char *file)
{
	return concat_dir_and_file(NULL, dir, file);
}
const char *concat_dir_and_file_s1(const char *dir, const char *file)
{
	static char file_path[MAX_PATH_LEN+1];
	return concat_dir_and_file(file_path, dir, file);
}
const char *concat_dir_and_file_s2(const char *dir, const char *file)
{
	static char file_path[MAX_PATH_LEN+1];
	return concat_dir_and_file(file_path, dir, file);
}
char *concat_dir_and_dir(char *buf, const char *dir, const char *dir2)
{
	concat_dir_and_file(buf, dir, dir2);
	add_trailing_slash_in_handling(buf);
	return buf;
}
// Concatenate path and file
// "/dir1/dir2"   "file"       ==> "/dir1/dir2/file"
// "/dir1/dir2/"  "file"       ==> "/dir1/dir2/file"
// "/dir1/dir2/"  "./dir/file" ==> "/dir1/dir2/./dir/file"
// "dir1/dir2"    "file"       ==> "dir1/dir2/file"
// "/"            "file"       ==> "/file"
// ""             "file"       ==> "file"
// buf and dir can be the same address
char *concat_dir_and_file(char *buf, const char *dir, const char *file)
{
	static char file_path[MAX_PATH_LEN+1];
	if (buf == NULL) {
		buf = file_path;
	}
	if (file[0] == '/') {
		// "/file" ==> "file"
		file++;
	}
	char tmp_buf[MAX_PATH_LEN+1];
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
//
// | type              | example              |include  |can     |can     |can    |
// |                   |                      |directory|be      |include |include|
// |                   |                      |         |relative|"."/".."|symlink|
// |-------------------|----------------------|---------|--------|--------|-------|
// | file_name         |`file`                |   No    |  --    |  Yes   |  Yes  |
// | rel_path          |`../src/filename.ext` |   Yes   | always |  Yes   |  Yes  |
// | file_path         |`../file`,'/dir/file' |   Yes   |  Yes   |  Yes   |  Yes  |
// | full_path         |`/symlink/dir/../file`|   Yes   |  No    |  Yes   |  Yes  |
// | normalized_path   |`/symlink/file`       |   Yes   |  No    |  No    |  Yes  |
// |abs_path(real_path)|`/home/user/file`     |   Yes   |  No    |  No    |  No   |

// TODO: FULL_PATH can be converted to ABS_PATH
//       but ABS_PATH can not be converted to FULL_PATH
//       so you may keep FULL_PATH as long as possible, not replacing with ABS_PATH
// TODO: switch_epc_buf...() shall compare file path in FULL_PATH first and in ABS_PATH next
// TODO: memorize_file_pos() shall memorize file path as it is in FULL_PATH not in ABS_PATH
//       recall_file_pos() shall open file path as it is
//       goto_file_pos() shall open file path as it is
//------------------------------------------------------------------------------

PRIVATE char *normalize_file_path__(const char *file_path, char *parent, char *child);
char *normalize_file_path(char *file_path)
{
	// "/dir1/../????"
	//  ^
	remove_redundant_slash(file_path);
	char *ptr = skip_slash(file_path);
	normalize_file_path__(ptr, ptr, ptr);
	return file_path;
}
PRIVATE char *normalize_file_path__(const char *file_path, char *parent, char *child)
{
	// "/aa/bb/cc"  "/aa/bb/cc"  "/aa/bb/cc"
	//   ^            ^            ^
	//   ^            ^               ^
	//   ^               ^               ^
	for ( ; ; ) {
		if (is_char_file_path_min(child) == 0) {
			break;
		}
		if (strcmp(child, ".") == 0) {
			// "/." ==> "/"  "/aa/." ==> "/aa/"
			//   x                x
			strlcpy__(child, child+1, MAX_PATH_LEN);
		} else
		if (strlcmp__(child, "./") == 0) {
			// "/./???" ==> "/???"  "/aa/./bb" ==> "/aa/bb"
			//   xx                      xx
			strlcpy__(child, child+2, MAX_PATH_LEN);
			child = parent;
		} else
		if (strcmp(child, "..") == 0) {
			// "/.." ==> "/"  "/aa/.." ==> "/"
			//   xx             xxxxx
			strlcpy__(parent, child+2, MAX_PATH_LEN);
			child = parent;
		} else
		if (strlcmp__(child, "../") == 0) {
			// "/../aa" ==> "/aa",  "/aa/../bb" ==> "/bb"  "/aa/bb/../cc" ==> "/aa/cc"
			//   xxx                  xxxxxx                    xxxxxx
			strlcpy__(parent, child+3, MAX_PATH_LEN);
			child = parent;
			if (file_path < child) {
				// not string top, go backward in line
				break;
			}
		} else {
			char *gchild = skip_slash(skip_file_name_max(child));
			if (*gchild) {
				// "/aa/../bb"      "/aa/bb/../cc"
				//   ^  ^             ^  ^  ^
				//   |  +--gchild     |  |  +--gchild
				//   +-----child      |  +-----child
				//   +-----file_path  +--------file_path
				// recursive call
				child = normalize_file_path__(file_path, child, gchild);
			}
			else {
				child = gchild;
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

// Note: path be gotten is a directory path
char *get_abs_dir_path(const char *path, char *abs_path)
{
	get_abs_path(path, abs_path);
	add_trailing_slash_in_handling(abs_path);
	return abs_path;
}
// get absolute path (not include symlinks)
// Note: path be gotten may be a file path or a directory path
char *get_abs_path(const char *path, char *abs_path)
{
	char full_path[MAX_PATH_LEN+1];
	get_full_path(path, full_path);			// --> full_path
	get_real_path(full_path, abs_path);		// --> abs path (real path)
	return abs_path;
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
		// "/???" already full path
		strlcpy__(buf, path, MAX_PATH_LEN);
	} else if (path[0] == '~') {
		// "~", "~user", "~/???" or "~user/???"
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
			// "~/???"   "~/???"
			//   ^    ==>   ^
			len++;
		}
		concat_dir_and_file(buf, user_dir, &path[len]);
	} else {
		// "filename"   ==> "/home/user/tools/filename"
		// "./filename" ==> "/home/user/tools/filename"
		concat_dir_and_file(buf, get_full_path_of_cur_dir(NULL), path);
	}
	return normalize_file_path(buf);
}

char *get_real_path(const char *path, char *buf)
{
#ifndef HAVE_REALPATH
#error "HAVE_REALPATH not defined"
#endif // HAVE_REALPATH
	if (is_path_exist(path)) {
		// `path` actually exists.
		realpath__(path, buf, MAX_PATH_LEN);
	} else {
		// `path` does not actually exist.
		// `/path/to/file` does not exist but `/path/to` may exist.
		// So get a real path of `/path/to` e.g. `/real-path/to`
		// and then return `/real-path/to/file`
		char dir[MAX_PATH_LEN+1];
		char file[MAX_PATH_LEN+1];
		separate_path_to_dir_and_file(path, dir, file);
		realpath__(dir, buf, MAX_PATH_LEN);
		concat_dir_and_file(buf, buf, file);
	}
	return buf;
}

#if defined(HAVE_REALPATH)
// return normalized(canonicalized) absolute file path
// NOTE: `realpath()` needs a path actually exists.
//       otherwise conversion fails.
char *realpath__(const char *path, char *buf, int buf_len)
{
	char buffer[MAX_PATH_LEN+1];
	// `path` actually exists.
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
	int ret = readlink(path, buffer, len);
	if (ret > 0)
		buffer[ret] = '\0';
	return ret;
}

// tests ==============================

#ifdef START_UP_TEST
void test_cwd_PWD()
{
	char buf[MAX_PATH_LEN+1];

	flf_dprintf("getcwd: [%s]\n", getcwd__(buf));
	flf_dprintf("getenv(PWD): [%s]\n", getenv_pwd(buf));

	change_cur_dir("..");
	flf_dprintf("getcwd: [%s]\n", getcwd__(buf));
	flf_dprintf("getenv(PWD): [%s]\n", getenv_pwd(buf));

	change_cur_dir("/home/user/tools/be/be/testfiles/symlinkd");
	flf_dprintf("getcwd: [%s]\n", getcwd__(buf));
	flf_dprintf("getenv(PWD): [%s]\n", getenv_pwd(buf));

	change_cur_dir(get_starting_dir());
	flf_dprintf("getcwd: [%s]\n", getcwd__(buf));
	flf_dprintf("getenv(PWD): [%s]\n", getenv_pwd(buf));
}

PRIVATE void test_cat_dir_and_file_(char *buf, const char *dir, const char *file,
 const char *expected);
void test_cat_dir_and_file()
{
	flf_dprintf("-----------------------\n");
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
	concat_dir_and_file(buf, dir, file);
	if (! IS_EQ_STR(buf, expected)) {
		flf_dprintf("dir: [%s], file: [%s]\n", dir, file);
		flf_dprintf("  buf: [%s]%s[%s]\n", buf, EQU_STR(buf, expected), expected);
	}
}
// /aaa/bbb/.. ==> /aaa
// /aaa/bbb/../ccc ==> /aaa/ccc
// /aaa/bbb/../../ccc ==> /ccc
// /../ccc ==> error
// /aaa/bbb/. ==> /aaa/bbb
// /aaa/bbb/./ccc ==> /aaa/bbb/ccc
// /. ==> /
PRIVATE const char *test_normalize_file_path_(const char *templ, const char *path);
void test_normalize_file_path()
{
	flf_dprintf("-----------------------\n");
	MY_UT_STR(test_normalize_file_path_("/%s",  "."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "././."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "././."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s",  ".."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s/", ".."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../../.."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../../.."), "/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/../c"), "/a/c");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/../c"), "/a/c/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/../ccc"), "/aaa/ccc");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/../ccc"), "/aaa/ccc/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/c/../.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/c/../.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/ccc/../.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/ccc/../.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/../../c"), "/c");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/../../c"), "/c/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/../../ccc"), "/ccc");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/../../ccc"), "/ccc/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/c/../../d"), "/a/d");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/c/../../d"), "/a/d/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/ccc/../../ddd"), "/aaa/ddd");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/ccc/../../ddd"), "/aaa/ddd/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/c/../../../d"), "/d");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/c/../../../d"), "/d/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/ccc/../../../ddd"), "/ddd");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/ccc/../../../ddd"), "/ddd/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/c/../d/../.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/c/../d/../.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/ccc/../ddd/../.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/ccc/../ddd/../.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../a"), "/a");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../a"), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../aaa"), "/aaa");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../aaa"), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../../../a"), "/a");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../../../a"), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../../../aaa"), "/aaa");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../../../aaa"), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../a/b/.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../a/b/.."), "/a/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "../aaa/bbb/.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "../aaa/bbb/.."), "/aaa/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/."), "/a/b/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/."), "/a/b/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/."), "/aaa/bbb/");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/."), "/aaa/bbb/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "a/b/./c"), "/a/b/c");
	MY_UT_STR(test_normalize_file_path_("/%s/", "a/b/./c"), "/a/b/c/");
	MY_UT_STR(test_normalize_file_path_("/%s",  "aaa/bbb/./ccc"), "/aaa/bbb/ccc");
	MY_UT_STR(test_normalize_file_path_("/%s/", "aaa/bbb/./ccc"), "/aaa/bbb/ccc/");
}
PRIVATE const char *test_normalize_file_path_(const char *templ, const char *path)
{
	static char buffer[MAX_PATH_LEN+1];
	snprintf(buffer, MAX_PATH_LEN, templ, path);
	normalize_file_path(buffer);
	return buffer;
}

PRIVATE void test_get_full_path_(const char *path);
void test_get_full_path()
{
	flf_dprintf("-----------------------\n");
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

	test_get_full_path_("#dir_history");	// internal buffer
	test_get_full_path_("#/dir_history");	// internal buffer
}
PRIVATE void test_get_full_path_(const char *path)
{
	char full_path[MAX_PATH_LEN+1];
	get_full_path(path, full_path);
	flf_dprintf("[%s] ==> [%s]\n", path, full_path);
}

#if defined(HAVE_REALPATH)
PRIVATE const char *test_realpath_(const char *path);
void test_realpath()
{
	flf_dprintf("-----------------------\n");
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
void test_get_file_name_extension()
{
	flf_dprintf("-----------------------\n");
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

PRIVATE void test_separate_path_to_dir_and_file__(char *path,
 const char *exp_dir, const char *exp_file);
void test_separate_path_to_dir_and_file()
{
	flf_dprintf("-----------------------\n");
	test_separate_path_to_dir_and_file__("", "", "");
	test_separate_path_to_dir_and_file__(".", "./", "");
	test_separate_path_to_dir_and_file__("..", "../", "");
	test_separate_path_to_dir_and_file__("...", "", "...");
	test_separate_path_to_dir_and_file__("/", "/", "");
	test_separate_path_to_dir_and_file__("/path/to/file", "/path/to/", "file");
	test_separate_path_to_dir_and_file__("/path/to/file", "/path/to/", "file");
	test_separate_path_to_dir_and_file__("path/to/file", "path/to/", "file");
	test_separate_path_to_dir_and_file__("/path/to/dir/", "/path/to/", "dir");
	test_separate_path_to_dir_and_file__("/path/to/dir/", "/path/to/", "dir");
	test_separate_path_to_dir_and_file__("path/to/dir/", "path/to/", "dir");
	test_separate_path_to_dir_and_file__("file", "", "file");
	test_separate_path_to_dir_and_file__("/file", "/", "file");
	test_separate_path_to_dir_and_file__("./file", "./", "file");
	test_separate_path_to_dir_and_file__("../file", "../", "file");
	test_separate_path_to_dir_and_file__(".../file", ".../", "file");
	test_separate_path_to_dir_and_file__("dir/", "", "dir");
	test_separate_path_to_dir_and_file__("/dir/", "/", "dir");
	test_separate_path_to_dir_and_file__("./dir/", "./", "dir");
	test_separate_path_to_dir_and_file__("../dir/", "../", "dir");
	test_separate_path_to_dir_and_file__(".../dir/", ".../", "dir");
}
PRIVATE void test_separate_path_to_dir_and_file__(char *path,
 const char *exp_dir, const char *exp_file)
{
	char buf_dir[MAX_PATH_LEN+1];
	char buf_file[MAX_PATH_LEN+1];
	separate_path_to_dir_and_file(path, buf_dir, buf_file);
	if ((! IS_EQ_STR(buf_dir, exp_dir)) || (! IS_EQ_STR(buf_file, exp_file))) {
		flf_dprintf("path[%s]\n", path);
		flf_dprintf("  dir[%s]%s[%s], file[%s]%s[%s]\n",
		 buf_dir, EQU_STR(buf_dir, exp_dir), exp_dir,
		 buf_file, EQU_STR(buf_file, exp_file), exp_file);
	}
}
#endif // START_UP_TEST

// End of utilpath.c
