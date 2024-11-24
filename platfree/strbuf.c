// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "strbuf.h"
#include "xalloc.h"
#include "path.h"
#include "termas.h"
#include "wcmbs.h"

void sb_init_ws(struct strbuf *sb, const xchar *name)
{
	memset(sb, 0, sizeof(*sb));
	sb->off.ws = sb_puts(sb, name);
}

void sb_destroy(struct strbuf *sb)
{
	free(sb->buf);
}

xchar *sb_detach(struct strbuf *sb)
{
	xchar *ret = sb->buf;
	uint unused = align_down(sb->cap - sb->len, 8);

	if (unused >= 64)
		ret = xrealloc(sb->buf, sb->len);

	sb->len = -1;
	return ret;
}

/*
 * Ensure strbuf has at least sb->len + new spaces (excluding null terminator)
 * to store characters.
 */
static inline void sb_grow(struct strbuf *sb, uint new)
{
	REALLOCBUF(sb->buf, sb->len + new + 1, sb->cap);
}

static inline uint sb_avail(struct strbuf *sb)
{
	return sb->cap - sb->len - 1;
}

uint sb_puts_at(struct strbuf *sb, uint off, const xchar *s)
{
	uint len = xstrlen(s);
	uint overlap = sb->len - off;

	if (len > overlap)
		sb_grow(sb, len - overlap);

	memcpy(&sb->buf[off], s, len + 1);
	if (len > overlap)
		sb->len += len - overlap;
	else
		sb->len -= overlap - len;

	return len;
}

#ifdef CONFIG_STRBUF_SANITIZE_PATH
static void sb_sanitize_pth_sep(struct strbuf *sb)
{
	const xchar *usep = xstrrchr(sb->buf, PTH_SEP_UNIX);
	const xchar *wsep = xstrrchr(sb->buf, PTH_SEP_WIN32);

	int mixed = usep && wsep;
	int suffixed = (usep && usep[1] == 0) || (wsep && wsep[1]);

	if (likely(!mixed && !suffixed))
		return;

	char *path;
#ifdef UNICODE
	size_t len = wcs_to_mbs(sb->buf, &path);

	if (len == maxof(len)){
		warn("sb_sanitize_pth_sep() mixed: %d, suffixed: %d",
		     mixed, suffixed);
		return;
	}
#else
	path = sb->buf;
#endif

	if (mixed)
		warn("path '%s' is mixing separators", path);
	else
		warn("path '%s' has trailing separator", path);

#ifdef UNICODE
	free(path);
#endif
}
#else
#define sb_sanitize_pth_sep NOOP
#endif

uint sb_pth_append(struct strbuf *sb, const xchar *name)
{
	uint len = xstrlen(name);
	uint ret = len + 1;

	sb_grow(sb, ret);

	sb->buf[sb->len] = C(PTH_SEP);
	sb->len += 1;

	memcpy(&sb->buf[sb->len], name, len + 1);
	sb->len += len;

	/* sanitize this fucking path */
	sb_sanitize_pth_sep(sb);
	return ret;
}

uint sb_printf_at(struct strbuf *sb, uint off, const xchar *fmt, ...)
{
	int nr;
	uint avail;
	uint i = 0;
	va_list ap[2];

	va_start(ap[0], fmt);
	va_copy(ap[1], ap[0]);
	sb_grow(sb, 42);

retry:
	avail = sb_avail(sb);
	nr = xvsnprintf(&sb->buf[off], avail + 1, fmt, ap[i]);

	BUG_ON(nr < 0);
	if (nr > avail) {
		BUG_ON(i);
		sb_grow(sb, nr);
		i += 1;
		goto retry;
	}

	va_end(ap[0]);
	va_end(ap[1]);

	sb->len += nr;
	return nr;
}

void sb_trim(struct strbuf *sb)
{
	xchar *h = sb->buf;
	while (xisspace(*h))
		h++;

	if (*h == 0) {
		sb->buf[0] = 0;
		sb->len = 0;
		return;
	}

	xchar *t = &sb->buf[sb->len];
	while (xisspace(*(t - 1)))
		t--;

	if (h == sb->buf && t == &sb->buf[sb->len])
		return;

	sb->len = t - h;
	memmove(sb->buf, h, sb->len);
	sb->buf[sb->len] = 0;
}

void sb_pth_to_dirname(struct strbuf *sb)
{
	xchar *sep = pth_last_sep(sb->buf);

	sb->len -= sep - sb->buf;
	sb->buf[sb->len] = 0;
}
