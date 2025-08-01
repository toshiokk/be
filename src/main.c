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

#ifdef ENABLE_NCURSES
#warning "Terminal control via ncurses (curses_...)"
#else // ENABLE_NCURSES
////#warning "Terminal control via own terminal interface library (termif_...)"
#endif // ENABLE_NCURSES

PRIVATE int init_locale();
PRIVATE int parse_options(int argc, char *argv[]);
#ifdef START_UP_TEST
PRIVATE void start_up_test();
PRIVATE void start_up_test2();
PRIVATE void test_modulo();
#endif // START_UP_TEST

PRIVATE int write_exit_file(int restart);

PRIVATE void die_save_file(const char *die_file_path);

PRIVATE void app_main_loop();

PRIVATE editor_panes_t root_editor_panes;
#ifdef ENABLE_FILER
PRIVATE filer_panes_t root_filer_panes;
#endif // ENABLE_FILER
#ifdef ENABLE_RC
PRIVATE const char *main_rc_file_name = RC_FILE_NAME;	// standard rc file
#endif // ENABLE_RC

int restart_be = 0;

// If screen columns is wider than this value, set two panes mode at start up
////#define SCRN_COLS_TWO_PANES		120

int main(int argc, char *argv[])
{
	init_app_mode();
dtflf_d_printf("Start %s ==============================\n", APP_NAME " " __DATE__ " " __TIME__);
	_mlc_init
	change_cur_dir(get_starting_dir());
	get_home_dir();
	get_tty_name();
	signal_init();
	init_locale();
	cache_users();
	cache_groups();
#ifdef ENABLE_DEBUG
	set_progerr_callback(progerr_cb_func);
#endif // ENABLE_DEBUG

	_mlc_memorize_count
	init_bufferss();		// parse_options() needs epc_buf. So create here.
	init_cur_editor_panes(&root_editor_panes, NULL);
#ifdef ENABLE_FILER
	init_cur_filer_panes(&root_filer_panes, get_starting_dir());
#endif // ENABLE_FILER
	_mlc_differ_count

	parse_options(argc, argv);		// parse command line options
	init_default_app_color();
	check_wsl();
	check_availability_of_script();
#ifdef START_UP_TEST
	start_up_test();
#endif // START_UP_TEST

#ifdef ENABLE_RC
	if (GET_APPMD(app_RCFILE)) {
		read_rc_file(main_rc_file_name);			// read standard rc file
	}
#endif // ENABLE_RC
#ifdef ENABLE_SYNTAX
	_mlc_memorize_count
	register_default_color_syntax();
	_mlc_differ_count
#endif // ENABLE_SYNTAX

	// initialize terminal interface(curses/termif)
	flf_d_printf("initializing terminal\n");
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

	flf_d_printf("load_histories_if_file_newer()\n");
	load_histories_if_file_newer();

	flf_d_printf("load_last_key_macro()\n");
	load_last_key_macro(1);
#endif // ENABLE_HISTORY

	load_cut_buffers();

#ifdef START_UP_TEST
	start_up_test2();
#endif // START_UP_TEST

	set_die_on_callback(app_die_on);

#ifdef ENABLE_HELP
////#define SPLASH_ON_START_UP__EXIT
#endif // ENABLE_HELP
#ifdef SPLASH_ON_START_UP__EXIT
	disp_splash(0);
	MSLEEP(300);
#endif // SPLASH_ON_START_UP__EXIT
flf_d_printf("opening files --------------------------------------------\n");
	// If there's a +LINE flag, it is the first non-option argument
	int start_line_num = 0;			// Line to start at
	if (0 < optind && optind < argc && argv[optind][0] == '+') {
flf_d_printf("optind:%d: %s\n", optind, argv[optind]);
		sscanf(&argv[optind][1], "%d", &start_line_num);
		optind++;
	}
	// More than one file is specified on the command line,
	// load them all and switch to the first one afterward.
	if (optind < argc) {
		clear_files_loaded();
		begin_check_break_key();
		for ( ; optind < argc; optind++) {
flf_d_printf("optind:%d: %s\n", optind, argv[optind]);
			// CURDIR: changed in editor
			if (load_file_name_upp_low_(argv[optind],
			 TUL0 | OOE1 | MOE0 | RDOL0 | FOLF0 | LFH0 | RECURS1 | MFPL0) <= 0) {
				tio_beep();
			}
			tio_refresh();
			if (check_break_key()) {
				break;
			}
		}
		end_check_break_key();
	}
	if (count_edit_bufs()) {
#ifdef ENABLE_HISTORY
		if (goto_last_file_line_col_in_history() == 0) {
			doe_switch_to_top_buffer();
		}
#endif // ENABLE_HISTORY
		if (start_line_num > 0) {
			goto_line_col_in_cur_buf(start_line_num, -1);
		}
		disp_files_loaded_if_ge_0();
	}

	//--------------
	app_main_loop();
	//--------------

#ifdef SPLASH_ON_START_UP__EXIT
	disp_splash(-1);
	MSLEEP(200);
#endif // SPLASH_ON_START_UP__EXIT

	set_die_on_callback(NULL);

	set_item_color_by_idx(ITEM_COLOR_IDX_DEFAULT, 0);
	tio_destroy();

	save_cut_buffers_if_local_newer();

	reduce_log_file_size(get_exec_log_file_path(), MAX_LOG_FILE_SIZE_KB);
	write_exit_file(restart_be);

	_mlc_check_count
	free_all_allocated_memory();
#ifdef ENABLE_FILER
	free_filer_panes();
#endif // ENABLE_FILER
	free_editor_panes();
	signal_clear();
#ifdef ENABLE_DEBUG
	set_progerr_callback(NULL);
#endif // ENABLE_DEBUG

	_mlc_check_count
	_D_(_mlc_check_leak)

dtflf_d_printf("Exit %s ===============================\n", APP_NAME " " __DATE__ " " __TIME__);
	printf("\n");
	return 0;
}

