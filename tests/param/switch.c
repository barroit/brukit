// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "partest.h"

#include "param.h"

const char *usage[] = {
	"run [-f | --[no-]force] [--[no-]enable]",
	NULL,
};

int __main(int argc, const char **argv)
{
	int force = 0;
	int enable = 0;

	struct option opt[] = {
		OPT_SWITCH('f', "force", &force, "forced to do something"),
		OPT_SWITCH(0, "enable", &enable, "enable something"),
		OPT_END(),
	};

	parse_param(argc, argv, usage, opt, 0);

	pprint_int(force);
	pprint_int(enable);
	return 0;
}
