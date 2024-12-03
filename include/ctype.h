/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_CTYPE_H
#define NG39_CTYPE_H

#include "attr.h"
#include "compiler.h"

#define _C  0x01  /* Control character */
#define _S  0x02  /* Space character */
#define _P  0x04  /* Punctuation */
#define _D  0x08  /* Digit */
#define _U  0x10  /* Uppercase letter */
#define _L  0x20  /* Lowercase letter */
#define _X  0x40  /* Hex digit */
#define _SP 0x80  /* Space (' ') */

extern const unsigned char _ctype[];

#define __ischrtype(x) (__same_type(x, char) || __same_type(x, unsigned char))
#define __ischrrange(x) (__ischrtype(x) ? : ((unsigned char)(x) == x))

#define __ismask(x) _ctype[(int)(unsigned char)(x)]
#define __isctype(x, t) (__ischrrange(x) && (__ismask(x) & (t)) != 0)

#define isalpha(c)  __isctype(c, _U|_L)
#define isdigit(c)  __isctype(c, _D)
#define iscntrl(c)  __isctype(c, _C)
#define isgraph(c)  __isctype(c, _P|_U|_L|_D)
#define islower(c)  __isctype(c, _L)
#define isprint(c)  __isctype(c, _P|_U|_L|_D|_SP)
#define ispunct(c)  __isctype(c, _P)
#define isspace(c)  __isctype(c, _S)
#define isupper(c)  __isctype(c, _U)
#define isxdigit(c) __isctype(c, _D|_X)

#define isascii(c) (!((unsigned char)(c) & 0x80))
#define toascii(c) ((unsigned char)(c) & 0x7f)

static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A' - 'a';
	return c;
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a' - 'A';
	return c;
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)

#endif /* NG39_CTYPE_H */
