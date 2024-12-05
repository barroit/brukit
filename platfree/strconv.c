// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "strconv.h"

#include <wchar.h>

#include "compiler.h"
#include "xalloc.h"

size_t mb_wcstombs(char **__dest, const wchar_t *__src)
{
	char *buf;
	const wchar_t *str = __src;

	mbstate_t ps;
	size_t size = wcsrtombs(NULL, &str, 0, &ps);

	if (unlikely(size == maxof(size)))
		return size;
	size += 1;

	buf = xmalloc(size);
	str = __src;

	size = wcsrtombs(buf, &str, size, &ps);
	if (unlikely(size == maxof(size))) {
		free(buf);
		return size;
	}

	*__dest = buf;
	return size;
}

size_t mb_wcstombs_fb(char **dest, const wchar_t *src, const char *fb)
{
	char *buf;
	size_t len = mb_wcstombs(&buf, src);

	if (unlikely(len == maxof(len)))
		buf = (char *)fb;

	*dest = buf;
	return len;
}

size_t wc_mbstowcs(wchar_t **__dest, const char *__src)
{
	wchar_t *buf;
	const char *str = __src;

	mbstate_t ps;
	size_t size = mbsrtowcs(NULL, &str, 0, &ps);

	if (unlikely(size == maxof(size)))
		return size;
	size += 1;

	buf = xmalloc(size * sizeof(*buf));
	str = __src;

	size = mbsrtowcs(buf, &str, size, &ps);
	if (unlikely(size == maxof(size))) {
		free(buf);
		return size;
	}

	*__dest = buf;
	return size;
}

size_t wc_mbstowcs_fb(wchar_t **dest, const char *src, const wchar_t *fb)
{
	wchar_t *buf;
	size_t len = wc_mbstowcs(&buf, src);

	if (unlikely(len == maxof(len)))
		buf = (wchar_t *)fb;

	*dest = buf;
	return len;
}
