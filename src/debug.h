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

// If you wish enable debugging temprarily without running ./configure, define here
////#define ENABLE_DEBUG 1

#ifdef ENABLE_DEBUG
////
#define START_UP_TEST		// define this to run unit tests at the start up of application
#endif // ENABLE_DEBUG

//------------------------------------------------------------------------------
#ifdef ENABLE_DEBUG

#define DT0			0	// no Time in Debug log
#define DTSM		1	// SSSSSS.mmm
#define DTSU		2	// SSS.uuuuuu
#define DTHM		3	// HH:MM:SS.mmm
#define DTHMT		4	// HH:MM:SS.mmm:tty
#define DTHU		6	// HH:MM:SS.uuuuuu
#define DTYH		9	// YYMMDD-HHMMSS
#define DTYHM		12	// YYMMDD-HHMMSS.mmm

#define _FL_		__FILE__
#define _LN_		__LINE__
#define _FN_		__FUNCTION__

#define _D_(debug)							_FLF_;	debug;
#define _D(debug)							_FLF;	debug;
#define fl_d_printf(args...)				tflfl_d_printf_(DT0, _FL_, _LN_, "", "", args)
#define flf_d_printf(args...)				tflfl_d_printf_(DT0, _FL_, _LN_, _FN_, "", args)
#define smflf_d_printf(args...)				tflfl_d_printf_(DTSM, _FL_, _LN_, _FN_, "", args)
#define suflf_d_printf(args...)				tflfl_d_printf_(DTSU, _FL_, _LN_, _FN_, "", args)
#define hm_d_printf(args...)				tflfl_d_printf_(DTHM, "", 0, "", "", args)
#define hmflf_d_printf(args...)				tflfl_d_printf_(DTHM, _FL_, _LN_, _FN_, "", args)
#define hmtflf_d_printf(args...)			tflfl_d_printf_(DTHMT, _FL_, _LN_, _FN_, "", args)
#define huflf_d_printf(args...)				tflfl_d_printf_(DTHU, _FL_, _LN_, _FN_, "", args)
#define dtflf_d_printf(args...)				tflfl_d_printf_(DTYH, _FL_, _LN_, _FN_, "", args)
#define dthmflf_d_printf(args...)			tflfl_d_printf_(DTYHM, _FL_, _LN_, _FN_, "", args)
#define dtflfl_d_printf(label, args...)		tflfl_d_printf_(DTYH, _FL_, _LN_, _FN_, label, args)
#define flfl_d_printf(label, args...)		tflfl_d_printf_(DT0, _FL_, _LN_, _FN_, label, args)
#define d_printf(args...)					tflfl_d_printf_(DT0, "", 0, "", "", args)
#define e_printf(args...)					debug_printf(args)
#define e_vprintf(args...)					debug_vprintf(args)
#define progerr_printf(args...)		tflfl_d_printf_(0x10 | DTYH, _FL_, _LN_, _FN_, "PROGERR", args)

#else // ENABLE_DEBUG

#define _D_(debug)
#define _D(debug)
#define fl_d_printf(args...)
#define flf_d_printf(args...)
#define smflf_d_printf(args...)
#define suflf_d_printf(args...)
#define hm_d_printf(args...)
#define hmflf_d_printf(args...)
#define hmtflf_d_printf(args...)
#define huflf_d_printf(args...)
#define dtflf_d_printf(args...)
#define dthmflf_d_printf(args...)
#define dtflfl_d_printf(label, args...)
#define flfl_d_printf(label, args...)
#define d_printf(args...)
#define e_printf(args...)
#define e_vprintf(args...)
#define progerr_printf(args...)

#endif // ENABLE_DEBUG

#define warning_printf(args...)		flfl_d_printf("WARNING !!", args)
#define fatalerr_printf(args...)	flfl_d_printf("FATALERR !!!!", args)
#define _SMFLF_						smflf_d_printf("\n");
#define _HMFLF_						hmflf_d_printf("\n");
#define _UFLF_						huflf_d_printf("\n");
#define _FLF_						flf_d_printf("\n");
#define _FLF						flf_d_printf("");
#define _PROGERR_					flf_d_printf("PROGERR\n");
#define _FATALERR_					flf_d_printf("FATALERR\n");
#define _WARNING_					flf_d_printf("WARNING\n");

#ifdef ENABLE_DEBUG

void dump_memory(char *message, void *memory, int bytes);
void dump_string(char *message, const char* string);

void tflfl_d_printf_(int time, const char *file, int line,
 const char *func, const char *label, const char *format, ...);
const char* tflfl_vsprintf(char *buffer, int time, const char *file, int line,
 const char *func, const char *label, const char *format, va_list list);
void output_last_d_printf();

void set_debug_printf_output(int on1_off0);
void debug_printf(const char *format, ...);
void debug_vprintf(const char *format, va_list ap);

typedef int (*progerr_callback_t)(const char*);
void set_progerr_callback(progerr_callback_t callback);
int call_progerr_callback(const char* message);

#endif // ENABLE_DEBUG

//------------------------------------------------------------------------------
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
    dump_memory("actual:", actual, actual_len);								\
    dump_memory("expected:", expected, expected_len);						\
}
#endif // START_UP_TEST

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
