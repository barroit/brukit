# SPDX-License-Identifier: GPL-3.0-or-later or MIT

cmake_minimum_required(VERSION 3.21)

set(srctree $ENV{SRCTREE})
set(objtree $ENV{OBJTREE})
set(gendir  $ENV{GENDIR})

set(SANFLAGS -fsanitize=address,undefined)

file(MAKE_DIRECTORY ${gendir})

include(lib/cmake.include)

include_scp(lib/cmake.kconfig)

include_scp(lib/cmake.features)
include(${objtree}/features.cmake)

set(ENV{CC} ${CONFIG_CC_COMMAND})
set(ENV{LD} ${CONFIG_LD_COMMAND})

set(prog_name    ${CONFIG_PROG_NAME})
set(exec_name    ${CONFIG_EXEC_NAME})
set(prog_version ${CONFIG_PROG_VERSION})

if(CONFIG_ENABLE_ARCH_SPEC)
  include_scp(lib/cmake.arch)
  file(CREATE_LINK ../arch/${arch} include/arch SYMBOLIC)
endif()

set(CMAKE_C_COMPILER $ENV{CC})
string(TOLOWER ${CONFIG_LD_NAME} ld)
add_link_options(-fuse-ld=${ld})

if(WIN32)
  add_link_options(-Xlinker /ignore:longsections)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS true)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CONFIG_DEBUG_INFO_DWARF4)
  add_compile_options(-gdwarf-4)
elseif(CONFIG_DEBUG_INFO_DWARF5)
  add_compile_options(-gdwarf-5)
endif()

if(CONFIG_CC_OPTIMIZE_NONE)
  add_compile_options(-O0)
elseif(CONFIG_CC_OPTIMIZE_PERF)
  add_compile_options(-O2)
elseif(CONFIG_CC_OPTIMIZE_O3)
  add_compile_options(-O3)
endif()

add_compile_options(-Wall
		    -Wbad-function-cast
		    -Wformat-security
		    -Winit-self
		    -Wmissing-declarations
		    -Wmissing-prototypes
		    -Wnested-externs
		    -Wno-system-headers
		    -Wold-style-definition
		    -Wpacked
		    -Wstrict-prototypes
		    -Wno-switch
		    -Wshadow)

add_compile_options(-funsigned-char -fno-common)

if(CONFIG_ENABLE_DEBUG)
  add_compile_options(-fno-omit-frame-pointer)
endif()

if(CONFIG_ENABLE_SANITIZER)
  add_link_options(${SANFLAGS})
endif()

# ASAN library lacks certain C library functions.
if(WIN32 AND CONFIG_ENABLE_SANITIZER)
  add_compile_options(-U_DLL)
endif()

# Do this or compiler ignores flags
get_directory_property(CLFAGS COMPILE_OPTIONS)
set(COMPILE_FLAGS ${CLFAGS})

string(REPLACE -rc . prog_version_cmake ${prog_version})
project(${prog_name} VERSION ${prog_version_cmake} LANGUAGES C)

if(CONFIG_DUMP_COMPILE_COMMAND)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

if(CONFIG_ENABLE_I18N)
  include_scp(lib/cmake.gettext)
endif()

configure_file(include/program.h.in ${gendir}/program.h)

cc_include(${gendir}/autoconf.h)
cc_include(${gendir}/features.h)
cc_include(${gendir}/program.h)

include_directories(include)

if(UNIX)
  list(APPEND platlist unix)
  if(LINUX)
    list(APPEND platlist linux)
  elseif(BSD AND CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
    list(APPEND platlist freebsd)
  elseif(APPLE AND CMAKE_SYSTEM_NAME STREQUAL Darwin)
    list(APPEND platlist darwin)
  endif()
elseif(WIN32)
  list(APPEND platlist win32)
endif()

foreach(plat ${platlist})
  include_directories(${plat})
  list(APPEND pspec_path ${plat}/*.c)
endforeach()

file(GLOB pspec_src LIST_DIRECTORIES false ${pspec_path})
list(FILTER pspec_src EXCLUDE REGEX main\.)
if(WIN32 AND NOT CONFIG_ENABLE_GUI)
  list(FILTER pspec_src EXCLUDE REGEX \\.gui\\.c$)
endif()
add_library(platspec OBJECT ${pspec_src})
add_dependencies(platspec gen-autoconf gen-features)

file(GLOB pfree_src LIST_DIRECTORIES false platfree/*.c)
if(NOT Intl_FOUND)
  list(FILTER pfree_src EXCLUDE REGEX gettext\.c)
endif()
add_library(platfree OBJECT ${pfree_src})
add_dependencies(platfree gen-autoconf gen-features)

file(GLOB cmd_src LIST_DIRECTORIES false commands/*.c)
add_library(command OBJECT ${cmd_src})
add_dependencies(command gen-autoconf gen-features)

if(CONFIG_ENABLE_SANITIZER)
  target_compile_options(platfree PRIVATE ${SANFLAGS})
  target_compile_options(platspec PRIVATE ${SANFLAGS})
  target_compile_options(command  PRIVATE ${SANFLAGS})
endif()

if(UNIX)
  set(source unix/main.c)
elseif(WIN32)
  if(CONFIG_ENABLE_GUI)
    set(source WIN32)
    list(APPEND source win32/main.gui.c)
  else()
    set(source win32/main.tui.c)
  endif()
  include_scp(lib/cmake.manifest)
  list(APPEND source ${prog_name}.rc)
endif()

link_libraries(platfree platspec)

if(WIN32)
  link_libraries(shlwapi userenv)
  if(CONFIG_ENABLE_GUI)
    link_libraries(comctl32)
  endif()
endif()

add_executable(${exec_name} ${source})
add_dependencies(${exec_name} gen-autoconf gen-features)
target_link_libraries(${exec_name} PRIVATE command)

if(CONFIG_ENABLE_SANITIZER)
  target_compile_options(${exec_name} PRIVATE ${SANFLAGS})
endif()

add_compile_definitions(_GNU_SOURCE)

if(NOT CONFIG_ENABLE_DEBUG)
  add_compile_definitions(NDEBUG)
endif()

if(WIN32)
  add_compile_definitions(NTDDI_WIN7
			  WINVER=_WIN32_WINNT_WIN7
			  _WIN32_WINNT=_WIN32_WINNT_WIN7)

  add_compile_definitions(WIN32_LEAN_AND_MEAN)

  # See https://learn.microsoft.com/en-us/cpp/c-runtime-library/compatibility.
  add_compile_definitions(_CRT_NONSTDC_NO_WARNINGS
			  _CRT_DECLARE_NONSTDC_NAMES
			  _CRT_SECURE_NO_WARNINGS)
endif()

install(TARGETS ${exec_name} DESTINATION .)

if(CONFIG_ENABLE_I18N)
  install(DIRECTORY locale DESTINATION locale
	  FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)
endif()

if(WIN32)
  file(GLOB dll ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dll} DESTINATION .)
endif()

if(CONFIG_ENABLE_TEST)
  add_subdirectory(tests)
endif()
