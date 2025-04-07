/**************************************************************************
 *   termif.c - Linux terminal interface                                  *
 *                                                                        *
 *   Copyright (C) 2020-2020 Toshio Koike                                 *
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

#include "headers.h"

#ifndef ENABLE_NCURSES

// Escape sequences used:
//   ESC c						// clear screen
//   ESC [ 6 n					// report cursor pos.
//   ESC [ ?25h"				// cursor ON
//   ESC [ ?25l"				// cursor OFF
//   ESC [ {row} ; {col} H		// set cursor pos.
//   ESC [ {n} m				// set attributes (n : 0, 1, 7, 30--37, 40--47)

//   ESC [ 21m					// DNU: Linux console does NOT support this.
//   ESC [ 27m					// DNU: Linux console does NOT support this.
//   ESC [ 90m					// DNU: Linux console does NOT support this.
//   ESC [ 100m					// DNU: Linux console does NOT support this.

////#define TERMIF_MAX_SCRN_COLS		384		// = 1920[pixels] / 5[pixels/char] (Full HD)
////#define TERMIF_MAX_SCRN_COLS		512		// = 2560[pixels] / 5[pixels/char] (WQXGA)
#define TERMIF_MAX_SCRN_COLS		768		// = 3840[pixels] / 5[pixels/char] (4K landscape)
////#define TERMIF_MAX_SCRN_COLS		1536	// = 7680[pixels] / 5[pixels/char] (8K landscape)

////#define TERMIF_MAX_SCRN_LINES		108		// = 1080[pixels] / 10[pixels/char] (Full HD)
#define TERMIF_MAX_SCRN_LINES		384		// = 3840[pixels] / 10[pixels/char] (4K portrait)
////#define TERMIF_MAX_SCRN_LINES		768		// = 7680[pixels] / 10[pixels/char] (8K portrait)

#define TERMIF_LINE_BUF_LEN			(TERMIF_MAX_SCRN_COLS * MAX_UTF8C_BYTES)

// Virtual screen
// ++++-------------------------------- Background color
// ||||++++---------------------------- Foreground color
// |||||||| +-------------------------- 1:Reverse
// |||||||| |+------------------------- 1:Bold
// |||||||| ||+------------------------ 1:1st place of wide char., 0:narrow char.
// |||||||| |||+++++-++++++++-++++++++- UCS21
// |||||||| |||||||| |||||||| ||||||||
// bbbbffff RBWccccc cccccccc cccccccc
#define VSCR_CHAR_BGC		0xf0000000UL
#define VSCR_CHAR_FGC		0x0f000000UL
#define VSCR_CHAR_REV		0x00800000UL
#define VSCR_CHAR_BOLD		0x00400000UL
#define VSCR_CHAR_WIDTH1	0x00000000UL	// narrow char. or 2nd place of wide char.
#define VSCR_CHAR_WIDTH2	0x00200000UL	// 1st place of wide char.
#define VSCR_IS_COL1_WIDE_CHAR(vscr_char)	((vscr_char) & VSCR_CHAR_WIDTH2)
#define VSCR_CHAR_UCS21		0x001fffffUL	// 21 bits
#define VSCR_CHAR_UCS21_SPACE		' '		// space
#define VSCR_CHAR_ATTRS		(VSCR_CHAR_BGC | VSCR_CHAR_FGC | VSCR_CHAR_REV | VSCR_CHAR_BOLD)
#define VSCR_CHAR_ATTRS_DEFAULT				0x07000000UL
#define VSCR_CHAR_ATTRS_FROM_BGC(bgc)		(((bgc) << 28) & VSCR_CHAR_BGC)	// 0xf0000000
#define VSCR_CHAR_ATTRS_FROM_FGC(fgc)		(((fgc) << 24) & VSCR_CHAR_FGC)	// 0x0f000000
#define SET_ATTRS_FROM_BGC(attrs, bgc)		\
	((attrs) = VSCR_CHAR_ATTRS_FROM_BGC(bgc) | ((attrs) & ~VSCR_CHAR_BGC))
#define SET_ATTRS_FROM_FGC(attrs, fgc)		\
	((attrs) = VSCR_CHAR_ATTRS_FROM_FGC(fgc) | ((attrs) & ~VSCR_CHAR_FGC))
#define BGC_FROM_VSCR_CHAR_ATTRS(attrs)		(((attrs) & VSCR_CHAR_BGC) >> 28)	// 0x0f
#define FGC_FROM_VSCR_CHAR_ATTRS(attrs)		(((attrs) & VSCR_CHAR_FGC) >> 24)	// 0x0f
#define VSCR_CHAR_REPLACE_WIDTH_UCS21(vscr_char, width, ucs21)	\
	(vscr_char) = ((vscr_char) & VSCR_CHAR_ATTRS) | (width) | (ucs21)
typedef unsigned long vscreen_char_t;
typedef vscreen_char_t vscreen_buf_t[TERMIF_MAX_SCRN_LINES][TERMIF_MAX_SCRN_COLS];
PRIVATE vscreen_char_t vscreen_to_paint[TERMIF_MAX_SCRN_LINES][TERMIF_MAX_SCRN_COLS];
PRIVATE vscreen_char_t vscreen_painted[TERMIF_MAX_SCRN_LINES][TERMIF_MAX_SCRN_COLS];

PRIVATE vscreen_char_t termif_attrs;
PRIVATE char termif_cursor_on = 0;
PRIVATE short termif_cursor_yy = 0;
PRIVATE short termif_cursor_xx = 0;
PRIVATE char termif_cursor_on_sent = -1;

PRIVATE int termif_lines = 25;
PRIVATE int termif_columns = 80;

//------------------------------------------------------------------------------

PRIVATE struct termios term_settings_save;	/* The user's original term settings */

