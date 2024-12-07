/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRBUF_H
#define NG39_STRBUF_H

#include "attr.h"
#include "types.h"

struct strbuf_offset {
	uint ws;	/* a 'working space' for path operations */
};

struct strbuf {
	xchar *buf;

	uint len;
	uint cap;

	struct strbuf_offset off;
};

#define SB_INIT { 0 }

void sb_init(struct strbuf *sb);

void sb_destroy(struct strbuf *sb);

xchar *sb_detach(struct strbuf *sb);

static inline void sb_trunc(struct strbuf *sb, uint len)
{
	sb->len -= len;
	sb->buf[sb->len] = 0;
}

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s);

static inline uint sb_puts(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->len, s);
}

static inline uint sb_puts_at_ws(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->off.ws, s);
}

uint sb_putc_at(struct strbuf *sb, uint off, xchar c);

static inline uint sb_putc(struct strbuf *sb, xchar c)
{
	return sb_putc_at(sb, sb->len, c);
}

static inline uint sb_putc_at_ws(struct strbuf *sb, xchar c)
{
	return sb_putc_at(sb, sb->off.ws, c);
}

uint sb_printf_at(struct strbuf *sb,
		  uint off, const xchar *fmt, ...) __printf(3, 4);

uint sb_printf(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

uint sb_printf_at_ws(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

void sb_trim(struct strbuf *sb);

void sb_init_ws(struct strbuf *sb, const xchar *name);

uint sb_pth_append(struct strbuf *sb, const xchar *name);

uint sb_pth_append_at_ws(struct strbuf *sb, const xchar *name);

void sb_pth_to_dirname(struct strbuf *sb);

char *sb_mbs(struct strbuf *sb) __malloc;

char *sb_mbs_fb(struct strbuf *sb, const char *alt) __malloc;

#endif /* NG39_STRBUF_H */
