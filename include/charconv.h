/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_CHARCONV_H
#define NG39_CHARCONV_H

#include <stddef.h>

size_t cc_wcstombs(char **dest, const wchar_t *src);

#endif /* NG39_CHARCONV_H */
