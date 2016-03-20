# Include this from your Makefile to build your executable as a bootable kernel
# image for i386 using libfleet's C runtime and standard library.
FLEETDIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST)))/..)
include $(FLEETDIR)/build/target.mk
CFLAGS += -isystem $(FLEETDIR)/libc/include
LDLIBS += -lc -lkernel
LDFLAGS += -L$(FLEETDIR)/libc -L$(FLEETDIR)/kernel
LDFLAGS += -T $(FLEETDIR)/build/linker.ld

