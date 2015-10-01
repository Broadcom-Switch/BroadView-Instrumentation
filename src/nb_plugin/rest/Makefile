MODULE := rest

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wall -std=c99 -g -I. -I../../public/ -I../../../vendor/cjson
export OUT_REST=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_REST=$(MODULE).a

OBJECTS_REST := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_REST)/%.o : %.c
	@mkdir -p $(OUT_REST) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_REST)/$(LIBS_REST): $(patsubst %,$(OUT_REST)/%,$(subst :, ,$(OBJECTS_REST))) 
	@cd $(OUT_REST) && $(AR) rvs $(MODULE).a $(OBJECTS_REST)  
#	@cd $(OUT_REST) && $(CC) $(CFLAGS) -o $(MODULE) $(OBJECTS_REST) #This line needs to be removed.

#default target
$(MODULE) all: $(OUT_REST)/$(LIBS_REST) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_REST)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_REST=$(OUT_REST)"
	@echo "LIBS_REST=$(LIBS_REST)"
