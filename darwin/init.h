/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_INIT_H
#define NG39_INIT_H

#include "attr.h"

typedef void (*init_cb)(void);

struct init_data {
	int lv;
	init_cb cb;
};

#define INITCALL(name) INITCALL_LEVEL(name, 39)
#define INITCALL_PRE(name) INITCALL_LEVEL(name ## _pre, 01)
#define INITCALL_POST(name) INITCALL_LEVEL(name ## _post, 831)

#define __miku_init __used __no_asan __section("__DATA,__miku_init")

#define INITCALL_LEVEL(name, level) __INITCALL_LEVEL(__init_ ## name, level)
#define __INITCALL_LEVEL(name, level)				\
	static void name(void);					\
	static struct init_data __miku_init name ## _data = {	\
		.lv = level,					\
		.cb = name,					\
	};							\
	static void __cold name(void)

#endif /* NG39_INIT_H */
