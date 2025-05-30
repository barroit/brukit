#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

. scripts/use-posix-libkit.sh

if [ ! -f .program.in ]; then
	die 'missing .program.in'
fi

rm -f VERSION
printf '0.0\n' >VERSION

ini_section_st name .program.in >NAME

name=$(readlower <NAME)
conf=$(ini_section conf .program.in)
icon=$(ini_section icon .program.in)
repo=$(ini_section_st repo .program.in)

git remote set-url origin $repo

fhc_key='"fileHeaderComment.parameter"."*"'
license_new=$(ini_section_st license .program.in)
license_old=$(jq -r ".$fhc_key.license" .vscode/settings.json)

jq ".$fhc_key.license = \"$license_new\"" .vscode/settings.json >$$.tmp
mv $$.tmp .vscode/settings.json

perl -i -ne 'last if /^# みくみくにしてあげる♪$/; print' .gitignore
printf '%s\n' "$(cat .gitignore)" >.gitignore

ini_section readme .program.in >README

eval "$(cat .manifest.1)" >$name.manifest.in

cat <<EOF >$name.rc
// SPDX-License-Identifier: $license_new
#include "resource.h"

PROG_ICON	ICON	"$icon"
EOF

if ! printf '%s\n' "$conf" | grep -wq no_arch; then
	mkdir arch arch/x86 arch/arm64 arch/riscv

	cat <<-EOF >arch/.map
	riscv	riscv	riscv64
	arm64	arm64	aarch64
	x86	x86_64	amd64	x64
	EOF
fi

cat <<EOF >.pickignore
.manifest.1
.program.in.example
scripts/init-consumer.sh
EOF

if [ "$license_new" != "$license_old" ]; then
	cat <<-EOF >FIXLICENSE
	new	$license_new
	old	$license_old
	EOF

	git add .
	scripts/fix-license.sh "$license_old" "$license_new"
fi

rm .manifest.1
rm .program.in*
rm brukit

(rm $0) &

scripts/setup-repo.sh

make scripts/build-locale.sh

git add .
git commit -sm 'INITIAL CONSUMER'
