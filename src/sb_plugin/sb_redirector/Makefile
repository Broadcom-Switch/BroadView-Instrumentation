MODULE := sbredirector

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wall -g -I./include -I../include -I../../public 

export OUT_SBREDIRECTOR=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_SBREDIRECTOR=$(MODULE).a

OBJECTS_SBREDIRECTOR := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_SBREDIRECTOR)/%.o : %.c
	@mkdir -p $(OUT_SBREDIRECTOR) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_SBREDIRECTOR)/$(LIBS_SBREDIRECTOR): $(patsubst %,$(OUT_SBREDIRECTOR)/%,$(subst :, ,$(OBJECTS_SBREDIRECTOR))) 
	@cd $(OUT_SBREDIRECTOR) && $(AR) rvs $(MODULE).a $(OBJECTS_SBREDIRECTOR)  

#default target
$(MODULE) all: $(OUT_SBREDIRECTOR)/$(LIBS_SBREDIRECTOR) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_SBREDIRECTOR)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_SBREDIRECTOR=$(OUT_SBREDIRECTOR)"
	@echo "LIBS_SBREDIRECTOR=$(LIBS_SBREDIRECTOR)"
