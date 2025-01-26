#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

if [[ ! $1 || ! $2 ]]; then
	>&2 echo licenses?
	exit 1
fi

if [[ ! $3 ]]; then
	git ls-files | xargs -n 10 bash $0 "$1" "$2"
	exit
fi

rep="SPDX-License-Identifier: $2"

for f in ${@:3}; do
	if [[ ! -f $f || $f =~ .ico$ ]]; then
		continue
	fi

	ln=$(head -n+2 $f)
	tmp=$f~
	n=2

	if ! grep -q "$1" <<< "$ln"; then
		continue
	fi

	>$tmp
	exec >> $tmp

	case $ln in
	'// '*)
		echo "// $rep"
		;;
	'/* '*)
		echo "/* $rep */"
		;;
	'#!'*)
		echo "${ln%%$'\n'*}"
		n=3
		;&
	'# '*)
		echo "# $rep"
		;;
	*)
		>&2 echo "unknown line format in $f"
		exit
		;;
	esac

	tail -n+$n $f

	mode=$(stat -c %a $f)

	mv $tmp $f
	chmod $mode $f
done
