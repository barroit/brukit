# SPDX-License-Identifier: GPL-3.0-or-later or MIT

RED='\033[0;31m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
RESET='\033[0m'

__log_err()
{
	while getopts 'e' opt; do
		case $opt in 
		e)
			use_cat=1
		esac
	done

	if [[ $use_cat ]]; then
		>&2 cat
	else
		>&2 echo $@
	fi
}

die()
{
	>&2 echo -en "${RED}fatal${RESET}: "
	__log_err $@
	exit 128
}

error()
{
	>&2 echo -en "${RED}error${RESET}: "
	__log_err $@
	return 1
}

warn()
{
	>&2 echo -en "${YELLOW}warning${RESET}: "
	__log_err $@
	return 1
}
