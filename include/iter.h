/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_ITER_H
#define NG39_ITER_H

#include "calc.h"

#define idx_for_each(idx, step) \
	for (idx = 0; idx < step; idx++)

/*
 * idx_for_each_reverse() handles indices up to the maximum value of type
 * idx - 1.
 */
#define idx_for_each_reverse(idx, init) \
	for (idx = init; idx != maxof(idx); idx--)

#define idx_for_each_from(idx, step) \
	for (; idx < step; idx++)

#endif /* NG39_ITER_H */
