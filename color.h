/**************************************************************************
 *   color.h                                                              *
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

#ifndef color_h
#define color_h

typedef enum {
	ITEM_COLOR_IDX_DEFAULT,				// default
	ITEM_COLOR_IDX_TITLE,				// title bar
	ITEM_COLOR_IDX_STATUS,				// status bar
	ITEM_COLOR_IDX_KEY_LIST,			// key list(key)
	ITEM_COLOR_IDX_KEY_LIST2,			// key list(Description)
	ITEM_COLOR_IDX_TEXT_NORMAL,			// contents
	ITEM_COLOR_IDX_TEXT_SELECTED,		// contents selected
	ITEM_COLOR_IDX_TEXT_SELECTED2,		// contents selected-2
	ITEM_COLOR_IDX_TEXT_SELECTED3,		// contents selected-3
	ITEM_COLOR_IDX_WARNING,				// warning
	ITEM_COLOR_IDX_WARNING2,			// warning-2
	ITEM_COLOR_IDX_WARNING3,			// warning-3
	ITEM_COLOR_IDX_WARNING4,			// warning-4
	ITEM_COLOR_IDX_CURSOR_CHAR,			// cursor
	ITEM_COLOR_IDX_CURSOR_LINE,			// cursor line
	ITEM_COLOR_IDX_LINE_NUMBER,			// line number / ruler
	ITEM_COLOR_IDX_MENU_FRAME,			// menu - frame
	ITEM_COLOR_IDX_MENU_ITEM,			// menu - item
	ITEM_COLOR_IDX_MENU_SELECTED,		// menu - selected item
	ITEM_COLOR_IDX_INPUT,				// input text
	MAX_ITEM_COLORS,
} item_color_idx_t;

//-----------------------------------------------------------------------------

typedef struct /*item_color*/ {
	char bgc;			// bg color
	char fgc;			// fg color
#ifdef ENABLE_DEBUG
#define S(str)		#str,
	char *item_name;	// "ITEM_COLOR_IDX_DEFAULT"
#else // ENABLE_DEBUG
#define S(str)
#endif // ENABLE_DEBUG
} item_color_t;

extern item_color_t item_colors[MAX_ITEM_COLORS];

#ifdef ENABLE_SYNTAX
typedef struct color_syntax_t {
	struct color_syntax_t *next;
	item_color_t color;
	regexp_t *regexp_start;		// Start regexp string
	regexp_t *regexp_end;		// End regexp string
} color_syntax_t;

typedef struct file_type_t {
	struct file_type_t *next;		// chain to the next entry, NULL for the last entry
	char *desc;						// Name of this file type
	regexp_t *regexp;				// regexp to be applied to file name
	color_syntax_t *color_syntax;	// color syntax chain
	char tab_size;					// tab size for this file type
} file_type_t;

extern file_type_t *file_types_head;
extern file_type_t *cur_file_type;
extern file_type_t *default_color_syntax;
#endif // ENABLE_SYNTAX

void init_default_app_color(void);
void set_work_space_color_on_app_list_mode();

void set_work_space_color_dark(void);
void clear_work_space_color_dark(void);
int is_work_space_color_dark(void);

void set_work_space_color_warn(void);
int is_work_space_color_warn(void);

void register_item_color(item_color_idx_t color_idx, int bgc, int fgc);
void get_color_by_idx(item_color_idx_t color_idx, char *fgc, char *bgc);
void set_color_by_idx(item_color_idx_t color_idx, int reverse);
void set_item_color(const item_color_t *item_color);

#ifdef ENABLE_SYNTAX
int set_file_type_by_cur_file_path(void);
int set_file_type_by_file_name(const char *file_path);
const color_syntax_t *get_color_syntax_head(void);
const color_syntax_t *get_default_color_syntax_head(void);
#endif // ENABLE_SYNTAX

int display_color_pairs(int yy, int xx);

#ifdef ENABLE_DEBUG
int display_item_colors(int yy, int xx);
#ifdef ENABLE_REGEX
int display_bracket_hl_colors(int yy, int xx);
#endif // ENABLE_REGEX
#endif // ENABLE_DEBUG

#endif // color_h

// End of color.h
