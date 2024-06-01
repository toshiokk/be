/**************************************************************************
 *   linewrap.c                                                           *
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

int priv_tab_size = DEFAULT_TAB_SIZE;	// Tab size gotten from buf_state

void set_wrap_line_tab_size(int tab_size)
{
	priv_tab_size = tab_size;
}

//-----------------------------------------------------------------------------

#ifdef START_UP_TEST
void test_wrap_line(void)
{
flf_d_printf("----------------------------------------------------------------------\n");
flf_d_printf("priv_tab_size: %d\n", priv_tab_size);
#define CMP_RET(func_call, ret)		_FLF_; if (func_call != ret) _WARNING_
#define C_R(func_call, ret)			CMP_RET(func_call, ret)
	C_R(max_wrap_line_idx("1234567890", 10), 0)
	C_R(max_wrap_line_idx("123456789012", 10), 1)
	C_R(max_wrap_line_idx("1234567890123456789012", 10), 2)
	C_R(max_wrap_line_idx("一二三四五六七八九零", 10), 1)
	C_R(max_wrap_line_idx("一二三四五六七八九零一", 10), 2)
	C_R(max_wrap_line_idx("1一二三四五六七八九", 10), 1)
	C_R(max_wrap_line_idx("1一二三四五六七八九零", 10), 2)
	C_R(max_wrap_line_idx("1一二三四五123456789012", 10), 2)
	C_R(max_wrap_line_idx("123456\t78", 10), 0)
	C_R(max_wrap_line_idx("1234567\t89", 10), 0)
	C_R(max_wrap_line_idx("12345678\t90", 10), 1)
	C_R(max_wrap_line_idx("123456789\t01", 10), 1)
	C_R(max_wrap_line_idx("123456\t12", 12), 0)
	C_R(max_wrap_line_idx("1234567\t12", 12), 0)
	C_R(max_wrap_line_idx("12345678\t12", 12), 1)
	C_R(max_wrap_line_idx("123456789\t12", 12), 1)
	C_R(max_wrap_line_idx("1234567890\t12", 12), 1)
	C_R(max_wrap_line_idx("12345678901\t12", 12), 1)
	C_R(max_wrap_line_idx("123456789012\t12", 12), 1)
	C_R(max_wrap_line_idx("1234567890123\t12", 12), 1)
	C_R(max_wrap_line_idx("12345678901234\t12", 12), 1)

	C_R(start_wl_idx_of_wrap_line("1234567890", 8, 10), 0)
	C_R(start_wl_idx_of_wrap_line("1234567890", 10, 10), 1)
	C_R(start_wl_idx_of_wrap_line("123456789012", 10, 10), 1)
	C_R(start_wl_idx_of_wrap_line("123456789012", 12, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1234567890123456789012", 20, 10), 2)
	C_R(start_wl_idx_of_wrap_line("1234567890123456789012", 22, 10), 2)

	C_R(start_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 10, 10), 0)
	C_R(start_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 10, 10), 0)
	C_R(end_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 0, 10, 10), 10)
	C_R(end_col_idx_of_wrap_line("1234567890123456789012345678901234567890", 1, 10, 10), 10)
	C_R(start_byte_idx_of_wrap_line("1234567890123456789012345678901234567890", 0, 10, 10), 10)
	C_R(start_byte_idx_of_wrap_line("1234567890123456789012345678901234567890", 1, 10, 10), 20)
	C_R(end_byte_idx_of_wrap_line_le("1234567890123456789012345678901234567890", 0, 10, 10), 10)
	C_R(end_byte_idx_of_wrap_line_le("1234567890123456789012345678901234567890", 1, 10, 10), 20)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 0, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 15, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 15, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 30, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 30, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七八九零一", 45, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 0, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 13, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 13, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 28, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 28, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七八九零一", 43, 10), 4)

	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")-1, 10), 0)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四"), 10), 0)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+2, 10), 1)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五"), 10), 1)
	C_R(start_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三"), 10), 0)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+2, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四"), 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+1, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+2, 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五"), 10), 1)
	C_R(start_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五")+1, 10), 1)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")-1, 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+1, 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+2, 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五")+1, 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+1, 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+2, 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四"), 10), 0)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+1, 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+2, 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五"), 10), 1)
	C_R(end_wl_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五")+1, 10), 1)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三"), 10), 6)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三")+1, 10), 8)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三")+2, 10), 8)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四"), 10), 8)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+1, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四")+2, 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五"), 10), 0)
	C_R(start_col_idx_of_wrap_line("一二三四五六七23456789012", strlen("一二三四五")+1, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三"), 10), 7)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+1, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三")+2, 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四"), 10), 0)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+1, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四")+2, 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五"), 10), 2)
	C_R(start_col_idx_of_wrap_line("1一二三四五六七23456789012", strlen("1一二三四五")+1, 10), 4)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 0, strlen("一二三四"), 10), 8)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 0, strlen("一二三四")+1, 10), 10)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 0, strlen("一二三四")+2, 10), 10)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 1, strlen("一二三四五"), 10), 10)
	C_R(end_col_idx_of_wrap_line("一二三四五六七23456789012", 1, strlen("一二三四五")+1, 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 0, strlen("1一二三四"), 10), 9)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 0, strlen("1一二三四")+1, 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 0, strlen("1一二三四")+2, 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 1, strlen("1一二三四五"), 10), 2)
	C_R(end_col_idx_of_wrap_line("1一二三四五六七23456789012", 1, strlen("1一二三四五")+1, 10), 4)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 7, 10), 12)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 8, 10), 12)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 9, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 10, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 0, 11, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 0, 10), 15)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 1, 10), 18)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 2, 10), 18)
	C_R(start_byte_idx_of_wrap_line("一二三四五六七23456789012", 1, 3, 10), 21)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 7, 10), 10)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 8, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 9, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 10, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 0, 11, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 0, 10), 13)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 1, 10), 16)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 2, 10), 16)
	C_R(start_byte_idx_of_wrap_line("1一二三四五六七23456789012", 1, 3, 10), 19)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 9, 10), 12)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 10, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 0, 11, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 0, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 1, 10), 15)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 2, 10), 18)
	C_R(end_byte_idx_of_wrap_line_le("一二三四五六七23456789012", 1, 3, 10), 18)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 0, 10), 0)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 9, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 10, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 0, 11, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 0, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 1, 10), 13)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 2, 10), 16)
	C_R(end_byte_idx_of_wrap_line_le("1一二三四五六七23456789012", 1, 3, 10), 16)
}
#endif // START_UP_TEST

// line wrapping macros
#define DEFINE_WRAP_LINE_VARS	\
	const char *ptr = line;		\
	int wl_idx = 0;				\
	int col_idx = 0;			\
	int total_col_idx = 0;		\
	int chr_cols;
#define CHAR_COLUMNS(ptr)													\
	((*ptr == '\0') ? 0 :													\
	 ((*ptr == '\t') ? (priv_tab_size - total_col_idx % priv_tab_size) :	\
	  ((*ptr == '\n') ? 1 :													\
	   ((is_ctrl_char((unsigned char)*ptr)) ? 2 :							\
	    (((unsigned char)*ptr < 0x80) ? 1 : utf8c_columns(ptr))				\
	   )																	\
	  )																		\
	 )																		\
	)
#define FORWARD_WRAP_LINE_COL_IDX()		\
	chr_cols = CHAR_COLUMNS(ptr);		\
	total_col_idx += chr_cols;			\
	col_idx += chr_cols;
#define FORWARD_WRAP_LINE_PTR()			\
	ptr += utf8c_bytes(ptr);
#define GOTO_NEXT_LINE_IF_LINE_EXCEED()		\
	if (*ptr == '\t') {						\
		if (col_idx >= width) {				\
			col_idx -= width;				\
			wl_idx++;						\
		}									\
	} else {								\
		if (col_idx > width) {				\
			col_idx = chr_cols;				\
			wl_idx++;						\
		}									\
	}
#define GOTO_NEXT_LINE_IF_LINE_END_COL()	\
	if (col_idx == width) {					\
		col_idx = 0;						\
		wl_idx++;							\
	}
#define GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS()	\
	if (*ptr != '\t') {						\
		/* If the next char would cause line-wrapping, */ \
		/* go to the next line earlier. */	\
		chr_cols = CHAR_COLUMNS(ptr);		\
		if (col_idx + chr_cols > width) {	\
			col_idx = 0;	/* go to top of the next line */ \
			wl_idx++;						\
		}									\
	}

