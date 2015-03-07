MODULE := log

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wall -g -I../../public

export OUT_LOG=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_LOG=$(MODULE).a

OBJECTS_LOG := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_LOG)/%.o : %.c
	@mkdir -p $(OUT_LOG) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_LOG)/$(LIBS_LOG): $(patsubst %,$(OUT_LOG)/%,$(subst :, ,$(OBJECTS_LOG))) 
	@cd $(OUT_LOG) && $(AR) rvs $(MODULE).a $(OBJECTS_LOG)  

#default target
$(MODULE) all: $(OUT_LOG)/$(LIBS_LOG) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_LOG)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_LOG=$(OUT_LOG)"
	@echo "LIBS_LOG=$(LIBS_LOG)"
