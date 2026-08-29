/**************************************************************************
 *   bincode.h - binary code conversion                                   *
 *                                                                        *
 *   Copyright (C) 2026-2026 PCR                                          *
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

#ifndef bincode_h
#define bincode_h

void conv_weird_byte_init(char ascii8);
short conv_weird_byte_first(short chr);
short conv_weird_byte_next();

void conv_utf8_to_bin_init(char ascii8);
int conv_utf8_to_bin(const char *line, char *out_buf);

void internal_to_display_conv_init();
const char *internal_to_display_conv(const char *utf8c);

#endif // bincode_h

// End of bincode.h