//[全角wrapping]
// Ex1
//  "aaaaaaaa漢"
//  |aaaaaaaa漢|
// Ex2
//  "aaaaaaaaa漢"
//  |aaaaaaaaa |
//  |漢        |
//[TAB wrapping]
// Ex1
//  "aaaaaaaa>>>>bbbb"
//  |aaaaaaaa>>|
//  |>>bbbb    |

int max_wrap_line_idx(const char *line, int width)
{
	int line_idx;

	line_idx = end_wl_idx_of_wrap_line(line, INT_MAX, width);
	return line_idx;
}
int start_wl_idx_of_wrap_line(const char *line, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if (ptr - line >= byte_idx)
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
	return wl_idx;
}
int end_wl_idx_of_wrap_line(const char *line, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if (ptr - line >= byte_idx)
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		if (ptr - line >= byte_idx)
			break;
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
	return wl_idx;
}
// line_idx=0, byte_idx=80 ==> col_idx=0
int start_col_idx_of_wrap_line(const char *line, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if (ptr - line >= byte_idx)
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
	return col_idx;
}
// line_idx=0, byte_idx=80 ==> col_idx=80
int end_col_idx_of_wrap_line(const char *line, int line_idx, int byte_idx, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if ((ptr - line >= byte_idx)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		if ((ptr - line >= byte_idx)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
	}
	return col_idx;
}
int start_byte_idx_of_wrap_line(const char *line, int line_idx, int column, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
		if ((wl_idx >= line_idx && col_idx > column)
		 || (wl_idx > line_idx))
			break;
	}
	return ptr - line;
}
int end_byte_idx_of_wrap_line_le(const char *line, int line_idx, int column, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
		FORWARD_WRAP_LINE_COL_IDX();
		if ((wl_idx >= line_idx && col_idx > column)
		 || (wl_idx > line_idx))
			break;
		FORWARD_WRAP_LINE_PTR();
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
	}
	return ptr - line;
}
int end_byte_idx_of_wrap_line_ge(const char *line, int line_idx, int column, int width)
{
	DEFINE_WRAP_LINE_VARS

	width = wrap_line_length(width);
	for ( ; *ptr; ) {
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		FORWARD_WRAP_LINE_COL_IDX();
		FORWARD_WRAP_LINE_PTR();
		if ((wl_idx >= line_idx && col_idx >= column)
		 || (wl_idx > line_idx))
			break;
		GOTO_NEXT_LINE_IF_LINE_EXCEED();
		GOTO_NEXT_LINE_IF_LINE_END_COL();
		GOTO_NEXT_LINE_IF_NEXT_CHAR_WRAPS();
	}
	return ptr - line;
}
int wrap_line_length(int width)
{
	return width > 0
	 ? width
	 : (CUR_EBUF_STATE(buf_LINE_WRAP_MODE) == 0
	  ? (MAX_EDIT_LINE_LEN * priv_tab_size + 1)		// will never wrap
	  : get_edit_win_columns_for_text());			// wrap line by editor window width
}

