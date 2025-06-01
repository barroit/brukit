// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "strbuf.h"

#include <ctype.h>
#include <wctype.h>
#include <stdarg.h>
#include <stdio.h>

#include "strconv.h"
#include "path.h"
#include "pathwalk.h"
#include "xalloc.h"
#include "xcf.h"

void sb_init(struct strbuf *sb)
{
	memset(sb, 0, sizeof(*sb));
}

void sb_destroy(struct strbuf *sb)
{
	free(sb->buf);
}

xchar *sb_detach(struct strbuf *sb)
{
	xchar *ret = sb->buf;
	uint used = sb->len + 1;
	uint unused = align_down(sb->cap - used, 8);

	if (unused >= 64)
		ret = xrealloc(sb->buf, used);

	return ret;
}

static inline void sb_grow(struct strbuf *sb, uint new)
{
	uint size = sb->len + new + 1;

	REALLOCBUF(sb->buf, size, sb->cap);
}

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s)
{
	uint len = xc_strlen(s);
	uint overlap = sb->len - off;

	if (len > overlap)
		sb_grow(sb, len - overlap);

	memcpy(&sb->buf[off], s, (len + 1) * sizeof(*s));
	if (len > overlap)
		sb->len += len - overlap;
	else
		sb->len -= overlap - len;

	return len;
}

uint sb_putc_at(struct strbuf *sb, uint off, const xchar c)
{
	uint overlap = sb->len - off;

	if (!overlap)
		sb_grow(sb, 1);

	sb->buf[off] = c;
	sb->buf[off + 1] = 0;
	if (!overlap)
		sb->len += 1;
	else
		sb->len -= overlap - 1;

	return 1;
}

static uint __sb_printf_at(struct strbuf *sb, uint off,
			   const xchar *fmt, va_list *ap)
{
	int nr;
	uint avail;
	uint i = 0;

	sb_grow(sb, 42);

retry:
	avail = sb->cap - sb->len - 1;
	nr = xc_vsnprintf(&sb->buf[off], avail + 1, fmt, ap[i]);

	BUG_ON(nr < 0);
	if (nr > avail) {
		BUG_ON(i);
		sb_grow(sb, nr);
		i += 1;
		goto retry;
	}

	sb->len += nr;
	return nr;
}

#define SB_PRINTF(sb, off, fmt)		\
({					\
	va_list ap[2];			\
	uint ret;			\
					\
	va_start(ap[0], fmt);		\
	va_copy(ap[1], ap[0]);		\
					\
	ret = __sb_printf_at(sb, off,	\
			     fmt, ap);	\
					\
	va_end(ap[0]);			\
	va_end(ap[1]);			\
					\
	ret;				\
})

uint sb_printf_at(struct strbuf *sb, uint off, const xchar *fmt, ...)
{
	return SB_PRINTF(sb, off, fmt);
}

uint sb_printf(struct strbuf *sb, const xchar *fmt, ...)
{
	return SB_PRINTF(sb, sb->len, fmt);
}

uint sb_printf_at_cwd(struct strbuf *sb, const xchar *fmt, ...)
{
	return SB_PRINTF(sb, sb->off.cwd, fmt);
}

void sb_trim(struct strbuf *sb)
{
	xchar *h;
	xchar *t;

	h = sb->buf;
	while (xc_isspace(*h))
		h++;

	if (*h == 0) {
		sb->buf[0] = 0;
		sb->len = 0;
		return;
	}

	t = &sb->buf[sb->len];
	while (xc_isspace(*(t - 1)))
		t--;

	if (h == sb->buf && t == &sb->buf[sb->len])
		return;

	sb->len = t - h;
	memmove(sb->buf, h, sb->len * sizeof(*h));
	sb->buf[sb->len] = 0;
}

void sb_pth_legacy_init_cwd(struct strbuf *sb, const xchar *name)
{
	memset(sb, 0, sizeof(*sb));
	sb->off.cwd = sb_puts(sb, name);
}

void sb_pth_legacy_reinit_cwd(struct strbuf *sb, const xchar *name)
{
	sb_trunc(sb, sb->len);
	sb->off.cwd = sb_puts(sb, name);
}

uint sb_pth_legacy_append(struct strbuf *sb, const xchar *name)
{
	uint __len = xc_strlen(name);
	uint len = __len + 1;

	sb_grow(sb, len);

	sb->buf[sb->len] = PTH_SEP;
	sb->len += 1;

	memcpy(&sb->buf[sb->len], name, (__len + 1) * sizeof(*name));
	sb->len += __len;

	return len;
}

uint sb_pth_legacy_append_at_cwd(struct strbuf *sb, const xchar *name)
{
	uint __len = xc_strlen(name);
	uint len = __len + 1;
	uint overlap = sb->len - sb->off.cwd;

	if (len > overlap)
		sb_grow(sb, len - overlap);

	sb->buf[sb->off.cwd] = PTH_SEP;

	memcpy(&sb->buf[sb->off.cwd + 1], name, (__len + 1) * sizeof(*name));
	if (len > overlap)
		sb->len += len - overlap;
	else
		sb->len -= overlap - len;

	return len;
}

void sb_pth_legacy_to_dirname(struct strbuf *sb)
{
	xchar *sep = pth_last_sep(sb->buf);

	sb->len = sep - sb->buf;
	sb->buf[sb->len] = 0;
}

