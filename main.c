/**************************************************************************
 *   main.c                                                               *
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

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif // HAVE_GETOPT_H

#ifdef ENABLE_DEBUG
#warning "**** ENABLE_DEBUG defined (debug output is ENABLED) ****"
#endif // ENABLE_DEBUG

#ifdef ENABLE_NCURSES
#warning "Terminal control via ncurses (curses_...)"
#else // ENABLE_NCURSES
#warning "Terminal control via own terminal interface library (termif_...)"
#endif // ENABLE_NCURSES

PRIVATE int init_locale(void);
PRIVATE int init_app_mode(void);
PRIVATE int parse_options(int argc, char *argv[]);
#ifdef START_UP_TEST
PRIVATE void start_up_test(void);
PRIVATE void start_up_test2(void);
#endif // START_UP_TEST
PRIVATE void exit_app(void);
PRIVATE int write_cur_dir_to_exit_file(void);
PRIVATE void die_save_file(const char *die_file_path);

char *main_rc_file_name = RC_FILE_NAME;	// standard rc file

int main(int argc, char *argv[])
{
#ifdef ENABLE_FILER
	filer_views_t filer_views;
#endif // ENABLE_FILER
	int start_line_num = 0;			// Line to start at

flf_d_printf("Start-------------------------------------------------------------\n");
	get_start_dir();
	signal_init();
_FLF_
	init_locale();
_FLF_
	init_app_mode();
_FLF_
	init_buffers();		// parse_options() needs c_e_b. So do here.
#ifdef ENABLE_FILER
	init_filer_views(&filer_views, get_start_dir());
	set_cur_filer_view();
#endif // ENABLE_FILER
_FLF_
	parse_options(argc, argv);		// parse command line options
flf_d_printf("Starting %s ----------------\n", APP_NAME " " __DATE__ " " __TIME__);
	cache_users();
	cache_groups();
_FLF_
	init_default_app_color();
_FLF_
#ifdef START_UP_TEST
	start_up_test();
#endif // START_UP_TEST

#ifdef ENABLE_RC
	if (GET_APPMD(app_RCFILE)) {
		read_rc_file(main_rc_file_name);			// read standard rc file
	}
#endif // ENABLE_RC
#ifdef ENABLE_SYNTAX
	register_default_color_syntax();
///_D_(dump_file_types());
#endif // ENABLE_SYNTAX

	// setup terminal
flf_d_printf("initializing terminal\n");
	// initialize terminal interface(curses/termif)
	tio_init();
flf_d_printf("columns:%d lines:%d\n", tio_get_columns(), tio_get_lines());
flf_d_printf("setting up windows\n");
	win_init_win_size();
#ifdef SCRN_COLS_TWO_PANES
	if (COLS >= SCRN_COLS_TWO_PANES) {
		// set two pane
		SET_APPMD(fl_FILER_PANES);
	}
#endif // SCRN_COLS_TWO_PANES

#ifdef ENABLE_HISTORY
flf_d_printf("init_histories()\n");
	init_histories();
flf_d_printf("load_histories()\n");
	load_histories();
	load_last_searched_needle();
flf_d_printf("load_key_macro()\n");
	load_key_macro(1);
#endif // ENABLE_HISTORY

#ifdef START_UP_TEST
	start_up_test2();
#endif // START_UP_TEST

	set_die_on_callback(app_die_on);

flf_d_printf("opening files --------------------------------------------\n");
#ifdef ENABLE_HELP
	disp_splash(0);
	MSLEEP(500);
#endif // ENABLE_HELP
_FLF_
	// If there's a +LINE flag, it is the first non-option argument
	if (0 < optind && optind < argc && argv[optind][0] == '+') {
flf_d_printf("optind:%d: %s\n", optind, argv[optind]);
		sscanf(&argv[optind][1], "%d", &start_line_num);
		optind++;
	}
_FLF_
	// More than one file is specified on the command line,
	// load them all and switch to the first one afterward.
	if (optind < argc) {
		clear_files_loaded();
		for ( ; optind < argc; optind++) {
flf_d_printf("optind:%d: %s\n", optind, argv[optind]);
			// CURDIR: changed in editor
			if (load_file_name_upp_low(argv[optind], TUL0, OOE1, MOE0, RECURSIVE1) <= 0) {
				tio_beep();
			}
			tio_refresh();
		}
	}
_FLF_
	if (count_edit_bufs()) {
		do_switch_to_top_file();
		if (start_line_num > 0)
			goto_line_col_in_cur_buf(start_line_num, 1);
		disp_files_loaded();
	}

_FLF_
	app_main_loop();
_FLF_

	set_die_on_callback(NULL);
	exit_app();
	return 0;
}

int app_main_loop(void)
{
#ifdef ENABLE_FILER
	char file_name[MAX_PATH_LEN+1];

	if (count_edit_bufs()) {
		// application was started as a EDITOR
_FLF_
		call_editor(0, 0);
_FLF_
	} else {
		// application was started as a FILER
		while (1) {
_FLF_
			call_filer(0, 0, "", "", file_name, MAX_PATH_LEN);
_FLF_
			if (count_edit_bufs() == 0) {
				// no file loaded in filer
				break;
			}
			call_editor(0, 0);
		}
	}
#else // ENABLE_FILER
	if (count_edit_bufs() == 0) {
_FLF_
		do_switch_to_file_list();
		do_open_file();
_FLF_
	}
	if (count_edit_bufs()) {
		call_editor(0, 0);
	}
#endif // ENABLE_FILER
	return 0;
}

//-----------------------------------------------------------------------------

PRIVATE int init_locale(void)
{
	// setup system environment
	setlocale(LC_ALL, "");	// set locale so that wchar related functions work
#if defined(ENABLE_NLS) && defined(ENABLE_UTF8)
e_printf("LANG: [%s]\n", getenv__("LANG"));
//	setenv("LANG", "ja_JP.UTF-8", 1);
//e_printf("LANG: [%s]\n", getenv__("LANG"));
	setlocale(LC_ALL, getenv__("LANG"));
e_printf("cur locale: %s\n", setlocale(LC_ALL, NULL));
e_printf("PACKAGE: %s, LOCALEDIR: %s\n", PACKAGE, LOCALEDIR);
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
//e_printf("MB_LEN_MAX: %d\n", MB_LEN_MAX);
//e_printf("MB_CUR_MAX: %d\n", MB_CUR_MAX);
//e_printf("MAX_UTF8C_BYTES: %d\n", MAX_UTF8C_BYTES);
#endif // defined(ENABLE_NLS) && defined(ENABLE_UTF8)
	return 0;
}

PRIVATE int init_app_mode(void)
{
	// setup application settings
	memset(&app_mode__, 0x00, sizeof(app_mode__));

	// editor and filer
#ifdef ENABLE_RC
	SET_APPMD(app_RCFILE);
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	SET_APPMD(app_HISTORY);
#endif // ENABLE_HISTORY
	CLR_APPMD(app_DRAW_CURSOR);
	SET_APPMD_VAL(app_KEY_LINES, 3);
	SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_NONE);
	// editor mode
	CLR_APPMD(app_EDITOR_FILER);
	set_app_func_key_table();
	CLR_APPMD(app_LIST_MODE);
	SET_APPMD(app_UTF8);
///	CLR_APPMD(app_MAP_KEY_7F_BS);
	SET_APPMD(app_MAP_KEY_7F_BS);

	CLR_APPMD(ed_EDITOR_PANES);
	CLR_APPMD(ed_DUAL_SCROLL);
	SET_APPMD(ed_SHOW_RULER);
	SET_APPMD(ed_SHOW_LINE_NUMBER);
	CLR_APPMD(ed_CURS_POSITIONING);
#ifdef USE_NKF
	SET_APPMD(ed_USE_NKF);
#endif // USE_NKF
	CLR_APPMD(ed_AUTO_INDENT);
	SET_APPMD_VAL(ed_BACKUP_FILES, BACKUP_FILES_MAX);
#ifdef ENABLE_REGEX
	SET_APPMD(ed_USE_REGEXP);
#ifdef ENABLE_SYNTAX
	SET_APPMD(ed_SYNTAX_HIGHLIGHT);
	SET_APPMD(ed_TAB_EOL_NOTATION);
#endif // ENABLE_SYNTAX
#endif // ENABLE_REGEX

	// filer mode
	SET_APPMD_VAL(fl_SHOW_FILE_INFO, SHOW_FILE_INFO_5);
	SET_APPMD_VAL(fl_FILE_SORT_BY, 0);
	CLR_APPMD(fl_FILER_PANES);

	return 0;
}

const char short_options[] = "C:cht:rne:dv?k";
#ifdef HAVE_GETOPT_LONG
int option_index = 0;
const struct option long_options[] = {
#ifdef ENABLE_RC
	{ "rcfile",        required_argument, 0, 'C' },
	{ "norcfile",      no_argument,       0, 'c' },
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	{ "nohistory",     no_argument,       0, 'h' },
#endif // ENABLE_HISTORY
	{ "tabsize",       required_argument, 0, 't' },
	{ "view",          no_argument,       0, 'r' },
#ifdef USE_NKF
	{ "nonkf",         no_argument,       0, 'n' },
	{ "encoding",      required_argument, 0, 'e' },
#endif // USE_NKF
	{ "debug",         no_argument,       0, 'd' },
	{ "version",       no_argument,       0, 'v' },		// show version
	{ "help",          no_argument,       0, '?' },		// show help text
#ifndef ENABLE_NCURSES
	{ "keyseq",        no_argument,       0, 'k' },		// investigate key sequence
#endif // ENABLE_NCURSES
	{ 0,               no_argument,       0, 0 }
};
#endif // HAVE_GETOPT_LONG

PRIVATE int parse_options(int argc, char *argv[])
{
	int optchr;

#ifdef ENABLE_DEBUG
	for (optchr = 0; optchr < argc; optchr++) {
		flf_d_printf("optind:%d: %s\n", optchr, argv[optchr]);
	}
#endif // ENABLE_DEBUG
#ifdef HAVE_GETOPT_LONG
	while ((optchr = getopt_long(argc, argv,
	 short_options, long_options, &option_index)) != -1) {
#else
	while ((optchr = getopt(argc, argv,
	 short_options)) != -1) {
#endif
flf_d_printf("optchr: %c\n", optchr);
if (optarg) {
flf_d_printf("*optarg: %c\n", *optarg);
}
		switch (optchr) {
#ifdef ENABLE_RC
		case 'C':
			main_rc_file_name = optarg;
flf_d_printf("main_rc_file_name: [%s]\n", main_rc_file_name);
			break;
		case 'c':
			CLR_APPMD(app_RCFILE);
			break;
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
		case 'h':
			CLR_APPMD(app_HISTORY);
			break;
#endif // ENABLE_HISTORY
		case 't':
			if (optarg) {
				int tab_size = 0;
				sscanf(optarg, "%d", &tab_size);
				if ((tab_size < 1) || (MAX_TAB_SIZE < tab_size)) {
flf_d_printf("Illegal tab size: [%d]\n", tab_size);
					show_usage();
				}
				SET_CUR_EBUF_STATE(buf_TAB_SIZE, tab_size);
			}
			break;
		case 'r':
			SET_CUR_EBUF_STATE(buf_VIEW_MODE, 1);
			break;
#ifdef USE_NKF
		case 'n':
			CLR_APPMD(ed_USE_NKF);
			break;
#endif // USE_NKF
#ifdef USE_NKF
		case 'e':
			if (optarg) {
				switch (optarg[0]) {
				case 'j':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_JIS);
					break;
				case 'e':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_EUCJP);
					break;
				case 's':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_SJIS);
					break;
				case 'u':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8);
					break;
				case 'a':
				default:
					break;
				}
			}
			break;
#endif // USE_NKF
		case 'd':
			SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_PRINTF);
#ifdef ENABLE_DEBUG
			set_debug_printf_output(1);
#endif // ENABLE_DEBUG
			break;
		case 'v':
			show_version();
			exit(0);
		case '?':
			show_usage();
			exit(0);
#ifndef ENABLE_NCURSES
		case 'k':
			investigate_key_sequence();
			exit(0);
#endif // ENABLE_NCURSES
		default:
			fprintf(stderr, "Unknown option: [%c]\n", optchr);
			break;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
#ifdef START_UP_TEST
PRIVATE void start_up_test(void)
{
	char buf[MAX_PATH_LEN+1];

///	tio_test();
	get_cwd(buf);
	flf_d_printf("getcwd: [%s]\n", buf);
	getenv_pwd(buf);
	flf_d_printf("getenv(PWD): [%s]\n", buf);
	flf_d_printf("getenv(USER): [%s]\n", getenv__("USER"));
	flf_d_printf("getenv(HOSTNAME): [%s]\n", getenv__("HOSTNAME"));
	flf_d_printf("getenv(LANG): [%s]\n", getenv__("LANG"));
	flf_d_printf("ttyname(0): [%s]\n", ttyname(0));

	flf_d_printf("sizeof(int): %d\n", sizeof(int));
	flf_d_printf("sizeof(long): %d\n", sizeof(long));
	flf_d_printf("sizeof(int *): %d\n", sizeof(int *));
	flf_d_printf("sizeof(app_mode_t): %d\n", sizeof(app_mode_t));
	flf_d_printf("sizeof(buf_state_t): %d\n", sizeof(buf_state_t));
	flf_d_printf("MAX_UTF8C_BYTES: %d\n", MAX_UTF8C_BYTES);
	flf_d_printf("PATH_MAX: %d\n", PATH_MAX);
	flf_d_printf("MAX_PATH_LEN: %d\n", MAX_PATH_LEN);
	flf_d_printf("MAX_SCRN_LINE_BUF_LEN: %d\n", MAX_SCRN_LINE_BUF_LEN);
	flf_d_printf("MAX_EDIT_LINE_LEN: %d\n", MAX_EDIT_LINE_LEN);

	flf_d_printf("#define KEY_RESIZE	0x%04x\n", KEY_RESIZE);
	flf_d_printf("#define KEY_HOME		0x%04x\n", KEY_HOME);
	flf_d_printf("#define KEY_END		0x%04x\n", KEY_END);
	flf_d_printf("#define KEY_UP		0x%04x\n", KEY_UP);
	flf_d_printf("#define KEY_DOWN		0x%04x\n", KEY_DOWN);
	flf_d_printf("#define KEY_LEFT		0x%04x\n", KEY_LEFT);
	flf_d_printf("#define KEY_RIGHT		0x%04x\n", KEY_RIGHT);
	flf_d_printf("#define KEY_PPAGE		0x%04x\n", KEY_PPAGE);
	flf_d_printf("#define KEY_NPAGE		0x%04x\n", KEY_NPAGE);
	flf_d_printf("#define KEY_IC		0x%04x\n", KEY_IC);
	flf_d_printf("#define KEY_DC		0x%04x\n", KEY_DC);
	flf_d_printf("#define KEY_BACKSPACE	0x%04x\n", KEY_BACKSPACE);
	flf_d_printf("#define KEY_ENTER		0x%04x\n", KEY_ENTER);
	flf_d_printf("#define KEY_F(0)		0x%04x\n", KEY_F(0));

///	test_wrap_line();
	test_normalize_full_path();
	test_get_full_path();
	test_realpath();
	test_get_file_name_extension();

///	test_get_intersection();
	get_mem_free_in_kb(1);
///	test_nn_from_num();
///	test_utf8c_encode();
	test_utf8c_bytes();
///	test_wcwidth();
#ifdef ENABLE_REGEX
	test_regexp();
#endif // ENABLE_REGEX
///
	test_make_ruler_text();
#ifdef ENABLE_FILER
	test_get_file_size_str();
#endif // ENABLE_FILER
	test_replace_str();

	test_get_n_th_file_name();
}
PRIVATE void start_up_test2(void)
{
#ifdef ENABLE_HISTORY
///
	_D_(test_joined_history(HISTORY_TYPE_IDX_SEARCH));
#endif // ENABLE_HISTORY
}
#endif // START_UP_TEST
//-----------------------------------------------------------------------------

// exit application
PRIVATE void exit_app(void)
{
	set_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	tio_clear_screen();
	tio_destroy();

	write_cur_dir_to_exit_file();

	free_all_allocated_memory();

	signal_clear();

flf_d_printf("Exit %s --------------------\n", APP_NAME " " __DATE__ " " __TIME__);
	printf("\n");
	exit(0);
}

PRIVATE int write_cur_dir_to_exit_file(void)
{
	char buffer[MAX_PATH_LEN+1];
	FILE *fp;

	// write current directory to the $HOME/EXIT_FILE_NAME
	snprintf(buffer, MAX_PATH_LEN+1, "%s/%s", get_home_dir(), EXIT_FILE_NAME);
	if ((fp = fopen(buffer, "w")) != NULL) {
		fputs(get_cur_dir(buffer), fp);
		if (fclose(fp) != 0) {
			return -1;
		}
		return 0;
	}
	return -1;
}

//-----------------------------------------------------------------------------

// Die (gracefully?)
void app_die_on(const char *msg)
{
	be_buf_t *edit_buf;

	tio_destroy();

	e_printf("%s", msg);

	// then save all of the modified buffers, if any
	for (edit_buf = EDIT_BUFS_TOP_BUF; IS_NODE_BOT_ANCH(edit_buf) == 0;
	 edit_buf = edit_buf->next) {
_FLF_
		set_cur_edit_buf(edit_buf);
		if (check_cur_buf_modified()) {
			// save the file if it's been modified
			die_save_file(get_c_e_b()->abs_path);
		}
	}

	signal_clear();
	exit(255);
}

// save modified files to save-file before dying
PRIVATE void die_save_file(const char *die_file_path)
{
	char file_path[MAX_PATH_LEN+5+1];
	int ret = 0;

	// If we can't save, we have REAL bad problems, but we might as well TRY.
	if (die_file_path[0] == '\0') {
		e_printf(_("\nNo file written\n"));
		return;
	} else {
		strlcpy__(file_path, die_file_path, MAX_PATH_LEN);
		strlcat__(file_path, MAX_PATH_LEN, "#");
	}
	if (is_strlen_not_0(file_path)) {
		ret = backup_and_save_cur_buf(file_path);
		if (ret == 0) {
			e_printf(_("\nBuffer written to %s\n"), file_path);
			return;
		}
	}
}

//-----------------------------------------------------------------------------

void free_all_allocated_memory(void)
{
_FLF_
	bufs_dump_all_bufs(bufs_top_anchor.next);
#ifdef ENABLE_HISTORY
_FLF_
///	save_key_macro();
#endif // ENABLE_HISTORY
	free_all_buffers();

#ifdef ENABLE_SYNTAX
_FLF_
	free_file_types();
#endif // ENABLE_SYNTAX
_FLF_
	bufs_dump_all_bufs(bufs_top_anchor.next);
}

//-----------------------------------------------------------------------------

PRIVATE void show_one_option(const char *shortflag, const char *longflag,
 const char *desc);
void show_usage(void)
{
	printf(_("\nUsage: " BIN_NAME " [+LINE] [option] [file(s)]\n"));
	//               12345678901234567890 12345678901234567890 12345678901234567890
	show_one_option(_("Option"),         _("Long option"),    _("Meaning"));
#ifdef ENABLE_RC
	show_one_option("-C RCFILE",         "--rcfile=RCFILE",   _("Read RCFILE"));
	show_one_option("-c",                "--norcfile",        _("Don't look at berc files"));
#endif // ENABLE_RC
#ifdef ENABLE_HISTORY
	show_one_option("-h",                "--nohistory",       _("Don't use history file"));
#endif // ENABLE_HISTORY
	show_one_option("-t NUM",            "--tabsize=NUM",     _("Set tab-size to NUM[1-15]"));
	show_one_option("-r",                "--view",            _("View mode(Read only mode)"));
#ifdef USE_NKF
	show_one_option("-n",                "--nonkf",           _("Don't use nkf"));
	show_one_option("-e a",              "--encoding=a",      _("ASCII character encoding"));
	show_one_option("-e j",              "--encoding=j",      _("JIS character encoding"));
	show_one_option("-e e",              "--encoding=e",      _("EUCJP character encoding"));
	show_one_option("-e s",              "--encoding=s",      _("SJIS character encoding"));
	show_one_option("-e u",              "--encoding=u",      _("UTF8 character encoding"));
#endif // USE_NKF
#ifdef ENABLE_DEBUG
	show_one_option("-d",                "--debug",           _("Output debug log to stderr"));
#endif // ENABLE_DEBUG
	show_one_option("-v",                "--version",         _("Show version information"));
	show_one_option("-?",                "--help",            _("Show this message"));
	show_one_option("+NUM",              "",                  _("Start at line number NUM"));
#ifndef ENABLE_NCURSES
	show_one_option("-k",                "--keyseq",          _("Investigate key codes"));
#endif // ENABLE_NCURSES
}

// Print one option explanation to the screen
PRIVATE void show_one_option(const char *shortflag, const char *longflag,
 const char *desc)
{
	printf(" %-20s", shortflag);
#ifdef HAVE_GETOPT_LONG
	printf(" %-20s", longflag);
#endif // HAVE_GETOPT_LONG
	printf(" %s\n", desc);
}

void show_version(void)
{
	printf(_("%s editor version %s (compiled at %s, %s)\n"),
	 APP_NAME, VERSION, __TIME__, __DATE__);
	printf(_(" Build Options:\n"));
#ifdef ENABLE_FILER
	printf("   --enable-filer\n");
#else
	printf("   --disable-filer\n");
#endif
#ifdef ENABLE_NLS
	printf("   --enable-nls\n");
#else
	printf("   --disable-nls\n");
#endif
#ifdef ENABLE_RC
	printf("   --enable-rc\n");
#else
	printf("   --disable-rc\n");
#endif
#ifdef ENABLE_UTF8
	printf("   --enable-utf8\n");
#else
	printf("   --disable-utf8\n");
#endif
#if defined(USE_NCURSES) || defined(USE_NCURSESW)
	printf("   --enable-ncurses\n");
#else
	printf("   --disable-ncurses\n");
#endif
#ifdef USE_NKF
	printf("   --enable-nkf\n");
#else
	printf("   --disable-nkf\n");
#endif
#ifdef ENABLE_HISTORY
	printf("   --enable-history\n");
#else
	printf("   --disable-history\n");
#endif
#ifdef ENABLE_REGEX
	printf("   --enable-regex\n");
#else
	printf("   --disable-regex\n");
#endif
#ifdef ENABLE_SYNTAX
	printf("   --enable-syntax\n");
#else
	printf("   --disable-syntax\n");
#endif
#ifdef USE_PCRE
	printf("   --enable-pcre\n");
#else
	printf("   --disable-pcre\n");
#endif
#ifdef ENABLE_UNDO
	printf("   --enable-undo\n");
#else
	printf("   --disable-undo\n");
#endif
#ifdef USE_BUSYBOX
	printf("   --enable-busybox\n");
#else
	printf("   --disable-busybox\n");
#endif
#ifdef ENABLE_HELP
	printf("   --enable-help\n");
#else // ENABLE_HELP
	printf("   --disable-help\n");
#endif // ENABLE_HELP
#ifdef ENABLE_DEBUG
	printf("   --enable-debug\n");
#else
	printf("   --disable-debug\n");
#endif
	printf("\n");
}

//-----------------------------------------------------------------------------
#ifdef ENABLE_HELP
///#define DEBUG_SPLASH
const char *splash_text_s[] = {
//0123456789012345678901234567890123456789012345678901234567
#ifndef	DEBUG_SPLASH
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
#else // DEBUG_SPLASH
 "bdbdbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
#endif // DEBUG_SPLASH
 "b    FFs   FFs     FFs                          d",
 "b    FFs   FFs     FFs                          d",
 "b  FFs FFs FFs FFs FFs    BBBBBBBs   BBBBBBBs   d",
 "bFFs   FFs FFs FFs FFFFFFsBBs   BBs  BBs        d",
 "b    FFFFFFFFFFFFsFFsFFs  BBBBBBBs   BBBBBBs    d",
 "b  FFs           FFs FFs  BBs   BBs  BBs        d",
 "bFFFFs FFFFFFFFFFFFs FFs  BBBBBBBs   BBBBBBBs   d",
 "b  FFs   FFs FFs   FFs    EEEEs  EsEs           d",
 "b  FFs   FFs FFs   FFs    Es     Es   Es        d",
 "b  FFs   FFs FFs FFs FFs  EEEs EEEsEsEEEsEEEsEEsd",
 "b  FFs FFs     FFs   FFs  Es   EsEsEs Es EsEsEs d",
 "b  FFs       FFs       FFsEEEEsEEEsEs Es EEEsEs d",
 "bdddddddddddddddddddddddddddddddddddddddddddddddd",
 "",
};
const char *splash_text_m[] = {
//0123456789012345678901234567890123456789012345678901234567
#ifndef	DEBUG_SPLASH
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
#else // DEBUG_SPLASH
 "bdbdbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
#endif // DEBUG_SPLASH
 "b      FFs    FFFs       FFs                            d",
 "b    FFFFs    FFs        FFFs                           d",
 "b  FFFs   FFs FFs FFFs  FFs       BBBBBBBs  BBBBBBBs    d",
 "bFFFs     FFs FFs FFs  FFs        BBs   BBs BBs         d",
 "b      FFsFFs FFs FFs FFFFFFFFFFFsBBBBBBBs  BBBBBBs     d",
 "b     FFFsFFFFFFFFFFsFFs     FFs  BBs   BBs BBs         d",
 "b    FFFs               FFs  FFs  BBBBBBBs  BBBBBBBs    d",
 "b  FFFFFsFFFFFFFFFFFFs   FFsFFs                         d",
 "bFFF  FFs   FFs FFs       FFFFs   EEEEs  EsEs           d",
 "b     FFs   FFs FFs        FFs    Es     Es   Es        d",
 "b     FFs   FFs FFs FFs   FFFs    EEEs EEEsEsEEEsEEEsEEsd",
 "b     FFs  FFs  FFFFFFs  FFsFFs   Es   EsEsEs Es EsEsEs d",
 "b     FFs FFs   FFFs   FFs   FFFs EEEEsEEEsEs Es EEEsEs d",
 "b     FFsFFs         FFs      FFFFs                     d",
 "bdddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
 "",
};
const char *splash_text_l[] = {
//01234567890123456789012345678901234567890123456789012345678901234567890123456789
#ifndef	DEBUG_SPLASH
 "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
#else // DEBUG_SPLASH
 "bdbdbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
#endif // DEBUG_SPLASH
 "b         FFFs      FFFFFs      FFFFs                                          d",
 "b       FFFFFFs     FFFFs       FFFFFs                                         d",
 "b      FFFFFs  FFFs FFFFs FFFFs FFFFs                                          d",
 "b     FFFFs    FFFs FFFFs FFFs  FFFFs            BBBBBBs   BBBBBBBs            d",
 "b   FFFFs      FFFs FFFFs FFFs FFFFs             BBs   BBs BBs                 d",
 "b FFFFs    FFFsFFFs FFFFs FFFs FFFs        FFFs  BBs   BBs BBs                 d",
 "b         FFFFsFFFs FFFFs FFFsFFFFFFFFFFFFFFFFFFsBBBBBBs   BBBBBs              d",
 "b        FFFFFsFFFFFFFFFFFFFFFFFs       FFFFs    BBs   BBs BBs                 d",
 "b       FFFFs  FFs        FFFFFFs       FFFFs    BBs   BBs BBs                 d",
 "b      FFFFs               FFFsFFs      FFFs     BBBBBBs   BBBBBBBs            d",
 "b     FFFFFFs             FFs   FFs    FFFs                                    d",
 "b   FFFFFFFFsFFFFFFFFFFFFFFFFFs  FFs  FFFFs                                    d",
 "b FFFFs FFFFs   FFFFs FFFFs       FFFFFFFs       EEEEEEs    Es                 d",
 "b       FFFFs   FFFFs FFFFs        FFFFFFs       Es         EsEs Es            d",
 "b       FFFFs   FFFFs FFFFs         FFFFs        Es         Es   Es            d",
 "b       FFFFs   FFFFs FFFFs FFs    FFFFFFFs      EEEEEs  EEEEsEsEEEs EEEs EsEs d",
 "b       FFFFs  FFFFs  FFFFFFFs    FFFFFFFFFFs    Es     Es  EsEs Es Es  EsEEs  d",
 "b       FFFFs  FFFs FFFFFFFs    FFFFs   FFFFFFs  Es     Es  EsEs Es Es  EsEs   d",
 "b       FFFFs FFs     FFFs    FFFFs     FFFFFFFFsEEEEEEs EEEEsEs  Es EEEs Es   d",
 "b       FFFFsFFs            FFFFs        FFFFFs                                d",
 "b       FFFFs           FFFFFFs                                                d",
 "bddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd",
 "",
};

void disp_splash(int delay)
{
	const char **splash_text;
	int top_y;
	int left_x;
	int msg1_x;
	int msg2_x;
	int msg3_x;
	int cols;
	int lines;
	int yy;
	int xx;
	char buffer[100+1];
#define MSG1_X_S	26
#define MSG1_X_M	34
#define MSG2_X_M	34
#define MSG3_X_M	26
#define MSG_X_L		49

	tio_set_cursor_on(0);
	if (delay >= 0) {
		// refresh whole screen
		tio_clear_flash_screen(delay);
	}
	splash_text = splash_text_l;
	cols = strlen(splash_text[0]);
	for (lines = 0; *splash_text[lines]; lines++) {
		// nothing to do
	}
	msg1_x = MSG_X_L;
	msg2_x = MSG_X_L;
	msg3_x = MSG_X_L;
	if (cols > tio_get_columns()
	 || lines > tio_get_lines()) {
		splash_text = splash_text_m;
		cols = strlen(splash_text[0]);
		for (lines = 0; *splash_text[lines]; lines++) {
			// nothing to do
		}
		msg1_x = MSG1_X_M;
		msg2_x = MSG2_X_M;
		msg3_x = MSG3_X_M;
	}
	if (cols > tio_get_columns()
	 || lines > tio_get_lines()) {
		splash_text = splash_text_s;
		cols = strlen(splash_text[0]);
		for (lines = 0; *splash_text[lines]; lines++) {
			// nothing to do
		}
		msg1_x = MSG1_X_S;
		msg2_x = -1;
		msg3_x = -1;
	}

	top_y = (tio_get_lines() - lines) / 2;
	cols = LIM_MAX(tio_get_columns(), strlen(splash_text[0]));
	left_x = (tio_get_columns() - cols) / 2;
	msg1_x = left_x + msg1_x;
	if (msg2_x >= 0) {
		msg2_x = left_x + msg2_x;
	}
	if (msg3_x >= 0) {
		msg3_x = left_x + msg3_x;
	}
	for (yy = 0; yy < lines; yy++) {
		for (xx = 0; xx < cols; xx++) {
			switch(splash_text[yy][xx]) {
			default:	tio_set_attrs(CL_CY, CL_BK, 0);		break;
			case 'F':	tio_set_attrs(CL_BK, CL_GR, 1);		break;
			case 'B':	tio_set_attrs(CL_BK, CL_RD, 1);		break;
			case 'E':	tio_set_attrs(CL_BK, CL_MG, 1);		break;
			case 'b':	tio_set_attrs(CL_BK, CL_RD, 1);		break;
			case 'd':	tio_set_attrs(CL_BK, CL_RD, 1);		break;
			case 's':	tio_set_attrs(CL_BL, CL_BK, 0);		break;
			}
#ifndef	DEBUG_SPLASH
			tio_output_string(top_y + yy, left_x + xx, " ", -1);
#else // DEBUG_SPLASH
			buffer[0] = splash_text[yy][xx];
			buffer[1] = '\0';
			tio_output_string(top_y + yy, left_x + xx, buffer, -1);
#endif // DEBUG_SPLASH
		}
	}
	tio_set_attrs(CL_CY, CL_BK, 0);
	tio_output_string(top_y + 1, msg1_x, "Version " VERSION, -1);
	tio_output_string(top_y + 2, msg1_x, __DATE__ " " __TIME__, -1);
	snprintf(buffer, 100+1, _("Screen size: %dx%d"), tio_get_columns(), tio_get_lines());
	if (msg2_x >= 0) {
		tio_output_string(top_y + lines / 2, msg2_x, buffer, -1);
	}
	if (msg3_x >= 0) {
		tio_output_string(top_y + lines - 2, msg3_x, _("We're invincible with this !!"), -1);
	}
	tio_refresh();
///	tio_set_cursor_on(1);
}
#endif // ENABLE_HELP

// End of main.c
