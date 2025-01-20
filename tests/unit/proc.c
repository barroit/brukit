// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "proc.h"

#include "iter.h"

#if defined(__unix__)
# define PROG_PATH "tests/masarg"
#elif defined(_WIN32)
# define PROG_PATH "tests/masarg.exe"
#endif

TESTDECL_BEGIN();

TESTDECL_ROUTINE(proc_exec_wait)
{
	const xchar *file = XC(PROG_PATH);
	u32 flags[] = {
		0,
		PROC_RD_STDOUT,
		PROC_RD_STDERR,
		PROC_RD_STDOUT | PROC_RD_STDERR,
	};
	proc_t proc[sizeof_array(flags)];
	uint i;

	idx_for_each(i, sizeof_array(flags)) {
		int err;

		err = proc_exec(flags[i], &proc[i], file, file, NULL);
		USSERT_EQ(err, 0);
	}

	idx_for_each(i, sizeof_array(flags)) {
		int ret;
		int err;

		err = proc_wait(&proc[i], &ret);
		USSERT_EQ(err, 0);
		USSERT_EQ(ret, 0);
	}

}

TESTDECL_END();
