/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_COMPILER_H
#define NG39_COMPILER_H

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifdef NDEBUG
# define BUG_ON(x) do { if (x); } while (0)
#else
# define BUG_ON(x) assert(!(x))
#endif

/*
 * Calculate the number of arguments (up to 6) passed to this macro. Useful for
 * making adaptive function or macro calls based on the number of arguments.
 */
#define ARGCOF(...) __ARGCOF(, ##__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)
#define __ARGCOF(...) ___ARGCOF(__VA_ARGS__)
#define ___ARGCOF(_1, _2, _3, _4, _5, _6, _7, x, ...) x

#define bsizeof(x) (CHAR_BIT * sizeof(x))

#define is_signed(x) ((typeof(x))-1 < 0)

/*
 * maxof() accepts any integer type, including constants, variables, and type
 * definitions.
 */
#define maxof(x) \
	((UINTMAX_MAX >> is_signed(x)) >> (bsizeof(uintmax_t) - bsizeof(x)))

#endif /* NG39_COMPILER_H */
