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

export USRCONF := $(srctree)/.config.unix
export DEFCONF := $(USRCONF).1

ifneq ($(wildcard $(USRCONF)),)
DOTCONF := $(USRCONF)
else
DOTCONF := $(DEFCONF)
GEN_DEFCONF := gen-defconf
endif

export DOTCONF
export KCONFIG_CONFIG := $(DOTCONF)

ifneq ($(wildcard $(USRCONF)),)
ifneq ($(wildcard $(DEFCONF)),)
RECONFIGURE := configure
RM_DEFCONF  := rm-defconf
else
RERECONFDEP := reconfdep
endif
endif

export RECONFDEP := $(objtree)/reconfdep

CMAKE_CC_FEATURE := $(objtree)/features.cmake

NOPYC := PYTHONDONTWRITEBYTECODE=y

build:

.PHONY: menuconfig gen-defconf rm-defconf \
	reconfdep configure dotplat build all

menuconfig:
	@$(NOPYC) scripts/kconfig.py menuconfig

$(gendir):
	@mkdir $@

$(CMAKE_CC_FEATURE): $(gendir)
	@$(NOPYC) scripts/cc-feature.py cmake

gen-defconf:
	@$(NOPYC) scripts/kconfig.py alldefconfig

rm-defconf:
	@rm $(DEFCONF)

reconfdep: $(GEN_DEFCONF) $(RM_DEFCONF)
	@$(NOPYC) scripts/reconfdep.py $(DOTCONF) $(RECONFDEP)

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
	@rm -f $(USRCONF)*
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
