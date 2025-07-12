// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include "calc.h"
#include "hex.h"
#include "iter.h"

UT_BEGIN();

UT_ROUTINE(hex_to_bin)
{
	int val;
	uint i;
	const xchar str1[] = { '0', '1', '2', '3', '4', '5', '6', '7',
			      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	const xchar str2[] = { '0', '1', '2', '3', '4', '5', '6', '7',
			       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	const xchar res[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
			      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

	for_each_idx(i, sizeof_array(str1)) {
		val = hex_to_bin(str1[i]);
		UA_EQ(val, res[i]);

		val = hex_to_bin(str2[i]);
		UA_EQ(val, res[i]);
	}
}

UT_ROUTINE(hex2bin)
{
	int err;
	u8 dst[4];

	err = hex2bin(dst, XC("ab"), 1);
	UA_NOT(err);
	UA_EQ(dst[0], 0xab);

	err = hex2bin(dst, XC("1234"), 2);
	UA_NOT(err);
	UA_EQ(dst[0], 0x12);
	UA_EQ(dst[1], 0x34);

	err = hex2bin(dst, XC("deadbeef"), 4);
	UA_NOT(err);
	UA_EQ(dst[0], 0xde);
	UA_EQ(dst[1], 0xad);
	UA_EQ(dst[2], 0xbe);
	UA_EQ(dst[3], 0xef);

	err = hex2bin(dst, XC("1234miku"), 4);
	UA_IS(err);
}

UT_ROUTINE(bin2hex)
{
	xchar dst[9] = { 0 };

	bin2hex(dst, (u8 []){ 0xab }, 1);
	UA_STREQ(dst, XC("ab"));

	bin2hex(dst, (u8 []){ 0x12, 0x34 }, 2);
	UA_STREQ(dst, XC("1234"));

	bin2hex(dst, (u8 []){ 0xde, 0xad, 0xbe, 0xef }, 4);
	UA_STREQ(dst, XC("deadbeef"));
}

UT_END();
