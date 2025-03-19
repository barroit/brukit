// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "path.h"

UT_BEGIN();

UT_ROUTINE(pth_is_abs)
{
	UA_FAIL(pth_is_abs(XC(".")));
#if defined(_WIN32)
	UA_PASS(pth_is_abs(XC("C:\\dir")));
#elif defined(__unix__)
	UA_PASS(pth_is_abs("/dir"));
#endif
}

UT_ROUTINE(pth_next_sep)
{
	xchar *sep = pth_next_sep(XC("path/to/dir/exe"));
	UA_STREQ(sep, XC("/to/dir/exe"));

#if defined(_WIN32)
	sep = pth_next_sep(XC("path\\to\\dir\\exe"));
	UA_STREQ(sep, XC("\\to\\dir\\exe"));

	sep = pth_next_sep(XC("path\\to\\dir/exe"));
	UA_STREQ(sep, XC("\\to\\dir/exe"));
#endif
}

UT_ROUTINE(pth_last_sep)
{
	xchar *sep = pth_last_sep(XC("path/to/dir/exe"));
	UA_STREQ(sep, XC("/exe"));

#if defined(_WIN32)
	sep = pth_last_sep(XC("path\\to\\dir\\exe"));
	UA_STREQ(sep, XC("\\exe"));

	sep = pth_last_sep(XC("path\\to\\dir/exe"));
	UA_STREQ(sep, XC("/exe"));
#endif
}

UT_ROUTINE(pth_is_dot)
{
	UA_PASS(pth_is_dot(XC(".")));
	UA_PASS(pth_is_dot(XC("..")));
	UA_FAIL(pth_is_dot(XC("...")));
}

UT_ROUTINE(pth_home)
{
	const xchar *name = pth_home();

	UA_NONNULL(name);
}

UT_ROUTINE(pth_executable)
{
	const xchar *name = pth_executable();

	UA_NONNULL(name);
}

UT_ROUTINE(pth_prefix)
{
	const xchar *name = pth_prefix();

	UA_NONNULL(name);
}

UT_ROUTINE(pth_locale)
{
	const xchar *name = pth_locale();

	UA_NONNULL(name);
}

UT_ROUTINE(pth_cwd)
{
	const xchar *name = pth_cwd();

	UA_NONNULL(name);
}

UT_END();
