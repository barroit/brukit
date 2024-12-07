// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "xalloc.h"

#include <string.h>
#include <wchar.h>

#include "size.h"
#include "tercol.h"
#include "termas.h"

#ifndef HAVE_REALLOCARRAY
void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	int overflow = mult_is_overflow(nmemb, size);

	if (unlikely(overflow)) {
		errno = ENOMEM;
		return NULL;
	}

	return realloc(ptr, nmemb * size);
}
#endif

#define ALLOC_MAX ((size_t)512 * SZ_1K)

#ifdef CONFIG_WARN_HUGE_ALLOC
static inline void warn_on_exceeds(const char *file, int line,
				   const char *func, size_t size)
{
	if (likely(size <= ALLOC_MAX))
		return;

	__termas(file, line, func,
		 TM_WARN, NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,
		 "allocation of %zu exceeds limit of %zu", size, ALLOC_MAX);
}
#else
#define warn_on_exceeds NOOP
#endif

static inline void assert_allocated(const char *file, int line,
				    const char *func, void *ptr, size_t size)
{
	if (likely(ptr))
		return;

	__termas(file, line, func,
		 TM_FATAL, NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,
		 "out of memory (tried to allocate %zu bytes)", size);
}

void *__malloc_x(const char *file,
		 int line, const char *func, size_t size)
{
	void *ptr;

	warn_on_exceeds(file, line, func, size);
	ptr = malloc(size);

	assert_allocated(file, line, func, ptr, size);
	return ptr;
}

void *__calloc_x(const char *file,
		 int line, const char *func, size_t nmemb, size_t size)
{
	void *ptr;
	size_t n = st_mult(nmemb, size);

	warn_on_exceeds(file, line, func, n);
	ptr = calloc(nmemb, size);

	assert_allocated(file, line, func, ptr, n);
	return ptr;
}

void *__realloc_x(const char *file,
		  int line, const char *func, void *ptr, size_t size)
{
	warn_on_exceeds(file, line, func, size);
	ptr = realloc(ptr, size);

	assert_allocated(file, line, func, ptr, size);
	return ptr;
}

void *__reallocarray_x(const char *file, int line,
		       const char *func, void *ptr, size_t nmemb, size_t size)
{
	size_t n = st_mult(nmemb, size);

	warn_on_exceeds(file, line, func, n);
	ptr = realloc(ptr, n);

	assert_allocated(file, line, func, ptr, n);
	return ptr;
}

char *__strdup_x(const char *file,
		 int line, const char *func, const char *s)
{
	char *ptr = strdup(s);

	if (likely(ptr))
		return ptr;

	__termas(file, line, func, TM_FATAL,
		 strerror(errno), MAS_SHOW_FILE | MAS_SHOW_FUNC,
		 "failed to duplicate string '%.10s...'", s);
	unreachable();
}

wchar_t *__wcsdup_x(const char *file,
		    int line, const char *func, const wchar_t *s)
{
	wchar_t *ptr = wcsdup(s);

	if (likely(ptr))
		return ptr;

	__termas(file, line, func,
		 TM_FATAL, strerror(errno),
		 MAS_SHOW_FILE | MAS_SHOW_FUNC,
		 "failed to duplicate wide string");
	unreachable();
}
