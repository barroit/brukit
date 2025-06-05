# SPDX-License-Identifier: GPL-3.0-or-later or MIT

menu "Core features"

menuconfig ENABLE_I18N
	bool "Enable i18n support"

if ENABLE_I18N

config TEXT_DOMAIN_NAME
	string "Text domain of gettext"
	default $(pg-name)

endif # ENABLE_I18N

menu "Formatting options"

config CNTRL_SUB
	string "Control character substitution"
	default "�"

config LINE_WRAP
	int "Line wrap threshold"
	default 80

config HELP_OPT_INDENT
	int "Help message indentation"
	default 2

config HELP_OPT_WRAP
	int "Help message wrap threshold"
	default 16

endmenu # Character settings

config NULL_DEVICE
	string "Null device path"
	default "NUL:" if $(WIN32)
	default "/dev/null"

endmenu # Program features
