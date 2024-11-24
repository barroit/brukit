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
		char fake[1];
		uint32_t len = sizeof(fake);
		int err;

		err = _NSGetExecutablePath(fake, &len);
		BUG_ON(!err);

		name = xmalloc(len);
		err = _NSGetExecutablePath(name, &len);
		BUG_ON(err);
	}

	return name;
}
