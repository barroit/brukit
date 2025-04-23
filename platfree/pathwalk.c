// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "pathwalk.h"

#include "path.h"
#include "xalloc.h"
#include "xc.h"
#include "xcf.h"

void __pw_init(struct pathwalk *pa, xchar *buf, uint len, xchar *rtb)
{
	BUG_ON(buf[-1] != 0);
	BUG_ON(!len);

	memset(pa, 0, sizeof(*pa));

	pa->buf = buf;
	pa->len = len;
	pa->rtb = rtb;

	if (pa->rtb == pa->buf)
		pa->flags |= PW_SHR_RTB;

	pw_step(pa);

	switch (pa->st) {
	case PW_ROOT_NAME:
		pa->root_len = pa->se_len;
		pw_step(pa);

		if (pa->st != PW_ROOT_DIR)
			break;

	case PW_ROOT_DIR:
		pa->root_len += 1;
		pa->flags |= PW_IS_ABS;
	}

	pa->st = PW_END;
}

void pw_init(struct pathwalk *pa, const xchar *path)
{
	uint len = xc_strlen(path);
	uint rsz = (len + 1) * sizeof(*path);
	uint bsz = (len + 2) * sizeof(*path);

	xchar *buf = xmalloc(bsz);
	xchar *rtb = xmalloc(rsz);

	buf[0] = 0;
	memcpy(&buf[1], path, rsz);

	__pw_init(pa, &buf[1], len, rtb);
}

void pw_destroy(struct pathwalk *pa)
{
	if (pa->rtb != pa->buf)
		free(pa->rtb);

	free(&pa->buf[-1]);
}

static inline void next_state(struct pathwalk *pa,
			      enum pathwalk_state st,
			      const xchar *se, uint len)
{
	pa->st = st;

	pa->se = se;
	pa->se_len = len;
}

const xchar *pw_skip_sep(const xchar *name)
{
	while (chr_is_sep(*name))
		name++;

	return name;
}

const xchar *pw_skip_nsep(const xchar *name, uint n)
{
	const xchar *p = pw_skip_sep(name);

	if (name + n == p)
		return p;

	return name;
}

const xchar *pw_skip_name(const xchar *name)
{
	while (*name && !chr_is_sep(*name))
		name++;

	return name;
}

const xchar *pw_skip_sep_back(const xchar *name)
{
	while (chr_is_sep(*name))
		name--;

	return name;
}

const xchar *pw_skip_nsep_back(const xchar *name, uint n)
{
	const xchar *p = pw_skip_sep_back(name);

	if (name - n == p)
		return p;

	return name;
}

const xchar *pw_skip_name_back(const xchar *name)
{
	while (*name && !chr_is_sep(*name))
		name--;

	const xchar *tmp = pw_skip_root(&name[1]);

	if (tmp != &name[1])
		name = &tmp[-1];
	return name;
}

int pw_step(struct pathwalk *pa)
{
	const xchar *prev = NULL;
	const xchar *next;

	switch (pa->st) {
	case PW_PRE:
		prev = pa->buf;
		next = pw_skip_root(prev);

		if (next != prev) {
			next_state(pa, PW_ROOT_NAME, prev, next - prev);
			break;
		}

	case PW_ROOT_NAME:
		if (!prev)
			prev = &pa->se[pa->se_len];
		next = pw_skip_sep(prev);

		if (next != prev) {
			next_state(pa, PW_ROOT_DIR, prev, next - prev);
			break;
		}

	case PW_ROOT_DIR:
		if (!prev)
			prev = &pa->se[pa->se_len];
		next = pw_skip_name(prev);

		if (next != prev) {
			next_state(pa, PW_FILE_NAME, prev, next - prev);
			break;
		}

		next_state(pa, PW_END, NULL, 0);
		goto done;

	case PW_FILE_NAME:
		prev = &pa->se[pa->se_len];
		next = pw_skip_sep(prev);

		if (*next) {
			prev = pw_skip_name(next);
			next_state(pa, PW_FILE_NAME, next, prev - next);
			break;
		} else if (next != prev) {
			next_state(pa, PW_TAIL_SEP, prev, next - prev);
			break;
		}

	case PW_TAIL_SEP:
		next_state(pa, PW_END, NULL, 0);
	default:
done:
		return -1;
	}

	return 0;
}

