ifeq ($(SBPLUGIN), sdk)
MODULE := sbsdk
endif

ifeq ($(SBPLUGIN), opennsl)
MODULE := sbopennsl
endif

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_SBPLUGIN=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_SBPLUGIN=$(MODULE).a

ifneq ($(BCM_PLATFORM), bcmsim)
override-target=unix-user
endif
include $(realpath $(SDK)/make/Make.config)

SB_BRCM_PLUGIN_COMMON_DIR = sb_common

ifeq ($(SBPLUGIN), sdk)
SB_BRCM_PLUGIN_DIR = sb_sdk
SB_BRCM_VENDOR_INCLUDE_DIR = $(INCDIR_SDK)
endif

ifeq ($(SBPLUGIN), opennsl)
SB_BRCM_PLUGIN_DIR = sb_opennsl
SB_BRCM_VENDOR_INCLUDE_DIR = $(INCDIR_OPENNSL)
CFLAGS +=-DCDP_PACKAGE
endif

CFLAGS += $(OPENAPPS_SDKFLAGS) -Wall -g -I$(SB_BRCM_VENDOR_INCLUDE_DIR) -I../../public -I../include -I../sb_brcm/$(SB_BRCM_PLUGIN_COMMON_DIR)/include -I../sb_brcm/$(SB_BRCM_PLUGIN_DIR)/ -I../../../platform 

searchdirs = $(realpath $(OPENAPPS_BASE)/src/sb_plugin/sb_brcm/$(SB_BRCM_PLUGIN_COMMON_DIR)/bst)

export CPATH += $(searchdirs)

OBJECTS_SBPLUGIN := $(notdir $(patsubst %.c,%.o,$(shell find . -name "*.c")))

export VPATH += $(dir $(shell find . -name "*.c"))

$(OUT_SBPLUGIN)/%.o : %.c
	@mkdir -p $(OUT_SBPLUGIN) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_SBPLUGIN)/$(LIBS_SBPLUGIN): $(patsubst %,$(OUT_SBPLUGIN)/%,$(subst :, ,$(OBJECTS_SBPLUGIN))) 
	@cd $(OUT_SBPLUGIN) && $(AR) rvs $(MODULE).a $(OBJECTS_SBPLUGIN)  

#default target
$(MODULE) all: $(OUT_SBPLUGIN)/$(LIBS_SBPLUGIN) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_SBPLUGIN)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_SBPLUGIN=$(OUT_SBPLUGIN)"
	@echo "LIBS_SBPLUGIN=$(LIBS_SBPLUGIN)"