//------------------------------------------------------------------------------

PRIVATE int init_locale()
{
	// setup system environment
	setlocale(LC_ALL, "");	// set locale so that wchar related functions work
#if defined(ENABLE_NLS)
flf_d_printf("LANG: [%s]\n", getenv__("LANG"));
flf_d_printf("cur locale: [%s]\n", setlocale(LC_ALL, NULL));
flf_d_printf("PACKAGE: [%s], LOCALEDIR: [%s]\n", PACKAGE, LOCALEDIR);
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
#endif // defined(ENABLE_NLS)
	return 0;
}

const char short_options[] = "C:ct:rne:bxwldv?k";
#ifdef HAVE_GETOPT_LONG
int option_index = 0;
const struct option long_options[] = {
	{ "tabsize",       required_argument, 0, 't' },
	{ "view",          no_argument,       0, 'r' },
#ifdef ENABLE_RC
	{ "rcfile",        required_argument, 0, 'C' },
	{ "norcfile",      no_argument,       0, 'c' },
#endif // ENABLE_RC
#ifdef USE_NKF
	{ "nonkf",         no_argument,       0, 'n' },
	{ "encoding",      required_argument, 0, 'e' },
	{ "binary",        no_argument,       0, 'b' },
	{ "text",          no_argument,       0, 'x' },
#endif // USE_NKF
#ifdef ENABLE_FILER
	{ "twopane",       no_argument,       0, 'w' },
#endif // ENABLE_FILER
#ifdef ENABLE_HIGH_BGC
	{ "highbgc",       no_argument,       0, 'l' },
#endif // ENABLE_HIGH_BGC
#ifdef ENABLE_DEBUG
	{ "debug",         no_argument,       0, 'd' },
#endif // ENABLE_DEBUG
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
	SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_NONE);
	set_debug_printf_output(GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF);
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
			SET_CUR_EBUF_STATE(buf_MODE, BUF_MODE_RO);	// Set a edit-buffer not saveable
			break;
