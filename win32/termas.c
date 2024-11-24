// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"

#include <windows.h>

#include "calc.h"

const char *strwinerr(void)
{
	static char buf[256];
	DWORD errnum = GetLastError();
	DWORD langid = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM |
		      FORMAT_MESSAGE_IGNORE_INSERTS;

	int err = !FormatMessageA(flags, NULL, errnum,
				  langid, buf, sizeof_array(buf), NULL);
	BUG_ON(err);

	errno = errnum;
	return buf;
}
