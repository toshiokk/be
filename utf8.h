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

// DNU: MB_LEN_MAX is defined to "16" in the latest Glibc header.
#define MAX_UTF8C_BYTES		4

#define UTF8_ZEN_SPACE				"　"	// Zenkaku(full width) space    "\xe3\x80\x80"
#define UTF8_ZEN_SPACE_NOTATION		"＿"	// Zenkaku(full width) low line "\xef\xbc\xbf"

#ifdef START_UP_TEST
void test_utf8c_encode(void);
#define DUMP_UTF8_BUF_LEN	(18+1)	// "00-00-00-00-00-00-"
const char *dump_utf8c(const char *utf8c, char *buf);
void test_utf8c_bytes(void);
void test_wcwidth(void);
#endif // START_UP_TEST

int utf8s_chars(const char *utf8s);
int utf8s_columns(const char *utf8s, int bytes);
int utf8c_bytes(const char *utf8s);
int utf8c_len(char utf8c_state, char utf8c);
int utf8c_prev_bytes(const char *utf8s_min, const char *utf8s);
int utf8c_columns(const char *utf8s);
wchar_t utf8c_decode(const char *utf8c);
int utf8c_encode(wchar_t wc, char *utf8c);

#endif // utf8_h

// End of utf8.h
