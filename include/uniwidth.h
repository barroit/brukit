/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_UNIWIDTH_H
#define NG39_UNIWIDTH_H

#include <stddef.h>

#include "attr.h"

#define uniwidth_size 59

extern const wchar_t uniwidth[][2];

int isfullwidth(wchar_t c) __leaf __pure;

#endif /* NG39_UNIWIDTH_H */
