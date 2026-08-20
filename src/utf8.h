/**************************************************************************
 *   utf8.h                                                               *
 *                                                                        *
 *   Copyright (C) 2005-2006 PCR                                          *
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

#ifndef utf8_h
#define utf8_h

#define UTF16_CHARS		65536		// 0x0000 -- 0xffff

// DNU: MB_LEN_MAX is defined to "16" in the latest Glibc header.
#define MAX_UTF8C_BYTES			6

// |ON_DEMAND_WCWIDTH|WA_VAGUE_CHAR_WIDTH|how character width are determined    |
// |-----------------|-------------------|--------------------------------------|
// |    undefined    |    undefined      |widths are gotten from 'wcwidth()'    |
// |    DEFINED      |    ---------      |widths are determined in the run time |
// |    undefined    |    DEFINED        |widths are determined by my self      |

#ifndef ENABLE_NCURSES
///
#define ON_DEMAND_WCWIDTH	// make wcwidth table on demand (when the character be displayed)
#if APP_REL_LVL == APP_REL_LVL_TEST1
#undef ON_DEMAND_WCWIDTH
#endif // APP_REL_LVL == APP_REL_LVL_TEST1
#endif // ENABLE_NCURSES

#ifndef ON_DEMAND_WCWIDTH
///
#define WA_VAGUE_CHAR_WIDTH		// determine character width by my self when it's vague
#endif // ON_DEMAND_WCWIDTH

										// redundant code ranges:
#define UTF8_2_MIN		0x00000080		// 0x00000000 ~ 0x0000007f
#define UTF8_2_MAX		0x00000800
#define UTF8_3_MIN		0x00000800		// 0x00000000 ~ 0x000007ff
#define UTF8_3_MAX		0x00010000
#define UTF8_4_MIN		0x00010000		// 0x00000000 ~ 0x0000ffff
#define UTF8_4_MAX		0x00200000
#define UTF8_5_MIN		0x00200000		// 0x00000000 ~ 0x001fffff
#define UTF8_5_MAX		0x04000000
#define UTF8_6_MIN		0x04000000		// 0x00000000 ~ 0x03ffffff
#define UTF8_6_MAX		0x80000000

#define WC_TO_UTF8_BYTE0_1(wc)		((wc) & 0x7f)
#define WC_TO_UTF8_BYTE0_2(wc)		(0xc0 | (((wc) & 0x7c0) >> 6))
#define WC_TO_UTF8_BYTE1_2(wc)		(0x80 | (((wc) & 0x03f) >> 0))
#define WC_TO_UTF8_BYTE0_3(wc)		(0xe0 | (((wc) & 0xf000) >> 12))
#define WC_TO_UTF8_BYTE1_3(wc)		(0x80 | (((wc) & 0x0fc0) >> 6))
#define WC_TO_UTF8_BYTE2_3(wc)		(0x80 | (((wc) & 0x003f) >> 0))
#define WC_TO_UTF8_BYTE0_4(wc)		(0xf0 | (((wc) & 0x1c0000) >> 18))
#define WC_TO_UTF8_BYTE1_4(wc)		(0x80 | (((wc) & 0x03f000) >> 12))
#define WC_TO_UTF8_BYTE2_4(wc)		(0x80 | (((wc) & 0x000fc0) >> 6))
#define WC_TO_UTF8_BYTE3_4(wc)		(0x80 | (((wc) & 0x00003f) >> 0))
#define WC_TO_UTF8_BYTE0_5(wc)		(0xf8 | (((wc) & 0x03000000) >> 24))
#define WC_TO_UTF8_BYTE1_5(wc)		(0x80 | (((wc) & 0x00fc0000) >> 18))
#define WC_TO_UTF8_BYTE2_5(wc)		(0x80 | (((wc) & 0x0003f000) >> 12))
#define WC_TO_UTF8_BYTE3_5(wc)		(0x80 | (((wc) & 0x00000fc0) >> 6))
#define WC_TO_UTF8_BYTE4_5(wc)		(0x80 | (((wc) & 0x0000003f) >> 0))
#define WC_TO_UTF8_BYTE0_6(wc)		(0xfc | (((wc) & 0x40000000) >> 30))
#define WC_TO_UTF8_BYTE1_6(wc)		(0x80 | (((wc) & 0x3f000000) >> 24))
#define WC_TO_UTF8_BYTE2_6(wc)		(0x80 | (((wc) & 0x00fc0000) >> 18))
#define WC_TO_UTF8_BYTE3_6(wc)		(0x80 | (((wc) & 0x0003f000) >> 12))
#define WC_TO_UTF8_BYTE4_6(wc)		(0x80 | (((wc) & 0x00000fc0) >> 6))
#define WC_TO_UTF8_BYTE5_6(wc)		(0x80 | (((wc) & 0x0000003f) >> 0))

#define UTF8_ZEN_SPACE				"　"	// Zenkaku(full width) space    "\xe3\x80\x80"
#define UTF8_ZEN_SPACE_NOTATION		"＿"	// Zenkaku(full width) low line "\xef\xbc\xbf"

#ifdef START_UP_TEST
void test_utf8c_encode();
#define DUMP_UTF8_BUF_LEN	(3 * MAX_UTF8C_BYTES +1)	// "00-00-00-00-00-00-"
const char *dump_utf8c(const char *utf8c, char *buf);
void test_wcwidth();
void test_utf8c_bytes();
#endif // START_UP_TEST

int utf8s_chars(const char *utf8s);
int utf8s_columns(const char *utf8s, int bytes);
int utf8c_bytes(const char *utf8s);
#define UTF8_BYTES(chr)					\
	(((UCHAR)chr < 0x80) ? 1			\
	 : (IS_UTF8_1ST_BYTE_2(chr) ? 2		\
	  : (IS_UTF8_1ST_BYTE_3(chr) ? 3	\
	   : (IS_UTF8_1ST_BYTE_4(chr) ? 4	\
		: (IS_UTF8_1ST_BYTE_5(chr) ? 5	\
		 : (IS_UTF8_1ST_BYTE_6(chr) ? 6	\
		  : 1))))))
#define UTF8_TRAILING_BYTES(chr)	(UTF8_BYTES(chr) - 1)
#define IS_UTF8_1ST_BYTE(chr)		(UTF8_BYTES(chr) >= 2)
#define IS_UTF8_1ST_BYTE_2(chr)		(((chr) & 0xe0) == 0xc0)
#define IS_UTF8_1ST_BYTE_3(chr)		(((chr) & 0xf0) == 0xe0)
#define IS_UTF8_1ST_BYTE_4(chr)		(((chr) & 0xf8) == 0xf0)
#define IS_UTF8_1ST_BYTE_5(chr)		(((chr) & 0xfc) == 0xf8)
#define IS_UTF8_1ST_BYTE_6(chr)		(((chr) & 0xfe) == 0xfc)
#define IS_UTF8_TRAILING_BYTE(chr)	(((chr) & 0xc0) == 0x80)

int utf8c_remaining_bytes(char utf8c_state, char utf8c);
int utf8c_prev_bytes(const char *utf8s_min, const char *utf8s);
int utf8c_columns(const char *utf8s);
wchar_t utf8c_decode(const char *utf8c);
char *utf8c_encode(wchar_t wc, char *utf8c);
int utf8c_encode_len(wchar_t wc, char *utf8c);
int utf8c_is_redundant_code(const char *utf8c);

#ifdef ON_DEMAND_WCWIDTH
void investigate_all_wcwidth();
ssize_t write_wcwidth_cache();
ssize_t read_wcwidth_cache();
const char *get_wcwidth_file_path();
#endif // ON_DEMAND_WCWIDTH

#ifdef START_UP_TEST
void test_my_utf8();
#endif // START_UP_TEST

int my_wctomb(char *utf8c, wchar_t wc);
int my_mblen(const char *utf8c, int max_len);
int my_mbtowc(const char *utf8c, int max_len);

int my_mbwidth(const char *utf8c, int max_len);
int my_wcwidth(wchar_t wc);

#endif // utf8_h

// End of utf8.h
