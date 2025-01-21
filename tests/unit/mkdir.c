// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"
#include "mkdir.h"

#include <unistd.h>

#include "path.h"

#if defined(CONFIG_WIDE_CHAR)
# define rmdir _wrmdir
#elif defined(_WIN32)
# define rmdir _rmdir
#endif

TESTDECL_BEGIN();

TESTDECL_ROUTINE(mkdirp)
{
	int err;

	err = mkdirp(XC("aaa"));
	USSERT_NOT(err);

	err = mkdirp(XC("aaa/bbb/bbb"));
	USSERT_NOT(err);

	err = mkdirp(XC("aaa////ccc/ccc"));
	USSERT_NOT(err);

	err = mkdirp(XC("aaa////ddd///ddd////"));
	USSERT_NOT(err);

	err = mkdirp(XC("aaa////eee///eee/"));
	USSERT_NOT(err);
}

TESTDECL_END(teardown)
{
	rmdir(XC("aaa/bbb/bbb"));
	rmdir(XC("aaa/bbb"));

	rmdir(XC("aaa/ccc/ccc"));
	rmdir(XC("aaa/ccc"));

	rmdir(XC("aaa/ddd/ddd"));
	rmdir(XC("aaa/ddd"));

	rmdir(XC("aaa/eee/eee"));
	rmdir(XC("aaa/eee"));

	rmdir(XC("aaa"));
}
