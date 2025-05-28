#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

. scripts/use-posix-libkit.sh

$LIBKIT_ROOT/../scripts/setup-repo.sh

ln -sf build.unix/tests/Testing/Temporary/LastTest.log tres.unix
ln -sf build.win32/tests/Testing/Temporary/LastTest.log tres.win32

ln -sf tres.unix tres
ln -sf build.unix build

if [ -f brukit ]; then
	ln -sf VERSION.1 VERSION
fi
