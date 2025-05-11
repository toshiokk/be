/**************************************************************************
 *   regexp.c                                                             *
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

void search_clear(search_t *search)
{
	strcpy__(search->needle, "");
}
int search_is_needle_set(search_t *search)
{
	if ((strlen(search->needle) == 4)
	 && (search->needle[0] == '[') && (search->needle[3] == ']')) {
		return 2;	// Bracket search "[()]"
	}
	if (strlen(search->needle)) {
		return 1;		// Not bracket search (ordinaly search)
	}
	return 0;			// needle not set
}

int search_str_in_line(search_t *search, matches_t *matches,
 const char *needle, int search_dir, int ignore_case, const char *haystack, int byte_idx)
{
	int match_len;

	if (needle) {
		strlcpy__(search->needle, needle, MAX_PATH_LEN);
		search->ignore_case = ignore_case;
	}
#ifdef ENABLE_REGEX
	if (GET_APPMD(ed_USE_REGEXP) == 0) {
#endif // ENABLE_REGEX
		match_len = search_str_in_line_strstr(search->needle, matches,
		 search_dir, search->ignore_case, haystack, byte_idx);
#ifdef ENABLE_REGEX
	} else {
		match_len = search_str_in_line_regexp(&search->regexp, &matches->regexp_matches,
		 needle, search_dir, ignore_case, haystack, byte_idx);
		matches_set_start_idx(matches,
		 regexp_matches_start_idx(&matches->regexp_matches, 0));
		matches_set_end_idx(matches,
		 regexp_matches_end_idx(&matches->regexp_matches, 0));
	}
#endif // ENABLE_REGEX
	return match_len;	// >0: match
}

//------------------------------------------------------------------------------

void matches_clear(matches_t *matches)
{
	matches->so = 0;
	matches->eo = 0;
#ifdef ENABLE_REGEX
	regexp_matches_clear(&matches->regexp_matches);
#endif // ENABLE_REGEX
}
void matches_set_start_idx(matches_t *matches, int byte_idx)
{
	matches->so = byte_idx;
#ifdef ENABLE_REGEX
	regexp_matches_set_start_idx(&matches->regexp_matches, 0, byte_idx);
#endif // ENABLE_REGEX
}
void matches_set_end_idx(matches_t *matches, int byte_idx)
{
	matches->eo = byte_idx;
#ifdef ENABLE_REGEX
	regexp_matches_set_end_idx(&matches->regexp_matches, 0, byte_idx);
#endif // ENABLE_REGEX
}
void matches_set_match_len(matches_t *matches, int match_len)
{
	matches->eo = matches->so + match_len;
#ifdef ENABLE_REGEX
	regexp_matches_set_match_len(&matches->regexp_matches, 0, match_len);
#endif // ENABLE_REGEX
}
int matches_start_idx(matches_t *matches)
{
	return matches->so;
}
int matches_end_idx(matches_t *matches)
{
	return matches->eo;
}
int matches_match_len(matches_t *matches)
{
	return matches->eo - matches->so;
}

void matches_dump_matches(matches_t *matches)
{
	flf_d_printf("so: %d\n", matches_start_idx(matches));
	flf_d_printf("eo: %d\n", matches_end_idx(matches));
	flf_d_printf("match_len: %d\n", matches_match_len(matches));
}

//------------------------------------------------------------------------------

#ifdef ENABLE_REGEX
int search_str_in_line_regexp(regexp_t *regexp, regexp_matches_t *regexp_matches,
 const char *needle, int search_dir, int ignore_case, const char *haystack, int byte_idx)
{
	int cflags = ignore_case ? REG_ICASE : 0;

	if (search_dir < 0) {
		// backward search. The last match is what we need.
		// First, search from the line head
		int ret = regexp_search(regexp, regexp_matches, needle,
		 haystack, 0, cflags, REG_NONE, 1);
		if (ret == 0 && regexp_matches_start_idx(regexp_matches, 0) <= byte_idx) {
			// Search forward until there is no more match.
			int prev_match_byte_idx;
			for ( ; ; ) {
				prev_match_byte_idx = regexp_matches_start_idx(regexp_matches, 0);
				// Second, search from the next byte of previous match pos
				ret = regexp_search(regexp, regexp_matches, needle,
				 haystack, regexp_matches_start_idx(regexp_matches, 0) + 1, cflags, REG_NONE, 1);
				if (ret == 0 && regexp_matches_start_idx(regexp_matches, 0) <= byte_idx) {
					// continue;
				} else {
					break;
				}
			}
			// Finally, put the subexpression matches in regexp_matches.
			// The REG_NOTBOL flag doesn't matter now.
			ret = regexp_search(regexp, regexp_matches, needle,
			 haystack, prev_match_byte_idx, cflags, REG_NONE, MAX_REGEX_MATCHES);
			if (ret == 0 && regexp_matches_start_idx(regexp_matches, 0) <= byte_idx) {
				return regexp_matches_match_len(regexp_matches, 0);
			}
		}
	} else {
		// forward search. The first match is what we need.
		// if byte_idx == 0, haystack is line top
		// if byte_idx > 0, haystack is not line top and add REG_NOTBOL eflag
		if (regexp_search(regexp, regexp_matches, needle,
		 haystack, byte_idx, cflags, REG_NONE, MAX_REGEX_MATCHES) == 0) {
			// Put the subexpression matches in regexp_matches.
			return regexp_matches_match_len(regexp_matches, 0);
		}
	}
	return 0;		// not found
}
#endif // ENABLE_REGEX

//------------------------------------------------------------------------------
PRIVATE const char *revstrstr(const char *haystack, const char *tail_of_haystack,
 const char *needle);
PRIVATE const char *revstristr(const char *haystack, const char *tail_of_haystack,
 const char *needle);
PRIVATE const char *stristr(const char *haystack, const char *needle);

int search_str_in_line_strstr(const char *needle, matches_t *matches,
 int search_dir, int ignore_case, const char *haystack, int byte_idx)
{
	const char *match_start;
	matches_clear(matches);
	if (search_dir < 0) {
		if (ignore_case == 0)
			match_start = revstrstr(haystack, &haystack[byte_idx], needle);
		else
			match_start = revstristr(haystack, &haystack[byte_idx], needle);
	} else {
		if (ignore_case == 0)
			match_start = strstr(&haystack[byte_idx], needle);
		else
			match_start = stristr(&haystack[byte_idx], needle);
	}
	if (match_start) {
		matches_set_start_idx(matches, match_start - haystack);
		matches_set_match_len(matches, strlen(needle));
	}
	return matches_match_len(matches);
}
PRIVATE const char *revstrstr(const char *haystack, const char *tail_of_haystack,
 const char *needle)
{
	const char *hh, *nn;

	for ( ; tail_of_haystack >= haystack; tail_of_haystack--) {
		for (hh = tail_of_haystack, nn = needle;
		 *nn && (*hh == *nn);
		 hh++, nn++) {
			// nothing to do
		}
		if (*nn == '\0')
			return tail_of_haystack;
	}
	return NULL;
}
PRIVATE const char *revstristr(const char *haystack, const char *tail_of_haystack,
 const char *needle)
{
	const char *hh, *nn = needle;

	for ( ; tail_of_haystack >= haystack; tail_of_haystack--) {
		for (hh = tail_of_haystack, nn = needle;
		 *nn && (tolower(*hh) == tolower(*nn));
		 hh++, nn++) {
			// nothing to do
		}
		if (*nn == '\0')
			return tail_of_haystack;
	}
	return NULL;
}
// This is now mutt's version (called mutt_stristr) because it doesn't
// use memory allocation to do a simple search (yuck).
PRIVATE const char *stristr(const char *haystack, const char *needle)
{
	const char *hh, *nn;

	if (haystack == NULL)
		return NULL;
	if (needle == NULL)
		return haystack;
	
	for ( ; *haystack; haystack++) {
		for (hh = haystack, nn = needle;
		 *hh && *nn && (tolower(*hh) == tolower(*nn));
		 hh++, nn++) {
			// nothing to do
		}
		if (*nn == 0)
			return haystack;
	}
	return NULL;
}

#ifdef ENABLE_REGEX

// Regular expression helper functions ----------------------------------------

#ifdef START_UP_TEST
void test_regexp()
{
	regexp_t regexp;
	regexp_matches_t regexp_matches;
	const char *needle;
	const char *replace_to;
	const char *haystack;
	char output[MAX_EDIT_LINE_LEN+1];
	int ret;

	flf_d_printf("---------------------------------------------------------\n");
#ifdef USE_PCRE
	flf_d_printf("regexp library: PCRE\n");
#else // USE_PCRE
	flf_d_printf("regexp library: glibc POSIX ERE(Extended Regular Expression)\n");
#endif // USE_PCRE

	regexp_init(&regexp);

	// test tab, EOL match
	needle = "\\t\\n";
	haystack = "aaa\t\nbbbtnccc";
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);
	MY_UT_INT(ret, 0);
	MY_UT_INT(regexp.regex_compiled.re_nsub, 0);
	MY_UT_INT(regexp_matches_start_idx(&regexp_matches, 0), 3);
	MY_UT_INT(regexp_matches_end_idx(&regexp_matches, 0), 5);

	needle = "\\t";
	haystack = "aaa\t\nbbbtnccc";
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);
	MY_UT_INT(ret, 0);
	MY_UT_INT(regexp.regex_compiled.re_nsub, 0);
	MY_UT_INT(regexp_matches_start_idx(&regexp_matches, 0), 3);
	MY_UT_INT(regexp_matches_end_idx(&regexp_matches, 0), 4);

	needle = "\\n";
	haystack = "aaa\t\nbbbtnccc";
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);
	MY_UT_INT(ret, 0);
	MY_UT_INT(regexp.regex_compiled.re_nsub, 0);
	MY_UT_INT(regexp_matches_start_idx(&regexp_matches, 0), 4);
	MY_UT_INT(regexp_matches_end_idx(&regexp_matches, 0), 5);

	// test replacement in sub-expression
	// REGEX: "abc(def|ghi)(jkl|mno)pqr" REPLACE-TO: "123\\2\\1456"
	// INPUT-TEXT: "abcghijklpqr" OUTPUT-TEXT: "123jklghi456"
	needle = "abc(def|ghi)(jkl|mno)pqr";
	haystack = "xxxxxabcghijklpqryyyyy";
	strlcpy__(output, haystack, MAX_EDIT_LINE_LEN);
	replace_to = "123\\2\\1456";
	ret = regexp_search(&regexp, &regexp_matches, needle, haystack, 0, REG_NONE, REG_NONE, 3);
	regexp_replace(&regexp, &regexp_matches, output, MAX_EDIT_LINE_LEN, replace_to);
	flf_d_printf("ret: %d\n", ret);
	regexp_dump_matches(&regexp, &regexp_matches, haystack);
	flf_d_printf("needle: [%s]\n", needle);
	flf_d_printf("replace_to: [%s]\n", replace_to);
	flf_d_printf("before: [%s]\n", haystack);
	flf_d_printf("after : [%s]\n", output);
	MY_UT_INT(ret, 0);
	MY_UT_INT(regexp.regex_compiled.re_nsub, 2);
	MY_UT_INT(regexp_matches_start_idx(&regexp_matches, 0), 5);
	MY_UT_INT(regexp_matches_end_idx(&regexp_matches, 0), 17);
	MY_UT_INT(regexp_matches_start_idx(&regexp_matches, 1), 8);
	MY_UT_INT(regexp_matches_end_idx(&regexp_matches, 1), 11);
	MY_UT_INT(regexp_matches_start_idx(&regexp_matches, 2), 11);
	MY_UT_INT(regexp_matches_end_idx(&regexp_matches, 2), 14);
	MY_UT_STR(output, "xxxxx123jklghi456yyyyy");
	flf_d_printf("---------------------------------------------------------\n");
}
#endif // START_UP_TEST

regexp_t *regexp_alloc()
{
	regexp_t *regexp;

	_mlc_set_caller
	regexp = (regexp_t*)malloc__(sizeof(regexp_t));
	return regexp_init(regexp);
}
regexp_t *regexp_init(regexp_t *regexp)
{
	memset(regexp, 0x00, sizeof(regexp_t));
	return regexp;
}
void regexp_free_regex_compiled(regexp_t *regexp)
{
	if (regexp) {
		if (regexp->is_compiled == 0)
			return;
		regexp->is_compiled = 0;
		regfree__(&regexp->regex_compiled);
	}
}
void regexp_free(regexp_t *regexp)
{
	if (regexp) {
		regexp_free_regex_compiled(regexp);
		free__(regexp);
	}
}
int regexp_search(regexp_t *regexp, regexp_matches_t *regexp_matches, const char *needle,
 const char *haystack, int byte_idx, int cflags, int eflags, size_t max_matches)
{
	int ret;

	regexp_matches_clear(regexp_matches);
	if (needle) {
		// auto compile regexp
		if (regexp_compile(regexp, needle, cflags))
			return -2;		// -2: compilation error
	}
	ret = regexp_search_compiled(regexp, haystack, byte_idx,
	 eflags, regexp_matches, max_matches);
	return ret;				// 0: match, -1:no match
}
int regexp_compile(regexp_t *regexp, const char *needle, int cflags)
{
	if (regexp->is_compiled) {
		if (strcmp(needle, regexp->needle_compiled) != 0
		 || cflags != regexp->cflags_compiled) {
			regexp_free_regex_compiled(regexp);
		}
	}
	if (regexp->is_compiled == 0) {
		strlcpy__(regexp->needle_compiled, needle, MAX_PATH_LEN);
		regexp->cflags_compiled = cflags;
		if ((regexp->error = regcomp__(&regexp->regex_compiled, needle, cflags)) != 0) {
			regerror(regexp->error, &regexp->regex_compiled,
			 regexp->regex_err_msg, MAX_PATH_LEN);
			regexp->is_compiled = 1;	// compilation error
		} else {
			regexp->is_compiled = 2;	// compiled
		}
	}
	return regexp->is_compiled == 2 ? 0 : -1;	// 0: compilation succeeded, -1: compilation error
}
int regexp_search_compiled(regexp_t *regexp, const char *haystack, int byte_idx,
 int eflags, regexp_matches_t *regexp_matches, size_t max_matches)
{
	int ret;

	if (regexp->is_compiled < 2) {
		return -1;	// compilation error
	}
	if ((ret = regexec_n(&regexp->regex_compiled, &haystack[byte_idx],
	 max_matches, regexp_matches->matches,
	 eflags | (byte_idx == 0 ? REG_NONE : REG_NOTBOL))) == 0) {
		regexp_matches_set_start_idx(regexp_matches, 0,
		 byte_idx + regexp_matches_start_idx(regexp_matches, 0));
		regexp_matches_set_end_idx(regexp_matches, 0,
		 byte_idx + regexp_matches_end_idx(regexp_matches, 0));
	}
	return ret;		// 0: match
}
#ifdef ENABLE_DEBUG
void regexp_dump_matches(regexp_t *regexp, regexp_matches_t *regexp_matches,
 const char *haystack)
{
	int match_idx;
	char buf1[MAX_SCRN_LINE_BUF_LEN+1];
	char buf2[MAX_SCRN_LINE_BUF_LEN+1];

	flf_d_printf("needle[%s]\n", regexp->needle_compiled);
	flf_d_printf("haystack[%s]\n", dump_str(haystack, buf1));
	if (regexp_matches_match_len(regexp_matches, 0) == 0) {
		flf_d_printf("match_len: %d\n", regexp_matches_match_len(regexp_matches, 0));
		return;
	}
	for (match_idx = 0; match_idx <= regexp->regex_compiled.re_nsub; match_idx++) {
		if (regexp_matches_match_len(regexp_matches, match_idx) > 0) {
			flf_d_printf("regexp_matches[%d]=%d,%d[%s]\n", match_idx,
			 regexp_matches_start_idx(regexp_matches, match_idx),
			 regexp_matches_end_idx(regexp_matches, match_idx),
			 dump_str(strcut__(buf1, MAX_SCRN_LINE_BUF_LEN, haystack,
			  regexp_matches_start_idx(regexp_matches, match_idx),
			  regexp_matches_end_idx(regexp_matches, match_idx)), buf2));
		}
	}
}
#endif // ENABLE_DEBUG
// replace including sub expressions
int regexp_replace(regexp_t *regexp, regexp_matches_t *regexp_matches,
 char *buffer, int buf_len, const char *replace_to)
{
	char output[MAX_EDIT_LINE_LEN+1];
	char *output_ptr;
	const char *replace_ptr;
	int subexp_num;		// sub-expression number(1--9)
	int match_len;

	// Iterate through the replacement text to handle subexpression replacement
	// using \1, \2, \3, etc.
	for (output_ptr = output, replace_ptr = replace_to;
	 *replace_ptr && output_ptr - output < buf_len; ) {
		if (*replace_ptr != '\\') {
			*output_ptr++ = *replace_ptr++;
		} else {
			subexp_num = replace_ptr[1] - '0';	// "\1" ==> 1
			if (1 <= subexp_num && subexp_num <= 9
			 && subexp_num <= regexp->regex_compiled.re_nsub) {
				replace_ptr += 2;	// Skip over the replacement expression (e.g. "\1")
				match_len = regexp_matches_match_len(regexp_matches, subexp_num);
				if (output_ptr - output + match_len < buf_len) {
					strlcpy__(output_ptr,
					 &buffer[regexp_matches_start_idx(regexp_matches, subexp_num)],
					 match_len);
					output_ptr += match_len;
				}
			} else {
				replace_ptr++;	// Skip over "\"
				*output_ptr++ = *replace_ptr++;
			}
		}
	}
	*output_ptr = '\0';
	replace_str(buffer, buf_len,
	 regexp_matches_start_idx(regexp_matches, 0),
	 regexp_matches_match_len(regexp_matches, 0),
	 output, -1);
	return output_ptr - output;	// return length of change
}

//------------------------------------------------------------------------------
void regexp_matches_clear(regexp_matches_t *regexp_matches)
{
#if 0
	for (int match_idx = 0; match_idx < MAX_REGEX_MATCHES; match_idx__) {
		regexp_matches->matches[match_idx].rm_so = 0;
		regexp_matches->matches[match_idx].rm_eo = 0;
	}
#else
	memset(regexp_matches, 0x00, sizeof(*regexp_matches));
#endif
}
void regexp_matches_set_start_idx(regexp_matches_t *regexp_matches, int match_idx, int byte_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	regexp_matches->matches[match_idx].rm_so = byte_idx;
}
void regexp_matches_set_end_idx(regexp_matches_t *regexp_matches, int match_idx, int byte_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	regexp_matches->matches[match_idx].rm_eo = byte_idx;
}
void regexp_matches_set_match_len(regexp_matches_t *regexp_matches, int match_idx, int match_len)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	regexp_matches->matches[match_idx].rm_eo
	 = regexp_matches->matches[match_idx].rm_so + match_len;
}
int regexp_matches_start_idx(regexp_matches_t *regexp_matches, int match_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	return regexp_matches->matches[match_idx].rm_so;
}
int regexp_matches_end_idx(regexp_matches_t *regexp_matches, int match_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	return regexp_matches->matches[match_idx].rm_eo;
}
int regexp_matches_match_len(regexp_matches_t *regexp_matches, int match_idx)
{
	if (IS_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES) == 0) {
		_WARNING_
		match_idx = MK_IN_RANGE(0, match_idx, MAX_REGEX_MATCHES);
	}
	return regexp_matches->matches[match_idx].rm_eo
	 - regexp_matches->matches[match_idx].rm_so;
}

//------------------------------------------------------------------------------
#ifndef USE_PCRE
PRIVATE char *conv_regex_pcre_to_posix(const char *regexp, char *regex_buf);
#endif // USE_PCRE

// Compile the regular expression regexp to preg.
// Returns 0 on success, non-zero if the expression is invalid.
int regcomp__(regex_t *preg, const char *regexp, int cflags)
{
	char buf[MAX_PATH_LEN+1];
#ifndef USE_PCRE
	regexp = conv_regex_pcre_to_posix(regexp, buf);
#endif // USE_PCRE
	int ret = regcomp(preg, regexp, REG_EXTENDED | cflags);	
	if (ret) {
		regerror(ret, preg, buf, MAX_PATH_LEN);
		warning_printf("regerror: [%s]\n", buf);
	}
	return ret;
}

int regexec_1(const regex_t *preg, const char *haystack,
 regmatch_t pmatch[], int eflags)
{
	int ret = regexec_n(preg, haystack, 1, pmatch, eflags);
	return ret;
}

int regexec_n(const regex_t *preg, const char *haystack,
 size_t nmatch, regmatch_t pmatch[], int eflags)
{
	if (haystack != NULL && *haystack != '\0') {
		int ret = regexec(preg, haystack, nmatch, pmatch, eflags);
		if (ret == 0 && pmatch) {
			// check match-length != 0
			if (pmatch[0].rm_so == pmatch[0].rm_eo) {
				pmatch[0].rm_eo++;
			}
		}
		return ret;
	}
	return REG_NOMATCH;
}

void regfree__(regex_t *preg)
{
	if (preg) {
		regfree(preg);
	}
}

//------------------------------------------------------------------------------
#ifndef USE_PCRE
// GNU C library's POSIX regex implementation does not support escape sequence such as "\t".
// So convert two bytes string "\t" to one byte character '\t'.
PRIVATE char *conv_regex_pcre_to_posix(const char *regexp, char *regex_buf)
{
	char *dest = regex_buf;
	for (const char *src = regexp; *src && (dest - regex_buf < MAX_PATH_LEN-1); src++) {
		switch (*src) {
		case '\\':
			switch (*(src+1)) {
			case 't':
				*dest++ = '\t';		// "\t" ==> '\t'
				src++;
				break;
			case 'n':
				*dest++ = '\n';		// "\n" ==> '\n'
				src++;
				break;
			default:
				*dest++ = *src++;	// "\x" ==> "\x"
				*dest++ = *src;
				break;
			}
			break;
		default:
			*dest++ = *src;
			break;
		}
	}
	*dest = '\0';
	return regex_buf;
}
#endif // USE_PCRE

#endif // ENABLE_REGEX

// End of regexp.c
