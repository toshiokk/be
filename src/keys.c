/**************************************************************************
 *   keys.c                                                               *
 *                                                                        *
 *   Copyright (C) 1999-2003 Chris Allegretta                             *
 *   Copyright (C) 2010-2018 PCR                                          *
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

PRIVATE key_code_t input_key_loop_(int mode);
PRIVATE key_code_t input_key_timeout_(int mode);
PRIVATE key_code_t input_key_with_key_macro_(int mode);
PRIVATE key_code_t input_key_repaint_(int mode);
PRIVATE key_code_t input_key_check_break_key();
PRIVATE key_code_t input_key_check_break_key_(int mode);

PRIVATE void start_wait_for_key_input();
PRIVATE int  check_timeout_for_key_input();

PRIVATE int check_terminal_resized__update();

PRIVATE int is_saving_check_break_key();
PRIVATE int is_restoring_check_break_key();
PRIVATE void put_key_codes_check_break_key(key_code_t key);
PRIVATE key_code_t get_key_codes_check_break_key();

PRIVATE void redraw_1s_after_key_input(key_code_t key);
PRIVATE void repaint_screen_after_60s(key_code_t key);

// |function                    |loop|wait    |repaint|resize|buffered|macro-rec|macro-PB|mapping|
// |----------------------------|----|--------|-------|------|--------|---------|--------|-------|
// |input_key_loop()            |Yes |forever | Yes   | Yes  | Yes    | Yes     | Yes    | Yes   |
// |input_unmapped_key_loop()   |Yes |forever | none  | Yes  | none   | none    | none   | none  |
// |input_key_repaint()         |no  |1000/200| Yes   | Yes  | Yes    | Yes     | Yes    | Yes   |
// |input_key_timeout()         |no  |1000/200| none  | Yes  | Yes    | Yes     | Yes    | Yes   |
// |input_key_with_macro_rec()  |no  | no     | none  | none | Yes    | Yes     | No     | Yes   |
// |input_key_with_macro_pb()   |no  | no     | none  | none | Yes    | Yes     | Yes    | Yes   |
// |input_key_check_break_key() |no  | no     | none  | none | Yes    | none    | none   | Yes   |
// |tio_input_key()             |no  | no     | none  | none | none   | none    | none   | none  |
//
// loop     : wait a key input forever
// wait     : wait a key for 1000 or 200 mSec
// repaint  : repaint screen when it's resized
// resize   : check resize event and resize screen
// buffered : playing back the buffered key strokes during a long period processing
// macro-rec: recording a key macro (key strokes)
// macro-PB : playing back a key macro (key strokes)
// mapping  : key code mapping from a system specific key to an abstruct key code

#define IK_LOOP			0x0080		// IK_L_______
#define IK_REPAINT		0x0040		// IK__P______
#define IK_TIMEOUT		0x0020		// IK___T_____
#define IK_RESIZE		0x0010		// IK____S____
#define IK_MACRO_PB		0x0008		// IK_____P___
#define IK_MACRO_REC	0x0004		// IK______R__
#define IK_BREAK_KEY	0x0002		// IK_______B_
#define IK_MAPPING		0x0001		// IK________M
#define IK_LPWSBPR_M		\
 (IK_LOOP|IK_REPAINT|IK_TIMEOUT|IK_RESIZE|IK_MACRO_PB|IK_MACRO_REC|0           |IK_MAPPING)
#define IK_LPWSBPR__		\
 (IK_LOOP|IK_REPAINT|IK_TIMEOUT|IK_RESIZE|IK_MACRO_PB|IK_MACRO_REC|0           |0         )
#define IK__PWSBPR_M		\
 (0      |IK_REPAINT|IK_TIMEOUT|IK_RESIZE|IK_MACRO_PB|IK_MACRO_REC|0           |IK_MAPPING)
#define IK__PWSBP__M		\
 (0      |IK_REPAINT|IK_TIMEOUT|IK_RESIZE|IK_MACRO_PB|0           |0           |IK_MAPPING)
#define IK______PR_M		\
 (0      |0         |0         |0        |IK_MACRO_PB|IK_MACRO_REC|0           |IK_MAPPING)
#define IK________BM		\
 (0      |0         |0         |0        |0          |0           |IK_BREAK_KEY|IK_MAPPING)

key_code_t input_key_loop()
{
	return input_key_loop_(IK_LPWSBPR_M);
}
key_code_t input_unmapped_key_loop()
{
	return input_key_loop_(IK_LPWSBPR__);
}
key_code_t input_key_timeout()
{
	return input_key_loop_(IK__PWSBPR_M);
}
key_code_t input_key_timeout_no_rec()
{
	return input_key_loop_(IK__PWSBP__M);
}
key_code_t input_key_with_key_macro()
{
	return input_key_loop_(IK______PR_M);
}
PRIVATE key_code_t input_key_check_break_key()
{
	return input_key_loop_(IK________BM);
}

PRIVATE key_code_t input_key_loop_(int mode)
{
	key_code_t key;
	while (IS_KEY_INVALID(key = input_key_repaint_(mode))) {
		if ((mode & IK_LOOP) == 0) {
			break;
		}
	}
	return key;
}

// Key input interval:
// | type                  | duration | condition                                          |
// |-----------------------|----------|----------------------------------------------------|
// | redraw all of screen  | 1000 ms  | on a first timeout after a key input               |
// | repaint all of screen | 60000 ms | on a first key input after no input more than 60 s |

PRIVATE key_code_t input_key_repaint_(int mode)
{
	key_code_t key = input_key_timeout_(mode);
	if (mode & IK_REPAINT) {
		redraw_1s_after_key_input(key);
		repaint_screen_after_60s(key);
	}
	return key;
}
PRIVATE key_code_t input_key_timeout_(int mode)
{
	if (mode & IK_TIMEOUT) {
		start_wait_for_key_input();
	}
	key_code_t key = K_NONE;
	for ( ; ; ) {
		if (mode & IK_RESIZE) {
			if (check_terminal_resized__update()) {
				key = K_RESIZE;
				break;
			}
		}
		key = input_key_with_key_macro_(mode);
		if (IS_KEY_VALID(key)) {
			break;
		}
		if (mode & IK_TIMEOUT) {
			if (check_timeout_for_key_input()) {
				break;
			}
			MSLEEP(10);		// wait 10[mS]
		} else {
			break;
		}
	}
	return key;
}
PRIVATE key_code_t input_key_with_key_macro_(int mode)
{
	key_code_t key = KEY_NONE;
	if (mode & IK_MACRO_PB) {
		key = key_macro_get_key();
	}
	if (IS_KEY_INVALID(key)) {
		key = get_menu_key_to_execute();
	}
	if (IS_KEY_INVALID(key)) {
		key = input_key_check_break_key_(mode);
	}
	if (mode & IK_MACRO_REC) {
		key_macro_put_key(key);
	}
	return key;
}
PRIVATE key_code_t input_key_check_break_key_(int mode)
{
	key_code_t key = get_key_codes_check_break_key();
	if (IS_KEY_VALID(key)) {
		// restore the buffered key strokes
		return key;
	}
	key = tio_input_key();
	if (mode & IK_MAPPING) {
		key = map_key_code(key);
	}
	put_key_codes_check_break_key(key);
	return key;
}

//==============================================================================

// Display update durations:
// | type                              | duration [mSec] |
// |-----------------------------------|-----------------|
// | current time update in title bar  | 1000            |
// | file list update                  | 1000            |
// | recording key macro               |  200            |
// | splash screen by key              | infinite        |
#define DEFAULT_KEY_WAIT_MSEC		1000	// return every 1[Sec]
#define KEY_MACRO_KEY_WAIT_MSEC		200		// fast title bar blinking
PRIVATE long key_wait_timeout_msec;
PRIVATE long key_wait_start_msec;
PRIVATE void start_wait_for_key_input()
{
	key_wait_timeout_msec = key_macro_is_recording()
	 ? KEY_MACRO_KEY_WAIT_MSEC : DEFAULT_KEY_WAIT_MSEC;
	key_wait_start_msec = get_msec();
}
PRIVATE int  check_timeout_for_key_input()
{
	return (long)(get_msec() - key_wait_start_msec) >= key_wait_timeout_msec;
}
//------------------------------------------------------------------------------

// Record key strokes pushed while long period processing(such as loading large or many files)
// and Restore it after finished the processing.
PRIVATE int key_codes_saved = -1;			// >=0: saving,    -1: not recording
PRIVATE int key_codes_restored = -1;		// >=0: restoring, -1: not restoring
#define MAX_KEY_STROKES_CHECK_BREAK_KEY		40
PRIVATE key_code_t key_codes_check_break_key[MAX_KEY_STROKES_CHECK_BREAK_KEY+1];

PRIVATE int is_saving_check_break_key()
{
	return (0 <= key_codes_saved && key_codes_saved < MAX_KEY_STROKES_CHECK_BREAK_KEY)
	 && (is_restoring_check_break_key() == 0);
}
PRIVATE int is_restoring_check_break_key()
{
	return ((0 <= key_codes_restored) && (key_codes_restored < key_codes_saved));
}
PRIVATE void put_key_codes_check_break_key(key_code_t key)
{
	// saving key strokes
	if (IS_KEY_VALID(key) && is_saving_check_break_key()) {
		key_codes_check_break_key[key_codes_saved] = key;
		key_codes_saved++;
	}
}
PRIVATE key_code_t get_key_codes_check_break_key()
{
	key_code_t key = KEY_NONE;
	if (is_restoring_check_break_key()) {
		key = key_codes_check_break_key[key_codes_restored];
		key_codes_restored++;
		if (key_codes_restored >= key_codes_saved) {
			key_codes_saved = -1;		// clear saved keys
			key_codes_restored = -1;	// stop restoration
		}
	}
	return key;
}
void begin_check_break_key()
{
	key_codes_saved = 0;		// start saving
	key_codes_restored = -1;	// stop restoration
	clear_sigint_signaled();
}
void end_check_break_key()
{
	if (key_codes_saved > 0) {
		key_codes_restored = 0;		// start restoring
	} else {
		key_codes_saved = -1;		// clear saved keys
		key_codes_restored = -1;	// stop restoration
	}
}
int check_break_key()
{
	if (is_saving_check_break_key()) {
		if (input_key_check_break_key() == K_C_c) {	// Ctrl-C key is the break key
			set_sigint_signaled();
flf_dprintf("sigint_signaled\n");
		}
	}
	return is_sigint_signaled();
}
//------------------------------------------------------------------------------

PRIVATE void redraw_1s_after_key_input(key_code_t key)
{
	static key_code_t prev_key = KEY_NONE;
	if (key < 0 && prev_key >= 0) {
		tio_redraw();
	}
	prev_key = key;
}
//------------------------------------------------------------------------------

///
#define WHOLE_UPDATE_INTERVAL_MSEC		60000	// 60[Sec]
///#define WHOLE_UPDATE_INTERVAL_MSEC		10000	// 10[Sec]
PRIVATE unsigned long msec_at_key_input = 0;
void clear_msec_past_after_key_input()
{
	msec_at_key_input = get_msec();
}
unsigned long get_msec_past_after_key_input()
{
	return get_msec() - msec_at_key_input;
}
PRIVATE void repaint_screen_after_60s(key_code_t key)
{
	if (key >= 0) {
		if (get_msec_past_after_key_input() >= WHOLE_UPDATE_INTERVAL_MSEC) {
			tio_repaint();
		}
		clear_msec_past_after_key_input();
	}
}
//------------------------------------------------------------------------------

PRIVATE int check_terminal_resized__update()
{
	if (tio_check_update_terminal_size()) {
		win_reinit_win_size();
		disp_status_bar_async(_("Screen resized to (%d, %d)"),
		 tio_get_columns(), tio_get_lines());
		update_screen_app(S_B_NONE, 1);
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------

// | --          |     To         |
// | From        |Backspace|Delete|
// |-------------|---------|------|
// |0x08         |    O    |  -   |
// |0x7f         |    O    |  O   |
// |KEY_BACKSPACE|    O    |  -   |
// |0x0113       |    -    |  O   |
// |KEY_DC       |    -    |  O   |

key_code_t map_key_code(key_code_t key)
{
	switch (key) {
	case KEY_BACKSPACE:		// 0x0107(only curses uses this ID)
		CLR_APPMD(app_MAP_KEY_7F_BS);	// set conversion of CHAR_DEL ==> BS
		key = AK_BS;		// KEY_BACKSPACE ==> BS
		break;
	case KEY_DC:			// 0x014a(only curses uses this ID)
		SET_APPMD(app_MAP_KEY_7F_BS);	// set conversion of CHAR_DEL ==> BS
		key = AK_DEL;
		break;
	case CHAR_DEL:			// 0x007f
		if (GET_APPMD(app_MAP_KEY_7F_BS)) {
			flf_dprintf("KEY_7F ==> BS\n");
			key = AK_BS;	// CHAR_DEL ==> BS
		} else {
			flf_dprintf("KEY_7F ==> DEL\n");
			key = AK_DEL;	// CHAR_DEL ==> Delete
		}
		break;
	default:
		break;
	}
	return key;
}

//------------------------------------------------------------------------------
key_name_table_t key_name_table[] = {
//							   1234567
	{ K_C_AT	, "^@", },
	{ K_C_a		, "^A", },
	{ K_C_b		, "^B", },
	{ K_C_c		, "^C", },
	{ K_C_d		, "^D", },
	{ K_C_e		, "^E", },
	{ K_C_f		, "^F", },
	{ K_C_g		, "^G", },
	{ K_C_h		, "^H", },
	{ K_TAB		, "TAB", },
	{ K_C_j		, "^J", },
	{ K_C_k		, "^K", },
	{ K_C_l		, "^L", },
	{ K_ENTER	, "ENTER", },
	{ K_C_n		, "^N", },
	{ K_C_o		, "^O", },
	{ K_C_p		, "^P", },
	{ K_C_q		, "^Q", },
	{ K_C_r		, "^R", },
	{ K_C_s		, "^S", },
	{ K_C_t		, "^T", },
	{ K_C_u		, "^U", },
	{ K_C_v		, "^V", },
	{ K_C_w		, "^W", },
	{ K_C_x		, "^X", },
	{ K_C_y		, "^Y", },
	{ K_C_z		, "^Z", },
	{ K_ESC		, "ESC", },		// 0x1b
	{ K_C_BAKSL	, "^\\", },		// 0x1c
	{ K_C_RBRAK	, "^]", },		// 0x1d
	{ K_C_CARET	, "^^", },		// 0x1e
	{ K_C_UNDLN	, "^_", },		// 0x1f

	{ K_SP		, "SP", },		// 0x20

	{ CHAR_DEL	, "CDEL", },
	{ K_BS		, "K_BS", },
///	{ K_ENTER	, "ENTER", },
	{ K_DEL		, "K_DEL", },
	{ K_INS		, "INS", },

	{ K_UP		, "UP", },
	{ K_DOWN	, "DOWN", },
	{ K_RIGHT	, "RIGHT", },
	{ K_LEFT	, "LEFT", },
	{ K_PPAGE	, "PGUP", },
	{ K_NPAGE	, "PGDN", },
	{ K_HOME	, "HOME", },
	{ K_END		, "END", },
	{ K_RESIZE	, "RESIZE", },

	{ K_S___UP		, "$UP", },
	{ K_S___DOWN	, "$DOWN", },
	{ K_S___RIGHT	, "$RIGHT", },
	{ K_S___LEFT	, "$LEFT", },
	{ K_S___PPAGE	, "$PGUP", },
	{ K_S___NPAGE	, "$PGDN", },
	{ K_S___HOME	, "$HOME", },
	{ K_S___END		, "$END", },
	{ K__C__UP		, "^UP", },
	{ K__C__DOWN	, "^DOWN", },
	{ K__C__RIGHT	, "^RIGHT", },
	{ K__C__LEFT	, "^LEFT", },
	{ K__C__PPAGE	, "^PGUP", },
	{ K__C__NPAGE	, "^PGDN", },
	{ K__C__HOME	, "^HOME", },
	{ K__C__END		, "^END", },
	{ K___M_UP		, "@UP", },
	{ K___M_DOWN	, "@DOWN", },
	{ K___M_RIGHT	, "@RIGHT", },
	{ K___M_LEFT	, "@LEFT", },
	{ K___M_PPAGE	, "@PGUP", },
	{ K___M_NPAGE	, "@PGDN", },
	{ K___M_HOME	, "@HOME", },
	{ K___M_END		, "@END", },
	{ K_SC__UP		, "^$UP", },
	{ K_SC__DOWN	, "^$DOWN", },
	{ K_SC__RIGHT	, "^$RIGHT", },
	{ K_SC__LEFT	, "^$LEFT", },
	{ K_SC__PPAGE	, "^$PGUP", },
	{ K_SC__NPAGE	, "^$PGDN", },
	{ K_SC__HOME	, "^$HOME", },
	{ K_SC__END		, "^$END", },
	{ K__CM_UP		, "@^UP", },
	{ K__CM_DOWN	, "@^DOWN", },
	{ K__CM_RIGHT	, "@^RIGHT", },
	{ K__CM_LEFT	, "@^LEFT", },
	{ K__CM_PPAGE	, "@^PGUP", },
	{ K__CM_NPAGE	, "@^PGDN", },
	{ K__CM_HOME	, "@^HOME", },
	{ K__CM_END		, "@^END", },
	{ K_S_M_UP		, "@$UP", },
	{ K_S_M_DOWN	, "@$DOWN", },
	{ K_S_M_RIGHT	, "@$RIGHT", },
	{ K_S_M_LEFT	, "@$LEFT", },
	{ K_S_M_PPAGE	, "@$PGUP", },
	{ K_S_M_NPAGE	, "@$PGDN", },
	{ K_S_M_HOME	, "@$HOME", },
	{ K_S_M_END		, "@$END", },
	{ K_SCM_UP		, "@^$UP", },
	{ K_SCM_DOWN	, "@^$DOWN", },
	{ K_SCM_RIGHT	, "@^$RIGHT", },
	{ K_SCM_LEFT	, "@^$LEFT", },
	{ K_SCM_PPAGE	, "@^$PGUP", },
	{ K_SCM_NPAGE	, "@^$PGDN", },
	{ K_SCM_HOME	, "@^$HOME", },
	{ K_SCM_END		, "@^$END", },

	{ K_MC_AT	, "@^@", },
	{ K_MC_a	, "@^A", },
	{ K_MC_b	, "@^B", },
	{ K_MC_c	, "@^C", },
	{ K_MC_d	, "@^D", },
	{ K_MC_e	, "@^E", },
	{ K_MC_f	, "@^F", },
	{ K_MC_g	, "@^G", },
	{ K_MC_h	, "@^H", },
	{ K_MC_i	, "@^I", },
	{ K_MC_j	, "@^J", },
	{ K_MC_k	, "@^K", },
	{ K_MC_l	, "@^L", },
	{ K_MC_m	, "@^M", },
	{ K_MC_n	, "@^N", },
	{ K_MC_o	, "@^O", },
	{ K_MC_p	, "@^P", },
	{ K_MC_q	, "@^Q", },
	{ K_MC_r	, "@^R", },
	{ K_MC_s	, "@^S", },
	{ K_MC_t	, "@^T", },
	{ K_MC_u	, "@^U", },
	{ K_MC_v	, "@^V", },
	{ K_MC_w	, "@^W", },
	{ K_MC_x	, "@^X", },
	{ K_MC_y	, "@^Y", },
	{ K_MC_z	, "@^Z", },
	{ K_M_ESC	, "@ESC", },
	{ K_MC_BAKSL, "@^\\", },
	{ K_MC_RBRAK, "@^]", },
	{ K_MC_CARET, "@^^", },
	{ K_MC_UNDLN, "@^_", },

	{ K_M_SP	, "@SP", },
	{ K_M_EXCLA	, "@!", },
	{ K_M_QUOTA	, "@\"", },
	{ K_M_SHARP	, "@#", },
	{ K_M_DOLLA	, "@$", },
	{ K_M_PERCE	, "@%", },
	{ K_M_AMPSD	, "@&", },
	{ K_M_APOST	, "@'", },
	{ K_M_LPARE	, "@(", },
	{ K_M_RPARE	, "@)", },
	{ K_M_ASTER	, "@*", },
	{ K_M_PLUS	, "@+", },
	{ K_M_COMMA	, "@,", },
	{ K_M_HYPHE	, "@-", },
	{ K_M_PERIO	, "@.", },
	{ K_M_SLASH	, "@/", },

	{ K_M_0		, "@0", },
	{ K_M_1		, "@1", },
	{ K_M_2		, "@2", },
	{ K_M_3		, "@3", },
	{ K_M_4		, "@4", },
	{ K_M_5		, "@5", },
	{ K_M_6		, "@6", },
	{ K_M_7		, "@7", },
	{ K_M_8		, "@8", },
	{ K_M_9		, "@9", },
	{ K_M_COLON	, "@:", },
	{ K_M_SEMCO	, "@;", },
	{ K_M_LESST	, "@<", },
	{ K_M_EQUAL	, "@=", },
	{ K_M_GREAT	, "@>", },
	{ K_M_QUEST	, "@?", },

	{ K_M_AT	, "@@", },
	{ K_M_A		, "@A", },
	{ K_M_B		, "@B", },
	{ K_M_C		, "@C", },
	{ K_M_D		, "@D", },
	{ K_M_E		, "@E", },
	{ K_M_F		, "@F", },
	{ K_M_G		, "@G", },
	{ K_M_H		, "@H", },
	{ K_M_I		, "@I", },
	{ K_M_J		, "@J", },
	{ K_M_K		, "@K", },
	{ K_M_L		, "@L", },
	{ K_M_M		, "@M", },
	{ K_M_N		, "@N", },
	{ K_M_O		, "@O", },
	{ K_M_P		, "@P", },
	{ K_M_Q		, "@Q", },
	{ K_M_R		, "@R", },
	{ K_M_S		, "@S", },
	{ K_M_T		, "@T", },
	{ K_M_U		, "@U", },
	{ K_M_V		, "@V", },
	{ K_M_W		, "@W", },
	{ K_M_X		, "@X", },
	{ K_M_Y		, "@Y", },
	{ K_M_Z		, "@Z", },
	{ K_M_LBRAK	, "@[", },
	{ K_M_BAKSL	, "@\\", },
	{ K_M_RBRAK	, "@]", },
	{ K_M_CARET	, "@^", },
	{ K_M_UNDLN	, "@_", },

	{ K_M_BAKQT	, "@`", },
	{ K_M_a		, "@a", },
	{ K_M_b		, "@b", },
	{ K_M_c		, "@c", },
	{ K_M_d		, "@d", },
	{ K_M_e		, "@e", },
	{ K_M_f		, "@f", },
	{ K_M_g		, "@g", },
	{ K_M_h		, "@h", },
	{ K_M_i		, "@i", },
	{ K_M_j		, "@j", },
	{ K_M_k		, "@k", },
	{ K_M_l		, "@l", },
	{ K_M_m		, "@m", },
	{ K_M_n		, "@n", },
	{ K_M_o		, "@o", },
	{ K_M_p		, "@p", },
	{ K_M_q		, "@q", },
	{ K_M_r		, "@r", },
	{ K_M_s		, "@s", },
	{ K_M_t		, "@t", },
	{ K_M_u		, "@u", },
	{ K_M_v		, "@v", },
	{ K_M_w		, "@w", },
	{ K_M_x		, "@x", },
	{ K_M_y		, "@y", },
	{ K_M_z		, "@z", },
	{ K_M_LBRAC	, "@{", },
	{ K_M_VERTB	, "@|", },
	{ K_M_RBRAC	, "@}", },
	{ K_M_TILDE	, "@~", },
	{ K_M_CDEL	, "@CDEL", },

	{ AK_BS		, "BS", },
	{ AK_DEL	, "DEL", },
	{ AK_M_BS	, "@BS", },
	{ AK_M_DEL	, "@DEL", },

	{ K_M_TAB	, "@TAB", },
	{ K_M_ENTER	, "@ENTER", },

	{ K_F01		, "F01", },
	{ K_F02		, "F02", },
	{ K_F03		, "F03", },
	{ K_F04		, "F04", },
	{ K_F05		, "F05", },
	{ K_F06		, "F06", },
	{ K_F07		, "F07", },
	{ K_F08		, "F08", },
	{ K_F09		, "F09", },
	{ K_F10		, "F10", },
	{ K_F11		, "F11", },
	{ K_F12		, "F12", },
	{ K_S_F01	, "$F01", },
	{ K_S_F02	, "$F02", },
	{ K_S_F03	, "$F03", },
	{ K_S_F04	, "$F04", },
	{ K_S_F05	, "$F05", },
	{ K_S_F06	, "$F06", },
	{ K_S_F07	, "$F07", },
	{ K_S_F08	, "$F08", },
	{ K_S_F09	, "$F09", },
	{ K_S_F10	, "$F10", },
	{ K_S_F11	, "$F11", },
	{ K_S_F12	, "$F12", },
};

// 0x01 ==> "^A"
const char *key_name_from_key_code(key_code_t key_code)
{
	for (int key_idx = 0; key_idx < get_key_name_table_entries(); key_idx++) {
		if (key_name_table[key_idx].key_code == key_code) {
			return key_name_table[key_idx].key_name;
		}
	}
	return "";
}
// "^A" ==> 0x01
key_code_t key_code_from_key_name(const char *key_name)
{
	for (int key_idx = 0; key_idx < get_key_name_table_entries(); key_idx++) {
		if (strcmp(key_name_table[key_idx].key_name, key_name) == 0) {
			return key_name_table[key_idx].key_code;
		}
	}
	return K_NONE;
}

// 0x0020 <==> " "
// 0x0080 <==> "\x80"
// 0x0028 <==> "(("
// 0x0029 <==> "))"
// 0x007b <==> "{{"
// 0x007d <==> "}}"
// 0x1b20 <==> "{@SP}"
// 0x1b28 <==> "{@(}"
// 0x1b29 <==> "{@)}"
// 0x1b7b <==> "(@{)"
// 0x1b7d <==> "(@})"
const char *key_str_from_key_code(key_code_t key_code)
{
	static char buf_s_[MAX_KEY_STR_LEN+1];

	if (key_code == '(') {
		snprintf(buf_s_, MAX_KEY_STR_LEN+1, "((");
	} else
	if (key_code == ')') {
		snprintf(buf_s_, MAX_KEY_STR_LEN+1, "))");
	} else
	if (key_code == '{') {
		snprintf(buf_s_, MAX_KEY_STR_LEN+1, "{{");
	} else
	if (key_code == '}') {
		snprintf(buf_s_, MAX_KEY_STR_LEN+1, "}}");
	} else
	if (is_key_print(key_code)) {
		snprintf(buf_s_, MAX_KEY_STR_LEN+1, "%c", (UINT16)key_code);
	} else {
		const char *str = get_key_name_from_key_code(key_code, NULL);
		if (contain_chrs(str, "()") == 0) {
			// "(UP)", "(@{)", "(@})", "(ffff)"
			snprintf(buf_s_, MAX_KEY_STR_LEN+1, "(%s)", str);
		} else {
			// "{@(}", "{@)}"
			snprintf(buf_s_, MAX_KEY_STR_LEN+1, "{%s}", str);
		}
	}
	return buf_s_;
}

int key_code_from_key_str(const char *str, key_code_t* key_code)
{
	*key_code = K_NONE;
	if (strlcmp__(str, "((") == 0) {
		*key_code = '(';
		return 2;
	} else
	if (strlcmp__(str, "))") == 0) {
		*key_code = ')';
		return 2;
	} else
	if (strlcmp__(str, "{{") == 0) {
		*key_code = '{';
		return 2;
	} else
	if (strlcmp__(str, "}}") == 0) {
		*key_code = '}';
		return 2;
	} else
	if ((str[0] == '(') || (str[0] == '{')) /*}*/ {		// "(RIGHT)"
		char end_chr = (str[0] == '(') ? ')' : /*{*/ '}';
		for (int len = 1; (len < (1+MAX_KEY_STR_LEN)) && str[len]; len++) {
			if (str[len] == end_chr) {
				char key_name[MAX_KEY_NAME_LEN+1];
				strlcpy__(key_name, &str[1], len-1);	// "RIGHT"
				*key_code = get_key_code_from_key_name(key_name);
				if (IS_KEY_VALID(*key_code)) {
					return len + 1;
				}
			}
		}
		return 1;		// No end mark found
	} else
	if (is_key_print((UINT8)str[0])) {
		*key_code = (UINT8)str[0];		// "A", "\x80" ~ "\xff"
		return 1;
	}
	*key_code = (UINT8)str[0];
	return 1;
}

