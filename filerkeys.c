/**************************************************************************
 *   filerkeys.c                                                          *
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

#ifdef ENABLE_FILER

// view mode actions:
//	| function                       | on file               | on directory         |
//	| ------------------------------ | --------------------- | -------------------- |
//	| tap_file()                     | return file name/path | change cur dir       |
//	| edit/rename/copy/delete file() | return file name/path | return dir name/path |
//	| move_file()                    | return cur path       | return cur path      |

func_key_table_t filer_func_key_table[] = {
//  12345678901234567890   123456789012345678901234567891234567890   123456789012345678901234567890
//       12345678901234567890
 { "Prev.Menu",           "Previous Menu",                           K_ESC,        KNA, KNA,
   EFAM_EXECUTE, F_I(dof_filer_menu_0), NULL },

 { "",                    "Filer Menu-1",                            KNA,          KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_filer_menu_0), NULL },
 { "Quit",                "Quit filer",                              K_('q'), K_C_Q, K_M_q,
   EFAM_EXECUTE, F_I(dof_quit_filer),   NULL },
 { "Quit to Home dir",    "Quit to Home directory",                  K_('Q'), K_M_Q, K_MC_Q,
   EFAM_EXECUTE, F_I(dof_quit_home_dir), NULL },
 { "Edit (project)",      "Edit file (project file)",                K_('e'), K_F12,    KNA,
   F_LM_FIL_NAM, F_I(dof_open_file),    NULL },
 { "Edit file",           "Edit file",                               K_M_e, K_F11,      KNA,
   F_LM_FIL_NAM, F_I(dof_open_file_non_recursive), NULL },
 { "Edit(project, history)", "Edit file(project, history)",          K_MC_E,       KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_open_file_from_history),    NULL },
 { "Open new file",       "Open new file",                           K_('E'), K_M_E,    KNA,
   F_LM_FIL_NAM, F_I(dof_open_new_file), NULL },
 { "Open files dropped",  "Open files dropped",                      K_('\''),     KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_drop_files_to_open), NULL },
 { "Open execution log file", "Open exec. log file",                 K_M_l,        KNA, KNA,
   EFAM_EXECUTE, F_I(dof_open_exec_log_file), NULL },
 { "Open project file",   "Open project file",                       K_M_p, K_M_P,      KNA,
   EFAM_EXECUTE, F_I(dof_open_proj_file), NULL },
 { "Enter file/dir",      "Enter file/directory",                    K_ENTER,      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_tap_file),   NULL },
 { "Enter file/dir",      "Enter file/directory",                    K_M_ENTER,    KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_tap_file),   NULL },
 { "Parent directory",    "Parent directory",                        K_BS, K_C_H,       KNA,
   EFAM_EXECUTE, F_I(dof_parent_directory), NULL },
#ifdef ENABLE_HELP
 { "Splash",              "Splash screen",                           K_M_v,        KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_splash), NULL },
 { "Color pairs",         "Display color pairs",                     K_M_c,        KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_display_color_settings), NULL },
#endif // ENABLE_HELP

 { "",                    "Filer Menu-2",                            KNA,          KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_filer_menu_0), NULL },
 { "Copy file",           "Copy file",                               K_('c'), K_F02,    KNA,
   F_LM_FIL_NAM, F_I(dof_copy_file),    NULL },
 { "Copy update",         "Copy file update",                        K_('C'), K_F03,    KNA,
   F_LM_FIL_NAM, F_I(dof_copy_file_update), NULL },
 { "Drop to Copy",        "Drop to Copy file",                       K_M_C,        KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_drop_files_to_copy), NULL },
 { "Move",                "Move file",                               K_('m'), K_M_m, K_F05,
   F_LM_CUR_DIR, F_I(dof_move_file),    NULL },
 { "Drop to Move",        "Drop to Move file",                       K_M_M,        KNA, KNA,
   F_LM_CUR_DIR, F_I(dof_drop_files_to_move),    NULL },
 { "Rename",              "Rename file",                             K_('n'), K_F04,    KNA,
   F_LM_FIL_NAM, F_I(dof_rename_file),  NULL },
 { "Trash file",          "Trash file",                              K_('d'), K_F06,    KNA,
   F_LM_FIL_NAM, F_I(dof_trash_file),   NULL },
 { "Delete file",         "Delete file",                             K_('D'), K_DEL,    KNA,
   F_LM_FIL_NAM, F_I(dof_delete_file),  NULL },
 { "Mark to delete",      "Mark to be Deleted",                      K_('f'), K_F07,    KNA,
   F_LM_FIL_NAM, F_I(dof_mark_to_delete_file), NULL },
 { "Size zero",           "File size zero",                          K_('F'), K_S_F07,  KNA,
   F_LM_FIL_NAM, F_I(dof_size_zero_file), NULL },
 { "View",                "View file",                               K_('p'),      KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_view_file),    NULL },
 { "Tail",                "View tail of file",                       K_('l'),      KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_tail_file),    NULL },
 { "Find file",           "Find file",                               K_('j'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_find_file),    NULL },

 { "",                    "Filer Menu-3",                            KNA,          KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_filer_menu_0), NULL },
 { "Up",                  "Move cursor UP",                          K_C_E, K_UP,       KNA,
   EFAM_EXECUTE, F_I(dof_up), NULL },
 { "Down",                "Move cursor DOWN",                        K_C_X, K_DOWN,     KNA,
   EFAM_EXECUTE, F_I(dof_down), NULL },
 { "Page up  ",           "Page UP",                                 K_C_R, K_PPAGE, K_LEFT,
   EFAM_EXECUTE, F_I(dof_page_up), NULL },
 { "Page down",           "Page DOWN",                               K_C_C, K_NPAGE, K_RIGHT,
   EFAM_EXECUTE, F_I(dof_page_down), NULL },

 { "Top",                 "Top of list",                             K_C_T, K_M_t, K_HOME,
   EFAM_EXECUTE, F_I(dof_top_of_list),  NULL },
 { "Bottom",              "Bottom of list",                          K_C_B, K_M_b, K_END,
   EFAM_EXECUTE, F_I(dof_bottom_of_list), NULL },
 { "Select",              "Select file",                             K_(' '),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_select_file),  NULL },
 { "Select all",          "Select all files",                        K_('a'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_select_all_files), NULL },
 { "Select none",         "Select no files",                         K_('A'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_select_no_file), NULL },
 { "Make directory",      "Make directory",                          K_('k'), K_F08, K_M_k,
   EFAM_EXECUTE, F_I(dof_make_directory), NULL },
 { "Change directory",    "Change directory",                        K_('h'), K_F09,    KNA,
   EFAM_EXECUTE, F_I(dof_change_directory), NULL },
 { "Beginning dir.",      "Beginning directory",                     K_('~'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_beginning_directory), NULL },
 { "Home directory",      "Home directory",                          K_('`'), K_F01,    KNA,
   EFAM_EXECUTE, F_I(dof_home_directory), NULL },
 { "Root directory",      "Root directory",                          K_('/'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_root_directory), NULL },
 { "Previous dir.",       "Previous directory",                      K_('b'), K_M_SLASH, KNA,
   EFAM_EXECUTE, F_I(dof_prev_directory), NULL },

 { "",                    "Filer Menu-4",                            KNA,          KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_filer_menu_0), NULL },
 { "Execute command w/ file","Execute command with file",            K_('x'), K_F10,    KNA,
   F_LM_FIL_NAM, F_I(dof_exec_command_with_file), NULL },
 { "Execute command w/ files","Execute command with files",          K_('X'),      KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_exec_command_with_files), NULL },
 { "Run soon",            "Run command soon",                        K_M_x,        KNA, KNA,
   EFAM_EXECUTE, F_I(dof_run_command_soon), NULL },
 { "Run soon w/ logging", "Run command soon w/ logging",             K_M_X,        KNA, KNA,
   EFAM_EXECUTE, F_I(dof_run_command_soon_w_log), NULL },
 { "Run command rel.",    "Run command (relative path)",             K_('r'),      KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_run_command_rel), NULL },
 { "Run command abs.",    "Run command (absolute path)",             K_M_r,        KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_run_command_abs), NULL },
 { "Run command(src/dest)","Run command (src/dest)",                 K_('t'),      KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_run_command_src_dst), NULL },
 { "Run command (sh)",    "Run command (sh path)",                   K_('T'),      KNA, KNA,
   F_LM_FIL_NAM, F_I(dof_run_command_sh), NULL },
 { "Real path",           "Convert Symlink to Real path",            K_M_L,        KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_real_path),    NULL },
#ifdef ENABLE_HELP
 { "File List",           "Display File List",                       K_M_SEMCO,    KNA, KNA,
   EFLM_NO_EXEC, F_I(doe_view_file_list), NULL },
 { "Key List",            "Display Key List",                        K_M_QUEST,    KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_view_func_list), NULL },
#endif // ENABLE_HELP

 { "",                    "Filer Menu-5",                            KNA,          KNA, KNA,
   EFLM_NO_EXEC, F_I(dof_filer_menu_0), NULL },
 { "Split pane",          "Split screen into panes",                 K_('|'), K_('\\'), KNA,
   EFAM_EXECUTE, F_I(dof_tog_panes), get_str_filer_panes },
 { "Switch pane",         "Switch filer pane",                       K_TAB, K_M_TAB,    KNA,
   EFAM_EXECUTE, F_I(dof_tog_panex), get_str_filer_panex },
 { "Refresh",             "Refresh filer screen",                    K_(','),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_refresh_filer), NULL },
 { "Show dot file",       "Show dot file",                           K_('.'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_tog_show_dot_file), get_str_show_dot_file },
 { "Show file info.",     "Show file information",                   K_('-'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_inc_show_file_info), get_str_show_file_info },
 { "Sort mode",           "File Sort mode",                          K_('s'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_inc_sort_by), get_str_sort_by },
 { "Clear sort mode",     "Clear File Sort mode",                    K_('S'),      KNA, KNA,
   EFAM_EXECUTE, F_I(dof_clear_sort_by), NULL },
 { "Display key list",    "Toggle Displaying key list",              K_M_K,        KNA, KNA,
   EFAM_EXECUTE, F_I(dof_inc_key_list_lines), get_str_key_list_lines },
 { "Code-7F Key-BS",      "Toggle Code-7F Key-BS",                   K_M_D,        KNA, KNA,
   EFAM_EXECUTE, F_I(doe_tog_map_key_7f_bs), get_str_map_key_7f_bs },

 { "",                    "",                                        KNA,          KNA, KNA,
   EFAM_EXECUTE, F_I(NULL),            NULL },
};

#endif // ENABLE_FILER

// End of filerkeys.c
