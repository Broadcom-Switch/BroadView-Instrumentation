MODULE := vendorinit

CC ?= gcc
AR ?= ar
OPENAPPS_OUTPATH ?= .

# NOOP - no-operation, used to suppress "Nothing to do for ..." messages.
NOOP  ?= @:

export OUT_VENDORINIT=$(OPENAPPS_OUTPATH)/$(MODULE)
export LIBS_VENDORINIT=$(MODULE).a

ifneq ($(BCM_PLATFORM), bcmsim)
override-target=unix-user
endif
include $(realpath $(SDK)/make/Make.config)

ifeq ($(SBPLUGIN), sdk)
INCDIR_DRIVER_INIT =$(INCDIR_SDK)
ifneq ($(PLATFORM), im_n2948_6xlm)
searchdirs = ./driver_sdk/bcm_svk
else
searchdirs = ./driver_sdk/n2948
endif
searchdirs += ./driver_sdk/common
CFLAGS += -I./driver_sdk/common
#searchdirs = $(realpath $(OPENAPPS_BASE)/vendor/driver/driver_sdk)
endif

ifeq ($(SBPLUGIN), opennsl)
INCDIR_DRIVER_INIT =$(INCDIR_OPENNSL)
CFLAGS +=-DCDP_PACKAGE
searchdirs = ./driver_opennsl
#searchdirs = $(realpath $(OPENAPPS_BASE)/vendor/driver/driver_opennsl)
endif

#CFLAGS += $(OPENAPPS_SDKFLAGS) -Wall -g -I$(INCDIR_SDK) -I../../public -I../include -I../sb_sdk/include -I../../../output/gto-2_6-sdk_6.4.0/sdk-all-6.4.0/include

CFLAGS += $(OPENAPPS_SDKFLAGS) -Wall -g -I$(INCDIR_DRIVER_INIT)

#searchdirs = $(realpath $(OPENAPPS_BASE)/vendor/driver)

export CPATH += $(searchdirs)

#OBJECTS_VENDORINIT := $(notdir $(patsubst %.c,%.o,$(shell find ./$(searchdirs) -name "*.c")))
OBJECTS_VENDORINIT := $(notdir $(patsubst %.c,%.o,$(shell find $(searchdirs) -name "*.c")))

export VPATH += $(dir $(shell find . -name "*.c"))

#$(OUT_VENDORINIT)/%.o :  $(searchdirs)/%.c
	#@mkdir -p $(OUT_VENDORINIT) 
	#$(CC) $(CFLAGS) -c  $< -o $@ 

define ARC_INCLUDE_C_template
$(OUT_VENDORINIT)/$(patsubst %.c,%.o,$(2)): $(1)$(2)
	@mkdir -p $(OUT_VENDORINIT) 
	$(CC) $(CFLAGS) -c $(1)$(2) -o $(OUT_VENDORINIT)/$(patsubst %.c,%.o,$(2))
endef

$(foreach srcfile,$(shell find $(searchdirs) -name "*.c"),$(eval $(call ARC_INCLUDE_C_template,$(dir $(srcfile)),$(notdir $(srcfile)))))

# target for .a 
$(OUT_VENDORINIT)/$(LIBS_VENDORINIT): $(patsubst %,$(OUT_VENDORINIT)/%,$(subst :, ,$(OBJECTS_VENDORINIT))) 
	@cd $(OUT_VENDORINIT) && $(AR) rvs $(MODULE).a $(OBJECTS_VENDORINIT)  

#default target
$(MODULE) all: $(OUT_VENDORINIT)/$(LIBS_VENDORINIT) 
	$(NOOP)

clean-$(MODULE) clean: 
	rm -rf $(OUT_VENDORINIT)

#target to print all exported variables
debug-$(MODULE) dump-variables: 
	@echo "OUT_VENDORINIT=$(OUT_VENDORINIT)"
	@echo "LIBS_VENDORINIT=$(LIBS_VENDORINIT)"
	@echo "OBJECTS_VENDORINIT=$(OBJECTS_VENDORINIT)"
	@echo "INCDIR_DRIVER_INIT=$(INCDIR_DRIVER_INIT)"
	@echo "searchdirs=$(searchdirs)"

