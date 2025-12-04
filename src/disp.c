/**************************************************************************
 *   disp.c                                                               *
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

void set_title_bar_color_by_state(int color_idx, char invert)
{
	if (geteuid() == 0) {
		// If you are super user, you see strange color on title bar.
		set_item_color_by_idx(ITEM_COLOR_IDX_ERROR, invert);
	} else {
		// Normal color
		set_item_color_by_idx(color_idx, invert);
	}
}

const char *root_notation()
{
	static char notation[MAX_PATH_LEN+1];
	snprintf_(notation, MAX_PATH_LEN, "%s%s",
	 (geteuid() == 0) ? "[ROOT] " : "",
	 is_app_chooser_viewer_mode() ? get_str_app_mode() : "");
	return notation;
}

//------------------------------------------------------------------------------

PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 const char *msg, va_list ap);
PRIVATE int get_status_bar_color_idx(char status_bar_display);

void clear_status_bar_displayed()
{
	app_stack_entry *app_stk_ptr = get_app_stack_ptr(-1);
	app_stk_ptr->status_bar_displayed = S_B_NONE;
}

void disp_status_bar_type(s_b_d_t status_bar_to_display, const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	disp_status_bar_percent_va(status_bar_to_display, msg, ap);
	va_end(ap);
}

// Status bar overwrite policy:
// | --      |                       previous display                                |
// |next req.|S_B_NONE|S_B_AUTO|S_B_CURS|S_B_ING |S_B_WARN|S_B_ERR |S_B_DONE|S_B_ASYN|
// |---------|--------|--------|--------|--------|--------|--------|--------|--------|
// |S_B_NONE | --     | --     | --     | --     | --     | --     | --     | --     |
// |S_B_AUTO | over-W | over-W | over-W | over-W | reject | reject | reject | reject |
// |S_B_CURS | over-W | over-W | over-W | over-W | over-W | over-W | over-W | over-W |
// |S_B_ING  | over-W | over-W | over-W | over-W | reject | reject | over-W | over-W |
// |S_B_WARN | over-W | over-W | over-W | over-W | over-W | reject | over-W | over-W |
// |S_B_ERR  | over-W | over-W | over-W | over-W | over-W | over-W | over-W | over-W |
// |S_B_DONE | over-W | over-W | over-W | over-W |prv:nxt |prv:nxt | over-W | over-W |
// |S_B_ASYN | over-W | over-W | over-W | over-W | over-W | over-W | over-W | over-W |
PRIVATE char sb_overwrite_policy_table[S_B_TYPES][S_B_TYPES] = {
//             |S_B_NONE|S_B_AUTO|S_B_CURS|S_B_ING |S_B_WARN|S_B_ERR |S_B_DONE|S_B_ASYN|
/* S_B_NONE */ {    1   ,    1   ,    1   ,    1   ,    1   ,    1   ,    1   ,    1   },
/* S_B_AUTO */ {    1   ,    1   ,    1   ,    1   ,    0   ,    0   ,    0   ,    0   },
/* S_B_CURS */ {    1   ,    1   ,    1   ,    1   ,    0   ,    0   ,    0   ,    1   },
/* S_B_ING  */ {    1   ,    1   ,    1   ,    1   ,    0   ,    0   ,    1   ,    1   },
/* S_B_WARN */ {    1   ,    1   ,    1   ,    1   ,    1   ,    0   ,    1   ,    1   },
/* S_B_ERR  */ {    1   ,    1   ,    1   ,    1   ,    1   ,    1   ,    1   ,    1   },
/* S_B_DONE */ {    1   ,    1   ,    1   ,    1   ,    2   ,    2   ,    1   ,    1   },
/* S_B_ASYN */ {    1   ,    1   ,    1   ,    1   ,    1   ,    1   ,    1   ,    1   },
};
// Note: 'prv:nxt' uses previous color
// over-W: over-write

// Examples:
//  Reading File filename.ext ...
//  Writing File filename.ext ...
//  "abc" not found  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CHR:2f ENC:ASCII EOL:LF(NIX)
//  12 files loaded  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CHR:2f ENC:ASCII EOL:LF(NIX)
PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 const char *msg, va_list ap)
{
	app_stack_entry *app_stk_ptr = get_app_stack_ptr(-1);
	int dividend = 1;
	int divisor = 1;

hmflf_dprintf("[%d](%d?%d)[%s]\n", get_app_stack_depth(),
 app_stk_ptr->status_bar_displayed, status_bar_to_display, msg);
#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == EF_EDITOR) {
#endif // ENABLE_FILER
		dividend = EPCBVC_CL->line_num-1;
		divisor = get_epc_buf()->buf_lines-1;
#ifdef ENABLE_FILER
	} else {
		dividend = get_cur_fv_file_idx();
		divisor = get_cur_fv_file_info_entries();
	}
