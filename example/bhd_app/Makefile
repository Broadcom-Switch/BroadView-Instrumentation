MODULE := bviewbhdapp

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .
CFLAGS += -Wall -g -I. -I../../src/public/ -I$(OPENAPPS_OUTPATH) -I../../vendor/cjson

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_BHDAPP=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_BHDAPP=$(MODULE).a

OBJECTS_BHDAPP := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_BHDAPP)::
	@echo Making BHD EXAMPLE APP
	@mkdir -p $(OUT_BHDAPP)

$(OUT_BHDAPP)/%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@

# target for .a 
$(OUT_BHDAPP)/$(LIBS_BHDAPP): $(patsubst %,$(OUT_BHDAPP)/%,$(subst :, ,$(OBJECTS_BHDAPP))) 
	@cd $(OUT_BHDAPP) && $(AR) rvs $(MODULE).a $(OBJECTS_BHDAPP)  
#	@cd $(OUT_BHDAPP) && $(CC) $(CFLAGS) -L../ -L. -o $(MODULE) $(OBJECTS_BHDAPP) -lm -lcjson -lpthread

#default target
$(MODULE) all: $(OUT_BHDAPP) $(OUT_BHDAPP)/$(LIBS_BHDAPP) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning BHD EXAMPLE APP
	@rm -f $(OUT_BHDAPP)/*.o $(OUT_BHDAPP)/*.a
	@rmdir $(OUT_BHDAPP) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_BHDAPP=$(OUT_BHDAPP)"
	@echo "LIBS_BHDAPP=$(LIBS_BHDAPP)"
