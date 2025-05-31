# SPDX-License-Identifier: GPL-3.0-or-later or MIT

ifneq ($(filter extra-prereqs,$(.FEATURES)),extra-prereqs)
  $(error GNU Make >= 4.3 is required. Your Make version is $(MAKE_VERSION))
endif

MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

srctree := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
gendir  := $(srctree)/include/generated
objtree := $(srctree)/build.unix

CMAKEFLAGS ?= $(shell cat $(objtree)/flags 2>/dev/null)

export SRCTREE := $(srctree)
export GENDIR  := $(gendir)
export OBJTREE := $(objtree)

ifneq ($(wildcard $(objtree)/tools),)
  export CC := $(shell grep CC $(objtree)/tools | cut -f2)
  export LD := $(shell grep LD $(objtree)/tools | cut -f2)
  export MK := $(shell grep MK $(objtree)/tools | cut -f2)
else
  export CC := gcc
  export LD := ld.bfd
  export MK := Ninja
endif

ifneq ($(LLVM),)
  export CC := clang
  export LD := ld.lld
endif

ifneq ($(MAKEFILE),)
  export MK := Unix Makefiles
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
	depconfig configure dotplat build all dump-flags

$(objtree)/.dir:
	@mkdir $(objtree)
	@touch $@

$(gendir)/.dir:
	@mkdir $(gendir)
	@touch $@

$(objtree)/tools: $(objtree)/.dir
	@printf '%s\t%s\n' CC "$(CC)" >$@
	@printf '%s\t%s\n' LD "$(LD)" >>$@
	@printf '%s\t%s\n' MK "$(MK)" >>$@

menuconfig: $(objtree)/tools
	@scripts/kconfig.py menuconfig

$(objtree)/features.cmake: $(objtree)/.dir $(gendir)/.dir
	@scripts/cc-feature.py cmake

gen-defconf: $(objtree)/tools
	@scripts/kconfig.py alldefconfig

rm-defconf:
	@rm $(DEFCONF)

$(objtree)/depconf: $(objtree)/.dir
	@touch $@

depconfig: $(depconfig_prereq) $(objtree)/depconf
	@scripts/depconf.py

dump-flags: $(objtree)/.dir
	@printf '%s\n' '$(CMAKEFLAGS)' >$(objtree)/flags

configure: $(objtree)/features.cmake depconfig dump-flags
	@cmake -G '$(MK)' -S . -B $(objtree) $(CMAKEFLAGS)

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

test_src := $(wildcard $(objtree)/tests/unit/*.t $(objtree)/tests/param/*.p)
test := $(patsubst $(objtree)/tests/%,%,$(test_src))

.PHONY: tests $(test)

tests:
	@ctest --test-dir $(objtree)/tests --parallel $(shell nproc)

$(test):
	@$(objtree)/tests/$@

scripts_src := $(wildcard $(srctree)/scripts/*.sh $(srctree)/scripts/*.py)
scripts := $(patsubst $(srctree)/%,%,$(scripts_src))
args := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

.PHONY: $(args) $(scripts)

$(args):
	@:

$(scripts):
	@./$@ $(args)
