MODULE := vend_cjson

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

CFLAGS += -Wall -std=c99 -g -I. -I../../public/ 

export OUT_VEND_CJSON=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_VEND_CJSON=$(MODULE).a

OBJECTS_VEND_CJSON := $(patsubst %.c,%.o,$(wildcard *.c))

$(OUT_VEND_CJSON)/%.o : %.c
	@mkdir -p $(OUT_VEND_CJSON) 
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_VEND_CJSON)/$(LIBS_VEND_CJSON): $(patsubst %,$(OUT_VEND_CJSON)/%,$(subst :, ,$(OBJECTS_VEND_CJSON))) 
	@cd $(OUT_VEND_CJSON) && $(AR) rvs $(MODULE).a $(OBJECTS_VEND_CJSON)  

#default target
$(MODULE) all: $(OUT_VEND_CJSON)/$(LIBS_VEND_CJSON) 

clean-$(MODULE) clean: 
	rm -rf $(OUT_VEND_CJSON)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_VEND_CJSON=$(OUT_VEND_CJSON)"
	@echo "LIBS_VEND_CJSON=$(LIBS_VEND_CJSON)"
