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

func_key_t filer_func_key_table[] = {
//      "12345678901234567890","123456789012345678901234567890"
// 123456789012345678901234
 {EFNM, "Menu",                "Previous Menu",
   {K_ESC,         KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFLM, "Menu",                "Previous Menu",
   {K_ESC,         KNA, KNA}, F_I(dof_quit_filer),                 get_str_setting_none },

 {EFNM, "",                    "Filer Editor Menu",
   {KNA,           KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFAM, "Quit",                "Quit filer",
   {K_q_, K_C_Q, K_M_q     }, F_I(dof_quit_filer),                 get_str_setting_none },
 {EFNM, "Quit to home dir",    "Quit to Home directory",
   {K_Q_, K_M_Q,       KNA }, F_I(dof_quit_home_dir),              get_str_setting_none },
 {EFNM, "Restart filer",       "Restart filer",
   {K_MC_Q,        KNA, KNA}, F_I(dof_restart_filer),              get_str_setting_none },
 {EFNM, "Edit ",               "Edit file (project file)",
   {K_e_, K_F01,        KNA}, F_I(dof_open_file_recursive),        get_str_setting_none },
 {EFNM, "View File",           "View file (Read-Only)",
   {K_E_,          KNA, KNA}, F_I(dof_open_file_ro),               get_str_setting_none },
 {EFNM, "Edit ",               "Edit file",
   {K_M_e, K_F02,       KNA}, F_I(dof_open_file_non_recursive),    get_str_setting_none },
 {EFNM, "Edit ",               "Edit file",
   {K_M_E,         KNA, KNA}, F_I(dof_open_file_from_history),     get_str_setting_none },
 {EFNM, "Open locked file",    "Open locked file",
   {K_MC_E,        KNA, KNA}, F_I(dof_open_file_locked),           get_str_setting_none },
 {EFNM, "Open new file",       "Open new file",
   {K_M_n, K_M_N,       KNA}, F_I(dof_open_new_file),              get_str_setting_none },
 {EFNM, "Open files input",    "Open files input",
   {K_QUOTA,       KNA, KNA}, F_I(dof_input_files_to_open),        get_str_setting_none },
 {EFNM, "Open files dropped",  "Open files dropped",
   {K_APOST,       KNA, KNA}, F_I(dof_drop_files_to_open),         get_str_setting_none },
 {EFLM, "EntFN",               "Enter cur file name",
   {K_e_, K_n_,         KNA}, F_I(dof_enter_file_name),            get_str_setting_none },
 {EFLM, "EntFN",               "Enter cur file name",
   {K_M_e, K_M_n,       KNA}, F_I(dof_enter_file_name_add),        get_str_setting_none },
 {EFLM, "EntFP",               "Enter cur file path",
   {K_E_, K_N_,         KNA}, F_I(dof_enter_file_path),            get_str_setting_none },
 {EFLM, "EntFP",               "Enter cur file path",
   {K_M_E, K_M_N,       KNA}, F_I(dof_enter_file_path_add),        get_str_setting_none },
 {EFLM, "EntDi",               "Enter current dir path",
   {K_c_, K_C_,         KNA}, F_I(dof_enter_dir_path),             get_str_setting_none },
 {EFLM, "EntDi",               "Enter current dir path",
   {K_m_, K_M_,         KNA}, F_I(dof_enter_dir_path),             get_str_setting_none },
 {EFLM, "EntDi",               "Enter current dir path",
   {K_M_c, K_M_m,       KNA}, F_I(dof_enter_dir_path_add),         get_str_setting_none },
 {EFAM, "Open exec. log file", "Open execution log file",
   {K_M_l,         KNA, KNA}, F_I(dof_open_exec_log_file),         get_str_setting_none },
 {EFAM, "Open project file",   "Open project file",
   {K_M_p, K_M_P,      KNA }, F_I(dof_open_proj_file),             get_str_setting_none },

 {EFNM, "",                    "Filer File Menu",
   {KNA,           KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFNM, "Copy ",               "Copy file",
   {K_c_, K_F03, K_M_c     }, F_I(dof_copy_file),                  get_str_setting_none },
 {EFNM, "CpUpd",               "Copy file update",
   {K_C_, K_S_F03,     KNA }, F_I(dof_copy_file_update),           get_str_setting_none },
 {EFNM, "Drop to Copy",        "Drop to Copy file",
   {K_M_C,         KNA, KNA}, F_I(dof_drop_files_to_copy),         get_str_setting_none },
 {EFNM, "Move ",               "Move file",
   {K_m_, K_F04, K_M_m     }, F_I(dof_move_file),                  get_str_setting_none },
 {EFNM, "MvUpd",               "Move file update",
   {K_M_, K_S_F04,     KNA }, F_I(dof_move_file_update),           get_str_setting_none },
 {EFNM, "Renam",               "Rename file",
   {K_n_, K_F05, K_N_      }, F_I(dof_rename_file),                get_str_setting_none },
 {EFNM, "Drop to Move",        "Drop to Move file",
   {K_M_M,         KNA, KNA}, F_I(dof_drop_files_to_move),         get_str_setting_none },
 {EFNM, "Trash",               "Trash file",
   {K_d_, K_F06,        KNA}, F_I(dof_trash_file),                 get_str_setting_none },
 {EFNM, "Delet",               "Delete file",
   {K_D_, K_S_F06, K_DEL   }, F_I(dof_delete_file),                get_str_setting_none },
 {EFNM, "MkDel",               "Mark to be Deleted",
   {K_f_,          KNA, KNA}, F_I(dof_mark_to_delete_file),        get_str_setting_none },
 {EFNM, "Size zero",           "File size zero",
   {K_F_,          KNA, KNA}, F_I(dof_size_zero_file),             get_str_setting_none },
 {EFNM, "View",                "View file",
   {K_p_,          KNA, KNA}, F_I(dof_view_file),                  get_str_setting_none },
 {EFNM, "Tail",                "View tail of file",
   {K_l_,          KNA, KNA}, F_I(dof_tail_file),                  get_str_setting_none },
 {EFAM, "Find file",           "Find file",
   {K_j_,          KNA, KNA}, F_I(dof_find_file),                  get_str_setting_none },
 {EFAM, "Unzip file",          "Unzip file",
   {K_u_,          KNA, KNA}, F_I(dof_unzip_file),                 get_str_setting_none },
 {EFAM, "Zip file",            "Zip file",
   {K_U_,          KNA, KNA}, F_I(dof_zip_file),                   get_str_setting_none },

 {EFNM, "",                    "Filer Cursor Menu",
   {KNA,           KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFAM, "Up",                  "Move cursor UP",
   {K_C_E, K_UP,        KNA}, F_I(dof_up),                         get_str_setting_none },
 {EFAM, "Down",                "Move cursor DOWN",
   {K_C_X, K_DOWN,      KNA}, F_I(dof_down),                       get_str_setting_none },
 {EFAM, "Page up  ",           "Page UP",
   {K_C_R, K_PPAGE, K_LEFT }, F_I(dof_page_up),                    get_str_setting_none },
 {EFAM, "Page down",           "Page DOWN",
   {K_C_C, K_NPAGE, K_RIGHT}, F_I(dof_page_down),                  get_str_setting_none },
 {EFAM, "Top",                 "Top of list",
   {K_C_T, K_M_t, K_HOME   }, F_I(dof_top_of_list),                get_str_setting_none },
 {EFAM, "Bottom",              "Bottom of list",
   {K_C_B, K_M_b, K_END    }, F_I(dof_bottom_of_list),             get_str_setting_none },
 {EFAM, "Set filter",          "Set file name filter",
   {K_M_f,         KNA, KNA}, F_I(dof_set_filter),                 get_str_setting_none },
 {EFAM, "Select",              "Select file",
   {K_SP,          KNA, KNA}, F_I(dof_select_file),                get_str_setting_none },
 {EFAM, "Select all",          "Select all files",
   {K_a_,          KNA, KNA}, F_I(dof_select_all_files),           get_str_setting_none },
 {EFAM, "Select none",         "Select no files",
   {K_A_,          KNA, KNA}, F_I(dof_select_no_file),             get_str_setting_none },

 {EFNM, "",                    "Filer Directory Menu",
   {KNA,           KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFAM, "Mkdir",               "Make directory",
   {K_k_, K_F07, K_M_k     }, F_I(dof_make_directory),             get_str_setting_none },
 {EFAM, "Chdir",               "Change directory",
   {K_h_, K_F08,        KNA}, F_I(dof_change_directory),           get_str_setting_none },
 {EFAM, "Beginning dir.",      "Beginning directory",
   {K_TILDE,       KNA, KNA}, F_I(dof_beginning_directory),        get_str_setting_none },
 {EFAM, "Home directory",      "Home directory",
   {K_BAKQT,       KNA, KNA}, F_I(dof_home_directory),             get_str_setting_none },
 {EFAM, "Root directory",      "Root directory",
   {K_SLASH,       KNA, KNA}, F_I(dof_root_directory),             get_str_setting_none },
 {EFAM, "Previous dir.",       "Previous directory",
   {K_b_, K_M_SLASH,    KNA}, F_I(dof_prev_directory),             get_str_setting_none },
 {EFNM, "Enter file/dir",      "Enter file/directory",
   {K_ENTER, K_M_ENTER, KNA}, F_I(dof_tap_file),                   get_str_setting_none },
 {EFLM, "Enter file/dir",      "Enter file/directory",
   {K_ENTER,       KNA, KNA}, F_I(dof_tap_file_to_enter),          get_str_setting_none },
 {EFLM, "Enter file/dir",      "Enter file/directory",
   {K_M_ENTER,     KNA, KNA}, F_I(dof_tap_file_to_enter_add),      get_str_setting_none },
 {EFAM, "Parent directory",    "Parent directory",
   {K_C_H, K_BS,        KNA}, F_I(dof_parent_directory),           get_str_setting_none },

 {EFNM, "",                    "Filer Execution Menu",
   {KNA,           KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFNM, "Exec ",               "Execute command with file",
   {K_x_, K_F09,        KNA}, F_I(dof_exec_command_with_file),     get_str_setting_none },
 {EFNM, "Exec w files",        "Execute command with files",
   {K_X_,          KNA, KNA}, F_I(dof_exec_command_with_files),    get_str_setting_none },
 {EFAM, "Run soon",            "Run command soon w/ logging",
   {K_M_x, K_M_r,       KNA}, F_I(dof_run_command_soon),           get_str_setting_none },
 {EFNM, "Run  ",               "Run command (relative path)",
   {K_r_, K_F10,        KNA}, F_I(dof_run_command_rel),            get_str_setting_none },
 {EFNM, "Run abs path",        "Run command (absolute path)",
   {K_R_,          KNA, KNA}, F_I(dof_run_command_abs),            get_str_setting_none },
 {EFNM, "Run (s/d-dir)",       "Run command (src/dest dir)",
   {K_t_,          KNA, KNA}, F_I(dof_run_command_src_dst_dir),    get_str_setting_none },
 {EFNM, "Run (s/d-file)",      "Run command (src/dest file)",
   {K_T_,          KNA, KNA}, F_I(dof_run_command_src_dst_file),   get_str_setting_none },
 {EFNM, "Real path",           "Convert Symlink to Real path",
   {K_M_L,         KNA, KNA}, F_I(dof_real_path),                  get_str_setting_none },
#ifdef ENABLE_HELP
 {EFNM, "Splash",              "Splash screen",
   {K_M_v,         KNA, KNA}, F_I(dof_splash),                     get_str_setting_none },
 {EFNM, "File List",           "Display File List",
   {K_M_SEMCO,     KNA, KNA}, F_I(dof_view_file_list),             get_str_setting_none },
 {EFNM, "Key List",            "Display Key List",
   {K_M_QUEST,     KNA, KNA}, F_I(dof_view_func_list),             get_str_setting_none },
#endif // ENABLE_HELP

 {EFNM, "",                    "Filer Mode Menu",
   {KNA,           KNA, KNA}, F_I(dof_menu_0),                     get_str_setting_none },
 {EFAM, "Split pane",          "Split screen into panes",
   {K_VERTB, K_M_VERTB, KNA}, F_I(dof_tog_panes),                  get_str_filer_panes },
 {EFAM, "Switch pane",         "Switch filer pane",
   {K_TAB, K_M_TAB,     KNA}, F_I(dof_tog_panex),                  get_str_filer_panex },
 {EFAM, "Refresh",             "Refresh filer screen",
   {K_COMMA,       KNA, KNA}, F_I(dof_refresh_filer),              get_str_setting_none },
 {EFAM, "Show dot file",       "Show dot file",
   {K_PERIO,       KNA, KNA}, F_I(dof_tog_show_dot_file),          get_str_show_dot_file },
 {EFAM, "Show file info.",     "Increment file view mode",
   {K_HYPHE,       KNA, KNA}, F_I(dof_inc_file_view_mode),         get_str_file_view_mode },
 {EFAM, "Sort mode",           "Increment file sort mode",
   {K_s_,          KNA, KNA}, F_I(dof_inc_file_sort_mode),         get_str_file_sort_mode },
 {EFAM, "Clear sort mode",     "Clear file sort mode",
   {K_S_,          KNA, KNA}, F_I(dof_clear_file_sort_mode),       get_str_setting_none },
 {EFAM, "Show zebra stripes",  "Show zebra stripes",
   {K_UNDLN,       KNA, KNA}, F_I(dof_tog_show_zebra_striping),    get_str_show_zebra_striping },
 {EFAM, "Display key list",    "Increment key list lines",
   {K_M_K,         KNA, KNA}, F_I(dof_inc_key_list_lines),         get_str_key_list_lines },

 {EFNM, "",                    "",
   {KNA,           KNA, KNA}, F_I(NULL),            NULL },
};

#endif // ENABLE_FILER

// End of filerkeys.c
