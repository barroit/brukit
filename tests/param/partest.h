/* SPDX-License-Identifier: GPL-3.0-or-later or MIT */
/*
 * Copyright 2025 Jiamu Sun <barroit@linux.com>
 */

#ifndef NG39_PARTEST_H
#define NG39_PARTEST_H

#include <stdio.h>

int __main(int argc, const char **argv);

#define pprint_int(var)  printf("39\t%s\t%d\n", #var, var)
#define pprint_uint(var) printf("39\t%s\t%u\n", #var, var)

#define pprint_str(var) printf("39\t%s\t%s\n", #var, var)

#define pprint_cmd(var) printf("39\tcmd\t%s\n", #var)

#define pprint_cmdmode(var) printf("39\tmode\t%s\n", #var)

#endif /* NG39_PARTEST_H */
