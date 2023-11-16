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

func_key_table_t editor_func_key_table[] = {
//  12345678901234567890   123456789012345678901234567890   123456789012345678901234567890
//      12345678901234567890
 { "",                    "File Menu",                      K_ESC,        KNA, KNA,
   1,F_I(doe_editor_menu_1), NULL },
 { "Prev.Menu",           "Previous Menu",                  K_M_ESC, K_M_BACKQUOTE, KNA,
   0,F_I(doe_editor_menu_0), NULL },
 { "Open File",           "Open file",                      K_M_e,        KNA, KNA,
   0,F_I(doe_open_file), NULL },
 { "Open New File",       "Open new file",                  K_M_E, K_M_n,      KNA,
   0,F_I(doe_open_new_file), NULL },
 { "Reopen",              "Reopen file",                    K_C_Z,        KNA, KNA,
   0,F_I(doe_reopen_file), NULL },
 { "Reopen, Last L.",     "Reopen file and go to Last line",KNA,          KNA, KNA,
   0,F_I(doe_reopen_file_last_line), NULL },
 { "Write File as",       "Write file as",                  K_M_s,        KNA, KNA,
   0,F_I(doe_write_file_to), NULL },
 { "Write File",          "Write file",                     K_M_w,        KNA, KNA,
   0,F_I(doe_write_file_ask), NULL },
 { "Write File Alwy.",    "Write file always",              K_M_W,        KNA, KNA,
   0,F_I(doe_write_file_always), NULL },
 { "Write All",           "Write all files",                K_M_a,        KNA, KNA,
   0,F_I(doe_write_all_ask), NULL },
 { "Write All Modif.",    "Write all files modified",       K_M_A, K_M_S,      KNA,
   0,F_I(doe_write_all_modified), NULL },
 { "Close",               "Close file",                     K_C_Q,        KNA, KNA,
   1,F_I(doe_close_file_ask), NULL },
 { "Close All inter.",    "Close all files",                K_M_q,        KNA, KNA,
   1,F_I(doe_close_all_ask), NULL },
 { "Close All Modif.",    "Close all files modified",       K_M_Q,        KNA, KNA,
   1,F_I(doe_close_all_modified), NULL },
 { "Open Files",          "Open files in buffer",           K_M_L,        KNA, KNA,
   0,F_I(doe_open_files_in_buf), NULL },
 { "Read into buffer",    "Read file into current pos",     K_M_i,        KNA, KNA,
   0,F_I(doe_read_file_into_cur_pos), NULL },

 { "",                    "Cursor Menu",                    KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_2), NULL },
 { "Up",                  "Move cursor UP",                 K_C_E, KNA,   K_UP,
   1,F_I(doe_up), NULL },
 { "Down",                "Move cursor DOWN",               K_C_X, KNA,   K_DOWN,
   1,F_I(doe_down), NULL },
 { "Back",                "Move cursor LEFT",               K_C_S, KNA,   K_LEFT,
   1,F_I(doe_left), NULL },
 { "Forward",             "Move cursor RIGHT",              K_C_D, KNA,   K_RIGHT,
   1,F_I(doe_right), NULL },
 { "Prev Page",           "Page UP",                        K_C_R, KNA,   K_PPAGE,
   1,F_I(doe_page_up), NULL },
 { "Next Page",           "Page DOWN",                      K_C_C, KNA,   K_NPAGE,
   1,F_I(doe_page_down), NULL },
 { "Prev Word",           "Move backward one word",         K_C_A,        KNA, KNA,
   1,F_I(doe_prev_word), NULL },
 { "Next Word",           "Move forward one word",          K_C_F,        KNA, KNA,
   1,F_I(doe_next_word), NULL },
 { "Start of line",       "Start of the line",              K_C_T, KNA,   K_HOME,
   1,F_I(doe_start_of_line), NULL },
 { "End of line",         "End of the line",                K_C_B, K_C_V, K_END,
   1,F_I(doe_end_of_line), NULL },
 { "First line",          "Top of the file",                K_M_t,        KNA, KNA,
   1,F_I(doe_first_line), NULL },
 { "Last line",           "Bottom of the file",             K_M_b,        KNA, KNA,
   1,F_I(doe_last_line), NULL },
 { "Go To Line",          "Go to specified line",           K_C_UNDERLINE,KNA, KNA,
   0,F_I(doe_goto_input_line), NULL },
 { "TagJump",             "Tagged jump",                    K_M_j, K_F12,      KNA,
   1,F_I(doe_goto_file_in_cur_line), NULL },
 { "Change directory",    "Goto directory",                 K_M_J, K_SF12,     KNA,
   1,F_I(doe_goto_directory_in_cur_line), NULL },

 { "",                    "Edit Menu",                      KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_3), NULL },
 { "Tab",                 "Insert a TAB character",         K_TAB,        KNA, KNA,
   0,F_I(doe_tab), NULL },
 { "Enter",               "Insert a carriage return",       K_C_M, KNA,   K_ENTER,
   1,F_I(doe_carriage_return), NULL },
 { "Backspace",           "Backspace",                      K_C_H, KNA,   K_BS,
   0,F_I(doe_backspace), NULL },
 { "Delete",              "Delete a character",             K_C_G, K_DEL, K_DC,
   0,F_I(doe_delete_char), NULL },
 { "UpperLower",          "Invert upper/lower letter",      K_C_W,        KNA, KNA,
   0,F_I(doe_conv_upp_low_letter), NULL },
 { "Ctrl code",           "Input control code",             K_C_AT, K_M_9,     KNA,
   0,F_I(doe_control_code), NULL },
 { "Char code",           "Input character code",           K_M_AT, K_M_0,     KNA,
   0,F_I(doe_charcode), NULL },
 { "Paste history",       "Paste from history",             K_M_h,        KNA, KNA,
   0,F_I(doe_paste_history), NULL },
 { "Record/Stop-rec",     "Start/Stop recording key-macro", K_F01, K_M_r     , KNA,
   0,F_I(doe_start_rec__cancel_rec), NULL },
 { "End-rec/Playback",    "End-rec/Playback key-macro",     K_F02, K_M_p     , KNA,
   0,F_I(doe_end_rec__playback), NULL },
 { "Playback",            "Playback key-macro",             K_M_2,        KNA, KNA,
   0,F_I(doe_playback), NULL },
 { "Playback-1",          "Playback 1st newest key-macro",  K_SF01,       KNA, KNA,
   0,F_I(doe_playback_last_1), NULL },
 { "Playback-2",          "Playback 2nd newest key-macro",  K_SF02,       KNA, KNA,
   0,F_I(doe_playback_last_2), NULL },
