#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

from os import environ as env, path
from sys import argv

if len(argv) < 3:
	exit(128)

top = env['TOP']
build = env['BUILD']

sample_name = path.join(top, 'scripts', 'reconfdep.def')
dotconfig_name = argv[1]
reconfdep_name = argv[2]

if not path.exists(reconfdep_name):
	open(reconfdep_name, 'x').close()

sample = open(sample_name, 'r')
dotconfig = open(dotconfig_name, 'r')
reconfdep = open(reconfdep_name, 'r')

dotconf = { 'y': True }
confdep = { 'y': True }

exec(dotconfig.read(), {}, dotconf)
exec(reconfdep.read(), {}, confdep)

deps = sample.read().split('\n')
deps = [ dep for dep in deps if dep and dep[0] != '#' ]
update = 0

for dep in deps:
	if dep in dotconf:
		if dep in confdep and dotconf[dep] == confdep[dep]:
			continue

		confdep[dep] = dotconf[dep]
	elif dep in confdep:
		del confdep[dep]
	else:
		continue

	update = 1

if not update:
	exit(0)

del confdep['y']

reconfdep = open(reconfdep_name, 'r+')

head = '# Generated by scripts/reconfdep.py\n'
head_len = len(head)

if reconfdep.tell() == 0:
	reconfdep.write(head)
else:
	reconfdep.seek(head_len)

for dep in confdep:
	val = dotconf[dep]
	t = type(val)

	if t is bool:
		val = 'y'
	elif t is str:
		val = f"\"{val}\""

	reconfdep.write(f"\n{dep}={val}\n")

reconfdep.truncate()