#ifdef USE_NKF
		case 'n':
			CLR_APPMD(ed_USE_NKF);
			break;
		case 'e':
			if (optarg) {
				switch (optarg[0]) {
				default:
				case 'a':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_ASCII);
					break;
				case 'u':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8);
					break;
				case 'e':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_EUCJP);
					break;
				case 's':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_SJIS);
					break;
				case 'j':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_JIS);
					break;
				case 'b':
					SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_BINARY);
					break;
				}
			}
			break;
		case 'b':
			// same as "-encoding b"
			SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_BINARY);
			break;
		case 'x':
			// same as "-encoding u"
			SET_CUR_EBUF_STATE(buf_ENCODE, ENCODE_UTF8);
			break;
#endif // USE_NKF
#ifdef ENABLE_FILER
		case 'w':
			SET_APPMD(fl_FILER_PANES);
			SET_APPMD_VAL(fl_SHOW_FILE_INFO, SHOW_FILE_INFO_1);
			break;
#ifdef ENABLE_HIGH_BGC
		case 'l':
			// NOTE: Linux console does not support highlight for background
			//       So Do-Not-Use on Linux console.
			SET_APPMD(app_HIGH_BGC);
			tio_enable_high_bgc(GET_APPMD(app_HIGH_BGC));
			break;
#endif // ENABLE_HIGH_BGC
#endif // ENABLE_FILER
#ifdef ENABLE_DEBUG
		case 'd':
			SET_APPMD_VAL(app_DEBUG_PRINTF, DEBUG_PRINTF);
			set_debug_printf_output(GET_APPMD(app_DEBUG_PRINTF) == DEBUG_PRINTF);
			break;
