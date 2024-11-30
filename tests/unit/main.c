// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <stdio.h>

#include "atexit.h"
#include "iter.h"

int main(void)
{
	uint i;
	ptrdiff_t n = __testdecl_end - __testdecl_begin - 1;
	unitest_routine_t *func = (typeof(func))__testdecl_begin + 1;

	if (__testdecl_teardown)
		atexit_push(__testdecl_teardown);

	if (__testdecl_setup)
		__testdecl_setup();

	idx_for_each(i, n)
		func[i]();

	return 0;
}
