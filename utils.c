/**************************************************************************
 *   utils.c                                                              *
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

PRIVATE void (*die_on_callback)(const char *msg) = NULL;
void set_die_on_callback(void (*die_on_cb)(const char *msg))
{
	die_on_callback = die_on_cb;
}
void die_on(const char *msg)
{
	if (die_on_callback) {
		(*die_on_callback)(msg);
		// never returns
	}
	exit(255);
}

// Copy one malloc()ed string to another pointer.
// Should be used as: dest = malloc_strcpy(string);
char *malloc_strcpy(const char *string)
{
	char *buf;

	buf = char_malloc(strlen(string) + 1);
	strcpy__(buf, string);
	return buf;
}

//-----------------------------------------------------------------------------
#ifdef MEMORY_LEAK_CHECKER

#define MAX_MALLOCS_TO_MONITOR	2000000	// 2,000,00
static struct malloc_caller malloc_callers[MAX_MALLOCS_TO_MONITOR];

static const char *caller_file_name = NULL;	// "filename.c"
static int caller_line_num = 0;				// line number
void mlc_init()
{
	for (int alloc_idx = 0; alloc_idx < MAX_MALLOCS_TO_MONITOR; alloc_idx++) {
		mlc_clear_caller(&malloc_callers[alloc_idx]);
	}
	_mlc_set_caller
}
void mlc_set_caller(const char *file_name, int line_num)
{
	caller_file_name = file_name;
	caller_line_num = line_num;
}
struct malloc_caller* mlc_register_caller()
{
	for (int alloc_idx = 0; alloc_idx < MAX_MALLOCS_TO_MONITOR; alloc_idx++) {
		if (malloc_callers[alloc_idx].file_name == NULL) {
			mlc_update_caller(&(malloc_callers[alloc_idx]));
			return &(malloc_callers[alloc_idx]);
		}
	}
	return NULL;
}
void mlc_update_caller(struct malloc_caller* caller)
{
	caller->file_name = caller_file_name;
	caller->line_num = caller_line_num;
}

void mlc_clear_caller(struct malloc_caller* caller)
{
	caller->file_name = NULL;
}

int mlc_check_leak()
{
	int count = 1;
	flf_d_printf("\n");
	for (int alloc_idx = 0; alloc_idx < MAX_MALLOCS_TO_MONITOR; alloc_idx++) {
		if (malloc_callers[alloc_idx].file_name) {
			d_printf("%3d:%3d: file_name: [%s], line_num: %d\n", count, alloc_idx,
			 malloc_callers[alloc_idx].file_name,
			 malloc_callers[alloc_idx].line_num);
			count++;
		}
	}
}

// allocate additional memory for monitoring memory leaks
//		struct malloc_header malloc_header;	// additional memory
//		void memory[size];					// <== return this address to user
//
static int malloced_count = 0;
static int max_malloced_count = 0;
static size_t malloced_size = 0;
static size_t max_malloced_size = 0;
static int save_malloced_count = 0;
static size_t save_malloced_size = 0;

void mlc_check_count()
{
	flf_d_printf("max_count: %d, max_size: %d, count: %d, size: %d\n",
	 max_malloced_count, max_malloced_size, malloced_count, malloced_size);
}
void mlc_memorize_count()
{
	save_malloced_count = malloced_count;
	save_malloced_size = malloced_size;
	mlc_check_count();
}
void mlc_differenciate_count()
{
	mlc_check_count();
	flf_d_printf("count: %d, size: %d\n",
	 malloced_count - save_malloced_count, malloced_size - save_malloced_size);
}
#endif // MEMORY_LEAK_CHECKER

void *malloc__(size_t bytes)
{
	void *ptr;

#ifdef MEMORY_LEAK_CHECKER
	bytes += sizeof(struct malloc_header);
#endif // MEMORY_LEAK_CHECKER
	ptr = malloc(bytes);
	if (ptr == NULL && bytes != 0) {
		_FATALERR_
		die_on(_("Out of memory!"));
	}
#ifdef MEMORY_LEAK_CHECKER
	bytes -= sizeof(struct malloc_header);
	struct malloc_header* mptr = (struct malloc_header*)ptr;
	mptr->size = bytes;
	mptr->caller = mlc_register_caller();

	malloced_count++;
	max_malloced_count = MAX(max_malloced_count, malloced_count);
	malloced_size += bytes;
	max_malloced_size = MAX(max_malloced_size, malloced_size);

	ptr += sizeof(struct malloc_header);
#endif // MEMORY_LEAK_CHECKER
	return ptr;
}

void *remalloc__(void *ptr, size_t bytes)
{
	if (ptr == NULL) {
		return malloc__(bytes);
	}
#ifdef MEMORY_LEAK_CHECKER
	ptr -= sizeof(struct malloc_header);
	struct malloc_header* mptr = (struct malloc_header*)ptr;
	struct malloc_caller* caller = mptr->caller;
	malloced_size -= mptr->size;
	bytes += sizeof(struct malloc_header);
#endif // MEMORY_LEAK_CHECKER
	ptr = realloc(ptr, bytes);
	if (ptr == NULL && bytes != 0) {
		_FATALERR_
		die_on(_("Out of memory!"));
	}
#ifdef MEMORY_LEAK_CHECKER
	bytes -= sizeof(struct malloc_header);
	mptr = (struct malloc_header*)ptr;
	mlc_update_caller(caller);
	mptr->size = bytes;
	mptr->caller = caller;
	malloced_size += bytes;

	ptr += sizeof(struct malloc_header);
#endif // MEMORY_LEAK_CHECKER
	return ptr;
}

void free_ptr(void **ptr)
{
	free__(*ptr);
	*ptr = NULL;
}

void free__(void *ptr)
{
	if (ptr) {
#ifdef MEMORY_LEAK_CHECKER
		ptr -= sizeof(struct malloc_header);
		struct malloc_header* mptr = (struct malloc_header*)ptr;
		malloced_count--;
		malloced_size -= mptr->size;
		mlc_clear_caller(mptr->caller);
#endif // MEMORY_LEAK_CHECKER
		free(ptr);
	}
}

//-----------------------------------------------------------------------------

// remalloc memory just enough for a string
// NOTE: DONOT use for be_line_t::data
void remalloc_and_null_terminate_string(char **ptr)
{
	remalloc_and_null_terminate_length(ptr, strlen(*ptr));
}
// remalloc memory for the length of string
// NOTE: DONOT use for be_line_t::data
void remalloc_and_null_terminate_length(char **ptr, size_t len)
{
	_mlc_set_caller
	*ptr = char_remalloc(*ptr, len + 1);
	(*ptr)[len] = '\0';
}

//=============================================================================

#define POLYNOMIAL_CRC16CCITT	0x1021
unsigned short crc16ccitt = 0xffff;

unsigned short clear_crc16ccitt(void)
{
	crc16ccitt = 0xffff;
	return crc16ccitt;
}

unsigned short calc_crc16ccitt(unsigned char byte)
{
	int cnt;
	int do_xor;

	for (cnt = 0; cnt < 8; cnt++) {
		if (crc16ccitt & 0x8000)
			do_xor = 1;
		else
			do_xor = 0;
		crc16ccitt <<= 1;
		if (byte & 0x80)
			crc16ccitt += 1;
		if (do_xor)
			crc16ccitt ^= POLYNOMIAL_CRC16CCITT;
		byte <<= 1;
	}
	return crc16ccitt;
}
//-----------------------------------------------------------------------------
const char *cur_ctime(void)
{
	time_t cur_time;
#define HHCMMCSS_LEN		8	// "23:59:59"
	static char buf_time[HHCMMCSS_LEN+1];

	cur_time = time(NULL);
	strlcpy__(buf_time, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	return buf_time;
}
//-----------------------------------------------------------------------------
char *get_ssspuuuuuu(char *buf)
{
	long usec;

	usec = get_usec();
	snprintf_(buf, 10+1, "%03d.%06d",
	 (int)(usec / 1000000), (int)(usec % 1000000));
	return buf;
}
char *get_sssssspmmm(char *buf)
{
	long msec;

	msec = get_msec();
	snprintf_(buf, 10+1, "%06d.%03d",
	 (int)(msec / 1000), (int)(msec % 1000));
	return buf;
}
unsigned long get_msec(void)
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	// 999999.999
	return (tv.tv_sec % 1000000) * 1000 + (tv.tv_usec / 1000) % 1000;
}
unsigned long get_usec(void)
{
	struct timeval tv;
	struct timezone tz;

	gettimeofday(&tv, &tz);
	// 999.999999
	return (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;
}
const char *cur_hhcmmcss_mmm(void)
{
#define HHCMMCSS_LEN		8	// "23:59:59"
	struct timeval tv;
	struct timezone tz;
	time_t cur_time;
	char hhcmmcss[HHCMMCSS_LEN+1];
#define HHCMMCSS_MMM_LEN		(8+1+3)
	static char buf_time[HHCMMCSS_MMM_LEN+1];

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec;
	strlcpy__(hhcmmcss, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	snprintf_(buf_time, HHCMMCSS_MMM_LEN+1, "%s.%03d", hhcmmcss, (int)(tv.tv_usec / 1000 % 1000));
	return buf_time;
}
const char *cur_hhcmmcss_uuuuuu(void)
{
#define HHCMMCSS_LEN		8	// "23:59:59"
	struct timeval tv;
	struct timezone tz;
	time_t cur_time;
	char hhcmmcss[HHCMMCSS_LEN+1];
#define HHCMMCSS_UUUUUU_LEN		(8+1+6)
	static char buf_time[HHCMMCSS_UUUUUU_LEN+1];

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec;
	strlcpy__(hhcmmcss, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	snprintf_(buf_time, HHCMMCSS_UUUUUU_LEN+1, "%s.%06d", hhcmmcss, (int)tv.tv_usec);
	return buf_time;
}

//-----------------------------------------------------------------------------------

int int_min(int aa, int bb)
{
	return aa <= bb ? aa : bb;
}
int int_max(int aa, int bb)
{
	return aa >= bb ? aa : bb;
}

//-----------------------------------------------------------------------------------

// /proc/meminfo
// MemTotal:        3917308 kB
// MemFree:         2777988 kB
int get_mem_free_in_kb(int update)
{
	FILE *fp;
	char buffer[100+1];
	char buf[100+1];
	static int kb = 256 * 1024;		// 256 MB

	if (update) {
///_TFLF_
		if ((fp = fopen("/proc/meminfo", "r")) != NULL) {
			while (fgets(buffer, 100, fp) != 0) {
///flf_d_printf("%s", buffer);
				if (strncmp(buffer, "MemFree:", 8) == 0) {
					if (sscanf(buffer, "%10s %d", buf, &kb) >= 2) {
						break;
					}
				}
			}
		}
		if (fclose(fp) != 0) {
			// error
		}
///_TFLF_
	}

#ifdef ENABLE_DEBUG
///#define DEBUG_MEM_SHORTAGE
#ifdef DEBUG_MEM_SHORTAGE
	kb /= 100;		// 1GB ==> 10MB
#endif // DEBUG_MEM_SHORTAGE
#endif // ENABLE_DEBUG

///flf_d_printf("MemFree:%d\n", kb);
	return kb;
}

//-----------------------------------------------------------------------------------

#ifdef START_UP_TEST
void test_nn_from_num(void)
{
	int num;
#ifdef ENABLE_DEBUG
	char buf[2+1];
#endif // ENABLE_DEBUG

	for (num = -20; num < 260 + 260 + 100 + 20; num++) {
		flf_d_printf("%d ==> [%s]\n", num, nn_from_num(num, buf));
	}
}
#endif // START_UP_TEST
char *nn_from_num(int num, char *buf)
{
	if (num < 0) {
		snprintf_(buf, 2+1, "-%d", (-num) % 10);	// -9 -- -1
	} else if (num < 100) {
		snprintf_(buf, 2+1, "%02d", num);	// 00 -- 99
	} else if (num < 260 + 100) {
		num -= 100;
		snprintf_(buf, 2+1, "%c%d", 'A' + num / 10, num % 10);	// a0 -- z9
	} else if (num < 260 + 260 + 100) {
		num -= (260 + 100);
		snprintf_(buf, 2+1, "%d%c", num / 26, 'A' + num % 26);	// 0a -- 9z
	} else {
		snprintf_(buf, 2+1, "%c%d", '@', num % 10);
	}
	return buf;
}

//-----------------------------------------------------------------------------

#define USR_NAME_LEN	8
#define GRP_NAME_LEN	8
#define MAX_USERS	200
#define MAX_GROUPS	100

static struct uid_name {
	uid_t uid;
	char username[USR_NAME_LEN+1];
	char homedir[MAX_PATH_LEN+1];
} uid_name_cache[MAX_USERS];
static int num_users = -1;

static struct gid_name {
	gid_t gid;
	char grpname[GRP_NAME_LEN+1];
} gid_name_cache[MAX_GROUPS];
static int num_groups = -1;

const char *get_user_name(uid_t uid)
{
#define USER_ID_LEN		8
	static char user_id[USER_ID_LEN+1];
	int idx;

	if (num_users < 0) {
		cache_users();
	}
	for (idx = 0; idx < num_users; idx++) {
		if (uid == uid_name_cache[idx].uid)
			return uid_name_cache[idx].username;
	}
	snprintf_(user_id, USER_ID_LEN+1, "%d", uid);
	return user_id;
}
const char *get_user_home_dir(const char *username)
{
	int idx;

	for (idx = 0; idx < num_users; idx++) {
		if (strlcmp__(username, uid_name_cache[idx].username) == 0) {
			return uid_name_cache[idx].homedir;
		}
	}
	return username;
}
const char *get_group_name(gid_t gid)
{
	static char group_id[USER_ID_LEN+1];
	int idx;

	if (num_groups < 0) {
		cache_groups();
	}
	for (idx = 0; idx < num_groups; idx++) {
		if (gid == gid_name_cache[idx].gid)
			return gid_name_cache[idx].grpname;
	}
	snprintf_(group_id, USER_ID_LEN+1, "%d", gid);
	return group_id;
}
int cache_users(void)
{
	struct passwd *pwent;

	// cache user name
	setpwent();
	for (num_users = 0; num_users < MAX_USERS; num_users++) {
		if ((pwent = getpwent()) == NULL)
			break;
		uid_name_cache[num_users].uid = pwent->pw_uid;
		strlcpy__(uid_name_cache[num_users].username, pwent->pw_name, USR_NAME_LEN);
		strlcpy__(uid_name_cache[num_users].homedir, pwent->pw_dir, MAX_PATH_LEN);
///flf_d_printf("%2d:%4d:[%s]:[%s]\n",
/// num_users,
/// uid_name_cache[num_users].uid,
/// uid_name_cache[num_users].username,
/// uid_name_cache[num_users].homedir);
	}
	endpwent();
	return num_users;
}
int cache_groups(void)
{
	struct group *grent;

	// cache group name
	setgrent();
	for (num_groups = 0; num_groups < MAX_GROUPS; num_groups++) {
		if ((grent = getgrent()) == NULL)
			break;
		gid_name_cache[num_groups].gid = grent->gr_gid;
		strlcpy__(gid_name_cache[num_groups].grpname, grent->gr_name, GRP_NAME_LEN);
////flf_d_printf("%2d:%4d:[%s]\n", num_groups,
//// gid_name_cache[num_groups].gid,
//// gid_name_cache[num_groups].grpname);
	}
	endgrent();
	return num_groups;
}

// End of utils.c