//------------------------------------------------------------------------------

PRIVATE void set_string_to_vscreen(const char *string, int bytes);
PRIVATE void put_narrow_char_to_vscreen(vscreen_char_t ucs21);
PRIVATE void put_wide_char_to_vscreen(vscreen_char_t ucs21);

#ifdef ENABLE_DEBUG
#if 0
PRIVATE void dump_vscreen(int yy, int len);
#endif
#endif // ENABLE_DEBUG

PRIVATE void send_cursor_pos_string_to_term(int yy, int xx, const char *string, int bytes);
PRIVATE void send_cursor_on_to_term(char on_off);
PRIVATE void send_cursor_pos_to_term(short yy, short xx);
PRIVATE int receive_cursor_pos_from_term(int *yy, int *xx);
PRIVATE vscreen_char_t attrs_sent = VSCR_CHAR_ATTRS_DEFAULT;	// attributes sent to terminal
PRIVATE void send_attrs_to_term(vscreen_char_t attrs);
PRIVATE void send_all_off_to_term(void);
PRIVATE void send_bold_to_term(int bold);
PRIVATE void send_bgc_to_term(int bgc);
PRIVATE void send_fgc_to_term(int fgc);

PRIVATE void send_printf_to_term(const char *format, ...);
PRIVATE void send_string_to_term(const char *string, int bytes);
PRIVATE void send_string_to_term__(const char *string, int bytes);

//------------------------------------------------------------------------------

int termif_init(void)
{
	save_term_settings(&term_settings_save);
	return 0;
}
int termif_begin(void)
{
	set_term_no_intr();
	set_term_raw();
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	termif_get_screen_size_from_term();
	termif_attrs = VSCR_CHAR_ATTRS_DEFAULT;
	termif_clear_screen();
	return 0;
}
int termif_end(void)
{
	send_all_off_to_term();
	send_cursor_on_to_term(1);
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	restore_term_settings(&term_settings_save);
	termif_clear_screen();
	return 0;
}
//------------------------------------------------------------------------------

