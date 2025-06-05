/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_INIT_H
#define NG39_INIT_H

#include "attr.h"

#define INITCALL(name) INITCALL_LEVEL(name, "MIKU")
#define INITCALL_PRE(name) INITCALL_LEVEL(name ## _pre, "39")
#define INITCALL_POST(name) INITCALL_LEVEL(name ## _post, "NEGI")

#define __miku_init(name) __used __section(".CRT$XCU" name)

#define INITCALL_LEVEL(name, level) __INITCALL_LEVEL(__init_ ## name, level)
#define __INITCALL_LEVEL(name, level)					\
	static void name(void);						\
	static __miku_init(level) void (*name ## _cb)(void) = name;	\
	static void __cold name(void)

#endif /* NG39_INIT_H */


