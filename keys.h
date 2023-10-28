/**************************************************************************
 *   keys.h                                                               *
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

#ifndef keys_h
#define keys_h

// HP-UX 10 & 11 do not seem to support KEY_HOME and KEY_END
#if !defined(KEY_HOME) || !defined(KEY_END)
#define KEY_HOME		0x0106
#define KEY_END			0x0168
#endif // !defined(KEY_HOME) || !defined(KEY_END)

// Snatch these out of the ncurses defs, so we can use them
#if !defined(KEY_UP) || !defined(KEY_DOWN)
#define KEY_UP			0x0103
#define KEY_DOWN		0x0102
#endif // !defined(KEY_UP) || !defined(KEY_DOWN)

#define CTRL_CHAR(chr)		((chr) - '@')				// Ctrl-x
#define CHAR_ESC			0x1b
#define TWO_BYTE_KEY_CODE(h, l)	((h) << 8 | (l))
#define TBKC(h, l)			TWO_BYTE_KEY_CODE(h, l)
#define KEY_META(key)		TBKC(CHAR_ESC, key)			// Alt-x
#define KEY_META_CTRL(chr)	KEY_META(CTRL_CHAR(chr))	// Alt-Ctrl-x

#define K_C(chr)			CTRL_CHAR(chr)				// Ctrl-x
#define K_M(key)			KEY_META(key)				// Alt-x
#define K_MC(chr)			KEY_META_CTRL(chr)			// Alt-Ctrl-x

#define K_NONE			-1

#define K_C_AT			K_C('@')
#define K_C_SP			K_C('@')
#define K_C_A			K_C('A')
#define K_C_B			K_C('B')
#define K_C_C			K_C('C')
#define K_C_D			K_C('D')
#define K_C_E			K_C('E')
#define K_C_F			K_C('F')
#define K_C_G			K_C('G')
#define K_C_H			K_C('H')
#define K_C_I			K_C('I')
#define K_TAB			K_C_I		// 09
#define K_C_J			K_C('J')
#define K_C_K			K_C('K')
#define K_C_L			K_C('L')
#define K_C_M			K_C('M')
#define K_ENTER			KEY_ENTER	// 0d
#define K_C_N			K_C('N')
#define K_C_O			K_C('O')
#define K_C_P			K_C('P')
#define K_C_Q			K_C('Q')
#define K_C_R			K_C('R')
#define K_C_S			K_C('S')
#define K_C_T			K_C('T')
#define K_C_U			K_C('U')
#define K_C_V			K_C('V')
#define K_C_W			K_C('W')
#define K_C_X			K_C('X')
#define K_C_Y			K_C('Y')
#define K_C_Z			K_C('Z')
#define K_C_LBRACKET	K_C('[')	// 1b
#define K_ESC			CHAR_ESC	// 1b
#define K_C_BACKSLASH	K_C('\\')	// 1c
#define K_C_RBRACKET	K_C(']')	// 1d
#define K_C_CARET		K_C('^')	// 1e
#define K_C_UNDERLINE	K_C('_')	// 1f

#define K_M_0			K_M('0')
#define K_M_1			K_M('1')
#define K_M_2			K_M('2')
#define K_M_3			K_M('3')
#define K_M_4			K_M('4')
#define K_M_5			K_M('5')
#define K_M_6			K_M('6')
#define K_M_7			K_M('7')
#define K_M_8			K_M('8')
#define K_M_9			K_M('9')
#define K_M_a			K_M('a')
#define K_M_b			K_M('b')
#define K_M_c			K_M('c')
#define K_M_d			K_M('d')
#define K_M_e			K_M('e')
#define K_M_f			K_M('f')
#define K_M_g			K_M('g')
#define K_M_h			K_M('h')
#define K_M_i			K_M('i')
#define K_M_j			K_M('j')
#define K_M_k			K_M('k')
#define K_M_l			K_M('l')
#define K_M_m			K_M('m')
#define K_M_n			K_M('n')
#define K_M_o			K_M('o')
#define K_M_p			K_M('p')
#define K_M_q			K_M('q')
#define K_M_r			K_M('r')
#define K_M_s			K_M('s')
#define K_M_t			K_M('t')
#define K_M_u			K_M('u')
#define K_M_v			K_M('v')
#define K_M_w			K_M('w')
#define K_M_x			K_M('x')
#define K_M_y			K_M('y')
#define K_M_z			K_M('z')
#define K_M_A			K_M('A')
#define K_M_B			K_M('B')
#define K_M_C			K_M('C')
#define K_M_D			K_M('D')
#define K_M_E			K_M('E')
#define K_M_F			K_M('F')
#define K_M_G			K_M('G')
#define K_M_H			K_M('H')
#define K_M_I			K_M('I')
#define K_M_J			K_M('J')
#define K_M_K			K_M('K')
#define K_M_L			K_M('L')
#define K_M_M			K_M('M')
#define K_M_N			K_M('N')
#define K_M_O			K_M('O')
#define K_M_P			K_M('P')
#define K_M_Q			K_M('Q')
#define K_M_R			K_M('R')
#define K_M_S			K_M('S')
#define K_M_T			K_M('T')
#define K_M_U			K_M('U')
#define K_M_V			K_M('V')
#define K_M_W			K_M('W')
#define K_M_X			K_M('X')
#define K_M_Y			K_M('Y')
#define K_M_Z			K_M('Z')
#define K_M_BS			K_M(K_C_H)		// 1b08
#define K_M_TAB			K_M(K_TAB)		// 1b09
#define K_M_CR			K_M(K_C_M)		// 1b0d
#define K_M_ESC			K_M(CHAR_ESC)	// 1b1b
#define K_M_SP			K_M(' ')		// 1b20
#define K_M_EXCLAMATION		K_M('!')	// 1b21
#define K_M_QUOTATION		K_M('"')	// 1b22
#define K_M_SHARP			K_M('#')	// 1b23
#define K_M_DOLLAR			K_M('$')	// 1b24
#define K_M_PERCENT			K_M('%')	// 1b25
#define K_M_AMPERSAND		K_M('&')	// 1b26
#define K_M_APOSTROPHE		K_M('\'')	// 1b27
#define K_M_LPARENTHESIS	K_M('(')	// 1b28
#define K_M_RPARENTHESIS	K_M(')')	// 1b29
#define K_M_ASTERISK		K_M('*')	// 1b2a
#define K_M_PLUS			K_M('+')	// 1b2b
#define K_M_COMMA			K_M(',')	// 1b2c
#define K_M_HYPHEN			K_M('-')	// 1b2d
#define K_M_PERIOD			K_M('.')	// 1b2e
#define K_M_SLASH			K_M('/')	// 1b2f
#define K_M_COLON			K_M(':')	// 1b3a
#define K_M_SEMICOLON		K_M(';')	// 1b3b
#define K_M_LESSTHAN		K_M('<')	// 1b3c
#define K_M_EQUAL			K_M('=')	// 1b3d
#define K_M_GREATERTHAN		K_M('>')	// 1b3e
#define K_M_QUESTION		K_M('?')	// 1b3f
#define K_M_AT				K_M('@')	// 1b40
#define K_M_LBRACKET		K_M('[')	// 1b5b
#define K_M_BACKSLASH		K_M('\\')	// 1b5c
#define K_M_RBRACKET		K_M(']')	// 1b5d
#define K_M_CARET			K_M('^')	// 1b5e
#define K_M_UNDERLINE		K_M('_')	// 1b5f
#define K_M_BACKQUOTE		K_M('`')	// 1b60
#define K_M_LBRACE			K_M('{')	// 1b7b
#define K_M_VERTBAR			K_M('|')	// 1b7c
#define K_M_RBRACE			K_M('}')	// 1b7d
#define K_M_TILDE			K_M('~')	// 1b7e
#define K_M_DEL				K_M(0x7f)	// 1b7f

#define K_MC_LBRACKET		K_M(K_C('['))	// 1b1b
#define K_MC_BACKSLASH		K_M(K_C('\\'))	// 1b1c
#define K_MC_RBRACKET		K_M(K_C(']'))	// 1b1d
#define K_MC_CARET			K_M(K_C('^'))	// 1b1e
#define K_MC_UNDERLINE		K_M(K_C('_'))	// 1b1f

#define K_MC_A			K_M(K_C('A'))	// 1b01
#define K_MC_B			K_M(K_C('B'))
#define K_MC_C			K_M(K_C('C'))
#define K_MC_D			K_M(K_C('D'))
#define K_MC_E			K_M(K_C('E'))
#define K_MC_F			K_M(K_C('F'))
#define K_MC_G			K_M(K_C('G'))
#define K_MC_H			K_M(K_C('H'))
#define K_MC_I			K_M(K_C('I'))
#define K_MC_J			K_M(K_C('J'))
#define K_MC_K			K_M(K_C('K'))
#define K_MC_L			K_M(K_C('L'))
#define K_MC_M			K_M(K_C('M'))
#define K_MC_N			K_M(K_C('N'))
#define K_MC_O			K_M(K_C('O'))
#define K_MC_P			K_M(K_C('P'))
#define K_MC_Q			K_M(K_C('Q'))
#define K_MC_R			K_M(K_C('R'))
#define K_MC_S			K_M(K_C('S'))
#define K_MC_T			K_M(K_C('T'))
#define K_MC_U			K_M(K_C('U'))
#define K_MC_V			K_M(K_C('V'))
#define K_MC_W			K_M(K_C('W'))
#define K_MC_X			K_M(K_C('X'))
#define K_MC_Y			K_M(K_C('Y'))
#define K_MC_Z			K_M(K_C('Z'))	// 1b1a

#define K_F01			KEY_F(1)
#define K_F02			KEY_F(2)
#define K_F03			KEY_F(3)
#define K_F04			KEY_F(4)
#define K_F05			KEY_F(5)
#define K_F06			KEY_F(6)
#define K_F07			KEY_F(7)
#define K_F08			KEY_F(8)
#define K_F09			KEY_F(9)
#define K_F10			KEY_F(10)
#define K_F11			KEY_F(11)
#define K_F12			KEY_F(12)
#define K_SF01			KEY_F(12+1)
#define K_SF02			KEY_F(12+2)
#define K_SF03			KEY_F(12+3)
#define K_SF04			KEY_F(12+4)
#define K_SF05			KEY_F(12+5)
#define K_SF06			KEY_F(12+6)
#define K_SF07			KEY_F(12+7)
#define K_SF08			KEY_F(12+8)
#define K_SF09			KEY_F(12+9)
#define K_SF10			KEY_F(12+10)
#define K_SF11			KEY_F(12+11)
#define K_SF12			KEY_F(12+12)

#define K_IC			KEY_IC
#define K_DC			KEY_DC
//							Key-Backspace	Key-Delete
// console(TL11Fuji)		0x08			0x7f
// console(Slackware12.2)	0x7f			0x0113
// console(Ubuntu11.04)		0x0107			0x014a
// konsole(TL11Fuji)		0x08			0x0113
// xterm(Ubuntu11.04)		0x0107			0x014a
// console(Ubuntu16.04)		0x08			0x014a
// MX Linux(21)				0x7f			1b,5b,33,7e
#define K_BS			0x1008
#define K_DEL			0x107f			// console: DEL, konsole: BS
#define K_HOME			KEY_HOME
#define K_END			KEY_END
#define K_PPAGE			KEY_PPAGE
#define K_NPAGE			KEY_NPAGE
#define K_UP			KEY_UP
#define K_DOWN			KEY_DOWN
#define K_LEFT			KEY_LEFT
#define K_RIGHT			KEY_RIGHT

#define S_C_AT		"\x00"
#define S_C_A		"\x01"
#define S_C_B		"\x02"
#define S_C_C		"\x03"
#define S_C_D		"\x04"
#define S_C_E		"\x05"
#define S_C_F		"\x06"
#define S_C_G		"\x07"
#define S_C_H		"\x08"
#define S_C_I		"\x09"
#define S_C_J		"\x0a"
#define S_C_K		"\x0b"
#define S_C_L		"\x0c"
#define S_C_M		"\x0d"
#define S_C_N		"\x0e"
#define S_C_O		"\x1f"
#define S_C_P		"\x10"
#define S_C_Q		"\x11"
#define S_C_R		"\x12"
#define S_C_S		"\x13"
#define S_C_T		"\x14"
#define S_C_U		"\x15"
#define S_C_V		"\x16"
#define S_C_W		"\x17"
#define S_C_X		"\x18"
#define S_C_Y		"\x19"
#define S_C_Z		"\x1a"

#define NO_KEY_ASSIGNED		(-0x8000)
#define NKA					NO_KEY_ASSIGNED

#define IS_CHAR_KEY(key)	(' ' <= (key) && (key) < 0x0100)

#define FUNC_ID(func)	func, #func
#define F_I(func)		FUNC_ID(func)

typedef struct {
	char *desc;			// short description
	char *help;			// explanation
	key_code_t key1;
	key_code_t key2;
	key_code_t key3;
	char list_mode;		// executable in list mode
	int (*func)(void);
	char *func_id;
	const char *(*func_get)(void);	// function to get assigned value
} func_key_table_t;

#define MAX_KEY_NAME_LEN	5
typedef struct /*key_name_table_t*/ {
	key_code_t key_code;
	char *key_name;
} key_name_table_t;
extern key_name_table_t key_name_table[];

