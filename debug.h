/**************************************************************************
 *   debug.h                                                              *
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

#ifndef debug_h
#define debug_h

///#define FORCE_ENABLE_DEBUG		// define this to force output debug log
#ifdef FORCE_ENABLE_DEBUG
#define ENABLE_DEBUG 1
#endif // FORCE_ENABLE_DEBUG

#ifdef ENABLE_DEBUG
///#define START_UP_TEST
#endif // ENABLE_DEBUG

#ifdef START_UP_TEST
#define MY_UT_INT(actual, expected)		if ((actual) != (expected)) {		\
  warning_printf("[%d] != [%d]\n", actual, expected); assert(1);			\
}
#define MY_UT_UINT(actual, expected)	if ((actual) != (expected)) {		\
  warning_printf("[%u] != [%u]\n", actual, expected); assert(1);			\
}
#define MY_UT_STR(actual, expected)		if (strcmp((actual), (expected))) {	\
  warning_printf("[%s] != [%s]\n", actual, expected); assert(1);			\
}
#define MY_UT_BIN(actual, actual_len, expected, expected_len)				\
 if ((memcmp((actual), (expected), MIN_(actual_len, expected_len)) == 0)	\
  && (actual_len != expected_len)) {										\
  warning_printf("binary data different\n");								\
    dump_memory("actual:", actual, actual_len);							\
    dump_memory("expected:", expected, expected_len);						\
}
#endif // START_UP_TEST

#ifdef ENABLE_DEBUG

#define _FN_		__FUNCTION__
#define _FL_		__FILE__
#define _LN_		__LINE__

#define _D_(debug)						_FLF_;	debug;
#define _D(debug)						_FLF;	debug;
#define fl_d_printf(args...)			tflfl_d_printf_(0, _FL_, _LN_, "", "", args)
#define flf_d_printf(args...)			tflfl_d_printf_(0, _FL_, _LN_, _FN_, "", args)
#define tflf_d_printf(args...)			tflfl_d_printf_(1, _FL_, _LN_, _FN_, "", args)
#define mflf_d_printf(args...)			tflfl_d_printf_(3, _FL_, _LN_, _FN_, "", args)
#define uflf_d_printf(args...)			tflfl_d_printf_(6, _FL_, _LN_, _FN_, "", args)
#define flfl_d_printf(label, args...)	tflfl_d_printf_(0, _FL_, _LN_, _FN_, label, args)
#define d_printf(args...)				tflfl_d_printf_(0, "", 0, "", "", args)
#define e_printf(args...)				debug_printf(args)
#define e_vprintf(args...)				debug_vprintf(args)

#else // ENABLE_DEBUG

#define _D_(debug)
#define _D(debug)
#define fl_d_printf(args...)
#define flf_d_printf(args...)
#define tflf_d_printf(args...)
#define mflf_d_printf(args...)
#define uflf_d_printf(args...)
#define flfl_d_printf(label, args...)
#define d_printf(args...)
#define e_printf(args...)
#define e_vprintf(args...)

#endif // ENABLE_DEBUG

#define progerr_printf(args...)		flfl_d_printf("PROGERR !!", args)
#define warning_printf(args...)		flfl_d_printf("WARNING !!", args)
#define fatalerr_printf(args...)	flfl_d_printf("FATALERR !!!!", args)
#define _TFLF_						tflf_d_printf("\n");
#define _MFLF_						mflf_d_printf("\n");
#define _UFLF_						uflf_d_printf("\n");
#define _FLF_						flf_d_printf("\n");
#define _FLF						flf_d_printf("");
#define _PROGERR_					flf_d_printf("PROGERR\n");
#define _FATALERR_					flf_d_printf("FATALERR\n");
#define _WARNING_					flf_d_printf("WARNING\n");

#ifdef ENABLE_DEBUG
void dump_memory(char *message, void *memory, int bytes);
void dump_string(char *message, const char* string);

void tflfl_d_printf_(int time, const char *file, int line,
 const char *func, const char *label,
 const char *format, ...);
void output_last_d_printf(void);

void set_debug_printf_output(int on1_off0);
void debug_printf(const char *format, ...);
void debug_vprintf(const char *format, va_list ap);
#endif // ENABLE_DEBUG

// Usage of "FALLTHROUGH":
//	switch (value) {
//	case VAL1:
//		c = a + b;
//		// FALLTHROUGH (necessary to clearify that this fall-through is intended)
//	case VAL2:
//		f = d + e;
//		break;
//	case VAL3:
//		// FALLTHROUGH (<== not necessary because nothing done in the section "case VAL3:")
//	case VAL4:
//		f = d + e;
//		break;
//	}

#endif // debug_h

// End of debug.h
