// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "mkdir.h"

#include <errno.h>
#include <sys/stat.h>

#include "path.h"
#include "xalloc.h"

static mode_t mkdir_mask(void)
{
	mode_t mask = umask(0);

	umask(mask);
	return ~mask;
}

int __mkdirp(char *buf)
{
	int err;
	mode_t mask = mkdir_mask();
	char *sep = pth_next_sep(buf);

	if (sep == buf) {
		while (chr_is_sep(sep[1]))
			sep++;
		sep = pth_next_sep(&sep[1]);
	}

	while (sep) {
		*sep = 0;
		err = mkdir(buf, mask);

		if (err && errno != EEXIST)
			return err;

		*sep = PTH_SEP_UNIX;
		while (chr_is_sep(sep[1]))
			sep++;

		if (!sep[1])
			return 0;
		sep = pth_next_sep(&sep[1]);
	}

	return mkdir(buf, mask);
}

int mkdirp(const char *name)
{
	char *buf = xstrdup(name);
	int ret = __mkdirp(buf);

	free(buf);
	return ret;
}