int pw_step_back(struct pathwalk *pa)
{
	const xchar *prev;
	const xchar *next;
	const xchar *tmp;

	switch (pa->st) {
	case PW_END:
		prev = &pa->buf[pa->len - 1];
		next = pw_skip_sep_back(prev);

		if (next == prev) {
			next = pw_skip_root_back(prev);

			if (!*next) {
				next_state(pa, PW_ROOT_NAME,
					   &next[1], prev - next);
				break;
			}

			next = pw_skip_name_back(prev);
			next_state(pa, PW_FILE_NAME, &next[1], prev - next);
			break;
		}

		if (!*next) {
			next_state(pa, PW_ROOT_DIR, &next[1], prev - next);
			break;
		}

		tmp = pw_skip_root_back(next);

		if (!*tmp)
			next_state(pa, PW_ROOT_DIR, &next[1], prev - next);
		else
			next_state(pa, PW_TAIL_SEP, &next[1], prev - next);
		break;

	case PW_TAIL_SEP:
		prev = &pa->se[-1];
		next = pw_skip_name_back(prev);

		next_state(pa, PW_FILE_NAME, &next[1], prev - next);
		break;

	case PW_FILE_NAME:
		prev = &pa->se[-1];
		next = pw_skip_sep_back(prev);

		if (!*next) {
			if (next == prev) {
				next_state(pa, PW_PRE, NULL, 0);
				goto done;
			}

			next_state(pa, PW_ROOT_DIR, &next[1], prev - next);
			break;
		}

		tmp = pw_skip_root_back(next);

		if (*tmp) {
			prev = next;
			next = pw_skip_name_back(prev);

			next_state(pa, PW_FILE_NAME, &next[1], prev - next);
			break;
		}

		if (next != prev)
			next_state(pa, PW_ROOT_DIR, &next[1], prev - next);
		else
			next_state(pa, PW_ROOT_NAME, &tmp[1], prev - tmp);
		break;

	case PW_ROOT_DIR:
		prev = &pa->se[-1];
		next = &pa->buf[-1];

		if (!*prev) {
			next_state(pa, PW_PRE, NULL, 0);
			goto done;
		}

		next_state(pa, PW_ROOT_NAME, &next[1], prev - next);
		break;

	case PW_ROOT_NAME:
		next_state(pa, PW_PRE, NULL, 0);
	default:
done:
		return -1;
	}

	return 0;
}

const xchar *pw_to_parent(struct pathwalk *pa)
{
	uint len = pa->root_len;
	enum pathwalk_state st = pa->st;

	pw_step_back(pa);

	if (pa->st == PW_TAIL_SEP)
		pw_step_back(pa);
	if (st == PW_END)
		pw_step_back(pa);

	switch (pa->st) {
	case PW_FILE_NAME:
		len = pa->se - pa->buf + pa->se_len;
		break;

	case PW_ROOT_NAME:
		len = pa->se - pa->buf + pa->se_len;
		if (pa->flags & PW_IS_ABS)
			len += 1;
		break;

	case PW_ROOT_DIR:
		len = pa->se - pa->buf + 1;
		break;

	case PW_PRE:
		if (!(pa->flags & PW_IS_ABS))
			return XC(".");

		goto out;
	}

	if (pa->flags & PW_SHR_RTB) {
		pa->st = PW_END;
		pa->len = len;
	}

out:
	if (!(pa->flags & PW_SHR_RTB))
		memcpy(pa->rtb, pa->buf, len * sizeof(*pa->buf));

	pa->rtb[len] = 0;
	return pa->rtb;
}

const xchar *pw_basename(struct pathwalk *pa)
{
	pa->st = PW_END;
	pw_step_back(pa);

	if (pa->st == PW_TAIL_SEP) {
		*(xchar *)pa->se = 0;
		pw_step_back(pa);
	}

	if (pa->st == PW_ROOT_DIR) {
		const xchar *ret = &pa->se[pa->se_len - 1];

		pw_step_back(pa);

		if (pa->st == PW_ROOT_NAME)
			return pa->buf;
		return ret;
	}

	return pa->se;
}

const xchar *pw_dirname(struct pathwalk *pa)
{
	BUG_ON(pa->flags & PW_SHR_RTB);

	enum pathwalk_state st = pa->st;
	const xchar *se = pa->se;

	const xchar *ret = pw_to_parent(pa);

	pa->st = st;
	pa->se = se;
	return ret;
}
