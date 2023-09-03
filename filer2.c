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

void begin_fork_exec_repeat(void)
{
	restore_term_for_shell();
	clear_fork_exec_counter();
	clear_handler_sigint_called();
}
void end_fork_exec_repeat(void)
{
	pause_after_exec();
	reinit_term_for_filer();
}

//-----------------------------------------------------------------------------

#define MAX_EXECV_ARGS		10

PRIVATE int fork_exec_sh_c(int set_term, int separate_bef_exec, int pause_aft_exec,
 const char *command);
PRIVATE int args_from_va_list(char **args, va_list ap);
PRIVATE int fork_execv_hist(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const *args);
#ifdef ENABLE_HISTORY
PRIVATE void output_exec_args_history(char * const *args);
#endif // ENABLE_HISTORY
PRIVATE int fork_execv(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[]);

int fork_exec_once_sh_c(int separate_bef_exec, int pause_aft_exec, const char *command)
{
	clear_fork_exec_counter();
	return fork_exec_sh_c(SETTERM1, separate_bef_exec, pause_aft_exec, command);
}
int fork_exec_repeat_sh_c(int separate_bef_exec, const char *command)
{
	return fork_exec_sh_c(SETTERM0, separate_bef_exec, PAUSE0, command);
}
PRIVATE int fork_exec_sh_c(int set_term, int separate_bef_exec, int pause_aft_exec,
 const char *command)
{
	char *args[MAX_EXECV_ARGS+1];

	// sh -c "command arg1 arg2 arg3"
	args[0] = "sh";
	args[1] = "-c";
	args[2] = (char *)command;
	args[3] = NULL;

#ifdef ENABLE_HISTORY
	if (get_fork_exec_counter() == 0) {
		update_history(HISTORY_TYPE_IDX_EXEC, command);
	}
#endif // ENABLE_HISTORY
	return fork_execv(set_term, separate_bef_exec, pause_aft_exec, args);
}
int fork_exec_once(int separate_bef_exec, int pause_aft_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];

	clear_fork_exec_counter();

	va_start(ap, pause_aft_exec);
	args_from_va_list(args, ap);
	va_end(ap);

flf_d_printf("%016p\n", &separate_bef_exec);
flf_d_printf("%016p\n", &pause_aft_exec);
flf_d_printf("%016p\n", &args[0]);
flf_d_printf("%016p\n", &args[1]);
	return fork_execv_hist(SETTERM1, separate_bef_exec, pause_aft_exec, args);
}
int fork_exec_repeat(int separate_bef_exec, ...)
{
	va_list ap;
	char *args[MAX_EXECV_ARGS+1];

	va_start(ap, separate_bef_exec);
	args_from_va_list(args, ap);
	va_end(ap);

flf_d_printf("%016p\n", &separate_bef_exec);
flf_d_printf("%016p\n", &args[0]);
flf_d_printf("%016p\n", &args[1]);
flf_d_printf("[%s]\n", args[0]);
flf_d_printf("[%s]\n", args[1]);
	return fork_execv_hist(SETTERM0, separate_bef_exec, PAUSE0, args);
}

// convert va_list to "char *argv[]"
PRIVATE int args_from_va_list(char **args, va_list ap)
{
	int arg_idx;

	for (arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		args[arg_idx] = va_arg(ap, char *);
		if (args[arg_idx] == NULL)
			break;
	}
	args[arg_idx] = NULL;
	return arg_idx;
}

PRIVATE int fork_execv_hist(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const *args)
{
#ifdef ENABLE_HISTORY
flf_d_printf("[%s]\n", args[0]);
flf_d_printf("[%s]\n", args[1]);
	if (get_fork_exec_counter() == 0) {
		output_exec_args_history(args);
	}
#endif // ENABLE_HISTORY
	return fork_execv(set_term, separate_bef_exec, pause_aft_exec, args);
}

#ifdef ENABLE_HISTORY
PRIVATE void output_exec_args_history(char * const *args)
{
	int arg_idx;
	const char *arg;
	char buffer[MAX_PATH_LEN+1];

	buffer[0] = '\0';
	for (arg_idx = 0; arg_idx < MAX_EXECV_ARGS; arg_idx++) {
		arg = args[arg_idx];
flf_d_printf("[%s]\n", arg);
		if (arg == NULL)
			break;
		concat_file_name_separating_by_space(buffer, MAX_PATH_LEN, arg);
	}
flf_d_printf("[%s]\n", buffer);
	update_history(HISTORY_TYPE_IDX_EXEC, buffer);
}
#endif // ENABLE_HISTORY

