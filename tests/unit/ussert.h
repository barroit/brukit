/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_USSERT_H
#define NG39_USSERT_H

#include <stdlib.h>

#include "attr.h"
#include "compiler.h"
#include "termas.h"
#include "xchar.h"

extern uint __test_failure_count;

#define __ERROR_RETURN_ON(x)					\
do {								\
	if (x) {						\
		__tm_error(NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,	\
			   "\nassertion `%s' failed", #x);	\
		__test_failure_count += 1;			\
		return;						\
	}							\
} while (0)

#define __error_printf_on(x, fmt, ...)				\
({								\
	int __err = x;						\
	if (__err) {						\
		__tm_error(NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,	\
			   "\nassertion `%s' failed" fmt,	\
			   #x, ##__VA_ARGS__);			\
	}							\
	__err;							\
})

#define MUST_PASS(x) !(x)
#define MUST_FAIL(x) !!(x)

#define MUST_EQUAL(a, b) a != b
#define MUST_DIFF(a, b)  a == b

#define USSERT_PASS(x) __ERROR_RETURN_ON(MUST_PASS(x))
#define USSERT_FAIL(x) __ERROR_RETURN_ON(MUST_FAIL(x))

#define USSERT_EQUAL(a, b) __ERROR_RETURN_ON(MUST_EQUAL(a, b))
#define USSERT_DIFF(a, b)  __ERROR_RETURN_ON(MUST_DIFF(a, b))

#define USSERT_ZERO(x)    USSERT_EQUAL(x, 0)
#define USSERT_NONNULL(x) USSERT_DIFF(x, 0)

int __ussert_strequal(const char *file, int line, const char *func,
		      const char *expr, const xchar *__s1, const xchar *__s2);

#define ___ussert_strequal(expr, s1, s2) \
	____ussert_strequal(expr, s1, s2)

#define ____ussert_strequal(expr, s1, s2) \
	__ussert_strequal(__FILE__, __LINE__, __func__, #expr, s1, s2)

#define USSERT_STREQUAL(s1, s2) 				\
do {								\
	if (___ussert_strequal(xc_strcmp(s1, s2), s1, s2))	\
		return;						\
} while (0)

#define USSERT_PTREQUAL(p1, p2) 				\
do {								\
	if (__error_printf_on(MUST_EQUAL(p1, p2),		\
			      "\na %p\nb %p", p1, p2))		\
		return;						\
} while (0)

#endif /* NG39_USSERT_H */
