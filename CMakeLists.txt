# SPDX-License-Identifier: GPL-3.0-or-later or MIT

cmake_minimum_required(VERSION 3.21)

# Do not use relative paths. In certain scenarios, such as when the tree is
# under z:, a relative path may cause CMake to misinterpret it, leading to
# unexpected behavior.
set(TREE ${CMAKE_SOURCE_DIR})
set(TOOL ${TREE}/cmake)
set(GEN  ${TREE}/include/generated)
set(ARCH ${TREE}/arch)

set(name    $ENV{BSNAME})
set(version $ENV{BSVERSION})
set(arch    $ENV{BSARCH})
set(build   $ENV{BSBUILD})

include(${TOOL}/include)

include_scp(${TOOL}/kconfig)

if(NOT CONFIG_NO_ARCHSPEC AND EXISTS ${TREE}/arch/generic)
  include_scp(${TOOL}/arch)
endif()

if(CONFIG_CC_IS_GCC)
  set(CMAKE_C_COMPILER gcc)
  set(cc_min_ver 9)
else()
  set(CMAKE_C_COMPILER clang)
  set(cc_min_ver 6)
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

if(CMAKE_C_COMPILER_VERSION VERSION_LESS cc_min_ver)
  message(FATAL_ERROR "compiler version ${cc_min_ver} or higher is required")
endif()

if(CONFIG_EXPORT_COMPILE_COMMAND)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

configure_file(${TREE}/include/program.h.in ${GEN}/program.h)

cc_include(${GEN}/autoconf.h)
cc_include(${GEN}/program.h)

include_directories(${TREE}/include)

if(UNIX)
  list(APPEND plat unix)
  if(LINUX)
    list(APPEND plat linux)
  elseif(BSD)
    list(APPEND plat bsd)
  elseif(APPLE)
    list(APPEND plat darwin)
  endif()
elseif(WIN32)
  list(APPEND plat win32)
endif()

foreach(p ${plat})
  include_directories(${TREE}/${p})
endforeach()

foreach(p ${plat})
  list(APPEND platspec ${TREE}/${p}/*.c)
endforeach()

file(GLOB source LIST_DIRECTORIES false ${pathspec})
list(FILTER source EXCLUDE REGEX main\.)
add_library(platspec OBJECT ${source})

file(GLOB source LIST_DIRECTORIES false ${TREE}/generic/*.c
					${TREE}/commands/*.c)
add_library(platfree OBJECT ${source})

if(UNIX)
  set(source ${TREE}/unix/main.c)
elseif(WIN32)
  if(CONFIG_NO_GUI)
    set(source ${TREE}/win32/main.tui.c)
  else()
    set(source WIN32)
    list(APPEND source ${TREE}/win32/main.gui.c)
  endif()
  include_scp(${TOOL}/manifest)
  list(APPEND source ${TREE}/${name}.rc)
endif()

add_executable(${name} ${source})

target_link_libraries(${name} platfree platspec)

if(NOT CONFIG_NO_I18N)
  find_package(Intl)
  if(Intl_FOUND)
    include_scp(${TOOL}/gettext)
  endif()
endif()

if(WIN32)
  target_link_libraries(${name} shlwapi)
  if(NOT CONFIG_NO_GUI)
    target_link_libraries(${name} comctl32)
  endif()
  if(CONFIG_DEBUG_ON AND NOT CONFIG_NO_REPORT_HOOK)
    target_link_libraries(${name} dbghelp)
    have(RPHK)
  endif()
endif()

add_compile_definitions(_GNU_SOURCE)

if(NOT CONFIG_DEBUG_ON)
  add_compile_definitions(NDEBUG)
endif()

if(WIN32)
  add_compile_definitions(NTDDI_WIN7
			  WINVER=_WIN32_WINNT_WIN7
			  _WIN32_WINNT=_WIN32_WINNT_WIN7
			  _CRT_SECURE_NO_WARNINGS)
endif()

install(TARGETS ${name} DESTINATION .)

if(NOT CONFIG_NO_I18N)
  install(DIRECTORY ${TREE}/locale/ DESTINATION locale
	  FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)
endif()

if(WIN32)
  file(GLOB dll ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dll} DESTINATION .)
endif()
