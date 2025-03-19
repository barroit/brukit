// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <unistd.h>

#include "mkdir.h"
#include "path.h"

#if defined(CONFIG_ENABLE_WCHAR)
# define rmdir _wrmdir
#elif defined(_WIN32)
# define rmdir _rmdir
#endif

UT_BEGIN();

UT_ROUTINE(mkdirp)
{
	int err;

	err = mkdirp(XC("aaa"));
	UA_NOT(err);

	err = mkdirp(XC("aaa/bbb/bbb"));
	UA_NOT(err);

	err = mkdirp(XC("aaa////ccc/ccc"));
	UA_NOT(err);

	err = mkdirp(XC("aaa////ddd///ddd////"));
	UA_NOT(err);

	err = mkdirp(XC("aaa////eee///eee/"));
	UA_NOT(err);
}

UT_END(teardown)
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
