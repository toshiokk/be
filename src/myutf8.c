/*****************************************************************************
	myutf8.c
 *****************************************************************************/

#include "utilincs.h"
#include "tio.h"

PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len);

int my_mbwidth(const char *utf8c, int max_len)
{
#ifndef VAGUE_WIDTH_CHAR
	return my_wcwidth(my_mbtowc(utf8c, max_len));
#else // VAGUE_WIDTH_CHAR
	wchar_t wc = my_mbtowc(utf8c, max_len);
	return my_wcwidth(wc);
#endif // VAGUE_WIDTH_CHAR
}
int my_wcwidth(wchar_t wc)
{
	int columns;
	if (is_wide_chr(wc)) {
		return 2;
	}
	if (is_vague_width_chr(wc)) {
#ifndef ON_DEMAND_WCWIDTH
		return 2;
#else // ON_DEMAND_WCWIDTH
		columns = get_wcwidth_cache(wc);
		if (columns) {
			return columns;
		}
#endif // ON_DEMAND_WCWIDTH
	}
	columns = wcwidth(wc);
	if (columns < 1)
		columns = 1;		// narrow char.
	return columns;
}

#ifdef VAGUE_WIDTH_CHAR
int is_wide_chr(wchar_t wc)
{
	return 0
	// WIDE_CHAR_LIST
	 || (0x3000 <= wc && wc < 0xa000)
	;
}
int is_vague_width_chr(wchar_t wc)
{
	return 0
	// VAGUE_WIDTH_CHAR_LIST
	 || (0x02c0 <= wc && wc < 0x03a0)
	 || (0x2000 <= wc && wc < 0x2800)
	 || (0x2900 <= wc && wc < 0x2e80)
	 || (0x2fc0 <= wc && wc < 0x3400)
	 || (0xa000 <= wc && wc < 0xac00)
	 || (0xe000 <= wc && wc < 0xf900)
	;
}
#ifdef ON_DEMAND_WCWIDTH
// -1: not investigated yet
//  0: investigation failed
//  1: narrow character
//  2: wide character
char wcwidth_cache[65536] = { 0 };	// 0x0000 -- 0xffff
void clear_wcwidth_cache()
{
	if (wcwidth_cache[0] == 0) {
		memset(wcwidth_cache, -1, sizeof(wcwidth_cache));
	}
}
char get_wcwidth_cache(wchar_t wc)
{
	clear_wcwidth_cache();
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
#endif // VAGUE_WIDTH_CHAR

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
#define IS_UTF8_2ND_BYTE(byte)		(((byte) & 0xc0) == 0x80)	// 10xxxxxx
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
		if (IS_UTF8_2ND_BYTE(utf8c[idx]) == 0)
			break;		// UTF8 sequence trancated !!
		wc = (wc << 6) | (utf8c[idx] & 0x3f);
	}
	len = idx;
	*pwc = wc;
	return len;		// return length
}

// End of myutf8.c
