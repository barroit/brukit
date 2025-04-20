#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-or-later or MIT

# To quick set up libkit.sh in your project
#   1 - symlink scripts/libkit.sh against path/to/this/root/current/libkit.sh
#   2 - symlink scripts/use-libkit.sh against this script
#   3 - source scripts/use-libkit.sh in other scripts

libkit=scripts/libkit.sh
libkit=$(perl -e 'use Cwd "abs_path"; print abs_path(shift); "\n"' $libkit)
LIBKIT_ROOT=$(dirname $libkit)

. $libkit
