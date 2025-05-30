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

typedef short key_code_t;		// signed short

#define KEY_NONE			((key_code_t)(-1))
#define K_NONE				KEY_NONE
#define KEY_NOT_ASSIGNED	KEY_NONE
#define KNA					KEY_NOT_ASSIGNED
#define K_VALID				K_C_AT
#define IS_KEY_INVALID(key)	((key) == KEY_NONE)
#define IS_KEY_VALID(key)	(! IS_KEY_INVALID(key))

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
#define CHAR_ESC			0x1b						// ASCII ESC
#define CHAR_DEL			0x7f						// ASCII DEL
#define TWO_BYTE_KEY_CODE(h, l)	(((key_code_t)(h) << 8) | (l))
#define TBKC(h, l)			TWO_BYTE_KEY_CODE((h), (l))
#define KEY_META(key)		TWO_BYTE_KEY_CODE(CHAR_ESC, key)	// Alt-x
#define KEY_META_CTRL(chr)	KEY_META(CTRL_CHAR(chr))			// Alt-Ctrl-x
#define IS_BYTE_KEY(key)	(((key) & 0xff00) == 0x0000)
#define IS_WORD_KEY(key)	(((key) & 0xff00) != 0x0000)
#define IS_META_KEY(key)	(((key) & 0xff00) == KEY_META(0x00))
#define IS_LOWER_KEY(key)	((IS_BYTE_KEY(key) || IS_META_KEY(key)) && islower(key & 0xff))
#define IS_UPPER_KEY(key)	((IS_BYTE_KEY(key) || IS_META_KEY(key)) && isupper(key & 0xff))

// | func          | ^M | '0' | 'a' | 'A' |0x0101| @0  | @a  | @A  |
// |---------------|----|-----|-----|-----|------|-----|-----|-----|
// | IS_BYTE_KEY() |  1 |  1  |  1  |  1  |   0  |  0  |  0  |  0  |
// | IS_WORD_KEY() |  0 |  0  |  0  |  0  |   1  |  1  |  1  |  1  |
// | IS_META_KEY() |  0 |  0  |  0  |  0  |   0  |  1  |  1  |  1  |
// | IS_LOWER_KEY()|  0 |  0  |  1  |  0  |   0  |  0  |  1  |  0  |
// | IS_UPPER_KEY()|  0 |  0  |  0  |  1  |   0  |  0  |  0  |  1  |

#define K_(chr)				(chr)
#define K_C(chr)			CTRL_CHAR(chr)				// Ctrl-x
#define K_M(key)			KEY_META(key)				// Alt-x
#define K_MC(chr)			KEY_META_CTRL(chr)			// Alt-Ctrl-x

// Shift, Ctrl, Meta(Alt) modifier
#define K_S_MOD			0x0100
#define K_C_MOD			0x0200
#define K_M_MOD			0x0400

#define K_MOD_S__(key)		((key) + (K_S_MOD | 0       | 0      ))
#define K_MOD__C_(key)		((key) + (0       | K_C_MOD | 0      ))
#define K_MOD_SC_(key)		((key) + (K_S_MOD | K_C_MOD | 0      ))
#define K_MOD___M(key)		((key) + (0       | 0       | K_M_MOD))
#define K_MOD_S_M(key)		((key) + (K_S_MOD | 0       | K_M_MOD))
#define K_MOD__CM(key)		((key) + (0       | K_C_MOD | K_M_MOD))
#define K_MOD_SCM(key)		((key) + (K_S_MOD | K_C_MOD | K_M_MOD))

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
#define K_TAB			K_C_I		// 0x09
#define K_C_J			K_C('J')
#define K_C_K			K_C('K')
#define K_C_L			K_C('L')
#define K_C_M			K_C('M')
#define K_ENTER			K_C_M		// 0x0d
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
#define K_C_LBRAK		K_C('[')	// 0x1b
#define K_ESC			CHAR_ESC	// 0x1b
#define K_C_BAKSL		K_C('\\')	// 0x1c
#define K_C_RBRAK		K_C(']')	// 0x1d
#define K_C_CARET		K_C('^')	// 0x1e
#define K_C_UNDLN		K_C('_')	// 0x1f
#define K_SP			' '			// ' '

