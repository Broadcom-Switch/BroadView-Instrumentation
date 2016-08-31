ifeq ($(SBPLUGIN), ovsdb)
MODULE := sbovsdb
endif

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_SBPLUGIN=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_SBPLUGIN=$(MODULE).a

ifeq ($(SBPLUGIN), ovsdb)
SB_PLUGIN_DIR = sb_ovsdb
endif

SB_OVSDB_VENDOR_INCLUDE_DIR = $(OPENAPPS_BASE)/vendor/ovsdb/include


CFLAGS += -Wall -g -I$(SB_OVSDB_VENDOR_INCLUDE_DIR) -I../../public -I./include -I./$(SB_PLUGIN_DIR)/include -I../include -I./ -I../../../platform -I./bst/include




searchdirs = $(realpath $(OPENAPPS_BASE)/src/sb_plugin/sb_ovsdb/)

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
	@echo "OVSDB LIBS are copying"
	@cp $(OPENAPPS_BASE)/vendor/ovsdb/*.a $(OUT_SBPLUGIN)/
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_SBPLUGIN)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_SBPLUGIN=$(OUT_SBPLUGIN)"
	@echo "LIBS_SBPLUGIN=$(LIBS_SBPLUGIN)"
