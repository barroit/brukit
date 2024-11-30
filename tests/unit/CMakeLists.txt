# SPDX-License-Identifier: GPL-3.0-or-later or MIT

if(CONFIG_CC_HAS_TOPLEVEL_REORDER)
  add_compile_options(-fno-toplevel-reorder)
endif()

set(UT_PTH_TMP ${CMAKE_CURRENT_BINARY_DIR}/.tmp)
file(MAKE_DIRECTORY ${UT_PTH_TMP})
configure_file(unitest.h.in ${CMAKE_CURRENT_SOURCE_DIR}/unitest.h)

add_executable(test_path path.c main.c)
add_executable(test_strbuf strbuf.c main.c)

add_test(NAME api_path COMMAND test_path)
add_test(NAME api_strbuf COMMAND test_strbuf)
