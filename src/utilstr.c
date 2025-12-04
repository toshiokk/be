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

// NOTE: If no 'buf_len' parameter in the function input arguments,
//       'MAX_PATH_LEN' would be expected to the buffer size.

#define STR_BUF_LEN		4096
int is_ctrl_char(unsigned char uchar)
{
	return (uchar < 0x20) || (uchar == 0x7f);
}
int is_graph_char(unsigned char uchar)
{
	return is_ctrl_char(uchar) == 0;
}

//------------------------------------------------------------------------------
#ifndef HAVE_STRCASECMP
// This function is equivalent to strcasecmp().
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
// This function is equivalent to strncasecmp().
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

// translate a character to a character
char *str_tr(char *string, char chr_from, char chr_to)
{
	for (char *ptr = string; *ptr; ptr++) {
		if (*ptr == chr_from) {
			*ptr = chr_to;
		}
	}
	return string;
}

//------------------------------------------------------------------------------
char *conv_esc_str(char *string)
{
	char buf[MAX_PATH_LEN+1];
	char *dest = buf;
	for (char *src = string; *src; src++) {
		switch (*src) {
		case '\\':
			switch (*(src+1)) {
			case 'a':
			case 'r':
			case 't':
				switch (*(src+1)) {
				case 'a':		*dest = '\a';		break;
				case 't':		*dest = '\t';		break;
				case 'r':		*dest = '\r';		break;
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

//------------------------------------------------------------------------------
#ifdef START_UP_TEST
void test_utilstr()
{
	MY_UT_INT(contain_chr("(){}", ' '), 0);
	MY_UT_INT(contain_chr("(){}", '('), 1);
	MY_UT_INT(contain_chrs("(){}", "<>"), 0);
	MY_UT_INT(contain_chrs("(){}", "({"), 1);
}
void test_replace_str()
{
	char buffer[100+1];

	strcpy__(buffer, "abcdefghijklmnop");
	MY_UT_STR(replace_str(buffer, 100, 7, 4, "HIJK", 4), "abcdefgHIJKlmnop");

	strcpy__(buffer, "abcdefghijklmnop");
	MY_UT_STR(replace_str(buffer, 100, 7, 4, "", 0), "abcdefglmnop");

	strcpy__(buffer, "abcdefghijklmnop");
	MY_UT_STR(replace_str(buffer, 100, 7, 0, "HIJK", 4), "abcdefgHIJKhijklmnop");
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

int insert_str_separating_by_space(char *buffer, size_t buf_len, size_t offset, const char *str)
{
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

char *insert_str_chr(char *buffer, size_t buf_len, size_t offset, char chr)
{
	char buf[sizeof(char) + 1];
	buf[0] = chr;
	buf[sizeof(char)] = '0';
	return insert_str(buffer, buf_len, offset, buf, sizeof(char));
}

// "AAAACCCC", "bbbb" ==> "AAAAbbbbCCCC"
char *insert_str(char *buffer, size_t buf_len, size_t offset,
 const char *string, len_t insert_len)
{
	int org_len = strnlen(buffer, buf_len);
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

// "aaaaaaaaaa", "bb" ==> "bbaaaaaaaaaa"
// NOTE: 'buffer' must have at least the size ('len' + 1)
char *str_prepend(char *dest, size_t buf_len, const char *str)
{
	size_t len = strnlen(str, buf_len);
	buf_len = LIM_MIN(0, buf_len - len);		// avoid buffer overrun
	strlcpy__(&(dest[len]), dest, buf_len);		// "??aaaaaaaaaa"
	memcpy__(dest, str, len);					// "bbaaaaaaaaaa"
	return dest;
}

char *concat_file_path_separating_by_space(char *buffer, size_t buf_len, const char *string)
{
	if (is_strlen_not_0(buffer) && buffer[strlen(buffer)-1] != ' ') {
		// "command" ==> "command "
		strlcat__(buffer, buf_len, " ");
	}
	string = quote_file_path_s(string);
	if (strnlen(buffer, buf_len) + strnlen(string, buf_len) <= buf_len) {
		strlcat__(buffer, buf_len, string);
	}
	return buffer;
}
char *quote_file_path_s(const char *string)
{
	static char buf_s[MAX_PATH_LEN+1];
	return quote_file_path_if_necessary(buf_s, string);
}
char *quote_file_path_buf(char *buf, const char *string)
{
	return quote_file_path_if_necessary(buf, string);
}
char *quote_file_path_if_necessary(char *buf, const char *string)
{
	if (contain_chrs(string, " \"'$#|&<>()")) {
		// [abc def.txt] ==> ['abc def.txt']
		// [abc"def.txt] ==> ['abc"def.txt']
		// [abc'def.txt] ==> ['abc\'def.txt']
		return quote_string(buf, string, '\'');
	} else {
		strlcpy__(buf, string, MAX_PATH_LEN);	// no quotation necessary
	}
	return buf;
}

#ifdef START_UP_TEST
PRIVATE void test_get_one_file_path__(const char *file_paths,
 const char *path1, const char *path2, const char *path3, const char *path4, const char *path5);
void test_get_one_file_path()
{
	flf_dprintf("-----------------------\n");
	test_get_one_file_path__(" '/path/to/file 1' '/path/to/file 2' '/path/to/file 3' ",
	 "/path/to/file 1", "/path/to/file 2", "/path/to/file 3", "", "");
	test_get_one_file_path__(" \" path/to/file 1 \" ' path/to/file 2 ' \" path/to/file 3 \" ",
	 " path/to/file 1 ", " path/to/file 2 ", " path/to/file 3 ", "", "");
	test_get_one_file_path__(" /path/to/file-1 /path/to/file-2 /path/to/file-3 ",
	 "/path/to/file-1", "/path/to/file-2", "/path/to/file-3", "", "");
	test_get_one_file_path__("'/path/to/file 1' '/path/to/file 2' '/path/to/file 3",
	 "/path/to/file 1", "/path/to/file 2", "/path/to/file 3", "", "");
}
PRIVATE void test_get_one_file_path__(const char *file_paths,
 const char *path1, const char *path2, const char *path3, const char *path4, const char *path5)
{
	char path[MAX_PATH_LEN+1];
	for (const char *ptr = file_paths; ; ) {
		const char *ptr2 = get_one_file_path(ptr, path);
		MY_UT_STR(path, path1);
		if (is_strlen_0(path)) {
			break;
		}
		ptr = ptr2;
		path1 = path2, path2 = path3, path3 = path4, path4 = path5;
	}
}
#endif // START_UP_TEST

// "'/path/to/file1' '/path/to/file2' ..."
// "\"/path/to/file1\" \"/path/to/file2\" ..."
const char *get_one_file_path(const char *str, char *buf)
{
	const char *ptr = str;
	skip_space(&ptr);
	char *dest = buf;
	char quote_chr = ' ';
	if ((*ptr == '\'') || (*ptr == '\"')) {
		quote_chr = *ptr++;
	}
	for ( ; *ptr; ) {
		if (*ptr == quote_chr) {
			ptr++;
			break;
		}
		*dest++ = *ptr++;
	}
	*dest = '\0';
	return ptr;
}

int is_strlen_0(const char *str)
{
	return *str == '\0';
}
int is_strlen_not_0(const char *str)
{
	return *str;
}
size_t strlen_path(const char *str)
{
	return strnlen(str, MAX_PATH_LEN);
}

char *strcat_printf(char *buffer, size_t buf_len, const char *format, ...)
{
	char buf[MAX_PATH_LEN+1];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buf, MAX_PATH_LEN, format, ap);
	va_end(ap);
	strlcat__(buffer, buf_len, buf);
	return buffer;
}
// To avoid "warning: '__builtin___snprintf_chk' output may be truncated ...", wrap snprintf()
int snprintf_(char *buffer, size_t buf_len, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = vsnprintf(buffer, buf_len, format, ap);
	va_end(ap);
	return ret;
}
char *sprintf_s(const char *format, ...)
{
	static char buffer_s[MAX_PATH_LEN+1];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer_s, MAX_PATH_LEN, format, ap);
	va_end(ap);
	return buffer_s;
}
char *sprintf_s1(const char *format, ...)
{
	static char buffer_s[MAX_PATH_LEN+1];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer_s, MAX_PATH_LEN, format, ap);
	va_end(ap);
	return buffer_s;
}
char *sprintf_s2(const char *format, ...)
{
	static char buffer_s[MAX_PATH_LEN+1];
	va_list ap;
	va_start(ap, format);
	vsnprintf(buffer_s, MAX_PATH_LEN, format, ap);
	va_end(ap);
	return buffer_s;
}

// dest and src are overlappable
char *strcpy__(char *dest, const char *src)
{
	return strlcpy__(dest, src, strlen(src));
}
// NOTE: 'dest' must have at least the size ('len' + 1)
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
	size_t len = strlen(dest);
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
int strn_strn_cmp__(const char *dest, size_t dest_len, const char *src, size_t src_len)
{
	if (dest_len != src_len) {
		return dest_len - src_len;
	}
	return strncmp(dest, src, dest_len);
}

int strlcasecmp(const char *str1, const char *str2)
{
	return strncasecmp(str1, str2, strlen(str2));
}

// dest: "/home/user/tools/be/file_name.ext"
// src :            "tools/be/file_name.ext"
// src :                     "file_name.ext"
// src :                              ".ext"
int strcmp_from_tail(const char *dest, const char *src)
{
	ssize_t diff = strlen_path(dest) - strlen_path(src);
	if (diff < 0) {
		return -1;
	}
	return strcmp(&dest[diff], src);
}

// strchr("abc", chr);
//
// | chr  | strchr() | strchr__() |
// |------|----------|------------|
// | 'a'  | non-NULL | non-NULL   |
// | 'z'  | NULL     | NULL       |
// | '\0' | non-null | NULL       | <-- problem of strchr()

const char *strchr__(const char *str, char chr)
{
	return chr ? strchr(str, chr) : NULL;
}
int contain_chr(const char *str, char chr)
{
	return strchr__(str, chr) != NULL;
}
int contain_chrs(const char *str, const char *chrs)
{
	while (*chrs) {
		if (strchr__(str, *chrs++))
			return 1;
	}
	return 0;
}

char *strnset__(char *buf, char chr, size_t len)
{
	static char strnset_buf[MAX_PATH_LEN+1];
	if (buf == NULL) {
		buf = strnset_buf;
		len = LIM_MAX(MAX_PATH_LEN, len);
	}
	memset(buf, chr, len);
	buf[len] = '\0';	// always NUL terminate string
	return buf;
}
// dest and src are overlappable
char *strncpy__(char *dest, const char *src, size_t buf_len)
{
	size_t copy_len = LIM_MAX(strlen(src), buf_len);
	memmove(dest, src, copy_len);
	memset(&dest[copy_len], '\0', buf_len - copy_len);
	return dest;
}
// dest and src are overlappable
void *memcpy__(void *dest, const void *src, size_t len)
{
	return memmove(dest, src, len);
}

int tolower__(int chr)
{
	if (isalpha(chr))
		return tolower(chr);
	return chr;
}

char *strupper(char *buffer)
{
	for (char *ptr = buffer ; *ptr; ptr++) {
		*ptr = toupper(*ptr);
	}
	return buffer;
}

char *strlower(char *buffer)
{
	for (char *ptr = buffer ; *ptr; ptr++) {
		*ptr = tolower(*ptr);
	}
	return buffer;
}

//------------------------------------------------------------------------------
char *shrink_str__adjust_col(char *str, int columns, int n_over_10)
{
	shrink_str(str, columns, n_over_10);
	return expand_str_columns(str, columns);
}
char *shrink_str(char *str, int columns, int n_over_10)
{
	char buf[MAX_PATH_LEN+1];
	shrink_str_buf(buf, str, columns, n_over_10);
	strlcpy__(str, buf, MAX_PATH_LEN);		// copy back to original buffer
	return str;
}
char *shrink_str_static(const char *str, int columns, int n_over_10)
{
	static char buf[MAX_PATH_LEN+1];
	return shrink_str_buf(buf, str, columns, n_over_10);
}
// "/very/long/long/path/to/file" ==> "/very/lo...th/to/file"
//                                       n/10         (10-n)/10
// Note: 'buf' and 'str' are not overlappable
#define STR_TILDE		"~~"
#define STR_TILDE_LEN	2	// strlen(STR_TILDE)
char *shrink_str_buf(char *buf, const char *str, int columns, int n_over_10)
{
	int str_cols = utf8s_columns(str, MAX_PATH_LEN);
	if (str_cols <= columns) {
		// enough space
		strlcpy__(buf, str, MAX_PATH_LEN);
	} else {
		if (columns > STR_TILDE_LEN) {
			int space1 = LIM_MIN(0, (columns - STR_TILDE_LEN) * n_over_10 / 10);
			int space2 = LIM_MIN(0, (columns - STR_TILDE_LEN) - space1);
			int byte_idx1 = get_byte_idx_from_col_idx(str, space1, -1, NULL);
			int byte_idx2 = get_byte_idx_from_col_idx(str, str_cols - space2, +1, NULL);
			strlcpy__(buf, str, byte_idx1);
			strcat__(buf, STR_TILDE);
			strcat__(buf, &str[byte_idx2]);
		} else {
			int space2 = LIM_MIN(0, columns);
			int byte_idx2 = get_byte_idx_from_col_idx(str, str_cols - space2, +1, NULL);
			strlcpy__(buf, &str[byte_idx2], MAX_PATH_LEN);
		}
	}
	return buf;
}

char *adjust_str_columns(char *utf8s, int columns)
{
	truncate_str_tail_columns(utf8s, columns);
	return expand_str_columns(utf8s, columns);
}
int truncate_str_tail_columns(char *utf8s, int columns)
{
	int bytes = get_byte_idx_from_col_idx(utf8s, columns, -1, NULL);
	utf8s[bytes] = '\0';
	return bytes;
}
char *expand_str_columns(char *utf8s, int columns)
{
	int cols = utf8s_columns(utf8s, MAX_PATH_LEN);
	if (columns > cols) {
		int bytes = strlen(utf8s);
		strnset__(&utf8s[bytes], ' ', columns - cols);
	}
	return utf8s;
}

int get_byte_idx_from_col_idx(const char *utf8s, int columns, int left_right, int *col_idx__)
{
	int col_idx = 0;
	const char *ptr;
	const char *prev_ptr;

	int prev_col_idx = columns;
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

char *utf8s_strnset__(char *buf, const char *utf8c, size_t len)
{
	strcpy__(buf, "");
	for ( ; strnlen(buf, len) + strlen(utf8c) > len; ) {
		strlcat__(buf, len, utf8c);
	}
	return buf;
}

//------------------------------------------------------------------------------
int skip_space(const char **ptr)
{
	while (IS_WHITE_SPACE(*ptr))
		(*ptr)++;
	return IS_EOL(*ptr);
}
int skip_space_mutable(char **ptr)
{
	while (IS_WHITE_SPACE(*ptr))
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
	while (*ptr && is_char_file_path_min(ptr) == 0) {
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
		while (is_char_file_path_min(ptr)) {
			ptr += utf8c_bytes(ptr);
		}
	}
	return ptr;
}
char *skip_file_name_min(char *ptr)
{
	while (is_char_file_name_min(ptr)) {
		// skip file name
		ptr += utf8c_bytes(ptr);
	}
	return ptr;
}
char *skip_file_name_max(char *ptr)
{
	while (is_char_file_name_max(ptr)) {
		// skip file name
		ptr += utf8c_bytes(ptr);
	}
	return ptr;
}
const char *skip_one_separator_const(const char *ptr)
{
	if (is_char_separator(*ptr)) {
		ptr++;	// skip it
	}
	return ptr;
}
const char *skip_two_white_spaces(const char *ptr)
{
	return skip_white_space(skip_white_space(ptr));
}
const char *skip_white_space(const char *ptr)
{
	if (IS_WHITE_SPACE(ptr))
		ptr++;
	return ptr;
}
const char *skip_to_digit(const char *ptr)
{
	for ( ; *ptr && isdigit(*ptr) == 0; ) {
		ptr++;		// skip digits
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
const char *skip_string_const(const char *ptr)
{
	return ptr + strlen(ptr);
}
char *skip_string_mutable(char *ptr)
{
	return ptr + strlen(ptr);
}
char *skip_slash(char *ptr)
{
	if (*ptr == '/')
		ptr++;
	return ptr;
}

char *remove_line_tail_space(char *line)
{
	for (int off = strlen(line)-1; off >= 0; off--) {
		if (line[off] != ' ') {
			line[off+1] = '\0';
			break;
		}
	}
	return line;
}
char *remove_line_tail_lf(char *line)
{
	int len = LIM_MIN(0, (int)strlen(line) - 1);
	if (line[len] == '\n')
		line[len] = '\0';
	return line;
}

// Less flexible spec for file name,
// some characters are reserved for separator and not usable for file name
int is_char_file_path_min(const char *ptr)
{
	return is_char_file_name_min(ptr) || (*ptr == '/');
}
int is_char_file_name_min(const char *ptr)
{
	// non-file-name-chars are 0x01~0x1f," ()*,/:;<=>?[\]^`{|}~"
	return isalnum(*ptr) || strchr__("!\"#$%&\'+-.=@_~", *ptr) || (utf8c_bytes(ptr) >= 2);
}
// The most flexible spec for file name, only '/' is not usable for file name
int is_char_file_path_max(const char *ptr)
{
	return is_char_file_name_max(ptr) || (*ptr == '/');
}
int is_char_file_name_max(const char *ptr)
{
	// non-file-name-char is only '/'
	return (('\0' < *ptr && *ptr < 0x80) && (*ptr != '/')) || (utf8c_bytes(ptr) >= 2);
}
int is_char_id(char chr)
{
	return isalnum(chr) || (strchr__("_", chr) != NULL);
}
int is_char_separator(char chr)
{
	return is_char_white_space(chr) || is_char_non_white_space_separator(chr);
}
int is_char_non_white_space_separator(char chr)
{
	return strchr__(" ,:|()", chr) != NULL;
}
int is_char_white_space(char chr)
{
	return strchr__(" \t", chr) != NULL;
}

// 'buffer' and 'string' can be the same address
char *quote_string(char *buffer, const char *string, char quote_chr)
{
	char buf1[MAX_PATH_LEN+1];
	char buf2[MAX_PATH_LEN+1];
	// file name ==> "file name" or 'file name'
	if (contain_chr(string, '\'')) {
		// quote_chr = '\'':  ['file'name'] ==> ['\'file\'name\'']
		// quote_chr = '"' :  ['file'name'] ==> ["\'file\'name\'"]
		escape_quote_chr(buf1, string, quote_chr);
		string = buf1;
	}
	// [filename] ==> ['filename']
	snprintf(buf2, MAX_PATH_LEN+1, "%c%s%c", quote_chr, string, quote_chr);
	strlcpy__(buffer, buf2, MAX_PATH_LEN);
	return buffer;
}
// ['file'name'] ==> [\'file\'name\']
char *escape_quote_chr(char *buffer, const char *string, char quote_chr)
{
	buffer[0] = '\0';
	for ( ; *string; string++) {
		if (*string == quote_chr) {
			strcat_printf(buffer, MAX_PATH_LEN, "%c%c", '\\', quote_chr);
		} else {
			strcat_printf(buffer, MAX_PATH_LEN, "%c", *string);
		}
	}
	return buffer;
}

// ['file\'name'] ==> [file'name]
// ["file\"name"] ==> [file"name]
char *unquote_string(char *buffer)
{
	if (is_quoted(buffer, '\'') || is_quoted(buffer, '"')) {
		char quote_chr = buffer[0];
		size_t len = strlen_path(buffer);
		char *ptr = buffer;
		for (int idx = 1; idx < len-1; idx++) {		// ["quoted-string"] ==> [quoted-string]
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
// ['string-quoted'] or ["string-quoted"]
char is_quoted(const char *str, char quote_chr)
{
	if (strlen_path(str) >= 2) {
		if (str[0] == quote_chr && str[strlen_path(str) - 1] == quote_chr) {
			// 'string-quoted' or "string-quoted"
			return quote_chr;
		}
	}
	return '\0';
}

char tail_char(const char *str)
{
	int len = strlen(str);
	if (len > 0) {
		len--;
	}
	return str[len];
}

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG
void test_quote_string()
{
	flf_dprintf("-----------------------\n");
	char buffer[MAX_PATH_LEN+1];
	MY_UT_STR(quote_file_path_buf(buffer, "abc\"def"), "'abc\"def'");
	MY_UT_STR(unquote_string(buffer), "abc\"def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc'def"), "'abc\\'def'");
	MY_UT_STR(unquote_string(buffer), "abc'def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc#def"), "'abc#def'");
	MY_UT_STR(unquote_string(buffer), "abc#def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc$def"), "'abc$def'");
	MY_UT_STR(unquote_string(buffer), "abc$def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc<def"), "'abc<def'");
	MY_UT_STR(unquote_string(buffer), "abc<def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc>def"), "'abc>def'");
	MY_UT_STR(unquote_string(buffer), "abc>def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc(def"), "'abc(def'");
	MY_UT_STR(unquote_string(buffer), "abc(def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc)def"), "'abc)def'");
	MY_UT_STR(unquote_string(buffer), "abc)def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc|def"), "'abc|def'");
	MY_UT_STR(unquote_string(buffer), "abc|def");
	MY_UT_STR(quote_file_path_buf(buffer, "abc&def"), "'abc&def'");
	MY_UT_STR(unquote_string(buffer), "abc&def");
	// no need of quoting
	MY_UT_STR(quote_file_path_buf(buffer, "abc@def"), "'abc@def'");
	MY_UT_STR(unquote_string(buffer), "abc@def");
}
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
	flf_dprintf("[%s{%s}%s]\n", buf1, buf2, buf3);
}

char *dump_str(const char *str, char *buf)
{
	strcpy__(buf, "");
	for (const char *ptr = str; *ptr; ptr++) {
		if ((*ptr < ' ') || (*ptr == 0x7f)) {
			strcat_printf(buf, STR_BUF_LEN, "{%02x}", *ptr);
		} else {
			strcat_printf(buf, STR_BUF_LEN, "%c", *ptr);
		}
	}
	return buf;
}
#endif // ENABLE_DEBUG

// End of utilstr.c
