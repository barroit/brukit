/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRBUF_H
#define NG39_STRBUF_H

#include "attr.h"
#include "types.h"

struct strbuf_offset {
	uint cwd;	/* 'working space' for path operations */
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

static inline void sb_trunc_to_cwd(struct strbuf *sb)
{
	sb->len = sb->off.cwd;
	sb->buf[sb->len] = 0;
}

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s);

static inline uint sb_puts(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->len, s);
}

static inline uint sb_puts_at_cwd(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->off.cwd, s);
}

uint sb_putc_at(struct strbuf *sb, uint off, xchar c);

static inline uint sb_putc(struct strbuf *sb, xchar c)
{
	return sb_putc_at(sb, sb->len, c);
}

static inline uint sb_putc_at_cwd(struct strbuf *sb, xchar c)
{
	return sb_putc_at(sb, sb->off.cwd, c);
}

uint sb_printf_at(struct strbuf *sb,
		  uint off, const xchar *fmt, ...) __printf(3, 4);

uint sb_printf(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

uint sb_printf_at_cwd(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

void sb_trim(struct strbuf *sb);


/*
 * __sb_mb_str*
 *	doesn't return a copy if buffer type is char *
 *
 * sb_mb_str*
 *	returns a copy of string
 */
char *__sb_mb_str(struct strbuf *sb);

char *__sb_mb_str_fb(struct strbuf *sb, const char *fb);

char *sb_mb_str(struct strbuf *sb);

char *sb_mb_str_fb(struct strbuf *sb, const char *fb);


void sb_pth_init(struct strbuf **sb, const xchar *name);

void sb_pth_reinit(struct strbuf **sb, const xchar *name);

xchar *sb_pth_detach(struct strbuf **sb);

void sb_pth_destroy(struct strbuf **sb);

uint sb_pth_push_cwd(struct strbuf *sb, const xchar *name);

uint sb_pth_push(struct strbuf *sb, const xchar *name);

uint sb_pth_push_no_sep(struct strbuf *sb, const xchar *name);

uint __sb_pth_join(struct strbuf *sb, ...);

#define sb_pth_join(sb, ...) __sb_pth_join(sb, __VA_ARGS__, NULL);

static inline void sb_pth_sync_cwd(struct strbuf *sb)
{
	sb->off.cwd = sb->len;
}

/*
 * Doesn't update sb->off.cwd
 */
void sb_pth_pop(struct strbuf *sb);

int sb_pth_in_root(struct strbuf *sb);

void sb_pth_legacy_init_cwd(struct strbuf *sb, const xchar *name);

void sb_pth_legacy_reinit_cwd(struct strbuf *sb, const xchar *name);

uint sb_pth_legacy_append(struct strbuf *sb, const xchar *name);

uint sb_pth_legacy_append_at_cwd(struct strbuf *sb, const xchar *name);

void sb_pth_legacy_to_dirname(struct strbuf *sb);

#endif /* NG39_STRBUF_H */
