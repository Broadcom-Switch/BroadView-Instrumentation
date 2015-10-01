MODULE := modulemgr

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wall -g -I../../public -I../../../vendor/cjson
export OUT_MODULEMGR=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_MODULEMGR=$(MODULE).a

OBJECTS_MODULEMGR := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_MODULEMGR)/%.o : %.c
	@mkdir -p $(OUT_MODULEMGR) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_MODULEMGR)/$(LIBS_MODULEMGR): $(patsubst %,$(OUT_MODULEMGR)/%,$(subst :, ,$(OBJECTS_MODULEMGR))) 
	@cd $(OUT_MODULEMGR) && $(AR) rvs $(MODULE).a $(OBJECTS_MODULEMGR)  

#default target
$(MODULE) all: $(OUT_MODULEMGR)/$(LIBS_MODULEMGR) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_MODULEMGR)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_MODULEMGR=$(OUT_MODULEMGR)"
	@echo "LIBS_MODULEMGR=$(LIBS_MODULEMGR)"
