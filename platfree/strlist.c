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

	memcpy(item->sc, str, (len + 1) * sizeof(*str));
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

	memcpy(item->__sc, str, len + 1);
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

static inline int need_rewind_sb(const char c)
{
	return isspace(c) || wc_termchar(c);
}

static inline int need_rewind_wc(wchar_t c)
{
	return iswspace(c) || wc_termchar(c);
}

static char *prev_word_mb(const char *seq)
{
	while (mbc(*--seq));

	return (char *)seq;
}

static char *next_word_mb(const char *seq)
{
	while (mbc(*++seq));

	return (char *)seq;
}

static inline wchar_t *prev_word_wc(const wchar_t *str)
{
	wchar_t *prev = (wchar_t *)str - 1;

	return prev - wcl(*prev);
}

static inline wchar_t *next_word_wc(const wchar_t *str)
{
	wchar_t *prev = (wchar_t *)str;

	return wch(*str) ? &prev[2] : &prev[1];
}

static const char *advance_word_mb(const char *ptr,
				   const char *tail, size_t limit)
{
	size_t cnt = 0;

	while (ptr < tail && cnt < limit) {
		wchar_t c;
		size_t len = utf8_class(*ptr);

		switch (len) {
		case UTF8_4B:
		case UTF8_3B:
			c = __mbtowc(ptr);
			cnt += 1 + wc_fullwidth(c);
			break;
		case UTF8_2B:
		case UTF8_1B:
			cnt++;
		}

		ptr += len;
	}

	return ptr;
}

static const char *rewind_word_mb(const char *ptr,
				  const char *head, size_t limit)
{
	size_t cnt = 0;
	const char *__ptr = ptr;

	while (ptr > head && cnt < limit) {
		wchar_t c;
		size_t len = utf8_class(*ptr);

		switch (len) {
		case UTF8_4B:
		case UTF8_3B:
		case UTF8_2B:
	 		c = __mbtowc(ptr);
			if (need_rewind_wc(c))
				return ptr;

			cnt += wc_fullwidth(c);
			break;
		case UTF8_1B:
			if (need_rewind_sb(*ptr))
				return ptr;
		}

		cnt += 1;
		ptr = prev_word_mb(ptr);
	}

	return __ptr;
}

static __maybe_unused void sl_read_line_mb(struct strlist *sl,
					   const char *str, size_t wrap)
{
	size_t len = strlen(str);

	const char *tail = &str[len];
	const char *prev;
	const char *next = str;

	size_t size = (wrap + 1) * 4 + 1;
	size_t __size = strlen(str) + 1;

	if (size > __size)
		size = __size;

	char *buf = xmalloc(size);

	while (next < tail) {
		next = advance_word_mb(next, tail, wrap + 1);
		prev = next;

		if (*prev) {
			prev = rewind_word_mb(prev, str, WORD_AVG_LEN);
			next = next_word_mb(prev);
		}

		while (isspace(*next))
			next++;

		size_t n = next - str;

		memcpy(buf, str, n);
		buf[n] = 0;

		wchar_t c = __mbtowc(prev);

		if (iswspace(c))
			buf[prev - str] = 0;

		sl_push_back_mb(sl, buf);
		str = next;
	}

	free(buf);
}

static const wchar_t *advance_word_wc(const wchar_t *ptr,
				      const wchar_t *tail, size_t limit)
{
	size_t cnt = 0;

	while (ptr < tail && cnt < limit) {
		if (wch(*ptr)) {
			cnt++;
			ptr++;
		} else {
			cnt += wc_fullwidth(*ptr);
		}

		cnt++;
		ptr++;
	}

	return ptr;
}

static const wchar_t *rewind_word_wc(const wchar_t *ptr,
				     const wchar_t *head, size_t limit)
{
	size_t cnt = 0;
	const wchar_t *__ptr = ptr;

	while (ptr > head && cnt < limit) {
		if (wch(*ptr)) {
			cnt++;
		} else {
			if (need_rewind_wc(*ptr))
				return ptr;
			cnt += wc_fullwidth(*ptr);
		}

		cnt++;
		ptr = prev_word_wc(ptr);
	}

	return __ptr;
}

static __maybe_unused void sl_read_line_wc(struct strlist *sl,
					   const wchar_t *str, uint wrap)
{
	size_t len = wcslen(str);

	const wchar_t *tail = &str[len];
	const wchar_t *prev;
	const wchar_t *next = str;

	size_t size = (wrap + 1 + 1) * sizeof(*str);
	wchar_t *buf = xmalloc(size);

	while (next < tail) {
		next = advance_word_wc(next, tail, wrap + 1);
		prev = next;

		if (*prev) {
			prev = rewind_word_wc(prev, str, WORD_AVG_LEN);
			next = next_word_wc(prev);
		}

		while (!wch(*next) && iswspace(*next))
			next++;

		size_t n = next - str;

		memcpy(buf, str, n * sizeof(*str));
		buf[n] = 0;

		if (iswspace(*prev))
			buf[prev - str] = 0;

		sl_push_back(sl, (xchar *)buf);
		str = next;
	}

	free(buf);
}

#ifdef CONFIG_ENABLE_WCHAR
# define __sl_read_line sl_read_line_wc
#else
# define __sl_read_line sl_read_line_mb
#endif

void sl_read_line(struct strlist *sl, const xchar *str, uint wrap)
{
	if (wrap == -1)
		wrap = CONFIG_LINE_WRAP;

	__sl_read_line(sl, str, wrap);
}

void sl_read_line_chr(struct strlist *sl, const char *str, uint wrap)
{
	if (wrap == -1)
		wrap = CONFIG_LINE_WRAP;

	sl_read_line_mb(sl, str, wrap);
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
