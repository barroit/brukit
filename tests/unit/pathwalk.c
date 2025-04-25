// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <string.h>

#include "pathwalk.h"
#include "xalloc.h"
#include "xcf.h"

UT_BEGIN();

UT_ROUTINE(pa_dirname_rel_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;

	pw_init(&pw, XC("relative/directory/file"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("relative/directory"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("relative/directory"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("relative/directory"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("relative"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("relative"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("relative"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("."));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("."));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("."));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("."));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("."));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("."));
}

UT_ROUTINE(pa_dirname_abs_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;

	pw_init(&pw, XC("////absolute//directory/file///"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("////absolute//directory"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("////absolute//directory"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("////absolute//directory"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("////absolute"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("////absolute"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("////absolute"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("/"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("/"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("/"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("/"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("/"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("/"));
}

UT_ROUTINE(pa_dirname_shr_rtb_rel_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;
	const xchar *path = XC("relative/directory/file///");

	size_t len = xc_strlen(path);
	size_t size = ((len + 1) * sizeof(*path)) * 2;

	xchar *ptb = xmalloc(size);
	xchar *rtb = &ptb[len + 1];

	memcpy(ptb, path, (len + 1) * sizeof(*path));
	__pw_init(&pw, ptb, len, rtb, PW_RTB_SHARE);

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("relative/directory"));

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("relative"));

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("."));

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("."));
}

UT_ROUTINE(pa_dirname_shr_rtb_abs_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;
	const xchar *path = XC("////absolute//directory/file///");

	size_t len = xc_strlen(path);
	size_t size = ((len + 1) * sizeof(*path)) * 2;

	xchar *ptb = xmalloc(size);
	xchar *rtb = &ptb[len + 1];

	memcpy(ptb, path, (len + 1) * sizeof(*path));
	__pw_init(&pw, ptb, len, rtb, PW_RTB_SHARE);

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("////absolute//directory"));

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("////absolute"));

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("/"));

	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("/"));
}

UT_ROUTINE_WIN32(pa_dirname_rel_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;

	pw_init(&pw, XC("C:relative\\directory\\\\file\\\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:relative\\directory"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:relative\\directory"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:relative\\directory"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:relative"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:relative"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:relative"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("."));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("."));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("."));
}

UT_ROUTINE_WIN32(pa_dirname_abs_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;

	pw_init(&pw, XC("C:\\\\\\\\absolute\\\\\\directory\\\\file\\\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute\\\\\\directory"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute\\\\\\directory"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute\\\\\\directory"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("C:\\"));
}

UT_ROUTINE_WIN32(pa_dirname_net_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;

	pw_init(&pw, XC("\\\\127.0.0.1\\c$\\path\\file\\\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$\\path"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$\\path"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$\\path"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
}

UT_ROUTINE_WIN32(pa_dirname_drive_path)
{
	struct pathwalk __cleanup(pw_destroy) pw;
	const xchar *dir;

	pw_init(&pw, XC("c:\\file"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("c:\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("c:\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("c:\\"));

	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("c:\\"));
	dir = pw_dirname(&pw);
	UA_STREQ(dir, XC("c:\\"));
	dir = pw_to_dirname(&pw);
	UA_STREQ(dir, XC("c:\\"));
}

UT_END();
