#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

. scripts/use-posix-libkit.sh

script=$1
program=$2

out=$$.stdout
err=$$.stderr

errcnt=0

trap 'rm -f $out $err' EXIT

# Now, log() prints pid.
current()
{
	printf "$pid\n"
}

run()
{
	export CTEST_NAME=$program

	>$err >$out
	$program "$@" 2>$err >$out &

	pid=$!

	wait $pid

	while read line; do
		log "$line"
	done <$err
}

UA_OPTEQ()
{
	name=$1

	expect=$2
	actual=$(v2 $out $name)

	if [ "$actual" != "$expect" ]; then
		read -d '' lines <<-EOF
		option '$name' doesn't match target
		a: $expect
		b: $actual
		EOF

		error "$lines"

		(( errcnt += 1 ))
	fi
}

UA_ERRLIKE()
{
	if ! grep -Fq "$1" $err; then
		error "stderr doesn't like target"
		printf '%s\n' "$1" | diff - $err

		(( errcnt += 1 ))
	fi
}

UA_OUTLIKE()
{
	if ! grep -Fq "$1" $out; then
		error "stdout doesn't like target"
		printf '%s\n' "$1" | diff - $out

		(( errcnt += 1 ))
	fi
}

. $script

if [ $errcnt != '0' ]; then
	exit 39
fi

exit
