/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRBUF_H
#define NG39_STRBUF_H

#include "attr.h"
#include "types.h"

struct strbuf_offset {
	size_t cwd;	/* 'working space' for path operations */
};

struct strbuf {
	xchar *buf;

	size_t len;
	size_t cap;

	struct strbuf_offset off;
};

#define SB_INIT { 0 }

void sb_init(struct strbuf *sb);

void sb_destroy(struct strbuf *sb);

static inline void sb_trunc(struct strbuf *sb, size_t len)
{
	sb->len -= len;
	sb->buf[sb->len] = 0;
}

static inline void sb_trunc_to_cwd(struct strbuf *sb)
{
	sb->len = sb->off.cwd;
	sb->buf[sb->len] = 0;
}

size_t sb_puts_at(struct strbuf *sb, size_t off, const xchar *s);

static inline size_t sb_puts(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->len, s);
}

static inline size_t sb_cwd_puts(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->off.cwd, s);
}

size_t sb_putc_at(struct strbuf *sb, size_t off, xchar c);

static inline size_t sb_putc(struct strbuf *sb, xchar c)
{
	return sb_putc_at(sb, sb->len, c);
}

static inline size_t sb_cwd_putc(struct strbuf *sb, xchar c)
{
	return sb_putc_at(sb, sb->off.cwd, c);
}

size_t sb_printf_at(struct strbuf *sb,
		    size_t off, const xchar *fmt, ...) __printf(3, 4);

size_t sb_printf(struct strbuf *sb, const xchar *fmt, ...) __printf(2, 3);

size_t sb_printf_at_cwd(struct strbuf *sb,
			const xchar *fmt, ...) __printf(2, 3);

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

size_t sb_pth_push_cwd(struct strbuf *sb, const xchar *name);

size_t sb_pth_push(struct strbuf *sb, const xchar *name);

size_t sb_pth_push_no_sep(struct strbuf *sb, const xchar *name);

size_t __sb_pth_join(struct strbuf *sb, ...);

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

void sb_pth_init_cwd_dumb(struct strbuf *sb, const xchar *name);

void sb_pth_reinit_cwd_dumb(struct strbuf *sb, const xchar *name);

size_t sb_pth_append_dumb(struct strbuf *sb, const xchar *name);

size_t sb_pth_append_at_cwd_dumb(struct strbuf *sb, const xchar *name);

void sb_pth_to_dirname_dumb(struct strbuf *sb);

#endif /* NG39_STRBUF_H */
