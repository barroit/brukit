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

#define __ERROR_RETURN_ON(x)					\
do {								\
	if (x) {						\
		__tm_error(NULL, TM_FLLN | TM_FUNC,		\
			   "\nassertion `%s' failed", #x);	\
		return;						\
	}							\
} while (0)

#define MUST_PASS(x) !(x)
#define MUST_FAIL(x) !!(x)

#define USSERT_PASS(x) __ERROR_RETURN_ON(MUST_PASS(x))
#define USSERT_FAIL(x) __ERROR_RETURN_ON(MUST_FAIL(x))

#define USSERT_NONNULL(x) __ERROR_RETURN_ON(x == 0)
#define USSERT_ZERO(x)    __ERROR_RETURN_ON(x != 0)

#define USSERT_EQUAL(a, b) __ERROR_RETURN_ON(a != b)

int __ussert_strequal(const char *file, int line, const char *func,
		      const char *expr, const xchar *__s1, const xchar *__s2);

#define __USSERT_STREQUAL(expr, s1, s2) ___USSERT_STREQUAL(expr, s1, s2)
#define ___USSERT_STREQUAL(expr, s1, s2) \
	__ussert_strequal(__FILE__, __LINE__, __func__, #expr, s1, s2)

#define USSERT_STREQUAL(s1, s2) 				\
do {								\
	if (__USSERT_STREQUAL(xc_strcmp(s1, s2), s1, s2))	\
		return;						\
} while (0)

#endif /* NG39_USSERT_H */
