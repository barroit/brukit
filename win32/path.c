// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"

#include <shlobj.h>
#include <shlwapi.h>

#include "charconv.h"
#include "termas.h"
#include "xalloc.h"
#include "xchar.h"

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
		wchar_t *buf;

		err = SHGetKnownFolderPath(&FOLDERID_Profile, 0, NULL, &buf);
		BUG_ON(err);

		if (IS_ENABLED(CONFIG_WIDE_CHAR)) {
			name = (typeof(name))buf;
		} else {
			size_t len = cc_wcstombs((char **)&name, buf);

			BUG_ON(len == maxof(len));
			CoTaskMemFree(buf);
		}
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
		name = xmalloc(size * sizeof(*name));
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


