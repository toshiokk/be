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

#define NODE_NEXT(node)			((node)->next)
#define NODE_PREV(node)			((node)->prev)

// "NODES" are "BUFFERs" or "LINEs"
// get BUFFER from BUFFERs or get LINE from BUFFER
#define NODES_TOP_ANCH(nodes)		(&((nodes)->top_anchor))
#define NODES_TOP_NODE(nodes)		(NODE_NEXT(NODES_TOP_ANCH(nodes)))
#define NODES_BOT_NODE(nodes)		(NODE_PREV(NODES_BOT_ANCH(nodes)))
#define NODES_BOT_ANCH(nodes)		(&((nodes)->bot_anchor))

//|Node pos.     |IS_PTR_NULL     |IS_TOP_MOST    |IS_NODE_INT    |IS_BOT_MOST    |IS_PTR_NULL|
//|              |        |IS_TOP_ANCH    |IS_TOP_NODE    |IS_BOT_NODE    |IS_BOT_ANCH        |
//|--------------|--------|-------|-------|-------|-------|-------|-------|-------|-----------|
//|TOP_ANCH->prev|    1   |   0   |   1   |   0   |   0   |   0   |   0   |   0   |   0       |
//|TOP_ANCH      |    0   |   1   |   1   |   0   |   0   |   0   |   0   |   0   |   0       |
//|TOP_NODE      |    0   |   0   |   1   |   1   |   1   |   0   |   0   |   0   |   0       |
//|INT_NODE      |    0   |   0   |   0   |   0   |   1   |   0   |   0   |   0   |   0       |
//|BOT_NODE      |    0   |   0   |   0   |   0   |   1   |   1   |   1   |   0   |   0       |
//|BOT_ANCH      |    0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0       |
//|BOT_ANCH->next|    0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1       |

//|Node pos.     |IS_PTR_NULL     |IS_TOP_MOST    |IS_NODE_INT    |IS_BOT_MOST    |IS_PTR_NULL|
//|              |        |IS_TOP_ANCH    |IS_TOP_NODE    |IS_BOT_NODE    |IS_BOT_ANCH        |
//|--------------|--------|-------|-------|-------|-------|-------|-------|-------|-----------|
//|TOP_ANCH->prev|    1   |   0   |   1   |   0   |   0   |   0   |   0   |   0   |   0       |
//|TOP_ANCH      |    0   |   1   |   1   |   0   |   0   |   0   |   0   |   0   |   0       |
//|no-valid-node |    -   |   -   |   -   |   -   |   -   |   -   |   -   |   -   |   -       |
//|BOT_ANCH      |    0   |   0   |   0   |   0   |   0   |   0   |   1   |   1   |   0       |
//|BOT_ANCH->next|    0   |   0   |   0   |   0   |   0   |   0   |   1   |   0   |   1       |

// "NODE" is "BUFFER" or "LINE"
#define IS_NODE_TOP_MOST(node)	(IS_NODE_TOP(node) || IS_NODE_TOP_ANCH(node) || IS_PTR_NULL(node))
#define IS_NODE_TOP_ANCH(node)	(IS_PTR_VALID(node) && IS_PTR_NULL(NODE_PREV(node)))
#define IS_NODE_TOP(node)		(IS_NODE_INT(node) && IS_NODE_TOP_ANCH(NODE_PREV(node)))
#define IS_NODE_INT(node)		(IS_PTR_VALID(node) \
							 && IS_PTR_VALID(NODE_PREV(node)) && IS_PTR_VALID(NODE_NEXT(node)))
#define IS_NODE_BOT(node)		(IS_NODE_INT(node) && IS_NODE_BOT_ANCH(NODE_NEXT(node)))
#define IS_NODE_BOT_ANCH(node)	(IS_PTR_VALID(node) && IS_PTR_NULL(NODE_NEXT(node)))
#define IS_NODE_BOT_MOST(node)	(IS_NODE_BOT(node) || IS_NODE_BOT_ANCH(node) || IS_PTR_NULL(node))

#define IS_NODE_ANCH(node)		(IS_NODE_TOP_ANCH(node) || IS_NODE_BOT_ANCH(node))

// ## Node structure ----------------------------------------------------------
//   top-anch
//     top-anch.prev = NULL
//     top-anch.next ==> node(1)
//   node(1)
//     node(1).prev ==> top-anch
//     node(1).next ==> node(2)
//   node(2)
//     node(2).prev ==> node(1)
//     node(2).next ==> node(3)
//                        :
//                        :
//   node(N-1)
//     node(N-1).prev ==> node(N-2)
//     node(N-1).prev ==> node(N)
//   node(N)
//     node(N).prev ==> node(N-1)
//     node(N).prev ==> bottom-anch
//   bottom-anch
//     bottom-anch.prev ==> node(N)
//     bottom-anch.next = NULL
//
// ## Node handling policy =======================================================
// ### Node insertion ----------------------------------------------------------
// - Insert to top:
//     insert between NODES_TOP_ANCH(nodes) and NODES_TOP_ANCH(nodes).next
//     insert_after(NODES_TOP_ANCH(nodes))
// - Insert to bottom:
//     insert between NODES_BOT_ANCH(nodes) and NODES_TOP_ANCH(nodes).prev
//     insert_before(NODES_BOT_ANCH(nodes))
//
// ### Node iteration ----------------------------------------------------------
// - When visit all nodes (including top/bottom-anchor)
//     for (node = NODES_TOP_ANCH(nodes);	// for (node = &(nodes.top_anchor);
//      IS_PTR_VALID(node);
//      node = NODE_NEXT(nodes)) {			// node = node->next;
//         // do something to "node"
//     }
// - When visit all intermediate nodes (excluding top/bottom-anchor)
//     for (node = NODES_TOP_NODE(nodes);	// for (node = &(nodes.top_anchor.next);
//      IS_NODE_INT(node);
//      node = NODE_NEXT(nodes)) {			// node = node->next;
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
be_line_t *line_init(be_line_t *line, char *initial_data);
be_line_t *line_set_string(be_line_t *line, const char *string);
be_line_t *line_set_string_len(be_line_t *line, const char *string, len_t len);
char *line_get_string(be_line_t *line);
void line_free(be_line_t *line);
void line_free_data(be_line_t *line);

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

be_line_t *line_separate(be_line_t *line, int byte_idx, insert_before_after_t before_after);

be_line_t *line_insert_with_string(be_line_t *line,
 insert_before_after_t before_after, const char *string);
be_line_t *line_insert_with_string_len(be_line_t *line,
 insert_before_after_t before_after, const char *string, len_t len);
void line_insert_with_string_len_before(be_line_t *line, const char *string, len_t len);

be_line_t *line_create_with_string(const char *string);
be_line_t *line_create_with_string_len(const char *string, len_t len);
be_line_t *line_concat_with_prev(be_line_t *line);
be_line_t *line_concat_with_next(be_line_t *line);

int line_renumber_from_line(be_line_t *line, size_t *_buf_size_);
const be_line_t *line_get_top_anch(const be_line_t *line);

size_t line_data_len(const be_line_t *line);
size_t line_data_strlen(const char *str);

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