#ifdef ENABLE_FILER
 { "Run line",            "Run current line as command",    K_M_CR, K_M_x,     KNA,
   1,F_I(doe_run_line_soon), NULL },
#endif // ENABLE_FILER
#ifdef ENABLE_UNDO
 { "Undo",                "Undo",                           K_M_u, K_M_z,      KNA,
   0,F_I(doe_undo), NULL },
 { "Redo",                "Redo",                           K_M_o,        KNA, KNA,
   0,F_I(doe_redo), NULL },
#endif // ENABLE_UNDO

 { "",                    "Search/Replace Menu",            KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_4), NULL },
 { "Search Bw",           "Search keyword forward",         K_SF03, K_C_RBRACKET, KNA,
   0,F_I(doe_search_backward_first), NULL },
 { "Search Fw",           "Search keyword backward",        K_SF04, K_C_BACKSLASH,  KNA,
   0,F_I(doe_search_forward_first), NULL },
 { "Srch Nx Bw",          "Search keyword backward again",  K_F03,        KNA, KNA,
   0,F_I(doe_search_backward_next), NULL },
 { "Srch Nx Fw",          "Search keyword forward again",   K_F04,        KNA, KNA,
   0,F_I(doe_search_forward_next), NULL },
 { "Replace",             "Replace keyword",                K_C_CARET,    KNA, KNA,
   0,F_I(doe_replace), NULL },
#ifdef ENABLE_REGEX
 { "Find Bracket()",      "Find counterpart bracket()",     K_M_RBRACKET, K_M_LBRACE, KNA,
   0,F_I(doe_find_bracket), NULL },
 { "Find Bracket)(",      "Find counterpart bracket)(",     K_M_LBRACKET, K_M_RBRACE, KNA,
   0,F_I(doe_find_bracket_reverse), NULL },
 { "Hilight Bracket()",   "Hilight counterpart bracket()", K_M_BACKSLASH, KNA, KNA,
   0,F_I(doe_highlight_bracket), NULL },
 { "Hilight Bracket)(",   "Hilight counterpart bracket)(", K_M_VERTBAR,   KNA, KNA,
   0,F_I(doe_highlight_bracket_reverse), NULL },
#endif // ENABLE_REGEX

 { "",                    "Buffer Menu",                    KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_5), NULL },
 { "Previous File",       "Open previous file",             K_M_COMMA,    KNA, KNA,
   1,F_I(doe_switch_to_prev_file), NULL },
 { "Next File",           "Open next file",                 K_M_PERIOD,   KNA, KNA,
   1,F_I(doe_switch_to_next_file), NULL },
 { "Top File",            "Open top file",                  K_M_LESSTHAN, KNA, KNA,
   0,F_I(doe_switch_to_top_file), NULL },
 { "Bottom File",         "Open bottom file",               K_M_GREATERTHAN, KNA, KNA,
   0,F_I(doe_switch_to_bot_file), NULL },
 { "File List",           "Display File List",              K_M_SEMICOLON, K_F11, KNA,
   0,F_I(doe_switch_to_file_list), NULL },
