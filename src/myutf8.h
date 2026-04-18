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

// |VAGUE_CHAR_AS_WIDE|ON_DEMAND_WCWIDTH| meaning                 |
// |------------------|-----------------|-------------------------|
// |    undefined     |    undefined    | vague character's width is gotten from 'wcwidth()' |
// |    DEFINED       |    --           | vague character's width is assumed as 'wide'       |
// |    undefined     |    DEFINED      | vague character's width is gotten on demand        |

#define WA_VAGUE_CHAR_AS_WIDE	// treat characters which width is vague as a wide character
#ifndef ENABLE_NCURSES
///#define ON_DEMAND_WCWIDTH	// make wcwidth table on demand (when displaying the character)
#endif // ENABLE_NCURSES

int my_mblen(const char *utf8c, int max_len);
int my_mbtowc(const char *utf8c, int max_len);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // myutf8_h

// End of myutf8.h
