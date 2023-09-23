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

void set_title_bar_color_by_state(int c_e_b_cut_mode)
{
	if (key_macro_is_recording()) {
		// Recording key macro, you see strange color on title bar.
		set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED2, 0);
	} else
	if (c_e_b_cut_mode) {
		// Marking text, you see strange color on title bar.
		set_color_by_idx(ITEM_COLOR_IDX_TEXT_SELECTED, 0);
	} else
	if (geteuid() == 0) {
		// If you are super user, you see strange color on title bar.
		set_color_by_idx(ITEM_COLOR_IDX_WARNING, 0);
	} else {
		// Normal color
		set_color_by_idx(ITEM_COLOR_IDX_TITLE, 0);
	}
}

const char *root_notation(void)
{
	return (geteuid() == 0) ? "[ROOT] " : "";
}

//-----------------------------------------------------------------------------
PRIVATE s_b_d_t status_bar_displayed = S_B_D_NONE;

PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 int dividend, int divisor, const char *msg, va_list ap);

// enable status bar update
void clear_status_bar_displayed(void)
{
	set_status_bar_displayed(S_B_D_NONE);
}
void set_status_bar_displayed(s_b_d_t s_b_d)
{
	status_bar_displayed = s_b_d;
}
void disp_status_bar_ing(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_ING, 0, 0, msg, ap);
	va_end(ap);
	tio_refresh();	// update screen soon
}
void disp_status_bar_done(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_DONE, 0, 0, msg, ap);
	va_end(ap);
}
void disp_status_bar_err(const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_ERR, 0, 0, msg, ap);
	va_end(ap);
	tio_beep();
}
void disp_status_bar_percent_editor(int dividend, int divisor, const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_PERCENT_EDITOR, dividend, divisor, msg, ap);
	va_end(ap);
}
void disp_status_bar_percent_filer(int dividend, int divisor, const char *msg, ...)
{
	va_list ap;

	va_start(ap, msg);
	disp_status_bar_percent_va(S_B_D_PERCENT_FILER, dividend, divisor, msg, ap);
	va_end(ap);
}

