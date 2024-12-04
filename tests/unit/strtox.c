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

	errno = 0;
	err = stx_strtoull(&res, XC("0101x"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, EINVAL);

	err = stx_strtoull(&res, XC("+0101"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtoull(&res, XC("-0101"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, EINVAL);

	errno = 0;
	err = stx_strtoull(&res, XC("18446744073709551616"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtoull(&res, XC("18446744073709551615"), 10);
	USSERT_NOT(err);
	USSERT_EQUAL(res, ULLONG_MAX);
}

TESTDECL_ROUTINE(strtoll)
{
	long long res;
	int err;

	errno = 0;
	err = stx_strtoll(&res, XC("9223372036854775808"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtoll(&res, XC("9223372036854775807"), 10);
	USSERT_NOT(err);
	USSERT_EQUAL(res, LLONG_MAX);

	errno = 0;
	err = stx_strtoll(&res, XC("-9223372036854775809"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtoll(&res, XC("-9223372036854775808"), 10);
	USSERT_NOT(err);
	USSERT_EQUAL(res, LLONG_MIN);
}

TESTDECL_ROUTINE(strtoul)
{
	ulong res;
	int err;

	errno = 0;
	err = stx_strtoul(&res, XC("18446744073709551616"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtoul(&res, XC("18446744073709551615"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtol)
{
	long res;
	int err;

	errno = 0;
	err = stx_strtol(&res, XC("9223372036854775808"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtol(&res, XC("9223372036854775807"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtol(&res, XC("-9223372036854775809"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtol(&res, XC("-9223372036854775808"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtouint)
{
	uint res;
	int err;

	errno = 0;
	err = stx_strtouint(&res, XC("4294967296"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtouint(&res, XC("4294967295"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtoint)
{
	int res;
	int err;

	errno = 0;
	err = stx_strtoint(&res, XC("2147483648"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtoint(&res, XC("2147483647"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtoint(&res, XC("-2147483649"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtoint(&res, XC("-2147483648"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtou64)
{
	u64 res;
	int err;

	errno = 0;
	err = stx_strtou64(&res, XC("18446744073709551616"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtou64(&res, XC("18446744073709551615"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtos64)
{
	s64 res;
	int err;

	errno = 0;
	err = stx_strtos64(&res, XC("9223372036854775808"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos64(&res, XC("9223372036854775807"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtos64(&res, XC("-9223372036854775809"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos64(&res, XC("-9223372036854775808"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtou32)
{
	u32 res;
	int err;

	errno = 0;
	err = stx_strtou32(&res, XC("4294967296"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtou32(&res, XC("4294967295"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtos32)
{
	s32 res;
	int err;

	errno = 0;
	err = stx_strtos32(&res, XC("2147483648"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos32(&res, XC("2147483647"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtos32(&res, XC("-2147483649"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos32(&res, XC("-2147483648"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtou16)
{
	u16 res;
	int err;

	errno = 0;
	err = stx_strtou16(&res, XC("65536"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtou16(&res, XC("65535"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtos16)
{
	s16 res;
	int err;

	errno = 0;
	err = stx_strtos16(&res, XC("32768"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos16(&res, XC("32767"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtos16(&res, XC("-32769"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos16(&res, XC("-32768"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtou8)
{
	u8 res;
	int err;

	errno = 0;
	err = stx_strtou8(&res, XC("256"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtou8(&res, XC("255"), 10);
	USSERT_NOT(err);
}

TESTDECL_ROUTINE(strtos8)
{
	s8 res;
	int err;

	errno = 0;
	err = stx_strtos8(&res, XC("128"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos8(&res, XC("127"), 10);
	USSERT_NOT(err);

	errno = 0;
	err = stx_strtos8(&res, XC("-129"), 10);
	USSERT_IS(err);
	USSERT_EQUAL(errno, ERANGE);

	err = stx_strtos8(&res, XC("-128"), 10);
	USSERT_NOT(err);
}

TESTDECL_END();
