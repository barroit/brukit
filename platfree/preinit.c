// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "init.h"

#include "udef.h"

INITCALL_PRE(udef)
{
	udef_verbose = 1;
}

INITCALL_POST(udef)
{
	udef_verbose = 0;
}
