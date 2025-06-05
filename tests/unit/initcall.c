// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "init.h"

static int int39;
static uint hex39;
static xchar *str39;

INITCALL(init39)
{
	int39 = 3939;
	hex39 = 0x3939;
	str39 = XC("mikumiku");
}

UT_BEGIN();

UT_ROUTINE(39check)
{
	UA_EQ(int39, 3939);
	UA_EQ(hex39, 0x3939);
	UA_STREQ(str39, XC("mikumiku"));
}

UT_END();
