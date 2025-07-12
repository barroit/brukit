// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "utf8.h"

unsigned char _utf8[] = {
	[0x00 ... 0x7f] = UTF8_1B,
	[0x80 ... 0xbf] = UTF8_CONT,
	[0xc0 ... 0xdf] = UTF8_2B,
	[0xe0 ... 0xef] = UTF8_3B,
	[0xf0 ... 0xf7] = UTF8_4B,
	[0xf8 ... 0xff] = -1,
};
