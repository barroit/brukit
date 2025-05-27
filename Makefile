# SPDX-License-Identifier: GPL-3.0-or-later or MIT

ifneq ($(filter extra-prereqs,$(.FEATURES)),extra-prereqs)
$(error GNU Make >= 4.3 is required. Your Make version is $(MAKE_VERSION))
endif

MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

srctree := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
gendir  := $(srctree)/include/generated
objtree := $(srctree)/build.unix

generator := Ninja

export SRCTREE := $(srctree)
export GENDIR  := $(gendir)
export OBJTREE := $(objtree)

export CC := gcc
export LD := ld.bfd

ifneq ($(LLVM),)
export CC := clang
export LD := ld.lld
endif

ifneq ($(MAKEFILE),)
generator := Unix Makefiles
endif

export DOTPLAT := $(srctree)/.platform

export DOTCONFIG := $(srctree)/.config.unix
export DEFCONFIG := $(DOTCONFIG).def

ifneq ($(wildcard $(DOTCONFIG)),)
RELCONFIG := $(DOTCONFIG)
else
RELCONFIG    := $(DEFCONFIG)
MK_DEFCONFIG := mk_defconfig
endif

export RELCONFIG
export KCONFIG_CONFIG := $(RELCONFIG)

ifneq ($(wildcard $(DOTCONFIG)),)
ifneq ($(wildcard $(DEFCONFIG)),)
RECONFIGURE  := configure
RM_DEFCONFIG := rm_defconfig
else
RERECONFDEP  := reconfdep
endif
endif

export RECONFDEP := $(objtree)/reconfdep

CMAKE_CC_FEATURE := $(objtree)/features.cmake

NOPYC := PYTHONDONTWRITEBYTECODE=y

build:

.PHONY: menuconfig mk_defconfig rm_defconfig \
	reconfdep configure dotplat build all

menuconfig:
	@$(NOPYC) scripts/kconfig.py menuconfig

$(gendir):
	@mkdir $@

$(CMAKE_CC_FEATURE): $(gendir)
	@$(NOPYC) scripts/cc-feature.py cmake

mk_defconfig:
	@$(NOPYC) scripts/kconfig.py alldefconfig

rm_defconfig:
	@rm $(DEFCONFIG)

reconfdep: $(MK_DEFCONFIG) $(RM_DEFCONFIG)
	@$(NOPYC) scripts/reconfdep.py $(RELCONFIG) $(RECONFDEP)

configure: $(CMAKE_CC_FEATURE) reconfdep
	@cmake -G "$(generator)" -S . -B $(objtree) $(EXTOPT)

dotplat:
	@if [ -f $(objtree)/CMakeCache.txt ] &&			\
	    [ "$$(cat $(DOTPLAT) 2>&1 )" != unix ]; then	\
		echo unix > $(DOTPLAT);			\
	fi

build: dotplat $(RECONFIGURE) $(RERECONFDEP)
	@cmake --build $(objtree) --parallel

all: configure build

.PHONY: clean distclean

clean:
	@cmake --build $(objtree) --target clean

distclean:
	@rm -rf include/generated
	@rm -f include/arch
	@rm -f $(DOTCONFIG)*
	@rm -f $(DOTPLAT)
	@git ls-files --directory -o $(objtree) | xargs rm -rf

__tests := $(wildcard $(objtree)/t/*.t)
tests   := $(patsubst $(objtree)/%,%,$(__tests))

.PHONY: $(tests)

$(tests):
	@$(objtree)/$@

.PHONY: t/all

t/all:
	@ctest --test-dir $(objtree)/tests --parallel $(shell nproc)

scripts := $(wildcard scripts/*.sh) $(wildcard scripts/*.py)
args    := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

export $(NOPYC)

.PHONY: $(args)

$(args):
	@:

.PHONY: $(scripts)

$(scripts):
	@./$@ $(args)
