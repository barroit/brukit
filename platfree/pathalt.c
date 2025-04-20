// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "pathalt.h"

#include "path.h"
#include "xalloc.h"
#include "xc.h"
#include "xcf.h"

void __pa_init(struct pathalt *pa, xchar *buf, uint len, xchar *rtb)
{
	BUG_ON(buf[-1] != 0);
	BUG_ON(!len);

	memset(pa, 0, sizeof(*pa));

	pa->buf = buf;
	pa->len = len;
	pa->rtb = rtb;

	if (pa->rtb == pa->buf)
		pa->flags |= PA_SHR_RTB;

	pa_step(pa);

	switch (pa->st) {
	case PA_ROOT_NAME:
		pa->root_len = pa->se_len;
		pa_step(pa);

		if (pa->st != PA_ROOT_DIR)
			break;

	case PA_ROOT_DIR:
		pa->root_len += 1;
		pa->flags |= PA_IS_ABS;
	}

	pa->st = PA_END;
}

void pa_init(struct pathalt *pa, const xchar *path)
{
	uint len = xc_strlen(path);
	uint rsz = (len + 1) * sizeof(*path);
	uint bsz = (len + 2) * sizeof(*path);

	xchar *buf = xmalloc(bsz);
	xchar *rtb = xmalloc(rsz);

	buf[0] = 0;
	memcpy(&buf[1], path, rsz);

	__pa_init(pa, &buf[1], len, rtb);
}

void pa_destroy(struct pathalt *pa)
{
	if (pa->rtb != pa->buf)
		free(pa->rtb);

	free(&pa->buf[-1]);
}

static inline void next_state(struct pathalt *pa,
			      enum pathalt_state st,
			      const xchar *se, uint len)
{
	pa->st = st;

	pa->se = se;
	pa->se_len = len;
}

const xchar *pa_skip_sep(const xchar *name)
{
	while (chr_is_sep(*name))
		name++;

	return name;
}

const xchar *pa_skip_nsep(const xchar *name, uint n)
{
	const xchar *p = pa_skip_sep(name);

	if (name + n == p)
		return p;

	return name;
}

const xchar *pa_skip_name(const xchar *name)
{
	while (*name && !chr_is_sep(*name))
		name++;

	return name;
}

const xchar *pa_skip_sep_back(const xchar *name)
{
	while (chr_is_sep(*name))
		name--;

	return name;
}

const xchar *pa_skip_nsep_back(const xchar *name, uint n)
{
	const xchar *p = pa_skip_sep_back(name);

	if (name - n == p)
		return p;

	return name;
}

const xchar *pa_skip_name_back(const xchar *name)
{
	while (*name && !chr_is_sep(*name))
		name--;

	const xchar *tmp = pa_skip_root(&name[1]);

	if (tmp != &name[1])
		name = &tmp[-1];
	return name;
}

int pa_step(struct pathalt *pa)
{
	const xchar *prev = NULL;
	const xchar *next;

	switch (pa->st) {
	case PA_PRE:
		prev = pa->buf;
		next = pa_skip_root(prev);

		if (next != prev) {
			next_state(pa, PA_ROOT_NAME, prev, next - prev);
			break;
		}

	case PA_ROOT_NAME:
		if (!prev)
			prev = &pa->se[pa->se_len];
		next = pa_skip_sep(prev);

		if (next != prev) {
			next_state(pa, PA_ROOT_DIR, prev, next - prev);
			break;
		}

	case PA_ROOT_DIR:
		if (!prev)
			prev = &pa->se[pa->se_len];
		next = pa_skip_name(prev);

		if (next != prev) {
			next_state(pa, PA_FILE_NAME, prev, next - prev);
			break;
		}

		next_state(pa, PA_END, NULL, 0);
		goto done;

	case PA_FILE_NAME:
		prev = &pa->se[pa->se_len];
		next = pa_skip_sep(prev);

		if (*next) {
			prev = pa_skip_name(next);
			next_state(pa, PA_FILE_NAME, next, prev - next);
			break;
		} else if (next != prev) {
			next_state(pa, PA_TAIL_SEP, prev, next - prev);
			break;
		}

	case PA_TAIL_SEP:
		next_state(pa, PA_END, NULL, 0);
	default:
done:
		return -1;
	}

	return 0;
}

