// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <string.h>

#include "pathalt.h"
#include "xalloc.h"
#include "xcf.h"

UT_BEGIN();

UT_ROUTINE(pa_dirname_rel_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	pa_init(&pa, XC("relative/directory/file"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("relative/directory"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("relative/directory"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("relative/directory"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("relative"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("relative"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("relative"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("."));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("."));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("."));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("."));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("."));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("."));
}

UT_ROUTINE(pa_dirname_abs_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	pa_init(&pa, XC("////absolute//directory/file///"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("////absolute//directory"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("////absolute//directory"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("////absolute//directory"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("////absolute"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("////absolute"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("////absolute"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("/"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("/"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("/"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("/"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("/"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("/"));
}

UT_ROUTINE(pa_dirname_shr_rtb_rel_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	const xchar *path = XC("relative/directory/file///");
	size_t len = xc_strlen(path);

	size_t rsz = (len + 1) * sizeof(path);
	size_t bsz = (len + 2) * sizeof(path);
	xchar *buf = xmalloc(bsz);

	buf[0] = 0;
	memcpy(&buf[1], path, rsz);
	__pa_init(&pa, &buf[1], len, &buf[1]);

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("relative/directory"));

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("relative"));

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("."));

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("."));
}

UT_ROUTINE(pa_dirname_shr_rtb_abs_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	const xchar *path = XC("////absolute//directory/file///");
	size_t len = xc_strlen(path);

	size_t rsz = (len + 1) * sizeof(path);
	size_t bsz = (len + 2) * sizeof(path);
	xchar *buf = xmalloc(bsz);

	buf[0] = 0;
	memcpy(&buf[1], path, rsz);
	__pa_init(&pa, &buf[1], len, &buf[1]);

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("////absolute//directory"));

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("////absolute"));

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("/"));

	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("/"));
}

UT_ROUTINE_WIN32(pa_dirname_rel_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	pa_init(&pa, XC("C:relative\\directory\\\\file\\\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:relative\\directory"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:relative\\directory"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:relative\\directory"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:relative"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:relative"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:relative"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("."));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("."));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("."));
}

UT_ROUTINE_WIN32(pa_dirname_abs_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	pa_init(&pa, XC("C:\\\\\\\\absolute\\\\\\directory\\\\file\\\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute\\\\\\directory"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute\\\\\\directory"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute\\\\\\directory"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:\\\\\\\\absolute"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("C:\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("C:\\"));
}

UT_ROUTINE_WIN32(pa_dirname_net_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	pa_init(&pa, XC("\\\\127.0.0.1\\c$\\path\\file\\\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$\\path"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$\\path"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$\\path"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\c$"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("\\\\127.0.0.1\\"));
}

UT_ROUTINE_WIN32(pa_dirname_drive_path)
{
	struct pathalt __cleanup(pa_destroy) pa;
	const xchar *dir;

	pa_init(&pa, XC("c:\\file"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("c:\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("c:\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("c:\\"));

	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("c:\\"));
	dir = pa_dirname(&pa);
	UA_STREQ(dir, XC("c:\\"));
	dir = pa_to_parent(&pa);
	UA_STREQ(dir, XC("c:\\"));
}

UT_END();
