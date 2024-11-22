// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "xalloc.h"
#include "termas.h"
#include "tercol.h"

#define ALLOC_MAX ((size_t)512 * SZ_1K)

#ifdef CONFIG_WARN_HUGE_ALLOC
static inline void check_size_limit(const char *name, size_t size)
{
	if (likely(size <= ALLOC_MAX))
		return;

	warn(H("%s", BOLD) "allocation of %zu exceeds limit of %zu",
	     name, size, ALLOC_MAX);
}
#else
#define check_size_limit NOOP
#endif

static inline void assert_allocated(const char *name, void *ptr, size_t size)
{
	if (likely(ptr))
		return;

	die(H("%s", BOLD) "out of memory (tried to allocate %zu bytes)",
	    name, size);
}

void *__xmalloc(const char *name, size_t size)
{
	void *ptr;

	check_size_limit(name, size);
	ptr = malloc(size);

	assert_allocated(name, ptr, size);
	return ptr;
}

void *__xcalloc(const char *name, size_t nmemb, size_t size)
{
	size_t n = st_mult(nmemb, size);
	void *ptr;

	check_size_limit(name, n);
	ptr = calloc(nmemb, size);

	assert_allocated(name, ptr, size);
	return ptr;
}

void *__xrealloc(const char *name, void *ptr, size_t size)
{
	check_size_limit(name, size);
	ptr = realloc(ptr, size);

	assert_allocated(name, ptr, size);
	return ptr;
}

void *__xreallocarray(const char *name, void *ptr, size_t nmemb, size_t size)
{
	size_t n = st_mult(nmemb, size);

	check_size_limit(name, n);
	ptr = realloc(ptr, n);

	assert_allocated(name, ptr, n);
	return ptr;
}

char *__xstrdup(const char *name, const char *s)
{
	char *ptr = strdup(s);

	if (likely(ptr))
		return ptr;

	die_errno(H("%s", BOLD) "failed to duplicate string \"%.10s...\"",
		  name, s);
}
