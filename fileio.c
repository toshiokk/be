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

#ifdef USE_NKF
PRIVATE int nkf_avalability = -1;	// -1: Unkown, 0: unavailable, 1: available
#endif // USE_NKF

PRIVATE int backup_files(const char *file_path, int depth);
PRIVATE char *make_backup_file_path(const char *orig_path, char *backup_path, int depth);

PRIVATE int load_file_into_new_buf__(const char *full_path, int flags);
PRIVATE int load_file_into_cur_buf__(const char *full_path, int flags);

#ifdef USE_NKF
PRIVATE int guess_encoding_by_nkf(const char *full_path);
PRIVATE int my_guess_bin_file(const char *full_path);
PRIVATE int my_guess_utf8_file(const char *full_path);
PRIVATE int load_file_into_cur_buf_nkf(const char *full_path, const char *nkf_options);
#endif // USE_NKF
PRIVATE int load_file_into_cur_buf_ascii(const char *file_name);
PRIVATE int load_file_into_cur_buf_binary(const char *full_path);
PRIVATE int load_into_cur_buf_fp(FILE *fp);

#ifdef USE_NKF
PRIVATE int save_cur_buf_to_file_nkf(const char *file_path, const char *nkf_options);
#endif // USE_NKF
PRIVATE int save_cur_buf_to_file_ascii(const char *file_path);
PRIVATE int save_cur_buf_to_file_binary(const char *file_path);
PRIVATE int save_cur_buf_to_fp(const char *file_path, FILE *fp);

int load_file_into_new_buf(const char *full_path, int flags)
{
	int lines = load_file_into_new_buf__(full_path, flags);
	if (lines < 0) {
		return lines;
	}
	// successfully loaded into new buffer

	if (append_magic_line_if_necessary()) {
		// Automatically add a magic-line
		SET_CUR_EBUF_STATE(buf_MAGIC_LINE_ADDED, 1);
	}
	buf_set_view_x_cur_line(get_epc_buf(), 0, CUR_EDIT_BUF_TOP_LINE);
	buf_set_view_x_cur_line(get_epc_buf(), 1, CUR_EDIT_BUF_TOP_LINE);
	BUFV0_CLBI(get_epc_buf()) = 0;
	BUFV1_CLBI(get_epc_buf()) = 0;
	renumber_cur_buf_from_top();
	update_cur_buf_crc();

	int tab_size;
	if ((tab_size = buf_guess_tab_size(get_epc_buf())) != 0) {
		GET_CUR_EBUF_STATE(buf_TAB_SIZE) = tab_size;
	}
	if (lines > MAX_LINES_LOADABLE) {
		disp_status_bar_done(_("New file"));		// new file (0 line)
	} else {
		disp_status_bar_ing(P_(_("%d line read %s"),
							   _("%d lines read %s"),
							   _("%d liness read %s"),
							   _("%d linesss read %s"),
		 lines), lines, buf_eol_str(get_epc_buf()));
	}
	return lines;	// >= 0: success
}
PRIVATE int load_file_into_new_buf__(const char *full_path, int flags)
{
	struct stat st;
	int ret = -1;

	if (is_strlen_not_0(full_path)) {
		ret = stat(full_path, &st);
	}
	if (ret < 0) {
		// New file
		if ((flags & OOE1) == 0) {
			if (flags & MOE1) {
				disp_status_bar_err(_("File [%s] not found"),
				 shrink_str_to_scr_static(full_path));
			}
			return -1;	// open error
		}
		create_edit_buf(full_path);
		lock_epc_buf_if_file_already_locked((flags & FOLF1) == 0);
		disp_status_bar_ing(_("New File"));
		return MAX_LINES_LOADABLE+1;		// new file (0 line)
	}
	if (S_ISREG(st.st_mode) == 0) {
		// special file
		if (flags & MOE1) {
			disp_status_bar_err(S_ISDIR(st.st_mode)
			 ? _("[%s] is a directory") : _("[%s] is a special file"),
			 shrink_str_to_scr_static(full_path));
		}
		return -2;		// open error
	}
	// Max. file size loadable is half of free memory.
	if ((st.st_size / 1000) >= get_mem_free_in_kb(1) / 2) {
		// file size too large
		disp_status_bar_err(_("[%s] is too large to read into buffer"),
		 shrink_str_to_scr_static(full_path));
		return -3;		// open error
	}
	// regular file
	disp_status_bar_ing(_("Reading File %s ..."), shrink_str_to_scr_static(full_path));
	create_edit_buf(full_path);
	lock_epc_buf_if_file_already_locked((flags & FOLF1) == 0);
	// set a edit-buffer not saveable if requested
	SET_CUR_EBUF_STATE(buf_MODE, (flags & RDOL1) ? buf_MODE_RO : 0);
	// memorize orginal file stat
	buf_get_file_stat(get_epc_buf(), full_path);

	ret = load_file_into_cur_buf__(full_path, flags & MOE1);

	if (ret < 0) {
		free_cur_edit_buf();
		return ret;
	}
	return ret;		// >= 0: loaded(lines)
}

