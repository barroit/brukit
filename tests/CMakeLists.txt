# SPDX-License-Identifier: GPL-3.0-or-later or MIT

function(add_cmd_test name file)
  add_test(NAME ${name}
	   COMMAND bash libtest.sh ${file} WORKING_DIRECTORY ../../tests)
endfunction()

include(CTest)

add_subdirectory(unit)

add_cmd_test(version version.sh)
