// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "pathwalk.h"

#include "path.h"
#include "xalloc.h"
#include "xc.h"
#include "xcf.h"

void __pw_init(struct pathwalk *pw, xchar *ptb,
	       uint len, xchar *rtb, u32 flags)
{
	memset(pw, 0, sizeof(*pw));

	pw->ptb = ptb;
	pw->len = len;
	pw->rtb = rtb;
	pw->flags = flags;

	pw_step(pw);

	switch (pw->st) {
	case PW_ROOT_NAME:
		pw->root_len = pw->comp_len;
		pw_step(pw);

		if (pw->st != PW_ROOT_DIR)
			break;

	case PW_ROOT_DIR:
		pw->root_len += 1;
		pw->flags |= PW_IS_ABS;
	}

	pw->st = PW_END;
}

void pw_init(struct pathwalk *pw, const xchar *path)
{
	uint len = xc_strlen(path);
	uint size = ((len + 1) * sizeof(*path)) * 2;

	xchar *ptb = xmalloc(size);
	xchar *rtb = &ptb[len + 1];

	memcpy(ptb, path, len + 1);
	memcpy(rtb, path, len + 1);

	__pw_init(pw, ptb, len, rtb, PW_RTB_SHARE);
}

void pw_destroy(struct pathwalk *pw)
{
	if (!(pw->flags & PW_RTB_EQPTB) && !(pw->flags & PW_RTB_SHARE))
		free(pw->rtb);

	free(pw->ptb);
}

