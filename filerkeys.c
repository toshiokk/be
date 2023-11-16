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

func_key_table_t filer_func_key_table[] = {
//  12345678901234567890   123456789012345678901234567890   123456789012345678901234567890
//       12345678901234567890
 { "",                    "Filer Menu-1",                   K_ESC,        KNA, KNA,
   1,F_I(dof_filer_menu_1), NULL },
 { "Prev.Menu",           "Previous Menu",                  K_M_ESC, K_M_BACKQUOTE, KNA,
   0,F_I(dof_filer_menu_0), NULL },
 { "Quit",                "Quit filer",                     'q', K_C_Q, K_M_q,
   1,F_I(dof_quit_filer),   NULL },
 { "Quit to Home dir",    "Quit to Home directory",         'Q', K_M_Q,        KNA,
   1,F_I(dof_quit_home_dir), NULL },
 { "Edit (project)",      "Edit file (project file)",       'e', K_F01,        KNA,
   0,F_I(dof_edit_file),    NULL },
 { "Edit file",           "Edit file",                      K_M_e,        KNA, KNA,
   1,F_I(dof_edit_file_non_recursive), NULL },
 { "Edit new file",       "Edit new file",                  'E', K_M_E,        KNA,
   1,F_I(dof_edit_new_file), NULL },
 { "Enter file/dir",      "Enter file/directory",           K_ENTER, K_C_M,    KNA,
   1,F_I(dof_enter_file),   NULL },
 { "Parent directory",    "Parent directory",               K_BS, K_C_H,       KNA,
   1,F_I(dof_parent_directory), NULL },

 { "",                    "Filer Menu-2",                   KNA,          KNA, KNA,
   1,F_I(dof_filer_menu_2), NULL },
 { "Copy file",           "Copy file",                      'c', K_F02,        KNA,
   1,F_I(dof_copy_file),    NULL },
 { "Copy update",         "Copy file update",               'C', K_F03,        KNA,
   1,F_I(dof_copy_file_update), NULL },
 { "Rename",              "Rename file",                    'n', K_F04,        KNA,
   1,F_I(dof_rename_file),  NULL },
 { "Move",                "Move file",                      'm', K_F05,        KNA,
   1,F_I(dof_move_file),    NULL },
 { "Trash file",          "Trash file",                     'd', K_F06,        KNA,
   0,F_I(dof_trash_file),   NULL },
 { "Delete file",         "Delete file",                    'D', K_DEL, K_DC,
   0,F_I(dof_delete_file),  NULL },
 { "View",                "View file",                      'p',          KNA, KNA,
   0,F_I(dof_view_file),    NULL },
 { "Tail",                "View tail of file",              'l',          KNA, KNA,
   0,F_I(dof_tail_file),    NULL },
 { "Mark to delete",      "Mark to be Deleted",             'f', K_F07,        KNA,
   0,F_I(dof_mark_to_delete_file), NULL },
 { "Size zero",           "File size zero",                 'F', K_F07,        KNA,
   0,F_I(dof_size_zero_file), NULL },
 { "Find file",           "Find file",                      'j',          KNA, KNA,
   0,F_I(dof_find_file),    NULL },

 { "",                    "Filer Menu-3",                   KNA,          KNA, KNA,
   0,F_I(dof_filer_menu_3), NULL },
 { "Top",                 "Top of list",                    K_C_T, K_M_t,      KNA,
   1,F_I(dof_top_of_list),  NULL },
 { "Bottom",              "Bottom of list",                 K_C_B, K_M_b,      KNA,
   1,F_I(dof_bottom_of_list), NULL },
 { "Select",              "Select file",                    ' ',          KNA, KNA,
   1,F_I(dof_select_file),  NULL },
 { "Select all",          "Select all files",               'a',          KNA, KNA,
   1,F_I(dof_select_all_files), NULL },
 { "Select none",         "Select no files",                'z',          KNA, KNA,
   1,F_I(dof_select_no_file), NULL },
 { "Make directory",      "Make directory",                 'k', K_F08,        KNA,
   1,F_I(dof_make_directory), NULL },
 { "Change directory",    "Change directory",               'h', K_F09,        KNA,
   1,F_I(dof_change_directory), NULL },
 { "Beginning dir.",      "Beginning directory",            '~',          KNA, KNA,
   1,F_I(dof_beginning_directory), NULL },
 { "Home directory",      "Home directory",                 '`', K_F12,        KNA,
   1,F_I(dof_home_directory), NULL },
 { "Root directory",      "Root directory",                 '/',          KNA, KNA,
   1,F_I(dof_root_directory), NULL },
 { "Previous dir.",       "Previous directory",             'b',          KNA, KNA,
   1,F_I(dof_prev_directory), NULL },

 { "",                    "Filer Menu-4",                   KNA,          KNA, KNA,
   0,F_I(dof_filer_menu_4), NULL },
 { "Execute command",     "Execute command",                'x', K_F10,        KNA,
   1,F_I(dof_exec_command_with_file), NULL },
 { "Execute command",     "Execute command with files",     'X',          KNA, KNA,
   1,F_I(dof_exec_command_with_files), NULL },
 { "Run command rel.",    "Run command (relative path)",    'r', K_F11,        KNA,
   1,F_I(dof_run_command_rel), NULL },
 { "Run command abs.",    "Run command (absolute path)",    'R',          KNA, KNA,
   1,F_I(dof_run_command_abs), NULL },
 { "Run command(src/dest)","Run command (src/dest)",        't',          KNA, KNA,
   1,F_I(dof_run_command_src_dst), NULL },
 { "Run command (sh)",    "Run command (sh path)",          'T',          KNA, KNA,
   1,F_I(dof_run_command_sh), NULL },
 { "Run soon",            "Run command soon",               K_M_CR, K_M_x,     KNA,
   1,F_I(dof_run_command_soon), NULL },
 { "Real path",           "Convert Symlink to Real path",   K_M_l,        KNA, KNA,
   0,F_I(dof_real_path),    NULL },
#ifdef ENABLE_HELP
 { "Splash",              "Splash screen",                  K_M_v,        KNA, KNA,
   0,F_I(dof_filer_splash), NULL },
#endif // ENABLE_HELP
 { "Color pairs",         "Display color pairs",            K_M_c,        KNA, KNA,
   0,F_I(dof_display_color_pairs), NULL },

 { "",                    "Filer Menu-5",                   KNA,          KNA, KNA,
   0,F_I(dof_filer_menu_5), NULL },
 { "Split pane",          "Split screen into panes",        '\\', K_MC_BACKSLASH, KNA,
   0,F_I(dof_tog_panes), get_str_filer_panes },
 { "Switch pane",         "Switch filer pane",              K_TAB, K_M_TAB, KNA,
   1,F_I(dof_switch_filer_pane), NULL },
 { "Refresh",             "Refresh filer screen",           ',',          KNA, KNA,
   0,F_I(dof_refresh_filer), NULL },
 { "Show dot file",       "Show dot file",                  '.',          KNA, KNA,
   0,F_I(dof_tog_show_dot_file), get_str_show_dot_file },
 { "Show file info.",     "Show file information",          '-',          KNA, KNA,
   0,F_I(dof_inc_show_file_info), get_str_show_file_info },
 { "Sort mode",           "File Sort mode",                 's',          KNA, KNA,
   0,F_I(dof_inc_sort_by), get_str_sort_by },
 { "Clear sort mode",     "Clear File Sort mode",           'S',          KNA, KNA,
   0,F_I(dof_clr_sort_by), get_str_sort_by },
 { "Display key list",    "Toggle Displaying key list",     K_M_K,        KNA, KNA,
   0,F_I(dof_inc_key_list_lines), get_str_key_list_lines },
 { "Code-7F Key-BS",      "Toggle Code-7F Key-BS",          K_M_D,        KNA, KNA,
   0,F_I(dof_tog_map_key_7f_bs), get_str_map_key_7f_bs },
 { "Exam key code",       "Examine key code",               K_M_k,        KNA, KNA,
   0,F_I(doe_examine_key_code), NULL },

//// { "user command-1",       "user defined command-1",         'y',          KNA, KNA,
////   0,F_I(dof_run_cmd_1),  NULL },
//// { "user command-2",       "user defined command-2",         'u',          KNA, KNA,
////   0,F_I(dof_run_cmd_2),  NULL },
//// { "user command-3",       "user defined command-3",         'i',          KNA, KNA,
////   0,F_I(dof_run_cmd_3),  NULL },
//// { "user command-4",       "user defined command-4",         'o',          KNA, KNA,
////   0,F_I(dof_run_cmd_4),  NULL },

 { "",                    "",                               KNA,          KNA, KNA,
   0,F_I(NULL),            NULL },
};

#endif // ENABLE_FILER

// End of filerkeys.c