//------------------------------------------------------------------------------
int backup_and_save_cur_buf_ask()
{
	char file_path[MAX_PATH_LEN+1];
	int ret = 0;

	// write to "abs_path" to avoid changing symlink but change a file pointed by symlink
	buf_get_file_path(get_epc_buf(), file_path);
	if (is_strlen_0(file_path) || file_path[0] == '#') {
		disp_status_bar_warn(_("File name is invalid"));
		return -1;
	}
	if (buf_has_orig_file_updated(get_epc_buf(), file_path) > 0) {
		// file is modified by another program
		ret = ask_yes_no(ASK_YES_NO,
		 _("File has modified by another program, OVERWRITE ?"));
		if (ret < 0) {
			disp_status_bar_warn(_("Overwriting file cancelled"));
			return -1;
		}
	}
	return backup_and_save_cur_buf(file_path);
}

int backup_and_save_cur_buf(const char *file_path)
{
	// TODO: do minimum check
	if (is_path_regular_file(file_path) == 0) {
		// directory or special file
		disp_status_bar_err(_("File [%s] is NOT a regular file !!"),
		 shrink_str_to_scr_static(file_path));
		return -1;
	}
	// file_path is exist as a regular file or not exist
	disp_status_bar_ing(_("Writing File %s ..."),
	 shrink_str_to_scr_static(file_path));
	if (GET_APPMD(ed_BACKUP_FILES)) {
		if (backup_files(file_path, get_backup_files()) < 0) {
			disp_status_bar_err(_("Error in backing up File [%s] !!"),
			 shrink_str_to_scr_static(file_path));
			return -1;
		}
	}

	int lines_written = save_cur_buf_to_file(file_path);

	if (S_ISREG(get_epc_buf()->orig_file_stat.st_mode)) {
		int mask = get_epc_buf()->orig_file_stat.st_mode & 07777;
		if (chmod(file_path, mask) < 0) {
			disp_status_bar_err(_("Can not set permissions %1$o on [%2$s]: %3$s"),
			 mask, shrink_str_to_scr_static(file_path), strerror(errno));
		}
	}
	// file was overwritten, get current file stat into orig_file_stat
	buf_get_file_stat(get_epc_buf(), file_path);
	update_cur_buf_crc();
	clear_cur_buf_modified();

	disp_status_bar_ing(P_(_("%d line written"),
						   _("%d lines written"),
						   _("%d liness written"),
						   _("%d linesss written"),
	 lines_written), lines_written);
	disp_title_bar_editor();

	return lines_written;		// -1: error
}
PRIVATE int backup_files(const char *file_path, int depth)
{
	depth = LIM_MIN(0, depth);
	for ( ; depth > 0; depth--) {
		char orig_path[MAX_PATH_LEN+1];
		char backup_path[MAX_PATH_LEN+1];
		make_backup_file_path(file_path, orig_path, depth-1);
		make_backup_file_path(file_path, backup_path, depth);
		if (is_path_regular_file(orig_path) > 0) {
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
		strlcat__(backup_path, MAX_PATH_LEN, BACKUP_FILE_SUFFIX);
	return backup_path;
}

//------------------------------------------------------------------------------

int load_file_into_buf(be_buf_t *buf, const char *full_path)
{
	be_buf_t *buf_save = get_epc_buf();
	set_epc_buf(buf);

	int ret = load_file_into_cur_buf__(full_path, 0);

	set_epc_buf(buf_save);
	return ret;		// >= 0: success
}

PRIVATE int load_file_into_cur_buf__(const char *full_path, int flags)
{
#ifdef USE_NKF
	const char *nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
#endif // USE_NKF

#ifdef USE_NKF
	if (GET_APPMD(ed_USE_NKF)) {
		if (GET_CUR_EBUF_STATE(buf_ENCODE) == ENCODE_ASCII) {
			// encoding is not specified on command line
			guess_encoding_by_nkf(full_path);
			if (GET_CUR_EBUF_STATE(buf_ENCODE) == ENCODE_BINARY) {
				disp_status_bar_warn(_("BINARY file !! [%s]"),
				 shrink_str_to_scr_static(full_path));
			}
		}
		switch (GET_CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
			////nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
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
		case ENCODE_BINARY:
			nkf_options = "-w";		// output UTF8
			break;
		}
		switch (GET_CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
			break;
		case ENCODE_EUCJP:
		case ENCODE_SJIS:
		case ENCODE_JIS:
			return load_file_into_cur_buf_nkf(full_path, nkf_options);
		case ENCODE_BINARY:
			return load_file_into_cur_buf_binary(full_path);
		}
	} // if (GET_APPMD(ed_USE_NKF))
#endif // USE_NKF
	switch (GET_CUR_EBUF_STATE(buf_ENCODE)) {
	default:
	case ENCODE_ASCII:
	case ENCODE_UTF8:
#ifdef USE_NKF
	case ENCODE_EUCJP:
	case ENCODE_SJIS:
	case ENCODE_JIS:
#endif // USE_NKF
		return load_file_into_cur_buf_ascii(full_path);
	case ENCODE_BINARY:
		return load_file_into_cur_buf_binary(full_path);
	}
}

//------------------------------------------------------------------------------

int save_buf_to_file(be_buf_t *buf, const char *file_path)
{
flf_d_printf("[%s]\n", file_path);
	be_buf_t *buf_save = get_epc_buf();
	set_epc_buf(buf);

	int ret = save_cur_buf_to_file(file_path);

	set_epc_buf(buf_save);
	return ret;
}

int save_cur_buf_to_file(const char *file_path)
{
#ifdef USE_NKF
	const char *nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
#endif // USE_NKF

#ifdef USE_NKF
	if (GET_APPMD(ed_USE_NKF)) {
		switch (GET_CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
			////nkf_options = "-Wwx";	// input UTF8, output UTF8, preserve HankakuKana
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
		case ENCODE_BINARY:
			nkf_options = "-W";		// input UTF8
			break;
		}
		switch (GET_CUR_EBUF_STATE(buf_ENCODE)) {
		default:
		case ENCODE_ASCII:
		case ENCODE_UTF8:
			break;
		case ENCODE_EUCJP:
		case ENCODE_SJIS:
		case ENCODE_JIS:
			if (nkf_avalability > 0) {
				return save_cur_buf_to_file_nkf(file_path, nkf_options);
			}
			break;
		case ENCODE_BINARY:
			return save_cur_buf_to_file_binary(file_path);
		}
	}
#endif // USE_NKF
	switch (GET_CUR_EBUF_STATE(buf_ENCODE)) {
	default:
	case ENCODE_ASCII:
	case ENCODE_UTF8:
#ifdef USE_NKF
	case ENCODE_EUCJP:
	case ENCODE_SJIS:
	case ENCODE_JIS:
#endif // USE_NKF
		return save_cur_buf_to_file_ascii(file_path);
	case ENCODE_BINARY:
		return save_cur_buf_to_file_binary(file_path);
	}
}

//------------------------------------------------------------------------------

#ifdef USE_NKF
PRIVATE int guess_encoding_by_nkf(const char *full_path)
{
	GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	// No encoding specified in the command line
	char buffer[MAX_PATH_LEN+1];
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf -g \"%s\"", full_path);
	FILE *fp;
	if ((fp = popen(buffer, "r")) <= 0) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	if (fgets(buffer, MAX_PATH_LEN, fp) == NULL) {
		// have read nothing, clear buffer
		buffer[0] = '\0';
		nkf_avalability = 0;	// set unavailable (0)
	} else {
		nkf_avalability = 1;	// set available (1)
	}
	if (pclose(fp) == -1) {
		return -1;
	}
	if (strlcmp__(buffer, "ASCII") == 0) {
		GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	} else if (strlcmp__(buffer, "UTF-8") == 0) {
		GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_UTF8;
	} else if (strlcmp__(buffer, "EUC-JP") == 0) {
		int guess_utf8 = my_guess_utf8_file(full_path);
		if (guess_utf8 < 0) {
			GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_EUCJP;
		} else {
			GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_UTF8;
		}
	} else if (strlcmp__(buffer, "Shift_JIS") == 0) {
		GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_SJIS;
	} else if (strlcmp__(buffer, "ISO-2022-JP") == 0) {
		GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_JIS;
	} else if (strlcmp__(buffer, "BINARY") == 0
	 && my_guess_bin_file(full_path)) {
		GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_BINARY;
	} else {
		// maybe, no nkf is available
		GET_CUR_EBUF_STATE(buf_ENCODE) = ENCODE_ASCII;
	}
	return 0;
}
// nkf's guessing of binary file is too strict.
// use my own guessing of binary file.
PRIVATE int my_guess_bin_file(const char *full_path)
{
#define BYTES_TO_BE_CHKED_BIN		MAX_PATH_LEN
#define BYTES_TO_BE_GUESSED_BIN		((BYTES_TO_BE_CHKED_BIN) / 20)
	FILE *fp = fopen(full_path, "rb");
	if (fp == NULL) {
		return -1;
	}
	int may_be_bin = 0;
	int bin_bytes_found = 0;
	for (int bytes_chked = 0; bytes_chked < BYTES_TO_BE_CHKED_BIN; ) {
		unsigned char bin_buf[BYTES_TO_BE_CHKED_BIN];
		int bytes = fread(bin_buf, 1, BYTES_TO_BE_CHKED_BIN, fp);
		if (bytes <= 0) {
			break;
		}
		for (int off = 0; off < bytes; off++) {
			switch (bin_buf[off]) {
			// 07:BEL, 09:TAB, 0a:LF, 0c:FF, 0d:CR, 1b:ESC
			case 0x07: case 0x09: case 0x0a: case 0x0c: case 0x0d: case 0x1b:
				break;
			default:
				if ((bin_buf[off] < 0x20) || (bin_buf[off] == 0x7f)) {
					bin_bytes_found++;
				}
				break;
			}
		}
		if (bin_bytes_found >= BYTES_TO_BE_GUESSED_BIN) {
			may_be_bin = bin_bytes_found;
			break;
		}
		bytes_chked += bytes;
	}
	if (fclose(fp) != 0) {	// 0: OK, EOF: error
		// nothing to do
	}
	return may_be_bin;
}
PRIVATE int my_guess_utf8_file(const char *full_path)
{
#define BYTES_TO_BE_CHKED_UTF8			65536
	FILE *fp = fopen(full_path, "rb");
	if (fp == NULL) {
		return -1;
	}
	char utf8c_state = 0;
	int utf8_bytes_found = 0;
	int illegal_utf8_seq_found = 0;
	for (int bytes_chked = 0; bytes_chked < BYTES_TO_BE_CHKED_UTF8; ) {
		unsigned char bin_buf[BYTES_TO_BE_CHKED_UTF8];
		int bytes = fread(bin_buf, 1, BYTES_TO_BE_CHKED_UTF8, fp);
		if (bytes <= 0) {
			break;
		}
		for (int off = 0; off < bytes; off++) {
			utf8c_state = utf8c_len(utf8c_state, bin_buf[off]);
			if (utf8c_state < 0) {
				// illegal sequence found
				illegal_utf8_seq_found++;
				break;
			} else
			if (utf8c_state > 0) {
				utf8_bytes_found++;
			}
		}
		if (illegal_utf8_seq_found) {
			break;
		}
		bytes_chked += bytes;
	}
	if (fclose(fp) != 0) {	// 0: OK, EOF: error
		// nothing to do
	}
	if (illegal_utf8_seq_found) {
		return -1;				// illegal utf8 sequence (maybe EUC-JP or Shift_JIS)
	}
	return utf8_bytes_found;	// 0: ASCII, >0: UTF8
}
PRIVATE int load_file_into_cur_buf_nkf(const char *full_path, const char *nkf_options)
{
	char buffer[MAX_PATH_LEN+1];
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf %s \"%s\"", nkf_options, full_path);
	FILE *fp;
	if ((fp = popen(buffer, "r")) <= 0) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	int lines = load_into_cur_buf_fp(fp);
	if (pclose(fp) == -1) {	// -1: error
		lines = -1;
	}
	return lines;
}
#endif // USE_NKF

