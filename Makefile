APPLICATION = co2-meter-riot
BOARD ?= raytac-mdbt50q-db
RIOTBASE ?= $(CURDIR)/../petr_RIOT
DEVELHELP ?= 1
QUIET ?= 1

# bindist specific stuff:
#
# build and use module "abc".
# use BIN_DIRS instead of DIRS, BIN_USEMODULE instead of USEMODULE,
# to flag that the directory should only be included when doing a normal build,
# and that the resulting .a should be saved when doing "make bindist"
BIN_DIRS += app/src
BIN_USEMODULE += co2-meter-riot-app
BIN_USEMODULE += $(APPLICATION_MODULE)

# list of extra files to include in binary distribution
DIST_FILES += Makefile

include $(RIOTBASE)/Makefile.include