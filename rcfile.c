/**************************************************************************
 *   rcfile.c                                                             *
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

#include "headers.h"

#ifdef ENABLE_RC

PRIVATE int read_rc_path(const char *rc_file_path, int complain);
PRIVATE int read_rc_path__(const char *rc_file_path, int complain);

#ifdef ENABLE_SYNTAX
PRIVATE int add_file_type(const char *file_type_name, const char *regexp_file_name);
PRIVATE int add_color_syntax(const char *regexp_start, const char *regexp_end, int bgc, int fgc);
PRIVATE int link_color_syntax_w_file_type(file_type_t *file_type, color_syntax_t *clr_syntax);
#endif // ENABLE_SYNTAX

PRIVATE void rcfile_error(const char *msg, ...);

const char *rc_file_path_reading = NULL;
#define MAX_RC_NEST_COUNT		3
int rc_file_nest_count = 0;

// The main rc file function, tries to open the rc file
// config file priority:
//		'pwd'/.berc
//		$HOME/.berc
//		SYSCONFDIR/berc
int read_rc_file(const char *rc_file_name)
{
	char rc_file_path[MAX_PATH_LEN+1];

	// read ./.berc
	if (get_start_dir()) {
		snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/.%s", get_start_dir(), rc_file_name);
		if (read_rc_path(rc_file_path, 0) == 0)
			return 0;
	}
	// read $HOME/.berc
	snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/.%s", get_home_dir(), rc_file_name);
	if (read_rc_path(rc_file_path, 0) == 0)
		return 0;

///#define SYSCONFDIR		"/usr/local/etc"
#ifdef SYSCONFDIR
	// read /usr/local/etc/berc
	snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/%s", SYSCONFDIR, rc_file_name);
	/* Try to open system wide rc file */
	if (read_rc_path(rc_file_path, 0) == 0)
		return 0;
#endif // SYSCONFDIR
	return -1;
}

PRIVATE int parse_number(void);
PRIVATE int parse_color_default(void);
PRIVATE int parse_color_title(void);
PRIVATE int parse_color_status(void);
PRIVATE int parse_color_key_list(void);
PRIVATE int parse_color_key_list2(void);
PRIVATE int parse_color_text_normal(void);
PRIVATE int parse_color_text_selected(void);
PRIVATE int parse_color_text_selected2(void);
PRIVATE int parse_color_warning(void);
PRIVATE int parse_color_warning2(void);
PRIVATE int parse_color_warning3(void);
PRIVATE int parse_color_cursor(void);
PRIVATE int parse_color_cursor_line(void);
PRIVATE int parse_color_line_number(void);
PRIVATE int parse_color_menu_frame(void);
PRIVATE int parse_color_menu_item(void);
PRIVATE int parse_color_menu_selected(void);
PRIVATE int parse_color_input(void);
#ifdef ENABLE_SYNTAX
PRIVATE int parse_file_type(void);
PRIVATE int parse_color_syntax(void);
PRIVATE int parse_tab_size(void);
#endif // ENABLE_SYNTAX
PRIVATE int parse_include(void);

PRIVATE int parse_bgc_fgc_register(int color_idx);
PRIVATE int parse_bgc_fgc(int *bgc, int *fgc);
PRIVATE int parse_key_names(key_code_t *keys, int max_keys_bind);
PRIVATE int parse_word(char *buffer, int buf_len);
#ifdef ENABLE_SYNTAX
PRIVATE int parse_string(char *buffer, int buf_len);
#endif // ENABLE_SYNTAX

PRIVATE int parse_color_name(int *bright, int *color);
PRIVATE int parse_keyword(const char **rc_line_ptr, const char *keyword);

