/**************************************************************************
 *   utf8.c                                                               *
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

#include "utilincs.h"
#include "tio.h"

#ifdef ON_DEMAND_WCWIDTH
#warning "**** ON_DEMAND_WCWIDTH defined ****"
PRIVATE void init_wcwidth_cache();
PRIVATE void clear_wcwidth_cache();
PRIVATE char get_wcwidth_on_demand(wchar_t wc);
#endif // ON_DEMAND_WCWIDTH

#ifdef WA_VAGUE_CHAR_WIDTH
PRIVATE int is_apparent_wide_chr(wchar_t wc);
PRIVATE int is_vague_narrow_chr(wchar_t wc);
PRIVATE int is_vague_wide_chr(wchar_t wc);
#endif // WA_VAGUE_CHAR_WIDTH

PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len);

#ifdef START_UP_TEST
void test_utf8c_encode()
{
	char utf8c[MAX_UTF8C_BYTES+1];

	for (wchar_t wc = 0x0000; wc <= 0x1fffff; wc++) {	// 21 bits(UCS21)
		utf8c_encode(wc, utf8c);
		flf_dprintf("%06x ==> [%s] ==> %06x\n", wc, dump_utf8c(utf8c, NULL), utf8c_decode(utf8c));
		if (wc >= 0x10000) {
			wc += (0x1000 - 1);
		} else if (wc >= 0x100) {
			wc += (0x100 - 1);
		}
	}
}
const char *dump_utf8c(const char *utf8c, char *buf)
{
	static char utf8c_buf[DUMP_UTF8_BUF_LEN+1];
	if (buf == NULL) {
		buf = utf8c_buf;
	}
	strcpy__(buf, "");
	for (int idx = 0; idx < utf8c_bytes(utf8c); idx++) {
		strcat_printf(buf, DUMP_UTF8_BUF_LEN+1, "%02x-", (UCHAR)utf8c[idx]);
	}
	return buf;
}

void test_wcwidth()
{
	for (wchar_t wc = 0x0000; wc < UTF16_CHARS; wc++) {
		int columns = my_wcwidth(wc);
		if ((wc & 0x1f) == 0) {
			flf_dprintf("%04x", wc);
		}
		d_printf(" %d", columns);
		if ((wc & 0x1f) == 0x1f) {
			d_printf("\n");
		}
	}
	d_printf("\n");
}
void test_utf8c_bytes()
{
	const char str[] = "\xed\xa0\x80\xed\xa0\x81 \xed\xa0\xa0\xed\xa0\xa1";
	const char *ptr = str;
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 3);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 6);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 7);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 10);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 13);
}
#endif // START_UP_TEST

// UTF-8 support functions ----------------------------------------------------

int utf8s_chars(const char *utf8s)
{
	int chars;

	for (chars = 0; *utf8s; chars++) {
		utf8s += utf8c_bytes(utf8s);
	}
	return chars;
}

int utf8s_columns(const char *utf8s, int bytes)
{
	const char *ptr;
	int columns;

	for (ptr = utf8s, columns = 0; *ptr && ptr - utf8s < bytes; ) {
		columns += utf8c_columns(ptr);
		ptr += utf8c_bytes(ptr);
	}
	return columns;
}

int utf8c_bytes(const char *utf8s)
{
	return my_mblen(utf8s, MAX_UTF8C_BYTES);
}

// UTF8 byte sequences:
// (utf8c < 0x80)			// 0xxxxxxx
// (utf8c & 0xe0) == 0xc0	// 110xxxxx,10xxxxxx
// (utf8c & 0xf0) == 0xe0	// 1110xxxx,10xxxxxx,10xxxxxx
// (utf8c & 0xf8) == 0xf0	// 11110xxx,10xxxxxx,10xxxxxx,10xxxxxx
// (utf8c & 0xfc) == 0xf8	// 111110xx,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx
// (utf8c & 0xfe) == 0xfc	// 1111110x,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx,10xxxxxx
// (utf8c & 0xfe) == 0xfe	// 1111111x

int utf8c_remaining_bytes(char utf8c_state, char utf8c)
{
	if (utf8c_state <= 0) {
		utf8c_state = UTF8_TRAILING_BYTES(utf8c);
	} else {
		if ((UCHAR)utf8c < 0x80) {	// 0xxxxxxx (illegal sequence)
			utf8c_state = -1;
		} else
		if (IS_UTF8_TRAILING_BYTE(utf8c)) {		// 10xxxxxx
			utf8c_state--;
		} else {							// 11xxxxxx (illegal sequence)
			utf8c_state = -1;
		}
	}
	return utf8c_state;				// return remaining bytes
}

int utf8c_prev_bytes(const char *utf8s_min, const char *utf8s)
{
	int bytes;
	const char *ptr;

	for (ptr = utf8s; utf8s_min < ptr && utf8s - ptr < MAX_UTF8C_BYTES; ) {
		ptr--;
		if (*(UCHAR *)ptr < 0x80 || 0xc0 <= *(UCHAR *)ptr)
			// ascii(0xxxxxxx) or first byte of UTF8(11xxxxxx)
			break;
	}
	bytes = utf8s - ptr;
	return bytes;
}

int utf8c_columns(const char *utf8c)
{
	return my_mbwidth(utf8c, MAX_UTF8C_BYTES);
}

wchar_t utf8c_decode(const char *utf8c)
{
	wchar_t wc = my_mbtowc(utf8c, MAX_UTF8C_BYTES);
	return wc;
}

char *utf8c_encode(wchar_t wc, char *utf8c)
{
	static char utf8c_s[MAX_UTF8C_BYTES+1];
	if (utf8c == NULL) {
		utf8c = utf8c_s;
	}
	utf8c_encode_len(wc, utf8c);
	return utf8c;
}
int utf8c_encode_len(wchar_t wc, char *utf8c)
{
	int bytes = wctomb(utf8c, wc);
	utf8c[bytes] = '\0';
	return bytes;
}

int utf8c_is_redundant_code(const char *utf8c)
{
	int len = UTF8_BYTES(*utf8c);
	wchar_t min_wc = 0x00;
	switch (len) {
	default:	min_wc = 0x00;			break;
	case 2:		min_wc = UTF8_2_MIN;	break;
	case 3:		min_wc = UTF8_3_MIN;	break;
	case 4:		min_wc = UTF8_4_MIN;	break;
	case 5:		min_wc = UTF8_5_MIN;	break;
	case 6:		min_wc = UTF8_6_MAX;	break;
	}
	wchar_t wc = utf8c_decode(utf8c);
	return wc < min_wc;
}

int my_mbwidth(const char *utf8c, int max_len)
{
	return my_wcwidth(my_mbtowc(utf8c, max_len));
}
int my_wcwidth(wchar_t wc)
{
#ifdef ON_DEMAND_WCWIDTH
	int columns = get_wcwidth_on_demand(wc);
	columns = MIN_MAX_(1, columns, 2);	// 1 / 2
	return columns;
#else // ON_DEMAND_WCWIDTH
#ifdef WA_VAGUE_CHAR_WIDTH
	if (is_apparent_wide_chr(wc)) {
		return 2;
	}
	if (is_vague_narrow_chr(wc)) {
		return 1;
	}
	if (is_vague_wide_chr(wc)) {
		return 2;
	}
#endif // WA_VAGUE_CHAR_WIDTH
	int columns = wcwidth(wc);
	columns = MIN_MAX_(1, columns, 2);	// 1 / 2
	return columns;
#endif // ON_DEMAND_WCWIDTH
}

#ifdef WA_VAGUE_CHAR_WIDTH
PRIVATE int is_apparent_wide_chr(wchar_t wc)
{
	return 0
	// WIDE_CHAR_LIST
	 || (0x3000 <= wc && wc < 0xa000)
	;
}
PRIVATE int is_vague_narrow_chr(wchar_t wc)
{
	return 0
	 || ((0x0080 <= wc) && (wc < 0x0100))	// Upper area of ASCII8
	 || ((0x2400 <= wc) && (wc < 0x2422))	// "␀" ~ "␡"
	 || ((0x2800 <= wc) && (wc < 0x2900))	// "⢀", "⢁" ...
	 || ((0xd800 <= wc) && (wc < 0xe000))	// Surrogate pairs are displayed in narrow
	;
}
PRIVATE int is_vague_wide_chr(wchar_t wc)
{
	return 0
	 || ((0x02c0 <= wc) && (wc < 0x03a0))
	 || ((0x03a0 <= wc) && (wc < 0x1100))
	 || ((0x1100 <= wc) && (wc < 0x1e00))
	 || ((0x2000 <= wc) && (wc < 0x2800))
	 || ((0x2100 <= wc) && (wc < 0x2400))
	 || ((0x2900 <= wc) && (wc < 0x2e80))
	 || ((0x2fc0 <= wc) && (wc < 0x3400))
	 || ((0xa000 <= wc) && (wc < 0xac00))
	 || ((0xd7a0 <= wc) && (wc < 0xd800))
	 || ((0xe000 <= wc) && (wc < 0xf900))
	;
}
#endif // WA_VAGUE_CHAR_WIDTH

#ifdef ON_DEMAND_WCWIDTH
// -1: not initialized
//  0: not investigated yet
//  1: narrow character
//  2: wide character
PRIVATE char wcwidth_cache[UTF16_CHARS] = { -1 };
PRIVATE void init_wcwidth_cache()
{
	if (wcwidth_cache[0] <= -1) {
		clear_wcwidth_cache();
		read_wcwidth_cache();
	}
}
PRIVATE void clear_wcwidth_cache()
{
	memset(wcwidth_cache, 0, sizeof(wcwidth_cache));
}
PRIVATE char get_wcwidth_on_demand(wchar_t wc)
{
	init_wcwidth_cache();
	wc = MIN_MAX_(0, wc, 65535);
	if (wcwidth_cache[wc] <= 0) {
		char width = investigate_wcwidth(wc);
		wcwidth_cache[wc] = width;
	}
	return wcwidth_cache[wc];	// 1, 2
}
void investigate_all_wcwidth()
{
	clear_wcwidth_cache();
	for (wchar_t wc = 0x0000; wc < UTF16_CHARS; wc++) {
		get_wcwidth_on_demand(wc);
	}
	write_wcwidth_cache();
}

ssize_t write_wcwidth_cache()
{
_FLF_
	return write_bin_data_to_file(get_wcwidth_file_path(), wcwidth_cache, UTF16_CHARS);
}
ssize_t read_wcwidth_cache()
{
_FLF_
	return read_bin_data_from_file(get_wcwidth_file_path(), wcwidth_cache, UTF16_CHARS);
}
const char *get_wcwidth_file_path()
{
	static char file_path[MAX_PATH_LEN+1] = "";
	if (is_strlen_0(file_path)) {
		concat_dir_and_file(file_path, get_app_dir(), "wcwidth.bin");
flf_dprintf("[%s]\n", file_path);
	}
	return file_path;
}
#endif // ON_DEMAND_WCWIDTH

// wctomb() in standard C library can not convert surrogate pair word from 0xd800 ~ 0xdfff.
// So I provide my_wctomb() which can do it.
int my_wctomb(char *utf8c, wchar_t wc)
{
	if (wc < UTF8_2_MIN) {				// 0xxxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_1(wc);
		utf8c[1] = '\0';
		return 1;
	} else
	if (wc < UTF8_2_MAX) {				// 00000yyy yyxxxxxx
		// 110yyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_2(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_2(wc);
		utf8c[2] = '\0';
		return 2;
	} else
	if (wc < UTF8_3_MAX) {				// zzzzyyyy yyxxxxxx
		// 1110zzzz 10yyyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_3(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_3(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_3(wc);
		utf8c[3] = '\0';
		return 3;
	} else
	if (wc < UTF8_4_MAX) {				// 00000000 000wwwzz zzzzyyyy yyxxxxxx
		// 11110www 10zzzzzz 10yyyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_4(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_4(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_4(wc);
		utf8c[3] = WC_TO_UTF8_BYTE3_4(wc);
		utf8c[4] = '\0';
		return 4;
	} else
	if (wc < UTF8_5_MAX) {				// 000000vv wwwwwwzz zzzzyyyy yyxxxxxx
		// 111110vv 10wwwwww 10yyyyyy 10xxxxxx 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_5(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_5(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_5(wc);
		utf8c[3] = WC_TO_UTF8_BYTE3_5(wc);
		utf8c[4] = WC_TO_UTF8_BYTE4_5(wc);
		utf8c[5] = '\0';
		return 5;
	} else /* if (wc < UTF8_6_MAX) */ {	// 0uvvvvvv wwwwwwzz zzzzyyyy yyxxxxxx
		// 1111110u 10vvvvvv 10wwwwww 10zzzzzz 10yyyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_6(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_6(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_6(wc);
		utf8c[3] = WC_TO_UTF8_BYTE3_6(wc);
		utf8c[4] = WC_TO_UTF8_BYTE4_6(wc);
		utf8c[5] = WC_TO_UTF8_BYTE5_6(wc);
		utf8c[6] = '\0';
		return 6;
	}
}

