/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TYPES_H
#define NG39_TYPES_H

#include <stdint.h>

#ifdef _WIN32
# include <basetsd.h>
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned int uint;

#ifdef CONFIG_WIDE_CHAR
# define xchar wchar_t
# define MW(x)   __MW(x)
# define __MW(x) L##x
#else
# define xchar char
# define MW(x) x
#endif

#ifdef _WIN32
typedef SSIZE_T ssize_t;
#endif

struct strbuf_offset {
	uint ws;	/* a 'working space' for path operations */
};

struct strbuf {
	xchar *buf;

	uint len;
	uint cap;

	struct strbuf_offset off;
};

#endif /* NG39_TYPES_H */
