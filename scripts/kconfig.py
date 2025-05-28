#!/usr/bin/python3
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

from libkit import *

if argc < 2:
	die('missing command name')

cmd = argv[1]
srctree = env_or_die('SRCTREE')

env['PYTHONPATH'] = f"{srctree}/scripts"
env['KCONFIG_FUNCTIONS'] = 'kinclude'

if cmd == 'menuconfig':
	env['MENUCONFIG_STYLE'] = 'aquatic'
	env['KCONFIG_CONFIG'] = env_or_die('USRCONF')
elif cmd == 'alldefconfig':
	pass
elif cmd == 'genconfig':
	pass
else:
	die(f"unknown command name '{cmd}'")

res = execl(argv[1:])

exit(res.returncode)
