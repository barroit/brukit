/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRLIST_H
#define NG39_STRLIST_H

#include "list.h"
#include "types.h"

struct strbuf;

struct strlist_item {
	union {
		struct strbuf *sb;
		const xchar *sr;
		xchar *sc;
	};

	struct list_head list;
};

struct strlist {
	struct list_head head;
	struct list_head idle;

	u32 flags;
};

/*
 * SL_STORE_COPY [| SL_DUP_ON_POP]
 *	sl_pop() returns malloc'ed address
 *
 * (SL_STORE_SBUF | SL_STORE_REF)
 *	do not pass address returned by sl_pop() to free(3)
 *
 * SL_DUP_ON_POP | [(SL_STORE_COPY | SL_STORE_REF)]
 *	sl_pop() returns malloc'ed address
 *
 * Hope we don't repeat the messy API of win32 :(
 */

#define SL_STORE_COPY (1 << 0)
#define SL_STORE_SBUF (1 << 1)
#define SL_STORE_REF  (1 << 2)
#define SL_CALC_SRLEN (1 << 3)  /* calculate string length; applicable to
				   SL_STORE_REF only */
#define SL_DUP_ON_POP (1 << 4)  /* SL_STORE_COPY has this enabled by default
				   due to internal implementation */

#define SL_INIT(name) {				\
	.head  = LIST_HEAD_INIT(name.head),	\
	.idle  = LIST_HEAD_INIT(name.idle),	\
	.flags = SL_STORE_COPY | SL_DUP_ON_POP,	\
}

#define STRLIST(name) struct strlist name = SL_INIT(name)

void sl_init(struct strlist *sl, u32 flags);

void sl_destroy(struct strlist *sl);

uint sl_push(struct strlist *sl, const xchar *s);

xchar *sl_pop(struct strlist *sl);

/*
 * LC_CTYPE-insensitive implementation for reading a string into lines. Be
 * sure to use this function only on verified strings, as it doesn't handle
 * many edge cases and may encounter unexpected results.
 */
void sl_read_line(struct strlist *sl, const xchar *s, uint wrap);

#endif /* NG39_STRLIST_H */