char investigate_wcwidth(wchar_t wc)
{
	char utf8c[MAX_UTF8C_BYTES+1];
	utf8c_encode(wc, utf8c);
	return investigate_utf8c_width(utf8c);
}
char investigate_utf8c_width(const char *utf8c)
{
	int yy;
	int xx;
	send_cursor_pos_to_term(0, 0);
	send_string_to_term(utf8c, -1);
	if (receive_cursor_pos_from_term(&yy, &xx)) {
		return MIN_MAX_(1, xx - 0, 2);	// 1 / 2
	}
	return -1;	// not gotten
}
//------------------------------------------------------------------------------
// get screen size from terminal
int termif_get_screen_size_from_term(void)
{
	int termif_lines_save;
	int termif_columns_save;
	int lines;
	int cols;

#define MAX_REPORT_TRIES		3
	termif_lines_save = termif_get_lines();
	termif_columns_save = termif_get_columns();
	termif_set_screen_size(TERMIF_MAX_SCRN_LINES, TERMIF_MAX_SCRN_COLS);
	for (int tries = 0; tries < MAX_REPORT_TRIES; tries++) {
		send_cursor_pos_to_term(TERMIF_MAX_SCRN_LINES-1, TERMIF_MAX_SCRN_COLS-1);
		if (receive_cursor_pos_from_term(&lines, &cols)) {
			if (lines < TERMIF_MAX_SCRN_LINES && cols < TERMIF_MAX_SCRN_COLS) {
mflf_d_printf("cols: %d, lines: %d\n", cols + 1, lines + 1);
				termif_set_screen_size(lines + 1, cols + 1);
				return 1;
			}
		}
	}
	termif_set_screen_size(termif_lines_save, termif_columns_save);
	return 0;
}
void termif_set_screen_size(int lines, int columns)
{
	termif_lines = lines;
	termif_columns = columns;
}
int termif_get_lines(void)
{
	return termif_lines;
}
int termif_get_columns(void)
{
	return termif_columns;
}
//------------------------------------------------------------------------------
void termif_clear_screen(void)
{
	termif_clear_vscreen_to_paint();
	termif_clear_vscreen_painted();
	// screen state is cached and not transfered to the console until 'termif_refresh()' called
	send_string_to_term("\x1b" "c", -1);
}
void termif_clear_vscreen_to_paint(void)
{
	for (int yy = 0; yy < termif_lines; yy++) {
		for (int xx = 0; xx < termif_columns; xx++) {
			vscreen_to_paint[yy][xx] = VSCR_CHAR_ATTRS_DEFAULT | VSCR_CHAR_UCS21_SPACE;
		}
	}
}
void termif_clear_vscreen_painted(void)
{
	for (int yy = 0; yy < termif_lines; yy++) {
		for (int xx = 0; xx < termif_columns; xx++) {
			vscreen_painted[yy][xx] = VSCR_CHAR_ATTRS_DEFAULT | VSCR_CHAR_UCS21;
		}
	}
}
void termif_set_cursor_pos(short yy, short xx)
{
	if (yy >= 0 && xx >= 0) {
		termif_cursor_yy = yy;
		termif_cursor_xx = xx;
	}
	// cursor position is cached and not transfered to the console until 'termif_refresh()' called
}
void termif_get_cursor_pos(int *yy, int *xx)
{
	*yy = termif_cursor_yy;
	*xx = termif_cursor_xx;
}
void termif_set_cursor_on(char on_off)
{
	termif_cursor_on = on_off;
	// cursor state is cached and not transfered to the console until 'termif_refresh()' called
}

void termif_set_attrs(int bgc, int fgc, int rev)
{
	if (rev >= 0) {
		if (rev == 0) {
			termif_attrs &= ~VSCR_CHAR_REV;
		} else {
			termif_attrs |= VSCR_CHAR_REV;
		}
	}
	if (bgc >= 0) {
		SET_ATTRS_FROM_BGC(termif_attrs, bgc);
	}
	if (fgc >= 0) {
		SET_ATTRS_FROM_FGC(termif_attrs, fgc);
	}
}
void termif_output_string(short yy, short xx, const char *string, int bytes)
{
	if (yy >= 0 && xx >= 0) {
		termif_cursor_yy = yy;
		termif_cursor_xx = xx;
	}
	if (bytes < 0) {
		bytes = strlen(string);
	}
	set_string_to_vscreen(string, bytes);
	// output is cached in buffer
	// and not transfered to the console until 'termif_refresh()' called
}

//------------------------------------------------------------------------------

