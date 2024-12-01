/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_USSERT_H
#define NG39_USSERT_H

#include <stdlib.h>

#include "attr.h"
#include "calc.h"
#include "cconv.h"
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

#define __ERROR_PRINTF_ON(x, fmt, ...)				\
({								\
	int __err = x;						\
	if (__err) {						\
		__tm_error(NULL, TM_FLLN | TM_FUNC,		\
			   "\nassertion `%s' failed" fmt,	\
			   #x, ##__VA_ARGS__);			\
	}							\
	__err;							\
})

#define USSERT_ZERO(x) __ERROR_RETURN_ON(x != 0)

#define USSERT_EQUAL(a, b) __ERROR_RETURN_ON(a != b)

#ifdef ANSI
#define USSERT_STREQUAL(a, b) 					\
do {								\
	if (__ERROR_PRINTF_ON(xc_strcmp(a, b) != 0,		\
			      "\na: %s\nb: %s", a, b))		\
		return;						\
} while (0)
#else
#define USSERT_STREQUAL(__a, __b)				\
do {								\
	char *a = NULL;						\
	char *b = NULL;						\
	int err = 0;						\
			  					\
	conv_wcstombs(__a, &a);					\
	conv_wcstombs(__b, &b);					\
	err = __ERROR_PRINTF_ON(xc_strcmp(a, b) != 0,		\
				"\na: %s\nb: %s", a, b);	\
	free(a);						\
	free(b);						\
	if(err)							\
		return;						\
} while (0)
#endif

#endif /* NG39_USSERT_H */