#if APP_REL_LVL == APP_REL_LVL_EXPERIMENTAL
 { "Previous buffers",    "Goto previous buffers",          K_M_LPARENTHESIS, KNA, KNA,
   0,F_I(doe_switch_to_prev_buffers), NULL },
 { "Next buffers",        "Goto next buffers",              K_M_RPARENTHESIS, KNA, KNA,
   0,F_I(doe_switch_to_next_buffers), NULL },
#endif // APP_REL_LVL
#ifdef ENABLE_HELP
 { "Key List",            "Display Key List",               K_M_k,        KNA, KNA,
   0,F_I(doe_switch_to_key_list), NULL },
 { "Func List",           "Display Function List",          K_M_f,        KNA, KNA,
   0,F_I(doe_switch_to_func_list), NULL },
#endif // ENABLE_HELP
 { "Open Proj File",      "Open Project file",              K_M_l,        KNA, KNA,
   0,F_I(doe_open_proj_file), NULL },
 { "Goto prev. pos",      "Goto previous pos.",             K_M_SLASH,    KNA, KNA,
   0,F_I(doe_return_to_prev_file_pos), NULL },
 { "Split pane",          "Split screen into panes",        K_MC_BACKSLASH, KNA, KNA,
   0,F_I(doe_tog_panes), get_editor_panes },
 { "Switch pane",         "Switch editor pane",             K_M_TAB,      KNA, KNA,
   0,F_I(doe_switch_editor_pane), NULL },

 { "",                    "Cut-buffer Menu",                KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_6), NULL },
 { "Mark Text",           "Mark text",                      K_C_N, K_F05,      KNA,
   0,F_I(doe_tog_mark), NULL },
 { "Clear Cut-buf",       "Clear Cut-buffers",              K_SF05,       KNA, KNA,
   0,F_I(doe_clear_cut_buf), NULL },
 { "Pop Cut-buf",         "Pop Cut-buffer",                 K_SF08,       KNA, KNA,
   0,F_I(doe_pop_cut_buf), NULL },
 { "Select all",          "Select all of buffer",           K_M_m,        KNA, KNA,
   0,F_I(doe_select_all_lines), NULL },
 { "Cut Text",            "Cut into Cut-buffer",            K_C_K, K_F06,      KNA,
   0,F_I(doe_cut_text), NULL },
 { "Delete Text",         "Delete text",                    K_SF06,       KNA, KNA,
   0,F_I(doe_delete_text), NULL },
 { "Copy Text",           "Copy to Cut-buffer",             K_C_Y, K_F07,      KNA,
   1,F_I(doe_copy_text), NULL },
 { "Paste Text",          "Paste from Cut-buf with pop",    K_C_P, K_F08,      KNA,
   0,F_I(doe_paste_text_with_pop), NULL },
 { "Paste2 Text",         "Paste from Cut-buf without pop", K_C_O, K_F09, K_IC,    
   0,F_I(doe_paste_text_without_pop), NULL },
 { "Duplic Text",         "Duplicate line",                 K_C_U, K_F10,      KNA,
   0,F_I(doe_duplicate_text), NULL },
 { "CutToHead",           "Cut to head of the line",        K_C_J,        KNA, KNA,
   0,F_I(doe_cut_to_head), NULL },
 { "CutToTail",           "Cut to tail of the line",        K_C_L,        KNA, KNA,
   0,F_I(doe_cut_to_tail), NULL },

 { "",                    "Settings Menu-1",                KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_7), NULL },
 { "Tab size",            "Toggle Tab size",                K_MC_T,       KNA, KNA,
   0,F_I(doe_tog_tab_size),       get_str_tab_size },
 { "Tab size",            "Incurement Tab size",            K_MC_Y,       KNA, KNA,
   0,F_I(doe_inc_tab_size),       get_str_tab_size },
 { "Draw cursor",         "Toggle Drawing cursor",          K_MC_C,       KNA, KNA,
   0,F_I(doe_tog_draw_cursor),    get_str_draw_cursor },
 { "Ignore case",         "Toggle Ignore case",             K_MC_I,       KNA, KNA,
   0,F_I(doe_tog_ignore_case),    get_str_ignore_case },
 { "Auto indent",         "Toggle Auto indent",             K_MC_O,       KNA, KNA,
   0,F_I(doe_tog_auto_indent),    get_str_auto_indent },
 { "Display key list",    "Toggle Displaying key list",     K_MC_K,       KNA, KNA,
   0,F_I(doe_inc_key_list_lines), get_str_key_list_lines },
 { "Back up files",       "Increment Back up files",        K_MC_B,       KNA, KNA,
   0,F_I(doe_inc_backup_files),   get_str_backup_files },
