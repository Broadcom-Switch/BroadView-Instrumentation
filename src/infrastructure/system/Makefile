MODULE := system

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_SYSTEM=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_SYSTEM=$(MODULE).a

SYSTEM_COMMON_DIR = system 

CFLAGS += -Wall -g -I../../public -I ../../nb_plugin/rest/ -I../../sb_plugin/include -I ./reg_hb -I ./system_utils -I ./reg_hb/api -I ./system_utils/api -I ./ -I ../../../vendor/cjson -I $(OPENAPPS_OUTPATH)
searchdirs = $(realpath $(OPENAPPS_BASE)/src/infrastructure/$(SYSTEM_COMMON_DIR)/system_utils)
searchdirs += $(realpath $(OPENAPPS_BASE)/src/infrastructure/$(SYSTEM_COMMON_DIR)/reg_hb)
searchdirs += $(realpath $(OPENAPPS_BASE)/src/infrastructure/$(SYSTEM_COMMON_DIR))

export CPATH += $(searchdirs)

OBJECTS_SYSTEM := $(notdir $(patsubst %.c,%.o,$(shell find . -name "*.c")))

export VPATH += $(dir $(shell find . -name "*.c"))

$(OUT_SYSTEM)::
	@echo Making SYSTEM
	@mkdir -p $(OUT_SYSTEM)

$(OUT_SYSTEM)/%.o : %.c
	@mkdir -p $(OUT_SYSTEM) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_SYSTEM)/$(LIBS_SYSTEM): $(patsubst %,$(OUT_SYSTEM)/%,$(subst :, ,$(OBJECTS_SYSTEM))) 
	@cd $(OUT_SYSTEM) && $(AR) rvs $(MODULE).a $(OBJECTS_SYSTEM)  

#default target
$(MODULE) all: $(OUT_SYSTEM) $(OUT_SYSTEM)/$(LIBS_SYSTEM) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning SYSTEM
	@rm -f $(OUT_SYSTEM)/*.o $(OUT_SYSTEM)/*.a
	@rmdir $(OUT_SYSTEM) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_SYSTEM=$(OUT_SYSTEM)"
	@echo "LIBS_SYSTEM=$(LIBS_SYSTEM)"
