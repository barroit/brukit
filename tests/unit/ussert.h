/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_UA_H
#define NG39_UA_H

#include <stdlib.h>

#include "attr.h"
#include "compiler.h"
#include "termas.h"
#include "xc.h"

extern uint __ut_fail_cnt;

#define UA_ERR_RET_ON(x)					\
do {								\
	if (x) {						\
		__tm_error(NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,	\
			   "\nassertion `%s' failed", #x);	\
		__ut_fail_cnt += 1;				\
		return;						\
	}							\
} while (0)

#define UA_ERR_ON(x, fmt, ...)					\
({								\
	int __err = x;						\
	if (__err) {						\
		__tm_error(NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,	\
			   "\nassertion `%s' failed" fmt,	\
			   #x, ##__VA_ARGS__);			\
	}							\
	__err;							\
})

#define IS(x)  ((x) ? 0 : 1)
#define NOT(x) ((x) ? 1 : 0)

#define MUST_PASS(x) IS(x)
#define MUST_FAIL(x) NOT(x)

#define MUST_EQ(a, b) MUST_PASS((a) == (b))
#define MUST_NE(a, b) MUST_PASS((a) != (b))

#define MUST_LT(a, b) MUST_PASS((a) < (b))
#define MUST_GT(a, b) MUST_PASS((a) > (b))

#define MUST_LE(a, b) MUST_PASS((a) <= (b))
#define MUST_GE(a, b) MUST_PASS((a) >= (b))

#define UA_PASS(x) UA_ERR_RET_ON(MUST_PASS(x))
#define UA_FAIL(x) UA_ERR_RET_ON(MUST_FAIL(x))

#define UA_ZERO(x)    UA_ERR_RET_ON(MUST_EQ(x, 0))
#define UA_NONNULL(x) UA_ERR_RET_ON(MUST_NE(x, NULL))

#define UA_IS(x)  UA_ERR_RET_ON(IS(x))
#define UA_NOT(x) UA_ERR_RET_ON(NOT(x))

#define __UA_INTCMP(__a, __b, expr) 				\
do {								\
	if (__signed_type(__a)) {				\
		long long a = __a;				\
		long long b = __b;				\
								\
		if (UA_ERR_ON(MUST_ ## expr,			\
			      "\na %lld\nb %lld", a, b)) {	\
			__ut_fail_cnt += 1;			\
			return;					\
		}						\
	} else {						\
		unsigned long long a = __a;			\
		unsigned long long b = __b;			\
								\
		if (UA_ERR_ON(MUST_ ## expr,			\
			      "\na %llu\nb %llu", a, b)) {	\
			__ut_fail_cnt += 1;			\
			return;					\
		}						\
	}							\
} while (0)

#define UA_EQ(a, b) __UA_INTCMP(a, b, EQ(a, b))
#define UA_NE(a, b) __UA_INTCMP(a, b, NE(a, b))

#define UA_LT(a, b) __UA_INTCMP(a, b, LT(a, b))
#define UA_GT(a, b) __UA_INTCMP(a, b, GT(a, b))

#define UA_LE(a, b) __UA_INTCMP(a, b, LE(a, b))
#define UA_GE(a, b) __UA_INTCMP(a, b, GE(a, b))

int __ua_streq(const char *file, int line,
	       const char *func, const char *expr,
	       const void *__s1, const void *__s2, int is_ansi);

#define ua_streq(expr, s1, s2, t) \
	__ua_streq(__FILE__, __LINE__, __func__, #expr, s1, s2, t)

#ifdef CONFIG_ENABLE_WCHAR
# define UA_STREQ UA_STREQ_WC
#else
# define UA_STREQ UA_STREQ_MB
#endif

#define UA_STREQ_MB(s1, s2) 				\
do {							\
	if (ua_streq(strcmp(s1, s2) == 0, s1, s2, 1)) {	\
		__ut_fail_cnt += 1;			\
		return;					\
	}						\
} while (0)

#define UA_STREQ_WC(s1, s2) 				\
do {							\
	if (ua_streq(wcscmp(s1, s2) == 0, s1, s2, 0)) {	\
		__ut_fail_cnt += 1;			\
		return;					\
	}						\
} while (0)

#define UA_PTREQ(p1, p2) 						\
do {									\
	if (UA_ERR_ON(MUST_EQ(p1, p2), "\na %p\nb %p", p1, p2)) {	\
		__ut_fail_cnt += 1;					\
		return;							\
	}								\
} while (0)

#endif /* NG39_UA_H */
