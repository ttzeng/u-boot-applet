extra-y     ?= $(notdir $(obj))
executables := $(strip $(extra-y))

ifeq ($(obj-y),)
obj-y   := $(addsuffix .o,$(executables))
endif
extra-y += $(addsuffix .bin,$(executables)) $(addsuffix .srec,$(executables))
lib-y   += stubs.o

clean-files := built-in.o lib.a *.o *.su .*.cmd .*.d

# We don't want gcc reordering functions if possible.  This ensures that an
# application's entry point will be the first function in the application's
# source file.
ccflags-y += $(call cc-option,-fno-toplevel-reorder)

ifeq ($(CONFIG_STANDALONE_LOAD_ADDR),)
CONFIG_STANDALONE_LOAD_ADDR := $(CONFIG_SYS_LOAD_ADDR)
endif

EXTRA_LDFLAGS += -Ttext $(CONFIG_STANDALONE_LOAD_ADDR)

quiet_cmd_link_elf = LD      $@
      cmd_link_elf = $(LD) $(KBUILD_LDFLAGS) -g $(EXTRA_LDFLAGS) \
                     -o $@ -e main $(obj)/built-in.o $(obj)/lib.a \
                     $(PLATFORM_LIBGCC) \
                     -L $(shell dirname `$(CC) $(CFLAGS) -print-libgcc-file-name`) -lgcc

$(obj)/stubs.o: $(obj)/../common/stubs.c
	$(call if_changed,cc_o_c)

$(addprefix $(obj)/,$(executables)): $(obj)/built-in.o $(obj)/lib.a FORCE
	$(call if_changed,link_elf)

$(obj)/%.bin: OBJCOPYFLAGS += -O binary
$(obj)/%.bin: $(obj)/%
	$(call if_changed,objcopy)

$(obj)/%.srec: OBJCOPYFLAGS += -O srec
$(obj)/%.srec: $(obj)/%
	$(call if_changed,objcopy)
