/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include_next <string.h>

/*
 * For functions in this file, behavior is undefined if the character sequence
 * is invalid.
 */

#ifndef NG39_NO_STD_HDR_PATCH
#ifndef NG39_STRING_PATCH_H
#define NG39_STRING_PATCH_H

#include <stddef.h>

#include "attr.h"
#include "types.h"

xchar *strskip(const xchar *s1, const xchar *s2) __pure;

int strskip2(const xchar *s1, const xchar *s2, const xchar **res);

xchar *__strchrnul(const xchar *s, xchar c) __pure;

size_t __mbslen(const char *s) __pure;

/*
 * Only supports UTF-8 input.
 */
wchar_t __mbtowc(const char *seq) __pure;

char *mbsws(const char *s);

wchar_t *wcsws(const wchar_t *s);

#endif /* NG39_STRING_PATCH_H */
#endif /* NG39_NO_STD_HDR_PATCH */
