/**************************************************************************
 *   termifkey.c - Linux terminal keyboard interface                      *
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

#include "headers.h"

#ifndef ENABLE_NCURSES

PRIVATE key_code_t input_and_decode_key_sequences();
PRIVATE key_code_t input_key();

key_code_t termif_input_key()
{
	return input_and_decode_key_sequences();
}

#define MAX_KEY_SEQ		7

struct _key_seq_ {
	key_code_t key;
	char *sequences;
} key_seq_table[] = {
	// linux console
	{ K_F01,		"\x1b[[A" },
	{ K_F02,		"\x1b[[B" },
	{ K_F03,		"\x1b[[C" },
	{ K_F04,		"\x1b[[D" },
	{ K_F05,		"\x1b[[E" },
	{ K_F06,		"\x1b[17~" },
	{ K_F07,		"\x1b[18~" },
	{ K_F08,		"\x1b[19~" },
	{ K_F09,		"\x1b[20~" },
	{ K_F10,		"\x1b[21~" },
	{ K_F11,		"\x1b[23~" },
	{ K_F12,		"\x1b[24~" },
	{ K_S_F01,		"\x1b[25~" },
	{ K_S_F02,		"\x1b[26~" },
	{ K_S_F03,		"\x1b[28~" },
	{ K_S_F04,		"\x1b[29~" },
	{ K_S_F05,		"\x1b[31~" },
	{ K_S_F06,		"\x1b[32~" },
	{ K_S_F07,		"\x1b[33~" },
	{ K_S_F08,		"\x1b[34~" },
	{ K_DEL,		"\x1b[3~" },
	{ K_INS,		"\x1b[2~" },
	{ K_HOME,		"\x1b[1~" },
	{ K_END,		"\x1b[4~" },
	{ K_PPAGE,		"\x1b[5~" },
	{ K_NPAGE,		"\x1b[6~" },
	{ K_UP,			"\x1b[A" },
	{ K_DOWN,		"\x1b[B" },
	{ K_RIGHT,		"\x1b[C" },
	{ K_LEFT,		"\x1b[D" },
	// xterm
	{ K_F01,		"\x1bOP" },
	{ K_F02,		"\x1bOQ" },
	{ K_F03,		"\x1bOR" },
	{ K_F04,		"\x1bOS" },
	{ K_F05,		"\x1b[15~" },
	{ K_F06,		"\x1b[17~" },
	{ K_F07,		"\x1b[18~" },
	{ K_F08,		"\x1b[19~" },
	{ K_F09,		"\x1b[20~" },
	{ K_F10,		"\x1b[21~" },
	{ K_F11,		"\x1b[23~" },
	{ K_F12,		"\x1b[24~" },
	{ K_S_F01,		"\x1b[1;2P" },
	{ K_S_F02,		"\x1b[1;2Q" },
	{ K_S_F03,		"\x1b[1;2R" },
	{ K_S_F04,		"\x1b[1;2S" },
	{ K_S_F05,		"\x1b[15;2~" },
	{ K_S_F06,		"\x1b[17;2~" },
	{ K_S_F07,		"\x1b[18;2~" },
	{ K_S_F08,		"\x1b[19;2~" },
	{ K_S_F09,		"\x1b[20;2~" },
	{ K_S_F10,		"\x1b[21;2~" },
	{ K_S_F11,		"\x1b[23;2~" },
	{ K_S_F12,		"\x1b[24;2~" },
	{ K___M_F01,	"\x1b[1;3P" },
	{ K___M_F02,	"\x1b[1;3Q" },
	{ K___M_F03,	"\x1b[1;3R" },
	{ K___M_F04,	"\x1b[1;3S" },
	{ K___M_F05,	"\x1b[15;3~" },
	{ K___M_F06,	"\x1b[17;3~" },
	{ K___M_F07,	"\x1b[18;3~" },
	{ K___M_F08,	"\x1b[19;3~" },
	{ K___M_F09,	"\x1b[20;3~" },
	{ K___M_F10,	"\x1b[21;3~" },
	{ K___M_F11,	"\x1b[23;3~" },
	{ K___M_F12,	"\x1b[24;3~" },
	{ K_S_M_F01,	"\x1b[1;4P" },
	{ K_S_M_F02,	"\x1b[1;4Q" },
	{ K_S_M_F03,	"\x1b[1;4R" },
	{ K_S_M_F04,	"\x1b[1;4S" },
	{ K_S_M_F05,	"\x1b[15;4~" },
	{ K_S_M_F06,	"\x1b[17;4~" },
	{ K_S_M_F07,	"\x1b[18;4~" },
	{ K_S_M_F08,	"\x1b[19;4~" },
	{ K_S_M_F09,	"\x1b[20;4~" },
	{ K_S_M_F10,	"\x1b[21;4~" },
	{ K_S_M_F11,	"\x1b[23;4~" },
	{ K_S_M_F12,	"\x1b[24;4~" },
	{ K__C__F01,	"\x1b[1;5P" },
	{ K__C__F02,	"\x1b[1;5Q" },
	{ K__C__F03,	"\x1b[1;5R" },
	{ K__C__F04,	"\x1b[1;5S" },
	{ K__C__F05,	"\x1b[15;5~" },
	{ K__C__F06,	"\x1b[17;5~" },
	{ K__C__F07,	"\x1b[18;5~" },
	{ K__C__F08,	"\x1b[19;5~" },
	{ K__C__F09,	"\x1b[20;5~" },
	{ K__C__F10,	"\x1b[21;5~" },
	{ K__C__F11,	"\x1b[23;5~" },
	{ K__C__F12,	"\x1b[24;5~" },
	{ K_SC__F01,	"\x1b[1;6P" },
	{ K_SC__F02,	"\x1b[1;6Q" },
	{ K_SC__F03,	"\x1b[1;6R" },
	{ K_SC__F04,	"\x1b[1;6S" },
	{ K_SC__F05,	"\x1b[15;6~" },
	{ K_SC__F06,	"\x1b[17;6~" },
	{ K_SC__F07,	"\x1b[18;6~" },
	{ K_SC__F08,	"\x1b[19;6~" },
	{ K_SC__F09,	"\x1b[20;6~" },
	{ K_SC__F10,	"\x1b[21;6~" },
	{ K_SC__F11,	"\x1b[23;6~" },
	{ K_SC__F12,	"\x1b[24;6~" },
	{ K__CM_F01,	"\x1b[1;7P" },
	{ K__CM_F02,	"\x1b[1;7Q" },
	{ K__CM_F03,	"\x1b[1;7R" },
	{ K__CM_F04,	"\x1b[1;7S" },
	{ K__CM_F05,	"\x1b[15;7~" },
	{ K__CM_F06,	"\x1b[17;7~" },
	{ K__CM_F07,	"\x1b[18;7~" },
	{ K__CM_F08,	"\x1b[19;7~" },
	{ K__CM_F09,	"\x1b[20;7~" },
	{ K__CM_F10,	"\x1b[21;7~" },
	{ K__CM_F11,	"\x1b[23;7~" },
	{ K__CM_F12,	"\x1b[24;7~" },
	{ K_SCM_F01,	"\x1b[1;8P" },
	{ K_SCM_F02,	"\x1b[1;8Q" },
	{ K_SCM_F03,	"\x1b[1;8R" },
	{ K_SCM_F04,	"\x1b[1;8S" },
	{ K_SCM_F05,	"\x1b[15;8~" },
	{ K_SCM_F06,	"\x1b[17;8~" },
	{ K_SCM_F07,	"\x1b[18;8~" },
	{ K_SCM_F08,	"\x1b[19;8~" },
	{ K_SCM_F09,	"\x1b[20;8~" },
	{ K_SCM_F10,	"\x1b[21;8~" },
	{ K_SCM_F11,	"\x1b[23;8~" },
	{ K_SCM_F12,	"\x1b[24;8~" },

	{ K_HOME,		"\x1b[H" },
	{ K_END,		"\x1b[F" },

	{ K_S___UP,		"\x1b[1;2A" },
	{ K_S___DOWN,	"\x1b[1;2B" },
	{ K_S___RIGHT,	"\x1b[1;2C" },
	{ K_S___LEFT,	"\x1b[1;2D" },
	{ K_S___INS,	"\x1b[2;2~" },
	{ K_S___DEL,	"\x1b[3;2~" },
	{ K___M_UP,		"\x1b[1;3A" },
	{ K___M_DOWN,	"\x1b[1;3B" },
	{ K___M_RIGHT,	"\x1b[1;3C" },
	{ K___M_LEFT,	"\x1b[1;3D" },
	{ K___M_INS,	"\x1b[2;3~" },
	{ K___M_DEL,	"\x1b[3;3~" },
	{ K_S_M_UP,		"\x1b[1;4A" },
	{ K_S_M_DOWN,	"\x1b[1;4B" },
	{ K_S_M_RIGHT,	"\x1b[1;4C" },
	{ K_S_M_LEFT,	"\x1b[1;4D" },
	{ K_S_M_INS,	"\x1b[2;4~" },
	{ K_S_M_DEL,	"\x1b[3;4~" },
	{ K__C__UP,		"\x1b[1;5A" },
	{ K__C__DOWN,	"\x1b[1;5B" },
	{ K__C__RIGHT,	"\x1b[1;5C" },
	{ K__C__LEFT,	"\x1b[1;5D" },
	{ K__C__INS,	"\x1b[2;5~" },
	{ K__C__DEL,	"\x1b[3;5~" },
	{ K_SC__UP,		"\x1b[1;6A" },
	{ K_SC__DOWN,	"\x1b[1;6B" },
	{ K_SC__RIGHT,	"\x1b[1;6C" },
	{ K_SC__LEFT,	"\x1b[1;6D" },
	{ K_SC__INS,	"\x1b[2;6~" },
	{ K_SC__DEL,	"\x1b[3;6~" },
	{ K__CM_UP,		"\x1b[1;7A" },
	{ K__CM_DOWN,	"\x1b[1;7B" },
	{ K__CM_RIGHT,	"\x1b[1;7C" },
	{ K__CM_LEFT,	"\x1b[1;7D" },
	{ K__CM_INS,	"\x1b[2;7~" },
	{ K__CM_DEL,	"\x1b[3;7~" },
	{ K_SCM_UP,		"\x1b[1;8A" },
	{ K_SCM_DOWN,	"\x1b[1;8B" },
	{ K_SCM_RIGHT,	"\x1b[1;8C" },
	{ K_SCM_LEFT,	"\x1b[1;8D" },
	{ K_SCM_INS,	"\x1b[2;8~" },
	{ K_SCM_DEL,	"\x1b[3;8~" },

};
#define SIZEOF_KEY_SEQ_TABLE	(sizeof(key_seq_table) / sizeof(key_seq_table[0]))

// input			| match			| output
// ---------------- | ------------- | ------
// XX				| no match		| XX
// XX-YY			| no match		| XX (Keep YY for the next input)
// XX-YY-ZZ			| no match		| XX (Keep YY-ZZ for the next input)
// ESC $			| partial match	| ESC
// ESC-XX			| partial match	| ESC-XX
// ESC-K2			| partial match	| continue input
// ESC-K2 $			| partial match	| ESC-K2
// ESC-K2-XX		| partial match	| ESC-K2 (Keep XX for the next input)
// ESC-K2-K3		| partial match	| continue input
// ESC-K2-K3 $		| partial match	| ESC-K2 (Keep K3 for the next input)
// ESC-K2-K3-XX		| partial match	| ESC-K2 (Keep K3-XX for the next input)
// ESC-K2-K3...Kn	| full match	| decoded-key
// ESC-K2-K3...XX	| partial match	| ESC-K2 (Keep K3...XX for the next input)
// ESC-K2-K3...Kn	| partial match	| continue input
// ESC-K2-K3...Kn $	| partial match	| ESC-K2 (Keep K3...Kn for the next input)
// ESC-K2-K3...K6-XX| partial match	| ESC-K2 (Keep K3...K6-XX for the next input)
//
// NOTE:
//   ESC,K2,K3...Kn : matching part
//   XX,YY,ZZ       : not matching part
//   $              : end of input

PRIVATE int strcmp_match_len(const char *str1, size_t str1_len, const char *str2);

PRIVATE key_code_t input_and_decode_key_sequences()
{
	// NOTE: This MUST be `unsigned` for UTF8 8bit character.
	static unsigned char key_buffer[MAX_KEY_SEQ+1] = "";	// 1 for NUL terminator
	static int key_cnt = 0;

	inline int get_key_from_buf_len(int len) {
		key_code_t key = KEY_NONE;
		if (len == 1) {
			key = key_buffer[0];
			key_cnt -= len;
			memmove(key_buffer, &key_buffer[len], key_cnt);
		} else if (len == 2) {
			key = TBKC(key_buffer[0], key_buffer[1]);
			key_cnt -= len;
			memmove(key_buffer, &key_buffer[len], key_cnt);
		}
		return key;
	}
	// ESC    key, return 0x001b
	// XX     key, return 0x00XX
	// ESC-XX key, return 0x1bXX
	// XX-YY  key, return 0x00XX
	inline int get_key_from_buf() {
		key_code_t key = KEY_NONE;
		if (key_cnt == 1) {
			key = get_key_from_buf_len(1);
		} else if (key_cnt >= 2) {
			if (key_buffer[0] != CHAR_ESC) {
				key = get_key_from_buf_len(1);
			} else {
				key = get_key_from_buf_len(2);
			}
		}
		return key;
	}

	key_code_t key = KEY_NONE;
	for ( ; key_cnt < MAX_KEY_SEQ; ) {
		key = input_key();
		if (key < 0) {
			key = get_key_from_buf();
			break;
		} else {
			key_buffer[key_cnt++] = key;
			key_buffer[key_cnt] = '\0';
			// compare to key sequences then:
			//   if full match:
			//     shift key_buffer by match length and return decoded key code
			//   if partial match:
			//     continue input next key
			//   if no match:
			//     return 0x1bXX
			int max_match_len = 0;
			for (int seq_idx = 0; seq_idx < SIZEOF_KEY_SEQ_TABLE; seq_idx++) {
				int len = strcmp_match_len((char *)key_buffer, key_cnt,
				 key_seq_table[seq_idx].sequences);
				if (len == strlen(key_seq_table[seq_idx].sequences)) {
					// full match, return decoded key code
					memmove(key_buffer, &key_buffer[len], key_cnt - len);
					key_cnt -= len;
					// return decoded key code
					return key_seq_table[seq_idx].key;
				}
				max_match_len = MAX_(max_match_len, len);
			}
			// not full match
			if (max_match_len < key_cnt) {
				// There is no possiblity to match any more
				key = get_key_from_buf();
				break;
			} else {
				// There is a possiblity to match, continue input
			}
		}
	}
	return key;
}

PRIVATE int strcmp_match_len(const char *str1, size_t str1_len, const char *str2)
{
	int len;
	for (len = 0; (len < str1_len) && (str1[len] || str2[len]); len++) {
		if (str1[len] != str2[len]) {
			break;
		}
	}
	return len;		// match len: 0--
}

PRIVATE key_code_t input_key()
{
	key_code_t key = -1;
	unsigned char buf[1];

	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);		// Not block in getchar()
	if (read(STDIN_FILENO, buf, 1) >= 1) {
		key = buf[0];
	}
	return key;
}

void investigate_key_sequence()
{
#if 1
	key_code_t key;
	char key_buffer[MAX_KEY_SEQ+1];
	int key_idx = 0;
	int keys;
	long start_msec;

	termif_init();
	termif_begin();
	for (key = -1 ; key < 0; ) {
		key = input_key();
	}
	key_buffer[key_idx] = key;
	key_idx++;
	key_buffer[key_idx] = '\0';
	for (start_msec = get_msec(); key_idx < MAX_KEY_SEQ && get_msec() < start_msec + 1000; ) {
		if ((key = input_key()) >= 0) {
			key_buffer[key_idx] = key;
			key_idx++;
			key_buffer[key_idx] = '\0';
		}
	}
	keys = key_idx;
	termif_end();
	printf("\n");
	for (key_idx = 0; key_idx < keys; key_idx++) {
		printf("%02x ", key_buffer[key_idx]);
	}
	printf("\n");
#else
	termif_init();
	termif_begin();
	for ( ; ; ) {
		key_code_t key = input_and_decode_key_sequences();
		if (key >= 0) {
			printf("%04x\n", key);
		}
		if (key == 'q') {
			break;
		}
	}
	termif_end();
#endif
}

#endif // ENABLE_NCURSES

// End of termifkey.c
