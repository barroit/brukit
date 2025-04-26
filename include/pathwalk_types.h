/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PATHWALK_TYPES_H
#define NG39_PATHWALK_TYPES_H

#include "types.h"

enum pathwalk_state {
	__PW_HEAD,

	PW_ROOT_DIR,
	PW_ROOT_NAME,
	PW_FILE_NAME,
	PW_TAIL_SEP,

	__PW_TAIL,
};

#define PW_IS_ABS    (1U << 0)
#define PW_RTB_SHARE (1U << 1)

/*
 * Some optimizations apply if pathwalk->ptb and pathwalk->rtb point to same
 * address. In that case, pathwalk->st becomes unreliable, and don't use:
 *
 *	pw_dirname
 */
struct pathwalk {
	xchar *ptb;
	uint len;

	const xchar *comp;
	uint comp_len;

	xchar *rtb;

	enum pathwalk_state st;

	uint root_len;

	u32 flags;
};

#endif /* NG39_PATHWALK_TYPES_H */
