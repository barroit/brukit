# SPDX-License-Identifier: GPL-3.0-or-later or MIT

add_executable(test_path path.c)
add_executable(test_strbuf strbuf.c)

link_libraries(platfree platspec)

add_test(NAME api_path COMMAND test_path)
add_test(NAME api_strbuf COMMAND test_strbuf)
