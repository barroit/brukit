/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PATHALT_TYPES_H
#define NG39_PATHALT_TYPES_H

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
 *
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

#endif /* NG39_PATHALT_TYPES_H */
