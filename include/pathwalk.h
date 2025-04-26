/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PATHWALK_H
#define NG39_PATHWALK_H

#include "types.h"

#include "pathwalk_types.h"

void __pw_init(struct pathwalk *pw, xchar *ptb,
	       uint len, xchar *rtb, u32 flags);

void pw_init(struct pathwalk *pw, const xchar *path);

void pw_destroy(struct pathwalk *pw);

static inline int pw_in_root(struct pathwalk *pw)
{
	return pw->st == PW_ROOT_NAME || pw->st == PW_ROOT_DIR;
}

/*
 * If reused by other APIs, sync length to pw->len after calling
 * this function.
 */
const xchar *pw_to_dirname(struct pathwalk *pw);

/*
 * Don't use if pw->ptb == pw->rtb. Buggy; avoid unless necessary.
 *
 * May modify pw->ptb. Must sync pw->len if reused by other APIs.
 * Otherwise, corrupts buffer and length.
 */
const xchar *pw_basename(struct pathwalk *pw);

/*
 * Don't use if pw->ptb == pw->rtb.
 */
const xchar *pw_dirname(struct pathwalk *pw);

int pw_step(struct pathwalk *pw);

int pw_step_back(struct pathwalk *pw);

const xchar *pw_skip_root(const xchar *name);
const xchar *pw_skip_sep(const xchar *name);
const xchar *pw_skip_nsep(const xchar *name, uint n);
const xchar *pw_skip_name(const xchar *name);

const xchar *pw_skip_root_back(const xchar *ptr, const xchar *end);
const xchar *pw_skip_sep_back(const xchar *ptr, const xchar *end);
const xchar *pw_skip_nsep_back(const xchar *ptr, const xchar *end, uint n);
const xchar *pw_skip_name_back(const xchar *ptr, const xchar *end);

#endif /* NG39_PATHWALK_H */
