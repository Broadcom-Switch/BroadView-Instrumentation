# makefile generated for /tmp/ec14242070_RELEASE_OPENAPPS_2.0.4.3-Broadcom-th_svk-gto-BL20-opennsl-6466-BPV/openapps/output/th_svk
#

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
   CROSS_COMPILE = /projects/nwsoft-toolchains/brl_2.0/gto/bin/powerpc-broadcom-linux-gnu-
endif
export CROSS_COMPILE

ifeq ($(platform),)
   platform = gto-2_6
endif
export platform

export __BYTE_ORDER =__BIG_ENDIAN

include $(OPENAPPS_BASE)/tools/Make.common
