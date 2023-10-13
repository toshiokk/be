/**************************************************************************
 *   line.h                                                               *
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

#ifndef line_h
#define line_h

// Max line length of text editable
#define MAX_EDIT_LINE_LEN	4096

#define IS_PTR_VALID(ptr)		((ptr) != NULL)
#define IS_PTR_INVALID(ptr)		IS_PTR_NULL(ptr)
#define IS_PTR_NULL(ptr)		((ptr) == NULL)

#define NEXT_NODE(node)			((node)->next)
#define PREV_NODE(node)			((node)->prev)

// "NODES" are "BUFFERs" or "LINEs"
// get BUFFER from BUFFERs or get LINE from BUFFER
#define NODES_TOP_ANCH(nodes)		(&((nodes)->top_anchor))
#define NODES_TOP_NODE(nodes)		(NEXT_NODE(NODES_TOP_ANCH(nodes)))
#define NODES_BOT_NODE(nodes)		(PREV_NODE(NODES_BOT_ANCH(nodes)))
#define NODES_BOT_ANCH(nodes)		(&((nodes)->bot_anchor))
// "NODE" is "BUFFER" or "LINE"
#define IS_NODE_TOP_ANCH(node)		(IS_PTR_NULL(node) || IS_PTR_NULL((node)->prev))
#define IS_NODE_TOP(node)			(IS_PTR_NULL(node) || IS_NODE_TOP_ANCH((node)->prev))
#define IS_NODE_INT(node)			IS_NODE_INTERMEDIATE(node)
#define IS_NODE_INTERMEDIATE(node)	(!IS_NODE_TOP_ANCH(node) && !IS_NODE_BOT_ANCH(node))
#define IS_NODE_BOT(node)			(IS_PTR_NULL(node) || IS_NODE_BOT_ANCH((node)->next))
#define IS_NODE_BOT_ANCH(node)		(IS_PTR_NULL(node) || IS_PTR_NULL((node)->next))
#define IS_NODE_INVALID(node)		(IS_NODE_TOP_ANCH(node) || IS_NODE_BOT_ANCH(node))
#define IS_NODE_VALID(node)			(IS_NODE_INVALID(node) == 0)

// Node handling policy =======================================================
// ## Node structure ----------------------------------------------------------
//   NODES_TOP_ANCH(nodes).prev = NULL;
//   NODES_TOP_ANCH(nodes).next ==> top-node
//                                     :
//                                     :
//   NODES_TOP_ANCH(nodes).prev ==> bottom-node
//   NODES_BOT_ANCH(nodes).next = NULL;
//
// ## Node insertion ----------------------------------------------------------
// - Insert to top:
//     insert between NODES_TOP_ANCH(nodes) and NODES_TOP_ANCH(nodes).next
//     insert_after(NODES_TOP_ANCH(nodes))
// - Insert to bottom:
//     insert between NODES_BOT_ANCH(nodes) and NODES_TOP_ANCH(nodes).prev
//     insert_before(NODES_BOT_ANCH(nodes))
//
// ## Node iteration ----------------------------------------------------------
// - When visit all nodes (including top/bottom-anchor)
//     for (node = NODES_TOP_ANCH(nodes);   // for (node = &(nodes.top_anchor);
//      IS_PTR_VALID(node);
//      node = NEXT_NODE(nodes)) {		// node = node->next;
//         // do something to "node"
//     }
// - When visit all intermediate nodes (excluding top/bottom-anchor)
//     for (node = NODES_TOP_NODE(nodes);	// for (node = &(nodes.top_anchor.next);
//      IS_NODE_INT(node);
//      node = NEXT_NODE(nodes)) {		// node = node->next;
//         // do something to "node"
//     }

typedef struct be_line_t {
	struct be_line_t *prev;		//!< Previous line
	struct be_line_t *next;		//!< Next line
	char *data;					//!< string (NULL: not allocated)
	int size;					//!< line size (including '\0')(0: not allocated)
	int line_num;				//!< line number (1,2,....)
	size_t buf_size;			//!< buffer size accumulated from line 1 to this line
} be_line_t;

typedef enum {
	WITH_PREV,		// concatenate with previous
	WITH_NEXT		// concatenate with next
} concat_prev_next_t;

typedef enum {
	INSERT_BEFORE,	// insert before
	INSERT_AFTER	// insert after
} insert_before_after_t;

be_line_t *line_create(void);
be_line_t *line_init(be_line_t *line, const char *string);
be_line_t *line_set_string(be_line_t *line, const char *string);
be_line_t *line_set_string_len(be_line_t *line, const char *string, len_t len);
char *line_get_string(be_line_t *line);
void line_free(be_line_t *line);

be_line_t *line_create_copy(be_line_t *src);
be_line_t *line_copy(be_line_t *dest, be_line_t *src);

be_line_t *line_insert(be_line_t *line, be_line_t *new_line,
 insert_before_after_t before_after);
be_line_t *line_insert_before(be_line_t *line, be_line_t *new_line);
be_line_t *line_insert_after(be_line_t *line, be_line_t *new_line);
be_line_t *line_insert_between(be_line_t *prev, be_line_t *new_line, be_line_t *next);
be_line_t *line_link(be_line_t *prev, be_line_t *next);
be_line_t *line_unlink_free(be_line_t *line);
be_line_t *line_unlink(be_line_t *line);

be_line_t *line_delete_string(be_line_t *line, int byte_idx, int delete_len);
be_line_t *line_insert_string(be_line_t *line, int byte_idx, const char *string, int insert_len);
be_line_t *line_replace_string(be_line_t *line, int byte_idx, int delete_len,
 const char *string, len_t insert_len);
be_line_t *line_insert_with_string(be_line_t *line,
 insert_before_after_t before_after, const char *string);
be_line_t *line_insert_with_string_len(be_line_t *line,
 insert_before_after_t before_after, const char *string, len_t len);
be_line_t *line_create_with_string(const char *string);
be_line_t *line_create_with_string_len(const char *string, len_t len);
be_line_t *line_concat(be_line_t *line, concat_prev_next_t prev_next);
be_line_t *line_concat_with_prev(be_line_t *line);
be_line_t *line_concat_with_next(be_line_t *line);

be_line_t *line_separate(be_line_t *line, int byte_idx, insert_before_after_t before_after);

int line_renumber_from_line(be_line_t *line, size_t *_buf_size_);
int line_count_lines(be_line_t *line);
const be_line_t *line_get_top_anch(const be_line_t *line);

size_t line_data_len(const be_line_t *line);
size_t line_strlen(const char *str);

//-----------------------------------------------------------------------------

#ifdef ENABLE_DEBUG
void line_dump_lines_from_top(const be_line_t *line, int lines, const be_line_t *cur_line);
void line_dump_lines(const be_line_t *line, int lines, const be_line_t *cur_line);
void line_dump(const be_line_t *line);
void line_dump_cur(const be_line_t *line, const be_line_t *cur_line);
void line_dump_byte_idx(const be_line_t *line, int byte_idx);
#endif // ENABLE_DEBUG

#endif // line_h

// End of line.h
