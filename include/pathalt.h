/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PATHALT_H
#define NG39_PATHALT_H

#include "types.h"

enum pathalt_state {
	PA_PRE,
	PA_ROOT_DIR,
	PA_ROOT_NAME,
	PA_FILE_NAME,
	PA_TAIL_SEP,
	PA_END,
};

#define PA_IS_ABS  (1U << 0)
#define PA_SHR_RTB (1U << 1)

/*
 * Null terminate buffer on both side, otherwise overflow.
 * Some optimizations apply if pathalt->buf and pathalt->rtb point to same
 * address. In that case, pathalt->st becomes unreliable, and don't use:
 *
 *	pa_dirname
 */
struct pathalt {
	xchar *buf;
	uint len;

	const xchar *se;
	uint se_len;

	xchar *rtb;

	enum pathalt_state st;

	uint root_len;

	u32 flags;
};

void __pa_init(struct pathalt *pa, xchar *buf, uint len, xchar *rtb);

void pa_init(struct pathalt *pa, const xchar *path);

void pa_destroy(struct pathalt *pa);

const xchar *pa_to_parent(struct pathalt *pa);

const xchar *pa_basename(struct pathalt *pa);

const xchar *pa_dirname(struct pathalt *pa);

int pa_step(struct pathalt *pa);

int pa_step_back(struct pathalt *pa);

const xchar *pa_skip_root(const xchar *name);
const xchar *pa_skip_sep(const xchar *name);
const xchar *pa_skip_nsep(const xchar *name, uint n);
const xchar *pa_skip_name(const xchar *name);

const xchar *pa_skip_root_back(const xchar *name);
const xchar *pa_skip_sep_back(const xchar *name);
const xchar *pa_skip_nsep_back(const xchar *name, uint n);
const xchar *pa_skip_name_back(const xchar *name);

#endif /* NG39_PATHALT_H */
