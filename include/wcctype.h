/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * Designed to work only with 16-bit wchar_t.
 *
 * Surrogate Range
 *	High	0xD800 ... 0xDBFF   1101 1000 0000 0000 ... 1101 1011 1111 1111
 *	Low	0xDC00 ... 0xDFFF   1101 1100 0000 0000 ... 1101 1111 1111 1111
 */

#ifndef NG39_WCCTYPE_H
#define NG39_WCCTYPE_H

#define _3WH 0xD800
#define _3WL 0xDC00

#define __wccmask(c)  ((c) & 0xF800)
#define __wcspmask(c) ((c) & 0xFC00)

#define iswcsp(c)  (__wccmask(c)  == _3WH) /* is surrogates? */
#define iswcsph(c) (__wcspmask(c) == _3WH)
#define iswcspl(c) (__wcspmask(c) == _3WL)

#endif /* NG39_WCCTYPE_H */
