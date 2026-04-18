/**************************************************************************
 *   search.h                                                             *
 *                                                                        *
 *   Copyright (C) 2000-2003 Chris Allegretta                             *
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

#ifndef search_h
#define search_h

void set_last_searched_needle(const char *needle);
const char *get_last_searched_needle();

extern search_t search__;
extern matches_t matches__;

void doe_search_first_backward();
void doe_search_first_forward();
void doe_search_next_backward();
void doe_search_next_forward();

void doe_replace();

#ifdef ENABLE_REGEX

void doe_find_bracket();
void doe_find_bracket_reverse();
void doe_find_brackets();
void doe_find_brackets_reverse();
#define MULTI_CHAR_BRACKET
#ifndef MULTI_CHAR_BRACKET
#define BRACKET_SEARCH_REGEXP_STR_LEN	(1+2+2+1)		// "[\<\>]"
#else // MULTI_CHAR_BRACKET
#define MAX_NUM_OF_BRACKET_CHAR			4				// "{{{{" / "}}}}"
#define BRACKET_SEARCH_REGEXP_STR_LEN	\
  (1 + 2*MAX_NUM_OF_BRACKET_CHAR + 1 + 2*MAX_NUM_OF_BRACKET_CHAR + 1)	// "(\{\{\{\{|\}\}\}\})"
#endif // MULTI_CHAR_BRACKET

#define MAX_BRACKET_NESTINGS	1000
#define MAX_BRACKETS_SEARCH		10000	// for avoiding infinite loop
int setup_bracket_search(int single1_multi2, char *str, int off, int rev_pairing, char *needle);
int search_bracket_within_buffer(be_line_t **ptr_line, int *ptr_byte_idx, char char_under_cursor,
 const char *needle, int search_dir, int skip_chars, char depth_increase,
 int *ptr_depth, int *prev_depth);

void prepare_colors_for_bracket_hl();
int get_colors_for_bracket_hl();
int get_color_idx_for_bracket_hl(char depth_increase, UINT8 *depth_0_occurances, int depth);
void set_color_for_bracket_hl(char depth_increase, UINT8 *depth_0_occurances, int depth);
void set_color_for_bracket_hl_by_idx(int color_idx);

#endif // ENABLE_REGEX

#endif // search_h

// End of search.h
