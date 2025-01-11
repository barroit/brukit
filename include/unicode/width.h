/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_UNICODE_WIDTH_H
#define NG39_UNICODE_WIDTH_H

#include <stddef.h>

#include "attr.h"

int iswide(wchar_t c) __leaf __pure;

#endif /* NG39_UNICODE_WIDTH_H */
