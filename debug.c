/**************************************************************************
 *   debug.c                                                              *
 *                                                                        *
 *   Copyright (C) 2007-2008 PCR                                          *
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

#include "utilincs.h"

#ifdef ENABLE_DEBUG
#warning "**** ENABLE_DEBUG defined (debug output is ENABLED) ****"

#define DEBUG_BUF_LEN	4096

void dump_memory(char *message, void *memory, int bytes)
{
	unsigned char *ptr;
	int cnt;
	int left;

	ptr = (unsigned char *)memory;
	e_printf("%s(%d)\n", message, bytes);
	for (cnt = 0; cnt < bytes; ) {
		e_printf("%08x ", ptr);
		for (left = cnt; cnt < left + 32 && cnt < bytes; cnt++) {
			if (cnt % 4 != 3)
				e_printf("%02x-", *ptr++);
			else
				e_printf("%02x ", *ptr++);
		}
		e_printf("\n");
	}
}
void dump_string(char *message, const char* string)
{
	char buffer[DEBUG_BUF_LEN+1] = "";
	for (const char *ptr = string; *ptr; ptr++) {
		if (*ptr == ' ' || isgraph((UINT8)*ptr)) {
			strcat_printf(buffer, DEBUG_BUF_LEN, "%c", *ptr);
		} else {
			strcat_printf(buffer, DEBUG_BUF_LEN, "%%%02x", (UINT8)*ptr);
		}
	}
	e_printf("%s [%s]\n", message, buffer);
}

#define RING_BUF_LINES		10
#ifdef RING_BUF_LINES
void put_to_ring_buffer(const char* str);
#endif

void tflfl_d_printf_(int time, const char *file, int line,
 const char *func, const char *label, 
const char *format, ...)
{
	char buffer[DEBUG_BUF_LEN+1];
	va_list list;
	va_start(list, format);
	tflfl_vsprintf(buffer, time, file, line, func, label, format, list);
	va_end(list);

	put_to_ring_buffer(buffer);
	debug_printf("%s", buffer);
	if (time & 0x10) {
		call_progerr_callback(buffer);
	}
}
const char* tflfl_sprintf_s_(int time, const char *file, int line,
 const char *func, const char *label, const char *format, ...)
{
	static char buffer[DEBUG_BUF_LEN+1];
	va_list list;
	va_start(list, format);
	const char* str = tflfl_vsprintf(buffer, time, file, line, func, label, format, list);
	va_end(list);
	return str;
}
const char* tflfl_vsprintf_s_(int time, const char *file, int line,
 const char *func, const char *label, const char *format, va_list list)
{
	static char buffer[DEBUG_BUF_LEN+1];
	return tflfl_vsprintf(buffer, time, file, line, func, label, format, list);
}
const char* tflfl_vsprintf(char *buffer, int time, const char *file, int line,
 const char *func, const char *label, const char *format, va_list list)
{
	char buf_time[MAX_PATH_LEN+1] = "";
	char buf[DEBUG_BUF_LEN+1];

	switch (time) {
	case 1:
		get_sssssspmmm(buffer);
		snprintf_(buf_time, MAX_PATH_LEN, "SEC %s", buffer);
		break;
	case 3:
		strlcat__(buf_time, MAX_PATH_LEN, cur_hhcmmcss_mmm());
		break;
	case 6:
		strlcat__(buf_time, MAX_PATH_LEN, cur_hhcmmcss_uuuuuu());
		break;
	case 9:
		strlcat__(buf_time, MAX_PATH_LEN, cur_yymmdd_hhmmss());
		break;
	default:
		break;
	}
	vsnprintf(buf, DEBUG_BUF_LEN, format, list);

	buffer[0] = '\0';
	if (buf_time[0]) {
		strlcat__(buffer, DEBUG_BUF_LEN, buf_time);
	}
	if (file[0]) {
		if (buffer[0]) { strlcat__(buffer, DEBUG_BUF_LEN, " "); }
		strcat_printf(buffer, DEBUG_BUF_LEN, "%s %d", file, line);
	}
	if (func[0]) {
		if (buffer[0]) { strlcat__(buffer, DEBUG_BUF_LEN, " "); }
		strlcat__(buffer, DEBUG_BUF_LEN, func);
	}
	if (label[0]) {
		if (buffer[0]) { strlcat__(buffer, DEBUG_BUF_LEN, " "); }
		strlcat__(buffer, DEBUG_BUF_LEN, label);
	}
	if (buffer[0]) { strlcat__(buffer, DEBUG_BUF_LEN, ": "); }
	strlcat__(buffer, DEBUG_BUF_LEN, buf);
	return buffer;
}

#ifdef RING_BUF_LINES
PRIVATE int ring_buf_idx = 0;
PRIVATE char ring_buffer[RING_BUF_LINES][DEBUG_BUF_LEN+1] =
 { "", "", "", "", "", "", "", "", "", "", };
void put_to_ring_buffer(const char* str)
{
	strlcpy__(ring_buffer[ring_buf_idx], str, DEBUG_BUF_LEN);
	if (++ring_buf_idx >= RING_BUF_LINES)
		ring_buf_idx = 0;
}
#endif

void output_last_d_printf()
{
#ifdef RING_BUF_LINES
	FILE *fp = fopen("becrash.log", "w");
	if (fp == NULL)
		return;
	for (int lines = 0; lines < RING_BUF_LINES; lines++) {
		fprintf(fp, "%s", ring_buffer[ring_buf_idx]);
		if (++ring_buf_idx >= RING_BUF_LINES)
			ring_buf_idx = 0;
	}
	if (fclose(fp) != 0) {
		// error
	}
#endif
}

PRIVATE int debug_printf_output = 0;
void set_debug_printf_output(int on1_off0)
{
	debug_printf_output = on1_off0;
	dtflf_d_printf("debug_printf() enabled\n");
}
void debug_printf(const char *format, ...)
{
	va_list list;
	va_start(list, format);
	debug_vprintf(format, list);
	va_end(list);
}
void debug_vprintf(const char *format, va_list ap)
{
	if (debug_printf_output) {
		vfprintf(stderr, format, ap);
	}
}

PRIVATE progerr_callback_t progerr_callback = NULL;
void set_progerr_callback(progerr_callback_t callback)
{
	progerr_callback = callback;
}
int call_progerr_callback(const char* message)
{
	if (progerr_callback) {
		return progerr_callback(message);
	}
	return 0;
}
#endif // ENABLE_DEBUG

// End of debug.c
