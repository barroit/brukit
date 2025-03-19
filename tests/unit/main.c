// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <stdio.h>

#include "atexit.h"
#include "iter.h"
#include "tercol.h"

#ifdef CONFIG_ENABLE_WCHAR
# include <unistd.h>
# include <fcntl.h>
#endif

int main(void)
{
	uint i;
	uint n = __ut_end - __ut_begin - 1;
	ut_cb *func = (typeof(func))__ut_begin + 1;

#ifdef CONFIG_ENABLE_WCHAR
	/*
	 * Debug output only.
	 */
	_setmode(STDOUT_FILENO, _O_U16TEXT);
#endif

	if (__ut_teardown)
		atexit_push(__ut_teardown);

	if (__ut_setup)
		__ut_setup();

	idx_for_each(i, n)
		func[i]();

	if (!__ut_fail_cnt)
		exit(0);

	uint percent = (n - __ut_fail_cnt) * 100 / n;
	const char *color = percent < 40 ? __fmtcol(RED) : __fmtcol(YELLOW);

	fprintf(stderr, "%s%u%% tests passed%s (%u tests failed out of %u)\n",
		color, percent, __fmtcol(RESET), __ut_fail_cnt, n);
	exit(128);
}

void __free(void *ptr)
{
	free(*(void **)ptr);
}
