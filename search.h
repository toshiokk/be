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

extern char last_searched_needle[MAX_PATH_LEN+1];

extern search_t search__;
extern matches_t matches__;

int do_search_backward(void);
int do_search_forward(void);
int do_search_next_backward(void);
int do_search_next_forward(void);

int do_replace(void);

#ifdef ENABLE_REGEX
int do_find_bracket(void);
int do_find_bracket_reverse(void);
#endif // ENABLE_REGEX

int search_string_once(const char *string_find);
int replace_string_loop(const char *replace_from, const char *replace_to, int *num_replaced_);
int replace_str_in_buffer(search_t *search, matches_t *matches, const char *replace_to);

int search_str_in_buffer(const char *needle,
 int search_dir, int ignore_case, int skip_here, int global_search);

void not_found_msg(const char *str);

#endif // search_h

// End of search.h
