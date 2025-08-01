#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([BE],[250701],[BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([buffers.h])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE # MANUAL_DESCRIPTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h libintl.h limits.h locale.h stdlib.h string.h sys/ioctl.h sys/param.h sys/time.h termio.h termios.h unistd.h utime.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
#AC_FUNC_STRNLEN
AC_CHECK_FUNCS([dup2 endgrent endpwent getcwd gettimeofday mblen memmove memset mkdir realpath regcomp setenv setlocale strnlen strcasecmp strchr strerror strncasecmp strrchr strstr strtol wcwidth])

# begin MANUAL_DESCRIPTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
echo "Running manually described check ========================================"

#AM_GNU_GETTEXT
#ALL_LINGUAS="ja"

AC_CHECK_HEADERS(regex.h, regex_h=1, regex_h=0)
AC_CHECK_HEADERS(curses.h, curses_h=1, curses_h=0)
AC_CHECK_HEADERS(ncurses.h, ncurses_h=1, ncurses_h=0)
AC_CHECK_HEADERS(ncurses/curses.h, ncurses_curses_h=1, ncurses_curses_h=0)
AC_CHECK_HEADERS(ncursesw/curses.h, ncursesw_curses_h=1, ncursesw_curses_h=0)
AC_CHECK_HEADERS(pcreposix.h, pcreposix_h=1, pcreposix_h=0)
AC_CHECK_HEADERS(pcre/pcreposix.h, pcre_pcreposix_h=1, pcre_pcreposix_h=0)

AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (`int' or `void').])

AC_CHECK_FUNCS(getopt_long)

AC_ARG_ENABLE(all, [  --enable-all  Enable ALL extra functionality (kitchen sink option)])
case "$enable_all" in
yes)
	enable_filer=yes
	enable_history=yes
	enable_undo=yes
	enable_nls=yes
	enable_syntax=yes
	enable_rc=yes
	enable_regex=yes
	enable_help=yes
	enable_debug=no
	enable_nkf=yes
	enable_ncurses=yes
	enable_pcre=yes
	enable_busybox=no
	enable_highbgc=yes
	;;
no)
	enable_filer=no
	enable_history=no
	enable_undo=no
	enable_nls=no
	enable_syntax=no
	enable_rc=no
	enable_regex=no
	enable_help=no
	enable_debug=no
	enable_nkf=no
	enable_ncurses=no
	enable_pcre=no
	enable_busybox=no
	enable_highbgc=no
	;;
*)
	;;
esac

AC_ARG_ENABLE(filer, [  --enable-filer  Enable filer (enabled by default)])
case "$enable_filer" in
yes|no)		;;
*)	enable_filer=yes		;;
esac

AC_ARG_ENABLE(history, [  --enable-history  Enable search/replace history (enabled by default)])
case "$enable_history" in
yes|no)		;;
*)	enable_history=yes		;;
esac

AC_ARG_ENABLE(undo, [  --enable-undo  Enable undo capability (enabled by default)])
case "$enable_undo" in
yes|no)		;;
*)	enable_undo=yes		;;
esac

AC_ARG_ENABLE(nls, [  --enable-nls  Enable NLS (enabled by default)])
case "$enable_nls" in
yes|no)		;;
*)	enable_nls=yes		;;
esac

AC_ARG_ENABLE(syntax, [  --enable-syntax  Enable syntax highlighting (enabled by default)])
case "$enable_syntax" in
yes|no)		;;
*)	enable_syntax=yes		;;
esac

AC_ARG_ENABLE(rc, [  --enable-rc  Enable use of rc file (enabled by default)])
case "$enable_rc" in
yes|no)		;;
*)	enable_rc=yes		;;
esac

AC_ARG_ENABLE(regex, [  --enable-regex  Enable regular expressions (enabled by default)])
case "$enable_regex" in
yes|no)		;;
*)	enable_regex=yes		;;
esac

AC_ARG_ENABLE(help, [  --enable-help  Enable help message (enabled by default)])
case "$enable_help" in
yes|no)		;;
*)	enable_help=yes		;;
esac

AC_ARG_ENABLE(debug, [  --enable-debug  Enable DEBUG (disabled by default)])
case "$enable_debug" in
yes|no)		;;
*)	enable_debug=no		;;
esac

AC_ARG_ENABLE(ncurses, [  --enable-ncurses  Use the ncurses library instead of own library])
case "$enable_ncurses" in
yes|no)		;;
*)	enable_ncurses=no	;;
esac

AC_ARG_ENABLE(pcre, [  --enable-pcre  Use the pcre library instead of regex])
case "$enable_pcre" in
yes|no)		;;
*)	enable_pcre=no	;;
esac

