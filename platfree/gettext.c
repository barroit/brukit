// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "gettext.h"

#include <errno.h>

const char *__gettext(const char *masid)
{
	int errnum = errno;
	const char *ret = gettext(masid);

	errno = errnum;
	return ret;
}
