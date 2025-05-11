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
	char *buf = char_malloc(strlen(string) + 1);
	strcpy__(buf, string);
	return buf;
}

//------------------------------------------------------------------------------
#ifdef MEMORY_LEAK_CHECKER

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

void mlc_check_leak()
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
#ifdef MEMORY_LEAK_CHECKER
	bytes += sizeof(struct malloc_header);
#endif // MEMORY_LEAK_CHECKER
	void *ptr = malloc(bytes);
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
	max_malloced_count = MAX_(max_malloced_count, malloced_count);
	malloced_size += bytes;
	max_malloced_size = MAX_(max_malloced_size, malloced_size);

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
	if (ptr == NULL) {
		_PROGERR_
		return;
	}
#ifdef MEMORY_LEAK_CHECKER
	ptr -= sizeof(struct malloc_header);
	struct malloc_header* mptr = (struct malloc_header*)ptr;
	malloced_count--;
	malloced_size -= mptr->size;
	mlc_clear_caller(mptr->caller);
#endif // MEMORY_LEAK_CHECKER
	free(ptr);
}

//------------------------------------------------------------------------------
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
#define INITIAL_CRC16CCITT		0xffff
PRIVATE UINT16 crc16ccitt = INITIAL_CRC16CCITT;
void clear_crc16ccitt()
{
	crc16ccitt = INITIAL_CRC16CCITT;
}
void calc_crc16ccitt(UINT8 byte)
{
	for (int cnt = 0; cnt < 8; cnt++) {
		int do_xor;
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
}
UINT16 get_crc16ccitt()
{
	return crc16ccitt;
}
//------------------------------------------------------------------------------
// NOTE: to avoid snprintf trancation warning of C compiler, add 20 bytes
#define YYYYSMMSDD_HHCMMCSS_LEN		(4+1+2+1+2+1+2+1+2+1+2)	// "2037/12/31 23:59:59"
#define YYYYMMDD_HHMMSS_LEN			(8+1+6)					// "20241009-235959"
#define YYMMDD_HHMMSS_LEN			(6+1+6)					// "241009-235959"
PRIVATE char *get_yyyysmmsdd_hhcmmcss(time_t abs_time, char *buf);
PRIVATE char *get_yyyymmdd_hhmmss(time_t abs_time, char *buf);

const char *cur_ctime_cdate(int time0_date1)
{
	if (time0_date1 == 0) {
		return cur_ctime();
	} else {
		return cur_cdate();
	}
}
const char *cur_cdate()
{
	static char buf_date[YYYYSMMSDD_HHCMMCSS_LEN+1];
	char buf_ymd_hms[YYYYSMMSDD_HHCMMCSS_LEN+1];

	time_t cur_time = time(NULL);
	get_yyyysmmsdd_hhcmmcss(cur_time, buf_ymd_hms);
	strlcpy__(buf_date, &(buf_ymd_hms[2]), YYSMMSDD_LEN);
	return buf_date;
}
const char *cur_ctime()
{
	static char buf_time[HHCMMCSS_LEN+1];
	time_t cur_time = time(NULL);
	strlcpy__(buf_time, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	return buf_time;
}
const char *cur_hhmmss()
{
#define HHMMSS_LEN		6	// "235959"
	static char buf_time[YYYYMMDD_HHMMSS_LEN+1];
	time_t cur_time = time(NULL);
	strlcpy__(buf_time, &(get_yyyymmdd_hhmmss(cur_time, buf_time)[8+1]), HHMMSS_LEN);
	return buf_time;
}
const char *cur_yymmdd_hhmmss()
{
#define HHMMSS_LEN		6	// "235959"
	static char buf_time[YYYYMMDD_HHMMSS_LEN+1];
	time_t cur_time = time(NULL);
	strlcpy__(buf_time, &(get_yyyymmdd_hhmmss(cur_time, buf_time)[2]), YYMMDD_HHMMSS_LEN);
	return buf_time;
}
//------------------------------------------------------------------------------
char *get_ssspuuuuuu(char *buf)
{
	long usec = get_usec();
	snprintf_(buf, 10+1, "%03d.%06d",
	 (int)(usec / 1000000), (int)(usec % 1000000));
	return buf;
}
char *get_sssssspmmm(char *buf)
{
	long msec = get_msec();
	snprintf_(buf, 10+1, "%06d.%03d",
	 (int)(msec / 1000), (int)(msec % 1000));
	return buf;
}
unsigned long get_msec()
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	// 999999.999
	return (tv.tv_sec % 1000000) * 1000 + (tv.tv_usec / 1000) % 1000;
}
unsigned long get_usec()
{
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv, &tz);
	// 999.999999
	return (tv.tv_sec % 1000) * 1000000 + tv.tv_usec;
}
const char *cur_hhcmmcss_mmm()
{
#define HHCMMCSS_MMM_LEN		(8+1+3)
	static char buf_time[HHCMMCSS_MMM_LEN+1];
	struct timeval tv;
	struct timezone tz;
	time_t cur_time;
	char hhcmmcss[HHCMMCSS_LEN+1];

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec;
	strlcpy__(hhcmmcss, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	snprintf_(buf_time, HHCMMCSS_MMM_LEN+1, "%s.%03d",
	 hhcmmcss, (int)(tv.tv_usec / 1000 % 1000));
	return buf_time;
}
const char *cur_hhcmmcss_uuuuuu()
{
#define HHCMMCSS_UUUUUU_LEN		(8+1+6)
	static char buf_time[HHCMMCSS_UUUUUU_LEN+1];
	struct timeval tv;
	struct timezone tz;
	time_t cur_time;
	char hhcmmcss[HHCMMCSS_LEN+1];

	gettimeofday(&tv, &tz);
	cur_time = tv.tv_sec;
	strlcpy__(hhcmmcss, &(ctime(&cur_time)[11]), HHCMMCSS_LEN);
	snprintf_(buf_time, HHCMMCSS_UUUUUU_LEN+1, "%s.%06d", hhcmmcss, (int)tv.tv_usec);
	return buf_time;
}

PRIVATE char *get_yyyysmmsdd_hhcmmcss(time_t abs_time, char *buf)
{
	struct tm tm_;
	struct tm *tm;

	if (abs_time == 0) {
		strcpy__(buf, "----/--/-- --:--:--");
	} else {
		tm = localtime_r(&abs_time, &tm_);		// THREAD_SAFE
		snprintf_(buf, YYYYSMMSDD_HHCMMCSS_LEN+1, "%04d/%02d/%02d %02d:%02d:%02d",
		 1900 + tm->tm_year, (char)(tm->tm_mon+1), (char)(tm->tm_mday),
		 (char)(tm->tm_hour), (char)(tm->tm_min), (char)(tm->tm_sec));
	}
	return buf;
}
PRIVATE char *get_yyyymmdd_hhmmss(time_t abs_time, char *buf)
{
	struct tm tm_;
	struct tm *tm;

	if (abs_time == 0) {
		strcpy__(buf, "-------- ------");
	} else {
		tm = localtime_r(&abs_time, &tm_);		// THREAD_SAFE
		snprintf_(buf, YYYYMMDD_HHMMSS_LEN+1, "%04d%02d%02d-%02d%02d%02d",
		 1900 + tm->tm_year, (char)(tm->tm_mon+1), (char)(tm->tm_mday),
		 (char)(tm->tm_hour), (char)(tm->tm_min), (char)(tm->tm_sec));
	}
	return buf;
}

//------------------------------------------------------------------------------

int int_min(int aa, int bb)
{
	return aa <= bb ? aa : bb;
}
int int_max(int aa, int bb)
{
	return aa >= bb ? aa : bb;
}

//------------------------------------------------------------------------------

// /proc/meminfo
// MemTotal:        3917308 kB
// MemFree:         2777988 kB
int get_mem_free_in_kb(int update)
{
	static int kb = 256 * 1024;		// 256 MB

	if (update) {
		FILE *fp = fopen("/proc/meminfo", "r");
		if (fp != NULL) {
			char buffer[100+1];
			while (fgets(buffer, 100, fp) != 0) {
				if (strlcmp__(buffer, "MemFree:") == 0) {
					char buf[100+1];
					if (sscanf(buffer, "%10s %d", buf, &kb) >= 2) {
#ifdef ENABLE_DEBUG
////#define DEBUG_MEM_SHORTAGE		// define this to test the behavior on memory shortage
#ifdef DEBUG_MEM_SHORTAGE
						kb /= 100;		// 1GB ==> 10MB
#endif // DEBUG_MEM_SHORTAGE
#endif // ENABLE_DEBUG
						break;
					}
				}
			}
			if (fclose(fp) != 0) {
				// error
			}
		}
	}
	return kb;
}

//------------------------------------------------------------------------------

#ifdef START_UP_TEST
void test_zz_from_num()
{
#ifdef ENABLE_DEBUG
	char buf[2+1];
#endif // ENABLE_DEBUG

	// (10 + 26 + 26) * (10 + 26 + 26) = 62 * 62 = 3844
	for (int num = -20; num < (100 + 1040 + 2704) + 10; num++) {
		flf_d_printf("%d ==> [%s]\n", num, zz_from_num(num, buf));
	}
}
#endif // START_UP_TEST
// 00 - 99: 100   =  100
// 0A - 0z:
//   :
// 9A - 9z: 10*52 =  520
// A0 - Az:
//   :
// z0 - zz: 52*62 = 3224
// ---------------- 3844
char *zz_from_num(int num, char *buf)
{
	inline char AZaz_from_num(int num) { // [0, 51] ==> [A-Za-z]
		char chr = '_';
		if (num < 26) {
			chr = 'A' + num % 26;
		} else
		if (num < 26 + 26) {
			num -= 26;
			chr = 'a' + num % 26;
		}
		return chr;
	}
	inline char _09AZaz_from_num(int num) { // [0, 61] ==> [0-9A-Za-z]
		char chr = '_';
		if (num < 10) {
			chr = '0' + num % 10;
		} else
		if (num < 10 + 26 + 26) {
			num -= 10;
			chr = AZaz_from_num(num);
		}
		return chr;
	}
	if (num < -9) {
		snprintf_(buf, 2+1, "-@");			// -@
	} else if (num < 0) {
		snprintf_(buf, 2+1, "-%d", -num);	// -9 -- -1
	} else if (num < 100) {
		snprintf_(buf, 2+1, "%02d", num);	// 00 -- 99
	} else if (num < 100 + 520) {
		num -= 100;
		snprintf_(buf, 2+1, "%d%c", num / 52, AZaz_from_num(num % 52));	// 0A -- 0z, 9A -- 9z
	} else if (num < 100 + 520 + 3224) {
		num -= (100 + 520);
		snprintf_(buf, 2+1, "%c%c",
		 AZaz_from_num(num / 62), _09AZaz_from_num(num % 62));			// A0 -- Az, z0 -- zz
	} else {
		snprintf_(buf, 2+1, "%c%c", '@', '@');		// @@
	}
	return buf;
}

//------------------------------------------------------------------------------

#define USR_NAME_LEN	8
#define GRP_NAME_LEN	8
#define MAX_USERS	200
#define MAX_GROUPS	100

static struct uid_name {
	uid_t uid;
	char user_name[USR_NAME_LEN+1];
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
			return uid_name_cache[idx].user_name;
	}
	snprintf_(user_id, USER_ID_LEN+1, "%d", uid);
	return user_id;
}
const char *get_user_home_dir(const char *user_name)
{
	for (int idx = 0; idx < num_users; idx++) {
		if (strlcmp__(user_name, uid_name_cache[idx].user_name) == 0) {
			return uid_name_cache[idx].homedir;
		}
	}
	return user_name;
}
const char *get_group_name(gid_t gid)
{
	static char group_id[USER_ID_LEN+1];

	if (num_groups < 0) {
		cache_groups();
	}
	for (int idx = 0; idx < num_groups; idx++) {
		if (gid == gid_name_cache[idx].gid)
			return gid_name_cache[idx].grpname;
	}
	snprintf_(group_id, USER_ID_LEN+1, "%d", gid);
	return group_id;
}
int cache_users()
{
	struct passwd *pwent;

	// cache user name
	setpwent();
	for (num_users = 0; num_users < MAX_USERS; num_users++) {
		if ((pwent = getpwent()) == NULL)
			break;
		uid_name_cache[num_users].uid = pwent->pw_uid;
		strlcpy__(uid_name_cache[num_users].user_name, pwent->pw_name, USR_NAME_LEN);
		strlcpy__(uid_name_cache[num_users].homedir, pwent->pw_dir, MAX_PATH_LEN);
	}
	endpwent();
	return num_users;
}
int cache_groups()
{
	struct group *grent;

	// cache group name
	setgrent();
	for (num_groups = 0; num_groups < MAX_GROUPS; num_groups++) {
		if ((grent = getgrent()) == NULL)
			break;
		gid_name_cache[num_groups].gid = grent->gr_gid;
		strlcpy__(gid_name_cache[num_groups].grpname, grent->gr_name, GRP_NAME_LEN);
	}
	endgrent();
	return num_groups;
}

