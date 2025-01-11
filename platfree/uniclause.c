// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unicode/clause.h"

#include "calc.h"

static const wchar_t clause[] = {
	0x0021, /* EXCLAMATION MARK */
	0x002C, /* COMMA */
	0x002E, /* FULL STOP */
	0x003F, /* QUESTION MARK */
	0x2025, /* TWO DOT LEADER */
	0x2026, /* HORIZONTAL ELLIPSIS */
	0x3001, /* IDEOGRAPHIC COMMA */
	0x3002, /* IDEOGRAPHIC FULL STOP */
	0xFF01, /* FULLWIDTH EXCLAMATION MARK */
	0xFF0C, /* FULLWIDTH COMMA */
	0xFF0E, /* FULLWIDTH FULL STOP */
	0xFF1F, /* FULLWIDTH QUESTION MARK */
};

int iseoc(wchar_t c)
{
	size_t l = 0;
	size_t r = sizeof_array(clause);

	while (l < r) {
		size_t m = (l + r) >> 1;

		if (c < clause[m])
			r = m;
		else if (c > clause[m])
			l = m + 1;
		else
			return 1;
	}

	return 0;
}
