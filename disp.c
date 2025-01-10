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
		set_color_by_idx(ITEM_COLOR_IDX_ERROR, invert);
	} else {
		// Normal color
		set_color_by_idx(color_idx, invert);
	}
}

const char *root_notation(void)
{
	static char notation[MAX_PATH_LEN+1];
	snprintf_(notation, MAX_PATH_LEN, "%s%s",
	 (geteuid() == 0) ? "[ROOT] " : "",
	 is_app_view_mode() ? "[VW]" : (is_app_chooser_mode() ? "[LIST]" : ""));
	return notation;
}

//------------------------------------------------------------------------------

PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 const char *msg, va_list ap);

void disp_status_bar_cursor(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_CURS, msg, ap);
	va_end(ap);
}
void disp_status_bar_ing(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_ING, msg, ap);
	va_end(ap);
	tio_refresh();	// update screen soon
}
void disp_status_bar_warn(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_WARN, msg, ap);
	va_end(ap);
	tio_beep();
}
void disp_status_bar_err(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_ERR, msg, ap);
	va_end(ap);
	tio_beep();
}
void disp_status_bar_done(const char *msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_DONE, msg, ap);
	va_end(ap);
}

// Examples:
//  Reading File filename.ext ...
//  Writing File filename.ext ...
//  "abc" not found  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CODE:2f ENC:ASCII EOL:LF(NIX)
//  2 files loaded  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CODE:2f ENC:ASCII EOL:LF(NIX)
PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 const char *msg, va_list ap)
{
	app_win_stack_entry *app_win = get_app_win_stack_entry(-1);
	int dividend = 1;
	int divisor = 1;
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char buffer[MAX_SCRN_LINE_BUF_LEN+1] = "";

#ifdef ENABLE_FILER
	if (GET_APPMD(app_EDITOR_FILER) == 0) {
#endif // ENABLE_FILER
		dividend = EPCBVC_CL->line_num-1;
		divisor = get_epc_buf()->buf_lines-1;
#ifdef ENABLE_FILER
	} else {
		dividend = get_cur_fv_file_idx();
		divisor = get_cur_filer_cur_pane_view()->file_list_entries;
	}
#endif // ENABLE_FILER

/////mflf_d_printf("status_bar_displayed: %d, status_bar_to_display: %d\n",
///// app_win->status_bar_displayed, status_bar_to_display);
	char color_idx = ITEM_COLOR_IDX_STATUS;
	char update = 0;		// reject
	switch (app_win->status_bar_displayed) {
	default:
	case S_B_D_NONE:
	case S_B_D_CURS:
	case S_B_D_ING:
		update = 1;			// overlap
		break;
	case S_B_D_WARN:
	case S_B_D_ERR:
	case S_B_D_DONE:
		switch (status_bar_to_display) {
		default:
		case S_B_D_ING:
			// reject update display
			break;
		case S_B_D_CURS:
			// preserve the previous color
			update = 2;		// "PREV : NEXT"
			break;
		case S_B_D_WARN:
		case S_B_D_ERR:
		case S_B_D_DONE:
			update = 1;		// "NEXT" (overlap)
			break;
		}
		break;
	}
	switch (status_bar_to_display) {
	default:
	case S_B_D_ING:
		break;
	case S_B_D_CURS:
		color_idx = app_win->status_bar_color_idx;
		break;
	case S_B_D_WARN:
		color_idx = ITEM_COLOR_IDX_WARNING3;
		break;
	case S_B_D_ERR:
		color_idx = ITEM_COLOR_IDX_ERROR;
		break;
	case S_B_D_DONE:
		break;
	}
/////mflf_d_printf("sb_displayed: %d, sb_to_display: %d, update: %d, color_idx: %d\n",
///// app_win->status_bar_displayed, status_bar_to_display, update, color_idx);

	if (update) {
		vsnprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
		strcpy__(buffer, "");
		switch (update) {
		default:
			break;
		case 1:
			// this time: "NEXT"
			strlcpy__(buffer, buf, MAX_SCRN_LINE_BUF_LEN);
			strlcpy__(app_win->status_bar_prev_msg, buffer, MAX_SCRN_LINE_BUF_LEN);
			// next time: "NEXT"
			break;
		case 2:
			// this time: "PREV | NEXT"
			if (is_strlen_not_0(app_win->status_bar_prev_msg)) {
				strlcpy__(buffer, app_win->status_bar_prev_msg, MAX_SCRN_LINE_BUF_LEN);
				strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, "  |  ");
			}
			strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, buf);
			strcpy__(app_win->status_bar_prev_msg, "");
			// next time: "NEXT"
			break;
		}
		adjust_utf8s_columns(buffer, main_win_get_columns());
		int col_idx = -1;
		if (divisor > 0) {
			// display percent indicator
			col_idx = MIN_MAX_(0, (main_win_get_columns() - 1) * dividend / divisor,
			 main_win_get_columns() - 1);
		}
		app_win->status_bar_color_idx = color_idx;
		app_win->status_bar_col_idx = col_idx;
		strlcpy__(app_win->status_bar_msg, buffer, MAX_SCRN_LINE_BUF_LEN);
		redisp_status_bar();
		app_win->status_bar_displayed = status_bar_to_display;