#endif // ENABLE_DEBUG
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
//------------------------------------------------------------------------------
// do_call_editor() : pass a edit-buffer and edit or browse it.
// do_call_filer()  : pass a directory and manage or browse it.
PRIVATE void app_main_loop()
{
	clear_app_stack_depth();
	clear_whole_screen_update_timer();	// avoid screen flashing on the first key input
#ifndef ENABLE_FILER
	if (count_edit_bufs() == 0) {
		doe_open_file_recursive();
	}
	while (has_bufs_to_edit()) {
		do_call_editor(0, APP_MODE_NORMAL, NULL, NULL);
	}
#else // ENABLE_FILER
	if (count_edit_bufs()) {
		// application was started as a EDITOR
		while (has_bufs_to_edit()) {
			do_call_editor(0, APP_MODE_NORMAL, NULL, NULL);
		}
	} else {
		// application was started as a FILER
		for ( ; ; ) {
			char file_path[MAX_PATH_LEN+1];
			do_call_filer(0, APP_MODE_NORMAL, "", "", file_path);
			if (has_bufs_to_edit() == 0) {
flf_d_printf("count_edit_bufs():%d, epc_buf_count_bufs():%d\n",
 count_edit_bufs(), epc_buf_count_bufs());
				// no file loaded in filer
				break;
			}
flf_d_printf("do_call_editor\n");
			do_call_editor(0, APP_MODE_NORMAL, NULL, NULL);
		}
	}
#endif // ENABLE_FILER
}
//------------------------------------------------------------------------------
#ifdef START_UP_TEST
PRIVATE void start_up_test()
{
	flf_d_printf("{{{{---------------------------------------------------------\n");
	test_flock();
	////tio_test();
	flf_d_printf("getenv(USER): [%s]\n", getenv__("USER"));
	flf_d_printf("getenv(HOSTNAME): [%s]\n", getenv__("HOSTNAME"));
	flf_d_printf("getenv(LANG): [%s]\n", getenv__("LANG"));
	flf_d_printf("get_tty_name(): [%s]\n", get_tty_name());
	flf_d_printf("get_tty_name_file_part(): [%s]\n", get_tty_name_file_part());
	flf_d_printf("exec_log_file_path: [%s]\n", get_exec_log_file_path());
	flf_d_printf("getenv(HOME): [%s]\n", get_home_dir());

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

	// memory address of various object
	flf_d_printf("mem adrs: 0x1234567890123456\n");
	char buf[MAX_PATH_LEN+1];
	flf_d_printf("auto buf: %p\n", buf);
	flf_d_printf("\"string\": %p\n", "string");
	void *allocated = malloc__(100);
	flf_d_printf("malloc  : %p\n", allocated);
	free__(allocated);

	flf_d_printf("#define KEY_RESIZE     0x%04x\n", KEY_RESIZE);
	flf_d_printf("#define KEY_HOME       0x%04x\n", KEY_HOME);
	flf_d_printf("#define KEY_END        0x%04x\n", KEY_END);
	flf_d_printf("#define KEY_UP         0x%04x\n", KEY_UP);
	flf_d_printf("#define KEY_DOWN       0x%04x\n", KEY_DOWN);
	flf_d_printf("#define KEY_LEFT       0x%04x\n", KEY_LEFT);
	flf_d_printf("#define KEY_RIGHT      0x%04x\n", KEY_RIGHT);
	flf_d_printf("#define KEY_PPAGE      0x%04x\n", KEY_PPAGE);
	flf_d_printf("#define KEY_NPAGE      0x%04x\n", KEY_NPAGE);
	flf_d_printf("#define KEY_IC         0x%04x\n", KEY_IC);
	flf_d_printf("#define KEY_DC         0x%04x\n", KEY_DC);
	flf_d_printf("#define KEY_BACKSPACE  0x%04x\n", KEY_BACKSPACE);
	flf_d_printf("#define KEY_ENTER      0x%04x\n", KEY_ENTER);
	flf_d_printf("#define KEY_F(0)       0x%04x\n", KEY_F(0));

	////test_wrap_line();
	test_cwd_PWD();
	test_normalize_path();
	test_get_full_path();
#if defined(HAVE_REALPATH)
	test_realpath();
#endif // HAVE_REALPATH

	test_get_file_name_extension();
	test_cat_dir_and_file();
	test_separate_path_to_dir_and_file();

	test_utilstr();
	test_get_one_file_path();

	////test_get_intersection();
	get_mem_free_in_kb(1);
	////test_zz_from_num();
	////test_utf8c_encode();
	test_utf8c_bytes();
	////test_wcwidth();
#ifdef ENABLE_REGEX
	test_regexp();
#endif // ENABLE_REGEX
	////test_make_ruler_text();
#ifdef ENABLE_FILER
	test_get_file_size_str();
	test_get_n_th_file();
#endif // ENABLE_FILER
	test_replace_str();

	////test_key_code_from_to_key_name();
	flf_d_printf("}}}}---------------------------------------------------------\n");
}
PRIVATE void start_up_test2()
{
	flf_d_printf("{{{{---------------------------------------------------------\n");
	check_multiple_assignment_of_key();
	check_all_functions_accessible_without_function_key();
	test_modulo();
	flf_d_printf("}}}}---------------------------------------------------------\n");
}
PRIVATE void test_modulo()
{
	flf_d_printf("modulo test --------------\n");
	for (int nn = -9; nn < 10; nn++) {
		flf_d_printf("%2d mod 7 = %2d, %2d mod 5 = %2d, %2d mod 3 = %2d\n",
		 nn, nn % 7, nn, nn % 5, nn, nn % 3);
	}
}
#endif // START_UP_TEST

//------------------------------------------------------------------------------

