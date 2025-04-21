/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PATHALT_H
#define NG39_PATHALT_H

#include "types.h"

#include "pathalt_types.h"

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
