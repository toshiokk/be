/**************************************************************************
 *   keymacro.h                                                           *
 *                                                                        *
 *   Copyright (C) 2005-2006 PCR                                          *
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

#ifndef keymacro_h
#define keymacro_h

#ifdef ENABLE_HISTORY
void load_key_macro(int last_n);
void save_key_macro(void);
#endif // ENABLE_HISTORY

int do_start_rec__cancel_rec(void);
int do_end_rec__playback(void);

int do_start_recording(void);
int do_cancel_recording(void);
int do_end_recording(void);
int do_playback(void);
int do_playback_last_1(void);
int do_playback_last_2(void);

void key_macro_start_recording(void);
void key_macro_put_key(key_code_t key);
int key_macro_is_recording(void);
void key_macro_cancel_recording(void);
void key_macro_end_recording(void);

int key_macro_start_playback(void);
key_code_t key_macro_get_key(void);
void key_macro_end_playback(void);
int key_macro_is_playing_back(void);

const char *short_key_name_from_key_code(key_code_t key_code, char *buf);
const char *key_name_from_key_code(key_code_t key_code, char *buf);
const char *short_key_name_from_key_name(const char *key_name, char *buf);
key_code_t key_code_from_key_name(char *key_name);
key_code_t key_code_from_short_key_name(char *short_key_name);
int get_key_name_table_entries(void);

char *get_string_from_key_macro(void);
void get_key_macro_from_string(const char *string);

#endif // keymacro_h

// End of keymacro.h