PRIVATE int write_exit_file(int restart)
{
	char script[MAX_PATH_LEN+1] = "";
	char file_path[MAX_PATH_LEN+1];
	// "cd %s\n"
	// "restartbe=1\n"
	strcat_printf(script, MAX_PATH_LEN+1, "cd '%s'\n", get_full_path_of_cur_dir(NULL));
	strcat_printf(script, MAX_PATH_LEN+1, "restartbe=%d\n", restart);
	cat_dir_and_file(file_path, get_home_dir(), EXIT_FILE_NAME);
	return write_text_to_file(file_path, 0, script);
}

int progerr_cb_func(const char* warning)
{
	set_work_space_color_warn();
	return write_to_warning_file(warning);
}
int write_to_warning_file(const char* warning)
{
	char file_path[MAX_PATH_LEN+1];
	// record a warning message even when no debug logging enabled
	cat_dir_and_file(file_path, get_app_dir(), WARNING_FILE_NAME);
	return write_text_to_file(file_path, 1, warning);
}

//------------------------------------------------------------------------------

// Die (gracefully?)
void app_die_on(const char *msg)
{
	tio_destroy();

	e_printf("%s", msg);

	// then save all of the modified buffers, if any
	for (be_buf_t *buf = EDIT_BUFS_TOP_BUF; IS_NODE_INT(buf); buf = NODE_NEXT(buf)) {
		set_epc_buf(buf);
		if (check_cur_ebuf_modified()) {
			// save the file if it's been modified
			die_save_file(buf_get_file_path(get_epc_buf(), NULL));
		}
	}

	signal_clear();
	exit(255);
}

// save modified files to save-file before dying
PRIVATE void die_save_file(const char *die_file_path)
{
	char file_path[MAX_PATH_LEN+5+1];
	// If we can't save, we have REAL bad problems, but we might as well TRY.
	if (die_file_path[0] == '\0') {
		e_printf("\nNo file written\n");
		return;
	} else {
		strlcpy__(file_path, die_file_path, MAX_PATH_LEN);
		strlcat__(file_path, MAX_PATH_LEN, "#");	// add '#'
	}
	if (is_strlen_not_0(file_path)) {
		if (backup_and_save_cur_buf(file_path) >= 0) {
			e_printf("\nBuffer written to %s\n", file_path);
			return;
		}
	}
}

//------------------------------------------------------------------------------

void free_all_allocated_memory()
{
#ifdef ENABLE_HISTORY
	save_histories_if_modified();
#endif // ENABLE_HISTORY
	_mlc_memorize_count
	free_all_buffers();
	_mlc_differ_count

#ifdef ENABLE_SYNTAX
	free_file_types();
#endif // ENABLE_SYNTAX
}

//------------------------------------------------------------------------------

