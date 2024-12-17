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

#define IS(x)  !(x)
#define NOT(x) x

#define MUST_PASS(x) !(x)
#define MUST_FAIL(x) !!(x)

#define MUST_EQUAL(a, b) a != b
#define MUST_DIFF(a, b)  a == b

#define MUST_LT(a, b) a >= b
#define MUST_GT(a, b) a <= b

#define MUST_LE(a, b) a > b
#define MUST_GE(a, b) a < b

#define USSERT_PASS(x) __ERROR_RETURN_ON(MUST_PASS(x))
#define USSERT_FAIL(x) __ERROR_RETURN_ON(MUST_FAIL(x))

#define USSERT_ZERO(x)    __ERROR_RETURN_ON(MUST_EQUAL(x, 0))
#define USSERT_NONNULL(x) __ERROR_RETURN_ON(MUST_DIFF(x, NULL))

#define USSERT_IS(x)  __ERROR_RETURN_ON(IS(x))
#define USSERT_NOT(x) __ERROR_RETURN_ON(NOT(x))

#define __USSERT_INTCMP(__a, __b, expr) 				\
do {									\
	if (__signed_type(__a)) {					\
		long long a = __a;					\
		long long b = __b;					\
									\
		if (__error_printf_on(MUST_ ## expr,			\
				      "\na %lld\nb %lld", a, b)) {	\
			__test_failure_count += 1;			\
			return;						\
		}							\
	} else {							\
		unsigned long long a = __a;				\
		unsigned long long b = __b;				\
									\
		if (__error_printf_on(MUST_ ## expr,			\
				      "\na %llu\nb %llu", a, b)) {	\
			__test_failure_count += 1;			\
			return;						\
		}							\
	}								\
} while (0)

#define USSERT_EQUAL(a, b) __USSERT_INTCMP(a, b, EQUAL(a, b))
#define USSERT_DIFF(a, b)  __USSERT_INTCMP(a, b, DIFF(a, b))

#define USSERT_LT(a, b) __USSERT_INTCMP(a, b, LT(a, b))
#define USSERT_GT(a, b) __USSERT_INTCMP(a, b, GT(a, b))

#define USSERT_LE(a, b) __USSERT_INTCMP(a, b, LE(a, b))
#define USSERT_GE(a, b) __USSERT_INTCMP(a, b, GE(a, b))

int __ussert_strequal(const char *file, int line,
		      const char *func, const char *expr,
		      const void *__s1, const void *__s2, int is_ansi);

#define ___ussert_strequal(expr, s1, s2, t) \
	____ussert_strequal(expr, s1, s2, t)
#define ____ussert_strequal(expr, s1, s2, t) \
	__ussert_strequal(__FILE__, __LINE__, __func__, #expr, s1, s2, t)

#ifdef CONFIG_WIDE_CHAR
# define USSERT_STREQUAL USSERT_STREQUAL_U
#else
# define USSERT_STREQUAL USSERT_STREQUAL_A
#endif

#define USSERT_STREQUAL_A(s1, s2) 					\
do {									\
	if (___ussert_strequal(strcmp(s1, s2) == 0, s1, s2, 1)) {	\
		__test_failure_count += 1;				\
		return;							\
	}								\
} while (0)

#define USSERT_STREQUAL_U(s1, s2) 					\
do {									\
	if (___ussert_strequal(wcscmp(s1, s2) == 0, s1, s2, 0)) {	\
		__test_failure_count += 1;				\
		return;							\
	}								\
} while (0)

#define USSERT_PTREQUAL(p1, p2) 				\
do {								\
	if (__error_printf_on(MUST_EQUAL(p1, p2),		\
			      "\na %p\nb %p", p1, p2)) {	\
		__test_failure_count += 1;			\
		return;						\
	}							\
} while (0)

#endif /* NG39_USSERT_H */
