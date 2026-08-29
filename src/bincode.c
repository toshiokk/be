/**************************************************************************
 *   bincode.c - binary code conversion                                   *
 *                                                                        *
 *   Copyright (C) 2026-2026 PCR                                          *
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

#define IS_BIN_BYTE_TO_BE_CONV(byte)			(byte == 0x00)
#define IS_BIN_BYTE_TO_BE_CONV_ASCII8(byte)		(IS_BIN_BYTE_TO_BE_CONV(byte) || (0x80 <= byte))

#define SURROGATE_PAIR_CODE		0xd800		// Surrogate pair code (0xd800~dbff,dc00~dfff)
#define BIN_00_BASE_CODE		SURROGATE_PAIR_CODE
#define BIN_80_BASE_CODE		0x0080

const char conv_bin_to_utf8_table_00[] = {
  WC_TO_UTF8_BYTE0_3(BIN_00_BASE_CODE), WC_TO_UTF8_BYTE1_3(BIN_00_BASE_CODE),
  WC_TO_UTF8_BYTE2_3(BIN_00_BASE_CODE), '\0' };
char conv_bin_to_utf8_table_80_ff[0x80][MAX_UTF8C_BYTES + 1];
//==============================================================================

PRIVATE int conv_weird_byte_remaining_bytes = 0;

PRIVATE int conv_weird_byte_holding_byte_idx = 0;
PRIVATE char conv_weird_byte_holding[MAX_UTF8C_BYTES+1] = "";
#define WEIRD_UTF8_BYTES_CONVERSION_BUF_LEN		(MAX_UTF8C_BYTES * MAX_UTF8C_BYTES)
PRIVATE int conv_weird_byte_converted_byte_idx = 0;
PRIVATE char conv_weird_byte_converted[WEIRD_UTF8_BYTES_CONVERSION_BUF_LEN+1] = "";
PRIVATE const char *conv_weird_byte_get_ptr = conv_weird_byte_holding;

void conv_weird_byte_init(char ascii8)
{
	conv_weird_byte_remaining_bytes = 0;
	conv_weird_byte_holding_byte_idx = 0;
	conv_weird_byte_converted_byte_idx = 0;

	for (short chr = 0x80; chr < 0x100; chr++) {
		char utf8c_buf[MAX_UTF8C_BYTES + 1];
		my_wctomb(utf8c_buf, BIN_80_BASE_CODE + chr - 0x80);
		if (strlcmp__(conv_bin_to_utf8_table_80_ff[chr - 0x80], utf8c_buf) == 0) {
			// already initialized
			break;
		}
		strlcpy__(conv_bin_to_utf8_table_80_ff[chr - 0x80], utf8c_buf, MAX_UTF8C_BYTES);
	}
}

// checking weird byte sequences
// (1) check weird utf8c sequence
//     if first byte is UTF8:
//       if trailing byte is weird binary byte:
//         (2) check weird binary byte
//         e.g: 0xc2, 0x00
//           0xc2 ==> 0xed, 0xa3, 0x82
//           0x00 ==> 0xed, 0xa0, 0x80
//       else:
//         e.g.: 0xc2, 0x20
//           0xc2 ==> 0xed, 0xa3, 0x82
//           0x20 ==> 0x20
//     else:
//       check weird binary byte
// (2) check weird binary byte
//     e.g.: 0x00 ==> 0xed, 0x80, 0x80
//     e.g.: 0x80 ==> 0xed, 0xa0, 0x80

PRIVATE void conv_weird_byte_hold_chr(short chr);
PRIVATE void conv_weird_byte_convert_held_bytes();
PRIVATE void conv_weird_byte_convert_byte(short chr);
PRIVATE void conv_weird_byte_put_converted_str(const char *str);
PRIVATE void conv_weird_byte_put_converted_chr(short chr);
PRIVATE void conv_weird_byte_start_sending(const char *conv_weird_byte_buf_ptr);
PRIVATE short conv_weird_byte_get();

short conv_weird_byte_first(short chr)
{
	if (conv_weird_byte_remaining_bytes == 0) {
		if (chr >= 0) {
			conv_weird_byte_remaining_bytes = UTF8_BYTES(chr);
			if (conv_weird_byte_remaining_bytes == 1) {
				conv_weird_byte_convert_byte(chr);
				conv_weird_byte_start_sending(conv_weird_byte_converted);
			} else {
				conv_weird_byte_hold_chr(chr);
				conv_weird_byte_remaining_bytes--;
			}
		}
	} else {
		if (chr >= 0) {
			if (! IS_UTF8_TRAILING_BYTE(chr)) {
				// illegal utf8 byte
				conv_weird_byte_convert_held_bytes();
				conv_weird_byte_convert_byte(chr);
				conv_weird_byte_start_sending(conv_weird_byte_converted);
			} else {
				conv_weird_byte_hold_chr(chr);
				conv_weird_byte_remaining_bytes--;
				if (conv_weird_byte_remaining_bytes == 0) {
					if (utf8c_is_redundant_code(conv_weird_byte_holding)) {
						conv_weird_byte_convert_held_bytes();
						conv_weird_byte_start_sending(conv_weird_byte_converted);
					} else {
						conv_weird_byte_start_sending(conv_weird_byte_holding);
					}
				}
			}
		} else {
			conv_weird_byte_convert_held_bytes();
			conv_weird_byte_start_sending(conv_weird_byte_converted);
		}
	}
	return conv_weird_byte_get();
}
short conv_weird_byte_next()
{
	return conv_weird_byte_get();
}

PRIVATE void conv_weird_byte_hold_chr(short chr)
{
	conv_weird_byte_holding[conv_weird_byte_holding_byte_idx++] = chr;
	conv_weird_byte_holding[conv_weird_byte_holding_byte_idx] = '\0';
}
PRIVATE void conv_weird_byte_convert_held_bytes()
{
	for (const char *str = conv_weird_byte_holding; *str ; str++) {
		conv_weird_byte_convert_byte((UCHAR)*str);
	}
}
PRIVATE void conv_weird_byte_convert_byte(short chr)
{
	if (IS_BIN_BYTE_TO_BE_CONV_ASCII8(chr)) {
		if (chr == 0x00) {
			conv_weird_byte_put_converted_str(conv_bin_to_utf8_table_00);
		} else {
			conv_weird_byte_put_converted_str(conv_bin_to_utf8_table_80_ff[chr - 0x80]);
		}
	} else {
		conv_weird_byte_put_converted_chr(chr);
	}
}
PRIVATE void conv_weird_byte_put_converted_str(const char *str)
{
	for ( ; *str ; str++) {
		conv_weird_byte_put_converted_chr((UCHAR)*str);
	}
}
PRIVATE void conv_weird_byte_put_converted_chr(short chr)
{
	conv_weird_byte_converted[conv_weird_byte_converted_byte_idx++] = chr;
	conv_weird_byte_converted[conv_weird_byte_converted_byte_idx] = '\0';
}
// start sending held or converted bytes
PRIVATE void conv_weird_byte_start_sending(const char *conv_weird_byte_buf_ptr)
{
	conv_weird_byte_remaining_bytes = 0;
	conv_weird_byte_holding_byte_idx = 0;
	conv_weird_byte_converted_byte_idx = 0;
	conv_weird_byte_get_ptr = conv_weird_byte_buf_ptr;
}
PRIVATE short conv_weird_byte_get()
{
	short chr = '\0';
	if (conv_weird_byte_remaining_bytes == 0) {
		chr = (UCHAR)*conv_weird_byte_get_ptr;
		if (chr) {
			// this is not the last byte
			conv_weird_byte_get_ptr++;
		}
	}
	return chr;
}

//==============================================================================

PRIVATE char conv_utf8_to_bin_mode = 0;		// 1: ASCII8
void conv_utf8_to_bin_init(char ascii8)
{
	conv_utf8_to_bin_mode = ascii8;
}
int conv_utf8_to_bin(const char *line, char *out_buf)
{
	size_t len = line_data_strlen(line);
	int out_off = 0;
	for (int in_off = 0; in_off < len; in_off += utf8c_bytes(&line[in_off])) {
		if (strlcmp__(&line[in_off], conv_bin_to_utf8_table_00) == 0) {
			out_buf[out_off++] = 0x00;
			continue;
		}
		if (conv_utf8_to_bin_mode) {
			short chr;
			for (chr = 0x80; chr < 0x100; chr++) {
				if (strlcmp__(&line[in_off], conv_bin_to_utf8_table_80_ff[chr - 0x80]) == 0) {
					out_buf[out_off++] = chr;
					break;
				}
			}
			if (chr < 0x100)
				continue;
		}
		// no conversion
		int bytes = utf8c_bytes(&line[in_off]);
		strlcpy__(&out_buf[out_off], &line[in_off], bytes);
		out_off += bytes;
	}
	out_buf[out_off] = '\0';
	return out_off;
}

//=================================================================================================

struct bin_range {
	short begin;
	short end;
	wchar_t base_code;
} bin_range_table[] = {
	{ 0x00, 0x01,  0xd800 },
	{ 0x01, 0x20,  0x0001 },
	{ 0x7f, 0x80,  0x007f },
	{ 0x80, 0xa0,  0x0080 },
};

#define NUM_OF_BIN_CODE		(1 + 0x1f + 1 + 0x20)	// = 65
char internal_to_display_conv_initialized = 0;
char i_to_d_conv_table_from[NUM_OF_BIN_CODE][MAX_UTF8C_BYTES + 1];
const char *i_to_d_conv_table_to[] = {
 // 00_1f
 "␀", "␁", "␂", "␃", "␄", "␅", "␆", "␇", "␈", "␉", "␊", "␋", "␌", "␍", "␎", "␏",
 "␐", "␑", "␒", "␓", "␔", "␕", "␖", "␗", "␘", "␙", "␚", "␛", "␜", "␝", "␞", "␟",
 // 7f
 "␡",
 // 80_9f
 "⢀", "⢁", "⢂", "⢃", "⢄", "⢅", "⢆", "⢇", "⢈", "⢉", "⢊", "⢋", "⢌", "⢍", "⢎", "⢏",
 "⢐", "⢑", "⢒", "⢓", "⢔", "⢕", "⢖", "⢗", "⢘", "⢙", "⢚", "⢛", "⢜", "⢝", "⢞", "⢟",
};

void internal_to_display_conv_init()
{
	if (internal_to_display_conv_initialized)
		return;

	int tbl_idx = 0;
	for (int range_idx = 0;
	 range_idx < (sizeof(bin_range_table) / sizeof(bin_range_table[0]));
	 range_idx++) {
		for (short chr = bin_range_table[range_idx].begin;
		 chr < bin_range_table[range_idx].end; chr++) {
			char utf8c_buf[MAX_UTF8C_BYTES + 1];
			my_wctomb(utf8c_buf, bin_range_table[range_idx].base_code
			 + chr - bin_range_table[range_idx].begin);
			strlcpy__(i_to_d_conv_table_from[tbl_idx++], utf8c_buf, MAX_UTF8C_BYTES);
		}
	}

	if (tbl_idx != NUM_OF_BIN_CODE) {
		progerr_printf("incorrect table size of i_to_d_conv_table_from\n");
	}
	internal_to_display_conv_initialized = 1;
}

const char *internal_to_display_conv(const char *utf8c)
{
	for (int tbl_idx = 0; tbl_idx < NUM_OF_BIN_CODE; tbl_idx++) {
		if (strlcmp__(utf8c, i_to_d_conv_table_from[tbl_idx]) == 0) {
			return i_to_d_conv_table_to[tbl_idx];
		}
	}
	return utf8c;
}

// End of bincode.c