PRIVATE void set_string_to_vscreen(const char *string, int bytes)
{
	const char *str;
	vscreen_char_t ucs21;
	int width;

	for (str = string; (str - string) < bytes; str += utf8c_bytes(str)) {
		ucs21 = utf8c_decode(str);
		width = utf8c_columns(str);
		if (width == 1) {			// narrow char.
			put_narrow_char_to_vscreen(ucs21);
		} else if (width == 2) {	// wide char.
			put_wide_char_to_vscreen(ucs21);
		}
	}
}
PRIVATE void put_narrow_char_to_vscreen(vscreen_char_t ucs21)
{
	if ((0 <= termif_cursor_yy && termif_cursor_yy < termif_lines)
	 && (0 <= termif_cursor_xx && termif_cursor_xx < termif_columns)) {
#define CHECK_AND_CORRECT_INCONSISTENCY
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
		if ((0 <= termif_cursor_xx-1)
		 && VSCR_IS_COL1_WIDE_CHAR(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1])) {
			// putting narrow char. to the 2nd byte of wide char.
			//                       clear 1st byte of wide char.
			//  v      v
			// [] ==> _A
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
		if (VSCR_IS_COL1_WIDE_CHAR(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx])
		 && (termif_cursor_xx+1 < termif_columns)) {
			// putting narrow char. to the 1st byte of wide char.
			//                       clear 2nd byte of wide char.
			// v      v
			// [] ==> A_
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
#endif // CHECK_AND_CORRECT_INCONSISTENCY
		vscreen_to_paint[termif_cursor_yy][termif_cursor_xx]
		 = (termif_attrs & VSCR_CHAR_ATTRS) | VSCR_CHAR_WIDTH1 | (ucs21 & VSCR_CHAR_UCS21);
		termif_cursor_xx++;
	}
}
PRIVATE void put_wide_char_to_vscreen(vscreen_char_t ucs21)
{
	if ((0 <= termif_cursor_yy && termif_cursor_yy < termif_lines)
	 && (0 <= termif_cursor_xx && termif_cursor_xx < termif_columns)
	 && (0 <= termif_cursor_xx+1 && termif_cursor_xx+1 < termif_columns)) {
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
		if ((0 <= termif_cursor_xx-1)
		 && VSCR_IS_COL1_WIDE_CHAR(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1])) {
			// putting 1st byte of wide char to the 2nd byte of wide char which is prevly placed.
			//                                clear 1st byte of wide char which is prevly placed.
			//  v      v
			// [] ==> _[]
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
		if (VSCR_IS_COL1_WIDE_CHAR(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+1])
		 && (termif_cursor_xx+2 < termif_columns)) {
			// putting 2nd byte of wide char to the 1st byte of prevly placed wide char.
			//                                clear 2nd byte of prevly placed wide char.
			//  v       v
			//   [] ==> []_
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+2],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
#endif // CHECK_AND_CORRECT_INCONSISTENCY
		vscreen_to_paint[termif_cursor_yy][termif_cursor_xx]
		 = (termif_attrs & VSCR_CHAR_ATTRS) | VSCR_CHAR_WIDTH2 | (ucs21 & VSCR_CHAR_UCS21);
		termif_cursor_xx++;
		vscreen_to_paint[termif_cursor_yy][termif_cursor_xx]
		 = (termif_attrs & VSCR_CHAR_ATTRS) | VSCR_CHAR_WIDTH1 | (ucs21 & VSCR_CHAR_UCS21);
		termif_cursor_xx++;
	}
}
#ifdef ENABLE_DEBUG
#if 0
PRIVATE void dump_vscreen(int yy, int len)
{
	char utf8c[MAX_UTF8C_BYTES + 1];
	for (int vscr_idx = 0; vscr_idx < 2; vscr_idx++) {
		vscreen_buf_t *buf = (vscr_idx == 0) ? &vscreen_to_paint : &vscreen_painted;
		flf_d_printf("%s(%d): ", (vscr_idx == 0) ? "topaint" : "painted", yy);
		for (int idx = 0; idx < len; idx++) {
			utf8c_encode((*buf)[yy][idx] & VSCR_CHAR_UCS21, utf8c);
			d_printf(" %s", utf8c);
		}
		d_printf("\n");
	}
}
#endif
#endif // ENABLE_DEBUG

//------------------------------------------------------------------------------
// transfer display data to the console

void termif_beep(void)
{
	send_string_to_term("\x07", -1);	// "\a"(^G)
}

// If narrow char, compare 1st place.
// If wide   char, compare 1st and 2nd places.
#define CMP_NARR_OR_WIDE_CHR()										\
	(VSCR_IS_COL1_WIDE_CHAR(vscreen_to_paint[yy][xx]) == 0			\
	 ? (vscreen_painted[yy][xx] != vscreen_to_paint[yy][xx])		\
	 : (vscreen_painted[yy][xx] != vscreen_to_paint[yy][xx]			\
	  || vscreen_painted[yy][xx+1] != vscreen_to_paint[yy][xx+1]))

