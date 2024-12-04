/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * See linux kernel commit 1c97be677f72b3c3 for the rationale behind using
 * WRITE_ONCE().
 */

#ifndef NG39_LIST_H
#define NG39_LIST_H

#include "attr.h"
#include "barrier.h"
#include "compiler.h"

#define LIST_POISON_1 (void *)0x3901
#define LIST_POISON_2 (void *)0x3902

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define LIST_HEAD_INIT(name) { &(name), &(name) }

static inline void list_head_init(struct list_head *head)
{
	head->next = head;
	head->prev = head;
}

void __cold __noreturn __list_add_report_invalid(const struct list_head *new,
						 const struct list_head *prev,
						 const struct list_head *next);

static __always_inline void __list_assert_add_valid(const struct list_head *new,
						    const struct list_head *prev,
						    const struct list_head *next)
{
	if (IS_ENABLED(CONFIG_LIST_SANITIZE_MOD)) {
		if (likely(new != prev && new != next &&
			   next->prev == prev && prev->next == next))
			return;

		__list_add_report_invalid(new, prev, next);
	}
}

void __cold __noreturn __list_del_report_invalid(const struct list_head *entry);

static __always_inline void __list_assert_del_valid(const struct list_head *entry)
{
	if (IS_ENABLED(CONFIG_LIST_SANITIZE_MOD)) {
		struct list_head *next = entry->next;
		struct list_head *prev = entry->prev;

		if (likely(next->prev == entry && prev->next == entry))
			return;

		__list_del_report_invalid(entry);
	}
}

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	__list_assert_add_valid(new, prev, next);

	next->prev = new;
	new->next = next;
	new->prev = prev;
	WRITE_ONCE(prev->next, new);
}

static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head *entry)
{
	__list_assert_del_valid(entry);

	struct list_head *prev = entry->prev;
	struct list_head *next = entry->next;

	next->prev = prev;
	WRITE_ONCE(prev->next, next);
}

static inline void list_del(struct list_head *entry)
{
	__list_del(entry);

	entry->next = LIST_POISON_1;
	entry->prev = LIST_POISON_2;
}

static inline int list_is_first(const struct list_head *entry,
				const struct list_head *head)
{
	return entry->prev == head;
}

static inline int list_is_last(const struct list_head *entry,
			       const struct list_head *head)
{
	return entry->next == head;
}

static inline int list_is_head(const struct list_head *entry,
			       const struct list_head *head)
{
	return entry == head;
}

static inline int list_is_empty(const struct list_head *head)
{
	return READ_ONCE(head->next) == head;
}

#define list_entry(list, type, memb) \
	container_of(list, type, memb)

#define list_first_entry(head, type, memb) \
	list_entry((head)->next, type, memb)

#define list_last_entry(head, type, memb) \
	list_entry((head)->prev, type, memb)

#define list_next_entry(pos, memb) \
	list_entry((pos)->memb.next, typeof(*(pos)), memb)

#define list_for_each(pos, head) \
	for (pos = (head)->next; !list_is_head(pos, head); pos = pos->next)

#define list_entry_is_head(entry, head, memb) \
	list_is_head(&(entry)->memb, head)

#define list_for_each_entry(pos, head, memb)			\
	for (pos = list_first_entry(head, typeof(*pos), memb);	\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = list_next_entry(pos, memb))

#define list_for_each_entry_safe(pos, tmp, head, memb)		\
	for (pos = list_first_entry(head, typeof(*pos), memb),	\
		tmp = list_next_entry(pos, memb);		\
	     !list_entry_is_head(pos, head, memb);		\
	     pos = tmp, tmp = list_next_entry(tmp, memb))

#endif /* NG39_LIST_H */
