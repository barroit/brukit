/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * NB: if you mix push() with push_back(), or pop() with pop_front(), the
 * behavior is undefined.
 */

#ifndef NG39_STRLIST_H
#define NG39_STRLIST_H

#include "compiler.h"
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
 * SL_DUP_ON_POP [|(SL_STORE_COPY | SL_STORE_REF)]
 *	sl_pop() returns malloc'ed address
 *
 * (SL_STORE_SBUF | SL_STORE_REF)
 *	don't call free(3) on the return value of sl_pop()
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

#define __SL_DEF_FLAGS (SL_STORE_COPY | SL_DUP_ON_POP)

#define SL_INIT(...) ADAP_CALL(__SL_INIT_, __VA_ARGS__)

#define __SL_INIT_1(name) __SL_INIT_2(name, __SL_DEF_FLAGS)
#define __SL_INIT_2(name, f) {			\
	.head  = LIST_HEAD_INIT(name.head),	\
	.idle  = LIST_HEAD_INIT(name.idle),	\
	.flags = f,				\
}

#define STRLIST(...) ADAP_CALL(__STRLIST_, __VA_ARGS__)

#define __STRLIST_1(name)    __STRLIST_2(name, __SL_DEF_FLAGS)
#define __STRLIST_2(name, f) struct strlist name = __SL_INIT_2(name, f)

void sl_init(struct strlist *sl, u32 flags);

void sl_destroy(struct strlist *sl);

uint __sl_push(struct strlist *sl, const xchar *s, int is_que);

static inline uint sl_push(struct strlist *sl, const xchar *str)
{
	return __sl_push(sl, str, 0);
}

static inline uint sl_push_back(struct strlist *sl, const xchar *str)
{
	return __sl_push(sl, str, 1);
}

xchar *__sl_pop(struct strlist *sl, int is_que);

static inline xchar *sl_pop(struct strlist *sl)
{
	return __sl_pop(sl, 0);
}

static inline xchar *sl_pop_front(struct strlist *sl)
{
	return __sl_pop(sl, 1);
}

/*
 * LC_CTYPE-insensitive implementation for reading a string into lines. Be
 * sure to use this function only on verified strings, as it doesn't handle
 * many edge cases and may encounter unexpected results.
 *
 * This function uses sl_push_back(), so it must pair with sl_pop_front().
 */
void sl_read_line(struct strlist *sl, const xchar *s, uint wrap);

#endif /* NG39_STRLIST_H */