//------------------------------------------------------------------------------
const char *get_at_host_name()
{
#define HOST_NAME_LEN		20
#define USER_AT_HOST_NAME_LEN		(USER_ID_LEN + 1 + HOST_NAME_LEN)
	static char buf_user_at_host[USER_AT_HOST_NAME_LEN+1];
	// @hostname
	snprintf_(buf_user_at_host, USER_AT_HOST_NAME_LEN+1, "@%s", get_host_name());
	return buf_user_at_host;
}

const char *get_host_name()
{
	static char hostname[HOST_NAME_LEN+1];

	gethostname(hostname, HOST_NAME_LEN);
	return hostname;
}

//------------------------------------------------------------------------------
char *select_plural_form(char *singular, char *plural, char *type3, char *type4, int number)
{
	switch (get_plural_form_index(number)) {
	default:
	case 0:
		return singular;
	case 1:
		return plural;
	case 2:
		return type3;
	case 3:
		return type4;
	}
}

int get_plural_form_index(int number)
{
#ifdef ENABLE_NLS
#define LANG_STR_LEN	2
	enum lang {
		LANG_AR,	// = 0
		LANG_BG,
		LANG_CS,
		LANG_DA,
		LANG_DE,
		LANG_EN,
		LANG_ES,
		LANG_FR,
		LANG_FI,
		LANG_GA,
		LANG_EL,
		LANG_HR,
		LANG_HU,
		LANG_IS,
		LANG_IT,
		LANG_IW,
		LANG_JA,
		LANG_KO,
		LANG_LT,
		LANG_LV,
		LANG_NL,
		LANG_NO,
		LANG_PL,
		LANG_PT,
		LANG_RO,
		LANG_RU,
		LANG_SK,
		LANG_SL,
		LANG_SV,
		LANG_TH,
		LANG_TR,
		LANG_ZH,
		LANG_ZZ,
	};
	char lang_names[LANG_ZZ+1][LANG_STR_LEN+1] = {
		"ar",	// LANG_AR
		"bg",	// LANG_BG
		"cs",	// LANG_CS
		"da",	// LANG_DA
		"de",	// LANG_DE
		"en",	// LANG_EN
		"es",	// LANG_ES
		"fr",	// LANG_FR
		"fi",	// LANG_FI
		"ga",	// LANG_GA
		"el",	// LANG_EL
		"hr",	// LANG_HR
		"hu",	// LANG_HU
		"is",	// LANG_IS
		"it",	// LANG_IT
		"iw",	// LANG_IW
		"ja",	// LANG_JA
		"ko",	// LANG_KO
		"lt",	// LANG_LT
		"lv",	// LANG_LV
		"nl",	// LANG_NL
		"no",	// LANG_NO
		"pl",	// LANG_PL
		"pt",	// LANG_PT
		"ro",	// LANG_RO
		"ru",	// LANG_RU
		"sk",	// LANG_SK
		"sl",	// LANG_SL
		"sv",	// LANG_SV
		"th",	// LANG_TH
		"tr",	// LANG_TR
		"zh",	// LANG_ZH
		"--",	// LANG_ZZ
	};
	static char cur_lang[LANG_STR_LEN+1] = "";
	static enum lang cur_lang_idx = LANG_EN;

	if (cur_lang[0] == '\0') {
		// current language is not set, set cur_lang_idx by env "LANG"
		strlcpy__(cur_lang, getenv__("LANG"), LANG_STR_LEN);	// "ja_JP.UTF-8" ==> "ja"
		for (cur_lang_idx = 0; cur_lang_idx < LANG_ZZ; cur_lang_idx++) {
			if (strncmp(cur_lang, lang_names[cur_lang_idx], LANG_STR_LEN) == 0)
				break;
		}
		if (cur_lang_idx == LANG_ZZ) {
			cur_lang_idx = LANG_EN;
		}
	}

	switch (cur_lang_idx) {
	default:
	case LANG_EN:
	case LANG_DE:
		if (number == 1)
			return 0;
		return 1;
	case LANG_JA:
	case LANG_KO:
	case LANG_ZH:
		return 0;
	case LANG_FR:
		if (number <= 1)		// 0,1
			return 0;
		return 1;
	case LANG_LV:
		if (number % 10 == 1 && number % 100 != 11)
			return 0;			// 1,21,31,...91,101,121,131,...191,...
		if (number != 0)		// 2,3,4,...20,22,23,24,...
			return 1;
		return 2;				// 0
	case LANG_GA:
		if (number == 1)		// 1
			return 0;
		if (number == 2)		// 2
			return 1;
		return 2;				// 0,3,4,5,...
	case LANG_LT:
		if (number % 10 == 1 && number % 100 != 11)
			return 0;			// 1,21,31,...91,101,121,131,...191,...
		if (number % 10 >= 2 && ((number % 100 < 10) || (number % 100 >= 20)))
			return 1;			// 2,3,4,...9,22,23,24,...29,32,33,34,...39,...
		return 2;				// 0,10,11,12,13,...19,20,30,...
	case LANG_CS:
	case LANG_HR:
	case LANG_RU:
		if (number % 10 == 1 && number % 100 != 11)
			return 0;			// 1,21,31,...91,101,121,131,...191,...
		if ((number % 10 >= 2) && (number % 10 <= 4)
		 && ((number % 100 < 10) || (number % 100 >= 20)))
			return 1;			// 2,3,4,22,23,24,32,33,34,...
		return 2;				// 0,5,6,7,8,9,10,11,12,13,...19,20,30,...
	case LANG_PL:
		if (number == 1)		// 1
			return 0;
		if ((number % 10 >= 2) && (number % 10 <= 4)
		 && ((number % 100 < 10) || (number % 100 >= 20)))
			return 1;			// 2,3,4,22,23,24,32,33,34,...
		return 2;				// 0,5,6,7,8,9,10,11,12,13,...19,20,30,...
	case LANG_SL:
		if (number % 100 == 1)	// 1,101,201,...
			return 0;
		if (number % 100 == 2)	// 2,102,202,...
			return 1;
		if (number % 100 == 3)	// 3,103,203,...
			return 2;
		return 3;				// 0,4,5,6,7,8,9,10,12,13,...100,104,105,106,...
	}
#else
	if (number == 1)
		return 0;
	return 1;
#endif
}

// End of utils.c
