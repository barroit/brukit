/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TYPES_H
#define NG39_TYPES_H

#include <stdint.h>
#include <sys/types.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#ifndef HAVE_UINT
typedef unsigned int uint;
#endif

#ifndef HAVE_SSIZE_T
typedef long long ssize_t;
#endif

#ifdef ANSI
typedef char xchar;
#else
typedef wchar_t xchar;
#endif

#endif /* NG39_TYPES_H */
