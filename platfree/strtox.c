// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "strtox.h"

#include <errno.h>
#include <limits.h>

#include "attr.h"
#include "xchar.h"

#define STRTOX_OVERFLOW (-1U << 31)

static const xchar *fixup_radix(uint *base, const xchar *s)
{
	if (*base == 0) {
		*base = 10;
		if (s[0] == '0') {
			*base = 8;
			if ((s[1] == 'x' || s[1] == 'X') && isxdigit(s[2]))
				*base = 16;
		}
	}

	if (*base == 16 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
		s += 2;

	return s;
}

static uint parse_integer(unsigned long long *__res,
			  const xchar *s, uint base, size_t limit)
{
	unsigned long long res = 0;
	uint ret = 0;

	for (; limit; limit--, s++, ret++) {
		uint c = s[0];
		uint lc = tolower(c);
		uint val;

		if ('0' <= c && c <= '9')
			val = c - '0';
		else if ('a' <= lc && lc <= 'z')
			val = lc - 'a';
		else
			break;

		if (val >= base)
			break;

		if (unlikely(res & (-1ULL << 60)))
			if (res > (ULLONG_MAX - val) / base)
				ret |= STRTOX_OVERFLOW;

		res = res * base + val;
	}

	*__res = res;
	return ret;
}

static int __strtoull(unsigned long long *__res, const xchar *s, uint base)
{
	unsigned long long res;
	uint ret;

	s = fixup_radix(&base, s);
	ret = parse_integer(&res, s, base, -1);

	if (ret & STRTOX_OVERFLOW)
		return -ERANGE;
	else if (ret == 0 || s[ret] != 0)
		return -EINVAL;

	*__res = res;
	return 0;
}

int stx_strtoull(unsigned long long *res, const xchar *s, uint base)
{
	if (s[0] == '+')
		s++;

	return __strtoull(res, s, base);
}

int stx_strtoll(long long *__res, const xchar *s, uint base)
{
	unsigned long long res;
	int err;

	if (s[0] == '-') {
		err = __strtoull(&res, &s[1], base);
		if (err)
			return err;
		if ((long long)-res > 0)
			return -ERANGE;
		*__res = -res;
	} else {
		err = stx_strtoull(&res, s, base);
		if (err)
			return err;
		if ((long long)res < 0)
			return -ERANGE;
		*__res = res;
	}

	return 0;
}

#define __STRTOU(res, s, base)				\
({							\
	unsigned long long __res;			\
	int ret;					\
							\
	ret = stx_strtoull(&__res, s, base);		\
	if (ret == 0 && __res != (typeof(*(res)))__res)	\
		ret = -ERANGE;				\
							\
	if (ret == 0)					\
		*res = __res;				\
	ret;						\
})

#define __STRTOS(res, s, base)				\
({							\
	long long __res;				\
	int ret;					\
							\
	ret = stx_strtoll(&__res, s, base);		\
	if (ret == 0 && __res != (typeof(*(res)))__res)	\
		ret = -ERANGE;				\
							\
	if (ret == 0)					\
		*res = __res;				\
	ret;						\
})

int stx_strtoul(ulong *res, const xchar *s, uint base)
{
	return __STRTOU(res, s, base);
}

int stx_strtol(long *res, const xchar *s, uint base)
{
	return __STRTOS(res, s, base);
}

int stx_strtouint(uint *res, const xchar *s, uint base)
{
	return __STRTOU(res, s, base);
}

int stx_strtoint(int *res, const xchar *s, uint base)
{
	return __STRTOS(res, s, base);
}

int stx_strtou64(u64 *res, const xchar *s, uint base)
{
	return __STRTOU(res, s, base);
}

int stx_strtos64(s64 *res, const xchar *s, uint base)
{
	return __STRTOS(res, s, base);
}

int stx_strtou32(u32 *res, const xchar *s, uint base)
{
	return __STRTOU(res, s, base);
}

int stx_strtos32(s32 *res, const xchar *s, uint base)
{
	return __STRTOS(res, s, base);
}

int stx_strtou16(u16 *res, const xchar *s, uint base)
{
	return __STRTOU(res, s, base);
}

int stx_strtos16(s16 *res, const xchar *s, uint base)
{
	return __STRTOS(res, s, base);
}

int stx_strtou8(u8 *res, const xchar *s, uint base)
{
	return __STRTOU(res, s, base);
}

int stx_strtos8(s8 *res, const xchar *s, uint base)
{
	return __STRTOS(res, s, base);
}