// commands which take number parameter
// Ex. "backup_files 3"
struct /*_rc_cmd_idx_*/ {
	char *command;
	mode_idx_t mode_idx;
} rc_cmd_idx[] = {
 { "draw_cursor",		APMD_DRAW_CURSOR,		},
 { "key_list_lines",	APMD_KEY_LINES,			},
 { "map_key_7f_bs",		APMD_MAP_KEY_7F_BS,		},
#ifdef ENABLE_UTF8
 { "utf8",				APMD_UTF8,				},
#endif // ENABLE_UTF8

 { "editor_panes",		EDMD_EDITOR_PANES,		},
 { "dual_scroll",		EDMD_DUAL_SCROLL,		},
 { "continuous_view",	EDMD_CONT_VIEW,			},
 { "show_ruler",		EDMD_SHOW_RULER,		},
 { "show_line_num",		EDMD_SHOW_LINE_NUMBER,	},
 { "cursor_center",		EDMD_CURS_POSITIONING,	},
 { "auto_indent",		EDMD_AUTO_INDENT,		},
 { "backup_files",		EDMD_BACKUP_FILES,		},
 { "ignore_case",		EDMD_IGNORE_CASE,		},
#ifdef ENABLE_REGEX
 { "regexp",			EDMD_USE_REGEXP,		},
#endif // ENABLE_REGEX
#ifdef ENABLE_SYNTAX
 { "syntax_hl",			EDMD_SYNTAX_HIGHLIGHT,	},
 { "tab_eol_notation",	EDMD_TAB_EOL_NOTATION,	},
#endif // ENABLE_SYNTAX

 { "show_dot_file",		FLMD_SHOW_DOT_FILE,		},
 { "show_file_info",	FLMD_SHOW_FILE_INFO,	},
 { "sort_by",			FLMD_FILE_SORT_BY,		},
 { "filer_panes",		FLMD_FILER_PANES,		},
};

// commands which take various type of parameter
// Ex. "color_default -white +black"
struct /*_rc_cmd_func_*/ {
	char *command;
	int (*func)();
} rc_cmd_func[] = {
	{ "color_default",			parse_color_default,		},
	{ "color_title",			parse_color_title,			},
	{ "color_status",			parse_color_status,			},
	{ "color_key_list",			parse_color_key_list,		},
	{ "color_key_list2",		parse_color_key_list2,		},
	{ "color_text_normal",		parse_color_text_normal,	},
	{ "color_text_selected",	parse_color_text_selected,	},
	{ "color_text_selected2",	parse_color_text_selected2,	},
	{ "color_warning",			parse_color_warning,		},
	{ "color_warning2",			parse_color_warning2,		},
	{ "color_warning3",			parse_color_warning3,		},
	{ "color_cursor",			parse_color_cursor,			},
	{ "color_cursor_line",		parse_color_cursor_line,	},
	{ "color_line_number",		parse_color_line_number,	},
	{ "color_menu_frame",		parse_color_menu_frame,		},
	{ "color_menu_item",		parse_color_menu_item,		},
	{ "color_menu_selected",	parse_color_menu_selected,	},
	{ "color_input",			parse_color_input,			},
#ifdef ENABLE_SYNTAX
	{ "file_type",				parse_file_type,			},
	{ "color_syntax",			parse_color_syntax,			},
	{ "tab_size",				parse_tab_size,				},
#endif // ENABLE_SYNTAX
	{ "include",				parse_include,				},
};

// Parse the RC file
#define IS_CHAR_FOR_ID(chr)			(isalnum(chr) || strchr__("_-", chr) != NULL)
PRIVATE const char *rc_line_ptr;
PRIVATE int rc_line_num = 0;

