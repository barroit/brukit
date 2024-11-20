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

/**
 * ARGCOF - calculate the number of arguments (up to 6) passed to this macro
 *
 * Returns the number of arguments (0 to 6). Useful for making adaptive
 * function or macro calls based on argument count.
 */
#define ARGCOF(...) __ARGCOF(, ##__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)
#define __ARGCOF(...) ___ARGCOF(__VA_ARGS__)
#define ___ARGCOF(_1, _2, _3, _4, _5, _6, _7, x, ...) x

/**
 * bsizeof - calculate the size of a value in bits
 * @x: value to calculate
 *
 * Evaluates to the size of the value in bits.
 */
#define bsizeof(x) (CHAR_BIT * sizeof(x))

/**
 * int_is_signed - check whether a value is signed at compile time
 * @x: value to check
 *
 * Evaluates to 1 if the value is signed, otherwise 0.
 */
#define is_signed(x) ((typeof(x))-1 < 0)

/**
 * maxof - calculate the maximum value of an integer
 * @x: integer to calculate
 *
 * Evaluates to the maximum value of the given integer.
 *
 * This macro accepts any integer type, including constants, variables, and
 * type definitions.
 */
#define maxof(x) \
	((UINTMAX_MAX >> is_signed(x)) >> (bsizeof(uintmax_t) - bsizeof(x)))

#endif /* NG39_COMPILER_H */
