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
// | function                       | on file               | on directory         |
// | ------------------------------ | --------------------- | -------------------- |
// | tap_file()                     | return file name/path | change cur dir       |
// | edit/rename/copy/delete file() | return file name/path | return dir name/path |
// | move_file()                    | return cur path       | return cur path      |

func_key_table_t filer_func_key_table[] = {
//             12345678901234567890   123456789012345678901234567890
// 12345678901234567890
 { EFAM_EXEC, "Menu",                "Previous Menu",
   {K_ESC,        KNA, KNA}, F_I(dof_filer_menu_0), get_str_setting_none },

 { EFLM_NOEX, "",                    "Filer Menu-1",
   {KNA,          KNA, KNA}, F_I(dof_filer_menu_0), get_str_setting_none },
 { EFAM_EXEC, "Quit",                "Quit filer",
   {K_q_, K_C_Q, K_M_q},     F_I(dof_quit_filer),   get_str_setting_none },
 { EFAM_EXEC, "Quit to Home dir",    "Quit to Home directory",
   {K_Q_, K_M_Q, K_MC_Q},    F_I(dof_quit_home_dir), get_str_setting_none },
 { F_LM_FLNM, "Edit ",               "Edit file (project file)",
   {K_e_, K_F11,       KNA}, F_I(dof_open_file),    get_str_setting_none },
 { F_LM_FLNM, "View File",           "View file (Read-Only)",
   {K_E_,         KNA, KNA}, F_I(dof_open_file_ro), get_str_setting_none },
 { F_LM_FLNM, "Edit ",               "Edit file",
   {K_M_e, K_F12,      KNA}, F_I(dof_open_file_non_recursive), get_str_setting_none },
 { F_LM_FLNM, "Edit(project, history)", "Edit file(project, history)",
   {K_M_E,        KNA, KNA}, F_I(dof_open_file_from_history), get_str_setting_none },
 { F_LM_FLNM, "Open new file",       "Open new file",
   {K_M_n, K_M_N,      KNA}, F_I(dof_open_new_file), get_str_setting_none },
 { F_LM_FLNM, "Open locked file",    "Open locked file",
   {K_MC_E,       KNA, KNA}, F_I(dof_open_locked_file), get_str_setting_none },
 { F_LM_FLNM, "Open files dropped",  "Open files dropped",
   {K_BAKSL,     KNA, KNA}, F_I(dof_drop_files_to_open), get_str_setting_none },
 { EFAM_EXEC, "Open execution log file", "Open exec. log file",
   {K_M_l,        KNA, KNA}, F_I(dof_open_exec_log_file), get_str_setting_none },
 { EFAM_EXEC, "Open project file",   "Open project file",
   {K_M_p, K_M_P,      KNA}, F_I(dof_open_proj_file), get_str_setting_none },
 { EFAM_EXEC, "Enter file/dir",      "Enter file/directory",
   {K_ENTER,      KNA, KNA}, F_I(dof_tap_file),   get_str_setting_none },
 { F_LM_FLNM, "Enter file/dir",      "Enter file/directory",
   {K_M_ENTER,    KNA, KNA}, F_I(dof_tap_file),   get_str_setting_none },
 { EFAM_EXEC, "Parent directory",    "Parent directory",
   {K_C_H, K_BS,       KNA}, F_I(dof_parent_directory), get_str_setting_none },
#ifdef ENABLE_HELP
 { EFLM_NOEX, "Splash",              "Splash screen",
   {K_M_v,        KNA, KNA}, F_I(dof_splash), get_str_setting_none },
 { EFLM_NOEX, "Color pairs",         "Display color pairs",
   {K_M_c,        KNA, KNA}, F_I(dof_display_color_settings), get_str_setting_none },
#endif // ENABLE_HELP

 { EFLM_NOEX, "",                    "Filer Menu-2",
   {KNA,          KNA, KNA}, F_I(dof_filer_menu_0), get_str_setting_none },
 { F_LM_FLNM, "Copy ",           "Copy file",
   {K_c_, K_F01,       KNA}, F_I(dof_copy_file),    get_str_setting_none },
 { F_LM_FLNM, "CpUpd",         "Copy file update",
   {K_C_, K_F02,       KNA}, F_I(dof_copy_file_update), get_str_setting_none },
 { F_LM_FLNM, "Drop to Copy",        "Drop to Copy file",
   {K_M_C,        KNA, KNA}, F_I(dof_drop_files_to_copy), get_str_setting_none },
 { F_LM_CUDI, "Move ",               "Move file",
   {K_m_, K_F03, K_M_m},     F_I(dof_move_file),    get_str_setting_none },
 { F_LM_FLNM, "Renam",               "Rename file",
   {K_n_, K_F04,       KNA}, F_I(dof_rename_file),  get_str_setting_none },
 { F_LM_CUDI, "Drop to Move",        "Drop to Move file",
   {K_M_M,        KNA, KNA}, F_I(dof_drop_files_to_move),    get_str_setting_none },
 { F_LM_FLNM, "Trash",               "Trash file",
   {K_d_, K_F05,       KNA},  F_I(dof_trash_file),   get_str_setting_none },
 { F_LM_FLNM, "Delet",               "Delete file",
   {K_D_, K_F06, K_DEL},      F_I(dof_delete_file),  get_str_setting_none },
 { F_LM_FLNM, "Mark to delete",      "Mark to be Deleted",
   {K_f_,         KNA,  KNA}, F_I(dof_mark_to_delete_file), get_str_setting_none },
 { F_LM_FLNM, "Size zero",           "File size zero",
   {K_F_, K_S_F06,      KNA}, F_I(dof_size_zero_file), get_str_setting_none },
 { F_LM_FLNM, "View",                "View file",
   {K_p_,         KNA, KNA}, F_I(dof_view_file),    get_str_setting_none },
 { F_LM_FLNM, "Tail",                "View tail of file",
   {K_l_,         KNA, KNA}, F_I(dof_tail_file),    get_str_setting_none },
 { EFAM_EXEC, "Find file",           "Find file",
   {K_j_,         KNA, KNA}, F_I(dof_find_file),    get_str_setting_none },

 { EFLM_NOEX, "",                    "Filer Menu-3",
   {KNA,          KNA, KNA}, F_I(dof_filer_menu_0), get_str_setting_none },
 { EFAM_EXEC, "Up",                  "Move cursor UP",
   {K_C_E, K_UP,       KNA}, F_I(dof_up), get_str_setting_none },
 { EFAM_EXEC, "Down",                "Move cursor DOWN",
   {K_C_X, K_DOWN,     KNA}, F_I(dof_down), get_str_setting_none },
 { EFAM_EXEC, "Page up  ",           "Page UP",
   {K_C_R, K_PPAGE, K_LEFT}, F_I(dof_page_up), get_str_setting_none },
 { EFAM_EXEC, "Page down",           "Page DOWN",
   {K_C_C, K_NPAGE, K_RIGHT}, F_I(dof_page_down), get_str_setting_none },

 { EFAM_EXEC, "Top",                 "Top of list",
   {K_C_T, K_M_t, K_HOME},   F_I(dof_top_of_list),  get_str_setting_none },
 { EFAM_EXEC, "Bottom",              "Bottom of list",
   {K_C_B, K_M_b, K_END},    F_I(dof_bottom_of_list), get_str_setting_none },
 { EFAM_EXEC, "Select",              "Select file",
   {K_SP,         KNA, KNA}, F_I(dof_select_file),  get_str_setting_none },
 { EFAM_EXEC, "Select all",          "Select all files",
   {K_a_,         KNA, KNA}, F_I(dof_select_all_files), get_str_setting_none },
 { EFAM_EXEC, "Select none",         "Select no files",
   {K_A_,         KNA, KNA}, F_I(dof_select_no_file), get_str_setting_none },
 { EFAM_EXEC, "Mkdir",               "Make directory",
   {K_k_, K_F07, K_M_k},     F_I(dof_make_directory), get_str_setting_none },
 { EFAM_EXEC, "Chdir",               "Change directory",
   {K_h_, K_F08,       KNA}, F_I(dof_change_directory), get_str_setting_none },
 { EFAM_EXEC, "Beginning dir.",      "Beginning directory",
   {K_TILDE,      KNA, KNA}, F_I(dof_beginning_directory), get_str_setting_none },
 { EFAM_EXEC, "Home directory",      "Home directory",
   {K_BAKQT,      KNA, KNA}, F_I(dof_home_directory), get_str_setting_none },
 { EFAM_EXEC, "Root directory",      "Root directory",
   {K_SLASH,      KNA, KNA}, F_I(dof_root_directory), get_str_setting_none },
 { EFAM_EXEC, "Previous dir.",       "Previous directory",
   {K_b_, K_M_SLASH,   KNA}, F_I(dof_prev_directory), get_str_setting_none },

 { EFLM_NOEX, "",                    "Filer Menu-4",
   {KNA,          KNA, KNA}, F_I(dof_filer_menu_0), get_str_setting_none },
 { F_LM_FLNM, "Exec ",               "Execute command with file",
   {K_x_, K_F09,       KNA}, F_I(dof_exec_command_with_file), get_str_setting_none },
 { F_LM_FLNM, "Execute command w/ files","Execute command with files",
   {K_X_,         KNA, KNA}, F_I(dof_exec_command_with_files), get_str_setting_none },
 { EFAM_EXEC, "Run soon",            "Run command soon",
   {K_M_x,        KNA, KNA}, F_I(dof_run_command_soon), get_str_setting_none },
 { EFAM_EXEC, "Run soon w/ logging", "Run command soon w/ logging",
   {K_M_X,        KNA, KNA}, F_I(dof_run_command_soon_w_log), get_str_setting_none },
 { F_LM_FLNM, "Run  ",               "Run command (relative path)",
   {K_r_, K_F10,       KNA}, F_I(dof_run_command_rel), get_str_setting_none },
 { F_LM_FLNM, "Run command abs.",    "Run command (absolute path)",
   {K_R_,         KNA, KNA}, F_I(dof_run_command_abs), get_str_setting_none },
 { F_LM_FLNM, "Run command(src/dest)","Run command (src/dest)",
   {K_t_,         KNA, KNA}, F_I(dof_run_command_src_dst), get_str_setting_none },
 { F_LM_FLNM, "Run command (sh)",    "Run command (sh path)",
   {K_T_,         KNA, KNA}, F_I(dof_run_command_symlink), get_str_setting_none },
 { EFLM_NOEX, "Real path",           "Convert Symlink to Real path",
   {K_M_L,        KNA, KNA}, F_I(dof_real_path),    get_str_setting_none },
#ifdef ENABLE_HELP
 { EFLM_NOEX, "File List",           "Display File List",
   {K_M_SEMCO,    KNA, KNA}, F_I(doe_view_file_list), get_str_setting_none },
 { EFLM_NOEX, "Key List",            "Display Key List",
   {K_M_QUEST,    KNA, KNA}, F_I(dof_view_func_list), get_str_setting_none },
#endif // ENABLE_HELP

 { EFLM_NOEX, "",                    "Filer Menu-5",
   {KNA,          KNA, KNA}, F_I(dof_filer_menu_0), get_str_setting_none },
 { EFAM_EXEC, "Split pane",          "Split screen into panes",
   {K_VERTB, K_BAKSL,  KNA}, F_I(dof_tog_panes), get_str_filer_panes },
 { EFAM_EXEC, "Switch pane",         "Switch filer pane",
   {K_TAB, K_M_TAB,    KNA}, F_I(dof_tog_panex), get_str_filer_panex },
 { EFAM_EXEC, "Refresh",             "Refresh filer screen",
   {K_COMMA,      KNA, KNA}, F_I(dof_refresh_filer), get_str_setting_none },
 { EFAM_EXEC, "Show dot file",       "Show dot file",
   {K_PERIO,      KNA, KNA}, F_I(dof_tog_show_dot_file), get_str_show_dot_file },
 { EFAM_EXEC, "Show file info.",     "Increment file view mode",
   {K_HYPHE,      KNA, KNA}, F_I(dof_inc_file_view_mode), get_str_file_view_mode },
 { EFAM_EXEC, "Sort mode",           "Increment file sort mode",
   {K_s_,         KNA, KNA}, F_I(dof_inc_file_sort_mode), get_str_file_sort_mode },
 { EFAM_EXEC, "Clear sort mode",     "Clear file sort mode",
   {K_S_,         KNA, KNA}, F_I(dof_clear_file_sort_mode), get_str_setting_none },
 { EFAM_EXEC, "Display key list",    "Increment key list lines",
   {K_M_K,        KNA, KNA}, F_I(dof_inc_key_list_lines), get_str_key_list_lines },
 { EFAM_EXEC, "Code-7F Key-BS",      "Toggle Code-7F Key-BS",
   {K_M_D,        KNA, KNA}, F_I(doe_tog_map_key_7f_bs), get_str_map_key_7f_bs },

 { EFAM_EXEC, "",                    "",
   {KNA,          KNA, KNA}, F_I(NULL),            NULL },
};

#endif // ENABLE_FILER

// End of filerkeys.c
