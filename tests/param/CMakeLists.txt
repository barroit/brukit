# SPDX-License-Identifier: GPL-3.0-or-later or MIT

set(prmsrc ${srctree}/tests/param)
set(prmobj ${objtree}/tests/param)

file(GLOB prms *.c)
list(FILTER prms EXCLUDE REGEX "(main)\\.c")

link_libraries(command)

foreach(name ${prms})
  get_filename_component(name ${name} NAME_WE)

  add_executable(${name}.p ${name}.c main.c)
  add_dependencies(${name}.p genconfig feature)

  add_test(NAME prm_${name}
	   COMMAND ${testcmd} ${prmsrc}/${name}.sh ${prmobj}/${name}.p
	   WORKING_DIRECTORY ${srctree})
endforeach()
