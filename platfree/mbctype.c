// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "mbctype.h"

unsigned char _39mbctype[] = {
	[0x00 ... 0x7F] = _9A,
	[0x80 ... 0xBF] = _9CB,
	[0xC0 ... 0xDF] = _9B,
	[0xE0 ... 0xEF] = _9C,
	[0xF0 ... 0xF7] = _9D,
	[0xF8 ... 0xFF] = -1,
};
