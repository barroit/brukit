// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <string.h>

#include "iter.h"
#include "levenshtein.h"

struct strpair {
	const xchar *s1;
	const xchar *s2;
};

static struct strpair str[] = {
	{ XC("test"), XC("") },
	{ XC("test"), XC("test") },
	{ XC("test"), XC("tset") },
	{ XC("test"), XC("tet") },
	{ XC("test"), XC("testa") },
	{ XC("test"), XC("2....") },

	/* Words I always misspell... */
	{ XC("rebase"), XC("rebsae") },
	{ XC("log"),    XC("lgo") },
	{ XC("commit"), XC("commmit") },
	{ XC("stash"),  XC("satsh") },
	{ XC("make"),   XC("mkae") },
};

#include <stdio.h>

UT_BEGIN();

UT_ROUTINE(levenshtein1)
{
	uint i;
	uint dist;
	struct lev_weight wt = { 1, 1, 1, 1 };
	uint sample[] = { 4, 0, 1, 1, 1, 5, 1, 1, 1, 1, 1 };

	for_each_idx(i, sizeof_array(str)) {
		dist = levenshtein(str[i].s1, str[i].s2, &wt);
		UA_EQ(dist, sample[i]);
	}
}

UT_ROUTINE(levenshtein2)
{
	uint i;
	uint dist;
	struct lev_weight wt = {
		.add = 2,
		.del = 1,
		.sub = 2,
		.swp = 1,
	};
	uint sample[] = { 4, 0, 1, 1, 2, 10, 1, 1, 2, 1, 1 };

	for_each_idx(i, sizeof_array(str)) {
		dist = levenshtein(str[i].s1, str[i].s2, &wt);
		UA_EQ(dist, sample[i]);
	}
}

UT_END();
