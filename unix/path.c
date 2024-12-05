// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"

#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

#include "strconv.h"
#include "termas.h"
#include "xalloc.h"

int pth_is_abs(const xchar *name)
{
	return name[0] == PTH_SEP;
}

xchar *pth_last_sep(const xchar *s)
{
	return strrchr(s, PTH_SEP);
}

char *delink(const char *name)
{
	struct stat st;
	int err = lstat(name, &st);

	if (unlikely(err))
		return NULL;

	ssize_t nr;
	size_t size = st.st_size ? : lgrow(64);
	char *buf;

retry:
	buf = xmalloc(size);
	nr = readlink(name, buf, size);

	if (unlikely(nr == size)) {
		free(buf);
		size = lgrow(size);
		goto retry;
	}

	if (unlikely(nr == -1)) {
		free(buf);
		return NULL;
	}

	buf[nr] = 0;
	return buf;
}

const char *pth_home(void)
{
	static char *name;

	if (unlikely(!name)) {
		uid_t pid = getuid();
		struct passwd *pwd = getpwuid(pid);

		BUG_ON(!pwd);
		name = xstrdup(pwd->pw_dir);
	}

	return name;
}
