MODULE := system

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_SYSTEM=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_SYSTEM=$(MODULE).a

CFLAGS += -Wall -g -I../../public -I ../../nb_plugin/rest/ -I../../sb_plugin/include -I $(OPENAPPS_OUTPATH)

OBJECTS_SYSTEM := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_SYSTEM)/%.o : %.c
	@mkdir -p $(OUT_SYSTEM) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_SYSTEM)/$(LIBS_SYSTEM): $(patsubst %,$(OUT_SYSTEM)/%,$(subst :, ,$(OBJECTS_SYSTEM))) 
	@cd $(OUT_SYSTEM) && $(AR) rvs $(MODULE).a $(OBJECTS_SYSTEM)  

#default target
$(MODULE) all: $(OUT_SYSTEM)/$(LIBS_SYSTEM) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_SYSTEM)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_SYSTEM=$(OUT_SYSTEM)"
	@echo "LIBS_SYSTEM=$(LIBS_SYSTEM)"
