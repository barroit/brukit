/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * For functions in this file, behavior is undefined if the character sequence
 * is invalid.
 */

#ifndef NG39_STRING_H
#define NG39_STRING_H

#include_next <string.h>

#include <stddef.h>

#include "attr.h"
#include "types.h"

xchar *strskip(const xchar *s1, const xchar *s2) __leaf __pure;

int strskip2(const xchar *s1, const xchar *s2, const xchar **res);

xchar *__strchrnul(const xchar *s, xchar c) __leaf __pure;

size_t __mbslen(const char *s) __leaf __pure;

/*
 * Only supports UTF-8 input.
 */
wchar_t __mbtowc(const char *seq) __leaf __pure;

char *mbsws(const char *s);

wchar_t *wcsws(const wchar_t *s);

#endif /* NG39_STRING_H */
