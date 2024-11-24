/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef	NG39_ATTR_H
#define NG39_ATTR_H

#define __unused __attribute__((__unused__))

#define __noreturn __attribute__((__noreturn__))

#define __pure __attribute__((__pure__))

#define __printf(m, n) \
	__attribute__((__format__(__printf__, m, n), __nonnull__(m)))

#define __cold __attribute__((__cold__))

#if __has_attribute(__access__)
# define __read_only(...) \
	__attribute__((__access__(__read_only__, __VA_ARGS__)))
#else
# define __read_only(...)
#endif

#endif /* NG39_ATTR_H */