PRIVATE int fork_execv(int set_term, int separate_bef_exec, int pause_aft_exec,
 char * const args[])
{
	pid_t pid;
	int exit_status;
	int ret;

	if (set_term && get_fork_exec_counter() == 0) {
		restore_term_for_shell();
	}
	if (separate_bef_exec > 0 && get_fork_exec_counter() == 0) {
		// output separator line
		printf("\n-------- command execution started here --------\n");
		fflush(stdout);
	}

	exit_status = -10000;
	if ((pid = fork()) == 0) {
		signal_clear();
		init_stderr();
///flf_d_printf("args[0]: %s\n", args[0]);
		execvp(args[0], args);
		exit(-10001);					// execution error
	} else {
		for ( ; ; ) {
			ret = waitpid(pid, &exit_status, 0);
mflf_d_printf("ret: %d, exit_status: %d\n", ret, exit_status);
			if (ret != -1)
				break;
		}
	}

	if (pause_aft_exec > 0) {
		printf("\n\aStatus:%d ", exit_status);
		pause_after_exec();
	}
	if (set_term && get_fork_exec_counter() == 0) {
		reinit_term_for_filer();
	}
	inc_fork_exec_counter();
	return exit_status;
}

PRIVATE int fork_exec_counter = 0;
void clear_fork_exec_counter(void)
{
	fork_exec_counter = 0;
}
int get_fork_exec_counter(void)
{
	return fork_exec_counter;
}
int inc_fork_exec_counter(void)
{
	return fork_exec_counter++;
}

void pause_after_exec(void)
{
///	set_term_no_intr();
	set_term_raw();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
//	fflush(stdin);
	getchar();	getchar();	getchar();	getchar();	getchar();
	getchar();	getchar();	getchar();	getchar();	getchar();
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	printf(
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
 "======== Hit any key to return to %s ======== ", APP_NAME);
	fflush(stdout);
	getchar();
	printf("\n");
}

//-----------------------------------------------------------------------------
int restore_term_for_shell(void)
{
	tio_clear_screen();
	tio_set_cursor_on(1);
	tio_suspend();
	signal_fork();
	return 0;
}
int reinit_term_for_filer(void)
{
	signal_init();
	tio_resume();
	tio_clear_flash_screen(1);
	return 0;
}
//-----------------------------------------------------------------------------

