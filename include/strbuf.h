/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRBUF_H
#define NG39_STRBUF_H

#define SB_INIT { 0 }

void sb_init_ws(struct strbuf *sb, const xchar *name);

void sb_destroy(struct strbuf *sb);

xchar *sb_detach(struct strbuf *sb);

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s);

#define sb_puts(__sb, s) sb_puts_at(__sb, (__sb)->len, s)

#define sb_puts_at_ws(__sb, s) sb_puts_at(__sb, (__sb)->off.ws, s)

uint sb_printf_at(struct strbuf *sb,
		  uint off, const xchar *fmt, ...) __printf(3, 4);

#define sb_printf(__sb, fmt, ...)				\
	__must_have_va_arg(__VA_ARGS__) +			\
	sb_printf_at(__sb, (__sb)->len, fmt, __VA_ARGS__)

#define sb_printf_at_ws(__sb, fmt, ...)				\
	__must_have_va_arg(__VA_ARGS__) +			\
	sb_printf_at(__sb, (__sb)->off.ws, fmt, __VA_ARGS__)

void sb_trim(struct strbuf *sb);

uint sb_pth_append(struct strbuf *sb, const xchar *name);

void sb_pth_to_dirname(struct strbuf *sb);

#endif /* NG39_STRBUF_H */