PRIVATE int read_rc_path(const char *rc_file_path, int complain)
{
	const char *rc_file_path_save;
	int ret;

	rc_file_path_save = rc_file_path_reading;
	rc_file_path_reading = rc_file_path;
	ret = read_rc_path__(rc_file_path, complain);
	rc_file_path_reading = rc_file_path_save;
	return ret;
}
PRIVATE int read_rc_path__(const char *rc_file_path, int complain)
{
	FILE *fp;
	char rc_line_buf[MAX_SCRN_LINE_BUF_LEN+1];
	const char *begin;
	char cmd[MAX_SCRN_LINE_BUF_LEN+1];
	int idx;
	int val;
	func_key_table_t *func_key_table;
#define MAX_KEYS_BOUND	3
	key_code_t keys[MAX_KEYS_BOUND];

///#ifdef ENABLE_SYNTAX
///	free_file_types();
///#endif // ENABLE_SYNTAX

///flf_d_printf("Attempting to read rc file %s ...\n", rc_file_path);
	rc_line_num = 0;
	if ((fp = fopen(rc_file_path, "r")) == NULL) {
///flf_d_printf("No rc file %s\n", rc_file_path);
		/* complain about the file not existing */
		if (complain) {
			rcfile_error(_("Unable to open config file: %s, %s\n"),
			 rc_file_path, strerror(errno));
		}
		return 1;
	}
	_mlc_memorize_count
///flf_d_printf("Reading rc file %s ...\n", rc_file_path);
	while (fgets(rc_line_buf, MAX_SCRN_LINE_BUF_LEN, fp) != 0) {
		remove_line_tail_lf(rc_line_buf);
///flf_d_printf("parsing_line: %s\n", rc_line_buf);
		rc_line_ptr = rc_line_buf;
		rc_line_num++;
		if (SKIP_SPACE(rc_line_ptr))
			continue;
		if (*rc_line_ptr == '#') {
			continue;		// Skip comment line
		}
		begin = rc_line_ptr;
		for ( ; *rc_line_ptr; rc_line_ptr++) {
			if (IS_CHAR_FOR_ID(*rc_line_ptr) == 0)
				break;
		}
		strlcpy__(cmd, begin, rc_line_ptr - begin);
///flf_d_printf("rc_command: %s\n", cmd);
		if (strlen(cmd)) {
			for (idx = 0; idx < ARRAY_SIZE_OF(rc_cmd_idx); idx++) {
				if (strcmp(rc_cmd_idx[idx].command, cmd) == 0) {
					val = parse_number();
					if (val >= 0) {
						set_mode_idx_val(rc_cmd_idx[idx].mode_idx, val);
					}
					goto read_rc_path_match;
				}
			}
			for (idx = 0; idx < ARRAY_SIZE_OF(rc_cmd_func); idx++) {
				if (strcmp(rc_cmd_func[idx].command, cmd) == 0) {
					(*rc_cmd_func[idx].func)();
					goto read_rc_path_match;
				}
			}
			if ((func_key_table = get_func_table_from_func_id(cmd)) != NULL) {
///
flf_d_printf("parsing: %s\n", rc_line_ptr);
				parse_key_names(keys, MAX_KEYS_BOUND);
				if (keys[0] >= 0) {
///
flf_d_printf("clear_keys_bound_to_func(%s)\n", func_key_table->func_id);
					clear_keys_if_bound(keys);
					clear_keys_bound_to_func(func_key_table);
					bind_key_to_func(func_key_table, keys);
					goto read_rc_path_match;
				}
			}
read_rc_path_match:;
		}
	}
	_mlc_differ_count
	if (fclose(fp) != 0) {
		return 1;
	}
	return 0;
}

#ifdef ENABLE_DEBUG
void dump_app_mode(void)
{
	int idx;

	for (idx = 0; idx < ARRAY_SIZE_OF(rc_cmd_idx); idx++) {
		flf_d_printf("%s = %s\n",
		 rc_cmd_idx[idx].command, get_str_mode_idx_val(rc_cmd_idx[idx].mode_idx));
	}
}
#endif // ENABLE_DEBUG

PRIVATE int parse_number(void)
{
	int val;

	if (sscanf(rc_line_ptr, "%d", &val) == 0) {
		rcfile_error(_("Not valid number: %s\n"), rc_line_ptr);
		return -1;
	}
	return val;
}

