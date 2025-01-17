# SPDX-License-Identifier: GPL-3.0-or-later or MIT

include(CTest)

set(TEST ${TREE}/tests)
set(cmd bash ${TEST}/libtest.sh ${TREE}/scripts/libutil.sh)

file(GLOB prog_test *.sh)
list(FILTER prog_test EXCLUDE REGEX "(libtest)\\.sh")

foreach(name ${prog_test})
  get_filename_component(name ${name} NAME_WE)

  add_test(NAME cmd_${name} COMMAND ${cmd} ${TEST}/${name}.sh ../${exec_name})
endforeach()

if(NOT CONFIG_WIDE_CHAR)
  add_subdirectory(param)
endif()

add_subdirectory(unit)