//-----------------------------------------------------------------------------
#define FORWARD_COL_IDX()										\
	if (*ptr == '\t') {											\
		col_idx += (priv_tab_size - col_idx % priv_tab_size);	\
		ptr++;													\
	} else if (*ptr == '\n') {									\
		col_idx++;												\
		ptr++;													\
	} else if (is_ctrl_char((unsigned char)*ptr)) {				\
		col_idx += 2;											\
		ptr++;													\
	} else if ((unsigned char)*ptr < 0x80) {					\
		col_idx++;												\
		ptr++;													\
	} else {													\
		col_idx += utf8c_columns(ptr);							\
		ptr += utf8c_bytes(ptr);								\
	}
#define FORWARD_VIS_IDX()										\
	if (*ptr == '\t') {											\
		vis_idx += (priv_tab_size - col_idx % priv_tab_size);	\
		col_idx += (priv_tab_size - col_idx % priv_tab_size);	\
		ptr++;													\
	} else if (*ptr == '\n') {									\
		vis_idx++;												\
		col_idx++;												\
		ptr++;													\
	} else if (is_ctrl_char((unsigned char)*ptr)) {				\
		vis_idx += 2;											\
		col_idx += 2;											\
		ptr++;													\
	} else if ((unsigned char)*ptr < 0x80) {					\
		vis_idx++;												\
		col_idx++;												\
		ptr++;													\
	} else {													\
		vis_idx += utf8c_bytes(ptr);							\
		col_idx += utf8c_columns(ptr);							\
		ptr += utf8c_bytes(ptr);								\
	}

// return byte_idx not exceed bytes
int byte_idx_from_byte_idx(const char *utf8s, int bytes)
{
	const char *ptr;
	const char *prev_ptr;

	for (prev_ptr = ptr = utf8s; *ptr && ptr - utf8s < bytes; ) {
		prev_ptr = ptr;
		ptr += utf8c_bytes(ptr);
	}
	if (ptr - utf8s > bytes)
		ptr = prev_ptr;
	return ptr - utf8s;
}

int byte_idx_from_col_idx(const char *utf8s, int columns, char_left_right_t left_right,
 int *col_idx__)
{
	int col_idx;
	int prev_col_idx;
	const char *ptr;
	const char *prev_ptr;

	prev_col_idx = columns;
	for (prev_ptr = ptr = utf8s, col_idx = 0; *ptr && col_idx < columns; ) {
		prev_col_idx = col_idx;
		prev_ptr = ptr;
		FORWARD_COL_IDX()
	}
///flf_d_printf("([%s], %d) ==> %d\n", utf8s, columns, ptr - utf8s);
	if (left_right == CHAR_LEFT && columns < col_idx) {
		col_idx = prev_col_idx;
		ptr = prev_ptr;
	}
	if (col_idx__)
		*col_idx__ = col_idx;
	return ptr - utf8s;
}

