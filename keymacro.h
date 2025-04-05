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
void load_last_key_macro(int last_n);
void update_key_macro_history(void);
#endif // ENABLE_HISTORY

int doe_start_rec__cancel_rec(void);
int doe_end_rec__playback(void);

int doe_start_recording(void);
int doe_cancel_recording(void);
int doe_end_recording(void);
int doe_playback_last_1(void);
int doe_playback_last_2(void);
#ifdef ENABLE_HISTORY
int doe_playback_string(void);
#endif // ENABLE_HISTORY

void key_macro_start_recording(void);
void key_macro_put_key(key_code_t key);
int key_macro_is_recording(void);
void key_macro_cancel_recording(void);
void key_macro_end_recording(void);

int key_macro_start_playback(void);
key_code_t key_macro_get_key(void);
void key_macro_end_playback(void);
int key_macro_is_playing_back(void);

char *get_string_from_key_macro(key_code_t *key_codes, int keys);
int get_key_macro_from_string(const char *string, key_code_t *key_codes);

#ifdef START_UP_TEST
void test_key_code_from_to_key_name();
#endif // START_UP_TEST

#endif // keymacro_h

// End of keymacro.h
