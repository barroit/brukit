/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef	NG39_ATTR_H
#define NG39_ATTR_H

#define __unused __attribute__((__unused__))

#define __noreturn __attribute__((__noreturn__))

#define __pure __attribute__((__pure__))

#define __nonnone(...) __attribute__((__nonnull__(__VA_ARGS__)))

#ifdef ANSI
# define __printf(m, n) \
	 __attribute__((__format__(__printf__, m, n), __nonnull__(m)))
#else
# define __printf(m, n)
#endif

#define __cold __attribute__((__cold__))

#if __has_attribute(__access__)
# define __read_only(...) \
	 __attribute__((__access__(__read_only__, __VA_ARGS__)))
#else
# define __read_only(...)
#endif

#define __warn_unused_result __attribute__((__warn_unused_result__))

#define __leaf __attribute__((__leaf__))

#undef __always_inline
#define __always_inline inline __attribute__(__always_inline__)

#endif /* NG39_ATTR_H */