PRIVATE char *get_file_size_str(char *buf_size, loff_t size);
PRIVATE const char *get_1k_to_999k_str(long size, char *buf);
#ifdef START_UP_TEST
void test_get_file_size_str(void)
{
	loff_t size;
	loff_t zeros;
	int bits;
	int bit;
#ifdef ENABLE_DEBUG
	char buf_size[20+1];
#endif // ENABLE_DEBUG

	for (bits = 1; bits <= 64; bits++) {
		// 0x01, 0x03, ... 0x7fffffffffffffff, 0xffffffffffffffff
		size = 0;
		for (bit = 0; bit < bits; bit++) {
			size |= ((loff_t)1) << bit;
		}
		flf_d_printf("%16llx ==> %20llu ==> %s\n", size, size, get_file_size_str(buf_size, size));
	}
	//                   PPPPPTTTGGGMMMKKK000
	for (zeros = 1; zeros <= 1000000000000000; zeros *= 10) {
		// 12345, 123450, ... 12,345,000,000,000,000
		size = 12345 * zeros;
		flf_d_printf("%16llx ==> %20llu ==> %s\n", size, size, get_file_size_str(buf_size, size));
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
	int is_link_dead = 0;
	struct tm *tm_ptr;
	char buf_name[MAX_PATH_LEN*3+1];		// for UTF8 file name
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
#define FILE_NAME_INFO_BUF_LEN	(SELECTED_MARK_LEN + MAX_PATH_LEN*3+1 +6+1+(6+1+6)+(8+1+8)+1)
	static char buffer[FILE_NAME_INFO_BUF_LEN+1];
#define MIN_FILE_NAME_SPACE		12		// "filename.ext"(DOS8.3)

	st_ptr = &file_info->st;
	lst_ptr = &file_info->lst;
	is_link = S_ISLNK(lst_ptr->st_mode);
	if (is_link)
		is_link_dead = (memcmp(st_ptr, lst_ptr, sizeof(*st_ptr)) == 0);

	strcpy__(buf_name, "");
	if (show_link && is_link) {
		strlcat__(buf_name, MAX_PATH_LEN*3, file_info->file_name);
		if (S_ISDIR(lst_ptr->st_mode)) {
			strlcat__(buf_name, MAX_PATH_LEN*3, "/");
		} else if (lst_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
			// Symlink's 'x' bits are usually set.
			// strlcat__(buf_name, "*", MAX_PATH_LEN*3);
		}
#define LINK_ARROW		"-->"
		strlcat__(buf_name, MAX_PATH_LEN*3, LINK_ARROW);
		if (file_info->symlink)
			strlcat__(buf_name, MAX_PATH_LEN*3, file_info->symlink);
		if (is_link_dead)
			strlcat__(buf_name, MAX_PATH_LEN*3, "!");
	} else {
		if (is_link)
			strlcat__(buf_name, MAX_PATH_LEN*3, file_info->symlink);
		else
			strlcat__(buf_name, MAX_PATH_LEN*3, file_info->file_name);
	}
	if (S_ISDIR(st_ptr->st_mode)) {
		strlcat__(buf_name, MAX_PATH_LEN*3, "/");
	} else if (is_link_dead == 0
	 && (st_ptr->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
		strlcat__(buf_name, MAX_PATH_LEN*3, "*");
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
	switch(GET_APPMD(fl_SHOW_FILE_INFO)) {
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

///flf_d_printf("<%s %s %s %-8s %-8s>\n", buf_size, buf_time, buf_mode,
/// get_user_name(show_link ? lst_ptr->st_uid : st_ptr->st_uid),
/// get_group_name(show_link ? lst_ptr->st_gid : st_ptr->st_gid));
	switch(GET_APPMD(fl_SHOW_FILE_INFO)) {
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

	file_name_cols = utf8s_columns(buf_name, INT_MAX);
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
		strlcat__(buf_name, MAX_PATH_LEN*3, "~");	// add truncated-mark
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
	if (show_link == 0 && is_link_dead) {
		strlcpy__(buffer, _(" <<broken-link>>"), FILE_NAME_INFO_BUF_LEN);
	} else {
		snprintf_(buffer, FILE_NAME_INFO_BUF_LEN+1, "%c%s%s",
		 selected ? '*' : ' ', buf_name, info_str_ptr);
	}
///dump_cur_win();
///flf_d_printf("[%s]\n", buffer);
	return buffer;
}
PRIVATE char *get_file_size_str(char *buf_size, loff_t size)
{
	char buf_1_to_999[5+1];		// 1.000 - 999.9 - 10000 - 99999

	if (size < 1000000ULL)					// less than 1 MEGA (0 - 999999)
		snprintf_(buf_size, 6+1, "%6d", (int)size);
	else if (size < 1000000000ULL)			// less than 1000 MEGA (1.000M - 999.9M)
		snprintf_(buf_size, 6+1, "%sM", get_1k_to_999k_str(size / 1000ULL, buf_1_to_999));
	else if (size < 1000000000000ULL)		// less than 1000 GIGA (1.000G - 999.9G)
		snprintf_(buf_size, 6+1, "%sG", get_1k_to_999k_str(size / 1000000ULL, buf_1_to_999));
	else if (size < 1000000000000000ULL)	// less than 1000 TERA (1.000T - 9999.9T)
		snprintf_(buf_size, 6+1, "%sT", get_1k_to_999k_str(size / 1000000000ULL, buf_1_to_999));
	else if (size < 1000000000000000000ULL)	// less than 1000 PETA (1.000P - 9999.9P)
		snprintf_(buf_size, 6+1, "%sP", get_1k_to_999k_str(size / 1000000000000ULL, buf_1_to_999));
	else									// more than 1000 PETA (100P - 18446P)
		snprintf_(buf_size, 6+1, "%sP", get_1k_to_999k_str(size / 1000000000000ULL, buf_1_to_999));
	return buf_size;
}
PRIVATE const char *get_1k_to_999k_str(long size, char *buf)
{
	if (size < 10000UL)					// less than 10 K (1.000K - 9.999K)
		snprintf_(buf, 5+1, "%1d.%03d",
		 (int)(size / 1000), (int)((size / 1) % 1000));
	else if (size < 100000UL)			// less than 100 K (10.00M - 99.99G)
		snprintf_(buf, 5+1, "%2d.%02d",
		 (int)(size / 1000), (int)((size / 10) % 100));
	else if (size < 1000000UL)			// less than 1000 K (100.0K - 999.9K)
		snprintf_(buf, 5+1, "%3d.%1d",
		 (int)(size / 1000), (int)((size / 100) % 10));
	else if (size < 1000 * 99999ULL)	// 1000K - 99999K
		snprintf_(buf, 5+1, "%5d", (int)(size / 1000));
	return buf;
}
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

	save_change_cur_dir(dir_save, fv->cur_dir);

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
			ent_ptr->file_name = malloc_strcpy(dirent->d_name);
			memcpy__(&ent_ptr->st, &st, sizeof(struct stat));
			memcpy__(&ent_ptr->lst, &lst, sizeof(struct stat));
			ent_ptr->symlink = NULL;
			if (S_ISLNK(lst.st_mode)) {
				if ((len = readlink__(dirent->d_name, symlink, MAX_PATH_LEN)) > 0) {
					ent_ptr->symlink = malloc_strcpy(symlink);
				}
			}
			ent_ptr->selected = 0;
			file_idx++;
		}
	}
	closedir(dir);
	fv->file_list_entries = file_idx;

make_file_list_ret:;
	change_cur_dir(dir_save);
	return fv->file_list_entries;
}
// Free malloc()ed memory
void free_file_list(filer_view_t *fv)
{
	int file_idx;

	if (fv->file_list) {
		for (file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
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
PRIVATE int comp_file_name(const void *aa, const void *bb);
PRIVATE int comp_file_extension(const void *aa, const void *bb);
PRIVATE int comp_file_time(const void *aa, const void *bb);
PRIVATE int comp_file_size(const void *aa, const void *bb);
PRIVATE int comp_file_type(file_info_t *aa, file_info_t *bb);
PRIVATE int comp_file_executable(file_info_t *aa, file_info_t *bb);
PRIVATE int get_file_type_no(struct stat *st);
PRIVATE int get_file_executable(struct stat *st);
PRIVATE int strtypecasecmp(const char *s1, const char *s2);
void sort_file_list(filer_view_t *fv)
{
	switch(GET_APPMD(fl_FILE_SORT_BY)) {
	case FILE_SORT_BY_NAME:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_name);
		break;
	case FILE_SORT_BY_EXT:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_extension);
		break;
	case FILE_SORT_BY_TIME:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_time);
		break;
	case FILE_SORT_BY_SIZE:
		qsort(fv->file_list, fv->file_list_entries, sizeof(file_info_t),
		 comp_file_size);
		break;
	}
}

// Sorting routine for file listings ------------------------------------------

// sort directories before files,
// sort by file name.
PRIVATE int comp_file_name(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	return strtypecasecmp(((file_info_t *)aa)->file_name, ((file_info_t *)bb)->file_name);
}
// sort directories before files,
// sort executable files before non-executables,
// sort by file extension then by file name.
PRIVATE int comp_file_extension(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;
	if ((ret = comp_file_executable((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	if ((ret = strcmp(get_file_name_extension(((file_info_t *)aa)->file_name),
	 get_file_name_extension(((file_info_t *)bb)->file_name))) != 0)
		return ret;
	return comp_file_name(aa, bb);
}
// sort directories before files,
// and then by modification time stamp.
PRIVATE int comp_file_time(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	return ((file_info_t *)aa)->st.st_mtime - ((file_info_t *)bb)->st.st_mtime;
}
// sort directories before files,
// and then by file size.
PRIVATE int comp_file_size(const void *aa, const void *bb)
{
	int ret;

	if ((ret = comp_file_type((file_info_t *)aa, (file_info_t *)bb)) != 0)
		return ret;

	return ((file_info_t *)aa)->st.st_size - ((file_info_t *)bb)->st.st_size;
}
// compare file type
PRIVATE int comp_file_type(file_info_t *aa, file_info_t *bb)
{
	int file_type_a;
	int file_type_b;

	file_type_a = get_file_type_no(&aa->st);
	file_type_b = get_file_type_no(&bb->st);
	return file_type_a - file_type_b;
}
// sort executable files before non-executables
PRIVATE int comp_file_executable(file_info_t *aa, file_info_t *bb)
{
	int file_type_a;
	int file_type_b;

	file_type_a = get_file_executable(&aa->st);
	file_type_b = get_file_executable(&bb->st);
	return file_type_a - file_type_b;
}
// rank by file type
PRIVATE int get_file_type_no(struct stat *st)
{
	if (S_ISDIR(st->st_mode))
		return 1;
	if (S_ISLNK(st->st_mode))
		return 2;
	if (S_ISFIFO(st->st_mode))
		return 3;
	if (S_ISSOCK(st->st_mode))
		return 4;
	if (S_ISCHR(st->st_mode))
		return 5;
	if (S_ISBLK(st->st_mode))
		return 5;
	if (S_ISREG(st->st_mode))
		return 7;
	return 0;
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
	return get_files_selected(cur_fv);
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
int get_first_file_idx_selected(void)
{
	int file_idx;

	for (file_idx = 0; file_idx < cur_fv->file_list_entries; file_idx++) {
		if (cur_fv->file_list[file_idx].selected)
			break;
	}
	if (file_idx < cur_fv->file_list_entries)
		return file_idx;
	// no file selected, return current file
	return cur_fv->cur_sel_idx;
}
int get_next_file_idx_selected(int start_file_idx)
{
	int file_idx = start_file_idx < 0 ? 0 : start_file_idx+1;

	for ( ; file_idx < cur_fv->file_list_entries; file_idx++) {
		if (cur_fv->file_list[file_idx].selected)
			break;
	}
	if (file_idx < cur_fv->file_list_entries)
		return file_idx;
	return -1;	// no selected file found
}
int select_file_if_none_selected(void)
{
	int files_selected;
	int sel_idx = cur_fv->cur_sel_idx;

	files_selected = get_files_selected_cfv();
	if (files_selected == 0) {
		cur_fv->file_list[sel_idx].selected = _FILE_SEL_AUTO_;
	}
	return files_selected;
}
void unselect_all_files_auto(char selection_bit)
{
	int file_idx;

	for (file_idx = 0 ; file_idx < cur_fv->file_list_entries; file_idx++) {
		cur_fv->file_list[file_idx].selected
		 = cur_fv->file_list[file_idx].selected & ~selection_bit;
	}
}

//-----------------------------------------------------------------------------

int research_file_name_in_file_list(filer_view_t *fv)
{
	int cur_sel_idx;

	cur_sel_idx = search_file_name_in_file_list(fv, fv->next_file);
	if (cur_sel_idx < 0) {
		if (fv->cur_sel_idx >= 0)
			cur_sel_idx = fv->cur_sel_idx;
		else
			cur_sel_idx = 0;
	}
	cur_sel_idx = MIN_MAX_(0, cur_sel_idx, fv->file_list_entries-1);
	fv->cur_sel_idx = cur_sel_idx;
	return 0;
}

int search_file_name_in_file_list(filer_view_t *fv, const char *file_name)
{
	int file_idx;
	int file_name_len;
	// 0,1: regular <-> non-regular file, 2,3: all file
	// 0,2: case sensitive, 1,3: case ignorant
	// 0: the same file type    , case sensitive
	// 1: the same file type    , case ignorant
	// 2: may diffrent file type, case sensitive
	// 3: may diffrent file type, case ignorant
	int cmp_type;

	// exact match
	for (file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
		if (strcmp(fv->file_list[file_idx].file_name, file_name) == 0) {
			return file_idx;
		}
	}
	// partial match in regular file
	for (cmp_type = 0; cmp_type < 4; cmp_type++) {
		for (file_name_len = strlen(file_name); file_name_len; file_name_len--) {
			for (file_idx = 0; file_idx < fv->file_list_entries; file_idx++) {
				if (cmp_type < 2) {		// cmp_type = 0, 1
					if ((S_ISREG(cur_fv->file_list[cur_fv->cur_sel_idx].st.st_mode)
					  == S_ISREG(cur_fv->file_list[file_idx].st.st_mode))
					 && (cmp_type == 0
						// case sensitive
					  ? strncmp(fv->file_list[file_idx].file_name, file_name, file_name_len) == 0
						// case ignorant
					  : strncasecmp(fv->file_list[file_idx].file_name, file_name,
						 file_name_len) == 0)) {
						return file_idx;
					}
				} else {	// cmp_type = 2, 3
					if (cmp_type == 2
						// case sensitive
					  ? strncmp(fv->file_list[file_idx].file_name, file_name, file_name_len) == 0
						// case ignorant
					  : strncasecmp(fv->file_list[file_idx].file_name, file_name,
						 file_name_len) == 0) {
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
