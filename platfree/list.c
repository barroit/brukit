// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "list.h"

#include "termas.h"

void __list_add_report_invalid(const struct list_head *new,
			       const struct list_head *prev,
			       const struct list_head *next)
{
	if (new == prev || new == next)
		bug("list_add double add; new=%px, prev=%px, next=%px",
		    new, prev, next);
	else if (prev == NULL || next == NULL)
		bug("list_add corruption; prev=%px, next=%px", prev, next);
	else if (next->prev != prev)
		bug("list_add corruption; next->prev should be prev (%px), but was %px",
		    prev, next->prev);
	else if (prev->next != next)
		bug("list_add corruption; prev->next should be next (%px), but was %px",
		    next, prev->next);

	unreachable();
}

void __list_del_report_invalid(const struct list_head *entry)
{
	struct list_head *prev = entry->prev;
	struct list_head *next = entry->next;

	if (prev == NULL || next == NULL ||
	    next == LIST_POISON_1 || prev == LIST_POISON_2)
		bug("list_del corruption; prev=%px, next=%px", prev, next);
	else if (next->prev != prev)
		bug("list_del corruption; next->prev should be prev (%px), but was %px",
		    prev, next->prev);
	else if (prev->next != next)
		bug("list_del corruption; prev->next should be next (%px), but was %px",
		    next, prev->next);

	unreachable();
}
