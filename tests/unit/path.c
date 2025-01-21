// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "path.h"

TESTDECL_BEGIN();

TESTDECL_ROUTINE(pth_is_abs)
{
	USSERT_FAIL(pth_is_abs(XC(".")));
#if defined(_WIN32)
	USSERT_PASS(pth_is_abs(XC("C:\\dir")));
#elif defined(__unix__)
	USSERT_PASS(pth_is_abs("/dir"));
#endif
}

TESTDECL_ROUTINE(pth_next_sep)
{
	xchar *sep = pth_next_sep(XC("path/to/dir/exe"));
	USSERT_STREQUAL(sep, XC("/to/dir/exe"));

#if defined(_WIN32)
	sep = pth_next_sep(XC("path\\to\\dir\\exe"));
	USSERT_STREQUAL(sep, XC("\\to\\dir\\exe"));

	sep = pth_next_sep(XC("path\\to\\dir/exe"));
	USSERT_STREQUAL(sep, XC("\\to\\dir/exe"));
#endif
}

TESTDECL_ROUTINE(pth_last_sep)
{
	xchar *sep = pth_last_sep(XC("path/to/dir/exe"));
	USSERT_STREQUAL(sep, XC("/exe"));

#if defined(_WIN32)
	sep = pth_last_sep(XC("path\\to\\dir\\exe"));
	USSERT_STREQUAL(sep, XC("\\exe"));

	sep = pth_last_sep(XC("path\\to\\dir/exe"));
	USSERT_STREQUAL(sep, XC("/exe"));
#endif
}

TESTDECL_ROUTINE(pth_is_dot)
{
	USSERT_PASS(pth_is_dot(XC(".")));
	USSERT_PASS(pth_is_dot(XC("..")));
	USSERT_FAIL(pth_is_dot(XC("...")));
}

TESTDECL_ROUTINE(pth_home)
{
	const xchar *name = pth_home();
	USSERT_NONNULL(name);
}

TESTDECL_ROUTINE(pth_executable)
{
	const xchar *name = pth_executable();
	USSERT_NONNULL(name);
}

TESTDECL_ROUTINE(pth_prefix)
{
	const xchar *name = pth_prefix();
	USSERT_NONNULL(name);
}

TESTDECL_ROUTINE(pth_locale)
{
	const xchar *name = pth_locale();
	USSERT_NONNULL(name);
}

TESTDECL_ROUTINE(pth_cwd)
{
	const xchar *name = pth_cwd();
	USSERT_NONNULL(name);
}

TESTDECL_END();
