// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "calc.h"

#include <inttypes.h>

#include "termas.h"

void __die_overflow(const char *file, int line, const char *func,
		    uintmax_t a, uintmax_t b, char op, uint size)
{
	__termas(file, line, func,
		 TM_FATAL, NULL, MAS_SHOW_FILE | MAS_SHOW_FUNC,
		 "%" PRIuMAX " %c %" PRIuMAX " overflows in %u-byte",
		 a, op, b, size);

	unreachable();
}
