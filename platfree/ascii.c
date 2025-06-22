// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 */

#include "ascii.h"

const unsigned char _ascii[] = {
	[0   ... 8]   = ASCII_CTRL,
	[9   ... 13]  = ASCII_CTRL | ASCII_SPACE,
	[14  ... 31]  = ASCII_CTRL,
	[32  ... 32]  = ASCII_SPACE | ASCII_BLANK,
	[33  ... 47]  = ASCII_PUNCT,
	[48  ... 57]  = ASCII_DIGIT,
	[58  ... 64]  = ASCII_PUNCT,
	[65  ... 70]  = ASCII_UPPER | ASCII_HEX,
	[71  ... 90]  = ASCII_UPPER,
	[91  ... 96]  = ASCII_PUNCT,
	[97  ... 102] = ASCII_LOWER | ASCII_HEX,
	[103 ... 122] = ASCII_LOWER,
	[123 ... 126] = ASCII_PUNCT,
	[127 ... 127] = ASCII_CTRL,
	[128 ... 159] = 0,
	[160 ... 160] = ASCII_SPACE | ASCII_BLANK,
	[161 ... 191] = ASCII_PUNCT,
	[192 ... 214] = ASCII_UPPER,
	[215 ... 215] = ASCII_PUNCT,
	[216 ... 222] = ASCII_UPPER,
	[223 ... 246] = ASCII_LOWER,
	[247 ... 247] = ASCII_PUNCT,
	[248 ... 255] = ASCII_LOWER,
};
