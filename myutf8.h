/*****************************************************************************
	myutf8.h
 *****************************************************************************/

#ifndef myutf8_h
#define myutf8_h

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

#define MAX_UTF8C_BYTES			4

#ifdef ENABLE_DEBUG
#ifdef ENABLE_UTF8
void test_my_mbwidth(void);
#endif // ENABLE_UTF8
#endif // ENABLE_DEBUG

int my_mbwidth(const char *utf8c, int max_len);
int my_wcwidth(wchar_t wc);
int my_mblen(const char *utf8c, int max_len);
int my_mbtowc(const char *utf8c, int max_len);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // myutf8_h

// End of myutf8.h
