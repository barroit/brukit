/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_MKDIR_H
#define NG39_MKDIR_H

#include "types.h"

int __mkdirp(xchar *buf);

int mkdirp(const xchar *name);

#endif /* NG39_MKDIR_H */
