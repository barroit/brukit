// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "partest.h"

#include "param.h"

const char *usage[] = {
	"run [--job=<N>]",
	NULL,
};

int __main(int argc, const char **argv)
{
	uint job = 0;

	struct option opt[] = {
		OPT_NUMBER('j', "job", &job, "allow N jobs at once"),
		OPT_END(),
	};

	parse_param(argc, argv, usage, opt, 0);

	pprint_uint(job);
	return 0;
}
