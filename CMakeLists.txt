# SPDX-License-Identifier: GPL-3.0-or-later or MIT

cmake_minimum_required(VERSION 3.21)

# Never use relative paths. In certain scenarios, such as when the tree is
# under z:, a relative path may cause CMake to misinterpret it, leading to
# unexpected behavior.
set(TREE $ENV{TREE})
set(LIB  ${TREE}/lib)
set(GEN  ${TREE}/include/generated)

file(MAKE_DIRECTORY ${GEN})

include(${LIB}/cmake.include)

if(NOT EXISTS ${TREE}/.config)
  set(ENV{KCONFIG_CONFIG} ${TREE}/.config.def)
  if(NOT EXISTS ${TREE}/.config.def)
    python3(${TREE}/scripts/kconfig.py alldefconfig)
  endif()
endif()

python3(${TREE}/scripts/kconfig.py genconfig --header-path ${GEN}/autoconf.h)
include_scp(${LIB}/cmake.kconfig)

set(prog_name ${CONFIG_PROG_NAME})
set(exec_name ${CONFIG_EXEC_NAME})
set(prog_version ${CONFIG_PROG_VERSION})

if(CONFIG_ARCH_SPEC)
  include_scp(${LIB}/cmake.arch)
endif()

set(CMAKE_C_COMPILER $ENV{CC})
if($ENV{LD} STREQUAL ld.lld)
  add_link_options(-fuse-ld=lld)
else()
  add_link_options(-fuse-ld=$ENV{LD})
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CONFIG_DEBUG_INFO_DWARF4)
  add_compile_options(-gdwarf-4)
elseif(CONFIG_DEBUG_INFO_DWARF5)
  add_compile_options(-gdwarf-5)
endif()

if(CONFIG_CC_OPTIMIZE_PERF)
  add_compile_options(-O2)
elseif(CONFIG_CC_OPTIMIZE_SIZE)
  add_compile_options(-Os)
elseif(CONFIG_CC_OPTIMIZE_O3)
  add_compile_options(-O3)
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

test_add_compile_options(CONFIG_DEBUG_PROG -fno-omit-frame-pointer)

test_add_compile_options(CONFIG_SANITIZE_PROG -fsanitize=address,undefined)
test_add_link_options(CONFIG_SANITIZE_PROG -fsanitize=address,undefined)

# ASAN library lacks certain C library functions.
if(WIN32 AND CONFIG_SANITIZE_PROG)
  add_compile_options(-U_DLL)
endif()

string(REPLACE -rc . prog_version_cmake ${prog_version})
project(${prog_name} VERSION ${prog_version_cmake} LANGUAGES C)

if(CONFIG_DUMP_COMPILE_CMD)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

configure_file(${TREE}/include/program.h.in ${GEN}/program.h)

cc_include(${GEN}/autoconf.h)
cc_include(${GEN}/program.h)
# cc_include(${TREE}/include/attrdecl.h)

include_directories(${TREE}/include)

if(UNIX)
  list(APPEND platlist unix)
  if(LINUX)
    list(APPEND platlist linux)
  elseif(BSD)
    list(APPEND platlist bsd)
  elseif(APPLE)
    list(APPEND platlist darwin)
  endif()
elseif(WIN32)
  list(APPEND platlist win32)
endif()

foreach(plat ${platlist})
  include_directories(${TREE}/${plat})
  list(APPEND pathspec ${TREE}/${plat}/*.c)
endforeach()

file(GLOB source LIST_DIRECTORIES false ${pathspec})
list(FILTER source EXCLUDE REGEX main\.)

add_library(platspec OBJECT ${source})

file(GLOB source LIST_DIRECTORIES false ${TREE}/platfree/*.c
					${TREE}/commands/*.c)
add_library(platfree OBJECT ${source})

if(UNIX)
  set(source ${TREE}/unix/main.c)
elseif(WIN32)
  if(CONFIG_WIN_GUI)
    set(source WIN32)
    list(APPEND source ${TREE}/win32/main.gui.c)
  else()
    set(source ${TREE}/win32/main.tui.c)
  endif()
  include_scp(${LIB}/cmake.manifest)
  list(APPEND source ${TREE}/${prog_name}.rc)
endif()

add_executable(${exec_name} ${source})

target_link_libraries(${exec_name} platfree platspec)

if(CONFIG_I18N_SUPP)
  find_package(Intl)
  if(Intl_FOUND)
    include_scp(${LIB}/cmake.gettext)
  endif()
endif()

if(WIN32)
  target_link_libraries(${exec_name} shlwapi)
  if(CONFIG_WIN_GUI)
    target_link_libraries(${exec_name} comctl32)
  endif()
  if(CONFIG_DEBUG_PROG AND CONFIG_REPORT_HOOK)
    target_link_libraries(${exec_name} dbghelp)
  endif()
endif()

add_compile_definitions(_GNU_SOURCE)

if(NOT CONFIG_DEBUG_PROG)
  add_compile_definitions(NDEBUG)
endif()

if(WIN32)
  add_compile_definitions(NTDDI_WIN7
			  WINVER=_WIN32_WINNT_WIN7
			  _WIN32_WINNT=_WIN32_WINNT_WIN7
			  _CRT_SECURE_NO_WARNINGS)
endif()

install(TARGETS ${exec_name} DESTINATION .)

if(CONFIG_I18N_SUPP)
  install(DIRECTORY ${TREE}/locale/ DESTINATION locale
	  FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)
endif()

if(WIN32)
  file(GLOB dll ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dll} DESTINATION .)
endif()
