/*****************************************************************************
	myutf8.h
 *****************************************************************************/

#ifndef myutf8_h
#define myutf8_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

// DNU: MB_LEN_MAX is defined to "16" in the latest Glibc header.
#define MAX_UTF8C_BYTES			6

int my_mbwidth(const char *utf8c, int max_len);
int my_wcwidth(wchar_t wc);

// |WA_VAGUE_CHAR_WIDTH|ON_DEMAND_WCWIDTH|how a vague character's widths are gotten|
// |-------------------|-----------------|-----------------------------------------|
// |    undefined      |    undefined    |widths are gotten from 'wcwidth()'       |
// |    DEFINED        |    ----         |widths are determined by my self         |
// |    undefined      |    DEFINED      |widths are determined from the terminal behavior|

///
#define WA_VAGUE_CHAR_WIDTH		// determine character width by my self when it's vague
#ifndef ENABLE_NCURSES
///
#define ON_DEMAND_WCWIDTH	// make wcwidth table on demand (when the character be displayed)
#endif // ENABLE_NCURSES

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

int my_wctomb(char *utf8c, wchar_t wc);
int my_mblen(const char *utf8c, int max_len);
int my_mbtowc(const char *utf8c, int max_len);

#ifdef START_UP_TEST
void test_my_utf8();
#endif // START_UP_TEST

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // myutf8_h

// End of myutf8.h