// refresh screen by sending pending data in vscreen_to_paint to the screen.
void termif_refresh(void)
{
	int start_xx;
	vscreen_char_t start_attrs;
	char utf8c[MAX_UTF8C_BYTES + 1];
	wchar_t ucs21;

	for (int yy = 0; yy < termif_lines; yy++) {
		for (int xx = 0; xx < termif_columns; ) {
			start_attrs = (vscreen_to_paint[yy][xx] & VSCR_CHAR_ATTRS);
			if (CMP_NARR_OR_WIDE_CHR()) {
				char line_buf[TERMIF_LINE_BUF_LEN + 1] = "";
				start_xx = xx;
				for ( ; xx < termif_columns; ) {
					ucs21 = vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21;
					if (((vscreen_to_paint[yy][xx] & VSCR_CHAR_ATTRS) != start_attrs)
					 || CMP_NARR_OR_WIDE_CHR() == 0) {
						break;
					}
					utf8c_encode(ucs21, utf8c);
					strlcat__(line_buf, TERMIF_LINE_BUF_LEN, utf8c);
					if (VSCR_IS_COL1_WIDE_CHAR(vscreen_to_paint[yy][xx])
					 && ((vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21)
					  == (vscreen_to_paint[yy][xx+1] & VSCR_CHAR_UCS21))) {
						// wide char and, 1st and 2nd UCS are the same.
						vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
						xx++;
					}
					vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
					xx++;
				}
				if (termif_cursor_on_sent) {
					// erase cursor before painting
					send_cursor_on_to_term(0);
				}
				send_attrs_to_term(start_attrs);
				send_cursor_pos_string_to_term(yy, start_xx, line_buf, -1);
			} else {
				xx++;
			}
		}
	}
	// restore cursor if erased
	send_cursor_pos_to_term(termif_cursor_yy, termif_cursor_xx);
	send_cursor_on_to_term(termif_cursor_on);
}

PRIVATE void send_cursor_pos_string_to_term(int yy, int xx, const char *string, int bytes)
{
	send_cursor_pos_to_term(yy, xx);
	send_string_to_term(string, bytes);
}

PRIVATE void send_cursor_on_to_term(char on_off)
{
	if (on_off != termif_cursor_on_sent) {
		if (on_off) {
			send_string_to_term("\x1b[?25h", -1);
		} else {
			send_string_to_term("\x1b[?25l", -1);
		}
		termif_cursor_on_sent = on_off;
	}
}
PRIVATE void send_cursor_pos_to_term(short yy, short xx)
{
	// always send cursor position, because cursor position in the console may be forwarded
	if ((0 <= yy && yy < termif_lines)
	 && (0 <= xx && xx < termif_columns)) {
#if 1
		send_printf_to_term("\x1b[%d;%dH", yy+1, xx+1);
#else
		send_printf_to_term("\x1b[%d;%df", yy+1, xx+1);
#endif
	} else {
		////warning_printf("(%d, %d)\n", xx, yy);	// not warn this
	}
}
PRIVATE int receive_cursor_pos_from_term(int *yy, int *xx)
{
#define MAX_REPORT_LEN		(11+11)		// "e[9;9R" -- "e[999;9999R"
	char buf[MAX_REPORT_LEN+1] = "";	// "e[999;9999R"
	char bufr[MAX_REPORT_LEN+1];		// "e[999;9999R"

	fflush(stdin);
	send_string_to_term("\x1b[6n", -1);
#define MAX_WAIT_USEC	1000000
#define SLEEP_USEC		1
#define MAX_RX_TRIES	((MAX_WAIT_USEC) / (SLEEP_USEC))
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in read()
	long usec_enter = get_usec();
	for (int tries = 0; tries < MAX_RX_TRIES; tries++) {
		usleep(SLEEP_USEC);	// wait for receiving answer back
		char time_out = 0;
		if ((get_usec() - usec_enter) >= MAX_WAIT_USEC) {
			time_out = 1;	// time out
		}
		int len;
		if ((len = read(STDIN_FILENO, bufr, MAX_REPORT_LEN)) > 0) {
			bufr[len] = '\0';
			strlcat__(buf, MAX_REPORT_LEN, bufr);
			if ((strlen(buf) >= MAX_REPORT_LEN) || (tail_char(buf) == 'R')) {
				// ESC [ lines ; cols R
				const char *ptr;
				for (ptr = buf; *ptr; ptr++) {
					// skip to a number character
					if (isdigit(*ptr))
						break;
				}
				int lines, cols;
				if (sscanf(ptr, "%d;%d", &lines, &cols) >= 2) {
					*yy = lines - 1;	// 0 --
					*xx = cols - 1;		// 0 --
					return 1;
				}
			}
		}
		if (time_out) {
			break;
		}
	}
	return 0;
}

