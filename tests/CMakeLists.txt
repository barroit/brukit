# SPDX-License-Identifier: GPL-3.0-or-later or MIT

include(CTest)

if(UNIX)
  set(sh /bin/sh)
elseif(WIN32)
  include_scp(${srctree}/lib/cmake.sh)
  set(sh ${sh_exe})
endif()

set(testdir ${srctree}/tests)
set(testcmd "${sh}" ${testdir}/exec.sh)

file(GLOB prog_test *.sh)
list(FILTER prog_test EXCLUDE REGEX "(exec)\\.sh")

foreach(name ${prog_test})
  get_filename_component(name ${name} NAME_WE)

  add_test(NAME cmd_${name}
	   COMMAND ${testcmd} ${testdir}/${name}.sh ${objtree}/${exec_name}
	   WORKING_DIRECTORY ${srctree})
endforeach()

add_subdirectory(param)
add_subdirectory(unit)

add_executable(masarg masarg.c)
