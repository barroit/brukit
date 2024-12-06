# SPDX-License-Identifier: GPL-3.0-or-later or MIT

config CC_HAS_TOPLEVEL_REORDER
	def_bool $(cc-option,-ftoplevel-reorder)

menu "Core features"

config I18N_SUPP
	bool "Enable i18n support"

menu "Character settings"

menuconfig SPEC_ALT_CNTRL
	bool "Customize control character replacement"
	default y

if SPEC_ALT_CNTRL

config ALT_CNTRL
	string "Control character replacement"
	default "�"

endif

config LINE_WIDTH
	int "Default line wrap threshold"
	default 80

endmenu # Character settings

endmenu # Program features