PRIVATE int load_file_into_cur_buf_ascii(const char *full_path)
{
	FILE *fp = fopen(full_path, "rb");
	if (fp == NULL) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	int lines = load_into_cur_buf_fp(fp);
	if (fclose(fp) != 0) {	// 0: OK, EOF: error
		lines = -1;
	}
	return lines;
}

PRIVATE int load_file_into_cur_buf_binary(const char *full_path)
{
#define BIN_LINE_LEN	64
#define BIN_BASE_CODE	0x2800	// use "Braille pattern" to show binary bytes
	FILE *fp = fopen(full_path, "rb");
	if (fp == NULL) {
		disp_status_bar_err(_("Can not read-open file [%s]: %s"),
		 shrink_str_to_scr_static(full_path), strerror(errno));
		return -1;
	}
	int lines = 0;
	for ( ; ; ) {
		unsigned char bin_buf[BIN_LINE_LEN];
		int bytes = fread(bin_buf, 1, BIN_LINE_LEN, fp);
		if (bytes <= 0) {
			break;
		}
		char text_buf[BIN_LINE_LEN * MAX_UTF8C_BYTES + 1] = "";
		for (int off = 0; off < bytes; off++) {
			unsigned char byte = bin_buf[off];
			char utf8c[MAX_UTF8C_BYTES+1];
			int utf8c_len;
			if (byte < ' ' || 0x7f <= byte) {
				// 0x00 ~ 0x1f --> 0xXX00 ~ 0xXX1f, 0x7f ~ 0xff --> 0xXX7f ~ 0xXXff
				utf8c_len = utf8c_encode(BIN_BASE_CODE + byte, utf8c);
			} else {
				utf8c[0] = byte;
				utf8c[1] = '\0';
				utf8c_len = 1;
			}
			strlncat__(text_buf, BIN_LINE_LEN * MAX_UTF8C_BYTES, utf8c, utf8c_len);
		}
		lines++;
		append_string_to_cur_edit_buf(text_buf);
	}
	if (fclose(fp) != 0) {	// 0: OK, EOF: error
		lines = -1;
	}
	return lines;
}

