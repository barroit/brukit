#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

. scripts/use-posix-libkit.sh

if [ ! -f .consumer.in ]; then
	die 'missing .consumer.in'
fi

rm -f VERSION
printf '0.0\n' >VERSION

ini_section_st name .consumer.in >NAME

name=$(readlower <NAME)
conf=$(ini_section conf .consumer.in)
icon=$(ini_section icon .consumer.in)
repo=$(ini_section_st repo .consumer.in)

git remote set-url origin $repo

fhc_key='"fileHeaderComment.parameter"."*"'
license_new=$(ini_section_st license .consumer.in)
license_old=$(jq -r ".$fhc_key.license" .vscode/settings.json)

jq ".$fhc_key.license = \"$license_new\"" .vscode/settings.json >$$.tmp
mv $$.tmp .vscode/settings.json

perl -i -ne 'last if /^# みくみくにしてあげる♪$/; print' .gitignore
printf '%s\n' "$(cat .gitignore)" >.gitignore

ini_section readme .consumer.in >README

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
scripts/init-consumer.sh
EOF

if [ "$license_new" != "$license_old" ]; then
	cat <<-EOF >.licensefix
	new	$license_new
	old	$license_old
	EOF

	if [ "$license_new" = GPL-3.0-or-later ]; then
		rm LICENSES/MIT
	elif [ "$license_new" = MIT ]; then
		rm LICENSES/GPL-3.0-or-later
	else
		die "unknown license '$license_new'"
	fi

	git add .
	scripts/fix-license.sh "$license_old" "$license_new"
fi

rm .manifest.1
rm .consumer.in
rm brukit

(rm $0) &

scripts/setup-repo.sh

make scripts/bump-locale.sh || true

git add .
git commit -sm 'INITIAL CONSUMER'
