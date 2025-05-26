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

def env_or_die(name):
	if not name in env:
		die(f"missing env '{name}'")
	return env[name]

def die_kconf(kconf, *args):
	die(f'{YELLOW}{kconf.filename}:{kconf.linenr}:{RESET}', *args)

srctree = env_or_die('SRCTREE')

cc = env_or_die('CC')
ld = env_or_die('LD')

def word(kconf, name, val, pos):
	pos = int(pos) - 1

	return val.split('\t')[pos]

def warn_off(kconf, name):
	kconf.warn = False

	return ''

def __if_success(cmd, y, n):
	cmd = cmd.split()

	try:
		res = run(cmd, stdout=DEVNULL, stderr=DEVNULL)
	except:
		return n

	if res.returncode == 0:
		return y
	else:
		return n

def success(kconf, name, cmd):
	return __if_success(cmd, 'y', 'n')

def failure(kconf, name, cmd):
	return __if_success(cmd, 'n', 'y')

def error_if(kconf, name, cond, mas):
	if cond == 'y':
		die_kconf(kconf, mas)
	return ''

def __if_exist(name, y, n):
	if path.exists(name):
		return y
	else:
		return n

def exist(kconf, name, file):
	return __if_exist(file, 'y', 'n')

def not_exist(kconf, name, file):
	return __if_exist(file, 'n', 'y')

def __if_less(v1, v2, y, n):
	if int(v1) < int(v2):
		return y
	else:
		return n

def less(kconf, name, v1, v2):
	return __if_less(v1, v2, 'y', 'n')

def greater(kconf, name, v1, v2):
	return __if_less(v1, v2, 'n', 'y')

def pg_info(kconf, name):
	name_path = path.join(srctree, 'NAME')
	version_path = path.join(srctree, 'VERSION')

	with open(name_path, 'r') as file:
		name = file.readline().rstrip().lower()

	with open(version_path, 'r') as file:
		version = file.readline().rstrip()

	arch = platform.machine().lower()
	build = platform.system()

	return f"{name}\t{version}\t{arch}\t{build}"

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
		return ''

	seg = res.stdout.split()
	name = seg[0]
	version = int(seg[1]) * 1000 + int(seg[2])
	minver = (9 if name == 'GCC' else 6) * 1000

	return f"{name}\t{version}\t{minver}"

def cc_option(kconf, name, opt):
	pid = getpid()
	out = f".tmp_{pid}.o"

	cmd = [ cc, '-Werror', opt, '-c', '-x', 'c', os.devnull, '-o', out ]
	res = run(cmd, stdout=DEVNULL, stderr=DEVNULL)

	if path.isfile(out):
		unlink(out)

	if res.returncode == 0:
		return 'y'
	else:
		return 'n'

def ld_info(kconf, name):
	cmd = [ ld, '-v' ]
	res = run(cmd, text=True, capture_output=True)

	if not res.stdout:
		return ''

	seg = res.stdout.split()

	if seg[0] == 'GNU' and seg[1] == 'ld':
		# GNU ld (GNU Binutils for Ubuntu) 2.42
		name = 'BFD'
		version = seg[-1]

	elif seg[0] == 'GNU' and seg[1] == 'gold':
		return ''

	else:
		# Ubuntu LLD 18.1.3 (compatible with GNU linkers)
		# or
		# LLD 18.1.8 (compatible with GNU linkers)
		# or dumbfucks
		# Microsoft (R) Incremental Linker Version 14.40.33812.0
		while len(seg) > 1 and seg[0] != "LLD":
			seg.pop(0)

		if seg[0] != 'LLD':
			return ''

		name = 'LLD'
		version = seg[1]

	ver_seg = re.match(r'^[0-9.]*', version).group(0).split('.')
	version = int(ver_seg[0]) * 1000 + int(ver_seg[1])

	return f"{name}\t{version}"

def ld_option(kconf, name, opt):
	cmd = [ ld, '-v', opt ]
	res = run(cmd, stdout=DEVNULL, stderr=DEVNULL)

	if res.returncode == 0:
		return 'y'
	else:
		return 'n'

def __is_platform(name):
	if platform.system() == name:
		return 'y'
	else:
		return 'n'

def is_win32(kconf, name):
	return __is_platform('Windows')

def is_unix(kconf, name):
	if __is_platform('Windows') == 'n':
		return 'y'
	else:
		return 'n'

def is_empty(kconf, name, val):
	if not val.strip():
		return 'y'
	else:
		return 'n'

def to_lower(kconf, name, val):
	return val.lower()

def to_upper(kconf, name, val):
	return val.upper()

functions = {
	'word':      (word,      2, 2),
	'warn-off':  (warn_off,  0, 0),

	'lower':     (to_lower,  1, 1),
	'upper':     (to_upper,  1, 1),

	'WIN32':     (is_win32,  0, 0),
	'UNIX':      (is_unix,   0, 0),

	'success':   (success,   1, 1),
	'failure':   (failure,   1, 1),
	'error-if':  (error_if,  2, 2),

	'exist':     (exist,     1, 1),
	'not-exist': (not_exist, 1, 1),

	'empty':     (is_empty,  1, 1),
	'less':      (less,      2, 2),
	'greater':   (greater,   2, 2),

	'pg-info':   (pg_info,   0, 0),

	'cc-info':   (cc_info,   0, 0),
	'cc-option': (cc_option, 1, 1),

	'ld-info':   (ld_info,   0, 0),
	'ld-option': (ld_option, 1, 1),
}
