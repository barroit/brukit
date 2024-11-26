#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

checkpatch()
{
	checkpatch.pl --no-tree -q --file $*
}

checkpatch --ignore SPDX_LICENSE_TAG scripts/init.sh
checkpatch $(find scripts -type f ! -name init.sh)

checkpatch commands/* \
	   darwin/* \
	   freebsd/* \
	   linux/* \
	   platfree/* \
	   unix/* \
	   win32/*

checkpatch $(find lib -type f ! -name 'Kconfig.*')

checkpatch --ignore CONFIG_DESCRIPTION lib/Kconfig.*

ignore=(attr.h types.h tercol.h path.h compiler.h)
checkpatch $(find include -maxdepth 1 -type f ${ignore[@]/#/! -name })

checkpatch --ignore MACRO_ARG_UNUSED,PREFER_DEFINED_ATTRIBUTE_MACRO \
	   include/attr.h

checkpatch --ignore NEW_TYPEDEFS include/types.h

checkpatch --ignore COMPLEX_MACRO include/tercol.h include/path.h

checkpatch --ignore MACRO_ARG_UNUSED include/compiler.h