AC_ARG_ENABLE(nkf, [  --enable-nkf  Use NKF for automatic character encoding conversion])
case "$enable_nkf" in
yes|no)		;;
*)	enable_nkf=yes	;;
esac

AC_ARG_ENABLE(busybox, [  --enable-busybox  Use BUSYBOX instead of GNU tools])
case "$enable_busybox" in
yes|no)		;;
*)	enable_busybox=no	;;
esac

AC_ARG_ENABLE(highbgc, [  --enable-highbgc  Use lighter Backgound color (8--15)])
case "$enable_highbgc" in
yes|no)		;;
*)	enable_highbgc=yes	;;
esac

echo "temporal configurations ============================:"
echo "enable_filer=$enable_filer"
echo "enable_history=$enable_history"
echo "enable_undo=$enable_undo"
echo "enable_nls=$enable_nls"
echo "enable_syntax=$enable_syntax"
echo "enable_rc=$enable_rc"
echo "enable_regex=$enable_regex"
echo "enable_ncurses=$enable_ncurses"
echo "enable_pcre=$enable_pcre"
echo "enable_nkf=$enable_nkf"
echo "enable_busybox=$enable_busybox"
echo "enable_highbgc=$enable_highbgc"
echo "enable_help=$enable_help"
echo "enable_debug=$enable_debug"

AC_CHECK_FUNC(mblen, have_mblen=1, have_mblen=0)

echo "check dependencies among configurations ============:"
dnl ---------------------------------------------------------------------------
dnl   ncurses with UTF8 support
dnl ---------------------------------------------------------------------------
if test "$curses_h" = "1" -o "$ncurses_h" = "1" -o "$ncurses_curses_h" = "1" -o "$ncursesw_curses_h" = "1"; then
	echo "checking availability of ncurses library with UTF8 support:"
	AC_CHECK_LIB(ncurses, initscr, have_ncurses=1, have_ncurses=0)
	AC_CHECK_LIB(ncursesw, add_wch, have_ncursesw=1, have_ncursesw=0)
	if test "$have_ncurses" = "1" ; then
		AC_DEFINE(HAVE_NCURSES, 1, [Define to 1 if you have ncurses.])
	fi
	if test "$have_ncursesw" = "1" ; then
		AC_DEFINE(HAVE_NCURSESW, 1, [Define to 1 if you have ncurses with Unicode support.])
	fi
fi

dnl ---------------------------------------------------------------------------
dnl   terminal interface selection between own library and NCURSES
dnl ---------------------------------------------------------------------------
case "$enable_ncurses" in
yes)
	if test "$have_ncurses" = "1" -o "$have_ncursesw" = "1" ; then
		enable_ncurse=yes
	else
		enable_ncurse=no
	fi
	;;
esac

dnl ---------------------------------------------------------------------------
dnl   library selection of regex and PCRE(Perl Compatible Regular Expressions)
dnl ---------------------------------------------------------------------------
case "$enable_pcre" in
yes)
	if test "$pcreposix_h" = "1" -o "$pcre_pcreposix_h" = "1" ; then
		AC_CHECK_LIB(pcreposix, regcomp, enable_pcre=yes, enable_pcre=no)
	else
		enable_pcre=no
	fi
	;;
esac

dnl ---------------------------------------------------------------------------
dnl   character encoding conversion using NKF filter
dnl ---------------------------------------------------------------------------
AC_MSG_CHECKING([usability of NKF filter ...])
case "$enable_nkf" in
yes)
	have_nkf=0
	if nkf -g main.c >/dev/null 2>/dev/null ; then
		have_nkf=1
	fi
	if test "$have_nkf" = "1" ; then
		AC_DEFINE(HAVE_NKF, 1, [Define to 1 if you have nkf.])
	fi
	;;
esac
AC_MSG_RESULT($enable_nkf)

AC_MSG_CHECKING([usability of BSD-script ...])
AC_CHECK_PROG(have_bsd_script, script, yes, no)
if test $have_bsd_script = no ; then
	echo " BDS-script program is not available."
	echo " To get logging from command execution, install BDS-script program by 'apt install bsdutils'."
fi

# ---------------------------------------------------------------------------
# check configuration dependencies
# ---------------------------------------------------------------------------
# dependencies between configurations:
#  configulation   depends on ...
#  --------------  -----------------------
#  enable_syntax   enable_regex, enable_rc
#  enable_highbgc  enable_ncurses=no

