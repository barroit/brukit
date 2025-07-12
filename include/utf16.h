/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * https://en.wikipedia.org/wiki/UTF-16
 */

#ifndef NG39_UTF16_H
#define NG39_UTF16_H

#define UTF16_HI_BASE 0xd800
#define UTF16_LO_BASE 0xdc00

#define UTF16_SURR_MASK 0xfc00

#ifdef HAVE_2BYTE_WCHAR
# define utf16_class(c) ((c) & UTF16_SURR_MASK)
# define is_utf16(c, b) (utf16_class(c) == b)
# define wch(c) is_utf16(c, UTF16_HI_BASE)
# define wcl(c) is_utf16(c, UTF16_LO_BASE)
#else
# define utf16_class(c) 0x3939
# define is_utf16(c, b) 0
# define wch(c) 0
# define wcl(c) 0
#endif

#endif /* NG39_UTF16_H */
