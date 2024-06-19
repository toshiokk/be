/**************************************************************************
 *   winin.h                                                              *
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

#ifndef winin_h
#define winin_h

// | command modifier key | replace/append string       | return value  |
// |----------------------|-----------------------------|---------------|
// | none                 | replace input file/dir name | REPLACE_STR_1 |
// | ALT                  | append input file/dir name  | APPEND_STR_2  |
#define REPLACE_STR_1	1	// input file/dir name
#define APPEND_STR_2	2	// Append input file/dir name

int input_string_tail(const char *default__, char *input_buf,
 int hist_type_idx, const char *msg, ...);
int input_string_pos(const char *default__, char *input_buf, int cursor_byte_idx,
 int hist_type_idx, const char *msg, ...);

#define ASK_ALL			0x80
#define ASK_REDO		0x40
#define ASK_UNDO		0x20
#define ASK_END			0x10
#define ASK_BACKWARD	0x08
#define ASK_FORWARD		0x04
#define ASK_YES			0x02
#define ASK_NO			0x01
#define ASK_YES_NO		(ASK_YES | ASK_NO)

#define ANSWER_FORCE	9	// force save even if not-modified
#define ANSWER_ALL		8	// save or replace all
#define ANSWER_REDO		5	// Redo replace
#define ANSWER_UNDO		4	// Undo replace
#define ANSWER_FORWARD	3	// search Forward
#define ANSWER_BACKWARD	2	// search Backward
#define ANSWER_YES		1	// Yes (save or replace)
#define ANSWER_NO		0	// No (save or replace)
#define ANSWER_CANCEL	-1	// cancel replace and not-return to beginning pos
#define ANSWER_END		-2	// cancel replace and return to beginning pos
#define ANSWER_NONE		-9
int ask_yes_no(int flags, const char *msg, ...);

void disp_key_list(char *key_lists[]);
void display_reverse_text(int yy, char *text);

#endif // winin_h

// End of winin.h
