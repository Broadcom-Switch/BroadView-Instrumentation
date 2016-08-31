#  global definitions
#

-include package.cfg

ifeq ($(OPENAPPS_BASE),)
   OPENAPPS_BASE := $(shell echo $(dir $(abspath $(firstword $(MAKEFILE_LIST)))) | sed -e 's/\(.*\)\/output\/.*/\1/')
endif
export OPENAPPS_BASE

ifeq ($(OPENAPPS_OUTPATH),)
   OPENAPPS_OUTPATH = $(shell echo $(dir $(abspath $(firstword $(MAKEFILE_LIST)))) | sed -e 's/\(.*\)\//\1/')
endif
export OPENAPPS_OUTPATH

ifeq ($(CROSS_COMPILE),)
   CROSS_COMPILE = /usr/bin/
endif
export CROSS_COMPILE

ifeq ($(platform),)
   platform = gto-2_6
endif
export platform

export __BYTE_ORDER =__BIG_ENDIAN

include $(OPENAPPS_BASE)/tools/Make.common
