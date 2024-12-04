/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_CALC_H
#define NG39_CALC_H

#include <limits.h>

#include "attr.h"
#include "compiler.h"
#include "types.h"

#define sizeof_array(x) (sizeof(x) / sizeof((x)[0]) + __st_array(x))

#define bsizeof(x) (CHAR_BIT * sizeof(x))

/*
 * maxof() accepts any integer type, including constants, variables, and type
 * definitions.
 */
#define maxof(x) \
	((UINTMAX_MAX >> __signed_type(x)) >> (bsizeof(uintmax_t) - bsizeof(x)))

/*
 * mult_is_overflow() accepts signed values, provided they are not negative.
 */
#define mult_is_overflow(a, b) ((a) && (b) > (maxof(a) / (a)))

/*
 * add_is_overflow() accepts signed values, provided they are not negative.
 */
#define add_is_overflow(a, b) ((b) > (maxof(a) - (a)))

void __noreturn __die_overflow(const char *file, int line, const char *func,
			       uintmax_t a, uintmax_t b, char op, uint size);

#define st_mult(a, b)						\
({								\
	if (unlikely(mult_is_overflow(a, b)))			\
		__die_overflow(__FILE__, __LINE__,		\
			       __func__, a, b, '*', sizeof(a));	\
	(a) * (b);						\
})

#define st_add(a, b)						\
({								\
	if (unlikely(add_is_overflow(a, b)))			\
		__die_overflow(__FILE__, __LINE__,		\
			       __func__, a, b, '*', sizeof(a));	\
	(a) + (b);						\
})

/*
 * lgrow() for buffer length, sgrow() for array size
 */
#define lgrow(x) align_down(((x + 8) * 3) >> 1, 8)
#define sgrow(x) align_down(x + (x >> 3) + 6, 8)

#endif /* NG39_CALC_H */