int col_idx_from_byte_idx(const char *utf8s, int col_idx, int bytes)
{
	const char *ptr;

	for (ptr = utf8s; *ptr && ptr - utf8s < bytes; ) {
		FORWARD_COL_IDX()
	}
	return col_idx;
}

int vis_idx_from_byte_idx(const char *utf8s, int bytes)
{
	int vis_idx;
	int col_idx;
	const char *ptr;

	for (ptr = utf8s, col_idx = 0, vis_idx = 0; *ptr && ptr - utf8s < bytes; ) {
		FORWARD_VIS_IDX()
	}
	return vis_idx;
}

#if 0
int char_idx_from_byte_idx(const char *utf8s, int bytes);
int char_idx_from_byte_idx(const char *utf8s, int bytes)
{
	int char_idx;
	int col_idx;
	const char *ptr;

	char_idx = 0;
	for (ptr = utf8s, col_idx = 0; *ptr && ptr - utf8s < bytes;
	 char_idx++) {
		FORWARD_COL_IDX()
	}
	return char_idx;
}
int vis_idx_from_col_idx(const char *utf8s, int columns);
int vis_idx_from_col_idx(const char *utf8s, int columns)
{
	int vis_idx;
	int col_idx;
	const char *ptr;

/////_D_(dump_memory("vis_idx_from_col_idx\n", utf8s, strlen(utf8s)))
	for (ptr = utf8s, col_idx = 0, vis_idx = 0; *ptr && col_idx < columns; ) {
		FORWARD_VIS_IDX()
	}
///flf_d_printf("vis_idx_from_col_idx([%s], %d) ==> %d\n", utf8s, columns, vis_idx);
	return vis_idx;
}
#endif

//------------------------------------------------------------------------------
#ifdef START_UP_TEST
PRIVATE int test_get_intersection_(int min1, int max1, int min2, int max2,
 int expected_ret, int expected_min, int expected_max);
void test_get_intersection(void)
{
	flf_d_printf("-----------------------\n");
	// aaaaaa bbbbbb
	MY_UT_INT(test_get_intersection_(0, 54, 64, 65, -10, 64, 54), 0);
	// aaaaaa bbbbbb
	MY_UT_INT(test_get_intersection_(10, 20, 30, 40, -10, 30, 20), 0);
	// bbbbbb aaaaaa
	MY_UT_INT(test_get_intersection_(60, 100, 10, 50, -10, 60, 50), 0);

	// aaaaaabbbbbb
	MY_UT_INT(test_get_intersection_(10, 20, 20, 40, 0, 20, 20), 0);
	// bbbbbbaaaaaa
	MY_UT_INT(test_get_intersection_(60, 100, 10, 60, 0, 60, 60), 0);

	// aaaaaa
	//   bbbbbb
	MY_UT_INT(test_get_intersection_(10, 100, 90, 110, 10, 90, 100), 0);
	//   aaaaaa
	// bbbbbb
	MY_UT_INT(test_get_intersection_(50, 100, 10, 60, 10, 50, 60), 0);
	//   aaaaaa
	// bbbbbbbbbb
	MY_UT_INT(test_get_intersection_(20, 100, 10, 110, 80, 20, 100), 0);
	// aaaaaaaaaa
	//   bbbbbb
	MY_UT_INT(test_get_intersection_(10, 100, 50, 60, 10, 50, 60), 0);
}
// [not intersect]
//  <-------->
//             <-------->
// [intersect pattern 1]
//  <-------->
//      <-------->
// [intersect pattern 2]
//    <-------->
//  <------------>
PRIVATE int test_get_intersection_(int min1, int max1, int min2, int max2,
 int expected_ret, int expected_min, int expected_max)
{
	int min;
	int max;
	int ret;

	ret = get_intersection(min1, max1, min2, max2, &min, &max);
	if ((ret != expected_ret) || (min != expected_min) || (max != expected_max)) {
warning_printf("get_intersection(min1:%d, max1:%d, min2:%d, max2:%d)\n"
 " ==> %d, min:%d, max:%d ?? %d, %d, %d\n", min1, max1, min2, max2,
 ret, min, max, expected_ret, expected_min, expected_max);
		return 1;
	}
	return 0;
}
#endif // START_UP_TEST
int get_intersection(int min1, int max1, int min2, int max2, int *min, int *max)
{
	int _min, _max;

	_min = MAX_(min1, min2);
	_max = MIN_(max1, max2);
	if (min)
		*min = _min;
	if (max)
		*max = _max;
	return _max - _min;		// >0: intersect, <=0: not intersect
}

// End of linewrap.c
