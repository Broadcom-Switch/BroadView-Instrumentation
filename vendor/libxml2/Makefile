MODULE := vend_libxml2

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wno-error -g -I./include -DHAVE_HIDDEN -DLIBXML_HTML_ENABLED
#CFLAGS += -Wno-error=unused-but-set-variable -Wno-error=unused-function -Wno-error=format -Wno-error=enum-compare -g -I./include
#CFLAGS += -Wall -g -I./include
export OUT_VEND_LIBXML2=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_VEND_LIBXML2=$(MODULE).a

OBJECTS_VEND_LIBXML2 := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_VEND_LIBXML2)::
	@echo Making libxml2
	@mkdir -p $(OUT_VEND_LIBXML2)

$(OUT_VEND_LIBXML2)/%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_VEND_LIBXML2)/$(LIBS_VEND_LIBXML2): $(patsubst %,$(OUT_VEND_LIBXML2)/%,$(subst :, ,$(OBJECTS_VEND_LIBXML2))) 
	@cd $(OUT_VEND_LIBXML2) && $(AR) rvs $(MODULE).a $(OBJECTS_VEND_LIBXML2)  

#default target
$(MODULE) all: $(OUT_VEND_LIBXML2) $(OUT_VEND_LIBXML2)/$(LIBS_VEND_LIBXML2) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning libxml2
	@rm -f $(OUT_VEND_LIBXML2)/*.o $(OUT_VEND_LIBXML2)/*.a
	@rmdir $(OUT_VEND_LIBXML2) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_VEND_LIBXML2=$(OUT_VEND_LIBXML2)"
	@echo "LIBS_VEND_LIBXML2=$(LIBS_VEND_LIBXML2)"
