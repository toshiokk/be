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
//   ESC [ {n} m				// set attributes
//     n : 0, 1, 7, 30--37, 40--47

////#define TERMIF_MAX_SCRN_COLS		384		// = 1920[pixels] / 5[pixels/char] (Full HD)
////#define TERMIF_MAX_SCRN_COLS		512		// = 2560[pixels] / 5[pixels/char] (WQXGA)
#define TERMIF_MAX_SCRN_COLS		768		// = 3840[pixels] / 5[pixels/char] (4K landscape)
////#define TERMIF_MAX_SCRN_LINES		108		// = 1080[pixels] / 10[pixels/char] (Full HD)
#define TERMIF_MAX_SCRN_LINES		384		// = 3840[pixels] / 10[pixels/char] (4K portrait)
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
#define VSCR_IS_WIDE_CHAR_COL1(vscr_char)	((vscr_char) & VSCR_CHAR_WIDTH2)
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
PRIVATE int termif_cursor_on = 0;
PRIVATE int termif_cursor_yy = 0;
PRIVATE int termif_cursor_xx = 0;

PRIVATE int termif_lines = 25;
PRIVATE int termif_columns = 80;

//-----------------------------------------------------------------------------

PRIVATE struct termios term_settings_save;	/* The user's original term settings */

//-----------------------------------------------------------------------------

PRIVATE void set_string_to_vscreen(const char *string, int bytes);
PRIVATE void put_narrow_char_to_vscreen(vscreen_char_t ucs21);
PRIVATE void put_wide_char_to_vscreen(vscreen_char_t ucs21);

#ifdef ENABLE_DEBUG
PRIVATE void dump_vscreen(int yy, int len);
#endif // ENABLE_DEBUG

PRIVATE void send_cursor_pos_string_to_term(int yy, int xx, const char *string, int bytes);
PRIVATE void send_cursor_on_to_term(int on_off);
PRIVATE void send_cursor_pos_to_term(int yy, int xx);
PRIVATE vscreen_char_t attrs_sent = VSCR_CHAR_ATTRS_DEFAULT;	// attributes sent to terminal
PRIVATE void send_attrs_to_term(vscreen_char_t attrs);
PRIVATE void send_all_off_to_term(void);
PRIVATE void send_bold_to_term(int bold);
///PRIVATE void send_reverse_to_term(int reverse);
PRIVATE void send_bgc_to_term(int bgc);
PRIVATE void send_fgc_to_term(int fgc);
PRIVATE void send_printf_to_term(const char *format, ...);
PRIVATE void send_string_to_term(const char *string, int bytes);
PRIVATE void send_string_to_term__(const char *string, int bytes);
PRIVATE void send_string_to_term_flush(void);

PRIVATE key_code_t input_and_decode_key_sequences(void);
PRIVATE key_code_t compare_key_seq(char *key_sequences);
PRIVATE key_code_t input_key(void);

