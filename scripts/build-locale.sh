#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

. scripts/use-posix-libkit.sh

note 'preparing source files ...'

make configure EXTOPT='-DCMAKE_C_FLAGS="-C -DINTL_PREP_MO"' >/dev/null

cd build.unix

i=$(make help | grep '\.i$' | cut -d' ' -f2)

printf '%s\n' "$i" | xargs -P$(nproc) -n1 make >/dev/null

cd ..

domain=$(readlower <NAME)

cd locale

src=$(find ../build.unix/CMakeFiles -type f -name '*.i')

cat <<EOF >.lang.$$
zh_CN
ja_JP
EOF
trap 'rm .lang.$$' EXIT

xgettext --add-comments=TRANSLATORS --omit-header --no-location \
	 --from-code=UTF-8 -LC -i -k_ -kN_ -k__H_ -k__HN_ $src

if [ -n "$1" ]; then
	if grep -q $1 .lang.$$; then
		langs=$(grep $1 .lang.$$)
	else
		die "unknown language \`$1'"
	fi
else
	langs=$(cat .lang.$$)
fi

for lang in $langs; do
	msgmerge -i --no-location -U $lang.po messages.po
	mkdir -p $lang/LC_MESSAGES
	msgfmt -o $lang/LC_MESSAGES/$domain.mo $lang.po
done

cd ..

note 'cleaning up source files ...'

make configure EXTOPT='-DCMAKE_C_FLAGS=""' >/dev/null
