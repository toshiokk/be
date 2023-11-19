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

void dump_memory(char *message, void *memory, int bytes)
{
	unsigned char *ptr;
	int cnt;
	int left;

	ptr = (unsigned char *)memory;
	e_printf("%s", message);
	for (cnt = 0; cnt < bytes; ) {
		e_printf("%08x ", ptr);
		for (left = cnt; cnt < left + 16 && cnt < bytes; cnt++) {
			if (cnt % 4 != 3)
				e_printf("%02x-", *ptr++);
			else
				e_printf("%02x ", *ptr++);
		}
		e_printf("\n");
	}
}
char *dump_str(char *str)
{
#define MAX_LN	(65536 * 2)
	static char buf[MAX_LN + 1];
	int cnt;
	int len;

	strcpy__(buf, "");
	len = 0;
	for (cnt = 0; str[cnt]; cnt++) {
		len = strnlen(buf, MAX_LN);
		if (str[cnt] == '\t') {
			strlcat__(buf, MAX_LN-len, "{TB}");
		} else if (str[cnt] == '\n') {
			strlcat__(buf, MAX_LN-len, "{NL}");
		} else if (str[cnt] == '\x7f') {
			strlcat__(buf, MAX_LN-len, "{DL}");
		} else {
			buf[len] = str[cnt];
			buf[len+1] = '\0';
		}
		len = strnlen(buf, MAX_LN);
		if (len >= MAX_LN-10)
			break;
	}
	return buf;
}

#define DEBUG_BUF_LEN	4096

#define RING_BUF_LINES		10
#ifdef RING_BUF_LINES
PRIVATE int ring_buf_idx = 0;
PRIVATE char ring_buffer[RING_BUF_LINES][DEBUG_BUF_LEN+1] =
 { "", "", "", "", "", "", "", "", "", "", };
#endif

void tflfl_d_printf_(int time, const char *file, int line,
 const char *func, const char *label,
 const char *format, ...)
{
	char buf[DEBUG_BUF_LEN+1];
	char buf_time[MAX_PATH_LEN+1] = "";
	char buf_file_line[MAX_PATH_LEN+1] = "";
	char buf_func[MAX_PATH_LEN+1] = "";
	char buf_label[MAX_PATH_LEN+1] = "";
	char buffer[DEBUG_BUF_LEN+1];
	va_list list;

	va_start(list, format);
	vsnprintf(buf, DEBUG_BUF_LEN, format, list);
	va_end(list);

	switch (time) {
	case 1:
		get_sssssspmmm(buffer);
		snprintf_(buf_time, MAX_PATH_LEN, "MSEC %s ", buffer);
		break;
	case 3:
		snprintf_(buf_time, MAX_PATH_LEN, "%s ", cur_hhcmmcss_mmm());
		break;
	case 6:
		snprintf_(buf_time, MAX_PATH_LEN, "%s ", cur_hhcmmcss_uuuuuu());
		break;
	default:
		break;
	}
	if (file[0]) {
		snprintf_(buf_file_line, MAX_PATH_LEN, "%s %d ", file, line);
	}
	if (func[0]) {
		snprintf_(buf_func, MAX_PATH_LEN, "%s ", func);
	}
	if (label[0]) {
		snprintf_(buf_label, MAX_PATH_LEN, "%s ", label);
	}
	snprintf_(buffer, DEBUG_BUF_LEN, "%s%s%s%s%s", buf_time, buf_file_line,
	 buf_func, buf_label, buf);
#ifdef RING_BUF_LINES
	strlcpy__(ring_buffer[ring_buf_idx], buffer, DEBUG_BUF_LEN);
	if (++ring_buf_idx >= RING_BUF_LINES)
		ring_buf_idx = 0;
#endif
	e_printf("%s", buffer);
}

void output_last_d_printf(void)
{
#ifdef RING_BUF_LINES
	FILE *fp;
	int lines;

	if ((fp = fopen("becrash.log", "w")) == NULL)
		return;
	for (lines = 0; lines < RING_BUF_LINES; lines++) {
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

#endif // ENABLE_DEBUG

// End of debug.c
