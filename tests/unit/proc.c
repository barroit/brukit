// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "iter.h"
#include "proc.h"

#if defined(__unix__)
# define PROG_PATH "tests/masarg"
#elif defined(_WIN32)
# define PROG_PATH "tests/masarg.exe"
#endif

UT_BEGIN();

UT_ROUTINE(proc_exec_wait)
{
	const xchar *file = XC(PROG_PATH);
	u32 flags[] = {
		0,
		PROC_REDIR_OUT,
		PROC_REDIR_ERR,
		PROC_REDIR_OUT | PROC_REDIR_ERR,
	};
	proc_t proc[sizeof_array(flags)];
	uint i;

	for_each_idx(i, sizeof_array(flags)) {
		int err;

		err = proc_exec(flags[i], &proc[i], file, file, NULL);
		UA_EQ(err, 0);
	}

	for_each_idx(i, sizeof_array(flags)) {
		int ret;
		int err;

		err = proc_wait(&proc[i], &ret);
		UA_EQ(err, 0);
		UA_EQ(ret, 0);
	}

}

UT_END();
