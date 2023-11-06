/**************************************************************************
 *   regexp.h                                                             *
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

#ifndef regexp_h
#define regexp_h

#define BACKWARD_DIR		(-1)
#define FORWARD_DIR			(+1)
#define BACKWARD_SEARCH		BACKWARD_DIR
#define FORWARD_SEARCH		FORWARD_DIR

#define IGNORE_CASE			1
#define CASE_SENSITIVE		0

#define NO_SKIP_HERE		0
#define SKIP_HERE			1

#ifdef ENABLE_REGEX

typedef struct {
	int is_compiled;						// 0: not-compiled
											// 1: error-in-compilation
											// 2: successfully-compiled
	char needle_compiled[MAX_PATH_LEN+1];	// compiled regexp string
	int cflags_compiled;					// compiled condition of ignore_case
	regex_t regex_compiled;					// compiled regexp
	int error;								// error num returned from regcomp()
	char regex_err_msg[MAX_PATH_LEN+1];		// error message returned from regerror()
} regexp_t;

// match positions for parenthetical subexpressions, max of 10
typedef struct {
#define MAX_REGEX_MATCHES		10		// 1 main and 9 sub-expressions
	regmatch_t matches[MAX_REGEX_MATCHES];
} regexp_matches_t;

#endif // ENABLE_REGEX

typedef struct {
	char needle[MAX_PATH_LEN+1];	// compiled regexp string
	int direction;
	int ignore_case;
#ifdef ENABLE_REGEX
	regexp_t regexp;
#endif // ENABLE_REGEX
} search_t;

typedef struct {
	int so;
	int eo;
#ifdef ENABLE_REGEX
	regexp_matches_t regexp_matches;
#endif // ENABLE_REGEX
} matches_t;

void search_clear(search_t *search);
int search_is_needle_set(search_t *search);
int search_str_in_line(search_t *search, matches_t *matches,
 const char *needle, int search_dir, int ignore_case, const char *haystack, int byte_idx);
int search_str_in_line_strstr(const char *needle, matches_t *matches,
 int search_dir, int ignore_case, const char *haystack, int byte_idx);
#ifdef ENABLE_REGEX
int search_str_in_line_regexp(regexp_t *regexp, regexp_matches_t *regexp_matches,
 const char *needle, int search_dir, int ignore_case,
 const char *haystack, int byte_idx);
#endif // ENABLE_REGEX

void matches_clear(matches_t *matches);
void matches_set_start_idx(matches_t *matches, int byte_idx);
void matches_set_end_idx(matches_t *matches, int byte_idx);
void matches_set_match_len(matches_t *matches, int match_len);
int matches_start_idx(matches_t *matches);
int matches_end_idx(matches_t *matches);
int matches_match_len(matches_t *matches);

void matches_dump_matches(matches_t *matches);

#ifdef ENABLE_REGEX

#ifdef START_UP_TEST
void test_regexp(void);
#endif // START_UP_TEST
regexp_t *regexp_alloc(void);
regexp_t *regexp_init(regexp_t *regexp);
int regexp_free_regex_compiled(regexp_t *regexp);
int regexp_free(regexp_t *regexp);

int regexp_search(regexp_t *regexp, regexp_matches_t *regexp_matches, const char *needle,
 const char *haystack, int byte_idx, int cflags, int eflags, size_t max_matches);
int regexp_compile(regexp_t *regexp, const char *needle, int cflags);
int regexp_search_compiled(regexp_t *regexp, const char *haystack, int byte_idx,
 int eflags, regexp_matches_t *regexp_matches, size_t max_matches);
#ifdef ENABLE_DEBUG
void regexp_dump_matches(regexp_t *regexp, regexp_matches_t *regexp_matches,
 const char *haystack);
#endif // ENABLE_DEBUG
int regexp_replace(regexp_t *regexp, regexp_matches_t *regexp_matches,
 char *buffer, int buf_len, const char *replace_to);

//------------------------------------------------------------------------------

void regexp_matches_clear(regexp_matches_t *regexp_matches);
void regexp_matches_set_start_idx(regexp_matches_t *regexp_matches, int match_idx,
 int byte_idx);
void regexp_matches_set_end_idx(regexp_matches_t *regexp_matches, int match_idx,
 int byte_idx);
void regexp_matches_set_match_len(regexp_matches_t *regexp_matches, int match_idx,
 int match_len);
int regexp_matches_start_idx(regexp_matches_t *regexp_matches, int match_idx);
int regexp_matches_end_idx(regexp_matches_t *regexp_matches, int match_idx);
int regexp_matches_match_len(regexp_matches_t *regexp_matches, int match_idx);

//------------------------------------------------------------------------------

#define REG_NONE	0	// for cflags(compile) and eflags(execute)

int regcomp__(regex_t *preg, const char *regexp, int cflags);
int regexec_1(const regex_t *preg, const char *haystack,
 regmatch_t pmatch[], int eflags);
int regexec_n(const regex_t *preg, const char *haystack,
 size_t nmatch, regmatch_t pmatch[], int eflags);
int regfree__(regex_t *preg);

#endif // ENABLE_REGEX

#endif // regexp_h

// End of regexp.h
