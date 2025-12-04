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

void set_title_bar_color_by_state(int color_idx, char invert);
const char *root_notation();

//! priority to display on the status bar
typedef enum /*S_B*/ {	// Status Bar Displayed
	S_B_NONE,				//! 0 none
	S_B_AUTO,				//! 1 automatic periodic update of information in editor/filer
	S_B_CURS,				//! 2 update on cursor movement in editor/filer by user
	S_B_ING,				//! 3 processing (overwritable by WARN, ERR and DONE)
	S_B_WARN,				//! 4 warning (overwritable by ERR and DONE)
	S_B_ERR,				//! 5 error (overwritable by DONE)
	S_B_DONE,				//! 6 done  (this message outputs on completion of a command)
	S_B_ASYN,				//! 7 asynchronous event (such as history file update by other inst.)
	S_B_TYPES,				//! 8 Number from S_B_NONE to S_B_ASYN
} s_b_d_t;

#define disp_status_bar_ing(args...)	disp_status_bar_type(S_B_ING, args)
#define disp_status_bar_warn(args...)	disp_status_bar_type(S_B_WARN, args)
#define disp_status_bar_err(args...)	disp_status_bar_type(S_B_ERR, args)
#define disp_status_bar_done(args...)	disp_status_bar_type(S_B_DONE, args)
#define disp_status_bar_async(args...)	disp_status_bar_type(S_B_ASYN, args)
void clear_status_bar_displayed();
void disp_status_bar_type(s_b_d_t status_bar_to_display, const char *msg, ...);
void redisp_status_bar();

void blank_status_bar();
void blank_key_list_lines();

void disp_status_bar_cwd();

#endif // disp_h

// End of disp.h
