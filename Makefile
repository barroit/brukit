# SPDX-License-Identifier: GPL-3.0-or-later or MIT

MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

export TREE := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

ifneq ($(LLVM),)
CC := clang
LD := ld.lld
else
CC := gcc
LD := ld
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
	@make -C $(TREE)/build clean

distclean:
	@rm -rf $(TREE)/include/generated
	@git ls-files --directory -o $(TREE)/build | xargs rm -rf
	@rm -f $(TREE)/.config*

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
