# SPDX-License-Identifier: GPL-3.0-or-later or MIT

function(unitest name)
  add_executable(${name}.t ${ARGN} ussert.c main.c)
  add_test(NAME api_${name} COMMAND ${name}.t)
endfunction()

if(CONFIG_CC_HAS_TOPLEVEL_REORDER)
  add_compile_options(-fno-toplevel-reorder)
endif()

set(pth_test_tmp ${CMAKE_CURRENT_BINARY_DIR}/.tmp)
file(MAKE_DIRECTORY ${pth_test_tmp})
configure_file(unitest.h.in ${CMAKE_CURRENT_SOURCE_DIR}/unitest.h)

file(GLOB tests *.c)
list(FILTER tests EXCLUDE REGEX "(main|ussert)\\.c")

foreach(name ${tests})
  get_filename_component(name ${name} NAME_WE)
  unitest(${name} ${name}.c)
endforeach()
