/**************************************************************************
 *   color.c                                                              *
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

#include "applvl.h"

// Color table for the various items
item_color_t item_colors[MAX_ITEM_COLORS];

//DDDD#ifndef ENABLE_HIGH_BGC
//DDDD#define CL_BG	CL_GY	// gray background color
//DDDD#else // ENABLE_HIGH_BGC
//DDDD#define CL_BG	CL_WH	// white background color
//DDDD#endif // ENABLE_HIGH_BGC

item_color_t default_item_colors[MAX_ITEM_COLORS] = {
	//  bgc, fgc
	{ CL_BK, CL_WH, S(ITEM_COLOR_IDX_DEFAULT)			},
#if APP_REL_LVL == APP_REL_LVL_STABLE
	// official release
	{ CL_LB, CL_WH, S(ITEM_COLOR_IDX_TITLE)				},
	{ CL_LB, CL_WH, S(ITEM_COLOR_IDX_STATUS)			},
	{ CL_GR, CL_BK, S(ITEM_COLOR_IDX_PARENT)			},
#endif
#if APP_REL_LVL == APP_REL_LVL_TEST
	// test release-1
	{ CL_CY, CL_WH, S(ITEM_COLOR_IDX_TITLE)				},
	{ CL_CY, CL_WH, S(ITEM_COLOR_IDX_STATUS)			},
	{ CL_LB, CL_BK, S(ITEM_COLOR_IDX_PARENT)			},
#endif
#if APP_REL_LVL == APP_REL_LVL_EXPERIMENTAL
	// experimental release
	{ CL_GR, CL_WH, S(ITEM_COLOR_IDX_TITLE)				},
	{ CL_GR, CL_WH, S(ITEM_COLOR_IDX_STATUS)			},
	{ CL_LB, CL_BK, S(ITEM_COLOR_IDX_PARENT)			},
#endif
	{ CL_WH, CL_BL, S(ITEM_COLOR_IDX_KEY_LIST)			},
	{ CL_BK, CL_BR, S(ITEM_COLOR_IDX_KEY_LIST2)			},
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_TEXT_NORMAL)		},
	{ CL_LC, CL_BK, S(ITEM_COLOR_IDX_TEXT_NORMAL2)		},
	{ CL_LR, CL_WH, S(ITEM_COLOR_IDX_TEXT_SELECTED1)	},
	{ CL_LG, CL_WH, S(ITEM_COLOR_IDX_TEXT_SELECTED2)	},
	{ CL_BR, CL_WH, S(ITEM_COLOR_IDX_TEXT_SELECTED3)	},
	{ CL_DF, CL_LR, S(ITEM_COLOR_IDX_WARNING1)			},	// high
	{ CL_DF, CL_LM, S(ITEM_COLOR_IDX_WARNING2)			},	// medium
	{ CL_DF, CL_YL, S(ITEM_COLOR_IDX_WARNING3)			},	// low
	{ CL_LR, CL_YL, S(ITEM_COLOR_IDX_ERROR)				},
	{ CL_BK, CL_WH, S(ITEM_COLOR_IDX_CURSOR_CHAR)		},
	{ CL_LC, CL_DF, S(ITEM_COLOR_IDX_CURSOR_LINE)		},
	{ CL_YL, CL_BK, S(ITEM_COLOR_IDX_LINE_NUMBER)		},
	{ CL_LC, CL_BL, S(ITEM_COLOR_IDX_MENU_FRAME)		},
	{ CL_LB, CL_CY, S(ITEM_COLOR_IDX_MENU_ITEM)			},
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_MENU_SELECTED)		},
	{ CL_WH, CL_BK, S(ITEM_COLOR_IDX_INPUT)				},
};

// Initialize with default application color set.
void init_default_app_color(void)
{
	memcpy__(item_colors, default_item_colors, sizeof(default_item_colors));
}

void set_work_space_color_on_app_list_mode()
{
	if (is_app_normal_mode()) {
		clear_work_space_color_dark();
	} else {
		set_work_space_color_dark();
	}
}

PRIVATE char work_space_color_dark = 0;
void set_work_space_color_dark(void)
{
	work_space_color_dark = 1;
}
void clear_work_space_color_dark(void)
{
	work_space_color_dark = 0;
}
int is_work_space_color_dark(void)
{
	return work_space_color_dark;
}

PRIVATE char work_space_color_warn = 0;
void set_work_space_color_warn(void)
{
	work_space_color_warn = 1;
}
int is_work_space_color_warn(void)
{
	return work_space_color_warn;
}

//------------------------------------------------------------------------------

// register item color
void register_item_color(item_color_idx_t color_idx, int bgc, int fgc)
{
flf_d_printf("(%d bgc:%d, fgc:%d)\n", color_idx, bgc, fgc);
	if (0 <= color_idx && color_idx < MAX_ITEM_COLORS) {
		item_colors[color_idx].bgc = bgc;
		item_colors[color_idx].fgc = fgc;
	}
}

// get item color
void get_color_by_idx(item_color_idx_t color_idx, char *fgc, char *bgc)
{
	if (0 <= color_idx && color_idx < MAX_ITEM_COLORS) {
		*bgc = item_colors[color_idx].bgc;
		*fgc = item_colors[color_idx].fgc;
	}
}
// set current color by item_idx
void set_color_by_idx(item_color_idx_t color_idx, int reverse)
{
	if (0 <= color_idx && color_idx < MAX_ITEM_COLORS) {
		if (color_idx == ITEM_COLOR_IDX_TEXT_NORMAL) {
			if (is_work_space_color_warn()) {
				color_idx = ITEM_COLOR_IDX_TEXT_SELECTED2;
			} else if (is_work_space_color_dark()) {
				color_idx = ITEM_COLOR_IDX_DEFAULT;
			}
		}
		if (item_colors[color_idx].bgc == CL_DF) {
			set_item_color(&item_colors[ITEM_COLOR_IDX_STATUS], reverse);
		} else if (item_colors[color_idx].fgc == CL_DF) {
			set_item_color(&item_colors[ITEM_COLOR_IDX_TEXT_NORMAL], reverse);
		}
		set_item_color(&item_colors[color_idx], reverse);
	}
}
void set_item_color(const item_color_t *item_color, int reverse)
{
	tio_set_attrs(item_color->bgc, item_color->fgc, reverse);
}

//------------------------------------------------------------------------------

#ifdef ENABLE_SYNTAX

file_type_t *file_types_head = NULL;		//!< head of file types
file_type_t *cur_file_type = NULL;			//!< current file type
file_type_t *default_color_syntax = NULL;	//!< default color syntax

// Select color set by current file name
int set_file_type_and_tab_size_by_cur_file_path(void)
{
	int ret;

	if ((ret = set_file_type_by_cur_file_path()) != 0) {
		GET_CUR_EBUF_STATE(buf_TAB_SIZE) = cur_file_type->tab_size;
	}
	return ret;
}
int set_file_type_by_cur_file_path(void)
{
	return set_file_type_by_file_name(buf_get_file_path(get_epc_buf(), NULL));
}
int set_file_type_by_file_name(const char *file_path)
{
	file_type_t *file_type;
	file_type_t *last_file_type;
	regexp_matches_t regexp_matches;

	cur_file_type = NULL;
	for (file_type = last_file_type = file_types_head; file_type != NULL;
	 last_file_type = file_type, file_type = NODE_NEXT(file_type)) {
		// set cur_file_type if match the file name regexp
		if (regexp_search_compiled(file_type->regexp, file_path, 0,
		 REG_NONE, &regexp_matches, 1) == 0) {
			cur_file_type = file_type;
			break;
		}
	}
	// if we haven't found a match, use the last(default) color set
	if (cur_file_type == NULL) {
		cur_file_type = last_file_type;
		return 0;
	}
	return 1;
}

// get the beginning of the currently selected color set
const color_syntax_t *get_color_syntax_head(void)
{
	if (cur_file_type)
		return cur_file_type->color_syntax;	// top of the color set
	return NULL;
}
const color_syntax_t *get_default_color_syntax_head(void)
{
	if (default_color_syntax)
		return default_color_syntax->color_syntax;	// top of the color set
	return NULL;
}

#endif // ENABLE_SYNTAX

//------------------------------------------------------------------------------

PRIVATE void display_color_pattern(int yy, int xx, int reverse);
int display_color_pairs(int yy, int xx)
{
	display_color_pattern(yy, xx+ 0, 0);
	display_color_pattern(yy, xx+34, 1);
	tio_refresh();
	return 0;
}
// display color pattern to the specified screen position
PRIVATE void display_color_pattern(int yy, int xx, int reverse)
{
	int bgc;
	int fgc;
#define BUF_LEN		100
	char buf[BUF_LEN+1];
	char buffer[BUF_LEN+1];

#define X_OFF		0
							//01234567890123456789
	snprintf(buf, BUF_LEN+1, "COLOR_PAIRS: %d, REV: %d", COLOR_PAIRS, reverse);
	set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	snprintf(buffer, BUF_LEN+1, "%-*s", BUF_LEN, buf);
	tio_output_string(central_win_get_mid_win_y() + yy + 0, xx + 0, buffer, -1);

	for (bgc = 0; bgc < COLORS16; bgc++) {
		for (fgc = 0; fgc < COLORS16; fgc++) {
			tio_set_attrs(bgc, fgc, reverse);
			snprintf(buf, BUF_LEN+1, "%X%X", bgc, fgc);
			tio_output_string(central_win_get_mid_win_y() + yy + 1 + bgc,
			 xx + X_OFF + fgc * 2, buf, -1);
		}
	}
}

#ifdef ENABLE_DEBUG
int display_item_colors(int yy, int xx)
{
	int item_idx;
	char buffer[MAX_PATH_LEN+1];

	for (item_idx = 0; item_idx < MAX_ITEM_COLORS; item_idx++) {
		set_color_by_idx(item_idx, 0);
		snprintf(buffer, MAX_PATH_LEN, "%2d: %-40s",
		 item_idx, default_item_colors[item_idx].item_name);
		tio_output_string(central_win_get_mid_win_y() + yy + item_idx, xx + 0, buffer, -1);
	}
	tio_refresh();
	return 0;
}
#ifdef ENABLE_REGEX
int display_bracket_hl_colors(int yy, int xx)
{
	UINT8 zero_occurances = 0;

	prepare_colors_for_bracket_hl();
	for (int depth = 0; depth < get_colors_for_bracket_hl(); depth++) {
		char buffer[MAX_PATH_LEN+1];
		set_color_for_bracket_hl(+1, &zero_occurances, depth);
		snprintf(buffer, MAX_PATH_LEN, "%3d: ([{<>}]) ", depth);
		tio_output_string(central_win_get_mid_win_y() + yy + depth, xx + 0, buffer, -1);
		set_color_for_bracket_hl(-1, &zero_occurances, -depth);
		snprintf(buffer, MAX_PATH_LEN, "%3d: ([{<>}]) ", -depth);
		tio_output_string(central_win_get_mid_win_y() + yy + depth, xx + 20, buffer, -1);
	}
	tio_refresh();
	return 0;
}
#endif // ENABLE_REGEX
#endif // ENABLE_DEBUG

// End of color.c
