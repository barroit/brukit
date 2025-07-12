/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 *
 * Locale free implementation of ctype.h
 */

/*
 * https://en.wikipedia.org/wiki/ASCII
 */

#ifndef NG39_ASCII_H
#define NG39_ASCII_H

#include "compiler.h"

#define ASCII_CTRL   0x01  /* Control character */
#define ASCII_SPACE  0x02  /* Space: '\t', '\n', etc. */
#define ASCII_PUNCT  0x04  /* Punctuation */
#define ASCII_DIGIT  0x08  /* '0'–'9' */
#define ASCII_UPPER  0x10  /* 'A'–'Z' */
#define ASCII_LOWER  0x20  /* 'a'–'z' */
#define ASCII_HEX    0x40  /* Hex digit: '0'–'9', 'A'–'F', 'a'–'f' */
#define ASCII_BLANK  0x80  /* Space (' ') only */

extern const unsigned char _ascii[];

#define ascii_class(c) _ascii[(int)(unsigned char)(c)]

#define is_ascii_type(x)  (__same_type(x, char) || \
			   __same_type(x, unsigned char))
#define is_ascii_range(x) (is_ascii_type(x) ? : ((unsigned char)(x) == (x)))
#define is_ascii(c, t)    (is_ascii_range(c) && (ascii_class(c) & (t)) != 0)

#define isalpha(c)  is_ascii(c, ASCII_UPPER | ASCII_LOWER)
#define isdigit(c)  is_ascii(c, ASCII_DIGIT)
#define iscntrl(c)  is_ascii(c, ASCII_CTRL)
#define isgraph(c)  is_ascii(c, ASCII_PUNCT | \
			     ASCII_UPPER | ASCII_LOWER | ASCII_DIGIT)
#define islower(c)  is_ascii(c, ASCII_LOWER)
#define isprint(c)  is_ascii(c, ASCII_PUNCT | ASCII_UPPER | \
			     ASCII_LOWER | ASCII_DIGIT | ASCII_BLANK)
#define ispunct(c)  is_ascii(c, ASCII_PUNCT)
#define isspace(c)  is_ascii(c, ASCII_SPACE)
#define isupper(c)  is_ascii(c, ASCII_UPPER)
#define isxdigit(c) is_ascii(c, ASCII_DIGIT | ASCII_HEX)
#define isascii(c)  (!((unsigned char)(c) & 0x80))

#define toascii(c) ((unsigned char)(c) & 0x7f)
#define tolower(c) (c | 0x20)
#define toupper(c) (c & ~0x20)

#endif /* NG39_ASCII_H */
