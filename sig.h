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

void signal_init();
void signal_fork();
void signal_clear();
void clear_sigint_signaled();
void set_sigint_signaled();
RETSIGTYPE handler_sigint(int signal);
int is_sigint_signaled();

RETSIGTYPE handler_sighup(int signal);
RETSIGTYPE handler_sigterm(int signal);
RETSIGTYPE handler_sigsegv(int signal);

void clear_sigwinch_signaled();
RETSIGTYPE handle_sigwinch(int signal);
int is_sigwinch_signaled();

#endif // sig_h

// End of sig.h