#define K_EXCLA			K_('!')		// 0x1b21
#define K_QUOTA			K_('"')		// 0x1b22
#define K_SHARP			K_('#')		// 0x1b23
#define K_DOLLA			K_('$')		// 0x1b24
#define K_PERCE			K_('%')		// 0x1b25
#define K_AMPSD			K_('&')		// 0x1b26
#define K_APOST			K_('\'')	// 0x1b27
#define K_LPARE			K_('(')		// 0x1b28
#define K_RPARE			K_(')')		// 0x1b29
#define K_ASTER			K_('*')		// 0x1b2a
#define K_PLUS			K_('+')		// 0x1b2b
#define K_COMMA			K_(',')		// 0x1b2c
#define K_HYPHE			K_('-')		// 0x1b2d
#define K_PERIO			K_('.')		// 0x1b2e
#define K_SLASH			K_('/')		// 0x1b2f

#define K_COLON			K_(':')		// 0x1b3a
#define K_SEMCO			K_(';')		// 0x1b3b
#define K_LESST			K_('<')		// 0x1b3c
#define K_EQUAL			K_('=')		// 0x1b3d
#define K_GREAT			K_('>')		// 0x1b3e
#define K_QUEST			K_('?')		// 0x1b3f

#define K_AT			K_('@')		// 0x1b40
#define K_A_			K_('A')
#define K_B_			K_('B')
#define K_C_			K_('C')
#define K_D_			K_('D')
#define K_E_			K_('E')
#define K_F_			K_('F')
#define K_G_			K_('G')
#define K_H_			K_('H')
#define K_I_			K_('I')
#define K_J_			K_('J')
#define K_K_			K_('K')
#define K_L_			K_('L')
#define K_M_			K_('M')
#define K_N_			K_('N')
#define K_O_			K_('O')
#define K_P_			K_('P')
#define K_Q_			K_('Q')
#define K_R_			K_('R')
#define K_S_			K_('S')
#define K_T_			K_('T')
#define K_U_			K_('U')
#define K_V_			K_('V')
#define K_W_			K_('W')
#define K_X_			K_('X')
#define K_Y_			K_('Y')
#define K_Z_			K_('Z')

#define K_LBRAK			K_('[')		// 0x5b
#define K_BAKSL			K_('\\')	// 0x5c
#define K_RBRAK			K_(']')		// 0x5d
#define K_CARET			K_('^')		// 0x5e
#define K_UNDLN			K_('_')		// 0x5f

#define K_BAKQT			K_('`')		// 0x60
#define K_a_			K_('a')
#define K_b_			K_('b')
#define K_c_			K_('c')
#define K_d_			K_('d')
#define K_e_			K_('e')
#define K_f_			K_('f')
#define K_g_			K_('g')
#define K_h_			K_('h')
#define K_i_			K_('i')
#define K_j_			K_('j')
#define K_k_			K_('k')
#define K_l_			K_('l')
#define K_m_			K_('m')
#define K_n_			K_('n')
#define K_o_			K_('o')
#define K_p_			K_('p')
#define K_q_			K_('q')
#define K_r_			K_('r')
#define K_s_			K_('s')
#define K_t_			K_('t')
#define K_u_			K_('u')
#define K_v_			K_('v')
#define K_w_			K_('w')
#define K_x_			K_('x')
#define K_y_			K_('y')
#define K_z_			K_('z')

#define K_LBRAC			K_('{')		// 0x7b
#define K_VERTB			K_('|')		// 0x7c
#define K_RBRAC			K_('}')		// 0x7d
#define K_TILDE			K_('~')		// 0x7e

#define K_M_SP			K_M(K_SP)		// 0x1b20
#define K_M_EXCLA		K_M(K_EXCLA)	// 0x1b21
#define K_M_QUOTA		K_M(K_QUOTA)	// 0x1b22
#define K_M_SHARP		K_M(K_SHARP)	// 0x1b23
#define K_M_DOLLA		K_M(K_DOLLA)	// 0x1b24
#define K_M_PERCE		K_M(K_PERCE)	// 0x1b25
#define K_M_AMPSD		K_M(K_AMPSD)	// 0x1b26
#define K_M_APOST		K_M(K_APOST)	// 0x1b27
#define K_M_LPARE		K_M(K_LPARE)	// 0x1b28
#define K_M_RPARE		K_M(K_RPARE)	// 0x1b29
#define K_M_ASTER		K_M(K_ASTER)	// 0x1b2a
#define K_M_PLUS		K_M(K_PLUS)		// 0x1b2b
#define K_M_COMMA		K_M(K_COMMA)	// 0x1b2c
#define K_M_HYPHE		K_M(K_HYPHE)	// 0x1b2d
#define K_M_PERIO		K_M(K_PERIO)	// 0x1b2e
#define K_M_SLASH		K_M(K_SLASH)	// 0x1b2f

