MODULE := pktmux

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wall -g -I../../public -I../../../vendor/cjson 
export OUT_PKT_MUX=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_PKT_MUX=$(MODULE).a

OBJECTS_PKT_MUX := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_PKT_MUX)::
	@echo Making PACKET MULTIPLEXER
	@mkdir -p $(OUT_PKT_MUX)

$(OUT_PKT_MUX)/%.o : %.c
	@mkdir -p $(OUT_PKT_MUX) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_PKT_MUX)/$(LIBS_PKT_MUX): $(patsubst %,$(OUT_PKT_MUX)/%,$(subst :, ,$(OBJECTS_PKT_MUX))) 
	@cd $(OUT_PKT_MUX) && $(AR) rvs $(MODULE).a $(OBJECTS_PKT_MUX)  

#default target
$(MODULE) all: $(OUT_PKT_MUX) $(OUT_PKT_MUX)/$(LIBS_PKT_MUX) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning PACKET MULTIPLEXER
	@rm -f $(OUT_PKT_MUX)/*.o $(OUT_PKT_MUX)/*.a
	@rmdir $(OUT_PKT_MUX) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_PKT_MUX=$(OUT_PKT_MUX)"
	@echo "LIBS_PKT_MUX=$(LIBS_MODULEMGR)"
