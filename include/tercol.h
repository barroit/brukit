/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_TERCOL_H
#define NG39_TERCOL_H

/*
 * \033[<attributes>;<foreground>;<background>m
 *
 * Foreground		Background		Attribute
 * 30    Black		40	Black		0	Reset
 * 31    Red		41	Red		1	Bold/bright
 * 32    Green		42	Green		2	Faint
 * 33    Yellow		43	Yellow		3	Italic
 * 34    Blue		44	Blue		4	Underline
 * 35    Magenta	45	Magenta		5	Blink
 * 36    Cyan		46	Cyan		7	Inverse
 * 37    White		47	White		22	Normal intensity
 * 90    Bright Black	100	Bright Black	24	No underline
 * 91    Bright Red	101	Bright Red	25	No blink
 * 92    Bright Green	102	Bright Green
 * 93    Bright Yellow	103	Bright Yellow
 * 94    Bright Blue	104	Bright Blue
 * 95    Bright Magenta	105	Bright Magenta
 * 96    Bright Cyan	106	Bright Cyan
 * 97    Bright White	107	Bright White
 */

#define __tc_beg "\033["
#define __tc_end "m"
#define __tc_sep ";"

#define __fmtcol(...)      ___fmtcol(ARGCOF(__VA_ARGS__), __VA_ARGS__)
#define ___fmtcol(...)     ____fmtcol(__VA_ARGS__)
#define ____fmtcol(x, ...) __fmtcol_##x(__VA_ARGS__)

#define __fmtcol_1(x)       __tc_beg #x __tc_end
#define __fmtcol_2(x, y)    __tc_beg #x __tc_sep #y __tc_end
#define __fmtcol_3(x, y, z) __tc_beg #x __tc_sep #y __tc_sep #z __tc_end

#define TC_RESET __fmtcol(0)

#define TC_BOLD_BLACK   __fmtcol(1, 30)
#define TC_BOLD_RED     __fmtcol(1, 31)
#define TC_BOLD_GREEN   __fmtcol(1, 32)
#define TC_BOLD_YELLOW  __fmtcol(1, 33)
#define TC_BOLD_BLUE    __fmtcol(1, 34)
#define TC_BOLD_MAGENTA __fmtcol(1, 35)
#define TC_BOLD_CYAN    __fmtcol(1, 36)
#define TC_BOLD_WHITE   __fmtcol(1, 37)

#define H_(x, c) TC_BOLD_##c x TC_RESET

#endif /* NG39_TERCOL_H */
