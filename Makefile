ifndef TARGET_COMPILE
    $(error TARGET_COMPILE not set)
endif

ifndef KP_DIR
    # If KP_DIR is not set, try to find KernelPatch in common locations
    ifneq ("$(wildcard ../KernelPatch)", "")
        KP_DIR = ../KernelPatch
    else ifneq ("$(wildcard ../../KernelPatch)", "")
        KP_DIR = ../../KernelPatch
    else ifneq ("$(wildcard ../../../KernelPatch)", "")
        KP_DIR = ../../../KernelPatch
    else
        # Default to a common location
        KP_DIR = ../KernelPatch
    endif
endif

CC = $(TARGET_COMPILE)gcc
LD = $(TARGET_COMPILE)ld

# Define include directories
INCLUDE_DIRS := . include patch/include linux/include linux/arch/arm64/include linux/tools/arch/arm64/include

# Create include flags
INCLUDE_FLAGS := $(foreach dir,$(INCLUDE_DIRS),-I$(KP_DIR)/kernel/$(dir))

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