//-----------------------------------------------------------------------------

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
	fcntl(STDIN_FILENO, F_SETFL, 0);				// block in getchar()
	restore_term_settings(&term_settings_save);
	return 0;
}
//-----------------------------------------------------------------------------
// get screen size from terminal
int termif_get_screen_size_from_term(void)
{
	int termif_lines_save;
	int termif_columns_save;
	int tries;
	int lines;
	int cols;

#define MAX_REPORT_TRIES		3
	termif_lines_save = termif_get_lines();
	termif_columns_save = termif_get_columns();
	termif_set_screen_size(999, 999);
	for (tries = 0; tries < MAX_REPORT_TRIES; tries++) {
		send_cursor_pos_to_term(998, 998);
		if (termif_get_cursor_pos(&lines, &cols)) {
			if (lines <= TERMIF_MAX_SCRN_LINES && cols <= TERMIF_MAX_SCRN_COLS) {
				termif_set_screen_size(lines, cols);
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
//-----------------------------------------------------------------------------
void termif_clear_screen(void)
{
	send_all_off_to_term();
	send_printf_to_term("\x1b" "c");
	memset((void *)vscreen_to_paint, 0x00, sizeof(vscreen_to_paint));
	termif_clear_vscreen_painted();
}
void termif_clear_vscreen_painted(void)
{
	memset((void *)vscreen_painted, 0x00, sizeof(vscreen_painted));
}
void termif_set_cursor_pos(int yy, int xx)
{
	if (yy >= 0 && xx >= 0) {
		termif_cursor_yy = yy;
		termif_cursor_xx = xx;
		send_cursor_pos_to_term(termif_cursor_yy, termif_cursor_xx);
	}
}
int termif_get_cursor_pos(int *yy, int *xx)
{
#define MIN_REPORT_LEN		6	// "e[9;9R"
#define MAX_REPORT_LEN		(10+10)	// "e[999;999R"
	char buf[MAX_REPORT_LEN+1];	// "e[999;999R"
	int len;
	const char *ptr;
	int lines, cols;

	fflush(stdin);
	send_printf_to_term("\x1b[6n");
	MSLEEP(100);	// wait for receiving answer back
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in read()
	if ((len = read(STDIN_FILENO, buf, MAX_REPORT_LEN)) >= MIN_REPORT_LEN) {
		buf[len] = '\0';
flf_d_printf("report:[%s]\n", buf);
		for (ptr = buf; *ptr; ptr++) {
			if (isdigit(*ptr))
				break;
		}
flf_d_printf("report:[%s]\n", ptr);
		// ESC [ lines ; cols R
		if (sscanf(ptr, "%d;%d", &lines, &cols) >= 2) {
flf_d_printf("cursor-pos: %d, %d\n", lines, cols);
			*yy = lines;
			*xx = cols;
			return 1;
		}
	}
	return 0;
}
void termif_set_attrs(int bgc, int fgc, int rev)
{
///flf_d_printf("bgc:%d, fgc:%d, rev:%d, bold:%d\n", bgc, fgc, rev, bold);
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
///flf_d_printf("attrs: %08lx\n", termif_attrs);
}

void termif_set_cursor_on(int on_off)
{
	termif_cursor_on = on_off;
	send_cursor_on_to_term(on_off);
}
void termif_beep(void)
{
	send_printf_to_term("\x07");	// "\a"(^G)
}
void termif_output_string(int yy, int xx, const char *string, int bytes)
{
	if (yy >= 0 && xx >= 0) {
		termif_cursor_yy = yy;
		termif_cursor_xx = xx;
	}
	if (bytes < 0) {
		bytes = strlen(string);
	}
	set_string_to_vscreen(string, bytes);
}
PRIVATE void set_string_to_vscreen(const char *string, int bytes)
{
	const char *str;
	vscreen_char_t ucs21;
	int width;

///flf_d_printf("[%s]\n", string);
///flf_d_printf("bytes: %d\n", bytes);
	for (str = string; str - string < bytes; str += utf8c_bytes(str)) {
///flf_d_printf("%d\n", str - string);
		ucs21 = utf8c_decode(str);
		width = utf8c_columns(str);
///flf_d_printf("%04x: %d\n", ucs21, width);
		if (width == 1) {			// narrow char.
			put_narrow_char_to_vscreen(ucs21);
		} else if (width == 2) {	// wide char.
			put_wide_char_to_vscreen(ucs21);
		}
	}
///_FLF_
}
PRIVATE void put_narrow_char_to_vscreen(vscreen_char_t ucs21)
{
	if ((0 <= termif_cursor_yy && termif_cursor_yy < termif_lines)
	 && (0 <= termif_cursor_xx && termif_cursor_xx < termif_columns)) {
#define CHECK_AND_CORRECT_INCONSISTENCY
#ifdef CHECK_AND_CORRECT_INCONSISTENCY
		if ((0 <= termif_cursor_xx-1)
		 && VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1])) {
			// putting narrow char. to the 2nd byte of wide char.
			//                       clear 1st byte of wide char.
			//  v      v
			// [] ==> _A
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
		if (VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx])
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
		 && VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1])) {
			// putting 1st byte of wide char to the 2nd byte of wide char which is prevly placed.
			//                                clear 1st byte of wide char which is prevly placed.
			//  v      v
			// [] ==> _[]
			VSCR_CHAR_REPLACE_WIDTH_UCS21(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx-1],
			 VSCR_CHAR_WIDTH1, VSCR_CHAR_UCS21_SPACE);
		}
		if (VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[termif_cursor_yy][termif_cursor_xx+1])
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
#endif // ENABLE_DEBUG
//-----------------------------------------------------------------------------
// If narrow char, compare 1st place.
// If wide   char, compare 1st and 2nd place.
#define CMP_NARR_OR_WIDE_CHR()										\
	(VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[yy][xx]) == 0			\
	 ? (vscreen_painted[yy][xx] != vscreen_to_paint[yy][xx])		\
	 : (vscreen_painted[yy][xx] != vscreen_to_paint[yy][xx]			\
	  || vscreen_painted[yy][xx+1] != vscreen_to_paint[yy][xx+1]))

