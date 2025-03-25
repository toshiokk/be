/**************************************************************************
 *   utils.h                                                              *
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

#ifndef utils_h
#define utils_h

#define PRIVATE		static

#define ARRAY_SIZE_OF(array)	(sizeof(array) / sizeof(array[0]))

#define SKIP_SPACE(ptr)		skip_space(&ptr)
#define IS_SPACE(ptr)		(*ptr == ' ' || *ptr == '\t')
#define IS_EOL(ptr)			(*ptr == '\n' || *ptr == '\0')

#define IDIV(xx, yy)	((xx) / ((yy) != 0 ? (yy) : 1))

#define BOOL_TO_ON_OFF(is_on)		((is_on) ? "ON" : "off")
#define SELECTED_OR_NOT(selected)	((selected) ? "<*>" : "< >")
#define BOOL_TO_0_1(is_on)			((is_on) ? "1" : "0")
#define BOOL_TO_1_2(is_on)			((is_on) ? "2" : "1")
#define BOOL_INV(is_on)				((is_on) ? 0 : 1)

#define FROM_IDX(idx)	((idx) + 1)		// get number from index
#define FROM_NUM(num)	((num) - 1)		// get index from number

//------------------------------------------------------------------------------

void set_die_on_callback(void (*die_on_cb)(const char *msg));
void die_on(const char *msg);

// Macros for STDC library
#define char_malloc(bytes)			(char *)malloc__(bytes)
#define char_remalloc(ptr, bytes)	(char *)remalloc__(ptr, bytes)

////#define MEMORY_LEAK_CHECKER
#ifndef MEMORY_LEAK_CHECKER

#define _mlc_init
#define _mlc_set_caller
#define _mlc_check_leak
#define _mlc_check_count
#define _mlc_memorize_count
#define _mlc_differ_count

#else // MEMORY_LEAK_CHECKER

#define _mlc_init				mlc_init();
#define _mlc_set_caller			mlc_set_caller(__FILE__, __LINE__);
#define _mlc_check_leak			mlc_check_leak();
#define _mlc_check_count		mlc_check_count();
#define _mlc_memorize_count		mlc_memorize_count();
#define _mlc_differ_count		mlc_differenciate_count();

struct malloc_caller {
	const char *file_name;	// "filename.c"
	int line_num;			// line number
};
struct malloc_header {
	size_t size;		// size which user requested and allocated
	struct malloc_caller *caller;
};
#define MAX_MALLOCS_TO_MONITOR	2000000	// 2,000,000

void mlc_init();
void mlc_set_caller(const char *file_name, int line_num);
struct malloc_caller* mlc_register_caller();
void mlc_update_caller(struct malloc_caller* caller);
void mlc_clear_caller(struct malloc_caller* caller);
void mlc_check_leak();

void mlc_check_count();
void mlc_memorize_count();
void mlc_differenciate_count();
#endif // MEMORY_LEAK_CHECKER

char *malloc_strcpy(const char *string);
void *malloc__(size_t bytes);
void *remalloc__(void *ptr, size_t bytes);
#define FREE_CLR_PTR(ptr)	free_ptr((void **)&(ptr))
#define FREE_CLR_PTR_FLF(ptr)	{ if (ptr) { _FLF_ } free_ptr((void **)&(ptr)); }
void free_ptr(void **ptr);
void free__(void *ptr);

void remalloc_and_null_terminate_string(char **ptr);
void remalloc_and_null_terminate_length(char **ptr, size_t len);

//------------------------------------------------------------------------------

void clear_crc16ccitt();
void calc_crc16ccitt(UINT8 byte);
UINT16 get_crc16ccitt();

//------------------------------------------------------------------------------
#define YYSMMSDD_LEN			8	// "24/12/31"
#define HHCMMCSS_LEN			8	// "23:59:59"
#define HHCMMCSS_YYSMMSDD_LEN	8	// "23:59:59" / "24/10/09"
const char *cur_ctime_cdate(int time0_date1);
const char *cur_cdate(void);
const char *cur_ctime(void);
const char *cur_hhmmss(void);
const char *cur_yymmdd_hhmmss(void);

#define USEC_VAR	long usec = get_usec();
#define USEC_BEGIN	usec = get_usec();
#define USEC_END	flf_d_printf("%d[usec]\n", get_usec() - usec); USEC_BEGIN
char *get_ssspuuuuuu(char *buf);
char *get_sssssspmmm(char *buf);
unsigned long get_msec(void);
unsigned long get_usec(void);
const char *cur_hhcmmcss_mmm(void);
const char *cur_hhcmmcss_uuuuuu(void);

#define MSLEEP(msec)		usleep((msec) * 1000);

//------------------------------------------------------------------------------

#define MIN_(aa, bb)		int_min((aa), (bb))
#define MAX_(aa, bb)		int_max((aa), (bb))
#define LIM_MAX(aa, bb)		int_min((aa), (bb))
#define LIM_MIN(aa, bb)		int_max((aa), (bb))
#define MIN_MAX_(min, value, max)	MAX_((min), MIN_((max), (value)))

#define IS_IN_RANGE(min, value, max)	((min) <= (value) && (value) < (max))
#define IS_BETWEEN(min, value, max)		((min) <= (value) && (value) <= (max))
#define IS_OVERWRAP(left, right, min, max)	\
	(IS_IN_RANGE(left, min, right) || IS_IN_RANGE(left, max-1, right))
#define MK_IN_RANGE(min, value, max)	MIN_MAX_((min), (value), (max-1))

int int_min(int aa, int bb);
int int_max(int aa, int bb);

int get_mem_free_in_kb(int update);

#ifdef START_UP_TEST
void test_zz_from_num(void);
#endif // START_UP_TEST
char *zz_from_num(int num, char *buf);

const char *get_user_name(uid_t uid);
const char *get_user_home_dir(const char *user_name);
const char *get_group_name(gid_t gid);
int cache_users(void);
int cache_groups(void);

const char *get_at_host_name();
const char *get_host_name();

char *select_plural_form(char *singular, char *plural, char *type3, char *type4, int number);
int get_plural_form_index(int number);

#endif // utils_h

// End of utils.h
