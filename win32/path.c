// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"

#include <shlobj.h>
#include <shlwapi.h>

#include "strconv.h"
#include "termas.h"
#include "xalloc.h"
#include "xc.h"

int chr_is_sep(xchar c)
{
	return c == XC(PTH_SEP_WIN32) || c == XC(PTH_SEP_UNIX);
}

int pth_is_abs(const xchar *name)
{
	return !PathIsRelative(name);
}

xchar *pth_next_sep(const xchar *s)
{
	while (*s) {
		if (chr_is_sep(*s))
			return (xchar *)s;
		s++;
	}

	return NULL;
}

xchar *pth_last_sep(const xchar *s)
{
	const xchar *sep = NULL;

	while (*s) {
		if (chr_is_sep(*s))
			sep = s;
		s++;
	}

	return (xchar *)sep;
}

const xchar *pth_home(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		int err;
		wchar_t *buf;

		err = SHGetKnownFolderPath(&FOLDERID_Profile, 0, NULL, &buf);
		BUG_ON(err);

		if (IS_ENABLED(CONFIG_ENABLE_WCHAR)) {
			name = (typeof(name))buf;
		} else {
			size_t len = mb_wcstombs((char **)&name, buf);

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

const xchar *pth_cwd(void)
{
	static xchar *name;

	if (unlikely(!name)) {
		DWORD size = GetCurrentDirectory(0, NULL);

		name = xmalloc(size * sizeof(xchar));

		int err = !GetCurrentDirectory(size, name);

		BUG_ON(err);
	}

	return name;
}
