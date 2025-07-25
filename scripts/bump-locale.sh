#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT
#
# You must execute this script via:
#
#	make bump-locale.sh
#

set -e

. scripts/use-posix-libkit.sh

note 'preparing source files ...'

export MAKEFILE=1

make distclean
make configure CMAKEFLAGS='-DCMAKE_C_FLAGS="-E -C -DINTL_PREP_MO"' >/dev/null
make dotplat

cd build.unix
make gen-autoconf gen-features

dot_i=$(make help | grep '\.i$' | cut -d' ' -f2)

printf '%s\n' "$dot_i" | xargs -P$(nproc) -n1 make >/dev/null

cd ..

domain=$(readlower <NAME)

cd locale

src=$(find ../build.unix/CMakeFiles -type f -name '*.i')

cat <<EOF >.lang.$$
zh_CN
ja
EOF
trap "rm $PWD/.lang.$$" EXIT

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

make distclean
