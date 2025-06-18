// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "strlist.h"

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#include "bitopt.h"
#include "compiler.h"
#include "mbctype.h"
#include "strbuf.h"
#include "unicode.h"
#include "wcctype.h"
#include "xalloc.h"
#include "xcf.h"

#define __sl_mode_mask (-1U << 28)
#define __sl_mode(f)   ((f) & __sl_mode_mask)

#define WORD_AVG_LEN 8

void sl_init(struct strlist *sl, u32 flags)
{
	if (popcount(__sl_mode(flags)) == 0)
		flags |= SL_STORE_COPY;
	if (flags & (SL_STORE_COPY | SL_STORE_CHR))
		flags |= SL_DUP_ON_POP;

	BUG_ON(popcount(__sl_mode(flags)) > 1);
	BUG_ON(flags & SL_CALC_SRLEN &&
	       (flags & (__sl_mode(flags) ^ SL_STORE_REF)));

	sl->flags = flags;
	list_head_init(&sl->head);

	if (sl->flags & SL_STORE_SBUF)
		list_head_init(&sl->idle);

	sl->items = 0;
}

static void __sl_destroy(struct strlist *sl, struct list_head *head)
{
	struct strlist_item *item, *tmp;

	list_for_each_entry_safe(item, tmp, head, list) {
		if (sl->flags & SL_STORE_SBUF)
			sb_destroy(item->sb);
		free(item);
	}
}

void sl_destroy(struct strlist *sl)
{
	__sl_destroy(sl, &sl->head);
	if (sl->flags & SL_STORE_SBUF)
		__sl_destroy(sl, &sl->idle);
}

/*
 * FIXME: rework this.
 */
size_t __sl_push(struct strlist *sl, const xchar *str, int is_que)
{
	size_t ret = 0;
	struct strlist_item *item = NULL;

	if (sl->flags & SL_STORE_SBUF && !list_is_empty(&sl->idle)) {
		item = list_first_entry(&sl->idle, struct strlist_item, list);
		sb_trunc(item->sb, item->sb->len);
		list_del(&item->list);
	}

	if (!item) {
		size_t n = sizeof(*item);
		void *buf;

		switch (__sl_mode(sl->flags)) {
		case SL_STORE_COPY:
			ret = xc_strlen(str);
			n += (ret + 1) * sizeof(*str);
			break;
		case SL_STORE_SBUF:
			n += sizeof(*item->sb);
			break;
		case SL_STORE_CHR:
			ret = strlen((char *)str);
			n += ret + 1;
		}

		item = xmalloc(n);
		buf = (void *)item + sizeof(*item);
		((char *)item)[n - 1] = 0;

		switch (__sl_mode(sl->flags)) {
		case SL_STORE_COPY:
			item->sc = buf;
			BUILD_BUG_ON(alignof(*item) < alignof(str));
			break;
		case SL_STORE_SBUF:
			item->sb = buf;
			sb_init(item->sb);
			BUILD_BUG_ON(alignof(*item) < alignof(*item->sb));
			break;
		case SL_STORE_CHR:
			item->__sc = buf;
			BUILD_BUG_ON(alignof(*item) < alignof((char *)str));
		}

		list_head_init(&item->list);
	}

	switch (__sl_mode(sl->flags)) {
	case SL_STORE_COPY:
		memcpy(item->sc, str, (ret + 1) * sizeof(*str));
		break;
	case SL_STORE_SBUF:
		ret = sb_puts(item->sb, str);
		break;
	case SL_STORE_REF:
		item->sr = str;
		if (sl->flags & SL_CALC_SRLEN)
			ret = xc_strlen(str);
		break;
	case SL_STORE_CHR:
		memcpy(item->__sc, (char *)str, ret + 1);
	}

	if (is_que)
		list_add_tail(&item->list, &sl->head);
	else
		list_add(&item->list, &sl->head);

	sl->items++;
	return ret;
}

xchar *sl_pop(struct strlist *sl)
{
	if (list_is_empty(&sl->head))
		return NULL;

	xchar *ret = NULL;
	struct strlist_item *item = list_first_entry(&sl->head,
						     typeof(*item), list);

	list_del(&item->list);

	switch (__sl_mode(sl->flags)) {
	case SL_STORE_COPY:
		ret = item->sc;
		break;
	case SL_STORE_SBUF:
		ret = item->sb->buf;
		break;
	case SL_STORE_REF:
		ret = (xchar *)item->sr;
		break;
	case SL_STORE_CHR:
		ret = (xchar *)item->__sc;
	}

	if (sl->flags & SL_STORE_CHR)
		ret = (xchar *)strdup((char *)ret);
	else if (sl->flags & SL_DUP_ON_POP)
		ret = xc_xstrdup(ret);

	if (sl->flags & SL_STORE_SBUF)
		list_add(&item->list, &sl->idle);
	else
		free(item);

	sl->items--;
	return ret;
}

static inline int need_rewind_sb(const char c)
{
	return isspace(c) || iseoc(c);
}

static inline int need_rewind_wc(wchar_t c)
{
	return iswspace(c) || iseoc(c);
}

static char *prev_word_mb(const char *seq)
{
	while (ismbcb(*--seq));

	return (char *)seq;
}

static char *next_word_mb(const char *seq)
{
	while (ismbcb(*++seq));

	return (char *)seq;
}

static inline wchar_t *prev_word_wc(const wchar_t *str)
{
	wchar_t *prev = (wchar_t *)str - 1;

	return prev - iswcspl(*prev);
}

static inline wchar_t *next_word_wc(const wchar_t *str)
{
	wchar_t *prev = (wchar_t *)str;

	return iswcsp(*str) ? &prev[2] : &prev[1];
}

static const char *advance_word_mb(const char *ptr,
				   const char *tail, size_t limit)
{
	size_t cnt = 0;

	while (ptr < tail && cnt < limit) {
		wchar_t c;
		size_t len = __mbctype(*ptr);

		switch (len) {
		case _9D:
		case _9C:
			c = __mbtowc(ptr);
			cnt += 1 + iswide(c);
			break;
		case _9B:
		case _9A:
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
		size_t len = __mbctype(*ptr);

		switch (len) {
		case _9D:
		case _9C:
		case _9B:
	 		c = __mbtowc(ptr);
			if (need_rewind_wc(c))
				return ptr;

			cnt += iswide(c);
			break;
		case _9A:
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

		sl_push_back_chr(sl, buf);
		str = next;
	}

	free(buf);
}

static const wchar_t *advance_word_wc(const wchar_t *ptr,
				      const wchar_t *tail, size_t limit)
{
	size_t cnt = 0;

	while (ptr < tail && cnt < limit) {
		switch (__wcspmask(*ptr)) {
		case _3WH:
			cnt++;
			ptr++;
			break;
		default:
			cnt += iswide(*ptr);
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
		switch (__wcspmask(*ptr)) {
		case _3WH:
			cnt++;
			break;
		default:
			if (need_rewind_wc(*ptr))
				return ptr;
			cnt += iswide(*ptr);
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

		while (!iswcsp(*next) && iswspace(*next))
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
	xchar *str;
	xchar **ret = xmalloc((sl->items + 1) * sizeof(void *));
	xchar **ptr = ret;

	while ((str = sl_pop(sl)) != NULL)
		*ptr++ = str;

	*ptr = NULL;
	return ret;
}