PRIVATE int parse_color_default(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_DEFAULT);
}
PRIVATE int parse_color_title(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TITLE);
}
PRIVATE int parse_color_status(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_STATUS);
}
PRIVATE int parse_color_key_list(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_KEY_LIST);
}
PRIVATE int parse_color_key_list2(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_KEY_LIST2);
}
PRIVATE int parse_color_text_normal(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TEXT_NORMAL);
}
PRIVATE int parse_color_text_selected(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TEXT_SELECTED);
}
PRIVATE int parse_color_text_selected2(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_TEXT_SELECTED2);
}
PRIVATE int parse_color_warning(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_WARNING);
}
PRIVATE int parse_color_warning2(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_WARNING2);
}
PRIVATE int parse_color_warning3(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_WARNING3);
}
PRIVATE int parse_color_cursor(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_CURSOR_CHAR);
}
PRIVATE int parse_color_cursor_line(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_CURSOR_LINE);
}
PRIVATE int parse_color_line_number(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_LINE_NUMBER);
}
PRIVATE int parse_color_menu_frame(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_MENU_FRAME);
}
PRIVATE int parse_color_menu_item(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_MENU_ITEM);
}
PRIVATE int parse_color_menu_selected(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_MENU_SELECTED);
}
PRIVATE int parse_color_input(void)
{
	return parse_bgc_fgc_register(ITEM_COLOR_IDX_INPUT);
}

PRIVATE int fgc, bgc;

#ifdef ENABLE_SYNTAX
PRIVATE int parse_file_type(void)
{
#define MAX_FILE_TYPE_NAME	40
#define MAX_REGEXP_LEN		256
	char file_type_name[MAX_FILE_TYPE_NAME+1] = "";
	char file_name_regexp[MAX_REGEXP_LEN+1] = "";

	parse_string(file_type_name, MAX_FILE_TYPE_NAME);
	parse_string(file_name_regexp, MAX_REGEXP_LEN);
	return add_file_type(file_type_name, file_name_regexp);
}

// Parse the color stuff into the item_colors array
PRIVATE int parse_color_syntax(void)
{
	char regexp_start[MAX_REGEXP_LEN+1] = "";
	char regexp_end[MAX_REGEXP_LEN+1] = "";

	while (*rc_line_ptr) {
		if (SKIP_SPACE(rc_line_ptr))
			break;
///flf_d_printf("parsing: %s", rc_line_ptr);
		switch (*rc_line_ptr) {
		case '<':
			rc_line_ptr++;
			if (parse_string(regexp_start, MAX_REGEXP_LEN))
				return 1;
			break;
		case '>':
			rc_line_ptr++;
			if (parse_string(regexp_end, MAX_REGEXP_LEN))
				return 1;
			break;
		case '=':
			rc_line_ptr++;
			if (parse_string(regexp_start, MAX_REGEXP_LEN))
				return 1;
			break;
		case '+':
		case '-':
		case ':':
			parse_bgc_fgc(&bgc, &fgc);
			break;
		default:
			rcfile_error(_("color %s not understood.\n"), rc_line_ptr);
			return 1;
		}
	}
	return add_color_syntax(regexp_start, regexp_end, bgc, fgc);
}
PRIVATE int parse_tab_size(void)
{
	int tab_size;

	SKIP_SPACE(rc_line_ptr);
	tab_size = DEFAULT_TAB_SIZE;
	if (sscanf(rc_line_ptr, "%d", &tab_size) == 1) {
		tab_size = tab_size ? tab_size : DEFAULT_TAB_SIZE;
	}
	if (cur_file_type) {
		cur_file_type->tab_size = tab_size;
	}
	return 0;
}
#endif // ENABLE_SYNTAX

// "include .emacs.berc"
PRIVATE int parse_include(void)
{
	char dir_part[MAX_PATH_LEN+1];
	char file_part[MAX_PATH_LEN+1];
	char rc_file_path[MAX_PATH_LEN+1];
	int ret = 0;

	SKIP_SPACE(rc_line_ptr);
	if (rc_file_nest_count < MAX_RC_NEST_COUNT) {
		++rc_file_nest_count;
		separate_dir_part_and_file_part(rc_file_path_reading, dir_part, file_part);
		snprintf_(rc_file_path, MAX_PATH_LEN+1, "%s/%s", dir_part, rc_line_ptr);
		ret = read_rc_path(rc_file_path, 1);
		--rc_file_nest_count;
	}
	return ret;
}

