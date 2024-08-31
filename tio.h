/**************************************************************************
 *   tio.h - terminal I/O                                                 *
 *                                                                        *
 *   Copyright (C) 2005-2008 PCR                                          *
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

#ifndef tio_h
#define tio_h

// MAX_SCRN_COLS			384		// = 1920[pixels] / 5[pixels/char] (full HD)
// MAX_SCRN_COLS			512		// = 2560[pixels] / 5[pixels/char] (WQXGA)
#define MAX_SCRN_COLS			768		// = 3840[pixels] / 5[pixels/char] (4K landscape)
// MAX_SCRN_COLS			1536	// = 7680[pixels] / 5[pixels/char] (8K landscape)
#define MAX_SCRN_LINES		384		// = 3840[pixels] / 10[pixels/char] (4K portrait)
#define MAX_SCRN_LINE_BUF_LEN	(MAX_SCRN_COLS * MAX_UTF8C_BYTES)	// = 4608

typedef short key_code_t;

#ifdef ENABLE_NCURSES
#include "cursesif.h"
// COLORS and COLOR_PAIRS are variables
#else // ENABLE_NCURSES
#include "termif.h"
// COLORS and COLOR_PAIRS are macro definitions
#endif // ENABLE_NCURSES

#define COLORS8			((COLOR_WHITE) + 1)
#define COLORS16		((COLORS8) * 2)

#define COLOR_DEFAULT	(-1)	// Not change color but use the value previously set
#define CL_DF		COLOR_DEFAULT
#define CL_HI		COLORS8

#define CL_BK		COLOR_BLACK
#define CL_RD		COLOR_RED
#define CL_GR		COLOR_GREEN
#define CL_BR		COLOR_YELLOW				// brown
#define CL_BL		COLOR_BLUE
#define CL_MG		COLOR_MAGENTA
#define CL_CY		COLOR_CYAN
#define CL_GY		COLOR_WHITE					// white (light gray)
#define CL_DG		((CL_HI) + (COLOR_BLACK))	// dark gray
#define CL_LR		((CL_HI) + (COLOR_RED))		// light red
#define CL_LG		((CL_HI) + (COLOR_GREEN))	// light green
#define CL_YL		((CL_HI) + (COLOR_YELLOW))	// light yellow
#define CL_LB		((CL_HI) + (COLOR_BLUE))	// light blue
#define CL_LM		((CL_HI) + (COLOR_MAGENTA))	// light magenta
#define CL_LC		((CL_HI) + (COLOR_CYAN))	// light cyan
#define CL_WH		((CL_HI) + (COLOR_WHITE))	// bright white

#ifndef ENABLE_16_BCG
#define LIMIT_BGC(bgc)			((bgc) & ((COLORS8)-1))		// [0, 7]
#else // ENABLE_16_BCG
#define LIMIT_BGC(bgc)			((bgc) & ((COLORS16)-1))	// [0, 15]
#endif // ENABLE_16_BCG

#define LIMIT_FGC(fgc)			((fgc) & ((COLORS16)-1))	// [0, 15]
#define MAKE_HIGH_COLOR(color)	((color) | (CL_HI))
#define GET_BASE_COLOR(color)	((color) & ((CL_HI)-1))

//-----------------------------------------------------------------------------

#ifdef START_UP_TEST
void tio_test(void);
#endif // START_UP_TEST

int tio_is_initialized(void);
int tio_init(void);
int tio_destroy(void);
int tio_begin(void);
int tio_end(void);

int tio_resize(void);
int tio_suspend(void);
int tio_resume(void);

int tio_check_update_terminal_size(void);

void tio_set_screen_size(int lines, int columns);
int tio_get_lines(void);
int tio_get_columns(void);

int tio_differ_fgc_from_bgc(int bgc, int fgc);
void tio_set_attrs(int bgc, int fgc, int rev);
void tio_set_attr_rev(int rev);

void tio_set_cursor_on(int on_off);
void tio_set_cursor_pos(int yy, int xx);

void tio_clear_flash_screen(int delay);
void tio_clear_screen(void);
void tio_clear_screen_with_color(void);
void tio_clear_lines(int line_1, int line_2);
const char *tio_blank_line(void);

void tio_output_string(int yy, int xx, const char *string, int bytes);
void tio_beep(void);
void tio_refresh(void);
void tio_repaint_all(void);

key_code_t tio_input_key(void);

void save_term_settings(struct termios *term_settings);
void restore_term_settings(struct termios *term_settings);
void set_term_no_intr(void);
void set_term_raw(void);
int init_stderr(void);

#endif // tio_h

// End of tio.h
