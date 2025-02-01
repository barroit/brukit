# SPDX-License-Identifier: GPL-3.0-or-later or MIT

cmake_minimum_required(VERSION 3.21)

set(TREE $ENV{TREE})
set(GEN  ${TREE}/include/generated)
set(BIN  ${CMAKE_BINARY_DIR})

file(MAKE_DIRECTORY ${GEN})

include(lib/cmake.include)

if(NOT EXISTS ${TREE}/.config)
  set(ENV{KCONFIG_CONFIG} ${TREE}/.config.def)
  if(NOT EXISTS ${TREE}/.config.def)
    python3(scripts/kconfig.py alldefconfig)
  endif()
endif()

python3(scripts/kconfig.py genconfig --header-path ${GEN}/autoconf.h)
include_scp(lib/cmake.kconfig)

if(EXISTS ${TREE}/.config)
  set(ENV{CC} ${CONFIG_CC_COMMAND})
  set(ENV{LD} ${CONFIG_LD_COMMAND})
endif()

set(prog_name ${CONFIG_PROG_NAME})
if(CONFIG_SPEC_EXEC_NAME)
  set(exec_name ${CONFIG_EXEC_NAME})
else()
  set(exec_name ${CONFIG_PROG_NAME})
endif()
set(prog_version ${CONFIG_PROG_VERSION})

if(CONFIG_ARCH_SPEC)
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

if(CONFIG_DEBUG_PROG)
  add_compile_options(-fno-omit-frame-pointer)
endif()

# ASAN library lacks certain C library functions.
if(WIN32 AND CONFIG_PROG_WITH_SANITIZER)
  add_compile_options(-U_DLL)
endif()

string(REPLACE -rc . prog_version_cmake ${prog_version})
project(${prog_name} VERSION ${prog_version_cmake} LANGUAGES C)

if(CONFIG_DUMP_CPL_CMD)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

include_scp(lib/cmake.feature)

configure_file(include/program.h.in ${GEN}/program.h)

cc_include(${GEN}/autoconf.h)
cc_include(${GEN}/features.h)
cc_include(${GEN}/program.h)

include_directories(include)

if(UNIX)
  list(APPEND platlist unix)
  if(LINUX)
    list(APPEND platlist linux)
  elseif(BSD AND CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
    list(APPEND platlist freebsd)
  elseif(APPLE AND CMAKE_SYSTEM_NAME STREQUAL macOS)
    list(APPEND platlist darwin)
    add_compile_definitions(__Darwin__)
  endif()
elseif(WIN32)
  list(APPEND platlist win32)
endif()

foreach(plat ${platlist})
  include_directories(${plat})
  list(APPEND pathspec ${plat}/*.c)
endforeach()

file(GLOB source LIST_DIRECTORIES false ${pathspec})
list(FILTER source EXCLUDE REGEX main\.)

add_library(platspec OBJECT ${source})

file(GLOB source LIST_DIRECTORIES false platfree/*.c commands/*.c)
add_library(platfree OBJECT ${source})

if(CONFIG_PROG_WITH_SANITIZER)
  target_compile_options(platfree PRIVATE -fsanitize=address,undefined)
  target_compile_options(platspec PRIVATE -fsanitize=address,undefined)
  add_link_options(-fsanitize=address,undefined)
endif()

if(UNIX)
  set(source unix/main.c)
elseif(WIN32)
  if(CONFIG_WIN_GUI)
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
  if(CONFIG_WIN_GUI)
    link_libraries(comctl32)
  endif()
  if(CONFIG_DEBUG_PROG AND CONFIG_REP_HOOK)
    link_libraries(dbghelp)
  endif()
endif()

add_executable(${exec_name} ${source})

if(CONFIG_I18N_SUPP)
  find_package(Intl)
  if(Intl_FOUND)
    include_scp(lib/cmake.gettext)
  endif()
endif()

add_compile_definitions(_GNU_SOURCE)

if(NOT CONFIG_DEBUG_PROG)
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

if(CONFIG_I18N_SUPP)
  install(DIRECTORY locale DESTINATION locale
	  FILES_MATCHING PATTERN *.mo PERMISSIONS OWNER_READ)
endif()

if(WIN32)
  file(GLOB dll ${CMAKE_BINARY_DIR}/*.dll)
  install(FILES ${dll} DESTINATION .)
endif()

if(CONFIG_ENABLE_TESTING)
  add_subdirectory(tests)
endif()
