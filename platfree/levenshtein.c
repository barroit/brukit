// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "levenshtein.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "calc.h"
#include "iter.h"
#include "xalloc.h"
#include "xcf.h"

uint levenshtein(const xchar *s1, const xchar *s2, const struct lev_weight *wt)
{
	size_t len1 = xc_strlen(s1);
	size_t len2 = xc_strlen(s2);

	size_t size = st_mult(len2 + 1, sizeof(uint));
	uint *buf = xcalloc(3, size);
	uint *row[3];

	row[0] = &buf[0];
	row[1] = &row[0][len2 + 1];
	row[2] = &row[1][len2 + 1];

	uint i;

	for_each_idx(i, len2 + 1)
		row[1][i] = i * wt->add;

	for_each_idx(i, len1) {
		uint j;

		row[2][0] = (i + 1) * wt->del;
		for_each_idx(j, len2) {
			uint del = row[1][j + 1] + wt->del;
			uint add = row[2][j] + wt->add;
			uint sub = row[1][j] + (s1[i] != s2[j]) * wt->sub;
			uint swp = -1;

			if (i && j && s1[i - 1] == s2[j] && s1[i] == s2[j - 1])
				swp = row[0][j - 1] + wt->swp;

			row[2][j + 1] = min(min(del, add), min(sub, swp));
		}

		uint *tmp = row[0];

		row[0] = row[1];
		row[1] = row[2];
		row[2] = tmp;
	}

	uint ret = row[1][len2];

	free(buf);
	return ret;
}
