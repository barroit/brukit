// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "iter.h"
#include "list.h"
#include "xalloc.h"

#define DLSIZE 10

struct data {
	uint val;
	struct list_head list;
};

LIST_HEAD(dl);
LIST_HEAD(dr);

struct data *rl[DLSIZE];
struct data *rr[DLSIZE];

UT_BEGIN(setup)
{
	uint i;
	struct data *ld, *rd;

	idx_for_each(i, DLSIZE) {
		ld = xmalloc(sizeof(*ld));
		rd = xmalloc(sizeof(*rd));

		ld->val = i;
		rd->val = i;

		list_add(&ld->list, &dl);
		list_add_tail(&rd->list, &dr);

		rl[i] = ld;
		rr[i] = rd;
	}
}

UT_ROUTINE(for_each)
{
	uint i = DLSIZE - 1;
	struct list_head *p;

	list_for_each(p, &dl) {
		UA_PTREQ(&rl[i]->list, p);
		i -= 1;
	}

	list_for_each(p, &dr) {
		i += 1;
		UA_PTREQ(&rr[i]->list, p);
	}
}

UT_ROUTINE(for_each_entry)
{
	uint i = DLSIZE - 1;
	struct data *d;

	list_for_each_entry(d, &dl, list) {
		UA_EQ(d->val, i);
		i -= 1;
	}

	list_for_each_entry(d, &dr, list) {
		i += 1;
		UA_EQ(d->val, i);
	}
}

UT_ROUTINE(for_each_entry_safe)
{
	struct data *d, *tmp;

	list_for_each_entry_safe(d, tmp, &dl, list) {
		list_del(&d->list);
		free(d);
	}
	UA_PASS(list_is_empty(&dl));

	list_for_each_entry_safe(d, tmp, &dr, list) {
		list_del(&d->list);
		free(d);
	}
	UA_PASS(list_is_empty(&dr));
}

UT_END();
