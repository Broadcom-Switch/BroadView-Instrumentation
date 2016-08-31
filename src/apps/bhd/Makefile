MODULE := bhd

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .
CFLAGS += -Wall -g -I. -I../../public -I../../../public -I./api -I../../sb_plugin/include -I../../../vendor/cjson -I../../../platform -I../../infrastructure/system/system_utils

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_BHD=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_BHD=$(MODULE).a

searchdirs = $(realpath $(OPENAPPS_BASE)/src/apps/bhd/api)

export CPATH += $(searchdirs)
OBJECTS_BHD := $(notdir $(patsubst %.c,%.o,$(shell find . -name "*.c")))

export VPATH += $(dir $(shell find . -name "*.c"))

$(OUT_BHD)::
	@echo Making BHD
	@mkdir -p $(OUT_BHD)

$(OUT_BHD)/%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_BHD)/$(LIBS_BHD): $(patsubst %,$(OUT_BHD)/%,$(subst :, ,$(OBJECTS_BHD))) 
	@cd $(OUT_BHD) && $(AR) rvs $(MODULE).a $(OBJECTS_BHD)  

#default target
$(MODULE) all: $(OUT_BHD) $(OUT_BHD)/$(LIBS_BHD) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning BHD
	@rm -f $(OUT_BHD)/*.o $(OUT_BHD)/*.a
	@rmdir $(OUT_BHD) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_BHD=$(OUT_BHD)"
	@echo "LIBS_BHD=$(LIBS_BHD)"
	@echo "OBJECTS_BHD=$(OBJECTS_BHD)"