#define K_M_0			K_M('0')		// 0x1b30
#define K_M_1			K_M('1')
#define K_M_2			K_M('2')
#define K_M_3			K_M('3')
#define K_M_4			K_M('4')
#define K_M_5			K_M('5')
#define K_M_6			K_M('6')
#define K_M_7			K_M('7')
#define K_M_8			K_M('8')
#define K_M_9			K_M('9')		// 0x1b39

#define K_M_COLON		K_M(K_COLON)	// 0x1b3a
#define K_M_SEMCO		K_M(K_SEMCO)	// 0x1b3b
#define K_M_LESST		K_M(K_LESST)	// 0x1b3c
#define K_M_EQUAL		K_M(K_EQUAL)	// 0x1b3d
#define K_M_GREAT		K_M(K_GREAT)	// 0x1b3e
#define K_M_QUEST		K_M(K_QUEST)	// 0x1b3f

#define K_M_AT			K_M(K_AT)		// 0x1b40
#define K_M_A			K_M('A')		// 0x1b41
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
#define K_M_Z			K_M('Z')		// 0x1b5a

#define K_M_LBRAK		K_M(K_LBRAK)	// 0x1b5b
#define K_M_BAKSL		K_M(K_BAKSL)	// 0x1b5c
#define K_M_RBRAK		K_M(K_RBRAK)	// 0x1b5d
#define K_M_CARET		K_M(K_CARET)	// 0x1b5e
#define K_M_UNDLN		K_M(K_UNDLN)	// 0x1b5f

#define K_M_BAKQT		K_M(K_BAKQT)	// 0x1b60
#define K_M_a			K_M('a')		// 0x1b61
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
#define K_M_z			K_M('z')		// 0x1b7a
#define K_M_LBRAC		K_M(K_LBRAC)	// 0x1b7b
#define K_M_VERTB		K_M(K_VERTB)	// 0x1b7c
#define K_M_RBRAC		K_M(K_RBRAC)	// 0x1b7d
#define K_M_TILDE		K_M(K_TILDE)	// 0x1b7e
#define K_M_CHAR_DEL	K_M(CHAR_DEL)	// 0x1b7f
/////#define K_M_BS		K_M(CHAR_DEL)		// 0x1b08

