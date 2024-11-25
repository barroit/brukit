// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"

#include <shlwapi.h>
#include <userenv.h>

#include "calc.h"
#include "compiler.h"
#include "mwstr.h"
#include "termas.h"
#include "xalloc.h"

int pth_is_abs(const xchar *name)
{
	return !PathIsRelative(name);
}

xchar *pth_last_sep(const xchar *s)
{
	return xc_strrchr(s, PTH_SEP_WIN32) ? : xc_strrchr(s, PTH_SEP_UNIX);
}

const xchar *pth_home(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		int err;
		DWORD size;

		GetProfilesDirectory(NULL, &size);
		name = xmalloc(size);

		err = !GetProfilesDirectory((xchar *)name, &size);
		BUG_ON(err);
	}

	return name;
}

const xchar *pth_executable(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		DWORD size = lgrow(64);
		DWORD ret;

retry:
		name = xmalloc(size);
		ret = GetModuleFileName(NULL, name, size);

		BUG_ON(ret == 0);
		if (ret == size) {
			free(name);
			size = lgrow(size);
			goto retry;
		}
	}

	return name;
}


