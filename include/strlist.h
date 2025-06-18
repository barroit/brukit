/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * The behavior is undefined if
 *	mixing push() with push_back()
 *	non-SL_STORE_CHR mode strlist uses sl_*_chr() functions
 *	SL_STORE_REF mode strlist uses sl_read_line()
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
		char *__sc;
	};

	struct list_head list;
};

struct strlist {
	struct list_head head;
	struct list_head idle;

	uint items;

	u32 flags;
};

/*
 * flags & SL_STORE_COPY
 * flags & SL_DUP_ON_POP
 *	sl_pop() returns malloc'ed address
 *
 * (flags & (SL_STORE_SBUF | SL_DUP_ON_POP)) == SL_STORE_SBUF
 * (flags & (SL_STORE_REF | SL_DUP_ON_POP)) == SL_STORE_REF
 *	don't call free(3) on the return value of sl_pop()
 */

/*
 * General-purpose flags
 *
 * SL_CALC_SRLEN
 *	calculate string length, only works with SL_STORE_REF
 *
 * SL_DUP_ON_POP
 *	duplicate strings when pop-related functions return, default for
 *	SL_STORE_COPY
 *
 * Operating modes
 *
 * SL_STORE_COPY
 *	store a copy of the strings
 *
 * SL_STORE_SBUF
 *	store strings in strbuf
 *
 * SL_STORE_REF
 *	store references to the strings
 *
 * SL_STORE_CHR
 *	like SL_STORE_COPY but forces narrow character storage
 */
#define SL_CALC_SRLEN (1U << 0)
#define SL_DUP_ON_POP (1U << 1)

#define SL_STORE_CHR  (1U << 28)
#define SL_STORE_REF  (1U << 29)
#define SL_STORE_SBUF (1U << 30)
#define SL_STORE_COPY (1U << 31)

#define SL__STORE_CHR  (SL_STORE_CHR | SL_DUP_ON_POP)
#define SL__STORE_COPY (SL_STORE_COPY | SL_DUP_ON_POP)

#define SL_INIT(...) ADAP_CALL(__SL_INIT_, __VA_ARGS__)

#define __SL_INIT_1(name) __SL_INIT_2(name, SL__STORE_COPY)
#define __SL_INIT_2(name, f) {			\
	.head  = LIST_HEAD_INIT(name.head),	\
	.idle  = LIST_HEAD_INIT(name.idle),	\
	.flags = f,				\
}

#define STRLIST(...) ADAP_CALL(__STRLIST_, __VA_ARGS__)

#define __STRLIST_1(name)    __STRLIST_2(name, SL__STORE_COPY)
#define __STRLIST_2(name, f) struct strlist name = __SL_INIT_2(name, f)

void sl_init(struct strlist *sl, u32 flags);

void sl_destroy(struct strlist *sl);

size_t __sl_push(struct strlist *sl, const xchar *str, int is_que);

static inline size_t sl_push(struct strlist *sl, const xchar *str)
{
	return __sl_push(sl, str, 0);
}

static inline size_t sl_push_chr(struct strlist *sl, const char *str)
{
	return sl_push(sl, (xchar *)str);
}

static inline size_t sl_push_back(struct strlist *sl, const xchar *str)
{
	return __sl_push(sl, str, 1);
}

static inline size_t sl_push_back_chr(struct strlist *sl, const char *str)
{
	return sl_push_back(sl, (xchar *)str);
}

xchar *sl_pop(struct strlist *sl);

static inline char *sl_pop_chr(struct strlist *sl)
{
	return (char *)sl_pop(sl);
}

/*
 * LC_CTYPE-insensitive implementation for reading a string into lines. Be
 * sure to use this function only on verified strings, as it doesn't handle
 * many edge cases and may encounter unexpected results.
 */
void sl_read_line(struct strlist *sl, const xchar *str, uint wrap);

void sl_read_line_chr(struct strlist *sl, const char *str, uint wrap);

xchar **sl_to_argv(struct strlist *sl);

#endif /* NG39_STRLIST_H */
