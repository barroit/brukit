// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "string.h"

#include <wctype.h>

#include "ascii.h"
#include "utf8.h"
#include "utf16.h"

xchar *strskip(const xchar *s1, const xchar *s2)
{
	do {
		if (!s2[0])
			return (xchar *)s1;
	} while (*s1++ == *s2++);

	return NULL;
}

int strskip2(const xchar *s1, const xchar *s2, const xchar **res)
{
	const xchar *rest = strskip(s1, s2);

	if (rest == NULL)
		return -1;

	*res = rest;
	return 0;
}

xchar *__strchrnul(const xchar *s, xchar c)
{
	while(*s && *s != c)
		s++;

	return (xchar *)s;
}

size_t __mbslen(const char *s)
{
	size_t len = 0;

	while (*s) {
		s += utf8_class(*s);
		len++;
	}

	return len;
}

wchar_t __mbtowc(const char *seq)
{
	uint shift = 6;
	uint mask = 0x1F;
	wchar_t res = 0;
	uint len = utf8_class(*seq);

	switch (len) {
	case UTF8_4B:
		res |= seq[3] & 0x3F;
		shift += 6;
		mask >>= 1;
	case UTF8_3B:
		res |= (seq[2] & 0x3F) << (shift - 6);
		shift += 6;
		mask >>= 1;
	case UTF8_2B:
		res |= ((seq[0] & mask) << shift) |
		       ((seq[1] & 0x3F) << (shift - 6));
		break;
	default:
		res = seq[0];
	}

	return res;
}

char *mbsws(const char *s)
{
	while (*s) {
		uint len = utf8_class(*s);
		wchar_t c;

		switch (len) {
		case UTF8_1B:
			if (isspace(*s))
				return (char *)s;
			break;
		default:
			c = __mbtowc(s);
			if (iswspace(c))
				return (char *)s;
		}

		s += len;
	}

	return NULL;
}

wchar_t *wcsws(const wchar_t *s)
{
	while (*s) {
		if (wch(*s))
			s += 2;
		else if (iswspace(*s))
			return (wchar_t *)s;
		else
			s += 1;
	}

	return NULL;
}
