/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRLIST_H
#define NG39_STRLIST_H

#include "compiler.h"
#include "list.h"
#include "types.h"

struct strbuf;

struct strentry {
	u32 mode;

	union {
		struct strbuf *sb;
		const xchar *sr;
		xchar *sc;
		char *__sc;
	};
	size_t len;

	struct list_head list;
};

struct strlist {
	struct list_head head;
	struct list_head idle;

	uint items;

	u32 flags;
};

/* In SL_MODE_MV, item->len stays unset without this. */
#define SL_CALC_LEN (1U << 0)

#define SL_MODE_CP (1U << 28)  /* stores a copy of str */
#define SL_MODE_MV (1U << 29)  /* stores a reference to str */
#define SL_MODE_SB (1U << 30)  /* stores str in strbuf */
#define SL_MODE_MB (1U << 31)  /* same as SL_MODE_MB, but *str must be char */

#define sl_mode_mask (-1U << 28)
#define sl_mode(f)   ((f) & sl_mode_mask)

#define SL_INIT(...) ADAP_CALL(__SL_INIT_, __VA_ARGS__)

#define __SL_INIT_1(name) __SL_INIT_2(name, SL_MODE_CP)
#define __SL_INIT_2(name, f) {			\
	.head  = LIST_HEAD_INIT(name.head),	\
	.idle  = LIST_HEAD_INIT(name.idle),	\
	.flags = f,				\
}

#define STRLIST(...) ADAP_CALL(__STRLIST_, __VA_ARGS__)

#define __STRLIST_1(name) __STRLIST_2(name, SL_MODE_CP)
#define __STRLIST_2(name, f) struct strlist name = __SL_INIT_2(name, f)

void sl_init(struct strlist *sl, u32 flags);

void sl_destroy(struct strlist *sl);

struct strentry *__sl_push(struct strlist *sl,
			   const xchar *str, size_t len, int backward);

#define sl_push(sl, str) __sl_push(sl, str, -1, 0)

#define sl_push_mb(sl, str) sl_push(sl, (void *)str)

#define sl_push_back(sl, str) __sl_push(sl, str, -1, 1)

#define sl_push_back_mb(sl, str) sl_push_back(sl, (void *)str)

struct strentry *sl_pop(struct strlist *sl);

void sl_put_idle(struct strlist *sl, struct strentry *item);

void sl_free(struct strentry *item);

const xchar *sl_str(struct strentry *item);

#define sl_str_mb(item) ((const char *)sl_str(item))

/**
 * sl_read_line - split string into lines
 * @sl:    strlist to add lines to
 * @str:   input string
 * @wrap:  line wrap length
 *
 * Breaks @str into multiple lines, wrapping at @wrap width, and appends each
 * line to @sl.
 *
 * This algorithm assumes monospace font.
 */
void sl_read_line(struct strlist *sl, const xchar *str, size_t wrap);

void sl_read_line_mb(struct strlist *sl,
		     const char *str, size_t len, size_t wrap);

void sl_read_line_wc(struct strlist *sl,
		     const wchar_t *str, size_t len, size_t wrap);

xchar **sl_to_argv(struct strlist *sl);

void free_argv(xchar **argv);

#endif /* NG39_STRLIST_H */
