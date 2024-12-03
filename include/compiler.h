/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_COMPILER_H
#define NG39_COMPILER_H

#include <assert.h>

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define static_assert _Static_assert

#ifdef NDEBUG
# define BUG_ON    NOOP
#else
# define BUG_ON(x) assert(!(x))
#endif

#define BUILD_BUG_ON(x) static_assert(!(x), "must not be " #x)

#define BUILD_BUG_ON_ZERO(x) ((int)(sizeof(struct { int:(-!!(x)); })))

#define __same_type(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))

#define __st_array(x) BUILD_BUG_ON_ZERO(__same_type(x, &x[0]))

#define __st_pow2(x) BUILD_BUG_ON_ZERO(!(x) || ((x) % 2) != 0)

#define __st_same_type(a, b) BUILD_BUG_ON_ZERO(!__same_type(a, b))

#define unreachable __builtin_unreachable

#ifdef HAVE_BUILTIN_ALIGN_DOWN
# define align_down(m, n) __builtin_align_down(m, n)
#else
# define align_down(m, n) (__st_pow2(n) + ((m) & ~(uintmax_t)(n - 1)))
#endif

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
#define ADAP_CALL(x, ...) __ADAP_CALL(x, ARGCOF(__VA_ARGS__), ##__VA_ARGS__)
#define __ADAP_CALL(x, ...) ___ADAP_CALL(x, __VA_ARGS__)
#define ___ADAP_CALL(x, n, ...) x##n(__VA_ARGS__)

#define NOOP(...)   ADAP_CALL(NOOP_, ##__VA_ARGS__)
#define NOOP_0(...) NOOP_1(0)
#define NOOP_1(...) NOOP_2(0, __VA_ARGS__)
#define NOOP_2(...) NOOP_3(0, __VA_ARGS__)
#define NOOP_3(...) NOOP_4(0, __VA_ARGS__)
#define NOOP_4(...) NOOP_5(0, __VA_ARGS__)
#define NOOP_5(...) NOOP_6(0, __VA_ARGS__)
#define NOOP_6(_1, _2, _3, _4, _5, _6) \
	do { if ((_1) && (_2) && (_3) && (_4) && (_5) && (_6)); } while (0)

#undef __CONCAT
#define CONCAT(a, b)   __CONCAT(a, b)
#define __CONCAT(a, b) a ## b

#define __kconf_enabled_1 , 1

#define IS_ENABLED(x)     __IS_ENABLED(CONCAT(__kconf_enabled_, x), 0)
#define __IS_ENABLED(...) ____IS_ENABLED(__VA_ARGS__)
#define ____IS_ENABLED(_, x, ...) x

#define container_of(x, type, memb)				\
({								\
	BUILD_BUG_ON(!__same_type(*(x), void) &&		\
		     !__same_type(*(x), ((type *)0)->memb));	\
	(type *)((void *)(x) - __builtin_offsetof(type, memb));	\
})

#endif /* NG39_COMPILER_H */