PRIVATE void fgetc_bufed_clear();
PRIVATE int fgetc_buffered(FILE *fp);

PRIVATE inline void load_into_cur_buf_append_line(be_line_t* line, char* line_buf, int* len,
 int* lines_read)
{
	line_insert_with_string_len_before(line, line_buf, *len);
	*len = 0;
	line_buf[0] = '\0';
	(*lines_read)++;
}
PRIVATE int load_into_cur_buf_fp(FILE *fp)
{
	int file_format_idx = 0;	// 0 = nix, 1 = Mac, 2 = DOS
	int lines_read = 0;
	int prev_chr = '\0';		// previous read character
	char line_buf[MAX_EDIT_LINE_LEN+1];
	int len = 0;
	be_line_t *line = CUR_EDIT_BUF_BOT_ANCH;

	line_buf[len] = '\0';
	fgetc_bufed_clear();
	for ( ; ; ) {
		int chr_int = fgetc_buffered(fp);	// read character
		switch (chr_int) {
		case '\n':
			if (prev_chr == '\r') {
				file_format_idx = 2;	// LF after CR (DOS format)
			} else {
				// LF (nix format)
				load_into_cur_buf_append_line(line, line_buf, &len, &lines_read);
			}
			break;
		case '\r':
			// CR (DOS/Mac format)
			file_format_idx = 1;		// line end is only CR (Mac format)
			load_into_cur_buf_append_line(line, line_buf, &len, &lines_read);
			break;
		default:
			if ((IS_UTF8_1ST_BYTE(chr_int) == 0)
			 ? (len >= MAX_EDIT_LINE_LEN)
			 : (len + utf8c_len(0, chr_int) >= MAX_EDIT_LINE_LEN)) {
				load_into_cur_buf_append_line(line, line_buf, &len, &lines_read);
			}
			line_buf[len++] = chr_int;
			line_buf[len] = '\0';
			break;
		case EOF:
			if (len > 0) {
				load_into_cur_buf_append_line(line, line_buf, &len, &lines_read);
			}
			break;
		}
		prev_chr = chr_int;
		if (chr_int == EOF)
			break;
	}
	switch (file_format_idx) {
	default:
	case 0:
		set_buf_eol(EOL_NIX);
		break;
	case 1:
		set_buf_eol(EOL_MAC);
		break;
	case 2:
		set_buf_eol(EOL_DOS);
		break;
	}
	if (check_break_key()) {
		lines_read = -1;
	}
	// 0 bytes of file returns 0
	return lines_read;	// >= 0: succeeded, < 0: error or stopped
}

