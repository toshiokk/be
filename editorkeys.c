/**************************************************************************
 *   editorkeys.c                                                         *
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

#if 1
#define K_REC		K_F01	// Record
#define K_PLB		K_F02	// Playback
#define K_PB1		K_SF01	// Playback 1st newest macro
#define K_PB2		K_SF02	// Playback 2nd newest macro
#else
#define K_REC		K_SF01
#define K_PLB		K_SF02
#define K_PB1		K_F01
#define K_PB2		K_F02
#endif

func_key_table_t editor_func_key_table[] = {
//  1234567890123456   123456789012345678901234567890   12345678901234567890
//      12345678901234567890
 { "",                "File Menu",                      K_ESC, K_M_1,      NKA,
   1,F_I(do_editor_menu_1), NULL },
 { "Prev.Menu",       "Previous Menu",                  K_M_ESC, K_M_BACKQUOTE, NKA,
   0,F_I(do_editor_menu_0), NULL },
 { "Open File",       "Open file",                      K_M_e,        NKA, NKA,
   0,F_I(do_open_file), NULL },
 { "Open New File",   "Open new file",                  K_M_E, K_M_n,      NKA,
   0,F_I(do_open_new_file), NULL },
 { "Reopen",          "Reopen file",                    K_C_Z,        NKA, NKA,
   0,F_I(do_reopen_file), NULL },
 { "Reopen, Last L.", "Reopen file and go to Last line",NKA,          NKA, NKA,
   0,F_I(do_reopen_file_last_line), NULL },
 { "Write File as",   "Write file as",                  K_M_s,        NKA, NKA,
   0,F_I(do_write_file_to), NULL },
 { "Write File",      "Write file",                     K_M_w,        NKA, NKA,
   0,F_I(do_write_file_ask), NULL },
 { "Write File Alwy.","Write file always",              K_M_W,        NKA, NKA,
   0,F_I(do_write_file_always), NULL },
 { "Write All",       "Write all files",                K_M_a,        NKA, NKA,
   0,F_I(do_write_all_ask), NULL },
 { "Write All Modif.","Write all files modified",       K_M_A,        NKA, NKA,
   0,F_I(do_write_all_modified), NULL },
 { "Close",           "Close file",                     K_C_Q,        NKA, NKA,
   1,F_I(do_close_file_ask), NULL },
 { "Close All inter.","Close all files",                K_M_q,        NKA, NKA,
   1,F_I(do_close_all_ask), NULL },
 { "Close All Modif.","Close all files modified",       K_M_Q,        NKA, NKA,
   1,F_I(do_close_all_modified), NULL },
 { "Open Files",      "Open files in buffer",           K_M_L,        NKA, NKA,
   0,F_I(do_open_files_in_buf), NULL },
 { "Read into buffer","Read file into current pos",     K_M_i,        NKA, NKA,
   0,F_I(do_read_file_into_cur_pos), NULL },

 { "",                "Cursor Menu",                    K_M_2,        NKA, NKA,
   1,F_I(do_editor_menu_2), NULL },
 { "Up",              "Move cursor UP",                 K_C_E, NKA,    K_UP,
   1,F_I(do_up), NULL },
 { "Down",            "Move cursor DOWN",               K_C_X, NKA,    K_DOWN,
   1,F_I(do_down), NULL },
 { "Back",            "Move cursor LEFT",               K_C_S, NKA,    K_LEFT,
   1,F_I(do_left), NULL },
 { "Forward",         "Move cursor RIGHT",              K_C_D, NKA,    K_RIGHT,
   1,F_I(do_right), NULL },
 { "Prev Page",       "Page UP",                        K_C_R, NKA,    K_PPAGE,
   1,F_I(do_page_up), NULL },
 { "Next Page",       "Page DOWN",                      K_C_C, NKA,    K_NPAGE,
   1,F_I(do_page_down), NULL },
 { "Prev Word",       "Move backward one word",         K_C_A,        NKA, NKA,
   1,F_I(do_prev_word), NULL },
 { "Next Word",       "Move forward one word",          K_C_F,        NKA, NKA,
   1,F_I(do_next_word), NULL },
 { "Start of line",   "Start of the line",              K_C_T, NKA,    K_HOME,
   1,F_I(do_start_of_line), NULL },
 { "End of line",     "End of the line",                K_C_B, K_C_V,  K_END,
   1,F_I(do_end_of_line), NULL },
 { "First line",      "Top of the file",                K_M_t,        NKA, NKA,
   1,F_I(do_first_line), NULL },
 { "Last line",       "Bottom of the file",             K_M_b,        NKA, NKA,
   1,F_I(do_last_line), NULL },
 { "Go To Line",      "Go to specified line",           K_C_UNDERLINE,NKA, NKA,
   0,F_I(do_goto_input_line), NULL },
 { "TagJump",         "Tagged jump",                    K_M_j, K_F12,      NKA,
   1,F_I(do_goto_file_in_cur_line), NULL },
 { "Change directory", "Goto directory",                K_M_J, K_SF12,     NKA,
   1,F_I(do_goto_directory_in_cur_line), NULL },

 { "",                "Edit Menu",                      K_M_3,        NKA, NKA,
   0,F_I(do_editor_menu_3), NULL },
 { "Tab",             "Insert a TAB character",         K_TAB,        NKA, NKA,
   0,F_I(do_tab), NULL },
 { "Enter",           "Insert a carriage return",       K_C_M, NKA,    K_ENTER,
   1,F_I(do_carriage_return), NULL },
 { "Backspace",       "Backspace",                      K_C_H, NKA,    K_BS,
   0,F_I(do_backspace), NULL },
 { "Delete",          "Delete a character",             K_C_G, K_DEL,  K_DC,
   0,F_I(do_delete_char), NULL },
 { "UpperLower",      "Invert upper/lower letter",      K_C_W,        NKA, NKA,
   0,F_I(do_conv_upp_low_letter), NULL },
 { "Ctrl code",       "Input control code",             K_C_AT, K_M_9,     NKA,
   0,F_I(do_control_code), NULL },
 { "Char code",       "Input character code",           K_M_AT, K_M_0,     NKA,
   0,F_I(do_charcode), NULL },
 { "Paste history",   "Paste from history",             K_M_h,        NKA, NKA,
   0,F_I(do_paste_history), NULL },
 { "Record/Stop-rec", "Start/Stop recording key-macro", K_REC, K_M_r     , NKA,
   0,F_I(do_record), NULL },
 { "End-rec/Playback","End-rec/Playback key-macro",     K_PLB, K_M_p     , NKA,
   0,F_I(do_playback), NULL },
 { "Playback-1",      "Playback 1st newest key-macro",  K_PB1,        NKA, NKA,
   0,F_I(do_start_playback_last_1), NULL },
 { "Playback-2",      "Playback 2nd newest key-macro",  K_PB2,        NKA, NKA,
   0,F_I(do_start_playback_last_2), NULL },
#ifdef ENABLE_FILER
 { "Run line",        "Run current line as command",    K_M_x,        NKA, NKA,
   0,F_I(do_run_line), NULL },
#endif // ENABLE_FILER
#ifdef ENABLE_UNDO
 { "Undo",            "Undo",                           K_M_u, K_M_z,      NKA,
   0,F_I(do_undo), NULL },
 { "Redo",            "Redo",                           K_M_o,        NKA, NKA,
   0,F_I(do_redo), NULL },
#endif // ENABLE_UNDO

 { "",                "Search/Replace Menu",            K_M_4,        NKA, NKA,
   0,F_I(do_editor_menu_4), NULL },
 { "Search Bw",       "Search keyword forward",         K_SF03, K_C_RBRACKET, NKA,
   0,F_I(do_search_backward), NULL },
 { "Search Fw",       "Search keyword backward",        K_SF04, K_C_BACKSLASH,  NKA,
   0,F_I(do_search_forward), NULL },
 { "Srch Nx Bw",      "Search keyword backward again",  K_F03,        NKA, NKA,
   0,F_I(do_search_next_backward), NULL },
 { "Srch Nx Fw",      "Search keyword forward again",   K_F04,        NKA, NKA,
   0,F_I(do_search_next_forward), NULL },
 { "Replace",         "Replace keyword",                K_C_CARET,    NKA, NKA,
   0,F_I(do_replace), NULL },
#ifdef ENABLE_REGEX
 { "Find Bracket()",  "Find counterpart bracket()",     K_M_RBRACKET,     NKA, NKA,
   0,F_I(do_find_bracket), NULL },
 { "Find Bracket)(",  "Find counterpart bracket)(",     K_M_RBRACE,     NKA, NKA,
   0,F_I(do_find_bracket_reverse), NULL },
#endif // ENABLE_REGEX

 { "",                "Buffer Menu",                    K_M_5,        NKA, NKA,
   0,F_I(do_editor_menu_5), NULL },
 { "Previous File",   "Open previous file",             K_M_COMMA,     NKA, NKA,
   1,F_I(do_switch_to_prev_file), NULL },
 { "Next File",       "Open next file",                 K_M_PERIOD,     NKA, NKA,
   1,F_I(do_switch_to_next_file), NULL },
 { "Top File",        "Open top file",                  K_M_LESSTHAN,     NKA, NKA,
   0,F_I(do_switch_to_top_file), NULL },
 { "Bottom File",     "Open bottom file",               K_M_GREATERTHAN,     NKA, NKA,
   0,F_I(do_switch_to_bot_file), NULL },
 { "File List",       "Display File List",              K_M_SEMICOLON,   K_F11, NKA,
   0,F_I(do_switch_to_file_list), NULL },
 { "Previous buffers","Goto previous buffers",          K_M_LPARENTHESIS, NKA, NKA,
   0,F_I(do_switch_to_prev_buffers), NULL },
 { "Next buffers",    "Goto next buffers",              K_M_RPARENTHESIS, NKA, NKA,
   0,F_I(do_switch_to_next_buffers), NULL },
#ifdef ENABLE_HELP
 { "Key List",        "Display Key List",               K_M_k,        NKA, NKA,
   0,F_I(do_switch_to_key_list), NULL },
 { "Func List",       "Display Function List",          K_M_f,        NKA, NKA,
   0,F_I(do_switch_to_func_list), NULL },
#endif // ENABLE_HELP
 { "Open Proj File",  "Open Project file",              K_M_l,        NKA, NKA,
   0,F_I(do_open_proj_file), NULL },
 { "Goto prev. pos",  "Goto previous pos.",             K_M_SLASH,     NKA, NKA,
   0,F_I(do_return_to_prev_file_pos), NULL },
 { "Split pane",      "Split screen into panes",        K_M_BACKSLASH, K_M_VERTBAR, NKA,
   0,F_I(do_tog_editor_panes), get_editor_panes },
 { "Switch pane",     "Switch editor pane",             K_M_TAB,   NKA, NKA,
   0,F_I(do_switch_editor_pane), NULL },

 { "",                "Cut-buffer Menu",                K_M_6,        NKA, NKA,
   0,F_I(do_editor_menu_6), NULL },
 { "Mark Text",       "Mark text",                      K_C_N, K_F05,      NKA,
   0,F_I(do_tog_mark), NULL },
 { "Clear Cut-buf",   "Clear Cut-buffers",              K_SF05,       NKA, NKA,
   0,F_I(do_clear_cut_buf), NULL },
 { "Pop Cut-buf",     "Pop Cut-buffer",                 K_SF08,       NKA, NKA,
   0,F_I(do_pop_cut_buf), NULL },
 { "Select all",      "Select all of buffer",           K_M_m,        NKA, NKA,
   0,F_I(do_select_all_lines), NULL },
 { "Cut Text",        "Cut into Cut-buffer",            K_C_K, K_F06,      NKA,
   0,F_I(do_cut_text), NULL },
 { "Delete Text",     "Delete text",                    K_SF06,       NKA, NKA,
   0,F_I(do_delete_text), NULL },
 { "Copy Text",       "Copy to Cut-buffer",             K_C_Y, K_F07,      NKA,
   1,F_I(do_copy_text), NULL },
 { "Paste Text",      "Paste from Cut-buf with pop",    K_C_P, K_F08,      NKA,
   0,F_I(do_paste_text_with_pop), NULL },
 { "Paste2 Text",     "Paste from Cut-buf without pop", K_C_O, K_F09, K_IC,    
   0,F_I(do_paste_text_without_pop), NULL },
 { "Duplic Text",     "Duplicate line",                 K_C_U, K_F10,      NKA,
   0,F_I(do_duplicate_text), NULL },
 { "CutToHead",       "Cut to head of the line",        K_C_J,        NKA, NKA,
   0,F_I(do_cut_to_head), NULL },
 { "CutToTail",       "Cut to tail of the line",        K_C_L,        NKA, NKA,
   0,F_I(do_cut_to_tail), NULL },

 { "",                "Settings Menu-1",                K_M_7,        NKA, NKA,
   0,F_I(do_editor_menu_7), NULL },
 { "Tab size",        "Toggle Tab size",                K_MC_T,        NKA, NKA,
   0,F_I(do_tog_tab_size),       get_str_tab_size },
 { "Tab size",        "Incurement Tab size",            K_MC_Y,        NKA, NKA,
   0,F_I(do_inc_tab_size),       get_str_tab_size },
 { "Draw cursor",     "Toggle Drawing cursor",          K_MC_C,        NKA, NKA,
   0,F_I(do_tog_draw_cursor),    get_str_draw_cursor },
 { "Ignore case",     "Toggle Ignore case",             K_MC_I,        NKA, NKA,
   0,F_I(do_tog_ignore_case),    get_str_ignore_case },
 { "Auto indent",     "Toggle Auto indent",             K_MC_O,        NKA, NKA,
   0,F_I(do_tog_auto_indent),    get_str_auto_indent },
 { "Display key list","Toggle Displaying key list",     K_MC_K,        NKA, NKA,
   0,F_I(do_editor_inc_key_list_lines), get_str_key_list_lines },
 { "Back up files",   "Increment Back up files",        K_MC_B,        NKA, NKA,
   0,F_I(do_inc_backup_files),   get_str_backup_files },
#ifdef ENABLE_REGEX
 { "RegExp",          "Toggle Regex search/replace",    K_MC_X,        NKA, NKA,
   0,F_I(do_tog_regexp),         get_str_regexp },
#endif // ENABLE_REGEX
#ifdef ENABLE_UTF8
 { "UTF-8",           "Toggle Display/Input UTF-8 code",K_MC_U,        NKA, NKA,
   0,F_I(do_tog_utf8),           get_str_utf8 },
#endif // ENABLE_UTF8

 { "",                "Settings Menu-2",                K_M_8,        NKA, NKA,
   0,F_I(do_editor_menu_8), NULL },
 { "Cursor center",   "Toggle Cursor-center mode",      K_MC_S,        NKA, NKA,
   0,F_I(do_inc_curs_positioning),  get_str_curs_positioning },
 { "Dual scroll",     "Toggle Dual scroll",             K_MC_D,        NKA, NKA,
   0,F_I(do_tog_dual_scroll),    get_str_dual_scroll },
 { "View mode",       "Toggle View mode",               K_MC_V,        NKA, NKA,
   0,F_I(do_tog_view_mode),      get_str_view_mode },
 { "Show ruler",      "Toggle Show Ruler",              K_MC_R,        NKA, NKA,
   0,F_I(do_tog_show_ruler),     get_str_show_ruler },
 { "Show line number","Toggle Show Line number",        K_MC_N,        NKA, NKA,
   0,F_I(do_tog_show_line_num),  get_str_show_line_num },
 { "Line-wrap mode",  "Toggle Line-wrap mode",          K_MC_L,        NKA, NKA,
   0,F_I(do_tog_line_wrap_mode), get_str_line_wrap_mode },
#ifdef ENABLE_SYNTAX
 { "Syntax HL",       "Toggle Syntax Highlighting",     K_MC_H,        NKA, NKA,
   0,F_I(do_tog_syntax_hl),      get_str_syntax_hl },
 { "TAB/EOL notation","Toggle TAB/EOL notation",        K_MC_Z,        NKA, NKA,
   0,F_I(do_tog_tab_eol_notation), get_str_tab_eol_notation },
#endif // ENABLE_SYNTAX
 { "Code-7F Key-BS",  "Toggle Code-7F Key-BS",          K_M_DEL,        NKA, NKA,
   0,F_I(do_tog_map_key_7f_bs), get_str_map_key_7f_bs },

 { "",                "File type Menu",                 K_M_9,        NKA, NKA,
   0,F_I(do_editor_menu_9), NULL },
 { "nix format",      "Set nix format (LF)",            NKA,          NKA, NKA,
   0,F_I(do_set_nix_file),       get_str_nix_file },
 { "Mac format",      "Set Mac format (CR)",            NKA,          NKA, NKA,
   0,F_I(do_set_mac_file),       get_str_mac_file },
 { "DOS format",      "Set DOS format (CR+LF)",         NKA,          NKA, NKA,
   0,F_I(do_set_dos_file),       get_str_dos_file },
#ifdef USE_NKF
 { "ASCII  encoding", "Set ASCII  encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_ascii),   get_str_encode_ascii },
 { "UTF-8  encoding", "Set UTF-8  encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_utf8),    get_str_encode_utf8 },
 { "EUC-JP encoding", "Set EUC-JP encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_eucjp),   get_str_encode_eucjp },
 { "S-JIS  encoding", "Set S-JIS  encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_sjis),    get_str_encode_sjis },
 { "JIS    encoding", "Set JIS    encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_jis),     get_str_encode_jis },
 { "BINARY encoding", "Set BINARY encoding",            NKA,          NKA, NKA,
   0,F_I(do_set_encode_binary),  get_str_encode_binary },
#endif // USE_NKF
#ifdef ENABLE_HELP
 { "Splash",          "Splash screen",                  K_M_v,        NKA, NKA,
   0,F_I(do_editor_splash), NULL },
#endif // ENABLE_HELP
 { "Color pairs",     "Display color pairs",            K_M_c,        NKA, NKA,
   0,F_I(do_editor_display_color_pairs), NULL },
 { "Refresh",         "Refresh editor screen",          K_M_ASTERISK,     NKA, NKA,
   1,F_I(do_refresh_editor), NULL },

 { "",                "",                               NKA,          NKA, NKA,
   0,F_I(NULL), NULL },
};

// End of editorkeys.c
