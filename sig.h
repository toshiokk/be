/**************************************************************************
 *   sig.h - signal handling                                              *
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

#ifndef sig_h
#define sig_h

void signal_init(void);
void signal_fork(void);
void signal_clear(void);
void clear_handler_sigint_called(void);
RETSIGTYPE handler_sigint(int signal);
int is_handler_sigint_called(void);
RETSIGTYPE handler_sighup(int signal);
RETSIGTYPE handler_sigterm(int signal);
RETSIGTYPE handler_sigsegv(int signal);

void clear_sigwinch_signaled(void);
RETSIGTYPE handle_sigwinch(int signal);
int is_sigwinch_signaled(void);

extern int ioctl_ws_col;
extern int ioctl_ws_row;
int ioctl_get_win_size(void);

#endif // sig_h

// End of sig.h
