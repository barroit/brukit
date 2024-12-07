// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "string.h"

#include "mbctype.h"

size_t __mbslen(const char *s)
{
	size_t len = 0;

	while (*s) {
		len += _39mbctype[(int)(unsigned char)*s];
		s++;
	}

	return len;
}
