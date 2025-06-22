/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

/*
 * https://en.wikipedia.org/wiki/UTF-8
 */

#ifndef NG39_UTF8_H
#define NG39_UTF8_H

extern unsigned char _utf8[];

enum utf8 {
	UTF8_CONT,	/* continuation byte */
	UTF8_1B,	/* 1-byte */
	UTF8_2B,	/* 2-byte lead */
	UTF8_3B,	/* 3-byte lead */
	UTF8_4B,	/* 4-byte lead */
};

#define utf8_class(c) (_utf8[(unsigned char)(c)])
#define is_utf8(c, t) (utf8_class(c) == t)

#define mb1(c) is_utf8(c, UTF8_1B)
#define mb2(c) is_utf8(c, UTF8_2B)
#define mb3(c) is_utf8(c, UTF8_3B)
#define mb4(c) is_utf8(c, UTF8_4B)
#define mbc(c) is_utf8(c, UTF8_CONT)

#endif /* NG39_UTF8_H */
