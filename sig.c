/**************************************************************************
 *   sig.c                                                                *
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

void signal_init(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(struct sigaction));

	// Trap SIGINT and SIGQUIT cuz we want them to do useful things.
///	sigact.sa_handler = SIG_IGN;
	sigact.sa_handler = handler_sigint;
	sigaction(SIGINT, &sigact, NULL);

	// Trap SIGHUP and SIGTERM cuz we want to write the file out.
	sigact.sa_handler = handler_sighup;
	sigaction(SIGHUP, &sigact, NULL);
	sigact.sa_handler = handler_sigterm;
	sigaction(SIGTERM, &sigact, NULL);

	// Trap SIGWINCH.
	sigact.sa_handler = handle_sigwinch;
	sigaction(SIGWINCH, &sigact, NULL);

	// Trap SIGSEGV.
	sigact.sa_handler = handler_sigsegv;
	sigaction(SIGSEGV, &sigact, NULL);

#ifndef _POSIX_VDISABLE
	sigact.sa_handler = SIG_IGN;
	sigaction(SIGTSTP, &sigact, NULL);		// Terminal SToP signal
#endif
}

void signal_fork(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(struct sigaction));

	// Trap SIGINT.
	sigact.sa_handler = handler_sigint;
	sigaction(SIGINT, &sigact, NULL);
	// Trap SIGWINCH.
	sigact.sa_handler = handle_sigwinch;
	sigaction(SIGWINCH, &sigact, NULL);
}

void signal_clear(void)
{
	struct sigaction sigact;

	memset(&sigact, 0, sizeof(struct sigaction));

	sigact.sa_handler = SIG_DFL;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGHUP, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGWINCH, &sigact, NULL);
	sigaction(SIGSEGV, &sigact, NULL);
#ifndef _POSIX_VDISABLE
	sigaction(SIGTSTP, &sigact, NULL);
#endif
}

PRIVATE int handler_sigint_called = 0;
void clear_handler_sigint_called(void)
{
	handler_sigint_called = 0;
}
RETSIGTYPE handler_sigint(int signal)
{
	handler_sigint_called = 1;
}
int is_handler_sigint_called(void)
{
	return handler_sigint_called;
}

RETSIGTYPE handler_sighup(int signal)
{
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(500);
	die_on("Received SIGHUP\n");
}
RETSIGTYPE handler_sigterm(int signal)
{
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(500);
	die_on("Received SIGTERM\n");
}
RETSIGTYPE handler_sigsegv(int signal)
{
	flf_d_printf("Received SIGSEGV\n");
#if 0
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(200);
	tio_beep(); MSLEEP(500);
#endif
	_D_(output_last_d_printf());
	exit(255);
}

PRIVATE int sigwinch_signaled = 0;
void clear_sigwinch_signaled(void)
{
	sigwinch_signaled = 0;
}
RETSIGTYPE handle_sigwinch(int signal)
{
	sigwinch_signaled = 1;
///	ioctl_get_win_size();
}
int is_sigwinch_signaled(void)
{
	return sigwinch_signaled;
}

///int ioctl_ws_col = 0;
///int ioctl_ws_row = 0;
///int ioctl_get_win_size(void)
///{
///	const char *tty;
///	int fd;
///	int result = 0;
///	struct winsize winsz;
///
///	tty = ttyname(0);
///	if (tty == NULL) {
///		ioctl_ws_col = -1;
///		return -1;
///	}
///	fd = open(tty, O_RDWR);
///	if (fd < 0) {
///		ioctl_ws_col = -2;
///		return -2;
///	}
///	result = ioctl(fd, TIOCGWINSZ, &winsz);
///	close(fd);
///	if (result < 0) {
///		ioctl_ws_col = -3;
///		return -3;
///	}
///	// screen size gotten.
///	// in some cases ncurses has already updated them but not in all cases.
///	ioctl_ws_col = winsz.ws_col;
///	ioctl_ws_row = winsz.ws_row;
//////flf_d_printf("cols: %d, lines: %d\n", ioctl_ws_col, ioctl_ws_row);
///	return 0;
///}

// End of sig.c
