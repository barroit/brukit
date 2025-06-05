// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "atexit.h"

#include "attr.h"
#include "init.h"
#include "list.h"
#include "xalloc.h"

struct atexit_chain {
	atexit_callback_t func;
	struct list_head list;
};

static LIST_HEAD(chain);

void atexit_push(atexit_callback_t func)
{
	struct atexit_chain *c = xmalloc(sizeof(*c));

	c->func = func;
	list_add(&c->list, &chain);
}

atexit_callback_t atexit_pop(void)
{
	struct atexit_chain *c = list_first_entry(&chain,
						  struct atexit_chain, list);
	atexit_callback_t ret = c->func;

	list_del(&c->list);
	free(c);
	return ret;
}

static void atexit_apply(void)
{
	atexit_callback_t func;

	while (!list_is_empty(&chain)) {
		func = atexit_pop();
		func();
	}
}

INITCALL(mount_chain)
{
	atexit(atexit_apply);
}
