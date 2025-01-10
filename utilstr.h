/**************************************************************************
 *   utilstr.h                                                            *
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

#ifndef utilstr_h
#define utilstr_h

int is_ctrl_char(unsigned char uchar);
int is_graph_char(unsigned char uchar);

#ifndef HAVE_STRCASECMP
#define strcasecmp my_stricmp
int my_stricmp(const char *s1, const char *s2);
#endif // HAVE_STRCASECMP
#ifndef HAVE_STRNCASECMP
#define strncasecmp my_strnicmp
int my_strnicmp(const char *s1, const char *s2, size_t n);
#endif // HAVE_STRNCASECMP

char* str_tr(char* string, char chr_from, char chr_to);

char *conv_esc_str(char *string);

typedef int len_t;	// len_t can be -1 indicating that length is NOT_SPECIFIED

void test_utilstr(void);
void test_replace_str(void);
char *replace_str(char *buffer, size_t buf_len, size_t start, int delete_len,
 const char *str, len_t insert_len);
int insert_str_separating_by_space(char *buffer, size_t buf_len,
 size_t offset, const char *str);
char *delete_str(char *buffer, size_t start, len_t delete_len);
char *insert_str_chr(char *buffer, size_t buf_len, size_t offset, char chr);
char *insert_str(char *buffer, size_t buf_len, size_t offset,
 const char *string, len_t insert_len);
char *concat_file_path_separating_by_space(char *buffer, size_t buf_len,
 const char *string);
const char *quote_file_path_static(const char *string);
const char *quote_file_path_buf(char *buf, const char *string);
const char *quote_file_path_if_necessary(char *buf, const char *string);

#ifdef START_UP_TEST
void test_get_one_file_path();
#endif // START_UP_TEST
const char *get_one_file_path(const char *ptr, char *buf);

int is_strlen_0(const char *str);
int is_strlen_not_0(const char *str);
size_t strlen_path(const char *str);

char *strcat_printf(char *buffer, size_t buf_len, const char *format, ...);
int snprintf_(char *buffer, size_t buf_len, const char *format, ...);
char *strcpy__(char *dest, const char *src);
char *strlcpy__(char *dest, const char *src, size_t len);
char *strcat__(char *dest, const char *src);
char *strlcat__(char *dest, size_t buf_len, const char *src);
char *strlncat__(char *dest, size_t buf_len, const char *src, size_t cat_len);
char *strcut__(char *buffer, size_t buf_len, const char *string, size_t so, size_t eo);
int strlcmp__(const char *dest, const char *src);
int strlcasecmp(const char *str1, const char *str2);
int strcmp_from_tail(const char *dest, const char *src);
char *strchr__(const char *str, char chr);
int contain_chr(const char *str, char chr);
int contain_chrs(const char *str, const char* chrs);
char *strnset__(char *buf, char chr, size_t len);
char *strncpy__(char *dest, const char *src, size_t buf_len);
void *memcpy__(void *dest, const void *src, size_t len);

int tolower_if_alpha(int chr);
char *strupper(char *buffer);
char *strlower(char *buffer);

char *shrink_str__adjust_col(char *str, int space, int n_over_10);
char *shrink_str(char *str, int space, int n_over_10);
char *shrink_str_static(const char *str, int space, int n_over_10);
char *shrink_str_buf(char *buf, const char *str, int space, int n_over_10);

int adjust_utf8s_columns(char *utf8s, int columns);
int truncate_tail_utf8s_columns(char *utf8s, int columns);
int get_byte_idx_from_col_idx(const char *utf8s, int columns, int left_right,
 int *col_idx__);
int expand_utf8s_columns(char *utf8s, int columns);

char *utf8s_strnset__(char *buf, const char *utf8c, size_t len);

//------------------------------------------------------------------------------
int skip_space(const char **ptr);
const char *skip_chars(const char *ptr, const char *chars);
const char *skip_to_file_path(const char *ptr);
const char *skip_file_path(const char *ptr);
char *skip_file_name(char *ptr);
#if 0
const char *skip_separator(const char *ptr);
#endif
const char *skip_one_separator(const char *ptr);
const char *skip_two_spaces(const char *ptr);
const char *skip_to_digit(const char *ptr);
const char *skip_digits(const char *ptr);
const char *skip_string_const(const char *ptr);
char *skip_string_mutable(char *ptr);

char *remove_line_tail_lf(char *line);

int is_char_file_path(const char *ptr);
int is_char_file_name(const char *ptr);
int is_char_id(char chr);
int is_char_separator(char chr);
int is_char_non_white_space_separator(char chr);
int is_char_white_space(char chr);

char *quote_string(char *buf, const char *string, char quote_chr);
char *escape_quote_chr(char *buffer, const char *string, char quote_chr);
char *unquote_string(char *buffer);
char is_quoted(const char *str, char quote_chr);
char tail_char(const char *str);

//------------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_str_w_caret(const char *string, int byte_idx);
void dump_str_w_caret2(const char *string, int byte_idx_1, int byte_idx_2);
char *dump_str(const char *str, char *buf);
#endif // ENABLE_DEBUG

#endif // utilstr_h

// End of utilstr.h