#ifdef VAGUE_WIDE_CHR
///
#define OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
#endif // VAGUE_WIDE_CHR
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
// If is_vague_wide_chr(), display this char as if wide char.
// (display "  " and display this wide char to 1st place)
// Expected effect:
//   If this char is narrow char, displayed "N ".
//   If this char is wide   char, displayed "WW".
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE

// refresh screen by sending pending data in vscreen_to_paint to the screen.
void termif_refresh(void)
{
	int cursor_on = 1;
	int yy, xx;
	int start_xx;
	vscreen_char_t start_attrs;
	char utf8c[MAX_UTF8C_BYTES + 1];
	wchar_t ucs21;
	char line_buf[TERMIF_LINE_BUF_LEN + 1];

	for (yy = 0; yy < termif_lines; yy++) {
		for (xx = 0; xx < termif_columns; ) {
			start_attrs = (vscreen_to_paint[yy][xx] & VSCR_CHAR_ATTRS);
			if (CMP_NARR_OR_WIDE_CHR()) {
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
				ucs21 = vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21;
				if (is_vague_wide_chr(ucs21)) {
					if (cursor_on) {
						// erase cursor before painting
						send_cursor_on_to_term(0);
						cursor_on = 0;
					}
					send_attrs_to_term(start_attrs);
					send_cursor_pos_string_to_term(yy, xx, "  ", -1);	// "  "
					utf8c_encode(ucs21, utf8c);
					send_cursor_pos_string_to_term(yy, xx, utf8c, -1);	// "H " or "WW"
					vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
					xx++;
					vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
					xx++;
				} else {
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
					start_xx = xx;
					strcpy(line_buf, "");
					for ( ; xx < termif_columns; ) {
						ucs21 = vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21;
						if (((vscreen_to_paint[yy][xx] & VSCR_CHAR_ATTRS) != start_attrs)
						 || CMP_NARR_OR_WIDE_CHR() == 0
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
						 || is_vague_wide_chr(ucs21)
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
													) {
							break;
						}
						utf8c_encode(ucs21, utf8c);
						strlcat__(line_buf, TERMIF_LINE_BUF_LEN, utf8c);
						if (VSCR_IS_WIDE_CHAR_COL1(vscreen_to_paint[yy][xx])
						 && ((vscreen_to_paint[yy][xx] & VSCR_CHAR_UCS21)
						  == (vscreen_to_paint[yy][xx+1] & VSCR_CHAR_UCS21))) {
							// wide char and, 1st and 2nd UCS are the same.
							vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
							xx++;
						}
						vscreen_painted[yy][xx] = vscreen_to_paint[yy][xx];
						xx++;
					}
					if (cursor_on) {
						// erase cursor before painting
						send_cursor_on_to_term(0);
						cursor_on = 0;
					}
					send_attrs_to_term(start_attrs);
					send_cursor_pos_string_to_term(yy, start_xx, line_buf, -1);
#ifdef OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
				}
#endif // OUTPUT_VAGUE_WIDE_CHR_AS_WIDE
			} else {
				xx++;
			}
		}
	}
	if (cursor_on == 0) {
		// If cursor erased in this func, restore cursor.
		send_cursor_pos_to_term(termif_cursor_yy, termif_cursor_xx);
		send_cursor_on_to_term(termif_cursor_on);
	}
///_FLF_
}

