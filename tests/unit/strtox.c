// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <errno.h>
#include <limits.h>

#include "strtox.h"

TESTDECL_BEGIN();

TESTDECL_ROUTINE(strtoull)
{
	unsigned long long res, res1, res2;
	int err, err1, err2;

	err1 = stx_strtoull(&res1, XC("3939"), 10);
	err2 = stx_strtoull(&res2, XC("3939"), 0);
	USSERT_NOT(err1 || err2);
	USSERT_EQUAL(res1, 3939);
	USSERT_EQUAL(res2, 3939);

	err1 = stx_strtoull(&res1, XC("3939"), 16);
	err2 = stx_strtoull(&res2, XC("0x3939"), 0);
	USSERT_NOT(err1 || err2);
	USSERT_EQUAL(res1, 0x3939);
	USSERT_EQUAL(res2, 0x3939);

	err1 = stx_strtoull(&res1, XC("0101"), 8);
	err2 = stx_strtoull(&res2, XC("0101"), 0);
	USSERT_NOT(err1 || err2);
	USSERT_EQUAL(res1, 0101);
	USSERT_EQUAL(res2, 0101);

	err = stx_strtoull(&res, XC("0101x"), 10);
	USSERT_EQUAL(err, -EINVAL);

	err = stx_strtoull(&res, XC("+0101"), 10);
	USSERT_NOT(err);

	err = stx_strtoull(&res, XC("-0101"), 10);
	USSERT_EQUAL(err, -EINVAL);

	err = stx_strtoull(&res, XC("18446744073709551616"), 10);
	USSERT_EQUAL(err, -ERANGE);

	err = stx_strtoull(&res, XC("18446744073709551615"), 10);
	USSERT_NOT(err);
	USSERT_EQUAL(res, ULLONG_MAX);
}

TESTDECL_ROUTINE(strtoll)
{
	long long res1, res2;
	int err1, err2;

	err1 = stx_strtoll(&res1, XC("9223372036854775808"), 10);
	err2 = stx_strtoll(&res2, XC("9223372036854775807"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
	USSERT_EQUAL(res2, LLONG_MAX);

	err1 = stx_strtoll(&res1, XC("-9223372036854775809"), 10);
	err2 = stx_strtoll(&res2, XC("-9223372036854775808"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
	USSERT_EQUAL(res2, LLONG_MIN);
}

TESTDECL_ROUTINE(strtoul)
{
	ulong res1, res2;
	int err1, err2;

	err1 = stx_strtoul(&res1, XC("18446744073709551616"), 10);
	err2 = stx_strtoul(&res2, XC("18446744073709551615"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtol)
{
	long res1, res2;
	int err1, err2;

	err1 = stx_strtol(&res1, XC("9223372036854775808"), 10);
	err2 = stx_strtol(&res2, XC("9223372036854775807"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);

	err1 = stx_strtol(&res1, XC("-9223372036854775809"), 10);
	err2 = stx_strtol(&res2, XC("-9223372036854775808"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtouint)
{
	uint res1, res2;
	int err1, err2;

	err1 = stx_strtouint(&res1, XC("4294967296"), 10);
	err2 = stx_strtouint(&res2, XC("4294967295"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtoint)
{
	int res1, res2;
	int err1, err2;

	err1 = stx_strtoint(&res1, XC("2147483648"), 10);
	err2 = stx_strtoint(&res2, XC("2147483647"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);

	err1 = stx_strtoint(&res1, XC("-2147483649"), 10);
	err2 = stx_strtoint(&res2, XC("-2147483648"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtou64)
{
	u64 res1, res2;
	int err1, err2;

	err1 = stx_strtou64(&res1, XC("18446744073709551616"), 10);
	err2 = stx_strtou64(&res2, XC("18446744073709551615"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtos64)
{
	s64 res1, res2;
	int err1, err2;

	err1 = stx_strtos64(&res1, XC("9223372036854775808"), 10);
	err2 = stx_strtos64(&res2, XC("9223372036854775807"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);

	err1 = stx_strtos64(&res1, XC("-9223372036854775809"), 10);
	err2 = stx_strtos64(&res2, XC("-9223372036854775808"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtou32)
{
	u32 res1, res2;
	int err1, err2;

	err1 = stx_strtou32(&res1, XC("4294967296"), 10);
	err2 = stx_strtou32(&res2, XC("4294967295"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtos32)
{
	s32 res1, res2;
	int err1, err2;

	err1 = stx_strtos32(&res1, XC("2147483648"), 10);
	err2 = stx_strtos32(&res2, XC("2147483647"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);

	err1 = stx_strtos32(&res1, XC("-2147483649"), 10);
	err2 = stx_strtos32(&res2, XC("-2147483648"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtou16)
{
	u16 res1, res2;
	int err1, err2;

	err1 = stx_strtou16(&res1, XC("65536"), 10);
	err2 = stx_strtou16(&res2, XC("65535"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtos16)
{
	s16 res1, res2;
	int err1, err2;

	err1 = stx_strtos16(&res1, XC("32768"), 10);
	err2 = stx_strtos16(&res2, XC("32767"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);

	err1 = stx_strtos16(&res1, XC("-32769"), 10);
	err2 = stx_strtos16(&res2, XC("-32768"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtou8)
{
	u8 res1, res2;
	int err1, err2;

	err1 = stx_strtou8(&res1, XC("256"), 10);
	err2 = stx_strtou8(&res2, XC("255"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_ROUTINE(strtos8)
{
	s8 res1, res2;
	int err1, err2;

	err1 = stx_strtos8(&res1, XC("128"), 10);
	err2 = stx_strtos8(&res2, XC("127"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);

	err1 = stx_strtos8(&res1, XC("-129"), 10);
	err2 = stx_strtos8(&res2, XC("-128"), 10);
	USSERT_EQUAL(err1, -ERANGE);
	USSERT_NOT(err2);
}

TESTDECL_END();
