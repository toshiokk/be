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

#ifdef START_UP_TEST
void test_utf8c_encode(void)
{
	char utf8c[MAX_UTF8C_BYTES+1];
#ifdef ENABLE_DEBUG
	char buf[DUMP_UTF8_BUF_LEN+1];
#endif // ENABLE_DEBUG

	for (wchar_t wc = 0x0000; wc <= 0x1fffff; wc++) {	// 21 bits(UCS21)
		utf8c_encode(wc, utf8c);
		flf_d_printf("%06x ==> [%s] ==> %06x\n", wc, dump_utf8c(utf8c, buf), utf8c_decode(utf8c));
		if (wc >= 0x10000) {
			wc += (0x1000 - 1);
		} else if (wc >= 0x100) {
			wc += (0x100 - 1);
		}
	}
}
const char *dump_utf8c(const char *utf8c, char *buf)
{
	strcpy__(buf, "");
	for (int idx = 0; idx < strlen(utf8c); idx++) {
		strcat_printf(buf, DUMP_UTF8_BUF_LEN+1, "%02x-", (unsigned char)utf8c[idx]);
	}
	return buf;
}

void test_utf8c_bytes(void)
{
	const char str[] = "\xed\xa0\x80\xed\xa0\x81 \xed\xa0\xa0\xed\xa0\xa1";
	const char *ptr = str;
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 3);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 6);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 7);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 10);
	ptr += utf8c_bytes(ptr);	MY_UT_INT(ptr - str, 13);
}
void test_wcwidth(void)
{
	wchar_t wc;
#ifdef ENABLE_DEBUG
	int columns;
#endif // ENABLE_DEBUG

	for (wc = 0x0000; wc < 0xffff; wc++) {
		columns = my_wcwidth(wc);
		if ((wc & 0x1f) == 0) {
			flf_d_printf("%04x", wc);
		}
		d_printf(" %d", columns);
		if ((wc & 0x1f) == 0x1f) {
			d_printf("\n");
		}
	}
	d_printf("\n");
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

int utf8c_len(char utf8c_state, char utf8c)
{
	if (utf8c_state <= 0) {
		if ((unsigned char)utf8c < 0x80) {	// 0xxxxxxx
			utf8c_state = 0;
		} else
		if ((utf8c & 0xe0) == 0xc0) {	// 110xxxxx-10xxxxxx
			utf8c_state = 1;
		} else
		if ((utf8c & 0xf0) == 0xe0) {	// 1110xxxx-10xxxxxx-10xxxxxx
			utf8c_state = 2;
		} else
		if ((utf8c & 0xf8) == 0xf0) {	// 11110xxx-10xxxxxx-10xxxxxx-10xxxxxx
			utf8c_state = 3;
		} else
		if ((utf8c & 0xfc) == 0xf8) {	// 111110xx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
			utf8c_state = 4;
		} else
		if ((utf8c & 0xfe) == 0xfc) {	// 1111110x-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
			utf8c_state = 5;
		} else {						// 1111111x
			utf8c_state = 0;
		}
	} else {
		if ((unsigned char)utf8c < 0x80) {	// 0xxxxxxx (illegal sequence)
			utf8c_state = -1;
		} else
		if ((utf8c & 0xc0) == 0x80) {		// 10xxxxxx
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
		if (*(unsigned char *)ptr < 0x80 || 0xc0 <= *(unsigned char *)ptr)
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

int utf8c_encode(wchar_t wc, char *utf8c)
{
	int bytes = wctomb(utf8c, wc);
	utf8c[bytes] = '\0';
	return bytes;
}

// End of utf8.c
