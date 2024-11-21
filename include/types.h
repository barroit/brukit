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

#ifdef CONFIG_WIDE_CHAR
# define xchar wchar_t
#else
# define xchar char
#endif

#endif /* NG39_TYPES_H */
