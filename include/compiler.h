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

#define BUILD_BUG_ON_ZERO(x) ((int)(sizeof(struct { int:(-!!(x)); })))

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#define __must_be_array(x) BUILD_BUG_ON_ZERO(__same_type(x, &x[0]))

#define unreachable __builtin_unreachable

#define sizeof_array(x) (sizeof(x) / sizeof((x)[0]) + __must_be_array(x))

#define bsizeof(x) (CHAR_BIT * sizeof(x))

#define is_signed(x) ((typeof(x))-1 < 0)

/*
 * maxof() accepts any integer type, including constants, variables, and type
 * definitions.
 */
#define maxof(x) \
	((UINTMAX_MAX >> is_signed(x)) >> (bsizeof(uintmax_t) - bsizeof(x)))

/*
 * mult_is_overflow() accepts signed values, provided they are not negative.
 */
#define mult_is_overflow(a, b) ((a) && (b) > (maxof(a) / (a)))

void __noreturn __die_overflow(const char *file, int line, const char *func,
			       uintmax_t a, uintmax_t b, char op, uint size);

#define __die_mult_overflow(a, b) \
	__die_overflow(__FILE__, __LINE__, __func__, a, b, '*', sizeof(a));

#define st_mult(a, b)				\
({						\
	if (unlikely(mult_is_overflow(a, b)))	\
		__die_mult_overflow(a, b);	\
	(a) * (b);				\
})

/*
 * Calculate the number of arguments (up to 6) passed to this macro. Useful for
 * making adaptive function or macro calls based on the number of arguments.
 */
#define ARGCOF(...) __ARGCOF(, ##__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0)
#define __ARGCOF(...) ___ARGCOF(__VA_ARGS__)
#define ___ARGCOF(_1, _2, _3, _4, _5, _6, _7, x, ...) x

/*
 * Call a function or macro based on the number of arguments
 */
#define ADAP_CALL(n, ...) __ADAP_CALL(n, ARGCOF(__VA_ARGS__), ##__VA_ARGS__)
#define __ADAP_CALL(n, ...) ___ADAP_CALL(n, __VA_ARGS__)
#define ___ADAP_CALL(n, x, ...) n##x(__VA_ARGS__)

/*
 * A helper to prevent the pyramid-like definitions.
 */
#define __NOPYRAMID(n, ...) ___NOPYRAMID(n, ARGCOF(__VA_ARGS__), ##__VA_ARGS__)
#define ___NOPYRAMID(n, ...) ____NOPYRAMID(n, __VA_ARGS__)
#define ____NOPYRAMID(n, x, ...) __PYRAMID_CALL_##x(n, __VA_ARGS__)
#define __PYRAMID_CALL_0(n, ...) n(0, 0, 0, 0, 0, 0)
#define __PYRAMID_CALL_1(n, ...) n(0, 0, 0, 0, 0, __VA_ARGS__)
#define __PYRAMID_CALL_2(n, ...) n(0, 0, 0, 0, __VA_ARGS__)
#define __PYRAMID_CALL_3(n, ...) n(0, 0, 0, __VA_ARGS__)
#define __PYRAMID_CALL_4(n, ...) n(0, 0, __VA_ARGS__)
#define __PYRAMID_CALL_5(n, ...) n(0, __VA_ARGS__)

/*
 * Don't show me that unused warning!
 */
#define PSEUDO(...)   ADAP_CALL(PSEUDO_, ##__VA_ARGS__)
#define PSEUDO_0(...) __NOPYRAMID(PSEUDO_6, ##__VA_ARGS__)
#define PSEUDO_1(...) PSEUDO_0
#define PSEUDO_2(...) PSEUDO_1
#define PSEUDO_3(...) PSEUDO_2
#define PSEUDO_4(...) PSEUDO_3
#define PSEUDO_5(...) PSEUDO_4
#define PSEUDO_6(_1, _2, _3, _4, _5, _6) \
	if ((_1) && (_2) && (_3) && (_4) && (_5) && (_6))

#define NOOP(...)   ADAP_CALL(NOOP_, ##__VA_ARGS__)
#define NOOP_0(...) __NOPYRAMID(NOOP_6, ##__VA_ARGS__)
#define NOOP_1      NOOP_0
#define NOOP_2      NOOP_1
#define NOOP_3      NOOP_2
#define NOOP_4      NOOP_3
#define NOOP_5      NOOP_4
#define NOOP_6(...) do { PSEUDO_6(__VA_ARGS__); } while (0)

#endif /* NG39_COMPILER_H */
