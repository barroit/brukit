#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set -e

echo 'preparing...'
make configure EXTOPT='-DCMAKE_C_FLAGS="-DINTL_PREP_MO"' >/dev/null

cd build

i=$(make help | grep '\.i$' | cut -d' ' -f2)

xargs -n1 -P$(nproc) make $@ <<< $i >/dev/null

cd ..

domain=$(grep name .program | cut -f2)

cd locale

src=$(find ../build/CMakeFiles -type f -name '*.i')
lang=(zh_CN ja_JP)

xgettext --omit-header \
	 --from-code=UTF-8 -LC -Ei -k_ -kN_ -k__H_ -k__HN_ $src

for l in ${lang[@]}; do
	msgmerge -Ei -U $l.po messages.po
	mkdir -p $l/LC_MESSAGES
	msgfmt -o $l/LC_MESSAGES/$domain.mo $l.po
done

cd ..

echo 'cleaning...'
make configure EXTOPT='-DCMAKE_C_FLAGS=""' >/dev/null
