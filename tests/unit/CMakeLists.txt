# SPDX-License-Identifier: GPL-3.0-or-later or MIT

function(unitest name)
  add_executable(test_${name} ${ARGN} ussert.c main.c)
  add_test(NAME api_${name} COMMAND test_${name})
endfunction()

if(CONFIG_CC_HAS_TOPLEVEL_REORDER)
  add_compile_options(-fno-toplevel-reorder)
endif()

set(UT_PTH_TMP ${CMAKE_CURRENT_BINARY_DIR}/.tmp)
file(MAKE_DIRECTORY ${UT_PTH_TMP})
configure_file(unitest.h.in ${CMAKE_CURRENT_SOURCE_DIR}/unitest.h)

unitest(path path.c)
unitest(strbuf strbuf.c)
