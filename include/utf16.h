/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * https://en.wikipedia.org/wiki/UTF-16
 *
 * Only supports 2-byte wchar_t.
 */

#ifndef NG39_UTF16_H
#define NG39_UTF16_H

#include <stddef.h>

#define UTF16_HI_BASE 0xD800
#define UTF16_LO_BASE 0xDC00

#define UTF16_SURR_MASK 0xFC00

#define utf16_class(c) ((c) & UTF16_SURR_MASK)
#define is_utf16(c, b) (utf16_class(c) == b)

#define wch(c) is_utf16(c, UTF16_HI_BASE)
#define wcl(c) is_utf16(c, UTF16_LO_BASE)

int wc_termchar(wchar_t c);

int wc_fullwidth(wchar_t c);

#endif /* NG39_UTF16_H */
