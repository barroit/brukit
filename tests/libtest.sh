# SPDX-License-Identifier: GPL-3.0-or-later or MIT

declare -A pp

__u=$1
__t=$2
__e=$3

out=$$.stdout
err=$$.stderr

ret=0

touch $out $err
trap 'rm $out $err' EXIT

source $__u

run()
{
	pp=()

	>$err >$out
	$__e "$@" 2>$err >$out &

	pid=$!

	wait $pid
	while read; do
		echo "$pid> $REPLY"
	done < <(cat $err)

	__pp=$(grep -P '^39\t' $out)

	if [[ -z $__pp ]]; then
		return
	fi

	while read; do
		k=$(cut -f2 <<< $REPLY)
		v=$(cut -f3- <<< $REPLY)

		pp[$k]=$v
	done <<< $__pp
}

PSSERT_OPTEQUAL()
{
	opt=$1
	str=${@:2}

	if [[ -v pp[$opt] ]]; then
		if [[ ${pp[$opt]} == $str ]]; then
			return 0
		else
			error -e <<-EOF
			option '$opt' doesn't match target
			a: $str
			b: ${pp[$opt]}
			EOF
		fi
	else
		error "option '$opt' missing in stdout"
	fi

	(( ret += 1 ))
}

PSSERT_ERRLIKE()
{
	if ! grep -q -- "$*" $err; then
		error "stderr doesn't like target"
		diff <(echo $*) $err
		(( ret += 1 ))
	fi
}

PSSERT_OUTLIKE()
{
	if ! grep -q -- "$*" $out; then
		error "stdout doesn't like target"
		diff <(echo $*) $out
		(( ret += 1 ))
	fi
}

source $__t

if [[ $ret -eq 0 ]]; then
	exit 0
else
	exit 39
fi
