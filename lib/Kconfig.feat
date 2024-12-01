# SPDX-License-Identifier: GPL-3.0-or-later or MIT

config CC_HAS_TOPLEVEL_REORDER
	def_bool $(cc-option,-ftoplevel-reorder)

menu "Core features"

config I18N_SUPP
	bool "Enable i18n support"

config TERMAS_SMALL_BUFFER
	bool "Use small (1K) buffer in termas report functions"

config TS_SMALL_UNIT
	bool "Display timestamps in milliseconds"

menuconfig SPEC_ALT_CNTRL
	bool "Customize control character replacement"
	default y

if SPEC_ALT_CNTRL

config ALT_CNTRL
	string "Control character replacement"
	default "�"

endif

endmenu # Program features