// UTF8 character byte length
int my_mblen(const char *utf8c, int max_len)
{
	wchar_t wc;
	return my_mbtowc__(&wc, utf8c, max_len);
}
int my_mbtowc(const char *utf8c, int max_len)
{
	wchar_t wc;
	my_mbtowc__(&wc, utf8c, max_len);
	return wc;
}
PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len)
{
	wchar_t wc = 0;
	int len = 1;

	if (max_len < 1) {
		*pwc = wc;
		return 0;
	}
	wc = utf8c[0];
	if ((utf8c[0] & 0x80) == 0x00) {	// 0xxxxxxx
		wc = utf8c[0] & 0x7f;			// 0xxxxxxx
		len = 1;
	} else
	if ((utf8c[0] & 0xe0) == 0xc0) {	// 110xxxxx-10xxxxxx
		wc = utf8c[0] & 0x1f;			// 110xxxxx
		len = 2;
	} else
	if ((utf8c[0] & 0xf0) == 0xe0) {	// 1110xxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x0f;			// 1110xxxx
		len = 3;
	} else
	if ((utf8c[0] & 0xf8) == 0xf0) {	// 11110xxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x07;			// 11110xxx
		len = 4;
	} else
	if ((utf8c[0] & 0xfc) == 0xf8) {	// 111110xx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x03;			// 111110xx
		len = 5;
	} else
	if ((utf8c[0] & 0xfe) == 0xfc) {	// 1111110x-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x01;			// 1111110x
		len = 6;
	}
	int idx;
	for (idx = 1; idx < max_len; idx++) {
		if (idx >= len)
			break;
		if (IS_UTF8_TRAILING_BYTE(utf8c[idx]) == 0)
			break;		// UTF8 sequence trancated !!
		wc = (wc << 6) | (utf8c[idx] & 0x3f);
	}
	len = idx;
	*pwc = wc;
	return len;		// return length
}

