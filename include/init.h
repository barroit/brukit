/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_INIT_H
#define NG39_INIT_H

#include "attr.h"

#if defined(__unix__)
# define INIT_ROUTINE(x) static void __cold __constructor __init_ ## x(void)
#elif defined(_WIN32)
# define __init_section    __section(".CRT$XCMIKU") __used
# define __init_decl(name) static __init_section void (*name ## _decl)(void)
# define __init_def(name)  static void name(void)
# define __initdecl(name) \
	 __init_def(name); __init_decl(name) = &name; __init_def(__cold name)
# define INIT_ROUTINE(x) __initdecl(__init_ ## x)
#endif

#endif /* NG39_INIT_H */