int pa_step_back(struct pathalt *pa)
{
	const xchar *prev;
	const xchar *next;
	const xchar *tmp;

	switch (pa->st) {
	case PA_END:
		prev = &pa->buf[pa->len - 1];
		next = pa_skip_sep_back(prev);

		if (next == prev) {
			next = pa_skip_root_back(prev);

			if (!*next) {
				next_state(pa, PA_ROOT_NAME,
					   &next[1], prev - next);
				break;
			}

			next = pa_skip_name_back(prev);
			next_state(pa, PA_FILE_NAME, &next[1], prev - next);
			break;
		}

		if (!*next) {
			next_state(pa, PA_ROOT_DIR, &next[1], prev - next);
			break;
		}

		tmp = pa_skip_root_back(next);

		if (!*tmp)
			next_state(pa, PA_ROOT_DIR, &next[1], prev - next);
		else
			next_state(pa, PA_TAIL_SEP, &next[1], prev - next);
		break;

	case PA_TAIL_SEP:
		prev = &pa->se[-1];
		next = pa_skip_name_back(prev);

		next_state(pa, PA_FILE_NAME, &next[1], prev - next);
		break;

	case PA_FILE_NAME:
		prev = &pa->se[-1];
		next = pa_skip_sep_back(prev);

		if (!*next) {
			if (next == prev) {
				next_state(pa, PA_PRE, NULL, 0);
				goto done;
			}

			next_state(pa, PA_ROOT_DIR, &next[1], prev - next);
			break;
		}

		tmp = pa_skip_root_back(next);

		if (*tmp) {
			prev = next;
			next = pa_skip_name_back(prev);

			next_state(pa, PA_FILE_NAME, &next[1], prev - next);
			break;
		}

		if (next != prev)
			next_state(pa, PA_ROOT_DIR, &next[1], prev - next);
		else
			next_state(pa, PA_ROOT_NAME, &tmp[1], prev - tmp);
		break;

	case PA_ROOT_DIR:
		prev = &pa->se[-1];
		next = &pa->buf[-1];

		if (!*prev) {
			next_state(pa, PA_PRE, NULL, 0);
			goto done;
		}

		next_state(pa, PA_ROOT_NAME, &next[1], prev - next);
		break;

	case PA_ROOT_NAME:
		next_state(pa, PA_PRE, NULL, 0);
	default:
done:
		return -1;
	}

	return 0;
}

const xchar *pa_to_parent(struct pathalt *pa)
{
	uint len = pa->root_len;
	enum pathalt_state st = pa->st;

	pa_step_back(pa);

	if (pa->st == PA_TAIL_SEP)
		pa_step_back(pa);
	if (st == PA_END)
		pa_step_back(pa);

	switch (pa->st) {
	case PA_FILE_NAME:
		len = pa->se - pa->buf + pa->se_len;
		break;

	case PA_ROOT_NAME:
		len = pa->se - pa->buf + pa->se_len;
		if (pa->flags & PA_IS_ABS)
			len += 1;
		break;

	case PA_ROOT_DIR:
		len = pa->se - pa->buf + 1;
		break;

	case PA_PRE:
		if (!(pa->flags & PA_IS_ABS))
			return XC(".");

		goto out;
	}

	if (pa->flags & PA_SHR_RTB) {
		pa->st = PA_END;
		pa->len = len;
	}

out:
	if (!(pa->flags & PA_SHR_RTB))
		memcpy(pa->rtb, pa->buf, len * sizeof(*pa->buf));

	pa->rtb[len] = 0;
	return pa->rtb;
}

const xchar *pa_basename(struct pathalt *pa)
{
	pa->st = PA_END;
	pa_step_back(pa);

	if (pa->st == PA_TAIL_SEP) {
		*(xchar *)pa->se = 0;
		pa_step_back(pa);
	}

	if (pa->st == PA_ROOT_DIR) {
		const xchar *ret = &pa->se[pa->se_len - 1];

		pa_step_back(pa);

		if (pa->st == PA_ROOT_NAME)
			return pa->buf;
		return ret;
	}

	return pa->se;
}

const xchar *pa_dirname(struct pathalt *pa)
{
	BUG_ON(pa->flags & PA_SHR_RTB);

	enum pathalt_state st = pa->st;
	const xchar *se = pa->se;

	const xchar *ret = pa_to_parent(pa);

	pa->st = st;
	pa->se = se;
	return ret;
}
