/**************************************************************************
 *   fileio.c                                                             *
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

PRIVATE int load_file_into_new_buf__(const char *full_path, int open_on_err, int msg_on_err);
PRIVATE int load_file_into_cur_buf__(const char *full_path, int load_binary_file, int msg_on_err);

PRIVATE int load_file_into_cur_buf_ascii(const char *file_name);
#ifdef USE_NKF
PRIVATE int guess_encoding_by_nkf(const char *full_path);
PRIVATE int load_file_into_cur_buf_nkf(const char *full_path, const char *nkf_options);
#endif // USE_NKF
PRIVATE int load_fp_into_cur_buf(FILE *fp);

int load_file_into_new_buf(const char *full_path, int open_on_err, int msg_on_err)
{
	int tab_size;
	int lines;

	lines = load_file_into_new_buf__(full_path, open_on_err, msg_on_err);

///flf_d_printf("%s:%d\n", full_path, lines);
	if (lines < 0) {
		return lines;
	}
	append_magic_line();
	CEPBV_CL = CUR_EDIT_BUF_TOP_NODE;
	renumber_cur_buf_from_top();
	update_cur_buf_crc();

	if ((tab_size = buf_guess_tab_size(get_cep_buf())) != 0) {
		CUR_EBUF_STATE(buf_TAB_SIZE) = tab_size;
	}
	disp_status_bar_ing(P_(_("%d line read %s"),
						   _("%d lines read %s"),
						   _("%d liness read %s"),
						   _("%d linesss read %s"),
	 lines), lines, buf_eol_str(get_cep_buf()));
	return lines;
}
PRIVATE int load_file_into_new_buf__(const char *full_path, int open_on_err, int msg_on_err)
{
	struct stat fileinfo;
	int ret = -1;

	if (is_strlen_not_0(full_path)) {
		ret = stat(full_path, &fileinfo);
	}
///flf_d_printf("[%s], ret %d\n", full_path, ret);
	if (ret < 0) {
		// New file
		if (open_on_err == 0) {
			if (msg_on_err) {
				disp_status_bar_err(_("File [%s] not found"), shrink_str_to_scr_static(full_path));
			}
			return -1;	// open error
		}
		create_edit_buf(full_path);
		disp_status_bar_ing(_("New File"));
		return 1;		// new file
	}
	if (S_ISREG(fileinfo.st_mode) == 0) {
		// special file
		if (msg_on_err) {
			disp_status_bar_err(S_ISDIR(fileinfo.st_mode)
			 ? _("[%s] is a directory") : _("[%s] is a special file"),
			 shrink_str_to_scr_static(full_path));
		}
		return -2;		// open error
	}
	// Max. file size loadable is half of free memory.
	if ((fileinfo.st_size / 1000) >= get_mem_free_in_kb(1) / 2) {
		// file size too large
		if (msg_on_err) {
			disp_status_bar_err(_("[%s] is too large to read into buffer"),
			 shrink_str_to_scr_static(full_path));
		}
		return -3;		// open error
	}
	// regular file
	disp_status_bar_ing(_("Reading File %s ..."), shrink_str_to_scr_static(full_path));
	create_edit_buf(full_path);
	memcpy__(&get_cep_buf()->orig_file_stat, &fileinfo, sizeof(fileinfo));

	ret = load_file_into_cur_buf__(full_path, 1, msg_on_err);

	if (ret < 0) {
///_FLF_
		free_cur_edit_buf();
		return -1;
	}
	return ret;				// loaded
}

//-----------------------------------------------------------------------------

PRIVATE int load_file_into_cur_buf__(const char *full_path, int load_binary_file, int msg_on_err)
{
#ifdef USE_NKF
	const char *nkf_options;
#endif // USE_NKF

////flf_d_printf("full_path: [%s]\n", full_path);
#ifdef USE_NKF
	if (GET_APPMD(ed_USE_NKF)) {
		if (CUR_EBUF_STATE(buf_ENCODE) == ENCODE_ASCII) {
			// encoding is not specified on command line
			guess_encoding_by_nkf(full_path);
			if (CUR_EBUF_STATE(buf_ENCODE) == ENCODE_BINARY && load_binary_file == 0) {
				if (msg_on_err) {
					disp_status_bar_err(_("BINARY file !! [%s]"),
					 shrink_str_to_scr_static(full_path));
				}
				return -1;		// do not load binary file
			}
		}
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
			break;
		case ENCODE_EUCJP:
			nkf_options = "-Ewx";	// input EUCJP, output UTF8, preserve HankakuKana
			break;
		case ENCODE_SJIS:
			nkf_options = "-Swx";	// input SJIS, output UTF8, preserve HankakuKana
			break;
		case ENCODE_JIS:
			nkf_options = "-Jwx";	// input JIS, output UTF8, preserve HankakuKana
			break;
		}
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			break;
		case ENCODE_EUCJP:
		case ENCODE_SJIS:
		case ENCODE_JIS:
			return load_file_into_cur_buf_nkf(full_path, nkf_options);
		}
	} // if (GET_APPMD(ed_USE_NKF))
#endif // USE_NKF
	return load_file_into_cur_buf_ascii(full_path);
}

PRIVATE int load_file_into_cur_buf_ascii(const char *full_path)
{
	FILE *fp;
	int lines;

	if ((fp = fopen(full_path, "rb")) == NULL) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	lines = load_fp_into_cur_buf(fp);
	if (fclose(fp) != 0) {	// 0: OK, EOF: error
		lines = -1;
	}
	return lines;
}

#ifdef USE_NKF
PRIVATE int guess_encoding_by_nkf(const char *full_path)
{
	char buffer[MAX_PATH_LEN+1];
	FILE *fp;

	CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	// No encoding specified in command line
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf -g \"%s\"", full_path);
	if ((fp = popen(buffer, "r")) <= 0) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	if (fgets(buffer, MAX_PATH_LEN, fp) == NULL)
		buffer[0] = '\0';
	if (pclose(fp) == -1) {
		return -1;
	}
///flf_d_printf("guessed encoding by nkf: %s", buffer);
	if (strlcmp__(buffer, "ASCII") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	} else if (strlcmp__(buffer, "UTF-8") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_UTF8;
	} else if (strlcmp__(buffer, "EUC-JP") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_EUCJP;
	} else if (strlcmp__(buffer, "Shift_JIS") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_SJIS;
	} else if (strlcmp__(buffer, "ISO-2022-JP") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_JIS;
	} else if (strlcmp__(buffer, "BINARY") == 0) {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_BINARY;
	} else {
		CUR_EBUF_STATE(buf_ENCODE) = ENCODE_BINARY;
	}
	return 0;
}
PRIVATE int load_file_into_cur_buf_nkf(const char *full_path, const char *nkf_options)
{
	char buffer[MAX_PATH_LEN+1];
	FILE *fp;
	int lines;

///flf_d_printf("nkf_options[%s]\n", nkf_options);
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf %s \"%s\"", nkf_options, full_path);
///flf_d_printf("[%s]\n", buffer);
	if ((fp = popen(buffer, "r")) <= 0) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	lines = load_fp_into_cur_buf(fp);
	if (pclose(fp) == -1) {	// -1: error
		lines = -1;
	}
	return lines;
}
#endif // USE_NKF

PRIVATE void fgetc_buffered_clear(void);
PRIVATE int fgetc_buffered(FILE *fp);

PRIVATE int load_fp_into_cur_buf(FILE *fp)
{
	int file_format_idx = 0;	// 0 = nix, 1 = Mac, 2 = DOS
	int chr_int;			// read character
	int prev_chr = '\0';	// previous read character
	char buf[MAX_EDIT_LINE_LEN+1];
	int len;
	int lines_read = 0;

	len = 0;
	buf[len] = '\0';
	fgetc_buffered_clear();
	for ( ; ; ) {
		chr_int = fgetc_buffered(fp);
		switch (chr_int) {
		case '\n':
			if (prev_chr == '\r') {
				file_format_idx = 2;	// LF after CR (DOS format)
				break;
			}
			// LF (nix format)
			goto append_line;
		case '\r':
			// CR (DOS/Mac format)
			file_format_idx = 1;		// line end is only CR (Mac format)
			goto append_line;
		default:
			if (len >= MAX_EDIT_LINE_LEN) {
				append_string_to_cur_edit_buf(buf);
				lines_read++;
				len = 0;
				buf[len] = '\0';
			}
			buf[len++] = chr_int;
			buf[len] = '\0';
			break;
		case EOF:
			if (len == 0)
				break;
append_line:;
			append_string_to_cur_edit_buf(buf);
			lines_read++;
			len = 0;
			buf[len] = '\0';
			break;
		}
		prev_chr = chr_int;
		if (chr_int == EOF)
			break;
	}
	switch (file_format_idx) {
	default:
	case 0:
		set_eol(EOL_NIX);
		break;
	case 1:
		set_eol(EOL_MAC);
		break;
	case 2:
		set_eol(EOL_DOS);
		break;
	}
////flf_d_printf("file_format_idx: %d\n", file_format_idx);
////flf_d_printf("CUR_EBUF_STATE(buf_EOL) => [%s]\n", buf_eol_str(get_cep_buf()));
	return lines_read;
}

PRIVATE char fgetc_buffered_buf[MAX_EDIT_LINE_LEN+1];
PRIVATE int fgetc_buffered_read_len = 0;
PRIVATE int fgetc_buffered_byte_idx = 0;

PRIVATE void fgetc_buffered_clear(void)
{
	fgetc_buffered_read_len = 0;
	fgetc_buffered_byte_idx = 0;
}
PRIVATE int fgetc_buffered(FILE *fp)
{
	int chr;

	if (fgetc_buffered_byte_idx >= fgetc_buffered_read_len) {
		if ((fgetc_buffered_read_len = fread(fgetc_buffered_buf, 1, MAX_EDIT_LINE_LEN, fp)) <= 0)
			return EOF;
		fgetc_buffered_byte_idx = 0;
	}
	chr = (unsigned char)fgetc_buffered_buf[fgetc_buffered_byte_idx++];
#define NUL_REPLACE_CHR		' '		// replace '\0' to ' '
	if (chr == '\0') {
		chr = NUL_REPLACE_CHR;		// replace '\0'
	}
	return chr;
}

//-----------------------------------------------------------------------------

int backup_and_save_cur_buf_ask(void)
{
	char file_path[MAX_PATH_LEN+1];
	int ret = 0;

	strlcpy__(file_path, get_cep_buf()->abs_path, MAX_PATH_LEN);
	if (is_strlen_0(file_path)) {
		if (input_new_file_name_n_ask(file_path) <= 0) {
			return -1;
		}
	}
///flf_d_printf("[%s]\n", file_path);
	if (buf_is_orig_file_updated(get_cep_buf()) > 0) {
		// file is modified by others
		ret = ask_yes_no(ASK_YES_NO,
		 _("File was modified by another program, OVERWRITE ?"));
		if (ret < 0) {
			disp_status_bar_done(_("Cancelled"));
			return -1;
		}
	}
	if (is_strlen_0(file_path) || file_path[0] == '#') {
		disp_status_bar_done(_("Cancelled"));
		return -1;
	}
///flf_d_printf("[%s]\n", file_path);
	if ((ret = backup_and_save_cur_buf(file_path)) < 0) {
		disp_status_bar_err(_("File [%s] can NOT be written !!"),
		 shrink_str_to_scr_static(file_path));
	}
	return ret;
}

int input_new_file_name_n_ask(char *file_path)
{
	int ret = 0;

	while (1) {
		ret = input_string(file_path, file_path,
		 HISTORY_TYPE_IDX_DIR, "%s:", _("File Name to Write"));

		if (ret <= 0) {
			set_edit_win_update_needed(UPDATE_SCRN_ALL_SOON);
			return 0;		// cancelled
		}
#ifdef ENABLE_FILER
		if (strcmp(file_path, "") == 0 || is_path_wildcard(file_path)) {
			ret = call_filer(1, 1, "", "", file_path, MAX_PATH_LEN);
			if (ret <= 0)
				continue;
			strlcpy__(file_path, file_path, MAX_PATH_LEN);
		}
#endif // ENABLE_FILER
		if (is_path_exist(file_path)) {
			if (is_path_regular_file(file_path) > 0) {
				// ask Overwrite
				ret = ask_yes_no(ASK_YES_NO,
				 _("File exists, OVERWRITE it ?"));
				if (ret < 0) {
					return 0;		// cancelled
				}
				if (ret == 0)
					continue;
			} else {
				// ask non regular file
				ret = ask_yes_no(ASK_YES_NO,
				 _("Path is not file, can not WRITE it"));
				if (ret < 0) {
					return 0;		// cancelled
				}
				continue;
			}
		}
		break;
	}
///flf_d_printf("[%s]\n", file_path);
	return 1;		// input
}

//-----------------------------------------------------------------------------

PRIVATE int backup_files(const char *file_path, int depth);
PRIVATE char *make_backup_file_path(const char *orig_path, char *backup_path, int depth);
PRIVATE int save_cur_buf_to_file__(const char *file_path);

int backup_and_save_cur_buf(const char *file_path)
{
	char abs_path[MAX_PATH_LEN+1];
	int mask = 0;
	int lines_written;

///flf_d_printf("[%s]\n", file_path);
	get_abs_path(file_path, abs_path);
	// TODO: do minimum check
	//  file_path is regular file and not dir and special file
	if (is_path_regular_file(file_path) == 0) {
		disp_status_bar_err(_("File [%s] is NOT regular file !!"),
		 shrink_str_to_scr_static(file_path));
		return -1;
	}
	disp_status_bar_ing(_("Writing File %s ..."), shrink_str_to_scr_static(get_cep_buf()->abs_path));
	if (GET_APPMD(ed_BACKUP_FILES)) {
		if (backup_files(file_path, get_backup_files()) < 0) {
			return -1;
		}
	}

	lines_written = save_cur_buf_to_file__(abs_path);

	if (S_ISREG(get_cep_buf()->orig_file_stat.st_mode)) {
		mask = get_cep_buf()->orig_file_stat.st_mode & 07777;
///flf_d_printf("chmod([%s], %05o)\n", abs_path, mask);
		if (chmod(abs_path, mask) < 0) {
			disp_status_bar_err(_("Can not set permissions %1$o on [%2$s]: %3$s"),
			 mask, shrink_str_to_scr_static(abs_path), strerror(errno));
		}
	}

	// get current file stat in orig_file_stat
	stat(get_cep_buf()->file_path, &get_cep_buf()->orig_file_stat);
	update_cur_buf_crc();

	disp_status_bar_ing(P_(_("%d line written"),
						   _("%d lines written"),
						   _("%d liness written"),
						   _("%d linesss written"),
	 lines_written), lines_written);
	CUR_EBUF_STATE(buf_MODIFIED) = 0;
	disp_editor_title_bar();

	return lines_written;		// -1: error
}
PRIVATE int backup_files(const char *file_path, int depth)
{
	char orig_path[MAX_PATH_LEN+1];
	char backup_path[MAX_PATH_LEN+1];
	struct stat st;

	depth = LIM_MIN(0, depth);
	for ( ; depth > 0; depth--) {
		make_backup_file_path(file_path, backup_path, depth);
		make_backup_file_path(file_path, orig_path, depth-1);
		if (!stat(orig_path, &st)) {
			remove(backup_path);
			if (rename(orig_path, backup_path) < 0) {
				return -1;
			}
		}
	}
	return 0;
}
PRIVATE char *make_backup_file_path(const char *orig_path, char *backup_path, int depth)
{
	strlcpy__(backup_path, orig_path, MAX_PATH_LEN);
	while (depth--)
		strlcat__(backup_path, MAX_PATH_LEN, "~");
	return backup_path;
}

PRIVATE int save_cur_buf_to_file_ascii(const char *file_path);
#ifdef USE_NKF
PRIVATE int save_cur_buf_to_file_nkf(const char *file_path, const char *nkf_options);
#endif // USE_NKF
PRIVATE int save_cur_buf_to_fp(const char *file_path, FILE *fp);

PRIVATE int save_cur_buf_to_file__(const char *file_path)
{
#ifdef USE_NKF
	const char *nkf_options;
#endif // USE_NKF

#ifdef USE_NKF
	if (GET_APPMD(ed_USE_NKF)) {
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
			break;
		case ENCODE_EUCJP:
			nkf_options = "-Wex";	// input UTF8, output EUCJP, preserve HankakuKana
			break;
		case ENCODE_SJIS:
			nkf_options = "-Wsx";	// input UTF8, output SJIS, preserve HankakuKana
			break;
		case ENCODE_JIS:
			nkf_options = "-Wjx";	// input UTF8, output JIS, preserve HankakuKana
			break;
		}
		switch (CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
		case ENCODE_BINARY:
			break;
		case ENCODE_EUCJP:
		case ENCODE_SJIS:
		case ENCODE_JIS:
			return save_cur_buf_to_file_nkf(file_path, nkf_options);
		}
	}
#endif // USE_NKF
	return save_cur_buf_to_file_ascii(file_path);
}

PRIVATE int save_cur_buf_to_file_ascii(const char *file_path)
{
	FILE *fp = 0;
	int lines;

	if ((fp = fopen(file_path, "wb")) == NULL) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	if ((lines = save_cur_buf_to_fp(file_path, fp)) < 0) {
		lines = -2;
	}
	if (fclose(fp) != 0) {
		disp_status_bar_err(_("Can not close file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		lines = -3;
	}
	return lines;
}
#ifdef USE_NKF
PRIVATE int save_cur_buf_to_file_nkf(const char *file_path, const char *nkf_options)
{
	char buffer[MAX_PATH_LEN+1] = "";
	FILE *fp = 0;
	int lines;

///flf_d_printf("nkf_options[%s]\n", nkf_options);
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf %s >\"%s\"", nkf_options, file_path);
///flf_d_printf("[%s]\n", buffer);
	if ((fp = popen(buffer, "w")) <= 0) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	if ((lines = save_cur_buf_to_fp(file_path, fp)) < 0) {
		return -2;
	}
	if (pclose(fp) == -1) {	// -1: error
		disp_status_bar_err(_("Can not close file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -3;
	}
	return lines;
}
#endif // USE_NKF

PRIVATE int save_cur_buf_to_fp(const char *file_path, FILE *fp)
{
	int lines_written;
	const be_line_t *line;
	int line_len;
	long size;

	lines_written = 0;
	for (line = CUR_EDIT_BUF_TOP_NODE; IS_NODE_INT(line); line = NODE_NEXT(line)) {
		if (IS_NODE_BOT(line) && line_data_len(line) == 0)
			break;			// do not output the magic line
		line_len = line_data_len(line);
		size = fwrite(line->data, 1, line_len, fp);
		if (size < line_len) {
			disp_status_bar_err(_("Can not write file [%s]: %s"),
			 shrink_str_to_scr_static(file_path), strerror(errno));
			return -1;
		}
		switch (CUR_EBUF_STATE(buf_EOL)) {
		default:
		case EOL_NIX:
			putc('\n', fp);
			break;
		case EOL_MAC:
			putc('\r', fp);
			break;
		case EOL_DOS:
			putc('\r', fp);
			putc('\n', fp);
			break;
		}
		lines_written++;
	}
	return lines_written;
}
//-----------------------------------------------------------------------------
PRIVATE int files_loaded = 0;
void clear_files_loaded(void)
{
///_FLF_
	files_loaded = 0;
}
int add_files_loaded(int files)
{
	return files_loaded += files;
}
int get_files_loaded(void)
{
	return files_loaded;
}
void disp_files_loaded_ifnon0(void)
{
	if (files_loaded > 0) {
		disp_files_loaded();
	}
}
void disp_files_loaded(void)
{
	disp_status_bar_done(P_(_("%d file loaded"),
							_("%d files loaded"),
							_("%d filess loaded"),
							_("%d filesss loaded"),
	 files_loaded), files_loaded);
}

// End of fileio.c