#define K_MC_AT			K_M(K_C_AT)		// 1b00
#define K_MC_A			K_M(K_C_A)		// 1b01
#define K_MC_B			K_M(K_C_B)
#define K_MC_C			K_M(K_C_C)
#define K_MC_D			K_M(K_C_D)
#define K_MC_E			K_M(K_C_E)
#define K_MC_F			K_M(K_C_F)
#define K_MC_G			K_M(K_C_G)
#define K_MC_H			K_M(K_C_H)
#define K_MC_I			K_M(K_C_I)
#define K_M_TAB			K_M(K_TAB)		// 0x1b09
#define K_MC_J			K_M(K_C_J)
#define K_MC_K			K_M(K_C_K)
#define K_MC_L			K_M(K_C_L)
#define K_MC_M			K_M(K_C_M)
#define K_M_ENTER		K_MC_M
#define K_MC_N			K_M(K_C_N)
#define K_MC_O			K_M(K_C_O)
#define K_MC_P			K_M(K_C_P)
#define K_MC_Q			K_M(K_C_Q)
#define K_MC_R			K_M(K_C_R)
#define K_MC_S			K_M(K_C_S)
#define K_MC_T			K_M(K_C_T)
#define K_MC_U			K_M(K_C_U)
#define K_MC_V			K_M(K_C_V)
#define K_MC_W			K_M(K_C_W)
#define K_MC_X			K_M(K_C_X)
#define K_MC_Y			K_M(K_C_Y)
#define K_MC_Z			K_M(K_C_Z)			// 0x1b1a
#define K_MC_LBRAK		K_M(K_C_LBRAK)		// 0x1b1b
#define K_M_ESC			K_M(CHAR_ESC)		// 0x1b1b
#define K_MC_BAKSL		K_M(K_C_BAKSL)		// 0x1b1c
#define K_MC_RBRAK		K_M(K_C_RBRAK)		// 0x1b1d
#define K_MC_CARET		K_M(K_C_CARET)		// 0x1b1e
#define K_MC_UNDLN		K_M(K_C_UNDLN)		// 0x1b1f

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
// Shift, Ctrl, Meta(Alt) modified key
#define K_S_F01			K_MOD_S__(K_F01)
#define K_S_F02			K_MOD_S__(K_F02)
#define K_S_F03			K_MOD_S__(K_F03)
#define K_S_F04			K_MOD_S__(K_F04)
#define K_S_F05			K_MOD_S__(K_F05)
#define K_S_F06			K_MOD_S__(K_F06)
#define K_S_F07			K_MOD_S__(K_F07)
#define K_S_F08			K_MOD_S__(K_F08)
#define K_S_F09			K_MOD_S__(K_F09)
#define K_S_F10			K_MOD_S__(K_F10)
#define K_S_F11			K_MOD_S__(K_F11)
#define K_S_F12			K_MOD_S__(K_F12)
#define K__C__F01		K_MOD__C_(K_F01)
#define K__C__F02		K_MOD__C_(K_F02)
#define K__C__F03		K_MOD__C_(K_F03)
#define K__C__F04		K_MOD__C_(K_F04)
#define K__C__F05		K_MOD__C_(K_F05)
#define K__C__F06		K_MOD__C_(K_F06)
#define K__C__F07		K_MOD__C_(K_F07)
#define K__C__F08		K_MOD__C_(K_F08)
#define K__C__F09		K_MOD__C_(K_F09)
#define K__C__F10		K_MOD__C_(K_F10)
#define K__C__F11		K_MOD__C_(K_F11)
#define K__C__F12		K_MOD__C_(K_F12)
#define K___M_F01		K_MOD___M(K_F01)
#define K___M_F02		K_MOD___M(K_F02)
#define K___M_F03		K_MOD___M(K_F03)
#define K___M_F04		K_MOD___M(K_F04)
#define K___M_F05		K_MOD___M(K_F05)
#define K___M_F06		K_MOD___M(K_F06)
#define K___M_F07		K_MOD___M(K_F07)
#define K___M_F08		K_MOD___M(K_F08)
#define K___M_F09		K_MOD___M(K_F09)
#define K___M_F10		K_MOD___M(K_F10)
#define K___M_F11		K_MOD___M(K_F11)
#define K___M_F12		K_MOD___M(K_F12)
#define K_SC__F01		K_MOD_SC_(K_F01)
#define K_SC__F02		K_MOD_SC_(K_F02)
#define K_SC__F03		K_MOD_SC_(K_F03)
#define K_SC__F04		K_MOD_SC_(K_F04)
#define K_SC__F05		K_MOD_SC_(K_F05)
#define K_SC__F06		K_MOD_SC_(K_F06)
#define K_SC__F07		K_MOD_SC_(K_F07)
#define K_SC__F08		K_MOD_SC_(K_F08)
#define K_SC__F09		K_MOD_SC_(K_F09)
#define K_SC__F10		K_MOD_SC_(K_F10)
#define K_SC__F11		K_MOD_SC_(K_F11)
#define K_SC__F12		K_MOD_SC_(K_F12)
#define K_S_M_F01		K_MOD_S_M(K_F01)
#define K_S_M_F02		K_MOD_S_M(K_F02)
#define K_S_M_F03		K_MOD_S_M(K_F03)
#define K_S_M_F04		K_MOD_S_M(K_F04)
#define K_S_M_F05		K_MOD_S_M(K_F05)
#define K_S_M_F06		K_MOD_S_M(K_F06)
#define K_S_M_F07		K_MOD_S_M(K_F07)
#define K_S_M_F08		K_MOD_S_M(K_F08)
#define K_S_M_F09		K_MOD_S_M(K_F09)
#define K_S_M_F10		K_MOD_S_M(K_F10)
#define K_S_M_F11		K_MOD_S_M(K_F11)
#define K_S_M_F12		K_MOD_S_M(K_F12)
#define K__CM_F01		K_MOD__CM(K_F01)
#define K__CM_F02		K_MOD__CM(K_F02)
#define K__CM_F03		K_MOD__CM(K_F03)
#define K__CM_F04		K_MOD__CM(K_F04)
#define K__CM_F05		K_MOD__CM(K_F05)
#define K__CM_F06		K_MOD__CM(K_F06)
#define K__CM_F07		K_MOD__CM(K_F07)
#define K__CM_F08		K_MOD__CM(K_F08)
#define K__CM_F09		K_MOD__CM(K_F09)
#define K__CM_F10		K_MOD__CM(K_F10)
#define K__CM_F11		K_MOD__CM(K_F11)
#define K__CM_F12		K_MOD__CM(K_F12)
#define K_SCM_F01		K_MOD_SCM(K_F01)
#define K_SCM_F02		K_MOD_SCM(K_F02)
#define K_SCM_F03		K_MOD_SCM(K_F03)
#define K_SCM_F04		K_MOD_SCM(K_F04)
#define K_SCM_F05		K_MOD_SCM(K_F05)
#define K_SCM_F06		K_MOD_SCM(K_F06)
#define K_SCM_F07		K_MOD_SCM(K_F07)
#define K_SCM_F08		K_MOD_SCM(K_F08)
#define K_SCM_F09		K_MOD_SCM(K_F09)
#define K_SCM_F10		K_MOD_SCM(K_F10)
#define K_SCM_F11		K_MOD_SCM(K_F11)
#define K_SCM_F12		K_MOD_SCM(K_F12)

