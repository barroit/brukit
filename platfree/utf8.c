// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "utf8.h"

unsigned char _utf8[] = {
	[0x00 ... 0x7F] = UTF8_1B,
	[0x80 ... 0xBF] = UTF8_CONT,
	[0xC0 ... 0xDF] = UTF8_2B,
	[0xE0 ... 0xEF] = UTF8_3B,
	[0xF0 ... 0xF7] = UTF8_4B,
	[0xF8 ... 0xFF] = -1,
};
