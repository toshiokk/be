/*****************************************************************************
	myutf8.c
 *****************************************************************************/

#include "utilincs.h"

#ifdef ENABLE_UTF8
#define WIDE_CHARS		(12 + 20 + 14 + 1)
const char *my_wide_utf8c[WIDE_CHARS] = {
 "■", "□", "◆", "◇", "○", "◎", "●", "★", "☆", "☐", "☑", "☒",
 "①", "②", "③", "④", "⑤", "⑥", "⑦", "⑧", "⑨", "⑩",
 "⑪", "⑫", "⑬", "⑭", "⑮", "⑯", "⑰", "⑱", "⑲", "⑳",
 "", "", "", "", "", "", "", "", "", "", "", "", "", "",
 "" };
PRIVATE wchar_t my_wide_wchar[WIDE_CHARS] = { 0 };
#endif // ENABLE_UTF8

PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len);

#ifdef ENABLE_UTF8
#ifdef ENABLE_DEBUG
void test_my_mbwidth(void)
{
	int chr_idx;

	for (chr_idx = 0; *my_wide_utf8c[chr_idx]; chr_idx++) {
		flf_d_printf("[%s]: %d\n", my_wide_utf8c[chr_idx],
		 my_mbwidth(my_wide_utf8c[chr_idx], MAX_UTF8C_BYTES));
		flf_d_printf("[%s]: %04x\n", my_wide_utf8c[chr_idx],
		 my_mbtowc(my_wide_utf8c[chr_idx], MAX_UTF8C_BYTES));
	}
}
#endif // ENABLE_DEBUG
PRIVATE int make_my_wcwidth_table(void)
{
	int chr_idx;

	if (my_wide_wchar[0])
		return 0;	// already made
	for (chr_idx = 0; *my_wide_utf8c[chr_idx]; chr_idx++) {
		my_wide_wchar[chr_idx] = my_mbtowc(my_wide_utf8c[chr_idx], MAX_UTF8C_BYTES);
	}
	return 1;
}
#endif // ENABLE_UTF8
int my_mbwidth(const char *utf8c, int max_len)
{
	return my_wcwidth(my_mbtowc(utf8c, max_len));
}
int my_wcwidth(wchar_t wc)
{
#ifndef ENABLE_UTF8
	return 1;
#else // ENABLE_UTF8
	int chr_idx;
	int columns;

///	if (wc == 0x0000) {		// U+0000
///		return 0;
///	}
	make_my_wcwidth_table();
	for (chr_idx = 0; *my_wide_utf8c[chr_idx]; chr_idx++) {
		if (my_wide_wchar[chr_idx] == wc)
			return 2;
	}
	columns = wcwidth(wc);
	if (columns < 1)
		columns = 1;		// narrow char.
	return columns;
#endif // ENABLE_UTF8
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
#define IS_UTF8_2ND_BYTE(byte)		(((byte) & 0xc0) == 0x80)	// 10xxxxxx
PRIVATE int my_mbtowc__(wchar_t *pwc, const char *utf8c, int max_len)
{
	wchar_t wc = 0;
	int len = 1;
	int idx;

#if 0
	return mbtowc(pwc, utf8c, max_len);
#else
	if (max_len < 1) {
		*pwc = wc;
		return 0;
	}
	wc = utf8c[0];
	if ((utf8c[0] & 0x80) == 0x00) {		// 0xxxxxxx
		wc = utf8c[0] & 0x7f;				// 0xxxxxxx
		len = 1;
	} else
	if ((utf8c[0] & 0xe0) == 0xc0) {		// 110xxxxx-10xxxxxx
		wc = utf8c[0] & 0x1f;				// 110xxxxx
		len = 2;
	} else
	if ((utf8c[0] & 0xf0) == 0xe0) {		// 1110xxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x0f;				// 1110xxxx
		len = 3;
	} else
	if ((utf8c[0] & 0xf8) == 0xf0) {		// 11110xxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x07;				// 11110xxx
		len = 4;
	} else
	if ((utf8c[0] & 0xfc) == 0xf8) {		// 111110xx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x03;				// 111110xx
		len = 5;
	} else
	if ((utf8c[0] & 0xfe) == 0xfc) {		// 1111110x-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx-10xxxxxx
		wc = utf8c[0] & 0x01;				// 1111110x
		len = 6;
	}
	for (idx = 1; idx < max_len; idx++) {
		if (idx >= len)
			break;
		if (IS_UTF8_2ND_BYTE(utf8c[idx]) == 0)
			break;
		wc = (wc << 6) | (utf8c[idx] & 0x3f);
	}
	*pwc = wc;
	return len;
#endif
}

// End of myutf8.c