void set_app_func_key_table(void);

void editor_menu_n(int group_idx);
void filer_menu_n(int group_idx);

int disp_drop_down_menu(int group_idx, int sel_idx, int yy, int xx);
int get_func_key_table_from_key_groups(void);
int get_func_key_table_from_key_entries(int group_idx);
short get_func_key_code(int group_idx, int entry_idx);
void exec_func(int group_idx, int entry_idx);
func_key_table_t *get_func_key_table_from_key_group(int group_idx);

int cmp_func_id(const char *func_id_1, const char *func_id_2);
void *get_app_function_for_key(key_code_t key);
const char *get_func_id_from_key(key_code_t key);
func_key_table_t *get_func_key_table_from_key(func_key_table_t *key_table, key_code_t key);
key_code_t get_key_for_func_id(char *func_id);
func_key_table_t *get_func_table_from_func_id(const char *func_id);

int is_key_bound_to_func(key_code_t key, func_key_table_t *func_key_table);
void clear_keys_if_bound(key_code_t *keys);
void clear_key_if_bound_to_func(key_code_t key, func_key_table_t *func_key_table);
void clear_keys_bound_to_func(func_key_table_t *func_key_table);
void bind_key_to_func(func_key_table_t *func_key_table, key_code_t *keys);

const char *short_key_name_from_func_id(char *buf);

void set_menu_key(key_code_t key);
key_code_t get_menu_key(void);
void set_menu_key_for_do_app_menu_0(void);

key_code_t input_key_loop(void);
key_code_t input_key_wait_return(void);

#endif // keys_h

// End of keys.h