PRIVATE void show_one_option(const char *shortflag, const char *longflag, const char *desc);
void show_usage()
{
	printf(_("\nUsage: " BIN_NAME " [+LINE] [option] [file(s)]\n"));
	//               12345678901234567890 12345678901234567890 12345678901234567890
	show_one_option(_("Option"),         _("Long option"),    _("Meaning"));
	show_one_option("-t NUM",            "--tabsize=NUM",     _("Set tab-size to NUM[1-15]"));
	show_one_option("-r",                "--view",            _("View mode(Read only mode)"));
#ifdef ENABLE_RC
	show_one_option("-C RCFILE",         "--rcfile=RCFILE",   _("Read RCFILE"));
	show_one_option("-c",                "--norcfile",        _("Don't look at berc files"));
#endif // ENABLE_RC
#ifdef USE_NKF
	show_one_option("-n",                "--nonkf",           _("Don't use nkf"));
	show_one_option("-e a",              "--encoding=a",      _("ASCII character encoding"));
	show_one_option("-e j",              "--encoding=j",      _("JIS character encoding"));
	show_one_option("-e e",              "--encoding=e",      _("EUCJP character encoding"));
	show_one_option("-e s",              "--encoding=s",      _("SJIS character encoding"));
	show_one_option("-e u",              "--encoding=u",      _("UTF8 character encoding"));
	show_one_option("-e b",              "--encoding=b",      _("BINARY file"));
	show_one_option("-b",                "--binary",          _("BINARY file (same as '-e b')"));
	show_one_option("-x",                "--text",            _("TEXT file (same as '-e u')"));
#endif // USE_NKF
#ifdef ENABLE_FILER
	show_one_option("-w",                "--twopane",         _("Two pane mode"));
#endif // ENABLE_FILER
#ifdef ENABLE_HIGH_BGC
	show_one_option("-l",                "--highbgc",         _("Use High Background Color"));
#endif // ENABLE_HIGH_BGC
#ifdef ENABLE_DEBUG
	show_one_option("-d",                "--debug",           _("Output debug log to stderr"));
#endif // ENABLE_DEBUG
	show_one_option("-v",                "--version",         _("Show version information"));
	show_one_option("-?",                "--help",            _("Show this message"));
	show_one_option("+NUM",              "",                  _("Start at line number NUM"));
#ifndef ENABLE_NCURSES
	show_one_option("-k",                "--keyseq",          _("Investigate key sequence"));
#endif // ENABLE_NCURSES
}
// Print one option explanation to the screen
PRIVATE void show_one_option(const char *shortflag, const char *longflag, const char *desc)
{
	printf(" %-20s", shortflag);
#ifdef HAVE_GETOPT_LONG
	printf(" %-20s", longflag);
#endif // HAVE_GETOPT_LONG
	printf(" %s\n", desc);
}

void show_version()
{
	printf(_("%s version %s (compiled at %s, %s)\n"),
	 APP_LONG_NAME, VERSION, __TIME__, __DATE__);
	printf(_(" Build Options:\n"));
#ifdef ENABLE_FILER //-------------------
	printf("   --enable-filer\n");
#else
	printf("   --disable-filer\n");
#endif
#ifdef ENABLE_NLS //-------------------
	printf("   --enable-nls\n");
#else
	printf("   --disable-nls\n");
#endif
#ifdef ENABLE_RC //-------------------
	printf("   --enable-rc\n");
#else
	printf("   --disable-rc\n");
#endif
#ifdef USE_NKF //-------------------
	printf("   --enable-nkf\n");
#else
	printf("   --disable-nkf\n");
#endif
#ifdef ENABLE_HISTORY //-------------------
	printf("   --enable-history\n");
#else
	printf("   --disable-history\n");
#endif
#ifdef ENABLE_REGEX //-------------------
	printf("   --enable-regex\n");
#else
	printf("   --disable-regex\n");
#endif
#ifdef ENABLE_SYNTAX //-------------------
	printf("   --enable-syntax\n");
#else
	printf("   --disable-syntax\n");
#endif
#ifdef ENABLE_UNDO //-------------------
	printf("   --enable-undo\n");
#else
	printf("   --disable-undo\n");
#endif
#ifdef ENABLE_HELP //-------------------
	printf("   --enable-help\n");
#else // ENABLE_HELP
	printf("   --disable-help\n");
#endif // ENABLE_HELP
#ifdef ENABLE_HIGH_BGC //-------------------
	printf("   --enable-highbgc\n");
#else
	printf("   --disable-highbgc\n");
#endif
#ifdef ENABLE_DEBUG //-------------------
	printf("   --enable-debug\n");
#else
	printf("   --disable-debug\n");
#endif
#if defined(USE_NCURSES) || defined(USE_NCURSESW) //-------------------
	printf("   --enable-ncurses\n");
#else
	printf("   --disable-ncurses\n");
#endif
#ifdef USE_PCRE //-------------------
	printf("   --enable-pcre\n");
#else
	printf("   --disable-pcre\n");
#endif
#ifdef USE_BUSYBOX //-------------------
	printf("   --enable-busybox\n");
#else
	printf("   --disable-busybox\n");
#endif
}

// End of main.c
