/**************************************************************************
 *   disp.h                                                               *
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

#ifndef disp_h
#define disp_h

void set_title_bar_color_by_state(int mode1, int mode2, char invert);
const char *root_notation(void);

//! priority to display on the status bar
typedef enum /*s_b_d*/ {	// Status Bar Displayed
	S_B_D_NONE,				//! 0 none
	S_B_D_PERCENT_EDITOR,	//! 1 cursor movement in editor
	S_B_D_PERCENT_FILER,	//! 2 cursor movement in filer
	S_B_D_ING,				//! 3 doing (overwritable by DONE or ERR)
	S_B_D_ERR,				//! 4 error (overwritable by DONE and after one second)
	S_B_D_DONE,				//! 5 done  (this message shall be output on completion of a command)
} s_b_d_t;
void clear_status_bar_displayed(void);
void set_status_bar_displayed(s_b_d_t status_bar_to_display);
void disp_status_bar_ing(const char *msg, ...);
void disp_status_bar_done(const char *msg, ...);
void disp_status_bar_err(const char *msg, ...);
void disp_status_bar_percent_editor(const char *msg, ...);
void disp_status_bar_percent_filer(const char *msg, ...);

void blank_status_bar(void);
void blank_key_list_lines(void);
int get_input_line_y(void);
int get_yes_no_line_y(void);

#endif // disp_h

// End of disp.h
