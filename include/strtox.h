/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_STRTOX_H
#define NG39_STRTOX_H

#include "types.h"

int stx_strtoull(unsigned long long *res, const xchar *s, uint base);
int stx_strtoul(ulong *res, const xchar *s, uint base);
int stx_strtouint(uint *res, const xchar *s, uint base);

int stx_strtoll(long long *res, const xchar *s, uint base);
int stx_strtol(long *res, const xchar *s, uint base);
int stx_strtoint(int *res, const xchar *s, uint base);

int stx_strtou64(u64 *res, const xchar *s, uint base);
int stx_strtou32(u32 *res, const xchar *s, uint base);
int stx_strtou16(u16 *res, const xchar *s, uint base);
int stx_strtou8(u8 *res, const xchar *s, uint base);

int stx_strtos64(s64 *res, const xchar *s, uint base);
int stx_strtos32(s32 *res, const xchar *s, uint base);
int stx_strtos16(s16 *res, const xchar *s, uint base);
int stx_strtos8(s8 *res, const xchar *s, uint base);

#endif /* NG39_STRTOX_H */
