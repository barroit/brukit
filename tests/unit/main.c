// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "atexit.h"
#include "iter.h"
#include "tercol.h"

#ifdef CONFIG_WIDE_CHAR
# include <unistd.h>
# include <fcntl.h>
#endif

int main(void)
{
	uint i;
	uint n = __testdecl_end - __testdecl_begin - 1;
	unitest_routine_t *func = (typeof(func))__testdecl_begin + 1;

#ifdef CONFIG_WIDE_CHAR
	/*
	 * Debug output only.
	 */
	_setmode(STDOUT_FILENO, _O_U16TEXT);
#endif

	if (__testdecl_teardown)
		atexit_push(__testdecl_teardown);

	if (__testdecl_setup)
		__testdecl_setup();

	idx_for_each(i, n)
		func[i]();

	if (!__test_failure_count)
		exit(0);

	uint percent = (n - __test_failure_count) * 100 / n;
	const char *color = percent < 40 ? __fmtcol(RED) : __fmtcol(YELLOW);

	fprintf(stderr, "%s%u%% tests passed%s (%u tests failed out of %u)\n",
		color, percent, __fmtcol(RESET), __test_failure_count, n);
	exit(128);
}

void __free(void *ptr)
{
	free(*(void **)ptr);
}
