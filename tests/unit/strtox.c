// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "unitest.h"

#include <errno.h>
#include <limits.h>

#include "strtox.h"

#if __SIZEOF_LONG__ == 8
# define __ULONG_MAX      "18446744073709551615"
# define __ULONG_OVERFLOW "18446744073709551616"
#elif __SIZEOF_LONG__ == 4
# define __ULONG_MAX      "4294967295"
# define __ULONG_OVERFLOW "4294967296"
#endif

#if __SIZEOF_LONG__ == 8
# define __LONG_MIN       "-9223372036854775808"
# define __LONG_MAX       "9223372036854775807"
# define __LONG_UNDERFLOW "-9223372036854775809"
# define __LONG_OVERFLOW  "9223372036854775808"
#elif __SIZEOF_LONG__ == 4
# define __LONG_MIN       "-2147483648"
# define __LONG_MAX       "2147483647"
# define __LONG_UNDERFLOW "-2147483649"
# define __LONG_OVERFLOW  "2147483648"
#endif

UT_BEGIN();

UT_ROUTINE(strtoull)
{
	unsigned long long res, res1, res2;
	int err, err1, err2;

	err1 = stx_strtoull(XC("3939"), 10, &res1);
	err2 = stx_strtoull(XC("3939"), 0, &res2);
	UA_NOT(err1 || err2);
	UA_EQ(res1, 3939);
	UA_EQ(res2, 3939);

	err1 = stx_strtoull(XC("3939"), 16, &res1);
	err2 = stx_strtoull(XC("0x3939"), 0, &res2);
	UA_NOT(err1 || err2);
	UA_EQ(res1, 0x3939);
	UA_EQ(res2, 0x3939);

	err1 = stx_strtoull(XC("0101"), 8, &res1);
	err2 = stx_strtoull(XC("0101"), 0, &res2);
	UA_NOT(err1 || err2);
	UA_EQ(res1, 0101);
	UA_EQ(res2, 0101);

	errno = 0;
	err = stx_strtoull(XC("0101x"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, EINVAL);

	err = stx_strtoull(XC("+0101"), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtoull(XC("-0101"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, EINVAL);

	errno = 0;
	err = stx_strtoull(XC("18446744073709551616"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtoull(XC("18446744073709551615"), 10, &res);
	UA_NOT(err);
	UA_EQ(res, ULLONG_MAX);
}

UT_ROUTINE(strtoll)
{
	long long res;
	int err;

	errno = 0;
	err = stx_strtoll(XC("9223372036854775808"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtoll(XC("9223372036854775807"), 10, &res);
	UA_NOT(err);
	UA_EQ(res, LLONG_MAX);

	errno = 0;
	err = stx_strtoll(XC("-9223372036854775809"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtoll(XC("-9223372036854775808"), 10, &res);
	UA_NOT(err);
	UA_EQ(res, LLONG_MIN);
}

UT_ROUTINE(strtoul)
{
	ulong res;
	int err;

	errno = 0;
	err = stx_strtoul(XC(__ULONG_OVERFLOW), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtoul(XC(__ULONG_MAX), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtol)
{
	long res;
	int err;

	errno = 0;
	err = stx_strtol(XC(__LONG_OVERFLOW), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtol(XC(__LONG_MAX), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtol(XC(__LONG_UNDERFLOW), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtol(XC(__LONG_MIN), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtouint)
{
	uint res;
	int err;

	errno = 0;
	err = stx_strtouint(XC("4294967296"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtouint(XC("4294967295"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtoint)
{
	int res;
	int err;

	errno = 0;
	err = stx_strtoint(XC("2147483648"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtoint(XC("2147483647"), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtoint(XC("-2147483649"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtoint(XC("-2147483648"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtou64)
{
	u64 res;
	int err;

	errno = 0;
	err = stx_strtou64(XC("18446744073709551616"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtou64(XC("18446744073709551615"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtos64)
{
	s64 res;
	int err;

	errno = 0;
	err = stx_strtos64(XC("9223372036854775808"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos64(XC("9223372036854775807"), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtos64(XC("-9223372036854775809"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos64(XC("-9223372036854775808"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtou32)
{
	u32 res;
	int err;

	errno = 0;
	err = stx_strtou32(XC("4294967296"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtou32(XC("4294967295"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtos32)
{
	s32 res;
	int err;

	errno = 0;
	err = stx_strtos32(XC("2147483648"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos32(XC("2147483647"), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtos32(XC("-2147483649"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos32(XC("-2147483648"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtou16)
{
	u16 res;
	int err;

	errno = 0;
	err = stx_strtou16(XC("65536"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtou16(XC("65535"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtos16)
{
	s16 res;
	int err;

	errno = 0;
	err = stx_strtos16(XC("32768"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos16(XC("32767"), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtos16(XC("-32769"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos16(XC("-32768"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtou8)
{
	u8 res;
	int err;

	errno = 0;
	err = stx_strtou8(XC("256"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtou8(XC("255"), 10, &res);
	UA_NOT(err);
}

UT_ROUTINE(strtos8)
{
	s8 res;
	int err;

	errno = 0;
	err = stx_strtos8(XC("128"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos8(XC("127"), 10, &res);
	UA_NOT(err);

	errno = 0;
	err = stx_strtos8(XC("-129"), 10, &res);
	UA_IS(err);
	UA_EQ(errno, ERANGE);

	err = stx_strtos8(XC("-128"), 10, &res);
	UA_NOT(err);
}

UT_END();
