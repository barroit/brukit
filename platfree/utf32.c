// SPDX-License-Identifier: GPL-3.0-or-later or MIT
/*
 * Copyright 2024, 2025 Jiamu Sun <barroit@linux.com>
 *
 * Generated by scripts/utf32gen.py.
 */

#include "utf32.h"

#include "calc.h"

static const wchar_t _termchar[] = {
	0x0021,  /* EXCLAMATION MARK */
	0x002c,  /* COMMA */
	0x002e,  /* FULL STOP */
	0x003f,  /* QUESTION MARK */
	0x2025,  /* TWO DOT LEADER */
	0x2026,  /* HORIZONTAL ELLIPSIS */
	0x3001,  /* IDEOGRAPHIC COMMA */
	0x3002,  /* IDEOGRAPHIC FULL STOP */
	0xff01,  /* FULLWIDTH EXCLAMATION MARK */
	0xff0c,  /* FULLWIDTH COMMA */
	0xff0e,  /* FULLWIDTH FULL STOP */
	0xff1f,  /* FULLWIDTH QUESTION MARK */
};

static const wchar_t _fullwidth[][2] = {
	{ 0x1100, 0x115f },
	{ 0x231a, 0x231b },
	{ 0x2329, 0x232a },
	{ 0x23e9, 0x23ec },
	{ 0x23f0, 0x23f0 },
	{ 0x23f3, 0x23f3 },
	{ 0x25fd, 0x25fe },
	{ 0x2614, 0x2615 },
	{ 0x2630, 0x2637 },
	{ 0x2648, 0x2653 },
	{ 0x267f, 0x267f },
	{ 0x268a, 0x268f },
	{ 0x2693, 0x2693 },
	{ 0x26a1, 0x26a1 },
	{ 0x26aa, 0x26ab },
	{ 0x26bd, 0x26be },
	{ 0x26c4, 0x26c5 },
	{ 0x26ce, 0x26ce },
	{ 0x26d4, 0x26d4 },
	{ 0x26ea, 0x26ea },
	{ 0x26f2, 0x26f3 },
	{ 0x26f5, 0x26f5 },
	{ 0x26fa, 0x26fa },
	{ 0x26fd, 0x26fd },
	{ 0x2705, 0x2705 },
	{ 0x270a, 0x270b },
	{ 0x2728, 0x2728 },
	{ 0x274c, 0x274c },
	{ 0x274e, 0x274e },
	{ 0x2753, 0x2755 },
	{ 0x2757, 0x2757 },
	{ 0x2795, 0x2797 },
	{ 0x27b0, 0x27b0 },
	{ 0x27bf, 0x27bf },
	{ 0x2b1b, 0x2b1c },
	{ 0x2b50, 0x2b50 },
	{ 0x2b55, 0x2b55 },
	{ 0x2e80, 0x2e99 },
	{ 0x2e9b, 0x2ef3 },
	{ 0x2f00, 0x2fd5 },
	{ 0x2ff0, 0x303e },
	{ 0x3041, 0x3096 },
	{ 0x3099, 0x30ff },
	{ 0x3105, 0x312f },
	{ 0x3131, 0x318e },
	{ 0x3190, 0x31e5 },
	{ 0x31ef, 0x321e },
	{ 0x3220, 0x3247 },
	{ 0x3250, 0xa48c },
	{ 0xa490, 0xa4c6 },
	{ 0xa960, 0xa97c },
	{ 0xac00, 0xd7a3 },
	{ 0xf900, 0xfaff },
	{ 0xfe10, 0xfe19 },
	{ 0xfe30, 0xfe52 },
	{ 0xfe54, 0xfe66 },
	{ 0xfe68, 0xfe6b },
	{ 0xff01, 0xff60 },
	{ 0xffe0, 0xffe6 },
};

int wc_termchar(wchar_t c)
{
	size_t l = 0;
	size_t r = sizeof_array(_termchar);

	while (l < r) {
		size_t m = (l + r) >> 1;

		if (c < _termchar[m])
			r = m;
		else if (c > _termchar[m])
			l = m + 1;
		else
			return 1;
	}

	return 0;
}

int wc_fullwidth(wchar_t c)
{
	size_t l = 0;
	size_t r = sizeof_array(_fullwidth);

	while (l < r) {
		size_t m = (l + r) >> 1;

		if (c < _fullwidth[m][0])
			r = m;
		else if (c > _fullwidth[m][1])
			l = m + 1;
		else
			return 1;
	}

	return 0;
}
