#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

if [ ! -L scripts/use-posix-libkit.sh ]; then
	git apply scripts-add-libkit-py.patch
else
	../barroit/scripts/setup-repo.sh
	ln -sf ../../barroit/current/libkit.py scripts/libkit.py
fi

ln -sf build.unix/tests/Testing/Temporary/LastTest.log tres.unix
ln -sf tres.unix tres
ln -sf build.unix build

mkdir -p share
ln -snf ../locale share/locale

if [ -f brukit ]; then
	ln -sf VERSION.1 VERSION
fi