PRIVATE int parse_bgc_fgc_register(int color_idx)
{
	int ret;

	ret = parse_bgc_fgc(&bgc, &fgc);
	register_item_color(color_idx, bgc, fgc);
	return ret;
}
PRIVATE int parse_bgc_fgc(int *bgc, int *fgc)
{
	int bright;

	*fgc = *bgc = -1;
	while (*rc_line_ptr) {
		if (SKIP_SPACE(rc_line_ptr))
			break;
///flf_d_printf("parsing: %s", rc_line_ptr);
		switch (*rc_line_ptr) {
		case '-':
			rc_line_ptr++;
			parse_color_name(&bright, bgc);
			break;
		case '+':
			rc_line_ptr++;
			parse_color_name(&bright, fgc);
			break;
		default:
			rcfile_error(_("color %s not understood.\n"), rc_line_ptr);
			return 1;
		}
	}
	return 0;
}

// "^Q UP @q"
PRIVATE int parse_key_names(key_code_t *keys, int max_keys_bind)
{
	int key_idx;
	char key_name[MAX_KEY_NAME_LEN+1];
	short key;

	// clear all keys
	for (key_idx = 0; key_idx < max_keys_bind; key_idx++) {
		keys[key_idx] = -1;
	}
	for (key_idx = 0; key_idx < max_keys_bind; ) {
		if (SKIP_SPACE(rc_line_ptr))
			break;
///flf_d_printf("parsing: %s\n", rc_line_ptr);
		if (parse_word(key_name, MAX_KEY_NAME_LEN)) {
			return 1;
		}
		if ((key = key_code_from_short_key_name(key_name)) < 0) {
			// unknown key name
		} else {
			if (key_idx >= max_keys_bind) {
				return 1;
			}
			keys[key_idx] = key;
			key_idx++;
		}
	}
	return 0;
}

PRIVATE int parse_word(char *buffer, int buf_len)
{
	const char *begin;

	if (SKIP_SPACE(rc_line_ptr))
		return 1;
	for (begin = rc_line_ptr; *rc_line_ptr; rc_line_ptr++) {
		if (IS_SPACE(rc_line_ptr) || IS_EOL(rc_line_ptr))
			break;
	}
	if (rc_line_ptr - begin > buf_len)
		return 1;
	strlcpy__(buffer, begin, rc_line_ptr - begin);
	return 0;
}

#ifdef ENABLE_SYNTAX
PRIVATE int parse_string(char *buffer, int buf_len)
{
	const char *begin;

	if (SKIP_SPACE(rc_line_ptr))
		return 1;
	if (*rc_line_ptr != '"')
		return 1;
	rc_line_ptr++;
	begin = rc_line_ptr;
	while (*rc_line_ptr) {
		if (*rc_line_ptr == '"' || IS_EOL(rc_line_ptr))
			break;
		if (*rc_line_ptr == '\\' && !IS_EOL(rc_line_ptr+1))
			rc_line_ptr++;
		rc_line_ptr++;
	}
	if (IS_EOL(rc_line_ptr))
		return 1;
	if (rc_line_ptr - begin > buf_len)
		return 1;
	strlcpy__(buffer, begin, rc_line_ptr - begin);
	rc_line_ptr++;
	return 0;
}
#endif // ENABLE_SYNTAX

PRIVATE struct /*_color_index_*/ {
	char *color_name;
	int color_index;
} color_names[] = {
	{ "black",		CL_BK,	},	//  0
	{ "dgray",		CL_DG,	},	//  8
	{ "red",		CL_RD,	},	//  1
	{ "lred",		CL_LR,	},	//  9
	{ "green",		CL_GR,	},	//  2
	{ "lgreen",		CL_LG,	},	// 10
	{ "brown",		CL_BR,	},	//  3
	{ "yellow",		CL_YL,	},	// 11
	{ "blue",		CL_BL,	},	//  4
	{ "lblue",		CL_LB,	},	// 12
	{ "magenta",	CL_MG,	},	//  5
	{ "lmagenta",	CL_LM,	},	// 13
	{ "cyan",		CL_CY,	},	//  6
	{ "lcyan",		CL_LC,	},	// 14
	{ "gray",		CL_GY,	},	//  7
	{ "white",		CL_WH,	},	// 15
	{ "none",		-1,	},
};