char *__sb_mb_str(struct strbuf *sb)
{
	char *ret = (char *)sb->buf;

	if (IS_ENABLED(CONFIG_ENABLE_WCHAR)) {
		size_t len = mb_wcstombs(&ret, (wchar_t *)sb->buf);

		if (len == maxof(len))
			ret = NULL;
	}

	return ret;
}

char *__sb_mb_str_fb(struct strbuf *sb, const char *fb)
{
	char *ret = __sb_mb_str(sb);

	if (ret == NULL)
		ret = (char *)fb;

	return ret;
}

char *sb_mb_str(struct strbuf *sb)
{
	char *ret = __sb_mb_str(sb);

	if (__same_type(ret, sb->buf) && ret == (char *)sb->buf)
		ret = strdup((char *)ret);

	return ret;
}

char *sb_mb_str_fb(struct strbuf *sb, const char *fb)
{
	char *ret = __sb_mb_str_fb(sb, fb);

	if (__same_type(ret, sb->buf) && ret == (char *)sb->buf)
		ret = strdup((char *)ret);

	return ret;
}

#define PW(sb) ((struct pathwalk *)((void *)sb + sizeof(*sb)))

void sb_pth_init(struct strbuf **__sb, const xchar *name)
{
	struct strbuf *sb;
	struct pathwalk *pw;

	uint size = sizeof(*sb) + sizeof(*pw);

	sb = xmalloc(size);
	pw = (void *)sb + sizeof(*sb);

	memset(sb, 0, sizeof(*sb));

	sb_pth_push_no_sep(sb, name);

	__pw_init(pw, sb->buf, sb->len, sb->buf, 0);
	*__sb = sb;
}

void sb_pth_reinit(struct strbuf **__sb, const xchar *name)
{
	struct strbuf *sb = *__sb;
	struct pathwalk *pw = (void *)sb + sizeof(*sb);

	sb_trunc(sb, sb->len);
	sb->off.cwd = 0;

	sb_pth_push_no_sep(sb, name);

	__pw_init(pw, sb->buf, sb->len, sb->buf, 0);
}

xchar *sb_pth_detach(struct strbuf **__sb)
{
	struct strbuf *sb = *__sb;
	xchar *ret = sb->buf;

	free(sb);
	return ret;
}

void sb_pth_destroy(struct strbuf **sb)
{
	xchar *buf = sb_pth_detach(sb);

	free(buf);
}

static void __sb_pth_add_no_sep(struct strbuf *sb, const xchar *name, uint len)
{
	memcpy(&sb->buf[sb->len], name, (len + 1) * sizeof(*name));
	sb->len += len;

	__pw_init(PW(sb), sb->buf, sb->len, sb->buf, 0);
}

static void __sb_pth_add(struct strbuf *sb, const xchar *name, uint len)
{
	if (!pw_in_root(PW(sb))) {
		sb->buf[sb->len] = PTH_SEP;
		sb->len += 1;
	}

	__sb_pth_add_no_sep(sb, name, len);
}

uint sb_pth_push_cwd(struct strbuf *sb, const xchar *name)
{
	if (!sb->off.cwd) {
		sb_trunc(sb, sb->len);
		return sb_pth_push_no_sep(sb, name);
	}

	uint comp_len = xc_strlen(name);
	uint len = comp_len + 1;
	uint overlap = sb->len - sb->off.cwd;

	if (len > overlap)
		sb_grow(sb, len - overlap);

	xchar *dst = &sb->buf[sb->off.cwd + 1];

	if (!pw_in_root(PW(sb)))
		sb->buf[sb->off.cwd] = PTH_SEP;
	else
		dst = &dst[-1];

	memcpy(dst, name, (comp_len + 1) * sizeof(*name));

	if (len > overlap)
		sb->len += len - overlap;
	else
		sb->len -= overlap - len;

	__pw_init(PW(sb), sb->buf, sb->len, sb->buf, 0);
	return len;
}

#define SB_PTH_ADD(sb, name, fn)	\
({					\
	uint comp_len = xc_strlen(name);\
	uint len = comp_len + 1;	\
					\
	sb_grow(sb, len);		\
	fn(sb, name, comp_len);		\
					\
	len;				\
})

uint sb_pth_push(struct strbuf *sb, const xchar *name)
{
	return SB_PTH_ADD(sb, name, __sb_pth_add);
}

uint sb_pth_push_no_sep(struct strbuf *sb, const xchar *name)
{
	return SB_PTH_ADD(sb, name, __sb_pth_add_no_sep);
}

uint __sb_pth_join(struct strbuf *sb, ...)
{
	va_list ap;
	va_list cp;
	uint len = 0;

	va_start(ap, sb);
	va_copy(cp, ap);

	while (39) {
		const xchar *str = va_arg(ap, const xchar *);

		if (!str)
			break;

		len += xc_strlen(str) + 1;
	}

	va_end(ap);
	sb_grow(sb, len);

	while (39) {
		const xchar *str = va_arg(cp, const xchar *);

		if (!str)
			break;

		uint comp_len = xc_strlen(str);

		__sb_pth_add(sb, str, comp_len);
	}

	va_end(cp);
	return len;
}

void sb_pth_pop(struct strbuf *sb)
{
	struct pathwalk *pw = PW(sb);

	pw_to_dirname(pw);
	sb->len = pw->len;
}

int sb_pth_in_root(struct strbuf *sb)
{
	return pw_in_root(PW(sb));
}
