// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "strbuf.h"

#include <stdarg.h>

#include "cconv.h"
#include "path.h"
#include "termas.h"
#include "xalloc.h"
#include "xchar.h"

#define DO_SB_PRINTF(__sb, __off, __fmt)			\
({								\
	va_list ap[2];						\
	va_start(ap[0], __fmt);					\
	va_copy(ap[1], ap[0]);					\
								\
	uint ret = __sb_printf_at(__sb, __off, __fmt, ap);	\
								\
	va_end(ap[0]);						\
	va_end(ap[1]);						\
								\
	ret;							\
})

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

	sb->len = -1;
	return ret;
}

void sb_trunc(struct strbuf *sb, uint len)
{
	BUG_ON(len > sb->len);

	sb->len -= len;
	sb->buf[sb->len] = 0;
}

/*
 * Ensure strbuf has at least sb->len + new spaces (excluding null terminator)
 * to store characters.
 */
static inline void sb_grow(struct strbuf *sb, uint new)
{
	REALLOCBUF(sb->buf, sb->len + new + 1, sb->cap);
}

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s)
{
	BUG_ON(off > sb->len);

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

uint sb_puts(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->len, s);
}

uint sb_puts_at_ws(struct strbuf *sb, const xchar *s)
{
	return sb_puts_at(sb, sb->off.ws, s);
}

/*
 * Tell the compiler this fmt cannot be NULL.
 */
static uint __sb_printf_at(struct strbuf *sb, uint off,
			   const xchar *fmt, va_list *ap) __nonnone(3);

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

uint sb_printf_at(struct strbuf *sb, uint off, const xchar *fmt, ...)
{
	return DO_SB_PRINTF(sb, off, fmt);
}

uint sb_printf(struct strbuf *sb, const xchar *fmt, ...)
{
	return DO_SB_PRINTF(sb, sb->len, fmt);
}

uint sb_printf_at_ws(struct strbuf *sb, const xchar *fmt, ...)
{
	return DO_SB_PRINTF(sb, sb->off.ws, fmt);
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

static void sanitize_pth_sep(struct strbuf *sb)
{
	const xchar *usep = xc_strrchr(sb->buf, PTH_SEP_UNIX);
	const xchar *wsep = xc_strrchr(sb->buf, PTH_SEP_WIN32);

	int mixed = usep && wsep;
	int suffixed = (usep && usep[1] == 0) || (wsep && wsep[1] == 0);

	if (likely(!mixed && !suffixed))
		return;

	char *path = (typeof(path))sb->buf;

	if (IS_ENABLED(CONFIG_WIDE_CHAR)) {
		size_t len = conv_wcstombs(&path, (wchar_t *)sb->buf);

		if (len == maxof(len)) {
			__tm_warn(NULL, TM_FUNC,
				  "mixed: %d, suffixed: %d", mixed, suffixed);
			return;
		}
	}

	if (mixed)
		warn("path '%s' is mixing separators", path);
	if (suffixed)
		warn("path '%s' has trailing separator", path);

	if (IS_ENABLED(CONFIG_WIDE_CHAR))
		free(path);
}

void sb_init_ws(struct strbuf *sb, const xchar *name)
{
	memset(sb, 0, sizeof(*sb));
	sb->off.ws = sb_puts(sb, name);

	if (IS_ENABLED(CONFIG_STRBUF_SANITIZE_PATH))
		sanitize_pth_sep(sb);
}

uint sb_pth_append(struct strbuf *sb, const xchar *name)
{
	uint __len = xc_strlen(name);
	uint len = __len + 1;

	sb_grow(sb, len);

	sb->buf[sb->len] = PTH_SEP;
	sb->len += 1;

	memcpy(&sb->buf[sb->len], name, (__len + 1) * sizeof(*name));
	sb->len += __len;

	/* sanitize this fucking path */
	if (IS_ENABLED(CONFIG_STRBUF_SANITIZE_PATH))
		sanitize_pth_sep(sb);
	return len;
}

uint sb_pth_append_at_ws(struct strbuf *sb, const xchar *name)
{
	uint __len = xc_strlen(name);
	uint len = __len + 1;
	uint overlap = sb->len - sb->off.ws;

	if (len > overlap)
		sb_grow(sb, len - overlap);

	sb->buf[sb->off.ws] = PTH_SEP;

	memcpy(&sb->buf[sb->off.ws + 1], name, (__len + 1) * sizeof(*name));
	if (len > overlap)
		sb->len += len - overlap;
	else
		sb->len -= overlap - len;

	return len;
}

void sb_pth_to_dirname(struct strbuf *sb)
{
	xchar *sep = pth_last_sep(sb->buf);

	sb->len = sep - sb->buf;
	sb->buf[sb->len] = 0;
}
