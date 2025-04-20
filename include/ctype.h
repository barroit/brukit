/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_CTYPE_H
#define NG39_CTYPE_H

#define _3C  0x01  /* Control character */
#define _3S  0x02  /* Space character */
#define _3P  0x04  /* Punctuation */
#define _3D  0x08  /* Digit */
#define _3U  0x10  /* Uppercase letter */
#define _3L  0x20  /* Lowercase letter */
#define _3X  0x40  /* Hex digit */
#define _3SP 0x80  /* Space (' ') */

/*
 * Don't play ctype.h on macos.
 */
#ifdef __APPLE__
# include_next <ctype.h>
#else

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

extern const unsigned char _ctype[];

#define __ischrtype(x) (__same_type(x, char) || __same_type(x, unsigned char))
#define __ischrrange(x) (__ischrtype(x) ? : ((unsigned char)(x) == (x)))

#define __ismask(x) _ctype[(int)(unsigned char)(x)]
#define __isctype(x, t) (__ischrrange(x) && (__ismask(x) & (t)) != 0)

#define isalpha(c)  __isctype(c, _3U|_3L)
#define isdigit(c)  __isctype(c, _3D)
#define iscntrl(c)  __isctype(c, _3C)
#define isgraph(c)  __isctype(c, _3P|_3U|_3L|_3D)
#define islower(c)  __isctype(c, _3L)
#define isprint(c)  __isctype(c, _3P|_3U|_3L|_3D|_3SP)
#define ispunct(c)  __isctype(c, _3P)
#define isspace(c)  __isctype(c, _3S)
#define isupper(c)  __isctype(c, _3U)
#define isxdigit(c) __isctype(c, _3D|_3X)

#define isascii(c) ((unsigned char)(c) <= 0x7f)
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

#endif /* bad __APPLE__ */
#endif /* NG39_CTYPE_H */
