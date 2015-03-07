MODULE := bst

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .
CFLAGS += -Wno-pointer-to-int-cast -Wall -g -I. -I../../public -I../../../public -I./api -I../../sb_plugin/include -I../../../vendor/cjson -I../../../platform

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_BST=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_BST=$(MODULE).a

searchdirs = $(realpath $(OPENAPPS_BASE)/src/apps/bst/api)

export CPATH += $(searchdirs)
OBJECTS_BST := $(notdir $(patsubst %.c,%.o,$(shell find . -name "*.c")))

export VPATH += $(dir $(shell find . -name "*.c"))

$(OUT_BST)/%.o : %.c
	@mkdir -p $(OUT_BST) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_BST)/$(LIBS_BST): $(patsubst %,$(OUT_BST)/%,$(subst :, ,$(OBJECTS_BST))) 
	@cd $(OUT_BST) && $(AR) rvs $(MODULE).a $(OBJECTS_BST)  

#default target
$(MODULE) all: $(OUT_BST)/$(LIBS_BST) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_BST)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_BST=$(OUT_BST)"
	@echo "LIBS_BST=$(LIBS_BST)"
	@echo "OBJECTS_BST=$(OBJECTS_BST)"
