/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TYPES_H
#define NG39_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#ifndef HAVE_UINT
typedef unsigned int uint;
#endif

#ifndef HAVE_ULONG
typedef unsigned long ulong;
#endif

#ifndef HAVE_SSIZE_T
typedef long long ssize_t;
#endif

#ifndef CONFIG_ENABLE_WCHAR
typedef char xchar;
#else
typedef wchar_t xchar;
#endif

#endif /* NG39_TYPES_H */
