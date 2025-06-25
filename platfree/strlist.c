// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "strlist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "ascii.h"
#include "bitopt.h"
#include "compiler.h"
#include "strbuf.h"
#include "utf8.h"
#include "utf16.h"
#include "xalloc.h"
#include "xcf.h"

#define WORD_AVG_LEN 8

#define ALLOC_ITEM(n, memb)				\
({							\
	struct strentry *__item = xmalloc(n);		\
	void *__buf = (void *)__item + sizeof(*__item);	\
							\
	__item->memb = __buf;				\
	__item;						\
})

void sl_init(struct strlist *sl, u32 flags)
{
	if (popcount(sl_mode(flags)) == 0)
		flags |= SL_MODE_CP;

	sl->flags = flags;
	list_head_init(&sl->head);

	if (sl->flags & SL_MODE_SB)
		list_head_init(&sl->idle);

	sl->items = 0;
}

static void __sl_destroy(struct strlist *sl, struct list_head *head)
{
	struct strentry *item, *tmp;

	list_for_each_entry_safe(item, tmp, head, list) {
		if (sl->flags & SL_MODE_SB)
			sb_destroy(item->sb);
		free(item);
	}
}

void sl_destroy(struct strlist *sl)
{
	__sl_destroy(sl, &sl->head);

	if (sl->flags & SL_MODE_SB)
		__sl_destroy(sl, &sl->idle);
}

static inline struct strentry *peek_idle(struct strlist *sl)
{
	return list_first_entry(&sl->idle, struct strentry, list);
}

static inline struct strentry *take_idle(struct strlist *sl)
{
	struct strentry *item = peek_idle(sl);

	list_del(&item->list);
	return item;
}

static struct strentry *make_item_cp(struct strlist *sl,
				     const xchar *str, size_t len)
{
	struct strentry *item;
	size_t size = sizeof(*item);

	if (len == -1)
		len = xc_strlen(str);
	size += (len + 1) * sizeof(*str);

	item = ALLOC_ITEM(size, sc);
	BUILD_BUG_ON(alignof(*item) < alignof(str));

	memcpy(item->sc, str, len * sizeof(*str));

	item->sc[len] = 0;
	item->len = len;
	return item;
}

static struct strentry *make_item_mb(struct strlist *sl,
				     const xchar *str, size_t len)
{
	struct strentry *item;
	size_t size = sizeof(*item);

	if (len == -1)
		len = xc_strlen(str);
	size += len + 1;

	item = ALLOC_ITEM(size, __sc);
	BUILD_BUG_ON(alignof(*item) < alignof(str));

	memcpy(item->__sc, str, len);

	item->__sc[len] = 0;
	item->len = len;
	return item;
}

static struct strentry *make_item_sb(struct strlist *sl,
				     const xchar *str, size_t len)
{
	struct strentry *item;

	BUILD_BUG_ON(alignof(*item) < alignof(*item->sb));

	if (!list_is_empty(&sl->idle)) {
		item = take_idle(sl);
		sb_trunc(item->sb, item->sb->len);
	} else {
		item = ALLOC_ITEM(sizeof(*item) + sizeof(*item->sb), sb);
		sb_init(item->sb);
	}

	item->len = sb_puts_at(item->sb, item->sb->len, str, len);
	return item;
}

static struct strentry *make_item_mv(struct strlist *sl, const xchar *str)
{
	struct strentry *item;

	if (!list_is_empty(&sl->idle))
		item = take_idle(sl);
	else
		item = ALLOC_ITEM(sizeof(*item), sr);

	item->sr = str;

	if (sl->flags & SL_CALC_LEN)
		item->len = xc_strlen(str);

	return item;
}

static struct strentry *make_item(struct strlist *sl,
				  const xchar *str, size_t len)
{
	struct strentry *item;
	u32 mode = sl_mode(sl->flags);

	switch (mode) {
	case SL_MODE_CP:
		item = make_item_cp(sl, str, len);
		break;

	case SL_MODE_SB:
		item = make_item_sb(sl, str, len);
		break;

	case SL_MODE_MB:
		item = make_item_mb(sl, (char *)str, len);
		break;

	case SL_MODE_MV:
		item = make_item_mv(sl, str);
	}

	item->mode = mode;
	list_head_init(&item->list);
	return item;
}

struct strentry *__sl_push(struct strlist *sl,
			   const xchar *str, size_t len, int backward)
{
	struct strentry *item = make_item(sl, str, len);

	if (backward)
		list_add_tail(&item->list, &sl->head);
	else
		list_add(&item->list, &sl->head);

	sl->items++;
	return item;
}

struct strentry *sl_pop(struct strlist *sl)
{
	if (list_is_empty(&sl->head))
		return NULL;

	struct strentry *item = list_first_entry(&sl->head,
						 struct strentry, list);

	list_del(&item->list);
	sl->items--;
	return item;
}

const xchar *sl_str(struct strentry *item)
{
	switch (item->mode) {
	case SL_MODE_CP:
		return item->sc;

	case SL_MODE_SB:
		return item->sb->buf;

	case SL_MODE_MV:
		return (xchar *)item->sr;

	case SL_MODE_MB:
		return (xchar *)item->__sc;
	}

	trap();
}

void sl_put_idle(struct strlist *sl, struct strentry *item)
{
	switch (item->mode) {
	case SL_MODE_MV:
	case SL_MODE_SB:
		list_add(&item->list, &sl->idle);
		break;

	default:
		trap();
	}
}

void sl_free(struct strentry *item)
{
	if (item->mode & SL_MODE_SB)
		sb_destroy(item->sb);

	free(item);
}

xchar **sl_to_argv(struct strlist *sl)
{
	xchar **ret = xmalloc((sl->items + 1) * sizeof(void *));
	xchar **ptr = ret;
	struct strentry *item;

	while ((item = sl_pop(sl))) {
		const xchar *str = sl_str(item);

		if (sl->flags & SL_MODE_MB)
			*ptr++ = strdup((char *)str);
		else
			*ptr++ = xc_strdup(str);

		sl_free(item);
	}

	*ptr = NULL;
	return ret;
}

void free_argv(xchar **argv)
{
	void *ptr = argv;

	while (*argv)
		free(*argv++);

	free(ptr);
}
