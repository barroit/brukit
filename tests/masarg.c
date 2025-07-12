// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include <stdio.h>
#include <stdlib.h>

#include "iter.h"
#include "strbuf.h"
#include "xc.h"

#ifdef CONFIG_ENABLE_WCHAR
# pragma GCC diagnostic ignored "-Wmissing-prototypes"
# define main wmain
# define puts _putws
#endif

int main(int argc, const xchar **argv)
{
	int i = 1;
	struct strbuf sb = SB_INIT;

	for_each_idx_from(i, argc) {
		sb_puts(&sb, argv[i]);
		sb_putc(&sb, XC(' '));
	}

	if (sb.len) {
		sb_trunc(&sb, 1);
		puts(sb.buf);
	}

	exit(0);
}
