// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <unistd.h>

#include "fiter.h"
#include "iter.h"
#include "path.h"

#if defined(_WIN32) && defined(CONFIG_ENABLE_WCHAR)
# define access _waccess
#endif

UT_BEGIN();

static void __cb(struct file *f, void *data)
{
	int err = access(f->path, F_OK);

	UA_NOT(err);
}

static int cb(struct file *f, void *data)
{
	uint cnt = __ut_fail_cnt;

	__cb(f, data);
	if (__ut_fail_cnt != cnt)
		return -1;

	return 0;
}

UT_ROUTINE(fiter)
{
	const xchar *ws = pth_prefix();
	u32 flags[] = {
		FI_LIST_DIR,
		FI__LIST_DIR_ONLY,
		FI_RECUR_DIR,
		FI__RECUR_DIR_ONLY,
		FI_NO_LNK,
		FI_NO_REG,
		FI_NO_UNK,
		FI_USE_STAT,
	};
	uint i;

	idx_for_each(i, sizeof_array(flags)) {
		int err = fiter(ws, cb, NULL, FI_RECUR_DIR);

		UA_NOT(err);
	}
}

UT_END();
