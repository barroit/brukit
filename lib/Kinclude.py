# SPDX-License-Identifier: GPL-3.0-or-later or MIT
# Copyright 2024 Jiamu Sun <barroit@linux.com>

from sys import exit
from os import getpid, mkdir, unlink, path, environ as env
from subprocess import run, PIPE, DEVNULL

import os
import re
import textwrap
import platform
import kconfiglib

RED='\033[0;31m'
YELLOW='\033[0;33m'
RESET='\033[0m'

def die(*args):
	print(f'{RED}fatal:{RESET}', *args)
	exit(128)

def die_kconf(kconf, *args):
	die(f'{YELLOW}{kconf.filename}:{kconf.linenr}:{RESET}', *args)

cc = env['CC']
if not cc:
	die('no cc provided (export CC in environment)')

ld = env['LD']
if not ld:
	die('no ld provided (export LD in environment)')

tree = env['TREE']
if not tree:
	die('no tree path specified (export BSTREE in env)')

def word(kconf, name, val, pos):
	pos = int(pos) - 1
	return val.split('\t')[pos]

def warn_off(kconf, name):
	kconf.warn = False
	return ""

def if_success(cmd, y, n):
	cmd = cmd.split()
	try:
		res = run(cmd, stdout=DEVNULL, stderr=DEVNULL)
	except:
		return n
	return y if res.returncode == 0 else n

def success(kconf, name, cmd):
	return if_success(cmd, 'y', 'n')

def failure(kconf, name, cmd):
	return if_success(cmd, 'n', 'y')

def error_if(kconf, name, cond, mas):
	if cond == 'y':
		die_kconf(kconf, mas)
	return ""

def if_exist(name, y, n):
	return y if path.exists(name) else n

def exist(kconf, name, file):
	return if_exist(file, 'y', 'n')

def not_exist(kconf, name, file):
	return if_exist(file, 'n', 'y')

def if_less(v1, v2, y, n):
	return y if int(v1) < int(v2) else n

def less(kconf, name, v1, v2):
	return if_less(v1, v2, 'y', 'n')

def greater(kconf, name, v1, v2):
	return if_less(v1, v2, 'n', 'y')

def pg_info(kconf, name):
	dotprog = path.join(tree, '.program')
	info = {}

	# Do not use with as it makes readability worse
	file = open(dotprog, 'r')
	for line in file:
		if not line.strip() or line.startswith("#"):
			continue

		k, v = line.split(maxsplit=1)
		info[k] = v.strip()
	file.close()

	name = info['name']
	version = info['version']
	arch = platform.machine().lower()
	build = platform.system()
	return f'{name}\t{version}\t{arch}\t{build}'

def cc_info(kconf, name):
	cmd = [ cc, '-E', '-P', '-x', 'c', '-' ]
	file = textwrap.dedent("""\
		#if defined(__clang__)
		Clang	__clang_major__	__clang_minor__	__clang_patchlevel__
		#elif defined(__GNUC__)
		GCC	__GNUC__	__GNUC_MINOR__	__GNUC_PATCHLEVEL__
		#endif
	""")
	res = run(cmd, text=True, input=file, capture_output=True)

	if not res.stdout:
		die_kconf(kconf, f"compiler '{cc}' is not supported")
	sl = res.stdout.split()

	name = sl[0]
	version = int(sl[1]) * 1000 + int(sl[2])
	minver = (9 if name == 'GCC' else 6) * 1000
	return f'{name}\t{version}\t{minver}'

def cc_option(kconf, name, opt):
	pid = getpid()
	out = f'.tmp_{pid}.o'
	cmd = [ cc, '-Werror', opt, '-c', '-x', 'c', os.devnull, '-o', out ]

	res = run(cmd, stdout=DEVNULL, stderr=DEVNULL)
	if path.isfile(out):
		unlink(out)

	return 'y' if res.returncode == 0 else 'n'

def ld_info(kconf, name):
	cmd = [ ld, '-v' ]
	res = run(cmd, text=True, capture_output=True)

	if not res.stdout:
		die_kconf(f'ld ‘{ld}’ is not supported')
	sl = res.stdout.split()

	if sl[0] == 'GNU' and sl[1] == 'ld':
		# GNU ld (GNU Binutils for Ubuntu) 2.42
		name = 'BFD'
		version = sl[-1]
	elif sl[0] == 'GNU' and sl[1] == 'gold':
		die_kconf('ld.gold is not supported')
	else:
		# Ubuntu LLD 18.1.3 (compatible with GNU linkers)
		name = 'LLD'
		version = sl[2]

	version = re.match(r'^[0-9.]*', version).group(0)
	sl = version.split('.')
	version = int(sl[0]) * 1000 + int(sl[1])
	return f'{name}\t{version}'

def ld_option(kconf, name, opt):
	cmd = [ ld, '-v', opt ]
	res = run(cmd, stdout=DEVNULL, stderr=DEVNULL)
	return 'y' if res.returncode == 0 else 'n'

def to_sver(kconf, name, val):
	val = int(val)
	major = val // 1000
	minor = val % 1000
	return f'{major}.{minor}'

def is_plat(name):
	return 'y' if platform.system() == name else 'n'

def is_win32(kconf, name):
	return is_plat('Windows')

def is_unix(kconf, name):
	return 'y' if is_plat('Windows') == 'n' else 'n'

functions = {
	"word":      (word,      2, 2),
	"warn-off":  (warn_off,  0, 0),
	"to-sver":   (to_sver,   1, 1),
	"is-win32":  (is_win32,  0, 0),
	"is-unix":   (is_unix,   0, 0),

	"success":   (success,   1, 1),
	"failure":   (failure,   1, 1),
	"error-if":  (error_if,  2, 2),

	"exist":     (exist,     1, 1),
	"not-exist": (not_exist, 1, 1),

	"less":      (less,      2, 2),
	"greater":   (greater,   2, 2),

	"pg-info":   (pg_info,   0, 0),

	"cc-info":   (cc_info,   0, 0),
	"cc-option": (cc_option, 1, 1),

	"ld-info":   (ld_info,   0, 0),
	"ld-option": (ld_option, 1, 1),
}
