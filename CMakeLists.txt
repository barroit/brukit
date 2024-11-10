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
