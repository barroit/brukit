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

void sb_destroy(struct strbuf *sb);

xchar *sb_detach(struct strbuf *sb);

void sb_trunc(struct strbuf *sb, uint len);

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s);

uint sb_puts(struct strbuf *sb, const xchar *s);

uint sb_puts_at_ws(struct strbuf *sb, const xchar *s);

uint sb_printf_at(struct strbuf *sb,
		  uint off, const xchar *fmt, ...) __printf(3, 4);

uint sb_printf(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

uint sb_printf_at_ws(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

void sb_trim(struct strbuf *sb);

void sb_init_ws(struct strbuf *sb, const xchar *name);

uint sb_pth_append(struct strbuf *sb, const xchar *name);

uint sb_pth_append_at_ws(struct strbuf *sb, const xchar *name);

void sb_pth_to_dirname(struct strbuf *sb);

#endif /* NG39_STRBUF_H */
