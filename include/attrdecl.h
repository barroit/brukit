/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef	NG39_ATTRDECL_H
#define NG39_ATTRDECL_H

#define __unused __attribute__((unused))

#define __argunused(name) name __unused

#define __noreturn __attribute__((noreturn))

#define __nonnull(...) __attribute__((nonnull(__VA_ARGS__)))

#define __pure __attribute__((pure))

#define __printf(m, n) __attribute__((format(printf, m, n))) __nonnull(m)

#define __cold __attribute__((cold))

#endif /* NG39_ATTRDECL_H */
