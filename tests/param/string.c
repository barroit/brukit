// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "partest.h"

#include "param.h"

const char *usage[] = {
	"run [--name=<name>]",
	NULL,
};

int __main(int argc, const char **argv)
{
	const char *name = NULL;

	struct opt opt[] = {
		OPT_STRING(0, "name", &name, "someone's name"),
		OPT_END(),
	};

	parse_param(argc, argv, usage, opt, 0);

	pprint_str(name);
	return 0;
}
