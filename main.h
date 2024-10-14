/**************************************************************************
 *   main.h                                                               *
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

#ifndef main_h
#define main_h

// space for displaying one func key
#define KEY_LIST_LEN		(1 + 10 + 1)

// If screen columns is wider than this value, set two panes mode at start up
///#define SCRN_COLS_TWO_PANES		120

#if 0
#define KEY_TO_TEMPL(key) \
 ((key) == CHAR_ESC ? "ESC" : \
  ((key) < ' ' ? "^%c" : \
   (0x1b00 <= (key) && (key) <= 0x1bff ? "M-%c" : \
	(KEY_F(1) <= (key) && (key) <= KEY_F(12) ? "F%d" : \
	 (KEY_F(13) <= (key) && (key) <= KEY_F(24) ? "SF%d" : \
	  ((key) == K_UP ? "UP" : "%02x") \
	 ) \
	) \
   ) \
  ) \
 )
#define KEY_TO_CHAR(key) \
 ((key) < ' ' ? ('@' + (key)) : \
  (0x1b00 <= (key) && (key) <= 0x1bff ? toupper((key) & 0x00ff) : \
   (KEY_F(1) <= (key) && (key) <= KEY_F(12) ? (key) - KEY_F(1) + 1 : \
	(KEY_F(13) <= (key) && (key) <= KEY_F(24) ? (key) - KEY_F(13) + 1 : \
	 (key) \
	) \
   ) \
  ) \
 )
#endif

void app_main_loop(void);

int write_text_to_file(const char *file_path, const char *text);

void app_die_on(const char *msg);
void free_all_allocated_memory(void);

void show_usage(void);
void show_version(void);

#ifdef ENABLE_HELP
void disp_splash(int delay);
#endif // ENABLE_HELP

#endif // main_h

// End of main.h
