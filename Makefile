# SPDX-License-Identifier: GPL-3.0-or-later or MIT

ifneq ($(filter check-symlink,$(.FEATURES)),check-symlink)
$(error GNU Make >= 4.0 is required. Your Make version is $(MAKE_VERSION))
endif

MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

export TREE := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

ifneq ($(PWD),$(TREE))
$(error you need to be inside the source tree ($(TREE)))
endif

ifneq ($(LLVM),)
CC := clang
LD := ld.lld
else
CC := gcc
LD := ld.bfd
endif

export CC LD

.PHONY: configure build all

build:
	@cmake --build $(TREE)/build --parallel

configure:
	@cmake -S $(TREE) -B $(TREE)/build

all: configure build

.PHONY: clean distclean

clean:
	@cmake --build $(TREE)/build --target clean

distclean:
	@rm -rf $(TREE)/include/generated
	@git ls-files --directory -o $(TREE)/build | xargs rm -rf

.PHONY: menuconfig

menuconfig:
	@$(TREE)/scripts/kconfig.py menuconfig

scripts := $(notdir $(wildcard scripts/*.sh))
args    := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))

.PHONY: $(args)

$(args):
	@:

.PHONY: $(scripts)

$(scripts):
	@bash scripts/$@ $(args)