#ifdef START_UP_TEST
PRIVATE char *test_my_wctomb(wchar_t wc);
void test_my_utf8()
{
_FLF_
	MY_UT_STR(test_my_wctomb(0x000000a0), "\xc2\xa0");
	MY_UT_STR(test_my_wctomb(0x000007ff), "\xdf\xbf");
	MY_UT_STR(test_my_wctomb(0x00000800), "\xe0\xa0\x80");
	MY_UT_STR(test_my_wctomb(0x0000ffff), "\xef\xbf\xbf");
	MY_UT_STR(test_my_wctomb(0x00010000), "\xf0\x90\x80\x80");
	MY_UT_STR(test_my_wctomb(0x001fffff), "\xf7\xbf\xbf\xbf");
	MY_UT_STR(test_my_wctomb(0x00200000), "\xf8\x88\x80\x80\x80");
	MY_UT_STR(test_my_wctomb(0x03ffffff), "\xfb\xbf\xbf\xbf\xbf");
	MY_UT_STR(test_my_wctomb(0x04000000), "\xfc\x84\x80\x80\x80\x80");
	MY_UT_STR(test_my_wctomb(0x7fffffff), "\xfd\xbf\xbf\xbf\xbf\xbf");
}
PRIVATE char *test_my_wctomb(wchar_t wc)
{
	static char utf8_buf[MAX_UTF8C_BYTES + 1];
	my_wctomb(utf8_buf, wc);
	return utf8_buf;
}
#endif // START_UP_TEST

// End of utf8.c