PRIVATE int parse_color_name(int *bright, int *color)
{
	int index;

///flf_d_printf("colorname: %s\n", colorname);
	*bright = 0;
	*color = -1;
	if (parse_keyword(&rc_line_ptr, "bright") == 0) {
		*bright = 1;
	}
	if (parse_keyword(&rc_line_ptr, "dark") == 0) {
		*bright = 0;
	}
	for (index = 0; index < ARRAY_SIZE_OF(color_names); index++) {
		if (parse_keyword(&rc_line_ptr, color_names[index].color_name) == 0) {
			*color = color_names[index].color_index;
			break;
		}
	}
	if (index >= ARRAY_SIZE_OF(color_names)) {
		rcfile_error(_("color name %s not understood.\n"), rc_line_ptr);
		return 1;
	}
	return 0;
}

PRIVATE int parse_keyword(const char **rc_line_ptr, const char *keyword)
{
	if (strlcasecmp(*rc_line_ptr, keyword) == 0) {
		*rc_line_ptr += strlen(keyword);
		return 0;
	}
	return -1;
}

#ifdef ENABLE_SYNTAX

int register_default_color_syntax(void)
{
#define CL_TAB_EOL	CL_GR	// color for TAB/EOL/"　" notation

	add_file_type("*", ".*");
	cur_file_type->tab_size = DEFAULT_TAB_SIZE;	// set default tab size
#if defined(ENABLE_UTF8)
	add_color_syntax(UTF8_ZEN_SPACE, "", -1, CL_TAB_EOL);	// Full width space
#endif // ENABLE_UTF8
	add_color_syntax("[\\t]", "", -1, CL_TAB_EOL);		// TAB
	add_color_syntax("[\\n]", "", -1, CL_TAB_EOL);		// EOL(End of line)
	default_color_syntax = cur_file_type;	// memorize this color syntax
	return 0;
}

PRIVATE int add_file_type(const char *file_type_name, const char *regexp_file_name)
{
	file_type_t **file_type_ptr;
	file_type_t *file_type = NULL;

///flf_d_printf("adding file type: [%s] [%s]\n", file_type_name, regexp_file_name);
	_mlc_set_caller
	file_type = (file_type_t *)malloc__(sizeof(file_type_t));
	file_type->regexp = regexp_alloc();
	file_type->next = NULL;
	if (regexp_compile(file_type->regexp, regexp_file_name, 0)) {
		rcfile_error(_("Bad regexp: [%s]:%s\n"),
		 regexp_file_name, file_type->regexp->regex_err_msg);
		goto add_file_type_err;
	}
	_mlc_set_caller
	file_type->desc = malloc_strcpy(file_type_name);
	file_type->color_syntax = NULL;
	file_type->tab_size = DEFAULT_TAB_SIZE;
	for (file_type_ptr = &file_types_head; *file_type_ptr != NULL; ) {
		file_type_ptr = &((*file_type_ptr)->next);
	}
	cur_file_type = *file_type_ptr = file_type;
	return 0;

add_file_type_err:;
	regexp_free(file_type->regexp);
	file_type->regexp = NULL;
	return 1;
}

PRIVATE int add_color_syntax(const char *regexp_start, const char *regexp_end, int bgc, int fgc)
{
	color_syntax_t *clr_syntax;

	if (strlen(regexp_start) == 0)
		return 1;

	_mlc_set_caller
	clr_syntax = (color_syntax_t *)malloc__(sizeof(color_syntax_t));
	clr_syntax->next = NULL;
	clr_syntax->regexp_end = NULL;

	clr_syntax->regexp_start = regexp_alloc();
	if (regexp_compile(clr_syntax->regexp_start, regexp_start, 0)) {
		rcfile_error(_("Bad regexp: [%s]:%s\n"),
		 regexp_start, clr_syntax->regexp_start->regex_err_msg);
		goto add_color_syntax_err1;
	}

	if (strlen(regexp_end)) {
		clr_syntax->regexp_end = regexp_alloc();
		if (regexp_compile(clr_syntax->regexp_end, regexp_end, 0)) {
			rcfile_error(_("Bad regexp: [%s]:%s\n"),
			 regexp_end, clr_syntax->regexp_end->regex_err_msg);
			goto add_color_syntax_err2;
		}
	}
	clr_syntax->color.bgc = bgc;
	clr_syntax->color.fgc = fgc;

	link_color_syntax_w_file_type(cur_file_type, clr_syntax);
	return 0;

add_color_syntax_err2:;
	regexp_free(clr_syntax->regexp_end);
	clr_syntax->regexp_end = NULL;
add_color_syntax_err1:;
	regexp_free(clr_syntax->regexp_start);
	clr_syntax->regexp_start = NULL;
	FREE_CLR_PTR(clr_syntax);
	return 1;
}

