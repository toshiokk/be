/*****************************************************************************
	myutf8.c
 *****************************************************************************/

#include "utilincs.h"
#include "tio.h"

PRIVATE int is_wide_chr(wchar_t wc);

#if defined(WA_VAGUE_CHAR_WIDTH) || defined(ON_DEMAND_WCWIDTH)
PRIVATE int is_vague_narrow_chr(wchar_t wc);
PRIVATE int is_vague_wide_chr(wchar_t wc);
#endif // defined(WA_VAGUE_CHAR_WIDTH) || defined(ON_DEMAND_WCWIDTH)

#ifdef ON_DEMAND_WCWIDTH
PRIVATE void init_wcwidth_cache();
PRIVATE char get_wcwidth_on_demand(wchar_t wc);
#endif // ON_DEMAND_WCWIDTH

PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len);

int my_mbwidth(const char *utf8c, int max_len)
{
	return my_wcwidth(my_mbtowc(utf8c, max_len));
}
int my_wcwidth(wchar_t wc)
{
	if (is_wide_chr(wc)) {
		return 2;
	}
	int columns;
#ifdef WA_VAGUE_CHAR_WIDTH
	if (is_vague_narrow_chr(wc)) {
		return 1;
	}
	if (is_vague_wide_chr(wc)) {
		return 2;
	}
#endif // WA_VAGUE_CHAR_WIDTH
#ifdef ON_DEMAND_WCWIDTH
	if (is_vague_narrow_chr(wc) || is_vague_wide_chr(wc)) {
		columns = get_wcwidth_on_demand(wc);
		if (columns) {
			return columns;
		}
	}
#endif // ON_DEMAND_WCWIDTH
	columns = wcwidth(wc);
	if (columns < 1)
		columns = 1;		// narrow char.
	return columns;
}

PRIVATE int is_wide_chr(wchar_t wc)
{
	return 0
	// WIDE_CHAR_LIST
	 || (0x3000 <= wc && wc < 0xa000)
	;
}
#if defined(WA_VAGUE_CHAR_WIDTH) || defined(ON_DEMAND_WCWIDTH)
PRIVATE int is_vague_narrow_chr(wchar_t wc)
{
	return 0
	 || ((0x0080 <= wc) && (wc < 0x0100))	// Upper area of ASCII8
	 || ((0x2400 <= wc) && (wc < 0x2422))	// "␀" ~ "␡"
	 || ((0x2800 <= wc) && (wc < 0x2900))	// "⢀", "⢁" ...
	 || ((0xd800 <= wc) && (wc < 0xe000))	// Surrogate pairs are displayed in narrow by my own rule
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
#endif // defined(WA_VAGUE_CHAR_WIDTH) || defined(ON_DEMAND_WCWIDTH)
#ifdef ON_DEMAND_WCWIDTH
// -1: not investigated yet
//  0: investigation failed
//  1: narrow character
//  2: wide character
PRIVATE char wcwidth_cache[65536] = { 0 };	// 0x0000 -- 0xffff
PRIVATE void init_wcwidth_cache()
{
	if (wcwidth_cache[0] == 0) {
		memset(wcwidth_cache, -1, sizeof(wcwidth_cache));
	}
}
PRIVATE char get_wcwidth_on_demand(wchar_t wc)
{
	init_wcwidth_cache();
	wc = MIN_MAX_(0, wc, 65535);
	if (wcwidth_cache[wc] < 0) {
		char width = investigate_wcwidth(wc);
		if (width < 0) {
			width = 0;
		}
		wcwidth_cache[wc] = width;
	}
	return wcwidth_cache[wc];	// 0, 1, 2
}
#endif // ON_DEMAND_WCWIDTH

// wctomb() in standard C library can not convert surrogate pair word from 0xd800 ~ 0xdfff.
// So I provide my_wctomb() which can do it.
int my_wctomb(char *utf8c, wchar_t wc)
{
	if (wc < 0x80) {		// 0xxxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_1(wc);
		utf8c[1] = '\0';
		return 1;
	} else
	if (wc < 0x0800) {		// 00000yyy yyxxxxxx
		// 110yyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_2(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_2(wc);
		utf8c[2] = '\0';
		return 2;
	} else
	if (wc < 0x010000) {	// zzzzyyyy yyxxxxxx
		// 1110zzzz 10yyyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_3(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_3(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_3(wc);
		utf8c[3] = '\0';
		return 3;
	} else
	if (wc < 0x0200000) {	// 00000000 000wwwzz zzzzyyyy yyxxxxxx
		// 11110www 10zzzzzz 10yyyyyy 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_4(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_4(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_4(wc);
		utf8c[3] = WC_TO_UTF8_BYTE3_4(wc);
		utf8c[4] = '\0';
		return 4;
	} else
	if (wc < 0x04000000) {	// 000000vv wwwwwwzz zzzzyyyy yyxxxxxx
		// 111110vv 10wwwwww 10yyyyyy 10xxxxxx 10xxxxxx
		utf8c[0] = WC_TO_UTF8_BYTE0_5(wc);
		utf8c[1] = WC_TO_UTF8_BYTE1_5(wc);
		utf8c[2] = WC_TO_UTF8_BYTE2_5(wc);
		utf8c[3] = WC_TO_UTF8_BYTE3_5(wc);
		utf8c[4] = WC_TO_UTF8_BYTE4_5(wc);
		utf8c[5] = '\0';
		return 5;
	} else /* if (wc < 0x80000000) */ {	// 0uvvvvvv wwwwwwzz zzzzyyyy yyxxxxxx
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

// End of myutf8.c
