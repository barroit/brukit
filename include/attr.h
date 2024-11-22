/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef	NG39_ATTR_H
#define NG39_ATTR_H

#define __unused __attribute__((unused))

#define __noreturn __attribute__((noreturn))

#ifdef __nonnull
# undef __nonnull
#endif
#define __nonnull(...) __attribute__((nonnull(__VA_ARGS__)))

#define __pure __attribute__((pure))

#define __printf(m, n) __attribute__((format(printf, m, n))) __nonnull(m)

#define __cold __attribute__((cold))

#if __has_attribute(access)
# define __read_only(...) __attribute__((access(read_only, __VA_ARGS__)))
#else
# define __read_only(...)
#endif

#endif /* NG39_ATTR_H */
