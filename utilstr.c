/**************************************************************************
 *   utilstr.c                                                            *
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

#define STR_BUF_LEN		4096

// [0x00, 0x1f], 0x7f
int is_ctrl_char(unsigned char uchar)
{
	int chr;

	chr = uchar;
	return (0x00 <= chr && chr < 0x20) || (0x7f <= chr && chr < 0x80);
}

//-----------------------------------------------------------------------------

#ifndef HAVE_STRCASECMP
/* This function is equivalent to strcasecmp(). */
int my_stricmp(const char *s1, const char *s2)
{
	for ( ; *s1 && *s2; s1++, s2++) {
		if (tolower(*s1) != tolower(*s2))
			break;
	}
	return (tolower(*s1) - tolower(*s2));
}
#endif
#ifndef HAVE_STRNCASECMP
/* This function is equivalent to strncasecmp(). */
int my_strnicmp(const char *s1, const char *s2, size_t n)
{
	for ( ; n > 0 && *s1 && *s2; n--, s1++, s2++) {
		if (tolower(*s1) != tolower(*s2))
			break;
	}
	if (n > 0)
		return (tolower(*s1) - tolower(*s2));
	return 0;
}
#endif

//-----------------------------------------------------------------------------

char *conv_esc_str(char *string)
{
	char *src;
	char *dest;
	char buf[MAX_PATH_LEN+1];

	dest = buf;
	for (src = string; *src; src++) {
		switch (*src) {
		case '\\':
			switch (*(src+1)) {
			case 'a':
			case 'r':
			case 't':
				switch (*(src+1)) {
				case 'a':
					*dest = '\a';
					break;
				case 't':
					*dest = '\t';
					break;
				case 'r':
					*dest = '\r';
					break;
				}
				src++;
				dest++;
				break;
			}
			break;
		default:
			*dest++ = *src;
			break;
		}
	}
	*dest = '\0';
	strlcpy__(string, buf, MAX_PATH_LEN);
	return string;
}

//-----------------------------------------------------------------------------
#ifdef START_UP_TEST
void test_replace_str(void)
{
	char buffer[100+1];

	strcpy(buffer, "abcdefghijklmnop");
	flf_d_printf("%s\n", buffer);
	flf_d_printf("%s\n", replace_str(buffer, 100, 7, 4, "HIJK", 4));
	strcpy(buffer, "abcdefghijklmnop");
	flf_d_printf("%s\n", replace_str(buffer, 100, 7, 4, "", 0));
	strcpy(buffer, "abcdefghijklmnop");
	flf_d_printf("%s\n", replace_str(buffer, 100, 7, 0, "HIJK", 4));
}
#endif // START_UP_TEST

char *replace_str(char *buffer, size_t buf_len, size_t start, int delete_len,
 const char *str, len_t insert_len)
{
	if (delete_len > 0) {
		delete_str(buffer, start, delete_len);
	}
	if (insert_len) {	// (insert_len > 0) or (insert_len == -1)
		insert_str(buffer, buf_len, start, str, insert_len);
	}
	return buffer;
}

int insert_str_separating_by_space(char *buffer, size_t buf_len, size_t offset,
 const char *str)
{
///	offset = strnlen(buffer, buf_len);
	if (offset) {
		// not head of line
		if (buffer[offset-1] != ' ') {
			// "abcdef" ==> "abc def"
			//     ^             ^
			insert_str(buffer, buf_len, offset, " ", -1);
			offset++;
		}
	}
	if (buffer[offset]) {
		// not tail of line
		if (buffer[offset] != ' ') {
			// "abc def" ==> "abc  def"
			//      ^             ^
			insert_str(buffer, buf_len, offset, " ", -1);
		}
	}
	// "abc  def" ==> "abc xyz def"
	//      ^              ^
	insert_str(buffer, buf_len, offset, str, -1);
	offset += strlen(str);
	// "abc xyz def"
	//         ^
	return offset;
}

// "AAAAbbbbCCCC" ==> "AAAACCCC"
char *delete_str(char *buffer, size_t start, len_t delete_len)
{
	strcpy__(&buffer[start], &buffer[start+delete_len]);
	return buffer;
}

