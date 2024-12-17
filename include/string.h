/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

/*
 * For functions in this file, behavior is undefined if the character sequence
 * is invalid.
 */

#ifndef NG39_STRING_H
#define NG39_STRING_H

#include_next <string.h>

#include <stddef.h>

#include "attr.h"
#include "types.h"

size_t __mbslen(const char *s) __leaf __pure;

wchar_t __mbtowc(const char *seq) __leaf __pure;

#endif /* NG39_STRING_H */