// buffered file input
PRIVATE char fgetc_bufed_buf[MAX_EDIT_LINE_LEN+1];
PRIVATE int fgetc_bufed_read_len = 0;
PRIVATE int fgetc_bufed_byte_idx = 0;

PRIVATE void fgetc_bufed_clear()
{
	fgetc_bufed_read_len = 0;
	fgetc_bufed_byte_idx = 0;
}
PRIVATE int fgetc_buffered(FILE *fp)
{
	if (fgetc_bufed_byte_idx >= fgetc_bufed_read_len) {
		if (check_break_key()) {
			return EOF;
		}
		if ((fgetc_bufed_read_len = fread(fgetc_bufed_buf, 1, MAX_EDIT_LINE_LEN, fp))
		 <= 0) {
			return EOF;
		}
		fgetc_bufed_byte_idx = 0;
	}
	int chr = (unsigned char)fgetc_bufed_buf[fgetc_bufed_byte_idx++];
#define NUL_REPLACE_CHR		' '		// replace '\0' to ' '
	if (chr == '\0') {
		chr = NUL_REPLACE_CHR;		// replace '\0'
	}
	return chr;
}

//------------------------------------------------------------------------------

#ifdef USE_NKF
PRIVATE int save_cur_buf_to_file_nkf(const char *file_path, const char *nkf_options)
{
	char buffer[MAX_PATH_LEN+1] = "";
	snprintf_(buffer, MAX_PATH_LEN+1, "nkf %s >\"%s\"", nkf_options, file_path);
	FILE *fp = 0;
	if ((fp = popen(buffer, "w")) <= 0) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	int lines;
	if ((lines = save_cur_buf_to_fp(file_path, fp)) < 0) {
		lines = -2;
	}
	if (pclose(fp) == -1) {	// -1: error
		disp_status_bar_err(_("Can not close file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		lines = -3;
	}
	return lines;
}
#endif // USE_NKF

PRIVATE int save_cur_buf_to_file_ascii(const char *file_path)
{
	FILE *fp = fopen(file_path, "wb");
	if (fp == NULL) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	int lines;
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

PRIVATE int save_cur_buf_to_file_binary(const char *file_path)
{
	FILE *fp = fopen(file_path, "wb");
	if (fp == NULL) {
		disp_status_bar_err(_("Can not write-open file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		return -1;
	}
	int lines_written = 0;
	for (const be_line_t *line = CUR_EDIT_BUF_TOP_LINE; IS_NODE_INT(line);
	 line = NODE_NEXT(line)) {
		if (IS_NODE_BOT_MOST(line) && (line_strlen(line) == 0)) {
			// There is a magic-line at the last line of the buffer
			if (GET_CUR_EBUF_STATE(buf_MAGIC_LINE_ADDED)) {
				// This magic-line is the one automatically added
				break;			// So do not output the magic-line
			}
		}
		unsigned char bin_buf[BIN_LINE_LEN];
		int line_len = strlen_path(line->data);
		int bin_off = 0;
		for (int text_off = 0; text_off < line_len; ) {
			bin_buf[bin_off++] = (utf8c_decode(&(line->data[text_off])) - BIN_BASE_CODE);
			text_off += utf8c_bytes(&(line->data[text_off]));
			if ((bin_off >= BIN_LINE_LEN) || (text_off >= line_len)) {
				int bytes = fwrite(bin_buf, 1, bin_off, fp);
				if (bytes < bin_off) {
					disp_status_bar_err(_("Can not write file [%s]: %s"),
					 shrink_str_to_scr_static(file_path), strerror(errno));
					lines_written = -2;
					break;
				}
				bin_off = 0;
				lines_written++;
			}
		}
	}
	if (fclose(fp) != 0) {
		disp_status_bar_err(_("Can not close file [%s]: %s"),
		 shrink_str_to_scr_static(file_path), strerror(errno));
		lines_written = -3;
	}
	return lines_written;
}

PRIVATE int save_cur_buf_to_fp(const char *file_path, FILE *fp)
{
	int lines_written = 0;
	for (const be_line_t *line = CUR_EDIT_BUF_TOP_LINE; IS_NODE_INT(line);
	 line = NODE_NEXT(line)) {
		if (IS_NODE_BOT_MOST(line) && (line_strlen(line) == 0)) {
			// There is a magic-line at the last line of the buffer
			if (GET_CUR_EBUF_STATE(buf_MAGIC_LINE_ADDED)) {
				// This magic-line is the one automatically added
				break;			// So do not output the magic-line
			}
		}
		size_t line_len = line_strlen(line);
		size_t size = fwrite(line->data, 1, line_len, fp);
		if (size < line_len) {
			disp_status_bar_err(_("Can not write file [%s]: %s"),
			 shrink_str_to_scr_static(file_path), strerror(errno));
			return -1;
		}
		switch (GET_CUR_EBUF_STATE(buf_EOL)) {
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

//------------------------------------------------------------------------------
PRIVATE int cnt_files_loaded = -1;	// -1: no file switched/loaded, 0: switched, 1--: loaded
void clear_files_loaded()
{
#ifdef ENABLE_HISTORY
	if (dir_history_fix()) { _WARNING_ }
#endif // ENABLE_HISTORY
	cnt_files_loaded = -1;
}
int add_files_loaded(int files)	// files = 0: not loaded but switched
{
	if (files >= 0) {
		cnt_files_loaded = LIM_MIN(0, cnt_files_loaded);
	}
	return cnt_files_loaded += files;
}
int get_files_loaded()
{
	return cnt_files_loaded;
}
void disp_files_loaded_if_ge_0()
{
	if (cnt_files_loaded >= 0) {
		disp_files_loaded();
	}
}
void disp_files_loaded()
{
	disp_status_bar_done(P_(_("%d file loaded"),
							_("%d files loaded"),
							_("%d filess loaded"),
							_("%d filesss loaded"),
	 cnt_files_loaded), cnt_files_loaded);
}

//------------------------------------------------------------------------------
#ifdef START_UP_TEST
void test_flock()
{
	MY_UT_INT(flock_lock("/absolute/path/to/file.txt"), 0);
	MY_UT_INT(flock_unlock("/absolute/path/to/file.txt"), 0);
	MY_UT_INT(flock_lock("/absolute/path/to/file.txt"), 0);
	MY_UT_INT(flock_lock("/absolute/path/to/file.txt"), 1);
	MY_UT_INT(flock_unlock("/absolute/path/to/file.txt"), 0);
	MY_UT_INT(flock_unlock("/absolute/path/to/file.txt"), 1);

	////
MY_UT_INT(flock_lock("/absolute/path/to/file.txt"), 0);
	// error on the next calling test_flock()
}
#endif // START_UP_TEST

PRIVATE int flock_create_lock_file(const char *full_path);
PRIVATE int flock_delete_lock_file(const char *full_path);
PRIVATE const char* flock_get_lock_file_path(const char* full_path);

int flock_lock(const char *full_path)
{
	if (flock_is_locked(full_path)) {
		return 1;	// lock already exist
	}
	flock_create_lock_file(full_path);
	return 0;		// successfully locked
}
int flock_unlock(const char *full_path)
{
	if (flock_is_locked(full_path) == 0) {
		return 1;	// lock not exist
	}
	flock_delete_lock_file(full_path);
	return 0;		// successfully unlocked
}
int flock_is_locked(const char *full_path)
{
	return is_path_exist(flock_get_lock_file_path(full_path));
}
PRIVATE int flock_create_lock_file(const char *full_path)
{
	return write_text_to_file(flock_get_lock_file_path(full_path), 0, "");
}
PRIVATE int flock_delete_lock_file(const char *full_path)
{
	return remove_file(flock_get_lock_file_path(full_path));
}

//      "/path/to/a/file/being/locked.txt"
//  ==> ".path.to.a.file.being.locked.txt"
//  ==> "$APP_DIR/.path.to.a.file.being.locked.txt"
// e.g. "/home/user/.be/.home.user.tools.be.be.app_defs.txt"
PRIVATE const char* flock_get_lock_file_path(const char* full_path)
{
	static char lock_file_path[MAX_PATH_LEN+1];
	char abs_path[MAX_PATH_LEN+1];
	get_abs_path(full_path, abs_path);
	if (strlcmp__(abs_path, "/") != 0) {
		progerr_printf("not full path [%s]\n", abs_path);
	}
	str_tr(abs_path, '/', '.');
	cat_dir_and_file(lock_file_path, get_app_dir(), abs_path);
	return lock_file_path;
}

// End of fileio.c