// "AAAACCCC", "bbbb" ==> "AAAAbbbbCCCC"
char *insert_str(char *buffer, size_t buf_len, size_t offset,
 const char *string, len_t insert_len)
{
	int org_len;

	org_len = strnlen(buffer, buf_len);
	if (insert_len < 0) {
		insert_len = strnlen(string, buf_len);
	}
	if (insert_len > 0) {
		if (org_len + insert_len <= buf_len) {
			strlcpy__(&buffer[offset + insert_len], &buffer[offset], org_len - offset);
			memmove(&buffer[offset], string, insert_len);
		} else {
			// buffer overflow
		}
	}
	return buffer;
}
char *concat_file_name_separating_by_space(char *buffer, size_t buf_len,
 const char *string)
{
	if (is_strlen_not_0(buffer) && buffer[strlen(buffer)-1] != ' ') {
		// "command" ==> "command "
		strlcat__(buffer, buf_len, " ");
	}
	string = quote_file_name_static(string);
	if (strnlen(buffer, buf_len) + strnlen(string, buf_len) <= buf_len) {
		strlcat__(buffer, buf_len, string);
	}
	return buffer;
}
const char *quote_file_name_static(const char *string)
{
	static char buf[MAX_PATH_LEN+1];

	return quote_file_name_if_necessary(buf, string);
}
const char *quote_file_name_buf(char *buf, const char *string)
{
	return quote_file_name_if_necessary(buf, string);
}
const char *quote_file_name_if_necessary(char *buf, const char *string)
{
	if (contain_chr(string, ' ')
	 || contain_chr(string, '\'') || contain_chr(string, '"')) {
		if (contain_chr(string, '\'')) {
			// abc'def.txt ==> "abc'def.txt"
			return quote_string(buf, string, '"');
		} else {
			// abc"def.txt ==> 'abc"def.txt'
			return quote_string(buf, string, '\'');
		}
	} else {
		strcpy__(buf, string);
	}
	return string;	// no quotation necessary
}

int is_strlen_0(const char *str)
{
	return is_strlen_not_0(str) == '\0';
}
int is_strlen_not_0(const char *str)
{
	return *str;
}

size_t str_path_len(const char *str)
{
	return strnlen(str, MAX_PATH_LEN);
}

char *strcat_printf(char *buffer, size_t buf_len, const char *format, ...)
{
	va_list ap;
	char buf[MAX_PATH_LEN+1];

	va_start(ap, format);
	vsnprintf(buf, MAX_PATH_LEN+1, format, ap);
	va_end(ap);
	strlcat__(buffer, buf_len, buf);
	return buffer;
}
int snprintf_(char *buffer, size_t buf_len, const char *format, ...)
{
	va_list ap;
	int ret;

	va_start(ap, format);
	ret = vsnprintf(buffer, buf_len, format, ap);
	va_end(ap);
	return ret;
}

// dest and src are overlappable
char *strcpy__(char *dest, const char *src)
{
	return strlcpy__(dest, src, strlen(src));
}
char *strlcpy__(char *dest, const char *src, size_t len)
{
	len = LIM_MAX(strlen(src), len);
	memmove(dest, src, len);
	dest[len] = '\0';	// always NUL terminate string
	return dest;
}

// dest and src are overlappable
char *strcat__(char *dest, const char *src)
{
	return memmove(&dest[strlen(dest)], src, strlen(src) + 1);
}
char *strlcat__(char *dest, size_t buf_len, const char *src)
{
	return strlncat__(dest, buf_len, src, strlen(src));
}
char *strlncat__(char *dest, size_t buf_len, const char *src, size_t cat_len)
{
	int len;

	len = strlen(dest);
	strlcpy__(&dest[len], src, LIM_MAX(buf_len - len, cat_len));
	return dest;
}

char *strcut__(char *buffer, size_t buf_len, const char *string, size_t so, size_t eo)
{
	buf_len = LIM_MAX(buf_len, eo - so);
	return strlcpy__(buffer, &string[so], buf_len);
}

int strlcmp__(const char *dest, const char *src)
{
	return strncmp(dest, src, strlen(src));
}

int strlcasecmp(const char *str1, const char *str2)
{
	return strncasecmp(str1, str2, strlen(str2));
}

