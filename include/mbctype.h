/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_MBTYPE_H
#define NG39_MBTYPE_H

extern unsigned char _39mbctype[];

enum mbctype {
	_9CB,  /* continuation bytes */
	_9A,   /* 1-byte */
	_9B,   /* 2-byte */
	_9C,   /* 3-byte */
	_9D,   /* 4-byte */
};

#define __mbctype(c) _39mbctype[(int)(unsigned char)(c)]
#define __ismbctype(c, t) (__mbctype(c) == t)

#define ismb1(c)  __ismbctype(c, _9A)
#define ismb2(c)  __ismbctype(c, _9B)
#define ismb3(c)  __ismbctype(c, _9C)
#define ismb4(c)  __ismbctype(c, _9D)
#define ismbcb(c) __ismbctype(c, _9CB)

#endif /* NG39_MBTYPE_H */
