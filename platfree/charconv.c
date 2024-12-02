// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "charconv.h"

#include <wchar.h>

#include "xalloc.h"

size_t cc_wcstombs(char **__dest, const wchar_t *__src)
{
	char *buf;
	const wchar_t *str = __src;

	mbstate_t ps;
	size_t size = wcsrtombs(NULL, &str, 0, &ps);

	if (unlikely(size == maxof(size)))
		return maxof(size);
	size += 1;

	buf = xmalloc(size);
	str = __src;

	size = wcsrtombs(buf, &str, size, &ps);
	if (unlikely(size == maxof(size))) {
		free(buf);
		return maxof(size);
	}

	*__dest = buf;
	return size - 1;
}
