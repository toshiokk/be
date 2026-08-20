/**************************************************************************
 *   encdet.h - character encoding determination                          *
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

#ifndef encdet_h
#define encdet_h

typedef enum {
	ENCDET_ASCII	= 0,	// 8 bits ASCII
	ENCDET_UTF8		= 1,
	ENCDET_BINARY	= 2,
	ENCDET_JIS		= 3,
	ENCDET_EUCJP	= 4,
	ENCDET_SJIS		= 5,
	ENCDET_SUPPORTED_ENCODINGS	= 6,	// ASCII, UTF8, BINARY, JIS, EUCJP, SJIS
} encoding_t;

int determine_encoding_file_path(const char *full_path);

#endif // encdet_h

// End of encdet.h
