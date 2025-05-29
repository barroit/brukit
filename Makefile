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
export DEPCONF := $(objtree)/depconf

export USRCONF := $(srctree)/.config.unix
export DEFCONF := $(USRCONF).1

ifneq ($(wildcard $(USRCONF)),)
  export DOTCONF := $(USRCONF)
else
  export DOTCONF := $(DEFCONF)
endif

ifneq ($(wildcard $(USRCONF)),)
  ifneq ($(wildcard $(DEFCONF)),)
    build_prereq := configure
    depconfig_prereq  := rm-defconf
  else
    build_prereq := depconfig
  endif
else
  depconfig_prereq := gen-defconf
endif

export KCONFIG_CONFIG := $(DOTCONF)
export PYTHONDONTWRITEBYTECODE := y

build:

.PHONY: menuconfig gen-defconf rm-defconf \
	depconfig configure dotplat build all

menuconfig:
	@scripts/kconfig.py menuconfig

$(objtree)/.dir:
	@mkdir $(objtree)
	@touch $@

$(gendir)/.dir:
	@mkdir $(gendir)
	@touch $@

$(objtree)/features.cmake: $(objtree)/.dir $(gendir)/.dir
	@scripts/cc-feature.py cmake

gen-defconf:
	@scripts/kconfig.py alldefconfig

rm-defconf:
	@rm $(DEFCONF)

$(objtree)/depconf: $(objtree)/.dir
	@touch $@

depconfig: $(depconfig_prereq) $(objtree)/depconf
	@scripts/depconf.py

configure: $(objtree)/features.cmake depconfig
	@cmake -G "$(generator)" -S . -B $(objtree) $(EXTOPT)

dotplat:
	@if [ -f $(objtree)/CMakeCache.txt ] &&			\
	    [ "$$(cat $(DOTPLAT) 2>&1)" != $$(uname -o) ]; then	\
		uname -o >$(DOTPLAT);				\
	fi

build: $(build_prereq) dotplat
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
	@rm -rf $(objtree)

tests := $(wildcard $(objtree)/t/*.t)
tests := $(patsubst $(objtree)/%,%,$(tests))

.PHONY: $(tests)

$(tests):
	@$(objtree)/$@

.PHONY: t/all

t/all:
	@ctest --test-dir $(objtree)/tests --parallel $(shell nproc)

scripts := $(wildcard scripts/*.sh) $(wildcard scripts/*.py)
args    := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

.PHONY: $(args)

$(args):
	@:

.PHONY: $(scripts)

$(scripts):
	@./$@ $(args)