char *strchr__(const char *str, char chr)
{
	return chr ? strchr(str, chr) : NULL;
}
char *strnset__(char *buf, char chr, size_t len)
{
	memset(buf, chr, len);
	buf[len] = '\0';	// always NUL terminate string
	return buf;
}
// dest and src are overlappable
char *strncpy__(char *dest, const char *src, size_t buf_len)
{
	size_t copy_len;

	copy_len = LIM_MAX(strlen(src), buf_len);
	memmove(dest, src, copy_len);
	memset(&dest[copy_len], '\0', buf_len - copy_len);
	return dest;
}
// dest and src are overlappable
void *memcpy__(void *dest, const void *src, size_t len)
{
	return memmove(dest, src, len);
}

int tolower_if_alpha(int chr)
{
	if (isalpha(chr))
		return tolower(chr);
	return chr;
}

char *strupper(char *buffer)
{
	char *ptr;

	for (ptr = buffer ; *ptr; ptr++) {
		*ptr = toupper(*ptr);
	}
	return buffer;
}

char *strlower(char *buffer)
{
	char *ptr;

	for (ptr = buffer ; *ptr; ptr++) {
		*ptr = tolower(*ptr);
	}
	return buffer;
}

//-----------------------------------------------------------------------------

char *shrink_str(char *str, int space, int n_over_10)
{
	char buf_[MAX_PATH_LEN+1];

	shrink_str_buf(buf_, str, space, n_over_10);
	strlcpy__(str, buf_, MAX_PATH_LEN);		// copy back to original buffer
	return str;
}
char *shrink_str_static(const char *str, int space, int n_over_10)
{
	static char buf[MAX_PATH_LEN+1];

	shrink_str_buf(buf, str, space, n_over_10);
	return buf;
}
// "/very/long/long/path/to/file" ==> "/very/lo...th/to/file"
//                                       n/10         (10-n)/10
char *shrink_str_buf(char *buf, const char *str, int space, int n_over_10)
{
	int str_cols;
	int space1 = 0, space2 = 0;
	int byte_idx1, byte_idx2;

	str_cols = utf8s_columns(str, INT_MAX);
	if (str_cols <= space) {
		// enough space
		strlcpy__(buf, str, MAX_PATH_LEN);
	} else {
#define STR_TILDE		"~~"
#define STR_TILDE_LEN	2	// strlen(STR_TILDE)
		if (space > STR_TILDE_LEN) {
			///space1 = LIM_MIN(0, (space - STR_TILDE_LEN) / 10 * n_over_10);
			space1 = LIM_MIN(0, (space - STR_TILDE_LEN) * n_over_10 / 10);
			space2 = LIM_MIN(0, (space - STR_TILDE_LEN) - space1);
			byte_idx1 = get_byte_idx_from_col_idx(str, space1, -1, NULL);
			byte_idx2 = get_byte_idx_from_col_idx(str, str_cols - space2, +1, NULL);
			strlcpy__(buf, str, byte_idx1);
			strcat__(buf, STR_TILDE);
			strcat__(buf, &str[byte_idx2]);
		} else {
			space2 = LIM_MIN(0, space);
			byte_idx2 = get_byte_idx_from_col_idx(str, str_cols - space2, +1, NULL);
			strlcpy__(buf, &str[byte_idx2], MAX_PATH_LEN);
		}
	}
	return buf;
}

int adjust_utf8s_columns(char *utf8s, int columns)
{
	truncate_tail_utf8s_columns(utf8s, columns);
	return expand_utf8s_columns(utf8s, columns);
}
int truncate_tail_utf8s_columns(char *utf8s, int columns)
{
	int bytes;

	bytes = get_byte_idx_from_col_idx(utf8s, columns, -1, NULL);
	utf8s[bytes] = '\0';
	return bytes;
}
int get_byte_idx_from_col_idx(const char *utf8s, int columns, int left_right,
 int *col_idx__)
{
	int col_idx;
	int prev_col_idx;
	const char *ptr;
	const char *prev_ptr;

	prev_col_idx = columns;
	for (prev_ptr = ptr = utf8s, col_idx = 0; *ptr && col_idx < columns; ) {
		prev_col_idx = col_idx;
		prev_ptr = ptr;
		col_idx += utf8c_columns(ptr);
		ptr += utf8c_bytes(ptr);
	}
	if (left_right < 0 && columns < col_idx) {
		col_idx = prev_col_idx;
		ptr = prev_ptr;
	}
	if (col_idx__)
		*col_idx__ = col_idx;
	return ptr - utf8s;
}

