/**************************************************************************
 *   editor3.c                                                            *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
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

PRIVATE int get_buf_line_num_digits(const be_buf_t *buf);
PRIVATE int get_line_num_digits(int max_line_num);

// ruler ======================================================================

#ifdef START_UP_TEST
void test_make_ruler_text(void)
{
	MY_UT_STR(make_ruler_text(0, 40), "1---5----10---15---20---25---30---35---4");
	MY_UT_STR(make_ruler_text(97, 40), "--100--105--110--115--120--125--130--135");
	MY_UT_STR(make_ruler_text(98, 40), "-100--105--110--115--120--125--130--135-");
	MY_UT_STR(make_ruler_text(99, 40), "100--105--110--115--120--125--130--135--");
	MY_UT_STR(make_ruler_text(100, 40), "----105--110--115--120--125--130--135--1");
}
#endif // START_UP_TEST

const char *get_ruler_text(int col_idx)
{
	return make_ruler_text(col_idx, get_edit_win_columns_for_text());
}
#define RULER_NUM_INTERVAL	5
#define R_N_I				RULER_NUM_INTERVAL
#define MAX_RULER_STR_LEN	(MAX_SCRN_LINE_BUF_LEN + R_N_I)
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
//12345678901234567890123456789012345678901234567890123456789012345678901234567890
//1---5----10---15---20---25---30---35---40---45---50---55---60---65---70---75---80
//9910-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//----9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//---9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//--9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990
//-9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//9915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
//915-9920-9925-9930-9935-9940-9945-9950-9955-9960-9965-9970-9975-9980-9985-9990-9995
const char *make_ruler_text(int start_col_idx, int length)
{
	static int start_col_idx_cached = -1;
	static int length_cached = -1;
	static char ruler_line_buf[MAX_RULER_STR_LEN+1] = "";	// ruler line
	int col_num;
	char col_num_str[4+1];	// "9999"

	length = MIN(MAX_RULER_STR_LEN, length);
	if (start_col_idx_cached == start_col_idx && length_cached == length) {
		// return cached string
	} else {
		start_col_idx_cached = start_col_idx;
		length_cached = length;

		strcpy__(ruler_line_buf, "");
		// 0, 5, 10, 15, 20, ...
		for (col_num = (start_col_idx / R_N_I) * R_N_I;
		 col_num <= MAX_EDIT_LINE_LEN * get_cur_buf_tab_size();
		 col_num = ((col_num / R_N_I) + 1) * R_N_I) {
			snprintf_(col_num_str, 4+1, "%d", col_num);
			if (col_num <= start_col_idx) {
				// line head
				if (start_col_idx == 0) {
					snprintf_(ruler_line_buf, R_N_I+1, "1----");
				} else {
					snprintf_(ruler_line_buf, R_N_I+1, "-----");
				}
			} else {
										//4,9,14,19, ...
				snprintf_(&ruler_line_buf[col_num - 1 - start_col_idx], R_N_I+1,
				 "%s----", col_num_str);
			}
			if (strnlen(ruler_line_buf, MAX_RULER_STR_LEN) >= length)
				break;
		}
		ruler_line_buf[length] = '\0';	// limit length
	}
	return ruler_line_buf;
}

// line number ================================================================

//   1 #include <stdio.h>
//   2 int main(int argc, char *argv[])
// 998 #include <stdio.h>
// 999 int main(int argc, char *argv[])
//    1 #include <stdio.h>
//    2 int main(int argc, char *argv[])
// 9998 #include <stdio.h>
// 9999 int main(int argc, char *argv[])
//          1 #include <stdio.h>
//          2 int main(int argc, char *argv[])
// 9999999998 #include <stdio.h>
// 9999999999 int main(int argc, char *argv[])

#define MIN_LINE_NUM_DIGITS			3		// 999(1000-1)
#define MIN_LINE_NUM_DIGITS_NUM		1000	// 10 ^ MIN_LINE_NUM_DIGITS
#define MAX_LINE_NUM_DIGITS			10		// 9999999999(10000000000-1)
#define LINE_NUM_SEPARATOR			" "		// "999 "
#define LINE_NUM_SEPARATOR_COLS		1		// strlen(LINE_NUM_SEPARATOR);

const char *get_line_num_string(const be_buf_t *buf, const be_line_t *line,
 char *buf_line_num)
{
	if (line) {
		// "9999 "
		snprintf_(buf_line_num, MAX_LINE_NUM_STR_LEN+1,
		 "%*d" LINE_NUM_SEPARATOR, get_buf_line_num_digits(buf), line->line_num);
	} else {
		// "     "
		snprintf_(buf_line_num, MAX_LINE_NUM_STR_LEN+1,
		 "%*s" LINE_NUM_SEPARATOR, get_buf_line_num_digits(buf), "");
	}
	return buf_line_num;
}
int get_buf_line_num_columns(const be_buf_t *buf)
{
	return get_buf_line_num_digits(buf) + LINE_NUM_SEPARATOR_COLS;		// "123" + " "
}
PRIVATE int get_buf_line_num_digits(const be_buf_t *buf)
{
	return get_line_num_digits(BUF_BOT_LINE(buf)->line_num);
}
PRIVATE int get_line_num_digits(int max_line_num)
{
	int digits;		// 3,    4,     5, ...     10
	long long num;	// 1000, 10000, 100000, ...10000000000

	// 999, 9999, 99999, ... 999999999
	for (digits = MIN_LINE_NUM_DIGITS, num = MIN_LINE_NUM_DIGITS_NUM;
	 digits <= MAX_LINE_NUM_DIGITS;
	 digits++, num *= 10) {
		if (max_line_num < num)
			break;
	}
	return digits;
}

//-----------------------------------------------------------------------------
int edit_win_get_path_lines(void)
{
	if (GET_APPMD(ed_EDITOR_PANES) == 0) {
		return 0;
	} else {
		return 1;
	}
}
int edit_win_get_text_lines(void)
{
	return sub_win_get_lines() - edit_win_get_path_lines() - get_ruler_lines();
}

int edit_win_get_path_y(void)
{
	return 0;
}
int edit_win_get_ruler_y(void)
{
	return edit_win_get_path_y() + edit_win_get_path_lines();
}
int edit_win_get_text_y(void)
{
	return edit_win_get_path_y() + edit_win_get_path_lines() + get_ruler_lines();
}

#define TAB_NOTATION	'>'
#define EOL_NOTATION	'<'
// string that is tab('\t')-expanded and linefeed('\n')-concatenated
int te_lf_concat_bytes = 0;								// bytes of (raw_byte + line-feed)
char te_lf_concat_buf[MAX_EDIT_LINE_LEN * 2 +1];		// raw_byte + line-feed
// string invisible code(TAB, Zenkaku-space, EOL) converted to character('>', '__', '<')
int te_vis_code_columns;										// length of tab-expanded line
char te_vis_code_buf[MAX_EDIT_LINE_LEN * MAX_TAB_SIZE +1];		// tab-expanded-visible-code
// tab-expansion
/* T:TAB, C:control-code ZZ:Zenkaku-space, L:'\n' */
/* original:     "TabcdCefghZZijkl" */
/* control_code: "TabcdCefghZZijklL" */
/* visible_code: ">   abcd^?efgh[]ijkl<" */
const char *te_tab_expand(const char *original)
{
	int col_idx;
	int bytes;
	const char *orig_ptr;
	char *vis_ptr;
#ifdef ENABLE_SYNTAX
	int notation = 0;		// is visible Tab/EOL/Zenkaku-space notation

	notation = (GET_APPMD(ed_SYNTAX_HIGHLIGHT) && GET_APPMD(ed_TAB_EOL_NOTATION));
#endif // ENABLE_SYNTAX

	te_concat_linefeed(original);
	orig_ptr = original;
	vis_ptr = te_vis_code_buf;
	for (col_idx = 0; *orig_ptr; ) {
		if (*orig_ptr == '\t') {
#ifdef ENABLE_SYNTAX
			*vis_ptr++ = notation ? TAB_NOTATION : ' ';
#else // ENABLE_SYNTAX
			*vis_ptr++ = ' ';
#endif // ENABLE_SYNTAX
			while (++col_idx % priv_tab_size) {
				*vis_ptr++ = ' ';
			}
			orig_ptr++;
		} else if (is_ctrl_char((unsigned char)*orig_ptr)) {
			*vis_ptr++ = '^';
			*vis_ptr++ = (*orig_ptr == CHAR_DEL) ? '?' : (*orig_ptr + '@');
			col_idx += 2;
			orig_ptr++;
		} else if ((unsigned char)*orig_ptr < 0x80) {	// ASCII
			*vis_ptr++ = *orig_ptr++;
			col_idx++;
		} else {
#ifdef ENABLE_SYNTAX
			if (notation && strlcmp__(orig_ptr, UTF8_ZEN_SPACE) == 0)
				strcpy__(vis_ptr, UTF8_ZEN_SPACE_NOTATION);
			else
				strlcpy__(vis_ptr, orig_ptr, utf8c_bytes(orig_ptr));
#else // ENABLE_SYNTAX
			strlcpy__(vis_ptr, orig_ptr, utf8c_bytes(orig_ptr));
#endif // ENABLE_SYNTAX
			col_idx += utf8c_columns(orig_ptr);
			bytes = utf8c_bytes(orig_ptr);
			vis_ptr += bytes;
			orig_ptr += bytes;
		}
	}
#ifdef ENABLE_SYNTAX
	*vis_ptr++ = notation ? EOL_NOTATION : ' ';
#else // ENABLE_SYNTAX
	*vis_ptr++ = ' ';
#endif // ENABLE_SYNTAX
	*vis_ptr = '\0';
	te_vis_code_columns = col_idx + 1;
	return te_vis_code_buf;
}
const char *te_concat_linefeed(const char *original)
{
	te_vis_code_buf[0] = '\0';	// clear te_vis_code_buf
	strlcpy__(te_lf_concat_buf, original, MAX_EDIT_LINE_LEN * 2);
	strlcat__(te_lf_concat_buf, MAX_EDIT_LINE_LEN * 2, "\n");
	te_lf_concat_bytes = strnlen(te_lf_concat_buf, MAX_EDIT_LINE_LEN * 2);
	return te_lf_concat_buf;
}

int te_tab_expand__max_wl_idx(const char *original)
{
	te_tab_expand(original);
	return max_wrap_line_idx(te_lf_concat_buf, -1);
}

// End of editor3.c