if test "$enable_syntax" = yes ; then
	if test "$enable_rc" = no ; then
		echo "FIXED: --enable-syntax requires --enable-rc."
		echo "  So turn on --enable-rc."
		enable_rc=yes
	fi
	if test "$enable_regex" = no ; then
		echo "FIXED: --enable-syntax requires --enable-regex."
		echo "  So turn on --enable-regex."
		enable_regex=yes
	fi
fi

use_ncurses=0
use_ncursesw=0
if test "$enable_ncurses" = yes ; then
	if test "$have_ncurses" = "1" -a "$have_mblen" = "1" ; then
		use_ncurses=1
		AC_DEFINE(USE_NCURSES, 1, [Define this to use ncurses.])
		LIBS="$LIBS -lncurses"
	elif test "$have_ncursesw" = "1" -a "$have_mblen" = "1" ; then
		use_ncursesw=1
		AC_DEFINE(USE_NCURSESW, 1, [Define this to use ncurses.])
		LIBS="$LIBS -lncursesw"
	fi
	if test "$use_ncurses" = "0" -a "$use_ncursesw" = "0" ; then
		echo "No ncurses library found. So I turn off --enable-ncurses."
		enable_ncurses=no
	fi
fi
if test "$enable_ncurses" = yes ; then
	if test "$enable_highbgc" = "yes" ; then
		echo "ncurses does not support lighter background color. So turn off --enable-highbgc."
		enable_highbgc="no"
	fi
fi

# output #define ENABLE_... and #define USE_...

if test "$enable_filer" = yes ; then
	AC_DEFINE(ENABLE_FILER, 1, [Define this to enable filer.])
fi
if test "$enable_history" = yes ; then
	AC_DEFINE(ENABLE_HISTORY, 1, [Define this to have search/execution/directory history and keyboard macro.])
fi
if test "$enable_undo" = yes ; then
	AC_DEFINE(ENABLE_UNDO, 1, [Define this to have undo capability.])
fi
if test "$enable_nls" = yes ; then
	AC_DEFINE(ENABLE_NLS, 1, [Define this to use NLS.])
fi
if test "$enable_syntax" = yes ; then
	AC_DEFINE(ENABLE_SYNTAX, 1, [Define this to have syntax highlighting, requires ENABLE_RC and ENABLE_REGEX too!])
fi
if test "$enable_rc" = yes ; then
	AC_DEFINE(ENABLE_RC, 1, [Define this to use the rc file.])
fi
if test "$enable_regex" = yes ; then
	AC_DEFINE(ENABLE_REGEX, 1, [Define this to use regular expressions.])
fi
if test "$enable_help" = yes ; then
	AC_DEFINE(ENABLE_HELP, 1, [Define this to enable help message.])
fi
if test "$enable_debug" = yes ; then
	AC_DEFINE(ENABLE_DEBUG, 1, [Define this to use DEBUG.])
fi
if test "$enable_ncurses" = yes ; then
	AC_DEFINE(ENABLE_NCURSES, 1, [Define this to use ncurses library instead of own library.])
fi
if test "$enable_pcre" = yes ; then
	AC_DEFINE(USE_PCRE, 1, [Define this to use the pcre instead of posix regex.])
	LIBS="$LIBS -lpcreposix"
fi
if test "$enable_nkf" = yes ; then
	AC_DEFINE(USE_NKF, 1, [Define this to use NKF for automatic character encoding conversion.])
fi
if test "$enable_busybox" = yes ; then
	AC_DEFINE(USE_BUSYBOX, 1, [Define this to use BUSYBOX.])
fi
if test "$enable_highbgc" = yes ; then
	AC_DEFINE(ENABLE_HIGH_BGC, 1, [Define this to use HIGH_BGC.])
fi

echo "final configurations ===============================:"
echo "enable_filer=$enable_filer"
echo "enable_history=$enable_history"
echo "enable_undo=$enable_undo"
echo "enable_nls=$enable_nls"
echo "enable_syntax=$enable_syntax"
echo "enable_rc=$enable_rc"
echo "enable_regex=$enable_regex"
echo "enable_help=$enable_help"
echo "enable_debug=$enable_debug"
echo "enable_ncurses=$enable_ncurses"
echo "enable_pcre=$enable_pcre"
echo "enable_nkf=$enable_nkf"
echo "enable_busybox=$enable_busybox"
echo "enable_highbgc=$enable_highbgc"

echo "have_ncurses=$have_ncurses"
echo "have_ncursesw=$have_ncursesw"
echo "have_mblen=$have_mblen"
echo "use_ncurses=$use_ncurses"
echo "use_ncursesw=$use_ncursesw"

echo "library option: LIBS=$LIBS"

# end MANUAL_DESCRIPTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
