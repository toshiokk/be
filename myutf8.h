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

#define VAGUE_WIDTH_CHAR	// treat characters which width is vague as a wide character
#ifdef VAGUE_WIDTH_CHAR
#ifndef ENABLE_NCURSES
#define ON_DEMAND_WCWIDTH	// make wcwidth table on demand (when displaying the character)
#endif // ENABLE_NCURSES

// |VAGUE_WIDTH_CHAR|ON_DEMAND_WCWIDTH| meaning                 |
// |----------------|-----------------|-------------------------|
// |    DEFINED     |    DEFINED      | vague character's width will be investigated on demand |
// |    DEFINED     |    undefined    | vague character's width is assumed as `wide`           |
// |    undefined   |    undefined    | invalid setting (error) |
// |    undefined   |    DEFINED      | invalid setting (error) |

int is_wide_chr(wchar_t wc);
int is_vague_width_chr(wchar_t wc);
#ifdef ON_DEMAND_WCWIDTH
void clear_wcwidth_cache();
char get_wcwidth_cache(wchar_t wc);
#endif // ON_DEMAND_WCWIDTH
#endif // VAGUE_WIDTH_CHAR

int my_mblen(const char *utf8c, int max_len);
int my_mbtowc(const char *utf8c, int max_len);

#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // myutf8_h

// End of myutf8.h
