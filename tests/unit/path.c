// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <stdio.h>

TESTDECL_BEGIN(setup)
{
	puts("setup");
}

TESTDECL_ROUTINE(path)
{
	puts("testing...");
}

TESTDECL_END(teardown)
{
	puts("cleanup");
}
