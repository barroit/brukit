// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "strtox.h"

#include <errno.h>
#include <limits.h>

#include "ascii.h"
#include "attr.h"
#include "compiler.h"
#include "xc.h"

#define STRTOX_OVERFLOW (-1U << 31)

static const xchar *fixup_radix(const xchar *s, uint *base)
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

static uint parse_integer(const xchar *s, uint base,
			  size_t limit, unsigned long long *__res)
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
			val = lc - 'a' + 10;
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

static int __strtoull(const xchar *s, uint base, unsigned long long *__res)
{
	unsigned long long res;
	uint ret;

	s = fixup_radix(s, &base);
	ret = parse_integer(s, base, -1, &res);

	if (ret & STRTOX_OVERFLOW) {
		errno = ERANGE;
		return -1;
	} else if (ret == 0 || s[ret] != 0) {
		errno = EINVAL;
		return -1;
	}

	*__res = res;
	return 0;
}

int stx_strtoull(const xchar *s, uint base, unsigned long long *res)
{
	if (s[0] == '+')
		s++;

	return __strtoull(s, base, res);
}

int stx_strtoll(const xchar *s, uint base, long long *__res)
{
	unsigned long long res;
	int err;

	if (s[0] == '-') {
		err = __strtoull(&s[1], base, &res);
		if (err)
			return err;
		if ((long long)-res > 0)
			goto err_range;
		*__res = -res;
	} else {
		err = stx_strtoull(s, base, &res);
		if (err)
			return err;
		if ((long long)res < 0)
			goto err_range;
		*__res = res;
	}

	return 0;

err_range:
	errno = ERANGE;
	return -1;
}

#define __STRTOU(s, base, res)					\
({								\
	unsigned long long __res;				\
	int ret;						\
								\
	ret = stx_strtoull(s, base, &__res);			\
	if (ret == 0 && __res != (typeof(*(res)))__res)	{	\
		errno = ERANGE;					\
		ret = -1;					\
	}							\
								\
	if (ret == 0)						\
		*res = __res;					\
	ret;							\
})

#define __STRTOS(s, base, res)					\
({								\
	long long __res;					\
	int ret;						\
								\
	ret = stx_strtoll(s, base, &__res);			\
	if (ret == 0 && __res != (typeof(*(res)))__res)	{	\
		errno = ERANGE;					\
		ret = -1;					\
	}							\
								\
	if (ret == 0)						\
		*res = __res;					\
	ret;							\
})

int stx_strtoul(const xchar *s, uint base, ulong *res)
{
	return __STRTOU(s, base, res);
}

int stx_strtol(const xchar *s, uint base, long *res)
{
	return __STRTOS(s, base, res);
}

int stx_strtouint(const xchar *s, uint base, uint *res)
{
	return __STRTOU(s, base, res);
}

int stx_strtoint(const xchar *s, uint base, int *res)
{
	return __STRTOS(s, base, res);
}

int stx_strtou64(const xchar *s, uint base, u64 *res)
{
	return __STRTOU(s, base, res);
}

int stx_strtos64(const xchar *s, uint base, s64 *res)
{
	return __STRTOS(s, base, res);
}

int stx_strtou32(const xchar *s, uint base, u32 *res)
{
	return __STRTOU(s, base, res);
}

int stx_strtos32(const xchar *s, uint base, s32 *res)
{
	return __STRTOS(s, base, res);
}

int stx_strtou16(const xchar *s, uint base, u16 *res)
{
	return __STRTOU(s, base, res);
}

int stx_strtos16(const xchar *s, uint base, s16 *res)
{
	return __STRTOS(s, base, res);
}

int stx_strtou8(const xchar *s, uint base, u8 *res)
{
	return __STRTOU(s, base, res);
}

int stx_strtos8(const xchar *s, uint base, s8 *res)
{
	return __STRTOS(s, base, res);
}