#endif // ENABLE_FILER

	char update = sb_overwrite_policy_table
		[MIN_MAX_(S_B_NONE, status_bar_to_display, S_B_ASYN)]
		[MIN_MAX_(S_B_NONE, app_stk_ptr->status_bar_displayed, S_B_ASYN)]
	;
	int color_idx = ITEM_COLOR_IDX_STATUS;
	switch (update) {
	default:
		break;
	case 1:
		color_idx = get_status_bar_color_idx(status_bar_to_display);
		break;
	case 2:
		color_idx = get_status_bar_color_idx(app_stk_ptr->status_bar_displayed);
		break;
	}

flf_dprintf("update: %d\n", update);
	if (update) {
		char buf[MAX_SCRN_LINE_BUF_LEN+1];
		char buffer[MAX_SCRN_LINE_BUF_LEN+1] = "";
		vsnprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
		switch (update) {
		default:
			break;
		case 1:
			// this time: "NEXT"
			strlcpy__(buffer, buf, MAX_SCRN_LINE_BUF_LEN);
			strlcpy__(app_stk_ptr->status_bar_prev_msg, buffer, MAX_SCRN_LINE_BUF_LEN);
			// next time: "NEXT"
			break;
		case 2:
			// this time: "PREV | NEXT"
			if (is_strlen_not_0(app_stk_ptr->status_bar_prev_msg)) {
				strlcpy__(buffer, app_stk_ptr->status_bar_prev_msg, MAX_SCRN_LINE_BUF_LEN);
				strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, "  |  ");
			}
			strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, buf);
			strlcpy__(app_stk_ptr->status_bar_prev_msg, buffer, MAX_SCRN_LINE_BUF_LEN);
			// next time: "NEXT"
			break;
		}
		adjust_str_columns(buffer, central_win_get_columns());
		int col_idx = -1;
		if (divisor > 0) {
			// display percent indicator
			col_idx = MIN_MAX_(0, (central_win_get_columns() - 1) * dividend / divisor,
			 central_win_get_columns() - 1);
		}
		app_stk_ptr->status_bar_color_idx = color_idx;
		app_stk_ptr->status_bar_col_idx = col_idx;
		strlcpy__(app_stk_ptr->status_bar_msg, buffer, MAX_SCRN_LINE_BUF_LEN);
		app_stk_ptr->status_bar_displayed = status_bar_to_display;
hm_dprintf("_SB_[%d](%d)[%s]\n", get_app_stack_depth(),
 status_bar_to_display, app_stk_ptr->status_bar_msg);
	}
	redisp_status_bar();
}
PRIVATE int get_status_bar_color_idx(char status_bar_display)
{
	int color_idx = ITEM_COLOR_IDX_STATUS;
	switch (status_bar_display) {
	default:
	case S_B_NONE:
	case S_B_AUTO:
	case S_B_CURS:
	case S_B_ING:
		break;
	case S_B_WARN:
		color_idx = ITEM_COLOR_IDX_WARNING3;
		break;
	case S_B_ERR:
		color_idx = ITEM_COLOR_IDX_ERROR;
		break;
	case S_B_DONE:
	case S_B_ASYN:
		break;
	}
	return color_idx;
}
void redisp_status_bar()
{
	app_stack_entry *app_stk_ptr = get_app_stack_ptr(-1);
	int color_idx = app_stk_ptr->status_bar_color_idx;
	int col_idx = app_stk_ptr->status_bar_col_idx;
	const char *msg = app_stk_ptr->status_bar_msg;

	set_item_color_by_idx(color_idx, 0);
	// display status bar
flf_dprintf("[%d] y:%d, [%s]\n", get_app_stack_depth(), central_win_get_status_line_y(), msg);
	central_win_output_string(central_win_get_status_line_y(), 0, msg, -1);
	if (col_idx >= 0) {
		int col_idx_1, col_idx_2;
		int byte_idx_1, byte_idx_2;
		// col_idx: 0 -- central_win_get_columns()-1
		byte_idx_1 = byte_idx_from_col_idx(msg, col_idx, CHAR_LEFT,  &col_idx_1);
		byte_idx_2 = byte_idx_from_col_idx(msg, col_idx+1, CHAR_RIGHT, &col_idx_2);
		// display percent indicator in reverse video
		set_item_color_by_idx(color_idx, 1);
		central_win_output_string(central_win_get_status_line_y(), col_idx_1,
		 &msg[byte_idx_1], byte_idx_2 - byte_idx_1);
	}
}

void blank_status_bar()
{
	central_win_clear_lines(central_win_get_status_line_y(), -1);
}

void blank_key_list_lines()
{
	set_item_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	central_win_clear_lines(central_win_get_key_list_line_y(),
	 central_win_get_key_list_line_y() + get_key_list_lines());
}

void disp_status_bar_cwd()
{
	disp_status_bar_done(_("CWD: %s"), get_full_path_of_cur_dir(NULL));
}

// End of disp.c
