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

#ifdef ENABLE_HISTORY
void load_key_macro(int last_n)
{
	get_key_macro_from_string(get_history_newest(HISTORY_TYPE_IDX_KEYMACRO, last_n));
}
void save_key_macro(void)
{
	update_history(HISTORY_TYPE_IDX_KEYMACRO, get_string_from_key_macro(), 0);
}
#endif // ENABLE_HISTORY

//-----------------------------------------------------------------------------

// start/abort recording
int doe_start_rec__cancel_rec(void)
{
	if (key_macro_is_recording() == 0)
		doe_start_recording();
	else
		doe_cancel_recording();
	return 0;
}
// end recording/start playback
int doe_end_rec__playback(void)
{
	if (key_macro_is_recording()) {
		doe_end_recording();
	} else {
		doe_playback();
	}
	return 0;
}
//-----------------------------------------------------------------------------
PRIVATE int start_playback_last_n(int last_n);
int doe_start_recording(void)
{
	disp_status_bar_done(_("Start key macro recording"));
	key_macro_start_recording();

	disp_editor_title_bar();
	tio_refresh();
	return 0;
}
int doe_cancel_recording(void)
{
	disp_status_bar_done(_("Cancel key macro recording"));
	key_macro_cancel_recording();

	disp_editor_title_bar();
	tio_refresh();
	return 0;
}
int doe_end_recording(void)
{
	disp_status_bar_done(_("End key macro recording"));
	key_macro_end_recording();

#ifdef ENABLE_HISTORY
	save_key_macro();
///_D_(dump_history_ix(HISTORY_TYPE_IDX_KEYMACRO));
#endif // ENABLE_HISTORY

	disp_editor_title_bar();
	tio_refresh();
	return 0;
}
int doe_playback(void)
{
	if (key_macro_start_playback() == 0) {
		disp_status_bar_done(_("No key macro recorded"));
	} else {
		disp_status_bar_done(_("Start key macro playback"));
	}
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
//-----------------------------------------------------------------------------
// Keyboard macro

#define KEY_CODE_STR_LEN	(2+MAX_KEY_NAME_LEN+1)	// "\(RIGHT)"

#define MAX_KEY_STROKES			MAX_EDIT_LINE_LEN / KEY_CODE_STR_LEN
#define MAX_KEY_MACRO_STR_LEN	(MAX_KEY_STROKES * KEY_CODE_STR_LEN)
// key macro recording --------------------------------------------------------
PRIVATE int key_macro_recording = -1;		// >=0:recording, -1: not recording
PRIVATE key_code_t key_codes_recording[MAX_KEY_STROKES+1];
// key macro recorded and for play back ---------------------------------------
PRIVATE int key_macro_recorded = 0;			// recorded key strokes
PRIVATE int key_macro_playing_back = -1;	// >=0:playing back
PRIVATE key_code_t key_codes_recorded[MAX_KEY_STROKES+1];
void key_macro_start_recording(void)
{
	key_macro_recording = 0;		// start recording
}
void key_macro_put_key(key_code_t key)
{
	if (key >= 0 && key_macro_is_recording()) {
		// recording
		if (key_macro_recording < MAX_KEY_STROKES)
			// record one key stroke
			key_codes_recording[key_macro_recording++] = key;
	}
}
void key_macro_delete_last_key(void)
{
	if (key_macro_recording > 0) {
		key_macro_recording--;	// cancel the last key (End-rec Key)
	}
}
int key_macro_is_recording(void)
{
	return key_macro_recording >= 0;
}
void key_macro_cancel_recording(void)
{
	key_macro_recording = -1;		// abort recording
}
void key_macro_end_recording(void)
{
	if (key_macro_is_recording()) {
		key_macro_delete_last_key();	// cancel the last key (End-rec Key)
	}
flf_d_printf("key_macro_recording: %d\n", key_macro_recording);
	key_macro_recorded = key_macro_recording;
	// copy "recording" to "recorded"
	memcpy__(key_codes_recorded, key_codes_recording,
	 key_macro_recorded * sizeof(key_codes_recording[0]));
	key_macro_cancel_recording();
}

//-----------------------------------------------------------------------------
////PRIVATE int cursor_positioning_mode_save = 0;
int key_macro_start_playback(void)
{
//#ifdef ENABLE_HISTORY
//	load_key_macro(1);
//#endif // ENABLE_HISTORY
	if (key_macro_is_recording()) {
		key_macro_delete_last_key();	// cancel the last key (Start-playback Key)
	}
	if (key_macro_recorded > 0) {	// recorded ?
		key_macro_playing_back = 0;	// playback key strokes
	}
	return key_macro_recorded;
////	cursor_positioning_mode_save = GET_APPMD(ed_CURS_POSITIONING);	// save CURS_POSITIONING
////	SET_APPMD_VAL(ed_CURS_POSITIONING, CURS_POSITIONING_CENTER);	// set CURS_POSITIONING_CENTER
}
key_code_t key_macro_get_key(void)
{
	key_code_t key = -1;

	if (key_macro_is_playing_back()) {
		if (key_macro_playing_back < key_macro_recorded) {
			// playback one key stroke
			key = key_codes_recorded[key_macro_playing_back++];
			if (key_macro_playing_back >= key_macro_recorded) {
				key_macro_end_playback();
				set_edit_win_update_needed(UPDATE_SCRN_ALL);
			}
		}
	}
	return key;
}
void key_macro_end_playback(void)
{
	key_macro_playing_back = -1;		// stop playback
////	SET_APPMD_VAL(ed_CURS_POSITIONING, cursor_positioning_mode_save);	// recover CURS_POSITIONING
}
int key_macro_is_playing_back(void)
{
	return key_macro_playing_back >= 0;
}
//-----------------------------------------------------------------------------
char *get_string_from_key_macro(void)
{
	static char line_buf[MAX_KEY_MACRO_STR_LEN + 1];
	int stroke_idx;
	char buf_key_name[MAX_KEY_NAME_LEN+1];		// "RIGHT"
	const char *key_name;
	char buf_key_code[KEY_CODE_STR_LEN+1];		// "a", "\\", "\(RIGHT)", "\1234"

	strcpy__(line_buf, "");
	for (stroke_idx = 0; stroke_idx < key_macro_recorded; stroke_idx++) {
		if (key_codes_recorded[stroke_idx] == '\\') {
			// "\\" ==> '\'
			snprintf(buf_key_code, KEY_CODE_STR_LEN+1, "\\\\");
		} else if (isgraph(key_codes_recorded[stroke_idx])) {
			snprintf(buf_key_code, KEY_CODE_STR_LEN+1,
			 "%c", (unsigned short)key_codes_recorded[stroke_idx]);
		} else {
			key_name = key_name_from_key_code(key_codes_recorded[stroke_idx], buf_key_name);
			if (key_name[0])
				// "\(UP)"
				snprintf(buf_key_code, KEY_CODE_STR_LEN+1, "\\(%s)", key_name);
			else
				// "\0102"
				snprintf(buf_key_code, KEY_CODE_STR_LEN+1, "\\%04x",
				 key_codes_recorded[stroke_idx]);
		}
		if (strlen(line_buf) + strlen(buf_key_code) < MAX_KEY_MACRO_STR_LEN) {
			strlcat__(line_buf, MAX_KEY_MACRO_STR_LEN, buf_key_code);
		}
	}
flf_d_printf("%d [%s]\n", key_macro_recorded, line_buf);
///get_key_macro_from_string(line_buf);
	return line_buf;
}
void get_key_macro_from_string(const char *string)
{
	int stroke_idx;
	const char *str;
	char key_name[MAX_KEY_NAME_LEN+1];
	int len;
	key_code_t key_code;
	int int_key_code;

flf_d_printf("[%s]\n", string);
	if (string == NULL)
		return;
	str = string;
	for (stroke_idx = 0; str[0]; stroke_idx++) {
		if (str[0] == '\\') {
			if (str[1] == '\\') {
				key_codes_recorded[stroke_idx] = str[1];	// "\\"
				for (len = 0; len < 2 && *str; len++) {
					str++;
				}
			} else
			if (str[1] == '(') {
				// "\(UP)"
				for (len = 2; len < 2+MAX_KEY_NAME_LEN+1 && str[len]; len++) {
					if (str[len] == ')') {
						break;
					}
				}
				if (str[len] == ')') {
					strlcpy__(key_name, &str[2], len-2);	// "RIGHT"
					key_code = key_code_from_key_name(key_name);
					if (key_code >= 0) {
						key_codes_recorded[stroke_idx] = key_code;
					}
					str = &str[len+1];
				} else {
					str = &str[len];
				}
			} else {
				// "\0102"
				sscanf(&str[1], "%04x", &int_key_code);
				key_codes_recorded[stroke_idx] = int_key_code;
				for (len = 0; len < 5 && *str; len++) {
					str++;
				}
			}
		} else {
			key_codes_recorded[stroke_idx] = *str++;
		}
	}
	key_macro_recorded = stroke_idx;
flf_d_printf("%d key strokes\n", key_macro_recorded);
}

// End of keymacro.c