PRIVATE int link_color_syntax_w_file_type(file_type_t *file_type, color_syntax_t *clr_syntax)
{
	color_syntax_t **color_syntax_ptr_ptr = NULL;

	for (color_syntax_ptr_ptr = &file_type->color_syntax;
	 *color_syntax_ptr_ptr != NULL; ) {
///_D_(dump_color_syntax(*color_syntax_ptr_ptr))
		color_syntax_ptr_ptr = &(*color_syntax_ptr_ptr)->next;
	}
	*color_syntax_ptr_ptr = clr_syntax;
	return 0;
}

void free_file_types(void)
{
	file_type_t *file_type, *f_next;
	color_syntax_t *clr_syntax, *c_next;

	_mlc_memorize_count
///_D_(dump_file_types())
	for (file_type = file_types_head; file_type != NULL; ) {
		FREE_CLR_PTR(file_type->desc);
		regexp_free(file_type->regexp);
		for (clr_syntax = file_type->color_syntax; clr_syntax != NULL; ) {
			if (clr_syntax->regexp_start) {
				FREE_CLR_PTR(clr_syntax->regexp_start);
			}
			if (clr_syntax->regexp_end) {
				FREE_CLR_PTR(clr_syntax->regexp_end);
			}
			c_next = clr_syntax->next;
			FREE_CLR_PTR(clr_syntax);
			clr_syntax = c_next;
		}
		f_next = file_type->next;
		FREE_CLR_PTR(file_type);
		file_type = f_next;
	}
	file_types_head = NULL;
	cur_file_type = NULL;
	_mlc_differ_count
}

//------------------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void dump_file_types(void)
{
	file_type_t *file_type;

flf_d_printf("{{\n");
	for (file_type = file_types_head; file_type != NULL; file_type = file_type->next) {
		dump_file_type(file_type, 0);
	}
flf_d_printf("--------------------------------------------------\n");
	for (file_type = file_types_head; file_type != NULL; file_type = file_type->next) {
		dump_file_type(file_type, 1);
	}
flf_d_printf("}}\n");
}
void dump_file_type(const file_type_t *file_type, int syntax)
{
	color_syntax_t *clr_syntax;

	if (file_type == NULL) {
		flf_d_printf("file_type: NULL\n");
	}
	flf_d_printf("%sfile_type:[%s][%s] tab_size:%d\n",
	 file_type == cur_file_type ? ">" : " ",
	 file_type->desc, file_type->regexp->needle_compiled, file_type->tab_size);
	if (syntax) {
		for (clr_syntax = file_type->color_syntax; clr_syntax != NULL;
		 clr_syntax = clr_syntax->next) {
			dump_color_syntax(clr_syntax);
		}
	}
}
void dump_color_syntax(const color_syntax_t *clr_syntax)
{
	flf_d_printf("%p: [%s]-[%s] %d/%d ->%p\n",
	 clr_syntax,
	 clr_syntax->regexp_start->needle_compiled,
	 clr_syntax->regexp_end ? clr_syntax->regexp_end->needle_compiled : "",
	 clr_syntax->color.bgc, clr_syntax->color.fgc,
	 clr_syntax->next);
}
#endif // ENABLE_DEBUG

#endif // ENABLE_SYNTAX

PRIVATE void rcfile_error(const char *msg, ...)
{
	va_list ap;

	e_printf("\n");
	if (rc_line_num > 0) {
		e_printf(_("Error in file: %s on line: %d"), rc_file_path_reading, rc_line_num);
	}
	va_start(ap, msg);
	e_vprintf(msg, ap);
	va_end(ap);
}

#endif // ENABLE_RC

// End of rcfile.c