static inline void next_state(struct pathwalk *pw,
			      enum pathwalk_state st,
			      const xchar *comp, uint len)
{
	pw->st = st;

	pw->comp = comp;
	pw->comp_len = len;
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

const xchar *pw_skip_sep_back(const xchar *ptr, const xchar *end)
{
	while (ptr >= end && chr_is_sep(*ptr))
		ptr--;

	return ptr;
}

const xchar *pw_skip_nsep_back(const xchar *ptr, const xchar *end, uint n)
{
	const xchar *next = pw_skip_sep_back(ptr, end);

	if (ptr - n == next)
		return next;

	return ptr;
}

const xchar *pw_skip_name_back(const xchar *ptr, const xchar *end)
{
	while (ptr >= end && !chr_is_sep(*ptr))
		ptr--;

	const xchar *tmp = pw_skip_root(&ptr[1]);

	if (tmp != &ptr[1])
		ptr = &tmp[-1];
	return ptr;
}

int pw_step(struct pathwalk *pw)
{
	const xchar *prev = NULL;
	const xchar *next;

	switch (pw->st) {
	case PW_PRE:
		prev = pw->ptb;
		next = pw_skip_root(prev);

		if (next != prev) {
			next_state(pw, PW_ROOT_NAME, prev, next - prev);
			break;
		}

	case PW_ROOT_NAME:
		if (!prev)
			prev = &pw->comp[pw->comp_len];
		next = pw_skip_sep(prev);

		if (next != prev) {
			next_state(pw, PW_ROOT_DIR, prev, next - prev);
			break;
		}

	case PW_ROOT_DIR:
		if (!prev)
			prev = &pw->comp[pw->comp_len];
		next = pw_skip_name(prev);

		if (next != prev) {
			next_state(pw, PW_FILE_NAME, prev, next - prev);
			break;
		}

		next_state(pw, PW_END, NULL, 0);
		goto done;

	case PW_FILE_NAME:
		prev = &pw->comp[pw->comp_len];
		next = pw_skip_sep(prev);

		if (*next) {
			prev = pw_skip_name(next);
			next_state(pw, PW_FILE_NAME, next, prev - next);
			break;
		} else if (next != prev) {
			next_state(pw, PW_TAIL_SEP, prev, next - prev);
			break;
		}

	case PW_TAIL_SEP:
		next_state(pw, PW_END, NULL, 0);
	default:
done:
		return -1;
	}

	return 0;
}

int pw_step_back(struct pathwalk *pw)
{
	const xchar *prev;
	const xchar *next;
	const xchar *tmp;

	switch (pw->st) {
	case PW_END:
		prev = &pw->ptb[pw->len - 1];
		next = pw_skip_sep_back(prev, pw->ptb);

		if (next == prev) {
			next = pw_skip_root_back(prev, pw->ptb);

			if (next < pw->ptb) {
				next_state(pw, PW_ROOT_NAME,
					   &next[1], prev - next);
				break;
			}

			next = pw_skip_name_back(prev, pw->ptb);
			next_state(pw, PW_FILE_NAME, &next[1], prev - next);
			break;
		}

		if (next < pw->ptb) {
			next_state(pw, PW_ROOT_DIR, &next[1], prev - next);
			break;
		}

		tmp = pw_skip_root_back(next, pw->ptb);

		if (tmp < pw->ptb)
			next_state(pw, PW_ROOT_DIR, &next[1], prev - next);
		else
			next_state(pw, PW_TAIL_SEP, &next[1], prev - next);
		break;

	case PW_TAIL_SEP:
		prev = &pw->comp[-1];
		next = pw_skip_name_back(prev, pw->ptb);

		next_state(pw, PW_FILE_NAME, &next[1], prev - next);
		break;

	case PW_FILE_NAME:
		prev = &pw->comp[-1];
		next = pw_skip_sep_back(prev, pw->ptb);

		if (next < pw->ptb) {
			if (next == prev) {
				next_state(pw, PW_PRE, NULL, 0);
				goto done;
			}

			next_state(pw, PW_ROOT_DIR, &next[1], prev - next);
			break;
		}

		tmp = pw_skip_root_back(next, pw->ptb);

		if (tmp >= pw->ptb) {
			prev = next;
			next = pw_skip_name_back(prev, pw->ptb);

			next_state(pw, PW_FILE_NAME, &next[1], prev - next);
			break;
		}

		if (next != prev)
			next_state(pw, PW_ROOT_DIR, &next[1], prev - next);
		else
			next_state(pw, PW_ROOT_NAME, &tmp[1], prev - tmp);
		break;

	case PW_ROOT_DIR:
		prev = &pw->comp[-1];
		next = &pw->ptb[-1];

		if (prev < pw->ptb) {
			next_state(pw, PW_PRE, NULL, 0);
			goto done;
		}

		next_state(pw, PW_ROOT_NAME, &next[1], prev - next);
		break;

	case PW_ROOT_NAME:
		next_state(pw, PW_PRE, NULL, 0);
	default:
done:
		return -1;
	}

	return 0;
}

const xchar *pw_to_parent(struct pathwalk *pw)
{
	uint len = pw->root_len;
	enum pathwalk_state st = pw->st;

	pw_step_back(pw);

	if (pw->st == PW_TAIL_SEP)
		pw_step_back(pw);
	if (st == PW_END)
		pw_step_back(pw);

	switch (pw->st) {
	case PW_FILE_NAME:
		len = pw->comp - pw->ptb + pw->comp_len;
		break;

	case PW_ROOT_NAME:
		len = pw->comp - pw->ptb + pw->comp_len;
		if (pw->flags & PW_IS_ABS)
			len += 1;
		break;

	case PW_ROOT_DIR:
		len = pw->comp - pw->ptb + 1;
		break;

	case PW_PRE:
		if (!(pw->flags & PW_IS_ABS))
			return XC(".");

		goto out;
	}

	if (pw->flags & PW_RTB_EQPTB) {
		pw->st = PW_END;
		pw->len = len;
	}

out:
	if (!(pw->flags & PW_RTB_EQPTB))
		memcpy(pw->rtb, pw->ptb, len * sizeof(*pw->ptb));

	pw->rtb[len] = 0;
	return pw->rtb;
}

const xchar *pw_basename(struct pathwalk *pw)
{
	pw->st = PW_END;
	pw_step_back(pw);

	if (pw->st == PW_TAIL_SEP) {
		*(xchar *)pw->comp = 0;
		pw_step_back(pw);
	}

	if (pw->st == PW_ROOT_DIR) {
		const xchar *ret = &pw->comp[pw->comp_len - 1];

		pw_step_back(pw);

		if (pw->st == PW_ROOT_NAME)
			return pw->ptb;
		return ret;
	}

	return pw->comp;
}

const xchar *pw_dirname(struct pathwalk *pw)
{
	BUG_ON(pw->flags & PW_RTB_EQPTB);

	enum pathwalk_state st = pw->st;
	const xchar *comp = pw->comp;

	const xchar *ret = pw_to_parent(pw);

	pw->st = st;
	pw->comp = comp;
	return ret;
}
