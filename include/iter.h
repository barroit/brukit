/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_ITER_H
#define NG39_ITER_H

#include "calc.h"

#define for_each_idx(idx, step) \
	for (idx = 0; idx < step; idx++)

/*
 * for_each_idx_reverse() handles indices up to the maximum value of type
 * idx - 1.
 */
#define for_each_idx_reverse(idx, init) \
	for (idx = init; idx != maxof(idx); idx--)

#define for_each_idx_from(idx, step) \
	for (; idx < step; idx++)

#endif /* NG39_ITER_H */
