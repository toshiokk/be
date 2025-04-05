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

extern search_t search__;
extern matches_t matches__;

int doe_search_backward_first(void);
int doe_search_forward_first(void);
int doe_search_backward_next(void);
int doe_search_forward_next(void);

int doe_replace(void);

#ifdef ENABLE_REGEX

int doe_find_bracket(void);
int doe_find_bracket_reverse(void);
int doe_highlight_bracket(void);
int doe_highlight_bracket_reverse(void);
#define BRACKET_SEARCH_REGEXP_STR_LEN	4	// "[<>]"
#define MAX_BRACKET_NESTINGS	1000
#define MAX_BRACKETS_SEARCH		10000	// for avoiding infinite loop
int setup_bracket_search(char char_under_cursor, int reverse_pair, char *needle);
int search_bracket_in_buffer(be_line_t **ptr_line, int *ptr_byte_idx,
 char char_under_cursor, const char *needle, int search_dir, int skip_here, char depth_increase,
 int *ptr_depth, int *prev_depth);

int prepare_colors_for_bracket_hl();
int get_colors_for_bracket_hl();
void set_color_for_bracket_hl(char depth_increase, UINT8 *zero_occurances, int depth);
void get_color_for_bracket_hl(int color_idx, char *bgc, char *fgc);

#endif // ENABLE_REGEX

int search_string_once(const char *needle, int search_count);
int replace_string_loop(const char *replace_from, const char *replace_to, int *num_replaced_);
int replace_str_in_buffer(search_t *search, matches_t *matches, const char *replace_to);

void disp_status_bar_not_found_msg(const char *str, int search_count);

#endif // search_h

// End of search.h
