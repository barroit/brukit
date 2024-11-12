# SPDX-License-Identifier: GPL-3.0-or-later or MIT

MAKEFLAGS += -rR
MAKEFLAGS += --no-print-directory

export UNIX := y

ROOT  := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
BUILD := $(ROOT)/build

BSNAME    := $(shell grep name $(ROOT)/.program | cut -f2)
BSVERSION := $(shell grep version $(ROOT)/.program | cut -f2)
BSARCH    := $(shell uname -m)
BSBUILD   := $(shell uname -s)
export BSNAME BSVERSION BSARCH BSBUILD

.PHONY: config build

config:
	@cmake -S $(ROOT) -B $(BUILD)

build: config
	@cmake --build $(BUILD) --parallel

# .PHONY: clean distclean

# clean:
# 	@make -C $(abs_build) clean

# distclean:
# 	@rm -rf $(abs_root)/include/generated
# 	@git ls-files --directory -o $(abs_root)/build | xargs rm -rf
# 	@rm -f $(abs_root)/.config*

.PHONY: menuconfig

menuconfig:
	@MENUCONFIG_STYLE=aquatic menuconfig

# scripts := $(notdir $(wildcard scripts/*.sh))
# args    := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))

# .PHONY: $(args)

# $(args):
# 	@:

# .PHONY: $(scripts)

# $(scripts):
# 	@bash scripts/$@ $(args)
