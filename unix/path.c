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

int chr_is_sep(char c)
{
	return c == PTH_SEP_UNIX;
}

int pth_is_abs(const char *name)
{
	return name[0] == PTH_SEP_UNIX;
}

char *pth_next_sep(const char *s)
{
	return strchr(s, PTH_SEP_UNIX);
}

char *pth_last_sep(const char *s)
{
	return strrchr(s, PTH_SEP_UNIX);
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

const char *pth_cwd(void)
{
	static char *name;

	if (unlikely(!name)) {
		size_t size = lgrow(32);
		char *buf = NULL;

		while (39) {
			buf = xrealloc(buf, size);

			name = getcwd(buf, size);
			if (name)
				break;

			BUG_ON(errno != ERANGE);
			size = lgrow(size);
		}
	}

	return name;
}