PRIVATE void send_cursor_pos_string_to_term(int yy, int xx, const char *string, int bytes)
{
	send_cursor_pos_to_term(yy, xx);
	send_string_to_term(string, bytes);
}

PRIVATE void send_cursor_on_to_term(int on_off)
{
	if (on_off) {
		send_printf_to_term("\x1b[?25h");
	} else {
		send_printf_to_term("\x1b[?25l");
	}
}
PRIVATE void send_cursor_pos_to_term(int yy, int xx)
{
	if ((0 <= yy && yy < termif_lines)
	 && (0 <= xx && xx < termif_columns)) {
#if 1
		send_printf_to_term("\x1b[%d;%dH", yy+1, xx+1);
#else
		send_printf_to_term("\x1b[%d;%df", yy+1, xx+1);
#endif
	} else {
		warning_printf("(%d, %d)\n", xx, yy);
mflf_d_printf("(%d, %d)\n", termif_columns, termif_lines);
	}
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
	real_bgc = LIMIT_BGC(real_bgc);
	real_fgc = LIMIT_FGC(real_fgc);
	real_fgc = differ_fgc_to_bgc(real_bgc, real_fgc);
	send_all_off_to_term();
	send_bgc_to_term(real_bgc);
	send_fgc_to_term(real_fgc);
	attrs_sent = attrs;
}
PRIVATE void send_all_off_to_term(void)
{
	send_printf_to_term("\x1b[0m");
	attrs_sent = VSCR_CHAR_ATTRS_DEFAULT;
}
PRIVATE void send_bold_to_term(int bold)
{
	if (bold) {
		send_printf_to_term("\x1b[1m");
///	} else {
///		send_printf_to_term("\x1b[21m");	// DNU: Linux console does NOT support this.
	}
}
///PRIVATE void send_reverse_to_term(int reverse)
///{
///	if (reverse) {
///		send_printf_to_term("\x1b[7m");
//////	} else {
//////		send_printf_to_term("\x1b[27m");	// DNU: Linux console does NOT support this.
///	}
///}
PRIVATE void send_bgc_to_term(int bgc)
{
	send_printf_to_term("\x1b[%dm", 40 + (bgc % COLORS));
	// NOTE: Linux console supports highlight only for foreground
	//       and does not support highlight for background
}
PRIVATE void send_fgc_to_term(int fgc)
{
	send_printf_to_term("\x1b[%dm", 30 + (fgc % COLORS));
	send_bold_to_term(fgc >= COLORS);
}
PRIVATE void send_printf_to_term(const char *format, ...)
{
#define MAX_ESC_SEQ_LEN		(10+10)	// "e[999;999R"
	va_list ap;
	char buffer[MAX_ESC_SEQ_LEN+1];

	va_start(ap, format);
	vsnprintf(buffer, MAX_ESC_SEQ_LEN+1, format, ap);
	va_end(ap);
///d_printf("XXX{%s}\n", buffer);
	send_string_to_term__(buffer, -1);
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
	send_string_to_term_flush();
}
PRIVATE void send_string_to_term_flush(void)
{
	fsync(STDOUT_FILENO);
}

//-----------------------------------------------------------------------------

key_code_t termif_input_key(void)
{
	return input_and_decode_key_sequences();
}

#define MAX_KEY_SEQ		7

