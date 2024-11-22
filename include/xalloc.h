/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_XALLOC_H
#define NG39_XALLOC_H

#ifdef CONFIG_PANIC_NOMEM
# define xmalloc(...)       __xmalloc(__FUNCTION__, __VA_ARGS__)
# define xcalloc(...)       __xcalloc(__FUNCTION__, __VA_ARGS__)
# define xrealloc(...)      __xrealloc(__FUNCTION__, __VA_ARGS__)
# define xreallocarray(...) __xreallocarray(__FUNCTION__, __VA_ARGS__)
# define xstrdup(...)       __xstrdup(__FUNCTION__, __VA_ARGS__)
#else
# define xmalloc       malloc
# define xcalloc       calloc
# define xrealloc      realloc
# define xreallocarray reallocarray
# define xstrdup       strdup
#endif

void *__xmalloc(const char *name, size_t size);
void *__xcalloc(const char *name, size_t nmemb, size_t size);
void *__xrealloc(const char *name, void *ptr, size_t size);
void *__xreallocarray(const char *name, void *ptr, size_t nmemb, size_t size);
char *__xstrdup(const char *name, const char *s);

#endif /* NG39_XALLOC_H */