// Examples:
//  Reading File filename.ext ...
//  Writing File filename.ext ...
//  "abc" not found  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CODE:2f ENC:ASCII EOL:LF(NIX)
//  2 files loaded  |  LINE:  12/1234 COLUMN:40/80 SIZE:  1000 CODE:2f ENC:ASCII EOL:LF(NIX)
PRIVATE void disp_status_bar_percent_va(s_b_d_t status_bar_to_display,
 int dividend, int divisor, const char *msg, va_list ap)
{
	int update = 0;
	char buf[MAX_SCRN_LINE_BUF_LEN+1];
	char buffer[MAX_SCRN_LINE_BUF_LEN+1];
	static char prev_msg[MAX_SCRN_LINE_BUF_LEN+1];
	int color_idx;
	int col_idx;
	int byte_idx;

	switch (status_bar_to_display) {
	default:
	case S_B_D_NONE:
	case S_B_D_PERCENT_FILER:
	case S_B_D_ING:
	case S_B_D_DONE:
///		if (status_bar_to_display >= status_bar_displayed) {
			// display this if higher or the same priority
			status_bar_displayed = status_bar_to_display;
			update = 1;
///		}
		break;
	case S_B_D_ERR:
		if (status_bar_to_display > status_bar_displayed) {
			// display this if higher priority
			status_bar_displayed = status_bar_to_display;
			update = 1;
		}
		break;
	case S_B_D_PERCENT_EDITOR:
		update = 1;
		break;
	}

///mflf_d_printf("SBD: %d,%d [%s]\n", status_bar_displayed, status_bar_to_display, prev_msg);
	if (update) {
///mflf_d_printf("%d:[%s]\n", status_bar_displayed, buffer);
		vsnprintf(buf, MAX_SCRN_LINE_BUF_LEN+1, msg, ap);
///mflf_d_printf("msg:[%s]\n", msg);
///mflf_d_printf("[%s]\n", buf);
		switch (status_bar_to_display) {
		default:
		case S_B_D_NONE:
		case S_B_D_PERCENT_FILER:
		case S_B_D_ING:
			// this time: "NEW_MSG"
			strlcpy__(buffer, buf, MAX_SCRN_LINE_BUF_LEN);
			strcpy__(prev_msg, "");
			// next time: "NEW_MSG"
			break;
		case S_B_D_DONE:
		case S_B_D_ERR:
			// this time: "NEW_MSG"
			strlcpy__(buffer, buf, MAX_SCRN_LINE_BUF_LEN);
			strlcpy__(prev_msg, buf, MAX_SCRN_LINE_BUF_LEN);
			// next time: "PREV_MSG || NEW_MSG"
			break;
		case S_B_D_PERCENT_EDITOR:
			// this time: "PREV_MSG || NEW_MSG"
			strcpy__(buffer, "");
			if (strnlen(prev_msg, MAX_SCRN_LINE_BUF_LEN)) {
				strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, prev_msg);
				strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, "  |  ");
			}
			strlcat__(buffer, MAX_SCRN_LINE_BUF_LEN, buf);
			strcpy__(prev_msg, "");
			// next time: "NEW_MSG"
			break;
		}
		adjust_utf8s_columns(buffer, main_win_get_columns());
		switch (status_bar_displayed) {
		default:
		case S_B_D_NONE:
		case S_B_D_PERCENT_FILER:
		case S_B_D_ING:
		case S_B_D_DONE:
		case S_B_D_PERCENT_EDITOR:
			color_idx = ITEM_COLOR_IDX_STATUS;
			break;
		case S_B_D_ERR:
			color_idx = ITEM_COLOR_IDX_WARNING;
			break;
		}
		if (tio_is_initialized() == FALSE) {
			e_printf("Terminal I/O not initialized !! ");
			e_vprintf(msg, ap);
		} else {
			set_color_by_idx(color_idx, 0);
			blank_status_bar();
			main_win_output_string(main_win_get_bottom_win_y() + STATUS_LINE, 0, buffer, -1);
			if (divisor > 0) {
				// display percent indicator "bar"
				col_idx = 1 + (main_win_get_columns()-2) * dividend / divisor;
				// col_idx: 1 -- main_win_get_columns()-1
				byte_idx = byte_idx_from_col_idx(buffer, col_idx, CHAR_RIGHT, &col_idx);
				set_color_by_idx(color_idx, 1);
				main_win_output_string(main_win_get_bottom_win_y() + STATUS_LINE, col_idx,
				 &buffer[byte_idx], -1);
			}
		}
///mflf_d_printf("SB: [%s]\n", buffer);
	}

	if (status_bar_to_display == S_B_D_PERCENT_EDITOR) {
		clear_status_bar_displayed();
	}
}

void blank_status_bar(void)
{
	main_win_clear_lines(main_win_get_bottom_win_y() + STATUS_LINE, -1);
}
void blank_yes_no_line(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	main_win_clear_lines(get_yes_no_line_y(), -1);
}

void blank_key_list_lines(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_KEY_LIST2, 0);
	main_win_clear_lines(main_win_get_bottom_win_y() + KEY_LIST_LINE,
	 main_win_get_bottom_win_y() + KEY_LIST_LINE + get_key_list_lines());
}

int get_input_line_y(void)
{
	return main_win_get_mid_win_y() + main_win_get_mid_win_lines() / 2;
}
int get_yes_no_line_y(void)
{
	// if there is key-list-line, input on KEY_LIST_LINE otherwise STATUS_LINE
	return main_win_get_bottom_win_y()
	 + (get_key_list_lines() ? KEY_LIST_LINE : STATUS_LINE);
}

// End of disp.c
