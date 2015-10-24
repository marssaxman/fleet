KERNEL_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(KERNEL_DIR)/target.mk
CFLAGS += -isystem $(KERNEL_DIR)/include
LDLIBS += -lkernel
LDFLAGS += -L$(KERNEL_DIR)