// Abstract key code
#define K_BS			0x1008		// abstract Backspace key code

#define K_HOME			KEY_HOME
#define K_END			KEY_END
#define K_PPAGE			KEY_PPAGE
#define K_NPAGE			KEY_NPAGE
#define K_DEL			KEY_DC
#define K_INS			KEY_IC
#define K_S___DEL		K_MOD_S__(K_DEL)
#define K_S___INS		K_MOD_S__(K_INS)
#define K__C__DEL		K_MOD__C_(K_DEL)
#define K__C__INS		K_MOD__C_(K_INS)
#define K___M_DEL		K_MOD___M(K_DEL)
#define K___M_INS		K_MOD___M(K_INS)
#define K_S_M_DEL		K_MOD_S_M(K_DEL)
#define K_S_M_INS		K_MOD_S_M(K_INS)
#define K_SC__DEL		K_MOD_SC_(K_DEL)
#define K_SC__INS		K_MOD_SC_(K_INS)
#define K__CM_DEL		K_MOD__CM(K_DEL)
#define K__CM_INS		K_MOD__CM(K_INS)
#define K_SCM_DEL		K_MOD_SCM(K_DEL)
#define K_SCM_INS		K_MOD_SCM(K_INS)


#define K_UP			KEY_UP
#define K_DOWN			KEY_DOWN
#define K_RIGHT			KEY_RIGHT
#define K_LEFT			KEY_LEFT

// WSL Terminal
#define K_S___UP		K_MOD_S__(K_UP)
#define K_S___DOWN		K_MOD_S__(K_DOWN)
#define K_S___RIGHT		K_MOD_S__(K_RIGHT)
#define K_S___LEFT		K_MOD_S__(K_LEFT)
#define K__C__UP		K_MOD__C_(K_UP)
#define K__C__DOWN		K_MOD__C_(K_DOWN)
#define K__C__RIGHT		K_MOD__C_(K_RIGHT)
#define K__C__LEFT		K_MOD__C_(K_LEFT)
#define K___M_UP		K_MOD___M(K_UP)
#define K___M_DOWN		K_MOD___M(K_DOWN)
#define K___M_RIGHT		K_MOD___M(K_RIGHT)
#define K___M_LEFT		K_MOD___M(K_LEFT)
#define K_SC__UP		K_MOD_SC_(K_UP)
#define K_SC__DOWN		K_MOD_SC_(K_DOWN)
#define K_SC__RIGHT		K_MOD_SC_(K_RIGHT)
#define K_SC__LEFT		K_MOD_SC_(K_LEFT)
#define K_S_M_UP		K_MOD_S_M(K_UP)
#define K_S_M_DOWN		K_MOD_S_M(K_DOWN)
#define K_S_M_RIGHT		K_MOD_S_M(K_RIGHT)
#define K_S_M_LEFT		K_MOD_S_M(K_LEFT)
#define K__CM_UP		K_MOD__CM(K_UP)
#define K__CM_DOWN		K_MOD__CM(K_DOWN)
#define K__CM_RIGHT		K_MOD__CM(K_RIGHT)
#define K__CM_LEFT		K_MOD__CM(K_LEFT)
#define K_SCM_UP		K_MOD_SCM(K_UP)
#define K_SCM_DOWN		K_MOD_SCM(K_DOWN)
#define K_SCM_RIGHT		K_MOD_SCM(K_RIGHT)
#define K_SCM_LEFT		K_MOD_SCM(K_LEFT)

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
#define S_C_O		"\x0f"
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
#define S_ESC		"\x1b"