const char *key_none_str()
{
	static char buf_s_[MAX_KEY_NAME_LEN+1];
	return strnset__(buf_s_, '-', MAX_KEY_NAME_LEN);	// "--------"
}
// "ESC", "SP", "!", ... "0", "@", "漢", "^@", "@@", "@^@", "%ffff"
const char *get_key_name_from_key_code__(key_code_t key_code, char *buf);
const char *get_key_name_from_key_code(key_code_t key_code, char *buf)
{
	const char *str = get_key_name_from_key_code__(key_code, buf);
/////flf_dprintf("%04x ==> [%s]\n", key_code, str);
	return str;
}
const char *get_key_name_from_key_code__(key_code_t key_code, char *buf)
{
	static char buf_s_[MAX_KEY_NAME_LEN+1];
	if (buf == NULL) {
		buf = buf_s_;
	}
	strcpy__(buf, "");
	if (IS_KEY_INVALID(key_code)) {
		return key_none_str();
	}
	const char *key_name = key_name_from_key_code(key_code);
	if (is_strlen_not_0(key_name)) {
		return key_name;
	}
	if (is_key_graph(key_code) || is_key_utf8_byte(key_code)) {
		snprintf(buf, MAX_KEY_NAME_LEN+1, "%c", key_code);
	} else
	if (IS_META_KEY(key_code)) {	// 0x1bxx
		unsigned char chr = KEY_LOW_BYTE(key_code);
		if (isgraph(chr)) {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "@%c", chr);
		} else if (0 <= chr && chr < 0x20) {
			snprintf(buf, MAX_KEY_NAME_LEN+1, "@^%c", '@' + chr);
		}
	}
	if (is_strlen_not_0(buf)) {
		return buf;
	}
	snprintf(buf, MAX_KEY_NAME_LEN+1, "%%%04x", key_code);	// "%ffff"
	return buf;
}
key_code_t get_key_code_from_key_name(const char *key_name)
{
	int len = strlen_path(key_name);
	if (len >= 1) {
		if (len == 1) {
			return key_name[0];
		}
		key_code_t key_code = key_code_from_key_name(key_name);
		if (IS_KEY_VALID(key_code)) {
			return key_code;
		}
		if (key_name[0] == '%') {
			// "%abcd"
			int int_key_code;
			if (sscanf(&key_name[1], "%x", &int_key_code) > 0) {
				return (key_code_t)int_key_code;
			}
		}
	}
	return KEY_NONE;
}

