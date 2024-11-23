#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

# Requires CC

from os import devnull, environ as env
from sys import argv
from subprocess import run

if len(argv) < 2:
	exit(128)

cc = env['CC']

cmd = [ cc, '-Werror', '-S', '-x', 'c', '-o', devnull, '-' ]
res = run(cmd, text=True, input=argv[1], capture_output=True)

exit(res.returncode)
