/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_XALLOC_H
#define NG39_XALLOC_H

#include <stdlib.h>
#include <string.h>

#include "calc.h"

#ifndef HAVE_REALLOCARRAY
void *reallocarray(void *ptr, size_t nmemb, size_t size) __warn_unused_result;
#endif

#ifdef CONFIG_PANIC_NOMEM
# define __xalloc(fn, ...)  __x##fn(__FILE__, __LINE__, __func__, __VA_ARGS__)
# define xmalloc(...)       __xalloc(malloc, __VA_ARGS__)
# define xcalloc(...)       __xalloc(calloc, __VA_ARGS__)
# define xrealloc(...)      __xalloc(realloc, __VA_ARGS__)
# define xreallocarray(...) __xalloc(reallocarray, __VA_ARGS__)
# define xstrdup(...)       __xalloc(strdup, __VA_ARGS__)
# define xwcsdup(...)       __xalloc(wcsdup, __VA_ARGS__)
#else
# define xmalloc       malloc
# define xcalloc       calloc
# define xrealloc      realloc
# define xreallocarray reallocarray
# define xstrdup       strdup
#endif

void *__xmalloc(const char *file,
		int line, const char *func, size_t size);

void *__xcalloc(const char *file,
		int line, const char *func, size_t nmemb, size_t size);

void *__xrealloc(const char *file,
		 int line, const char *func, void *ptr, size_t size);

void *__xreallocarray(const char *file, int line,
		      const char *func, void *ptr, size_t nmemb, size_t size);

char *__xstrdup(const char *file,
		int line, const char *func, const char *s);

wchar_t *__xwcsdup(const char *file,
		   int line, const char *func, const wchar_t *s);

#define REALLOCBUF(x, new, cap)					\
do {								\
	BUILD_BUG_ON(type_is_same(*(x), void) ||		\
		     type_is_same(*(x), void *));		\
	if (new > cap) {					\
		cap = lgrow(cap);				\
		if (cap < new)					\
			cap = new;				\
		x = xreallocarray(x, sizeof(*(x)), cap);	\
	}							\
} while (0)

#endif /* NG39_XALLOC_H */
