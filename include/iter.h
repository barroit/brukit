/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_ITER_H
#define NG39_ITER_H

/**
 * idx_for_each - iterate over an index
 * @idx: variable to operate on
 * @step: number of increments for the index
 */
#define idx_for_each(idx, step) \
	for (idx = 0; idx < step; idx++)

/**
 * idx_for_each_reverse - iterate backwards over an index
 * @idx: variable to operate on
 * @pos: starting index for the iteration
 *
 * This iterator handles indices up to the maximum value of type idx - 1.
 */
#define idx_for_each_reverse(idx, init) \
	for (idx = init; idx != maxof(idx); idx--)

#endif /* NG39_ITER_H */
