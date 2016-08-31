MODULE := bviewpackettraceapp

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .
CFLAGS += -Wall -g -I. -I../../src/public/ -I$(OPENAPPS_OUTPATH) -I../../vendor/cjson

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_PTAPP=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_PTAPP=$(MODULE).a

OBJECTS_PTAPP := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_PTAPP)::
	@echo Making PT EXAMPLE APP
	@mkdir -p $(OUT_PTAPP)

$(OUT_PTAPP)/%.o : %.c
	@mkdir -p $(OUT_PTAPP) 
	$(CC) $(CFLAGS) -c  $< -o $@

# target for .a 
$(OUT_PTAPP)/$(LIBS_PTAPP): $(patsubst %,$(OUT_PTAPP)/%,$(subst :, ,$(OBJECTS_PTAPP))) 
	@cd $(OUT_PTAPP) && $(AR) rvs $(MODULE).a $(OBJECTS_PTAPP)  
#	@cd $(OUT_PTAPP) && $(CC) $(CFLAGS) -L../ -L. -o $(MODULE) $(OBJECTS_PTAPP) -lm -lcjson -lpthread

#default target
$(MODULE) all: $(OUT_PTAPP) $(OUT_PTAPP)/$(LIBS_PTAPP) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning PT EXAMPLE APP
	@rm -f $(OUT_PTAPP)/*.o $(OUT_PTAPP)/*.a
	@rmdir $(OUT_PTAPP) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_PTAPP=$(OUT_PTAPP)"
	@echo "LIBS_PTAPP=$(LIBS_PTAPP)"
