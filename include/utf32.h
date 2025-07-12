/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * https://en.wikipedia.org/wiki/UTF-32
 *
 * Not really for handling utf-32, just named to match utf8.h and utf16.h.
 */

#ifndef NG39_UTF32_H
#define NG39_UTF32_H

#include <stddef.h>

int wc_termchar(wchar_t c);

int wc_fullwidth(wchar_t c);

#endif /* NG39_UTF32_H */
