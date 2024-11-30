/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_CONSTRUCTOR_H
#define NG39_CONSTRUCTOR_H

#include "attr.h"

#if defined(__unix__)
# define PROG_CTOR(x)	static void __cold __constructor x(void)
#elif defined(_WIN32)
# define __secalloc	__declspec(allocate(".CRT$XCMIKU"))
# define __secdecl(x)	__secalloc static __used void (*__ctor_ ## x)(void)
# define __ctordecl(x)	static void x(void)
# define __PROG_CTOR(x)	static void __cold x(void)
# define PROG_CTOR(x)	__ctordecl(x); __secdecl(x) = &x; __PROG_CTOR(x)
#endif

#endif /* NG39_CONSTRUCTOR_H */
