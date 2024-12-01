// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "cconv.h"

#include <wchar.h>

#include "compiler.h"
#include "xalloc.h"

enum mbstatus mbstatus(char c)
{
	static u8 map[] = {
		[0x00 ... 0x7F] = MB_ASCII,
		[0x80 ... 0xBF] = MB_LBT2,
		[0xC0 ... 0xDF] = MB_LBT3,
		[0xE0 ... 0xEF] = MB_LBT4,
		[0xF0 ... 0xF7] = MB_DATA,
		[0xF8 ... 0xFF] = MB_INVAL,
	};

	return map[(u8)c];
}

size_t conv_wcstombs(char **__dest, const wchar_t *__src)
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
	return size - 1;
}
