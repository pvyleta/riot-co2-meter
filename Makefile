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

DIST_FILES += Makefile

include $(RIOTBASE)/Makefile.include