/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_INIT_H
#define NG39_INIT_H

#include "attr.h"

#define __init_level(lv) (100 + lv)

#define INITCALL(name) INITCALL_LEVEL(name, __init_level(39))
#define INITCALL_PRE(name) INITCALL_LEVEL(name ## _pre, __init_level(01))
#define INITCALL_POST(name) INITCALL_LEVEL(name ## _post, __init_level(831))

#define __miku_init(level) __attr((__constructor__(level)))

#define INITCALL_LEVEL(name, level) __INITCALL_LEVEL(__init_ ## name, level)
#define __INITCALL_LEVEL(name, level) \
	static void __cold __miku_init(level) name(void)

#endif /* NG39_INIT_H */
