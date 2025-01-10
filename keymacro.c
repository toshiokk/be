/**************************************************************************
 *   keymacro.c                                                           *
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

#include "headers.h"

// Keyboard macro
#define MAX_KEY_STROKES			MAX_EDIT_LINE_LEN / KEY_CODE_STR_LEN
#define MAX_KEY_MACRO_STR_LEN	(MAX_KEY_STROKES * KEY_CODE_STR_LEN)
// key macro recording ---------------------------------------------------------
PRIVATE int key_strokes_recording = -1;			// >=0:recording, -1: not recording
PRIVATE key_code_t key_codes_recording[MAX_KEY_STROKES+1];
// key macro recorded  ---------------------------------------------------------
PRIVATE int key_strokes_recorded = 0;			// recorded key strokes
PRIVATE key_code_t key_codes_recorded[MAX_KEY_STROKES+1];
// key macro playing back ------------------------------------------------------
PRIVATE int key_strokes_playing_back = -1;		// >=0:playing back
//------------------------------------------------------------------------------

#ifdef ENABLE_HISTORY
char key_macro_loaded[MAX_PATH_LEN+1] = "";
void load_key_macro(int last_n)
{
	strlcpy__(key_macro_loaded, get_history_newest(HISTORY_TYPE_IDX_KEYMACRO, last_n),
	 MAX_PATH_LEN);
	key_strokes_recorded = get_key_macro_from_string(key_macro_loaded, key_codes_recorded);
}
void save_key_macro(void)
{
	update_history(HISTORY_TYPE_IDX_KEYMACRO,
	 get_string_from_key_macro(key_codes_recorded, key_strokes_recorded));
}
#endif // ENABLE_HISTORY

//------------------------------------------------------------------------------
// start/abort recording
int doe_start_rec__cancel_rec(void)
{
	if (key_macro_is_recording() == 0) {
		doe_start_recording();
	} else {
		doe_cancel_recording();
	}
	return 0;
}
// end recording/start playback
int doe_end_rec__playback(void)
{
	if (key_macro_is_recording()) {
		doe_end_recording();
	} else {
		doe_playback_last_1();
	}
	return 0;
}
//------------------------------------------------------------------------------
PRIVATE int start_playback_last_n(int last_n);
int doe_start_recording(void)
{
	disp_status_bar_done(_("Start key macro recording"));
	key_macro_start_recording();

	disp_title_bar_editor();
	tio_refresh();
	return 0;
}
int doe_cancel_recording(void)
{
	disp_status_bar_done(_("Cancel key macro recording"));
	key_macro_cancel_recording();

	disp_title_bar_editor();
	tio_refresh();
	return 0;
}
int doe_end_recording(void)
{
	disp_status_bar_done(_("End key macro recording"));
	key_macro_end_recording();

#ifdef ENABLE_HISTORY
	save_key_macro();
#endif // ENABLE_HISTORY

	disp_title_bar_editor();
	tio_refresh();
	return 0;
}
int doe_playback_last_1(void)
{
	start_playback_last_n(1);
	return 0;
}
int doe_playback_last_2(void)
{
	start_playback_last_n(2);
	return 0;
}
PRIVATE int start_playback_last_n(int last_n)
{
	disp_status_bar_done(_("Start key macro playback"));
#ifdef ENABLE_HISTORY
	load_key_macro(last_n);
#endif // ENABLE_HISTORY
	key_macro_start_playback();
	return 0;
}
#ifdef ENABLE_HISTORY
int doe_playback_string(void)
{
	if (chk_inp_str_ret_val_editor(input_string_pos(key_macro_loaded,
	 key_macro_loaded, MAX_PATH_LEN,
	 HISTORY_TYPE_IDX_KEYMACRO, _("Input key macro to run:")))) {
		// cancelled
		return 0;						// cancelled
	}
	key_strokes_recorded = get_key_macro_from_string(key_macro_loaded, key_codes_recorded);
	key_macro_start_playback();
	return 0;
}
#endif // ENABLE_HISTORY
void key_macro_start_recording(void)
{
	key_strokes_recording = 0;		// start recording
	start_title_bar_blinking();
}
void key_macro_put_key(key_code_t key)
{
	if (key >= 0 && key_macro_is_recording()) {
		// recording
		if (key_strokes_recording < MAX_KEY_STROKES)
			// record one key stroke
			key_codes_recording[key_strokes_recording++] = key;
	}
}
void key_macro_delete_last_key(void)
{
	if (key_strokes_recording > 0) {
		key_strokes_recording--;	// cancel the last key (End-rec Key)
	}
}
int key_macro_is_recording(void)
{
	return key_strokes_recording >= 0;
}
void key_macro_cancel_recording(void)
{
	key_strokes_recording = -1;		// abort recording
	stop_title_bar_blinking();
}
void key_macro_end_recording(void)
{
	if (key_macro_is_recording()) {
		key_macro_delete_last_key();	// cancel the last key (End-rec Key)
	}
flf_d_printf("key_strokes_recording: %d\n", key_strokes_recording);
	key_strokes_recorded = key_strokes_recording;
	// copy "recording" to "recorded"
	memcpy__(key_codes_recorded, key_codes_recording,
	 key_strokes_recorded * sizeof(key_codes_recording[0]));
	key_macro_cancel_recording();
}

//------------------------------------------------------------------------------
int key_macro_start_playback(void)
{
	if (key_macro_is_recording()) {
		key_macro_delete_last_key();	// cancel the last key (Start-playback Key)
	}
	if (key_strokes_recorded > 0) {		// recorded ?
		key_strokes_playing_back = 0;		// playback key strokes
	}
	return key_strokes_recorded;
}
key_code_t key_macro_get_key(void)
{
	key_code_t key = K_NONE;

	if (key_macro_is_playing_back()) {
		if (key_strokes_playing_back < key_strokes_recorded) {
			// playback one key stroke
			key = key_codes_recorded[key_strokes_playing_back++];
			if (key_strokes_playing_back >= key_strokes_recorded) {
				key_macro_end_playback();
				set_edit_win_update_needed(UPDATE_SCRN_ALL);
			}
		}
	}
	return key;
}
void key_macro_end_playback(void)
{
	key_strokes_playing_back = -1;		// stop playback
}
int key_macro_is_playing_back(void)
{
	return key_strokes_playing_back >= 0;
}
//------------------------------------------------------------------------------
// ==> "0"~"9", "A"~"Z", "a"~"z", "Ａ"~"Ｚ", "あ"~"ん", "亜"~"輪", ...
// ==> "\{M-(}", "\(M-{)", "\{UP}", "\{DEL}", ...
// ==> "\1b00", ...
char *get_string_from_key_macro(key_code_t *key_codes, int keys)
{
	static char str_buf[MAX_KEY_MACRO_STR_LEN + 1];
	strcpy__(str_buf, "");
	for (int key_idx = 0; key_idx < keys; key_idx++) {
		const char* str = key_str_from_key_code(key_codes[key_idx]);
		if (strlen(str_buf) + strlen(str) < MAX_KEY_MACRO_STR_LEN) {
			strlcat__(str_buf, MAX_KEY_MACRO_STR_LEN, str);
		}
	}
/////_D_(dump_string("string", str_buf))
	return str_buf;
}

// "0"~"9", "A"~"Z", "a"~"z", "Ａ"~"Ｚ", "あ"~"ん", "亜"~"輪", ...
// "((", "))", "{{", "}}"
// "{M-(}", "{M-)}", "(M-{)", "(M-})"
// "{1b00}"
int get_key_macro_from_string(const char *string, key_code_t *key_codes)
{
/////_D_(dump_string("string", string))
	int key_idx = 0;
	if (string == NULL)
		return key_idx;
	const char *str = string;
	for (key_idx = 0; key_idx < MAX_KEY_STROKES && *str; ) {
		key_code_t key_code;
		int len = key_code_from_key_str(str, &key_code);
		if (key_code >= 0) {
			key_codes[key_idx++] = key_code;
		}
		str += len;
	}
/////_D_(dump_memory("from_string", key_codes, key_idx * 2))
	return key_idx;
}

#ifdef START_UP_TEST
void test_key_code_from_to_key_name()
{
	key_code_t key_codes[MAX_KEY_STROKES+1];
	int keys = 0;			// recorded key strokes
	const char *ptr;
	key_code_t key_codes2[MAX_KEY_STROKES+1];
	int keys2;

flf_d_printf("MAX_KEY_STROKES: %d\n", MAX_KEY_STROKES);
flf_d_printf("get_key_name_table_entries(): %d\n", get_key_name_table_entries());
	for (key_code_t key = 0x0000; key < 0x0110; key++) {
		if (keys >= MAX_KEY_STROKES) {
			break;
		}
		key_codes[keys++] = (UINT16)key;
	}
	_D_(dump_memory("input:", key_codes, keys * 2))
	ptr = get_string_from_key_macro(key_codes, keys);
	_D_(dump_string("string", ptr))

	keys2 = get_key_macro_from_string(ptr, key_codes2);
	_D_(dump_memory("output:", key_codes2, keys2 * 2))
	MY_UT_BIN(key_codes, keys * 2, key_codes2, keys2 * 2);

	keys = 0;			// recorded key strokes
	for (int idx = 0; idx < get_key_name_table_entries(); idx++) {
		if (keys >= MAX_KEY_STROKES) {
			break;
		}
		key_codes[keys++] = key_name_table[idx].key_code;
	}
	_D_(dump_memory("input:", key_codes, keys * 2))
	ptr = get_string_from_key_macro(key_codes, keys);
	_D_(dump_string("string", ptr))

	keys2 = get_key_macro_from_string(ptr, key_codes2);
	_D_(dump_memory("output:", key_codes2, keys2 * 2))
	MY_UT_BIN(key_codes, keys * 2, key_codes2, keys2 * 2);
}
#endif // START_UP_TEST

// End of keymacro.c