#define FUNC_ID(func)	func, #func
#define F_I(func)		FUNC_ID(func)

typedef enum {	// | Normal-mode  | List-mode    |
				// |--------------|--------------|
	EFAM,		// | executable   | executable   | executable in All mode
	EFNM,		// | executable   |not executable| executable only in Normal mode
	EFLM,		// |not executable| executable   | executable only in List mode
} list_mode_t;

typedef struct {
	list_mode_t list_mode;		// executable in list mode
	char *desc;					// short description
	char *explanation;			// explanation
#define MAX_KEYS_BIND	3
	key_code_t keys[MAX_KEYS_BIND];
	int (*func)();
	char *func_id;
	const char *(*func_get)();	// function to get assigned value
} func_key_t;

#define MAX_KEY_NAME_LEN	8		// "MC-UNDLN"
#define NUM_STR(key)	key, #key
typedef struct /*key_name_table_t*/ {
	key_code_t key_code;
	char *long_key_name;
	char *key_name;
} key_name_table_t;
extern key_name_table_t key_name_table[];

#define KEY_CODE_STR_LEN	(1+MAX_KEY_NAME_LEN+1+1)	// "(MC-UNDLN)"

func_key_t *get_app_func_key_table();

void conv_func_id_to_key_names(char *func_id, int max_keys);

void *get_app_function_for_key(key_code_t key);
const char *get_func_id_from_key(key_code_t key);
func_key_t *get_fkey_entry_from_key(func_key_t *func_key, key_code_t key, int list_mode);
key_code_t get_key_for_func_id(const char *func_id);
func_key_t *get_fkey_entry_from_func_id(const char *func_id, int list_mode);

int is_fkey_entry_executable(func_key_t *func_key, int list_mode);

int is_key_assigned_to_func(key_code_t key, func_key_t *func_key);
void clear_fkey_tbl_using_these_keys(key_code_t *keys);
void clear_key_if_bound_to_func(key_code_t key, func_key_t *func_key);
void clear_fkey_tbl_keys(func_key_t *func_key);
void bind_key_to_func(func_key_t *func_key, key_code_t *keys);

void set_menu_key(key_code_t key);
key_code_t get_menu_key();
void set_menu_key_for_do_app_menu_0();

void clear_whole_screen_update_timer();
int check_whole_screen_update_timer();

void update_msec_when_input_key();
unsigned long msec_past_input_key();

key_code_t input_key_loop();
key_code_t input_unmapped_key_loop();
key_code_t input_key_wait_return();
key_code_t input_key_macro();

void begin_check_break_key();
void end_check_break_key();
int check_break_key();

key_code_t map_key_code(key_code_t key);

const char* key_str_from_key_code(key_code_t key_code);
int key_code_from_key_str(const char* str, key_code_t* key_code);

const char *long_key_none_str();
const char *long_key_name_from_key_code(key_code_t key_code, char *buf);
const char *long_key_name_from_key_code_null(key_code_t key_code, char *buf);
const char *short_key_name_from_key_code(key_code_t key_code, char *buf);
const char *short_key_name_from_key_name(const char *key_name, char *buf);
key_code_t key_code_from_key_name(char *key_name);
key_code_t key_code_from_short_key_name(char *short_key_name);

int get_key_name_table_entries();

int is_key_ctrl(key_code_t key);
int is_key_graph(key_code_t key);
int is_key_utf8_byte(key_code_t key);
int is_key_char(key_code_t key);

#endif // keys_h

// End of keys.h
