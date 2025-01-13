/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
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
# define __xalloc(name, ...) \
	 name ## _x(__FILE__, __LINE__, __func__, __VA_ARGS__)
# define xmalloc(...)       __xalloc(__malloc, __VA_ARGS__)
# define xcalloc(...)       __xalloc(__calloc, __VA_ARGS__)
# define xrealloc(...)      __xalloc(__realloc, __VA_ARGS__)
# define xreallocarray(...) __xalloc(__reallocarray, __VA_ARGS__)
# define xstrdup(...)       __xalloc(__strdup, __VA_ARGS__)
# define xwcsdup(...)       __xalloc(__wcsdup, __VA_ARGS__)
#else
# define xmalloc       malloc
# define xcalloc       calloc
# define xrealloc      realloc
# define xreallocarray reallocarray
# define xstrdup       strdup
# define xwcsdup       wcsdup
#endif

void *__malloc_x(const char *file,
		 int line, const char *func, size_t size);

void *__calloc_x(const char *file,
		 int line, const char *func, size_t nmemb, size_t size);

void *__realloc_x(const char *file,
		  int line, const char *func, void *ptr, size_t size);

void *__reallocarray_x(const char *file, int line,
		       const char *func, void *ptr, size_t nmemb, size_t size);

char *__strdup_x(const char *file,
		 int line, const char *func, const char *s);

wchar_t *__wcsdup_x(const char *file,
		    int line, const char *func, const wchar_t *s);

#define REALLOCBUF(x, new, cap)					\
do {								\
	BUILD_BUG_ON(__same_type(*(x), void) ||			\
		     __same_type(*(x), void *));		\
	if (new > cap) {					\
		cap = lgrow(cap);				\
		if (cap < new)					\
			cap = lgrow(new);			\
		x = xreallocarray(x, sizeof(*(x)), cap);	\
	}							\
} while (0)

#endif /* NG39_XALLOC_H */