PRIVATE void send_attrs_to_term(vscreen_char_t attrs)
{
	vscreen_char_t attrs_xor;
	attrs_xor = (attrs_sent ^ attrs) & VSCR_CHAR_ATTRS;
	if (attrs_xor == 0) {		// attributes changed ?
		return;
	}

	int bgc;
	int fgc;
	int real_bgc;
	int real_fgc;
	bgc = BGC_FROM_VSCR_CHAR_ATTRS(attrs);
	fgc = FGC_FROM_VSCR_CHAR_ATTRS(attrs);
	if ((attrs & VSCR_CHAR_REV) == 0) {
		real_bgc = bgc;
		real_fgc = fgc;
	} else {
		// reverse ==> swap foreground and background color
		real_bgc = fgc;
		real_fgc = bgc;
	}
	send_all_off_to_term();
	send_bgc_to_term(real_bgc);
	send_fgc_to_term(real_fgc);
	attrs_sent = attrs;
}
PRIVATE void send_all_off_to_term(void)
{
	send_string_to_term("\x1b[0m", -1);
	attrs_sent = VSCR_CHAR_ATTRS_DEFAULT;
}
PRIVATE void send_bold_to_term(int bold)
{
	if (bold) {
		send_string_to_term("\x1b[1m", -1);
	}
}
PRIVATE void send_bgc_to_term(int bgc)
{
#ifdef ENABLE_HIGH_BGC
	if (GET_APPMD(app_HIGH_BGC) == 0) {
#endif // ENABLE_HIGH_BGC
		send_printf_to_term("\x1b[%dm", 40 + (bgc % COLORS));
#ifdef ENABLE_HIGH_BGC
	} else {
		if (bgc < COLORS) {
			send_printf_to_term("\x1b[%dm", 40 + (bgc % COLORS));
		} else {
			// NOTE: highlight background color by ESC [ {100--107} m
			send_printf_to_term("\x1b[%dm", 100 + (bgc % COLORS));
		}
	}
#endif // ENABLE_HIGH_BGC
}
PRIVATE void send_fgc_to_term(int fgc)
{
#ifdef ENABLE_HIGH_BGC
	if (GET_APPMD(app_HIGH_BGC) == 0) {
#endif // ENABLE_HIGH_BGC
		send_printf_to_term("\x1b[%dm", 30 + (fgc % COLORS));
		send_bold_to_term(fgc >= COLORS);
#ifdef ENABLE_HIGH_BGC
	} else {
		if (fgc < COLORS) {
			send_printf_to_term("\x1b[%dm", 30 + (fgc % COLORS));
		} else {
			// NOTE: highlight foreground color by ESC [ {90--97} m
			send_printf_to_term("\x1b[%dm", 90 + (fgc % COLORS));
		}
	}
#endif // ENABLE_HIGH_BGC
}

PRIVATE void send_printf_to_term(const char *format, ...)
{
#define MAX_ESC_SEQ_LEN		(10+10)	// "e[999;999R"
	char buffer[MAX_ESC_SEQ_LEN+1];
	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(buffer, MAX_ESC_SEQ_LEN+1, format, ap);
	va_end(ap);
	send_string_to_term__(buffer, len);
}
PRIVATE void send_string_to_term(const char *string, int bytes)
{
	send_string_to_term__(string, bytes);
}
PRIVATE void send_string_to_term__(const char *string, int bytes)
{
	int written;

	if (bytes < 0) {
		bytes = strlen(string);
	}
	fcntl(STDOUT_FILENO, F_SETFL, 0);		// Block in write()
	if ((written = write(STDOUT_FILENO, string, bytes)) < bytes) {
		mflf_d_printf("ERROR: writing to STDOUT(%s)(%d < %d)\n",
		 strerror(errno), written, bytes);
		mflf_d_printf("[%s]\n", string);
	}
	fsync(STDOUT_FILENO);
}

#endif // ENABLE_NCURSES

// End of termif.c
