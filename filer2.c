/**************************************************************************
 *   filer2.c                                                             *
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

#include "headers.h"

#ifdef ENABLE_FILER

//-----------------------------------------------------------------------------

PRIVATE char *get_file_size_str(char *buf_size, loff_t size);
PRIVATE const char *get_1k_to_999k_str(long size, char *buf);
#ifdef START_UP_TEST
void test_get_file_size_str(void)
{
_FLF_
	loff_t size;
	loff_t zeros;
	int bits;
	int bit;
/////#ifdef ENABLE_DEBUG
	char buf_size[20+1];
/////#endif // ENABLE_DEBUG
	const char *expected_strs[] = {
		"     1",
		"     3",
		"     7",
		"    15",
		"    31",
		"    63",
		"   127",
		"   255",
		"   511",
		"  1023",
		"  2047",
		"  4095",
		"  8191",
		" 16383",
		" 32767",
		" 65535",
		"131071",
		"262143",
		"524287",
		"1.048M",
		"2.097M",
		"4.194M",
		"8.388M",
		"16.77M",
		"33.55M",
		"67.10M",
		"134.2M",
		"268.4M",
		"536.8M",
		"1.073G",
		"2.147G",
		"4.294G",
		"8.589G",
		"17.17G",
		"34.35G",
		"68.71G",
		"137.4G",
		"274.8G",
		"549.7G",
		"1.099T",
		"2.199T",
		"4.398T",
		"8.796T",
		"17.59T",
		"35.18T",
		"70.36T",
		"140.7T",
		"281.4T",
		"562.9T",
		"1.125P",
		"2.251P",
		"4.503P",
		"9.007P",
		"18.01P",
		"36.02P",
		"72.05P",
		"144.1P",
		"288.2P",
		"576.4P",
		"1.152E",
		"2.305E",
		"4.611E",
		"9.223E",
		"18.44E",

		" 12345",
		"123450",
		"1.234M",
		"12.34M",
		"123.4M",
		"1.234G",
		"12.34G",
		"123.4G",
		"1.234T",
		"12.34T",
		"123.4T",
		"1.234P",
		"12.34P",
		"123.4P",
		"1.234E",
		"12.34E",
	};

	int expected_idx = 0;
	for (bits = 1; bits <= 64; bits++) {
		// 0x01, 0x03, ... 0x7fffffffffffffff, 0xffffffffffffffff
		size = 0;
		for (bit = 0; bit < bits; bit++) {
			size |= ((loff_t)1) << bit;
		}
		get_file_size_str(buf_size, size);
		////flf_d_printf("%16llx ==> %20llu ==> %s\n", size, size, buf_size);
		MY_UT_STR(buf_size, expected_strs[expected_idx]);	expected_idx++;
	}
	//                   PPPPPTTTGGGMMMKKK000
	for (zeros = 1; zeros <= 1000000000000000; zeros *= 10) {
		// 12345, 123450, ... 12,345,000,000,000,000
		size = 12345 * zeros;
		get_file_size_str(buf_size, size);
		////flf_d_printf("%16llx ==> %20llu ==> %s\n", size, size, buf_size);
		MY_UT_STR(buf_size, expected_strs[expected_idx]);	expected_idx++;
	}
}
#endif // START_UP_TEST

// show_link==1: show symlink, ==0: show symlinked file
// truncate==1: truncate file name to show whole of file attribute
char *file_info_str(file_info_t *file_info, int show_link, int trunc_file_name, int selected)
{
	struct stat *st_ptr;
	struct stat *lst_ptr;
	int is_link;
	int is_link_broken = 0;
	struct tm *tm_ptr;
	char buf_name[MAX_PATH_LEN+1];
	loff_t size;
	char buf_size[20+1];
	char buf_time[20+1];
	int mode;
	char *type_str;
	char buf_mode[20+1];
#define FILE_INFO_BUF_LEN	100
	char buf_info[FILE_INFO_BUF_LEN+1];
	int info_str_len;
	char *info_str_ptr;
	int file_name_cols;
	int file_name_space;
	int rest_space_for_file_name;
	int info_space;
// *filename.ext 123456 070113-125959 user---- group---
#define SELECTED_MARK_LEN	1
#define FILE_NAME_INFO_BUF_LEN	(SELECTED_MARK_LEN + MAX_PATH_LEN*MAX_UTF8C_BYTES)
	static char buffer[FILE_NAME_INFO_BUF_LEN+1];
#define MIN_FILE_NAME_SPACE		12		// "filename.ext"(DOS8.3)

/////flf_d_printf("[%s], %d\n", file_info->file_name, get_file_type_num(file_info));
	st_ptr = &file_info->st;
	lst_ptr = &file_info->lst;
	is_link = S_ISLNK(lst_ptr->st_mode);
	if (is_link) {
		is_link_broken = (memcmp(st_ptr, lst_ptr, sizeof(*st_ptr)) == 0);
	}
	strcpy__(buf_name, "");
	if (show_link && is_link) {
		strlcat__(buf_name, MAX_PATH_LEN, file_info->file_name);
		///if (S_ISDIR(lst_ptr->st_mode)) {
		///	strlcat__(buf_name, MAX_PATH_LEN, "/");
		///} else if (lst_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
		///	// Symlink's 'x' bits are usually set.
		///	// strlcat__(buf_name, "*", MAX_PATH_LEN);
		///}
#define LINK_ARROW		" -> "
///#define LINK_ARROW		" >>"
///#define LINK_ARROW		" > "
		strlcat__(buf_name, MAX_PATH_LEN, LINK_ARROW);
		if (file_info->symlink)
			strlcat__(buf_name, MAX_PATH_LEN, file_info->symlink);
		if (is_link_broken)
			strlcat__(buf_name, MAX_PATH_LEN, "!");
	} else {
		if (is_link)
			strlcat__(buf_name, MAX_PATH_LEN, file_info->symlink);
		else
			strlcat__(buf_name, MAX_PATH_LEN, file_info->file_name);
	}
	if (S_ISDIR(st_ptr->st_mode)) {
		strlcat__(buf_name, MAX_PATH_LEN, "/");
		/////if (strcmp(file_info->file_name, "..") == 0) {
		/////	// show "../" like "../ [@host-name]"
		/////	strcat_printf(buf_name, MAX_PATH_LEN, " [%s]", get_at_host_name());
		/////}
	} else if (is_link_broken == 0
	 && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
		strlcat__(buf_name, MAX_PATH_LEN, "*");
	}

	strcpy__(buf_size, "");
	size = show_link ? lst_ptr->st_size : st_ptr->st_size;
///#define TEST_HUGE_SIZE
#ifdef	TEST_HUGE_SIZE
	size *= 1000000000000;
#endif // TEST_HUGE_SIZE
	// 2^32 = 4294967296 = 4e9 = 4Giga, 2^64 = 1844674407000000000 = 18e18 = 18Exa = 18446Peta
	if ((show_link && is_link)		// link itself
	 || (S_ISREG(st_ptr->st_mode) || S_ISDIR(st_ptr->st_mode))) {
		get_file_size_str(buf_size, size);
	} else if (S_ISCHR(st_ptr->st_mode)) {
		size = st_ptr->st_rdev;
		snprintf_(buf_size, 20+1, "%3dC%3d", (int)((size & 0xff00) >> 8), (int)(size & 0xff));
	} else if (S_ISBLK(st_ptr->st_mode)) {
		size = st_ptr->st_rdev;
		snprintf_(buf_size, 20+1, "%3dB%3d", (int)((size & 0xff00) >> 8), (int)(size & 0xff));
	} else if (S_ISFIFO(st_ptr->st_mode)) {
		snprintf_(buf_size, 20+1, "   FIFO");
	} else if (S_ISSOCK(st_ptr->st_mode)) {
		snprintf_(buf_size, 20+1, " SOCKET");
	}

	tm_ptr = localtime(show_link ? &lst_ptr->st_mtime : &st_ptr->st_mtime);
	snprintf_(buf_time, 20+1, "%02d%02d%02d-%02d%02d%02d",
	 tm_ptr->tm_year % 100, tm_ptr->tm_mon+1, tm_ptr->tm_mday,
	 tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);

	mode = show_link ? lst_ptr->st_mode : st_ptr->st_mode;
	if (S_ISREG(mode)) {
		type_str = " ";
	} else if (S_ISDIR(mode)) {
		type_str = "d";
	} else if (S_ISLNK(mode)) {
		type_str = "l";
	} else if (S_ISCHR(mode)) {
		type_str = "c";
	} else if (S_ISBLK(mode)) {
		type_str = "b";
	} else if (S_ISFIFO(mode)) {
		type_str = "f";
	} else if (S_ISSOCK(mode)) {
		type_str = "s";
	} else {
		type_str = "?";		// unknown file type
	}
	switch (GET_APPMD(fl_SHOW_FILE_INFO)) {
	default:
	case SHOW_FILE_INFO_0:
	case SHOW_FILE_INFO_1:
	case SHOW_FILE_INFO_2:
	case SHOW_FILE_INFO_3:
		snprintf_(buf_mode, 20+1, "%s", "");
		break;
	case SHOW_FILE_INFO_4:
		//						   t7777
		snprintf_(buf_mode, 20+1, "%s%04o", type_str, mode & 07777);
		break;
	case SHOW_FILE_INFO_5:
		//						   t r w x r w x r w x
		snprintf_(buf_mode, 20+1, "%s%c%c%c%c%c%c%c%c%c", type_str,
		 (mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-',
		 (mode & S_IXUSR) ? ((mode & S_ISUID) ? 's' : 'x') : ((mode & S_ISUID) ? 'S' : '-'),
		 (mode & S_IRGRP) ? 'r' : '-', (mode & S_IWGRP) ? 'w' : '-',
		 (mode & S_IXGRP) ? ((mode & S_ISGID) ? 's' : 'x') : ((mode & S_ISGID) ? 'S' : '-'),
		 (mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-',
		 (mode & S_IXOTH) ? 'x' : '-');
		break;
	}

	switch (GET_APPMD(fl_SHOW_FILE_INFO)) {
	default:
	case SHOW_FILE_INFO_0:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, "%s", " ");
		break;
	case SHOW_FILE_INFO_1:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s", buf_size);
		break;
	case SHOW_FILE_INFO_2:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s", buf_time);
		break;
	case SHOW_FILE_INFO_3:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s %s", buf_size, buf_time);
		break;
	case SHOW_FILE_INFO_4:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s %s %s %-8s",
		 buf_size, buf_time, buf_mode,
		 get_user_name(show_link ? lst_ptr->st_uid : st_ptr->st_uid));
		break;
	case SHOW_FILE_INFO_5:
		snprintf_(buf_info, FILE_INFO_BUF_LEN+1, " %s %s %s %-8s %-8s",
		 buf_size, buf_time, buf_mode,
		 get_user_name(show_link ? lst_ptr->st_uid : st_ptr->st_uid),
		 get_group_name(show_link ? lst_ptr->st_gid : st_ptr->st_gid));
		break;
	}
	for (info_str_ptr = buf_info; *info_str_ptr; info_str_ptr++) {
		if (info_str_ptr[1] != ' ')	// skip space
			break;
	}
	info_str_len = strnlen(info_str_ptr, FILE_INFO_BUF_LEN);

	file_name_cols = utf8s_columns(buf_name, main_win_get_columns());
	rest_space_for_file_name = LIM_MIN(0,
	 sub_win_get_columns() - (SELECTED_MARK_LEN + info_str_len));
	if (trunc_file_name) {
		// truncate file name not to overwrap to file info.
		// *file.ext    iiiiiiiiiiiiiiiiiiiiiii
		// *filename.e~ iiiiiiiiiiiiiiiiiiiiiii
		file_name_space = LIM_MIN(LIM_MAX(file_name_cols, MIN_FILE_NAME_SPACE),
		 rest_space_for_file_name);
	} else {
		// overwrap file name to file info.
		// *file.ext    iiiiiiiiiiiiiiiiiiiiiii
		// *filename.ext ~iiiiiiiiiiiiiiiiiiiii
		file_name_space = LIM_MAX(LIM_MIN(file_name_cols, rest_space_for_file_name),
		 sub_win_get_columns() - SELECTED_MARK_LEN);
	}
	if (file_name_cols > file_name_space) {
		truncate_tail_utf8s_columns(buf_name, file_name_space-1);
		strlcat__(buf_name, MAX_PATH_LEN, "~");	// add truncated-mark
	}
	expand_utf8s_columns(buf_name, file_name_space);
	info_space = LIM_MIN(0, sub_win_get_columns() - (SELECTED_MARK_LEN + file_name_space));
	if (info_space < strnlen(info_str_ptr, FILE_INFO_BUF_LEN)) {
		// truncate head
		info_str_ptr = &info_str_ptr[strnlen(info_str_ptr, FILE_INFO_BUF_LEN) - info_space];
		if (info_space >= 3) {
			info_str_ptr[0] = ' ';	// truncate-head mark
			info_str_ptr[1] = '~';	// truncate-head mark
		}
	}
	if (show_link == 0 && is_link_broken) {
		strlcpy__(buffer, _(" <<broken-link>>"), FILE_NAME_INFO_BUF_LEN);
	} else {
		snprintf_(buffer, FILE_NAME_INFO_BUF_LEN+1, "%c%s%s",
		 selected ? '*' : ' ', buf_name, info_str_ptr);
	}
	return buffer;
}
PRIVATE char *get_file_size_str(char *buf_size, loff_t size)
{
	char buf[5+1];		// 1.000 - 999.9 - 10000 - 99999

	if (size < 1000000ULL)					// less than 1 MEGA (0 - 999999)
		snprintf_(buf_size, 6+1, "%6d", (int)size);
	else if (size < 1000000000ULL)			// less than 1000 MEGA (1.000M - 999.9M)
		snprintf_(buf_size, 6+1, "%sM", get_1k_to_999k_str(size / 1000ULL, buf));
	else if (size < 1000000000000ULL)		// less than 1000 GIGA (1.000G - 999.9G)
		snprintf_(buf_size, 6+1, "%sG", get_1k_to_999k_str(size / 1000000ULL, buf));
	else if (size < 1000000000000000ULL)	// less than 1000 TERA (1.000T - 999.9T)
		snprintf_(buf_size, 6+1, "%sT", get_1k_to_999k_str(size / 1000000000ULL, buf));
	else if (size < 1000000000000000000ULL)	// less than 1000 PETA (1.000P - 999.9P)
		snprintf_(buf_size, 6+1, "%sP", get_1k_to_999k_str(size / 1000000000000ULL, buf));
	else									// less than 18000 EXA (1.000E - 18.44E)
		snprintf_(buf_size, 6+1, "%sE", get_1k_to_999k_str(size / 1000000000000000ULL, buf));
	return buf_size;
}
PRIVATE const char *get_1k_to_999k_str(long size, char *buf)
{
	if (size < 10000UL)					// less than 10K (1.000K - 9.999K)
		snprintf_(buf, 5+1, "%1d.%03d",
		 (int)(size / 1000), (int)((size / 1) % 1000));
	else if (size < 100000UL)			// less than 100K (10.00M - 99.99G)
		snprintf_(buf, 5+1, "%2d.%02d",
		 (int)(size / 1000), (int)((size / 10) % 100));
	else if (size < 1000000UL)			// less than 1000K (100.0K - 999.9K)
		snprintf_(buf, 5+1, "%3d.%1d",
		 (int)(size / 1000), (int)((size / 100) % 10));
	else if (size < 1000 * 99999ULL)	// 1000K - 99999K
		snprintf_(buf, 5+1, "%5d", (int)(size / 1000));
	return buf;
}

//     0 - 99999
//  100K -  999K
// 1.00M - 9.99M
// 10.0M - 99.9M
//  100M -  999M
// 1.00G - 9.99G
// 10.0G - 99.9G
//  100G -  999G

//-----------------------------------------------------------------------------
int make_file_list(filer_view_t *fv, const char *filter)
{
	char dir_save[MAX_PATH_LEN+1];
	DIR *dir;
	struct dirent *dirent;
	char symlink[MAX_PATH_LEN+1];
	file_info_t *ent_ptr;	// file entry
	struct stat st;
	struct stat lst;
	int entries;
	int file_idx;
	int len;

	change_cur_dir_after_save(dir_save, fv->cur_dir);

	free_file_list(fv);

	if (strcmp(filter, "") == 0)
		filter = "*";
	if ((dir = opendir(fv->cur_dir)) == NULL) {
		goto make_file_list_ret;
	}
	for (file_idx = 0; (dirent = readdir(dir)) != NULL; file_idx++) {
		// count files
	}
	entries = file_idx;
	_mlc_set_caller
	fv->file_list = (file_info_t *)malloc__(sizeof(file_info_t) * entries);

	rewinddir(dir);
	for (file_idx = 0; file_idx < entries && (dirent = readdir(dir)) != NULL; ) {
		if (lstat(dirent->d_name, &lst) < 0)	// stat link itself
			continue;
		if (S_ISLNK(lst.st_mode)
		 && stat(dirent->d_name, &st) >= 0) {	// stat linked file
			// stating file succeeded
		} else {
			// copy from lst to st
			memcpy__(&st, &lst, sizeof(struct stat));
		}
		if (S_ISDIR(st.st_mode)
		 || (S_ISDIR(st.st_mode) == 0 && fnmatch(filter, dirent->d_name, 0) == 0)) {
			if (GET_APPMD(fl_SHOW_DOT_FILE) == 0
			 && ((strncmp(dirent->d_name, ".", 1) == 0 && strcmp(dirent->d_name, "..") != 0)
			  || ((st.st_mode & RWXRWXRWX) == 0)
			  || ((st.st_mode & RWXRWXRWX) == RW0000RW0)))
				// ".", ".????" or (mode == 000)
				continue;
			ent_ptr = &fv->file_list[file_idx];
			// fill file_info_t
			_mlc_set_caller
			ent_ptr->file_name = malloc_strcpy(dirent->d_name);
			memcpy__(&ent_ptr->st, &st, sizeof(struct stat));
			memcpy__(&ent_ptr->lst, &lst, sizeof(struct stat));
			ent_ptr->symlink = NULL;
			if (S_ISLNK(lst.st_mode)) {
				if ((len = readlink__(dirent->d_name, symlink, MAX_PATH_LEN)) > 0) {
					_mlc_set_caller
					ent_ptr->symlink = malloc_strcpy(symlink);
				}
			}
			ent_ptr->selected = 0;
			file_idx++;
		}
	}
	closedir(dir);
	fv->file_list_entries = file_idx;
	fv->cur_sel_idx = MIN_(fv->file_list_entries-1, fv->cur_sel_idx);

make_file_list_ret:;
	change_cur_dir(dir_save);
	return fv->file_list_entries;
}
// Free malloc()ed memory
void free_file_list(filer_view_t *fv)
{
	if (fv->file_list) {
		for (int file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
			if (fv->file_list[file_idx].file_name) {
				FREE_CLR_PTR(fv->file_list[file_idx].file_name);
			}
			if (fv->file_list[file_idx].symlink) {
				FREE_CLR_PTR(fv->file_list[file_idx].symlink);
			}
		}
		FREE_CLR_PTR(fv->file_list);
	}
	fv->file_list_entries = 0;
}

//-----------------------------------------------------------------------------
// sort file list
PRIVATE int comp_file_type(file_info_t *aa, file_info_t *bb);
PRIVATE int comp_file_info(const void *aa, const void *bb);
PRIVATE int comp_file_info_(const void *aa, const void *bb);
PRIVATE int comp_file_name(const void *aa, const void *bb);
PRIVATE int comp_file_extension(const void *aa, const void *bb);
PRIVATE int comp_file_time(const void *aa, const void *bb);
PRIVATE int comp_file_size(const void *aa, const void *bb);
PRIVATE int comp_file_executable(file_info_t *aa, file_info_t *bb);
////PRIVATE int get_file_type_num(file_info_t *info);
PRIVATE int get_stat_file_type_num(struct stat *st, const char *file_name);
PRIVATE int get_file_executable(struct stat *st);
PRIVATE int strtypecasecmp(const char *s1, const char *s2);
void sort_file_list(filer_view_t *fv)
{
	qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t), comp_file_info);
}

// Comparison functions for file list ------------------------------------------
PRIVATE int comp_file_info(const void *aa, const void *bb)
{
	int ret;
	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0) {
		return ret;
	}
	///if (get_file_type_num((file_info_t *)aa) <= 22) {
	///	// always sort directory in first place
	///	return comp_file_name(aa, bb);
	///}
	ret = comp_file_info_(aa, bb);
	switch (GET_APPMD(fl_FILE_SORT_BY)) {
	default:
	case FILE_SORT_BY_NAME:
	case FILE_SORT_BY_EXT:
	case FILE_SORT_BY_TIME:
	case FILE_SORT_BY_SIZE:
		break;
	case FILE_SORT_BY_NAME_REV:
	case FILE_SORT_BY_EXT_REV:
	case FILE_SORT_BY_TIME_REV:
	case FILE_SORT_BY_SIZE_REV:
		ret = -ret;
		break;
	}
	return ret;
}
PRIVATE int comp_file_info_(const void *aa, const void *bb)
{
	int ret;

	switch (GET_APPMD(fl_FILE_SORT_BY)) {
	default:
	case FILE_SORT_BY_NAME:
	case FILE_SORT_BY_NAME_REV:
		if (ret = comp_file_name(aa, bb)) {			return ret;		}
		if (ret = comp_file_extension(aa, bb)) {	return ret;		}
		return ret;
	case FILE_SORT_BY_EXT:
	case FILE_SORT_BY_EXT_REV:
		if (ret = comp_file_executable((file_info_t *)aa, (file_info_t *)bb)) {
			return ret;
		}
		if (ret = comp_file_extension(aa, bb)) {	return ret;		}
		if (ret = comp_file_name(aa, bb)) {			return ret;		}
		return ret;
	case FILE_SORT_BY_TIME:
	case FILE_SORT_BY_TIME_REV:
		return comp_file_time(aa, bb);
	case FILE_SORT_BY_SIZE:
	case FILE_SORT_BY_SIZE_REV:
		return comp_file_size(aa, bb);
	}
}
// compare file type
PRIVATE int comp_file_type(file_info_t *aa, file_info_t *bb)
{
	return get_file_type_num(aa) - get_file_type_num(bb);
}
// sort directories before files,
// sort by file name.
PRIVATE int comp_file_name(const void *aa, const void *bb)
{
	return strtypecasecmp(((file_info_t *)aa)->file_name, ((file_info_t *)bb)->file_name);
}
// sort directories before files,
// sort executable files before non-executables,
// sort by file extension then by file name.
PRIVATE int comp_file_extension(const void *aa, const void *bb)
{
	return strcmp(get_file_name_extension(((file_info_t *)aa)->file_name),
	 get_file_name_extension(((file_info_t *)bb)->file_name));
}
// sort directories before files,
// and then by modification time stamp.
PRIVATE int comp_file_time(const void *aa, const void *bb)
{
	return ((file_info_t *)aa)->st.st_mtime - ((file_info_t *)bb)->st.st_mtime;
}
// sort directories before files,
// and then by file size.
PRIVATE int comp_file_size(const void *aa, const void *bb)
{
	return ((file_info_t *)aa)->st.st_size - ((file_info_t *)bb)->st.st_size;
}
// sort executable files before non-executables
PRIVATE int comp_file_executable(file_info_t *aa, file_info_t *bb)
{
	return get_file_executable(&aa->st) - get_file_executable(&bb->st);
}

// rank of file type
//  1: ..
//  2: .
// 10: unknown type
// 20: dir
// 30: symlink broken
// 31: symlink to unknown type
// 32: symlink to dir
// 33: symlink to symlink
// 34: symlink to FIFO
// 35: symlink to socket
// 36: symlink to char device
// 37: symlink to block device
// 38: symlink to regular file
// 40: FIFO
// 50: socket
// 60: char device
// 70: block device
// 80: regular file

////PRIVATE 
int get_file_type_num(file_info_t *info)
{
	int file_type_num = 0;
	if (S_ISLNK(info->lst.st_mode)) {
		if (memcmp(&(info->st), &(info->lst), sizeof(info->st)) != 0) {
			file_type_num = 30 + get_stat_file_type_num(&(info->st), info->file_name) / 10;
		} else {
			file_type_num = 30;	// broken symlink
		}
	} else {
		file_type_num = get_stat_file_type_num(&(info->st), info->file_name);
	}
	return file_type_num;
}
PRIVATE int get_stat_file_type_num(struct stat *st, const char *file_name)
{
	int file_type_num = 10;		// unknown type
	if (S_ISDIR(st->st_mode)) {
		if (strcmp(file_name, "..") == 0) {
			file_type_num = 1;	// ".."
		} else
		if (strcmp(file_name, ".") == 0) {
			file_type_num = 2;	// "."
		} else {
			file_type_num = 20;	// dir
		}
	} else
	if (S_ISLNK(st->st_mode)) {
		file_type_num = 30;		// symlink
	} else
	if (S_ISFIFO(st->st_mode)) {
		file_type_num = 40;		// FIFO
	} else
	if (S_ISSOCK(st->st_mode)) {
		file_type_num = 50;		// socket
	} else
	if (S_ISCHR(st->st_mode)) {
		file_type_num = 60;		// character device
	} else
	if (S_ISBLK(st->st_mode)) {
		file_type_num = 70;		// block device
	} else
	if (S_ISREG(st->st_mode)) {
		file_type_num = 80;		// regular file
	}
	return file_type_num;
}
PRIVATE int get_file_executable(struct stat *st)
{
	if (st->st_mode & S_IXUSR)
		return 0;
	return 1;
}
// smaller:
//  ispunct()
//  isdigit()
//  isalpha()
// larger:
PRIVATE int strtypecasecmp(const char *s1, const char *s2)
{
	int diff;

	for ( ; *s1 != '\0' && *s2 != '\0'; s1++, s2++) {
		if ((diff = ispunct(*s1) - ispunct(*s2)) != 0) {
			return -diff;
		}
		if (tolower(*s1) != tolower(*s2))
			break;
	}
	return (tolower(*s1) - tolower(*s2));
}

//-----------------------------------------------------------------------------
int get_files_selected_cfv(void)
{
	return get_files_selected(get_cur_filer_view());
}
int get_files_selected(filer_view_t *fv)
{
	int idx;
	int files_selected = 0;

	for (idx = 0; idx < fv->file_list_entries; idx++) {
		if (fv->file_list[idx].selected) {
			files_selected++;
		}
	}
	return files_selected;
}

int select_and_get_first_file_idx_selected(void)
{
	select_file_if_none_selected();
	return get_first_file_idx_selected();
}
int select_file_if_none_selected(void)
{
	int files_selected;
	int sel_idx = get_cur_filer_view()->cur_sel_idx;

	files_selected = get_files_selected_cfv();
	if (files_selected == 0) {
		get_cur_filer_view()->file_list[sel_idx].selected = _FILE_SEL_AUTO_;
	}
	return files_selected;
}
int get_first_file_idx_selected(void)
{
	int file_idx;

	for (file_idx = 0; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		if (get_cur_filer_view()->file_list[file_idx].selected)
			break;
	}
	if (file_idx < get_cur_filer_view()->file_list_entries)
		return file_idx;
	// no file selected, return current file
	return get_cur_filer_view()->cur_sel_idx;
}
int get_next_file_idx_selected(int start_file_idx)
{
	int file_idx = start_file_idx < 0 ? 0 : start_file_idx+1;

	for ( ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		if (get_cur_filer_view()->file_list[file_idx].selected)
			break;
	}
	if (file_idx < get_cur_filer_view()->file_list_entries)
		return file_idx;
	return -1;	// no selected file found
}
void unselect_all_files_auto(char selection_bit)
{
	for (int file_idx = 0 ; file_idx < get_cur_filer_view()->file_list_entries; file_idx++) {
		get_cur_filer_view()->file_list[file_idx].selected
		 = get_cur_filer_view()->file_list[file_idx].selected & ~selection_bit;
	}
}

//-----------------------------------------------------------------------------
int research_file_name_in_file_list(filer_view_t *fv)
{
	int file_idx = search_file_name_in_file_list(fv, fv->next_file);
	if (file_idx < 0) {
		if (fv->cur_sel_idx >= 0)
			file_idx = fv->cur_sel_idx;
		else
			file_idx = 0;
	}
	file_idx = MIN_MAX_(0, file_idx, fv->file_list_entries-1);
	fv->cur_sel_idx = file_idx;
	return 0;
}

int search_file_name_in_file_list(filer_view_t *fv, const char *file_name)
{
	int file_name_len;
	// 0,1: the same file type, 2,3: all file
	// 0,2: case sensitive, 1,3: case ignorant
	// 0: the same file type    , case sensitive
	// 1: the same file type    , case ignorant
	// 2: may diffrent file type, case sensitive
	// 3: may diffrent file type, case ignorant
	int cmp_type;

	// exact match
	for (int file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
		if (strcmp(fv->file_list[file_idx].file_name, file_name) == 0) {
			return file_idx;
		}
	}
	// partial match
	for (file_name_len = strlen(file_name); file_name_len; file_name_len--) {
		for (cmp_type = 0; cmp_type < 4; cmp_type++) {
			for (int file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
				if (((cmp_type < 2)		// cmp_type = 0, 1
				  && (S_ISREG(get_cur_filer_view()
					   ->file_list[get_cur_filer_view()->cur_sel_idx].st.st_mode)
					  == S_ISREG(get_cur_filer_view()
					   ->file_list[file_idx].st.st_mode)))
				 || (cmp_type >= 2)) {	// cmp_type = 2, 3
					if (((cmp_type % 2) == 0)
						// case sensitive
					  ? (strncmp(fv->file_list[file_idx].file_name, file_name,
						 file_name_len) == 0)
						// case ignorant
					  : (strncasecmp(fv->file_list[file_idx].file_name, file_name,
						 file_name_len) == 0)) {
						return file_idx;
					}
				}
			}
		}
	}
	return -1;
}

#endif // ENABLE_FILER

// End of filer2.c
