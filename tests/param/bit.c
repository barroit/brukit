// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "partest.h"

#include "param.h"

#define SHOW_BUILD  (1U << 0)
#define SHOW_PREFIX (1U << 1)
#define SHOW_CFLAGS (1U << 2)
#define SHOW_LIBVER (1U << 3)

#define __show_mask (-1U >> (32 - 4))

#define __bias(m) ((m) << 16)
#define __mode(m) ((m) | __bias(m))

const char *usage[] = {
	"run [-a | --all] [<options...>]",
	NULL,
	NULL,
};

int all;
u32 show = __show_mask;

struct opt opt[] = {
	OPT_FLAG('a', "all", &all, N_("display all info")),
	OPT_BIT(0, "build", &show,
		__mode(SHOW_BUILD), N_("show build platform")),
	OPT_BIT(0, "prefix", &show,
		__mode(SHOW_PREFIX), N_("show install location")),
	OPT_BIT(0, "cflag", &show,
		__mode(SHOW_CFLAGS), N_("list compile flags")),
	OPT_BIT(0, "lib", &show,
		__mode(SHOW_LIBVER), N_("list library versions")),
	OPT_END(),
};

int __main(int argc, const xchar **argv)
{
	param_parse(argc, argv, usage, opt, 0);

	if (!all)
		show >>= 16;
	else
		show &= __show_mask;

	pprint_hex(show);
	return 0;
}
