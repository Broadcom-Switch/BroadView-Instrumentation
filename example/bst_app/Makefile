MODULE := bviewbstapp

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .
CFLAGS += -Wall -g -I. -I../../src/public/ -I$(OPENAPPS_OUTPATH) -I../../vendor/cjson

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_BSTAPP=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_BSTAPP=$(MODULE).a

OBJECTS_BSTAPP := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_BSTAPP)::
	@echo Making BST EXAMPLE APP
	@mkdir -p $(OUT_BSTAPP)

$(OUT_BSTAPP)/%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_BSTAPP)/$(LIBS_BSTAPP): $(patsubst %,$(OUT_BSTAPP)/%,$(subst :, ,$(OBJECTS_BSTAPP))) 
	@cd $(OUT_BSTAPP) && $(AR) rvs $(MODULE).a $(OBJECTS_BSTAPP)  
#	@cd $(OUT_BSTAPP) && $(CC) $(CFLAGS) -L../ -L. -o $(MODULE) $(OBJECTS_BSTAPP) -lm -lcjson -lpthread

#default target
$(MODULE) all: $(OUT_BSTAPP) $(OUT_BSTAPP)/$(LIBS_BSTAPP) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning BST EXAMPLE APP
	@rm -f $(OUT_BSTAPP)/*.o $(OUT_BSTAPP)/*.a
	@rmdir $(OUT_BSTAPP) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_BSTAPP=$(OUT_BSTAPP)"
	@echo "LIBS_BSTAPP=$(LIBS_BSTAPP)"
	@echo "OBJECTS_BSTAPP=$(OBJECTS_BSTAPP)"