struct _key_seq_ {
	key_code_t key;
	char *sequences;
} key_seq_table[] = {
	//			 1---2---3---4---5---6---7---
	// linux console
	{ K_F01,	"\x1b\x5b\x5b\x41" },
	{ K_F02,	"\x1b\x5b\x5b\x42" },
	{ K_F03,	"\x1b\x5b\x5b\x43" },
	{ K_F04,	"\x1b\x5b\x5b\x44" },
	{ K_F05,	"\x1b\x5b\x5b\x45" },
	{ K_F06,	"\x1b\x5b\x31\x37\x7e" },
	{ K_F07,	"\x1b\x5b\x31\x38\x7e" },
	{ K_F08,	"\x1b\x5b\x31\x39\x7e" },
	{ K_F09,	"\x1b\x5b\x32\x30\x7e" },
	{ K_F10,	"\x1b\x5b\x32\x31\x7e" },
	{ K_F11,	"\x1b\x5b\x32\x33\x7e" },
	{ K_F12,	"\x1b\x5b\x32\x34\x7e" },
	{ K_SF01,	"\x1b\x5b\x32\x35\x7e" },
	{ K_SF02,	"\x1b\x5b\x32\x36\x7e" },
	{ K_SF03,	"\x1b\x5b\x32\x38\x7e" },
	{ K_SF04,	"\x1b\x5b\x32\x39\x7e" },
	{ K_SF05,	"\x1b\x5b\x33\x31\x7e" },
	{ K_SF06,	"\x1b\x5b\x33\x32\x7e" },
	{ K_SF07,	"\x1b\x5b\x33\x33\x7e" },
	{ K_SF08,	"\x1b\x5b\x33\x34\x7e" },
	{ K_IC,		"\x1b\x5b\x32\x7e" },
	{ K_DC,		"\x1b\x5b\x33\x7e" },
	{ K_HOME,	"\x1b\x5b\x31\x7e" },
	{ K_END,	"\x1b\x5b\x34\x7e" },
	{ K_PPAGE,	"\x1b\x5b\x35\x7e" },
	{ K_NPAGE,	"\x1b\x5b\x36\x7e" },
	{ K_UP,		"\x1b\x5b\x41" },
	{ K_DOWN,	"\x1b\x5b\x42" },
	{ K_LEFT,	"\x1b\x5b\x44" },
	{ K_RIGHT,	"\x1b\x5b\x43" },
	// xterm
	{ K_F01,	"\x1b\x4f\x50" },
	{ K_F02,	"\x1b\x4f\x51" },
	{ K_F03,	"\x1b\x4f\x52" },
	{ K_F04,	"\x1b\x4f\x53" },
	{ K_F05,	"\x1b\x5b\x31\x35\x7e" },
	{ K_F06,	"\x1b\x5b\x31\x37\x7e" },
	{ K_F07,	"\x1b\x5b\x31\x38\x7e" },
	{ K_F08,	"\x1b\x5b\x31\x39\x7e" },
	{ K_F09,	"\x1b\x5b\x32\x30\x7e" },
	{ K_F10,	"\x1b\x5b\x32\x31\x7e" },
	{ K_F11,	"\x1b\x5b\x32\x33\x7e" },
	{ K_F12,	"\x1b\x5b\x32\x34\x7e" },
	{ K_SF01,	"\x1b\x4f\x32\x50" },
	{ K_SF02,	"\x1b\x4f\x32\x51" },
	{ K_SF03,	"\x1b\x4f\x32\x52" },
	{ K_SF04,	"\x1b\x4f\x32\x53" },
	{ K_SF05,	"\x1b\x5b\x31\x35\x3b\x32\x7e" },
	{ K_SF06,	"\x1b\x5b\x31\x37\x3b\x32\x7e" },
	{ K_SF07,	"\x1b\x5b\x31\x38\x3b\x32\x7e" },
	{ K_SF08,	"\x1b\x5b\x31\x39\x3b\x32\x7e" },
	{ K_SF09,	"\x1b\x5b\x32\x30\x3b\x32\x7e" },
	{ K_SF10,	"\x1b\x5b\x32\x31\x3b\x32\x7e" },
	{ K_SF11,	"\x1b\x5b\x32\x33\x3b\x32\x7e" },
	{ K_SF12,	"\x1b\x5b\x32\x34\x3b\x32\x7e" },
	{ K_HOME,	"\x1b\x5b\x48" },
	{ K_END,	"\x1b\x5b\x46" },
//	{ KEY_SDC,	"\x1b\x5b\x33\x3b\x32\x7e" },
//	{ KEY_SHOME,"\x1b\x5b\x32\x48" },
//	{ KEY_SEND,	"\x1b\x5b\x32\x46" },
};

