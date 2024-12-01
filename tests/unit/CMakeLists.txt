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

unitest(path path.c)
unitest(strbuf strbuf.c)
unitest(init init.c)
