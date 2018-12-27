APPLICATION = co2-meter-riot
BOARD ?= mdbt50q-db
RIOTBASE ?= $(CURDIR)/../petr_RIOT
DEVELHELP ?= 1
QUIET ?= 1

BIN_DIRS += app/src
BIN_USEMODULE += co2-meter-riot-app
BIN_USEMODULE += $(APPLICATION_MODULE)

DIST_FILES += Makefile

include $(RIOTBASE)/Makefile.include