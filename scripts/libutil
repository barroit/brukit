# SPDX-License-Identifier: GPL-3.0-or-later or MIT

RED='\033[0;31m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
WHITE='\033[0;37m'
RESET='\033[0m'

die()
{
	>&2 echo -en "${RED}fatal${RESET}: "
	>&2 echo $*
	exit 128
}

error()
{
	>&2 echo -en "${RED}error${RESET}: "
	>&2 echo $*
	return 1
}

warn()
{
	>&2 echo -en "${YELLOW}warning${RESET}: "
	>&2 echo $*
	return 1
}
