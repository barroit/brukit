# SPDX-License-Identifier: GPL-3.0-or-later or MIT

cmake_minimum_required(VERSION 3.21)

set(ROOT ${CMAKE_SOURCE_DIR})
set(TOOL ${ROOT}/cmake)
set(GEN  ${ROOT}/include/generated)
set(ARCH ${ROOT}/arch)

set(name    $ENV{BSNAME})
set(arch    $ENV{BSARCH})
set(build   $ENV{BSBUILD})
set(version $ENV{BSVERSION})

include(${TOOL}/include)

include_scp(${TOOL}/kconfig)

if(NOT CONFIG_NO_ARCHSPEC AND EXISTS ${ROOT}/arch/generic)
  include_scp(${TOOL}/arch)
endif()

if(CONFIG_CC_IS_GCC)
  set(CMAKE_C_COMPILER gcc)
else()
  set(CMAKE_C_COMPILER clang)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CONFIG_DEBUG_INFO_NONE)
  add_compile_options(-g0)
elseif(CONFIG_DEBUG_INFO_EXTENDED)
  add_compile_options(-g3)
else()
  add_compile_options(-g)
endif()

if(CONFIG_OPTIMIZE_DEBUG)
  add_compile_options(-Og)
elseif(CONFIG_OPTIMIZE_O3)
  add_compile_options(-O3)
else()
  add_compile_options(-O)
endif()

add_compile_options(-Wall
		    -Wbad-function-cast
		    -Wformat-security
		    -Wformat-y2k
		    -Winit-self
		    -Wmissing-declarations
		    -Wmissing-prototypes
		    -Wnested-externs
		    -Wno-system-headers
		    -Wold-style-definition
		    -Wpacked
		    -Wstrict-prototypes
		    -Wswitch-enum
		    -Wshadow)

add_compile_options(-fshort-wchar
		    -funsigned-char
		    -fno-common)

test_add_compile_options(CONFIG_DEBUG_ON -fno-omit-frame-pointer)

test_add_compile_options(CONFIG_USE_ASAN -fsanitize=address)
test_add_link_options(CONFIG_USE_ASAN -fsanitize=address)

test_add_compile_options(CONFIG_USE_UBSAN -fsanitize=undefined)
test_add_link_options(CONFIG_USE_UBSAN -fsanitize=undefined)

test_add_compile_options(CONFIG_USE_TSAN -fsanitize=thread)
test_add_link_options(CONFIG_USE_TSAN -fsanitize=thread)

test_add_compile_options(CONFIG_USE_MSAN -fsanitize=memory)
test_add_link_options(CONFIG_USE_MSAN -fsanitize=memory)

# ASAN library lacks certain C library functions.
if(WIN32 AND CONFIG_USE_ASAN)
  add_compile_options(-U_DLL)
endif()

string(REPLACE -rc . version_cmake ${version})
project(${name} VERSION ${version_cmake} LANGUAGES C)

if(CONFIG_EXPORT_COMPILE_COMMAND)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

configure_file(${ROOT}/include/program.h.in ${GEN}/program.h)
