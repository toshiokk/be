/**************************************************************************
 *   line.c                                                               *
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

// be_line_t manipulation routines ============================================

be_line_t *line_create(void)
{
	be_line_t *line;

	line = (be_line_t *)malloc__(sizeof(be_line_t));
	return line_init(line, "");
}
be_line_t *line_init(be_line_t *line, const char *string)
{
	// initialize be_line_t
	line->prev = NULL;
	line->next = NULL;
	line->data = NULL;
	line->size = 0;
	line->line_num = 0;
	line->buf_size = 0;
	if (string && string[0]) {
		line_set_string(line, string);
	}
	return line;
}
be_line_t *line_create_with_string(const char *string)
{
	return line_create_with_string_len(string, -1);
}
be_line_t *line_create_with_string_len(const char *string, len_t len)
{
	be_line_t *new_line;

	new_line = line_create();
	return line_set_string_len(new_line, string, len);
}
be_line_t *line_set_string(be_line_t *line, const char *string)
{
	return line_set_string_len(line, string, -1);
}
be_line_t *line_set_string_len(be_line_t *line, const char *string, len_t len)
{
	char buffer[MAX_EDIT_LINE_LEN + 1];

	if (line->data) {
		if (IS_OVERWRAP(line->data, line->data + MAX_EDIT_LINE_LEN,
		 string, string + MAX_EDIT_LINE_LEN)) {
			// string will be destroyed in remalloc(), copy it to temporary buffer
			strlcpy__(buffer, string, MAX_EDIT_LINE_LEN);
			string = buffer;
		}
	}
	if (len < 0) {
		// limit length by MAX_EDIT_LINE_LEN
		len = byte_idx_from_byte_idx(string, MAX_EDIT_LINE_LEN);
	} else {
		len = byte_idx_from_byte_idx(string, LIM_MAX(len, MAX_EDIT_LINE_LEN));
	}
	line->size = len + 1;
	line->data = char_remalloc(line->data, len + 1);
	strlcpy__(line->data, string, len);	// copy string
	return line;			// return line
}
char *line_get_string(be_line_t *line)
{
	return line->data;
}
void line_free(be_line_t *line)
{
	if (line) {
		if (line->data) {
			FREE_CLR_PTR(line->data);
		}
		free__(line);
	}
}

// This is deep-copy.
be_line_t *line_create_copy(be_line_t *src)
{
	return line_copy(line_create(), src);
}
// This is deep-copy.
be_line_t *line_copy(be_line_t *dest, be_line_t *src)
{
	memcpy__(dest, src, sizeof(*src));
	dest->prev = NULL;
	dest->next = NULL;
	dest->data = NULL;
	return line_set_string(dest, src->data);
}

be_line_t *line_insert(be_line_t *line, be_line_t *new_line,
 insert_before_after_t before_after)
{
	if (before_after == INSERT_BEFORE) {
		return line_insert_before(line, new_line);
	} else {
		return line_insert_after(line, new_line);
	}
}
be_line_t *line_insert_before(be_line_t *line, be_line_t *new_line)
{
	return line_insert_between(line->prev, new_line, line);
}
be_line_t *line_insert_after(be_line_t *line, be_line_t *new_line)
{
	return line_insert_between(line, new_line, line->next);
}
// Splice a line into an existing be_line_t
be_line_t *line_insert_between(be_line_t *prev, be_line_t *new_line, be_line_t *next)
{
	line_link(prev, new_line);
	line_link(new_line, next);
	return new_line;
}
// Connect two lines
be_line_t *line_link(be_line_t *prev, be_line_t *next)
{
	if (prev) {
		prev->next = next;
	}
	if (next) {
		next->prev = prev;
	}
	return prev;
}

be_line_t *line_unlink_free(be_line_t *line)
{
	be_line_t *next_line;

	next_line = line->next;
	line_unlink(line);
	line_free(line);
	return next_line;	// return the next line
}
// Unlink a line from the rest of the be_line_t
be_line_t *line_unlink(be_line_t *line)
{
	line_avoid_wild_ptr_cur(line);
	if (line->prev) {
		line->prev->next = line->next;
	}
	if (line->next) {
		line->next->prev = line->prev;
	}
	line->prev = NULL;
	line->next = NULL;
	return line;
}

be_line_t *line_concat(be_line_t *line, concat_prev_next_t prev_next)
{
	if (prev_next == WITH_PREV) {
		return line_concat_with_prev(line);
	} else {
		return line_concat_with_next(line);
	}
}
// concatenate the line with the previous line
//[before]
//  aaaa			This line is freed.
// >bbbb			This line is returned.
//[after]
// >aaaabbbb
be_line_t *line_concat_with_prev(be_line_t *line)
{
	if (IS_NODE_TOP_ANCH(line)) {
		_PROGERR_
		return line;
	}
	if (IS_NODE_BOT_ANCH(line->prev)) {
		_PROGERR_
		return line;
	}
	//  aaaa
	// >bbbb
	line_insert_string(line, 0, line->prev->data, -1);
	//  aaaa
	// >aaaabbbb
	line_unlink_free(line->prev);
	// >aaaabbbb
	return line;
}
// concatenate the line with the next line
//[before]
// >aaaa			This line is returned.
//  bbbb			This line is freed.
//[after]
// >aaaabbbb
be_line_t *line_concat_with_next(be_line_t *line)
{
	if (IS_NODE_TOP_ANCH(line)) {
		_PROGERR_
		return line;
	}
	if (IS_NODE_BOT_ANCH(line->next)) {
		_PROGERR_
		return line;
	}
	// >aaaa
	//  bbbb
	line_insert_string(line, line_data_len(line), line->next->data, -1);
	// >aaaabbbb
	//  bbbb
	line_unlink_free(NEXT_NODE(line));
	// >aaaabbbb
	return line;
}
//-----------------------------------------------------------------------------
be_line_t *line_delete_string(be_line_t *line, int byte_idx, int delete_len)
{
	return line_replace_string(line, byte_idx, delete_len, "", 0);
}
be_line_t *line_insert_string(be_line_t *line, int byte_idx, const char *string, int insert_len)
{
	return line_replace_string(line, byte_idx, 0, string, insert_len);
}
be_line_t *line_replace_string(be_line_t *line, int byte_idx, int delete_len,
 const char *string, len_t insert_len)
{
	char buffer[MAX_EDIT_LINE_LEN + 1];

	strlcpy__(buffer, line->data, MAX_EDIT_LINE_LEN);
	replace_str(buffer, MAX_EDIT_LINE_LEN, byte_idx, delete_len, string, insert_len);
	return line_set_string(line, buffer);
}
//-----------------------------------------------------------------------------
be_line_t *line_insert_with_string(be_line_t *line, insert_before_after_t before_after,
 const char *string)
{
	return line_insert_with_string_len(line, before_after, string, -1);
}
be_line_t *line_insert_with_string_len(be_line_t *line, insert_before_after_t before_after,
 const char *string, len_t len)
{
	be_line_t *new_line;

	new_line = line_create_with_string_len(string, len);
	line_insert(line, new_line, before_after);
	return new_line;			// return inserted line
}
//-----------------------------------------------------------------------------
be_line_t *line_separate(be_line_t *line, int byte_idx, insert_before_after_t before_after)
{
	be_line_t *new_line;

	if (before_after == INSERT_BEFORE) {
		// insert before
		// aaaa^BBBBBB
		new_line = line_insert_with_string_len(line, before_after, line->data, byte_idx);
		// aaaa				<== newly created line
		// aaaa^BBBBBB
		line_set_string(line, &(line->data[byte_idx]));		// aaaaBBBBBB ==> BBBBBB
		// aaaa				<== newly created line
		// BBBBBB
	} else {
		// insert after
		// aaaa^bbbbbb
		new_line = line_insert_with_string(line, before_after, &line->data[byte_idx]);
		// aaaa^BBBBBB
		// BBBBBB			<== newly created line
		line_set_string_len(line, line->data, byte_idx);	// aaaaBBBBBB ==> aaaa
		// aaaa
		// BBBBBB			<== newly created line
	}
	return new_line;			// return newly created(inserted) line
}
//-----------------------------------------------------------------------------
int line_renumber_from_line(be_line_t *line, size_t *_buf_size_)
{
	int line_num = 0;
	size_t buf_size = 0;

	if (IS_NODE_TOP_ANCH(line)) {
		line = NEXT_NODE(line);
	} else if (IS_NODE_TOP(line)) {
		// nothing to do
	} else {
		// continue renumbering from middle line
		line_num = line->prev->line_num;
		buf_size = line->prev->buf_size;
	}
	for ( ; line && IS_NODE_BOT_ANCH(line) == 0; line = NEXT_NODE(line)) {
		line->line_num = ++line_num;
		if (line->size <= 0) {
			if (line->data)
				line->size = line_data_len(line) + 1;
		}
		buf_size += line->size;
		line->buf_size = buf_size;
	}
	if (_buf_size_) {
		*_buf_size_ = buf_size;
	}
	return line_num;
}

int line_count_lines(be_line_t *line)
{
	int count;

	for (count = 0; IS_NODE_BOT_ANCH(line) == 0; count++) {
		line = NEXT_NODE(line);
	}
	return count;
}

const be_line_t *line_get_top_anch(const be_line_t *line)
{
	for ( ; IS_NODE_INT(line); ) {
		line = PREV_NODE(line);
	}
	return line;
}

size_t line_data_len(const be_line_t *line)
{
	return line_strlen(line->data);
}
size_t line_strlen(const char *str)
{
	return strnlen(str, MAX_EDIT_LINE_LEN);
}

#ifdef ENABLE_DEBUG
void line_dump_lines_from_top(const be_line_t *line, int lines, const be_line_t *cur_line)
{
	line_dump_lines(line_get_top_anch(line), lines, cur_line);
}
void line_dump_lines(const be_line_t *line, int lines, const be_line_t *cur_line)
{
	for ( ; line && lines > 0; line = NEXT_NODE(line), lines--) {
		line_dump_cur(line, cur_line);
	}
}
void line_dump(const be_line_t *line)
{
	line_dump_cur(line, NULL);
}
void line_dump_cur(const be_line_t *line, const be_line_t *cur_line)
{
	if (line == NULL) {
		progerr_printf("line == NULL\n");
		return;
	}
	flf_d_printf("%s%03d,%08lx,<%08lx,>%08lx,%04d,%06d,%08lx[%s]\n",
	 line == cur_line ? ">" : " ",
	 line->line_num,
	 line, PREV_NODE(line), NEXT_NODE(line),
	 line->size, line->buf_size,
	 line->data, line->data);
}
void line_dump_byte_idx(const be_line_t *line, int byte_idx)
{
	flf_d_printf("%3d: ", line->line_num);	dump_str_w_caret(line->data, byte_idx);
}
#endif // ENABLE_DEBUG

// End of line.c
