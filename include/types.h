/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TYPES_H
#define NG39_TYPES_H

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned int uint;

#ifdef CONFIG_WIDE_CHAR
# define xchar wchar_t
# define C(x)  L##x
#else
# define xchar char
# define C(x)  x
#endif

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

#endif /* NG39_TYPES_H */
