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

/*
 * The buffer should be terminated by a null terminator on both sides.
 *
 * Buffers buf and rtb may point to the same address. In this case, some
 * optimizations may be performed. Functions such as pa_dirname must be
 * avoided. Moreover, st is not reliable.
 */
void __pa_init(struct pathalt *pa, xchar *buf, uint len, xchar *rtb);

void pa_init(struct pathalt *pa, const xchar *path);

void pa_destroy(struct pathalt *pa);

const xchar *pa_skip_root(const xchar *name);

const xchar *pa_skip_sep(const xchar *name);

const xchar *pa_skip_nsep(const xchar *name, uint n);

const xchar *pa_skip_name(const xchar *name);

const xchar *pa_skip_root_back(const xchar *name);

const xchar *pa_skip_sep_back(const xchar *name);

const xchar *pa_skip_nsep_back(const xchar *name, uint n);

const xchar *pa_skip_name_back(const xchar *name);

int pa_step(struct pathalt *pa);

int pa_step_back(struct pathalt *pa);

const xchar *pa_to_parent(struct pathalt *pa);

const xchar *pa_basename(struct pathalt *pa);

const xchar *pa_dirname(struct pathalt *pa);

#endif /* NG39_PATHALT_H */
