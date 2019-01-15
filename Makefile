APPLICATION = co2-meter-riot
BOARD ?= nrf52840dk
RIOTBASE ?= $(CURDIR)/../petr_RIOT
DEVELHELP ?= 1
QUIET ?= 1

BIN_DIRS += app/src
BIN_USEMODULE += co2-meter-riot-app
BIN_USEMODULE += $(APPLICATION_MODULE)

INCLUDES += -I${CURDIR}/app/inc

USEPKG += u8g2
FEATURES_REQUIRED += periph_spi

MY_SHELL ?= 1

ifeq ($(MY_SHELL),1)
    # we want to use SAUL:
    USEMODULE += saul_default
    # include the shell:
    USEMODULE += shell shell_commands
    # additional modules for debugging:
    USEMODULE += ps
    CFLAGS += -DMY_SHELL=1
endif

USEMODULE += bme280 hdc1000 ccs811

DIST_FILES += Makefile

include $(RIOTBASE)/Makefile.include