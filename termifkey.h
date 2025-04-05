/**************************************************************************
 *   termifkey.h - Linux terminal keyboard interface                      *
 *                                                                        *
 *   Copyright (C) 2020-2020 Toshio Koike                                 *
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

#ifndef termifkey_h
#define termifkey_h

// These keywords are defined in curses.h
#define KEY_RESIZE		0x019a
#define KEY_HOME		0x0106
#define KEY_END			0x0168
#define KEY_UP			0x0103
#define KEY_DOWN		0x0102
#define KEY_LEFT		0x0104
#define KEY_RIGHT		0x0105
#define KEY_PPAGE		0x0153
#define KEY_NPAGE		0x0152
#define KEY_IC			0x014b
#define KEY_DC			0x014a
#define KEY_BACKSPACE	0x0107
#define KEY_ENTER		0x0157
#define KEY_F(fknum)	(0x0108 + fknum)

key_code_t termif_input_key(void);

void investigate_key_sequence(void);

#endif // termifkey_h

// End of termifkey.h
