#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

# Start menuconfig/alldefconfig in a portable manner.
# You must provide TREE, CC, and LD in the process that starts this script.

from sys import argv, exit
from os import environ as env, getcwd, path, pathsep, chdir
from subprocess import run

if len(argv) < 2:
	exit(128)

cmd = argv[1]
ws  = env['TREE']
cwd = getcwd()
lib = path.join(ws, 'lib')
pyp = env.get('PYTHONPATH')

env['PYTHONPATH'] = (pyp + pathsep if pyp else '') + lib
env['PYTHONDONTWRITEBYTECODE'] = 'y'
env['KCONFIG_FUNCTIONS'] = 'Kinclude'

if cmd == 'menuconfig':
	env['MENUCONFIG_STYLE'] = 'aquatic'
elif cmd == 'alldefconfig':
	pass
elif cmd == 'genconfig':
	pass
else:
	exit(128)

if cwd != ws:
	chdir(ws)

res = run(argv[1:])
exit(res.returncode)
