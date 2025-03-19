# SPDX-License-Identifier: GPL-3.0-or-later or MIT

if(HAVE_TOPLEVEL_REORDER)
  add_compile_options(-fno-toplevel-reorder)
endif()

set(TEST_TMP ${CMAKE_CURRENT_BINARY_DIR}/.tmp)
file(MAKE_DIRECTORY ${TEST_TMP})
configure_file(unitest.h.in ${CMAKE_CURRENT_SOURCE_DIR}/unitest.h)

file(GLOB unit_tests *.c)
list(FILTER unit_tests EXCLUDE REGEX "(main|ussert)\\.c")

foreach(name ${unit_tests})
  get_filename_component(name ${name} NAME_WE)

  add_executable(${name}.t ${name}.c ussert.c main.c)
  add_dependencies(${name}.t genconfig feature)

  add_test(NAME api_${name} COMMAND ${name}.t WORKING_DIRECTORY ${BUILD})
endforeach()
