// stdincs.h

#ifndef stdincs_h
#define stdincs_h

#include "config.h"

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif // HAVE_LIMITS_H

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <time.h>
#include <utime.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <locale.h>
#include <limits.h>
#include <assert.h>
#include <fnmatch.h>

#ifdef ENABLE_UTF8
#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif // __USE_XOPEN
#include <wchar.h>		// wcwidth()
#endif // ENABLE_UTF8

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif // HAVE_TERMIOS_H

#ifdef HAVE_TERMIO_H
#include <termio.h>
#endif // HAVE_TERMIO_H

#ifdef ENABLE_REGEX

#if !(defined(HAVE_REGEX_H) || defined(HAVE_PCREPOSIX_H) || defined(HAVE_PCRE_PCREPOSIX_H))
// If no reg-exp header available, turn off ENABLE_REGEX.
#warning "**** No reg-exp header available, turn off ENABLE_REGEX ****"
#undef ENABLE_REGEX
#endif

#endif // ENABLE_REGEX


#ifdef ENABLE_REGEX

#ifdef USE_PCRE

#ifdef HAVE_PCREPOSIX_H
#include <pcreposix.h>
#else // HAVE_PCREPOSIX_H
#ifdef HAVE_PCRE_PCREPOSIX_H
#include <pcre/pcreposix.h>
#endif // HAVE_PCRE_PCREPOSIX_H
#endif // HAVE_PCREPOSIX_H

#else // USE_PCRE

#ifdef HAVE_REGEX_H
#include <regex.h>
#endif // HAVE_REGEX_H

#endif // USE_PCRE

#endif // ENABLE_REGEX


#ifdef ENABLE_NLS
#  ifdef HAVE_LIBINTL_H
#    include <libintl.h>
#  endif // HAVE_LIBINTL_H
#  define _(string) gettext(string)
#else // ENABLE_NLS
#  define _(string) (string)
#endif // ENABLE_NLS

#define P_(singular, plural, type3, type4, number)	\
 select_plural_form(singular, plural, type3, type4, number)


#ifndef BOOL
#define BOOL	unsigned char
#endif

#define UINT8	unsigned char
#define INT8	char
#define UINT16	unsigned short
#define INT16	short
#define UINT32	unsigned long
#define INT32	long

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif

#endif // stdincs_h

// End of stdincs.h
