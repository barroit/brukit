// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"
#include "xalloc.h"

const char *pth_executable(void)
{
	static char *name;

	if (unlikely(!name)) {
		int err;
		int mib[4] = {
			[0] = CTL_KERN,
			[1] = KERN_PROC,
			[2] = KERN_PROC_PATHNAME,
			[3] = -1,
		};
		size_t len;

		err = sysctl(mib, sizeof_array(mib), NULL, &len, NULL, 0);
		BUG_ON(err);

		name = xmalloc(len);
		err = sysctl(mib, sizeof_array(mib), name, &len, NULL, 0);
		BUG_ON(err);
	}

	return name;
}