int expand_utf8s_columns(char *utf8s, int columns)
{
	int cols;
	int bytes;

	cols = utf8s_columns(utf8s, INT_MAX);
	if (columns - cols > 0) {
		bytes = strlen(utf8s);
		strnset__(&utf8s[bytes], ' ', columns - cols);
	}
	return LIM_MIN(columns, cols);
}

//-----------------------------------------------------------------------------

int skip_space(const char **ptr)
{
	while (IS_SPACE(*ptr))
		(*ptr)++;
	return IS_EOL(*ptr);
}
const char *skip_chars(const char *ptr, const char *chars)
{
	while (*ptr && strchr__(chars, *ptr)) {
		ptr++;
	}
	return ptr;
}

const char *skip_to_file_path(const char *ptr)
{
	while (*ptr && is_file_path_char(ptr) == 0) {
		// skip to beginning of a file path
		ptr++;
	}
	return ptr;
}
const char *skip_file_path(const char *ptr)
{
	if (*ptr == '\'' || *ptr == '"') {
		// '\'abc/def\'ghi jkl.txt\''
		//  --       --
		char beginning_chr = *ptr;
		ptr++;	// skip beginning_chr
		while (*ptr) {
			if (*ptr == beginning_chr) {
				ptr++;		// skip closing '\'' or '"'
				break;
			}
			if (ptr[0] == '\\') {
				if (ptr[1])
					ptr++;		// skip '\\'
			}
			ptr++;		// skip the next char of '\\'
		}
	} else {
		// filename.txt
		while (is_file_path_char(ptr)) {
			ptr += utf8c_bytes(ptr);
		}
	}
	return ptr;
}
char *skip_file_name(char *ptr)
{
	while (is_file_name_char(ptr)) {
		// skip file name
		ptr += utf8c_bytes(ptr);
	}
	return ptr;
}
/////const char *skip_separator(const char *ptr)
/////{
/////	for ( ; is_separator(*ptr); ) {
/////		ptr++;
/////		// skip to the next token
/////	}
/////	return ptr;
/////}
const char *skip_one_separator(const char *ptr)
{
	if (is_separator(*ptr)) {
		ptr++;	// skip it
	}
	return ptr;
}
const char *skip_to_digit(const char *ptr)
{
	for ( ; *ptr && isdigit(*ptr) == 0; ) {
		ptr++;
		// skip digits
	}
	return ptr;
}
const char *skip_digits(const char *ptr)
{
	for ( ; isdigit(*ptr); ) {
		ptr++;
		// skip digits
	}
	return ptr;
}

char *remove_line_tail_lf(char *line)
{
	int len;

	len = LIM_MIN(0, (int)strlen(line) - 1);
	if (line[len] == '\n')
		line[len] = '\0';
	return line;
}

