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
//      "1234567890123456789012","123456789012345678901234567890"
// 123456789,123456789,123456789,123456789     123456789012345678901234567890
 {EFNM, "Menu",                  "0: Previous Menu",
  {K_ESC    ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFLM, "Menu",                  "Previous Menu",
  {K_M_BAKQT,K_M_TILDE,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },

 {EFAM, "",                      "1: Filer - Cursor Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFAM, "Quit",                  "Quit filer",
  {K_q_     ,KNA      ,KNA      ,KNA      },F_I(dof_quit),                get_str_none },
 {EFAM, "Quit",                  "Quit filer",
  {K_C_q    ,K_M_q    ,KNA      ,KNA      },F_I(dof_quit_immediate),      get_str_none },
 {EFNM, "Quit to home dir",      "Quit to Home directory",
  {K_Q_     ,K_M_Q    ,KNA      ,KNA      },F_I(dof_quit_home_dir),       get_str_none },
 {EFNM, "Restart filer",         "Restart filer",
  {K_MC_q   ,KNA      ,KNA      ,KNA      },F_I(dof_restart_filer),       get_str_none },
 {EFLM, "Quit",                  "Quit filer (List mode)",
  {K_ESC    ,KNA      ,KNA      ,KNA      },F_I(dof_quit),                get_str_none },

 {EFAM, "Up",                    "Move cursor UP",
  {K_C_e    ,K_UP     ,KNA      ,KNA      },F_I(dof_up),                  get_str_none },
 {EFAM, "Down",                  "Move cursor DOWN",
  {K_C_x    ,K_DOWN   ,KNA      ,KNA      },F_I(dof_down),                get_str_none },
 {EFAM, "Page up",               "Page UP",
  {K_C_r    ,K_PPAGE  ,KNA      ,KNA      },F_I(dof_page_up),             get_str_none },
 {EFAM, "half Page up",          "half Page UP",
  {K_C_s    ,K_LEFT   ,KNA      ,KNA      },F_I(dof_half_page_up),        get_str_none },
 {EFAM, "Page down",             "Page DOWN",
  {K_C_c    ,K_NPAGE  ,KNA      ,KNA      },F_I(dof_page_down),           get_str_none },
 {EFAM, "half Page down",        "half Page DOWN",
  {K_C_d    ,K_RIGHT  ,KNA      ,KNA      },F_I(dof_half_page_down),           get_str_none },
 {EFAM, "Top",                   "Top of list",
  {K_C_t    ,K_M_t    ,K_HOME   ,KNA      },F_I(dof_top_of_list),         get_str_none },
 {EFAM, "Bottom",                "Bottom of list",
  {K_C_b    ,K_M_b    ,K_END    ,KNA      },F_I(dof_bottom_of_list),      get_str_none },
 {EFAM, "Set filter",            "Set file name filter",
  {K_M_f    ,KNA      ,KNA      ,KNA      },F_I(dof_set_filter),          get_str_none },
 {EFAM, "Select",                "Select file",
  {K_SP     ,KNA      ,KNA      ,KNA      },F_I(dof_select_file),         get_str_none },
 {EFAM, "Select all",            "Select all files",
  {K_a_     ,KNA      ,KNA      ,KNA      },F_I(dof_select_all_files),    get_str_none },
 {EFAM, "Select all",            "Select all files",
  {K_C_a    ,KNA      ,KNA      ,KNA      },F_I(dof_select_all_regular_files),get_str_none },
 {EFAM, "Select none",           "Select no files",
  {K_A_     ,KNA      ,KNA      ,KNA      },F_I(dof_select_no_file),      get_str_none },
 {EFAM, "Mark ",                 "Start selection",
  {K_C_n    ,K_F05    ,K_M_5    ,KNA      },F_I(dof_toggle_selecting_files),get_str_none },
 {EFAM, "ClipF",                 "Send file name to clip-board",
  {K_M_AMPSD,K_F07    ,K_S_F07  ,K_M_7    },F_I(dof_clip_file_name),      get_str_none },
 {EFAM, "ClipP",                 "Send file path to clip-board",
  {K_M_ASTER,K_F08    ,K_S_F08  ,K_M_8    },F_I(dof_clip_file_path),      get_str_none },

 {EFAM, "",                      "2: Filer - Editor Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFAM, "Edit ",                 "Edit file (project file)",
  {K_e_     ,K_F11    ,KNA      ,KNA      },F_I(dof_open_files_recursive), get_str_none },
 {EFAM, "View File",             "View file (Read-Only)",
  {K_E_     ,KNA      ,KNA      ,KNA      },F_I(dof_open_files_ro),        get_str_none },
 {EFAM, "Edit ",                 "Edit file",
  {K_M_e    ,K_F12    ,KNA      ,KNA      },F_I(dof_open_files_non_recursive),get_str_none },
 {EFAM, "Edit ",                 "Edit file",
  {K_M_E    ,KNA      ,KNA      ,KNA      },F_I(dof_open_files_from_history),get_str_none },
 {EFNM, "Open locked file",      "Open locked file",
  {K_MC_e   ,KNA      ,KNA      ,KNA      },F_I(dof_open_files_locked),    get_str_none },
 {EFNM, "Open new file",         "Open new file",
  {K_M_n    ,K_M_N    ,KNA      ,KNA      },F_I(dof_open_new_file),       get_str_none },
 {EFNM, "Open files input",      "Open files input",
  {K_QUOTA  ,KNA      ,KNA      ,KNA      },F_I(dof_input_files_to_open), get_str_none },
 {EFNM, "Open files dropped",    "Open files dropped",
  {K_APOST  ,KNA      ,KNA      ,KNA      },F_I(dof_drop_files_to_open),  get_str_none },
 {EFAM, "Open exec. log file",   "Open execution log file",
  {K_M_l    ,KNA      ,KNA      ,KNA      },F_I(dof_open_exec_log_file),  get_str_none },
 {EFAM, "Open project file",     "Open project file",
  {K_M_p    ,K_M_P    ,KNA      ,KNA      },F_I(dof_open_proj_file),      get_str_none },

 {EFAM, "",                      "3: Filer - File Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFNM, "Copy ",                 "Copy file",
  {K_c_     ,K_M_c    ,KNA      ,KNA      },F_I(dof_copy_file_update),    get_str_none },
 {EFNM, "CpUpd",                 "Copy file update",
  {K_C_     ,KNA      ,KNA      ,KNA      },F_I(dof_copy_file_force),     get_str_none },
 {EFNM, "Drop to Copy",          "Drop to Copy file",
  {K_M_C    ,KNA      ,KNA      ,KNA      },F_I(dof_drop_files_to_copy),  get_str_none },
 {EFNM, "Move ",                 "Move file",
  {K_m_     ,K_M_m    ,KNA      ,KNA      },F_I(dof_move_file),           get_str_none },
 {EFNM, "MvUpd",                 "Move file update",
  {K_M_     ,KNA      ,KNA      ,KNA      },F_I(dof_move_file_update),    get_str_none },
 {EFNM, "Drop to Move",          "Drop to Move file",
  {K_M_M    ,KNA      ,KNA      ,KNA      },F_I(dof_drop_files_to_move),  get_str_none },
 {EFNM, "Renam",                 "Rename file",
  {K_n_     ,K_N_     ,KNA      ,KNA      },F_I(dof_rename_file),         get_str_none },
 {EFNM, "Trash",                 "Trash file",
  {K_d_     ,KNA      ,KNA      ,KNA      },F_I(dof_trash_file),          get_str_none },
 {EFNM, "Delet",                 "Delete file",
  {K_D_     ,AK_DEL   ,KNA      ,KNA      },F_I(dof_delete_file),         get_str_none },
 {EFNM, "MkDel",                 "Mark to be Deleted",
  {K_f_     ,KNA      ,KNA      ,KNA      },F_I(dof_mark_to_delete_file), get_str_none },
 {EFNM, "Size zero",             "File size zero",
  {K_F_     ,KNA      ,KNA      ,KNA      },F_I(dof_size_zero_file),      get_str_none },
 {EFNM, "View",                  "View file",
  {K_p_     ,KNA      ,KNA      ,KNA      },F_I(dof_view_file),           get_str_none },
 {EFNM, "Tail",                  "View tail of file",
  {K_l_     ,KNA      ,KNA      ,KNA      },F_I(dof_tail_file),           get_str_none },
 {EFAM, "Unzip file",            "Unzip file",
  {K_u_     ,KNA      ,KNA      ,KNA      },F_I(dof_unzip_file),          get_str_none },
 {EFAM, "Zip file",              "Zip file",
  {K_U_     ,KNA      ,KNA      ,KNA      },F_I(dof_zip_file),            get_str_none },
 {EFAM, "FindB",                 "Find file first backward",
  {K_C_RBRAK,K_S_F03  ,KNA      ,KNA      },F_I(dof_find_file_first_backw),get_str_none },
 {EFAM, "FindF",                 "Find file first forward",
  {K_C_BAKSL,K_S_F04  ,KNA      ,KNA      },F_I(dof_find_file_first_forw), get_str_none },
 {EFAM, "FindB",                 "Find file next backward",
  {K_M_SHARP,K_F03    ,K_M_3    ,KNA      },F_I(dof_find_file_next_backw),get_str_none },
 {EFAM, "FindF",                 "Find file next forward",
  {K_M_DOLLA,K_F04    ,K_M_4    ,KNA      },F_I(dof_find_file_next_forw), get_str_none },

 {EFAM, "",                      "4: Filer - Directory Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFAM, "Chdir",                 "Change directory",
  {K_h_     ,K_M_h    ,KNA      ,KNA      },F_I(dof_change_directory),    get_str_none },
 {EFNM, "Real path",             "Convert Symlink to Real path",
  {K_H_     ,K_M_H    ,KNA      ,KNA      },F_I(dof_real_path),           get_str_none },
 {EFAM, "Mkdir",                 "Make directory",
  {K_k_     ,K_M_k    ,KNA      ,KNA      },F_I(dof_make_directory),      get_str_none },
 {EFAM, "Beginning dir.",        "Beginning directory",
  {K_TILDE  ,KNA      ,KNA      ,KNA      },F_I(dof_beginning_directory), get_str_none },
 {EFAM, "Home directory",        "Home directory",
  {K_BAKQT  ,KNA      ,KNA      ,KNA      },F_I(dof_home_directory),      get_str_none },
 {EFAM, "Root directory",        "Root directory",
  {K_SLASH  ,KNA      ,KNA      ,KNA      },F_I(dof_root_directory),      get_str_none },
 {EFAM, "Previous dir.",         "Previous directory",
  {K_b_     ,K_M_SLASH,KNA      ,KNA      },F_I(dof_prev_directory),      get_str_none },
 {EFNM, "Enter file/dir",        "Enter file/directory",
  {K_ENTER  ,K_M_ENTER,KNA      ,KNA      },F_I(dof_tap_file),            get_str_none },
 {EFAM, "Parent directory",      "Parent directory",
  {K_C_h    ,AK_BS    ,K_C_g    ,KNA      },F_I(dof_parent_directory),    get_str_none },

 {EFAM, "",                      "5: Filer - Execution Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFNM, "Exec ",                 "Execute command with file",
  {K_x_     ,KNA      ,KNA      ,KNA      },F_I(dof_exec_for_each_files), get_str_none },
 {EFNM, "Exec w files",          "Execute command with files",
  {K_X_     ,KNA      ,KNA      ,KNA      },F_I(dof_exec_for_each_paths), get_str_none },
 {EFAM, "Run soon",              "Run command soon",
  {K_M_x    ,KNA      ,KNA      ,KNA      },F_I(dof_run_command_immediate),get_str_none },
 {EFAM, "Run cur dir",           "Run command in the cur-dir",
  {K_M_X    ,KNA      ,KNA      ,KNA      },F_I(dof_run_command_cur_dir), get_str_none },
 {EFNM, "Run  ",                 "Run command (relative path)",
  {K_r_     ,KNA      ,KNA      ,KNA      },F_I(dof_run_for_files),       get_str_none },
 {EFNM, "Run abs path",          "Run command (absolute path)",
  {K_R_     ,KNA      ,KNA      ,KNA      },F_I(dof_run_for_paths),       get_str_none },
 {EFAM, "Run from shell",        "Run script from shell",
  {K_M_r    ,KNA      ,KNA      ,KNA      },F_I(dof_run_command_shell),   get_str_none },
 {EFAM, "Run w/ symlink",        "Run command w/ symlink target",
  {K_M_R    ,KNA      ,KNA      ,KNA      },F_I(dof_run_for_symlink),     get_str_none },
 {EFNM, "Run (s/d-dir)",         "Run command (src/dest dir)",
  {K_t_     ,KNA      ,KNA      ,KNA      },F_I(dof_run_for_src_dst_files),get_str_none },
 {EFNM, "Run (s/d-file)",        "Run command (src/dest file)",
  {K_T_     ,KNA      ,KNA      ,KNA      },F_I(dof_run_for_src_dst_paths),get_str_none },
#ifdef ENABLE_HELP
 {EFNM, "Splash",                "Splash screen",
  {K_M_v    ,KNA      ,KNA      ,KNA      },F_I(dof_splash),              get_str_none },
 {EFNM, "File List",             "Display File List",
  {K_M_SEMCO,KNA      ,KNA      ,KNA      },F_I(dof_view_file_list),      get_str_none },
 {EFNM, "Key List",              "Display Key List",
  {K_M_QUEST,KNA      ,KNA      ,KNA      },F_I(dof_view_func_list),      get_str_none },
#endif // ENABLE_HELP

 {EFAM, "",                      "6: Filer - Enter file/dir Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFLM, "Enter",                 "Enter file/directory",
  {K_ENTER  ,KNA      ,KNA      ,KNA      },F_I(dof_tap_file_to_enter),   get_str_none },
 {EFLM, "Enter",                 "Enter file/directory",
  {K_M_ENTER,KNA      ,KNA      ,KNA      },F_I(dof_tap_file_to_enter_add),get_str_none },
 {EFLM, "EntFN",                 "Enter cur file name",
  {K_n_     ,KNA      ,KNA      ,KNA      },F_I(dof_enter_file_name),     get_str_none },
 {EFLM, "EntFN",                 "Enter cur file name",
  {K_M_n    ,KNA      ,KNA      ,KNA      },F_I(dof_enter_file_name_add), get_str_none },
 {EFLM, "EntFP",                 "Enter cur file path",
  {K_N_     ,KNA      ,KNA      ,KNA      },F_I(dof_enter_file_path),     get_str_none },
 {EFLM, "EntFP",                 "Enter cur file path",
  {K_M_N    ,KNA      ,KNA      ,KNA      },F_I(dof_enter_file_path_add), get_str_none },
 {EFLM, "EntDi",                 "Enter current dir path",
  {K_c_     ,K_m_     ,K_C_     ,K_M_     },F_I(dof_enter_dir_path),      get_str_none },
 {EFLM, "EntDi",                 "Enter current dir path",
  {K_M_c    ,K_M_m    ,K_M_C    ,K_M_M    },F_I(dof_enter_dir_path_add),  get_str_none },

 {EFAM, "",                      "7: Filer - Mode Menu",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(dof_menu_0),              get_str_none },
 {EFAM, "Split pane",            "Split screen into panes",
  {K_VERTB  ,K_M_VERTB,KNA      ,KNA      },F_I(dof_tog_panes),           get_str_filer_panes },
 {EFAM, "Switch pane",           "Switch filer pane",
  {K_TAB    ,K_M_TAB  ,KNA      ,KNA      },F_I(dof_tog_panex),           get_str_filer_panex },
 {EFAM, "Refresh",               "Refresh filer screen",
  {K_COMMA  ,K_SEMCO  ,KNA      ,KNA      },F_I(dof_refresh_filer),       get_str_none },
 {EFAM, "Show dot file",         "Show dot file",
  {K_PERIO  ,KNA      ,KNA      ,KNA      },F_I(dof_tog_show_dot_file),   get_str_show_dot_file },
 {EFAM, "Show file info.",       "Increment file view mode",
  {K_HYPHE  ,KNA      ,KNA      ,KNA      },F_I(dof_inc_file_view_mode),  get_str_file_view_mode },
 {EFAM, "Sort mode",             "Increment file sort mode",
  {K_s_     ,KNA      ,KNA      ,KNA      },F_I(dof_inc_file_sort_mode),  get_str_file_sort_mode },
 {EFAM, "Clear sort mode",       "Clear file sort mode",
  {K_S_     ,KNA      ,KNA      ,KNA      },F_I(dof_clear_file_sort_mode),get_str_none },
 {EFAM, "Show zebra stripes",    "Show zebra stripes",
  {K_UNDLN  ,KNA      ,KNA      ,KNA      },F_I(dof_tog_zebra_striping),  get_str_zebra_striping },
 {EFAM, "Display key list",      "Increment key list lines",
  {K_MC_k   ,KNA      ,KNA      ,KNA      },F_I(dof_inc_key_list_lines),  get_str_key_list_lines },
 {EFAM, "Display key list",      "Toggle Map key CDEL to BS",
  {K_MC_v   ,KNA      ,KNA      ,KNA      },F_I(dof_tog_map_key_7f_bs),   get_str_map_key_7f_bs },

 {EFAM, "",                      "",
  {KNA      ,KNA      ,KNA      ,KNA      },F_I(NULL),            NULL },
};

#endif // ENABLE_FILER

// End of filerkeys.c
