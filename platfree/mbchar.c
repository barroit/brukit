// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "mbchar.h"

enum mbstatus mbstatus(char c)
{
	static u8 map[] = {
		[0x00 ... 0x7F] = MB_ASCII,
		[0x80 ... 0xBF] = MB_LBT2,
		[0xC0 ... 0xDF] = MB_LBT3,
		[0xE0 ... 0xEF] = MB_LBT4,
		[0xF0 ... 0xF7] = MB_DATA,
		[0xF8 ... 0xFF] = MB_INVAL,
	};

	return map[(u8)c];
}