int get_key_name_table_entries()
{
	return ARRAY_SIZE_OF(key_name_table);
}

//------------------------------------------------------------------------------
// [0x00, 0x1f], 0x7f
int is_key_ctrl(key_code_t key)
{
	return (key < 0x0020) || (key == 0x007f);
}
// [0x20, 0x7e]
int is_key_graph(key_code_t key)
{
	return (0x0020 <= key) && (key < 0x007f);
}
// [0x80, 0xff]
int is_key_utf8_byte(key_code_t key)
{
	return ((0x0080 <= key) && (key < 0x0100));
}

int is_key_print(key_code_t key)
{
	return (key == ' ') || is_key_graph(key) || is_key_utf8_byte(key);
}
int is_key_input(key_code_t key)
{
	return ((key) != K_RESIZE) && ((key) != K_NONE);
}

#ifdef START_UP_TEST
int key_code_from_key_str__len(const char *str)
{
	key_code_t key_code;
	return key_code_from_key_str(str, &key_code);
}
int key_code_from_key_str__code(const char *str)
{
	key_code_t key_code;
	key_code_from_key_str(str, &key_code);
	return key_code;
}

void test_conversion_key_name__key_code()
{
_FLF_
	MY_UT_STR(get_key_name_from_key_code(K_a_			, NULL), "a");
	MY_UT_STR(get_key_name_from_key_code(K_A_			, NULL), "A");
	MY_UT_STR(get_key_name_from_key_code(K_C_a			, NULL), "^A");
	MY_UT_STR(get_key_name_from_key_code(K_M_a			, NULL), "@a");
	MY_UT_STR(get_key_name_from_key_code(K_M_A			, NULL), "@A");
	MY_UT_STR(get_key_name_from_key_code(K_MC_a			, NULL), "@^A");
	MY_UT_STR(get_key_name_from_key_code(0x1234			, NULL), "%1234");

	MY_UT_STR(key_str_from_key_code(K_a_)			, "a");
	MY_UT_STR(key_str_from_key_code(K_A_)			, "A");
	MY_UT_STR(key_str_from_key_code(K_C_a)			, "(^A)");
	MY_UT_STR(key_str_from_key_code(K_M_a)			, "(@a)");
	MY_UT_STR(key_str_from_key_code(K_M_A)			, "(@A)");
	MY_UT_STR(key_str_from_key_code(K_MC_a)			, "(@^A)");
	MY_UT_STR(key_str_from_key_code(K_ESC)			, "(ESC)");
	MY_UT_STR(key_str_from_key_code(K_C_BAKSL)		, "(^\\)");
	MY_UT_STR(key_str_from_key_code(K_C_RBRAK)		, "(^])");
	MY_UT_STR(key_str_from_key_code(K_C_CARET)		, "(^^)");
	MY_UT_STR(key_str_from_key_code(K_C_UNDLN)		, "(^_)");
	MY_UT_STR(key_str_from_key_code(K_M_LBRAK)		, "(@[)");
	MY_UT_STR(key_str_from_key_code(K_M_BAKSL)		, "(@\\)");
	MY_UT_STR(key_str_from_key_code(K_M_RBRAK)		, "(@])");
	MY_UT_STR(key_str_from_key_code(K_M_CARET)		, "(@^)");
	MY_UT_STR(key_str_from_key_code(K_M_UNDLN)		, "(@_)");
	MY_UT_STR(key_str_from_key_code(K_M_ESC)		, "(@ESC)");
	MY_UT_STR(key_str_from_key_code(K_MC_BAKSL)		, "(@^\\)");
	MY_UT_STR(key_str_from_key_code(K_MC_RBRAK)		, "(@^])");
	MY_UT_STR(key_str_from_key_code(K_MC_CARET)		, "(@^^)");
	MY_UT_STR(key_str_from_key_code(K_MC_UNDLN)		, "(@^_)");
	MY_UT_STR(key_str_from_key_code(0x1234)			, "(%1234)");

	MY_UT_INT(key_code_from_key_str__len("a")		, 1);
	MY_UT_INT(key_code_from_key_str__code("a")		, K_a_);
	MY_UT_INT(key_code_from_key_str__len("A")		, 1);
	MY_UT_INT(key_code_from_key_str__code("A")		, K_A_);
	MY_UT_INT(key_code_from_key_str__len("(^A)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(^A)")	, K_C_a);
	MY_UT_INT(key_code_from_key_str__len("(@a)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@a)")	, K_M_a);
	MY_UT_INT(key_code_from_key_str__len("(@A)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@A)")	, K_M_A);
	MY_UT_INT(key_code_from_key_str__len("(@^A)")	, 5);
	MY_UT_INT(key_code_from_key_str__code("(@^A)")	, K_MC_a);
	MY_UT_INT(key_code_from_key_str__len("(ESC)")	, 5);
	MY_UT_INT(key_code_from_key_str__code("(ESC)")	, K_ESC);
	MY_UT_INT(key_code_from_key_str__len("(^\\)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(^\\)")	, K_C_BAKSL);
	MY_UT_INT(key_code_from_key_str__len("(^])")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(^])")	, K_C_RBRAK);
	MY_UT_INT(key_code_from_key_str__len("(^^)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(^^)")	, K_C_CARET);
	MY_UT_INT(key_code_from_key_str__len("(^_)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(^_)")	, K_C_UNDLN);
	MY_UT_INT(key_code_from_key_str__len("(@[)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@[)")	, K_M_LBRAK);
	MY_UT_INT(key_code_from_key_str__len("(@\\)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@\\)")	, K_M_BAKSL);
	MY_UT_INT(key_code_from_key_str__len("(@])")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@])")	, K_M_RBRAK);
	MY_UT_INT(key_code_from_key_str__len("(@^)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@^)")	, K_M_CARET);
	MY_UT_INT(key_code_from_key_str__len("(@_)")	, 4);
	MY_UT_INT(key_code_from_key_str__code("(@_)")	, K_M_UNDLN);
	MY_UT_INT(key_code_from_key_str__len("(@ESC)")	, 6);
	MY_UT_INT(key_code_from_key_str__code("(@ESC)")	, K_M_ESC);
	MY_UT_INT(key_code_from_key_str__len("(@^\\)")	, 5);
	MY_UT_INT(key_code_from_key_str__code("(@^\\)")	, K_MC_BAKSL);
	MY_UT_INT(key_code_from_key_str__len("(@^])")	, 5);
	MY_UT_INT(key_code_from_key_str__code("(@^])")	, K_MC_RBRAK);
	MY_UT_INT(key_code_from_key_str__len("(@^^)")	, 5);
	MY_UT_INT(key_code_from_key_str__code("(@^^)")	, K_MC_CARET);
	MY_UT_INT(key_code_from_key_str__len("(@^_)")	, 5);
	MY_UT_INT(key_code_from_key_str__code("(@^_)")	, K_MC_UNDLN);
	MY_UT_INT(key_code_from_key_str__len("(%1234)")	, 7);
	MY_UT_INT(key_code_from_key_str__code("(%1234)"), 0x1234);
}
#endif // START_UP_TEST

// End of keys.c
