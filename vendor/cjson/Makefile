MODULE := vend_cjson

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

CFLAGS += -Wall -std=c99 -g -I. -I../../public/ 

export OUT_VEND_CJSON=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_VEND_CJSON=$(MODULE).a

OBJECTS_VEND_CJSON := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_VEND_CJSON)::
	@echo Making CJSON
	@mkdir -p $(OUT_VEND_CJSON)

$(OUT_VEND_CJSON)/%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_VEND_CJSON)/$(LIBS_VEND_CJSON): $(patsubst %,$(OUT_VEND_CJSON)/%,$(subst :, ,$(OBJECTS_VEND_CJSON))) 
	@cd $(OUT_VEND_CJSON) && $(AR) rvs $(MODULE).a $(OBJECTS_VEND_CJSON)  

#default target
$(MODULE) all: $(OUT_VEND_CJSON) $(OUT_VEND_CJSON)/$(LIBS_VEND_CJSON) 

clean-$(MODULE) clean: 
	@echo Cleaning CJSON
	@rm -f $(OUT_VEND_CJSON)/*.o $(OUT_VEND_CJSON)/*.a
	@rmdir $(OUT_VEND_CJSON) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_VEND_CJSON=$(OUT_VEND_CJSON)"
	@echo "LIBS_VEND_CJSON=$(LIBS_VEND_CJSON)"
