# SPDX-License-Identifier: GPL-3.0-or-later or MIT

source ../scripts/libutil.sh

assert_success()
{
	if [[ $? != 0 ]]; then
		error $*
	fi
}

source $1
