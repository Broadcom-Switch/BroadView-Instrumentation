MODULE := examplecfgapp

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .
CFLAGS += -Wall -g -I. -I../../src/public/ -I$(OPENAPPS_OUTPATH) -I../../vendor/cjson

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_EXAMPLE_CFG_APP=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_EXAMPLE_CFG_APP=$(MODULE).a

OBJECTS_EXAMPLE_CFG_APP := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_EXAMPLE_CFG_APP)/%.o : %.c
	@mkdir -p $(OUT_EXAMPLE_CFG_APP) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_EXAMPLE_CFG_APP)/$(LIBS_EXAMPLE_CFG_APP): $(patsubst %,$(OUT_EXAMPLE_CFG_APP)/%,$(subst :, ,$(OBJECTS_EXAMPLE_CFG_APP))) 
	@cd $(OUT_EXAMPLE_CFG_APP) && $(AR) rvs $(MODULE).a $(OBJECTS_EXAMPLE_CFG_APP)  
#	@cd $(OUT_EXAMPLE_CFG_APP) && $(CC) $(CFLAGS) -L../ -L. -o $(MODULE) $(OBJECTS_EXAMPLE_CFG_APP) -lm -lcjson -lpthread

#default target
$(MODULE) all: $(OUT_EXAMPLE_CFG_APP)/$(LIBS_EXAMPLE_CFG_APP) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_EXAMPLE_CFG_APP)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_EXAMPLE_CFG_APP=$(OUT_EXAMPLE_CFG_APP)"
	@echo "LIBS_EXAMPLE_CFG_APP=$(LIBS_EXAMPLE_CFG_APP)"
