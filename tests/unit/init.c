// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "init.h"

static int int39;
static uint hex39;
static float fp39;
static xchar *str39;

INIT_ROUTINE(init39)
{
	int39 = 3939;
	hex39 = 0x3939;
	fp39 = 39.39f;
	str39 = XC("mikumiku");
}

TESTDECL_BEGIN();

TESTDECL_ROUTINE(39check)
{
	USSERT_EQUAL(int39, 3939);
	USSERT_EQUAL(hex39, 0x3939);
	USSERT_EQUAL(fp39, 39.39f);
	USSERT_STREQUAL(str39, XC("mikumiku"));
}

TESTDECL_END();