#ifdef ENABLE_REGEX
 { "RegExp",              "Toggle Regex search/replace",    K_MC_X,       KNA, KNA,
   0,F_I(doe_tog_regexp),         get_str_regexp },
#endif // ENABLE_REGEX
#ifdef ENABLE_UTF8
 { "UTF-8",               "Toggle Display/Input UTF-8 code",K_MC_U,       KNA, KNA,
   0,F_I(doe_tog_utf8),           get_str_utf8 },
#endif // ENABLE_UTF8

 { "",                    "Settings Menu-2",                KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_8), NULL },
 { "Cursor center",       "Toggle Cursor-center mode",      K_MC_S,       KNA, KNA,
   0,F_I(doe_inc_cursor_positioning),  get_str_cursor_positioning },
 { "Dual scroll",         "Toggle Dual scroll",             K_MC_D,       KNA, KNA,
   0,F_I(doe_tog_dual_scroll),    get_str_dual_scroll },
 { "View mode",           "Toggle View mode",               K_MC_V,       KNA, KNA,
   0,F_I(doe_tog_view_mode),      get_str_view_mode },
 { "Show ruler",          "Toggle Show Ruler",              K_MC_R,       KNA, KNA,
   0,F_I(doe_tog_show_ruler),     get_str_show_ruler },
 { "Show line number",    "Toggle Show Line number",        K_MC_N,       KNA, KNA,
   0,F_I(doe_tog_show_line_num),  get_str_show_line_num },
 { "Line-wrap mode",      "Toggle Line-wrap mode",          K_MC_L,       KNA, KNA,
   0,F_I(doe_tog_line_wrap_mode), get_str_line_wrap_mode },
#ifdef ENABLE_SYNTAX
 { "Syntax HL",           "Toggle Syntax Highlighting",     K_MC_H,       KNA, KNA,
   0,F_I(doe_tog_syntax_hl),      get_str_syntax_hl },
 { "TAB/EOL notation",    "Toggle TAB/EOL notation",        K_MC_Z,       KNA, KNA,
   0,F_I(doe_tog_tab_eol_notation), get_str_tab_eol_notation },
#endif // ENABLE_SYNTAX
 { "Code-7F Key-BS",      "Toggle Code-7F Key-BS",          K_M_DEL,      KNA, KNA,
   0,F_I(dof_tog_map_key_7f_bs), get_str_map_key_7f_bs },

 { "",                    "File type Menu",                 KNA,          KNA, KNA,
   0,F_I(doe_editor_menu_9), NULL },
 { "nix format",          "Set nix format (LF)",            KNA,          KNA, KNA,
   0,F_I(doe_set_nix_file),       get_str_nix_file },
 { "Mac format",          "Set Mac format (CR)",            KNA,          KNA, KNA,
   0,F_I(doe_set_mac_file),       get_str_mac_file },
 { "DOS format",          "Set DOS format (CR+LF)",         KNA,          KNA, KNA,
   0,F_I(doe_set_dos_file),       get_str_dos_file },
#ifdef USE_NKF
 { "ASCII  encoding",     "Set ASCII  encoding",            KNA,          KNA, KNA,
   0,F_I(doe_set_encode_ascii),   get_str_encode_ascii },
 { "UTF-8  encoding",     "Set UTF-8  encoding",            KNA,          KNA, KNA,
   0,F_I(doe_set_encode_utf8),    get_str_encode_utf8 },
 { "EUC-JP encoding",     "Set EUC-JP encoding",            KNA,          KNA, KNA,
   0,F_I(doe_set_encode_eucjp),   get_str_encode_eucjp },
 { "S-JIS  encoding",     "Set S-JIS  encoding",            KNA,          KNA, KNA,
   0,F_I(doe_set_encode_sjis),    get_str_encode_sjis },
 { "JIS    encoding",     "Set JIS    encoding",            KNA,          KNA, KNA,
   0,F_I(doe_set_encode_jis),     get_str_encode_jis },
 { "BINARY encoding",     "Set BINARY encoding",            KNA,          KNA, KNA,
   0,F_I(doe_set_encode_binary),  get_str_encode_binary },
#endif // USE_NKF
#ifdef ENABLE_HELP
 { "Splash",              "Splash screen",                  K_M_v,        KNA, KNA,
   0,F_I(doe_editor_splash), NULL },
#endif // ENABLE_HELP
 { "Color pairs",         "Display color pairs",            K_M_c,        KNA, KNA,
   0,F_I(doe_display_color_pairs), NULL },
 { "Refresh",             "Refresh editor screen",          K_M_ASTERISK, KNA, KNA,
   1,F_I(doe_refresh_editor), NULL },

 { "",                    "",                               KNA,          KNA, KNA,
   0,F_I(NULL), NULL },
};

// End of editorkeys.c