mflf_d_printf("SB(%d):\n[%s]\n", status_bar_to_display, buffer);
	}
}
void redisp_status_bar()
{
	app_win_stack_entry *app_win = get_app_win_stack_entry(-1);
	char color_idx = app_win->status_bar_color_idx;
	int col_idx = app_win->status_bar_col_idx;
	const char *buffer = app_win->status_bar_msg;

	set_color_by_idx(color_idx, 0);
	blank_status_bar();
	// display status bar
	main_win_output_string(get_status_line_y(), 0, buffer, -1);
	if (col_idx >= 0) {
		int col_idx_1, col_idx_2;
		int byte_idx_1, byte_idx_2;
		// col_idx: 0 -- main_win_get_columns()-1
		byte_idx_1 = byte_idx_from_col_idx(buffer, col_idx, CHAR_LEFT,  &col_idx_1);
		byte_idx_2 = byte_idx_from_col_idx(buffer, col_idx+1, CHAR_RIGHT, &col_idx_2);
		set_color_by_idx(color_idx, 1);
		// display percent indicator
		main_win_output_string(get_status_line_y(), col_idx_1,
		 &buffer[byte_idx_1], byte_idx_2 - byte_idx_1);
	}
}

void blank_status_bar(void)
{
	main_win_clear_lines(get_status_line_y(), -1);
}

void blank_key_list_lines(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_lines(get_key_list_line_y(), get_key_list_line_y() + get_key_list_lines());
}

PRIVATE int input_line_y = 2;
// determines Y position of input-box avoiding the current cursor line
int determine_input_line_y()
{
	int cursor_y = win_get_saved_cursor_y();
	input_line_y = default_input_line_y();
	if (IS_IN_RANGE(input_line_y-1, cursor_y, input_line_y + 3+1)) {
		// avoid showing input box over the current cursor line
		if (input_line_y < cursor_y) {
			input_line_y = cursor_y - (3+1);
		} else {
			input_line_y = cursor_y + 1+1;
		}
	}
	return input_line_y = MIN_MAX_(TITLE_LINES, input_line_y, get_status_line_y());
}
int get_input_line_y(void)
{
	return input_line_y;
}
int default_input_line_y(void)
{
	return main_win_get_mid_win_y() + main_win_get_mid_win_lines() / 2;
}

int get_yes_no_line_y(void)
{
	// if there is key-list-line, input on KEY_LIST_LINE otherwise STATUS_LINE
	return get_key_list_lines() ? get_key_list_line_y() : get_status_line_y();
}

int get_status_line_y(void)
{
	return main_win_get_bottom_win_y() + STATUS_LINE;
}
int get_key_list_line_y(void)
{
	return main_win_get_bottom_win_y() + KEY_LIST_LINE;
}

// End of disp.c
