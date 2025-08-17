ifndef TARGET_COMPILE
    $(error TARGET_COMPILE not set)
endif

ifndef KP_DIR
    # If KP_DIR is not set, try to find SukiSU_KernelPatch_patch in common locations
    ifneq ("$(wildcard ../SukiSU_KernelPatch_patch)", "")
        KP_DIR = ../SukiSU_KernelPatch_patch
    else ifneq ("$(wildcard ../../SukiSU_KernelPatch_patch)", "")
        KP_DIR = ../../SukiSU_KernelPatch_patch
    else ifneq ("$(wildcard ../../../SukiSU_KernelPatch_patch)", "")
        KP_DIR = ../../../SukiSU_KernelPatch_patch
    else
        # Default to a common location
        KP_DIR = ../SukiSU_KernelPatch_patch
    endif
endif

CC = $(TARGET_COMPILE)gcc
LD = $(TARGET_COMPILE)ld

# Define include directories for SukiSU_KernelPatch_patch
INCLUDE_DIRS := kernel/. kernel/include kernel/patch/include kernel/linux/include kernel/linux/arch/arm64/include kernel/linux/tools/arch/arm64/include

# Create include flags
INCLUDE_FLAGS := $(foreach dir,$(INCLUDE_DIRS),-I$(KP_DIR)/$(dir))

# Add local directory to include path as fallback
INCLUDE_FLAGS += -I.

objs := midi.o

all: midi.kpm

midi.kpm: ${objs}
	${CC} -r -o $@ $^

%.o: %.c
	${CC} $(CFLAGS) $(INCLUDE_FLAGS) -c -O2 -o $@ $<

.PHONY: clean
clean:
	rm -rf *.kpm
	find . -name "*.o" | xargs rm -f