int is_file_path_char(const char *ptr)
{
	return is_file_name_char(ptr) || strchr__("/", *ptr);
}
int is_file_name_char(const char *ptr)
{
///	return (((' ' < *ptr) && (*ptr < 0x80) && (*ptr != '/')) || (utf8c_bytes(ptr) >= 2));
	return isalnum(*ptr) || strchr__("_-+.~!#$%&@=\"\'", *ptr) || (utf8c_bytes(ptr) >= 2);
	// non-file-name-chars are ' ' '\t' '/' '|' ':'
}
int is_separator(char chr)
{
	return is_white_space_separator(chr) || is_non_white_space_separator(chr);
}
int is_non_white_space_separator(char chr)
{
	return strchr__(" \t,:|()", chr) != NULL;
}
int is_white_space_separator(char chr)
{
	return strchr__(" \t", chr) != NULL;
}
int contain_chr(const char *string, char chr)
{
	return strchr(string, chr) != NULL;
}
char *quote_string(char *buffer, const char *string, char quote_chr)
{
	char buf[MAX_PATH_LEN+1];
	// file name ==> "file name" or 'file name'
	if (contain_chr(string, '\'')) {
		// "'file'name'" ==> "'\'file\'name\''"
		escape_quote_chr(buf, string, quote_chr);
		snprintf(buffer, MAX_PATH_LEN+1, "%c%s%c", quote_chr, buf, quote_chr);
	} else {
		// "filename" ==> "'filename'"
		snprintf(buffer, MAX_PATH_LEN+1, "%c%s%c", quote_chr, string, quote_chr);
	}
	return buffer;
}
// "'file'name'" ==> "\'file\'name\'"
char *escape_quote_chr(char *buffer, const char *string, char quote_chr)
{
	char buf[2+1];	// "\'"
	buffer[0] = '\0';
	for ( ; *string; string++) {
		if (*string == quote_chr) {
			snprintf(buf, 2+1, "%c%c", '\\', quote_chr);
		} else {
			snprintf(buf, 1+1, "%c", *string);
		}
		strlcat__(buffer, MAX_PATH_LEN+1, buf);
	}
	return buffer;
}

char *unquote_string(char *buffer)
{
	if (is_quoted(buffer, '\'') || is_quoted(buffer, '"')) {
		char quote_chr = buffer[0];
		size_t len = str_path_len(buffer);
		char *ptr = buffer;
		for (int idx = 1; idx < len-1; idx++) {
			if (buffer[idx] == '\\' && buffer[idx+1] == quote_chr) {
				*ptr++ = buffer[idx+1];	// "\'" ==> "'", '\"' ==> '"'
				idx++;
			} else {
				*ptr++ = buffer[idx];
			}
		}
		*ptr = '\0';	// NUL terminate
	}
	return buffer;
}
char is_quoted(const char *str, char quote_chr)
{
	if (str_path_len(str) >= 2) {
		if (str[0] == quote_chr && str[str_path_len(str) - 1] == quote_chr) {
			// 'string-quoted' or "string-quoted"
			return quote_chr;
		}
	}
	return '\0';
}

//-----------------------------------------------------------------------------------

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
		strlcpy__(cur_lang, getenv__("LANG"), LANG_STR_LEN);	// "ja_JP.UTF-8"
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

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_str_w_caret(const char *string, int byte_idx)
{
	char buf1[STR_BUF_LEN+1];
	char buf2[STR_BUF_LEN+1];

	if (IS_BETWEEN(0, byte_idx, strlen(string)) == 0) {
		warning_printf("byte_idx: %d out of range[%d, %d]\n",
		 byte_idx, 0, strlen(string));
		byte_idx = MIN_MAX_(0, byte_idx, strlen(string));
	}
	strcut__(buf1, STR_BUF_LEN, string, 0, byte_idx);
	strcut__(buf2, STR_BUF_LEN, string, byte_idx, strlen(string));
	d_printf("[%s^%s]\n", buf1, buf2);
}
void dump_str_w_caret2(const char *string, int byte_idx_1, int byte_idx_2)
{
	char buf1[STR_BUF_LEN+1];
	char buf2[STR_BUF_LEN+1];
	char buf3[STR_BUF_LEN+1];

	if (IS_BETWEEN(0, byte_idx_1, strlen(string)) == 0
	 || IS_BETWEEN(0, byte_idx_2, strlen(string)) == 0) {
		warning_printf("byte_idx_1:%d, byte_idx_2:%d out of range[%d, %d]\n",
		 byte_idx_1, byte_idx_2, 0, strlen(string));
		byte_idx_1 = MIN_MAX_(0, byte_idx_1, strlen(string));
		byte_idx_2 = MIN_MAX_(0, byte_idx_2, strlen(string));
	}
	strcut__(buf1, STR_BUF_LEN, string, 0, byte_idx_1);
	strcut__(buf2, STR_BUF_LEN, string, byte_idx_1, byte_idx_2);
	strcut__(buf3, STR_BUF_LEN, string, byte_idx_2, strlen(string));
	flf_d_printf("[%s{%s}%s]\n", buf1, buf2, buf3);
}
#endif // ENABLE_DEBUG

// End of utilstr.c