PRIVATE key_code_t input_and_decode_key_sequences(void)
{
	key_code_t key1;
	key_code_t key2;
	char key_sequences[MAX_KEY_SEQ+1];
///	static char key_sequences[MAX_KEY_SEQ+1];
	int key_idx;

again:;
	key_idx = 0;
	if ((key1 = input_key()) < 0) {
		return key1;
	}
	if (key1 == 0x1b) {
		key_sequences[key_idx++] = key1;
		key_sequences[key_idx] = '\0';
		key1 = -1;
		for ( ; key_idx < MAX_KEY_SEQ; ) {
			if ((key2 = input_key()) < 0)
				break;
			key_sequences[key_idx++] = key2;
			key_sequences[key_idx] = '\0';
			if ((key1 = compare_key_seq(key_sequences)) >= 0)
				break;
		}
	}
	if (key1 < 0) {
		if (strlen(key_sequences) > 1)
			goto again;
		key1 = key_sequences[0];
	}
	return key1;
}

PRIVATE key_code_t compare_key_seq(char *key_sequences)
{
	int seq_idx;

	if (strlen(key_sequences) == 2) {
		for (seq_idx = 0; seq_idx < sizeof(key_seq_table) / sizeof(key_seq_table[0]);
		 seq_idx++) {
			if (strncmp(key_sequences, key_seq_table[seq_idx].sequences, 2) == 0)
				return -1;
		}
		return (key_sequences[0] << 8) | key_sequences[1];
	} else if (strlen(key_sequences) >= 3) {
		for (seq_idx = 0; seq_idx < sizeof(key_seq_table) / sizeof(key_seq_table[0]);
		 seq_idx++) {
			if (strcmp(key_sequences, key_seq_table[seq_idx].sequences) == 0)
				return key_seq_table[seq_idx].key;
		}
	}
	return -1;
}

PRIVATE key_code_t input_key(void)
{
	key_code_t key = -1;
	unsigned char buf[1];

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	if (read(STDIN_FILENO, buf, 1) >= 1) {
		key = buf[0];
	}
////	flf_d_printf("getch():%04x %d\n", key, key);
	return key;
}

void investigate_key_sequence(void)
{
	key_code_t key;
	char key_sequences[MAX_KEY_SEQ+1];
///	static char key_sequences[MAX_KEY_SEQ+1];
	int key_idx = 0;
	int keys;
	long start_msec;

	termif_init();
	termif_begin();
	for (key = -1 ; key < 0; ) {
		key = input_key();
	}
	key_sequences[key_idx] = key;
	key_idx++;
	key_sequences[key_idx] = '\0';
	for (start_msec = get_msec(); key_idx < MAX_KEY_SEQ && get_msec() < start_msec + 1000; ) {
		if ((key = input_key()) >= 0) {
			key_sequences[key_idx] = key;
			key_idx++;
			key_sequences[key_idx] = '\0';
		}
	}
	keys = key_idx;
	termif_end();
	printf("\n");
	for (key_idx = 0; key_idx < keys; key_idx++) {
		printf("%02x ", key_sequences[key_idx]);
	}
	printf("\n");
}

#endif // ENABLE_NCURSES

// End of termif.c
