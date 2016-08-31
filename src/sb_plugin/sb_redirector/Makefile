MODULE := sbredirector

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

CFLAGS += -Wall -g -I./include -I../include -I../../public 

export OUT_SBREDIRECTOR=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_SBREDIRECTOR=$(MODULE).a

#OBJECTS_SBREDIRECTOR := $(patsubst %.c,%.o,$(wildcard *.c))

OBJECTS_SBREDIRECTOR = sbplugin_redirect_core.o sbplugin_redirect_system.o

ifneq (,$(filter bst,$(COMPONENTS)))
  OBJECTS_SBREDIRECTOR += sbplugin_redirect_bst.o
endif

ifneq (,$(filter pt,$(COMPONENTS)))
  OBJECTS_SBREDIRECTOR += sbplugin_redirect_packet_trace.o
endif

ifneq (,$(filter bhd,$(COMPONENTS)))
  OBJECTS_SBREDIRECTOR += sbplugin_redirect_bhd.o
endif

$(OUT_SBREDIRECTOR)::
	@echo Making SB-Redirector
	@mkdir -p $(OUT_SBREDIRECTOR)

$(OUT_SBREDIRECTOR)/%.o : %.c
	$(CC) $(CFLAGS) -c  $< -o $@ 

# target for .a 
$(OUT_SBREDIRECTOR)/$(LIBS_SBREDIRECTOR): $(patsubst %,$(OUT_SBREDIRECTOR)/%,$(subst :, ,$(OBJECTS_SBREDIRECTOR))) 
	@cd $(OUT_SBREDIRECTOR) && $(AR) rvs $(MODULE).a $(OBJECTS_SBREDIRECTOR)  

#default target
$(MODULE) all: $(OUT_SBREDIRECTOR) $(OUT_SBREDIRECTOR)/$(LIBS_SBREDIRECTOR) 
	$(NOOP)

clean-$(MODULE) clean: 
	@echo Cleaning SB-Redirector
	@rm -f $(OUT_SBREDIRECTOR)/*.o $(OUT_SBREDIRECTOR)/*.a
	@rmdir $(OUT_SBREDIRECTOR) 2>/dev/null; true

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_SBREDIRECTOR=$(OUT_SBREDIRECTOR)"
	@echo "LIBS_SBREDIRECTOR=$(LIBS_SBREDIRECTOR)"
	@echo "OBJECTS_SBREDIRECTOR=$(OBJECTS_SBREDIRECTOR)"
