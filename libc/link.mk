# Link your project against libc by including this file in your Makefile.
LIBC_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(LIBC_DIR)/target.mk
CFLAGS += -isystem $(LIBC_DIR)/include
LDLIBS += -lc
LDFLAGS += -L$(LIBC_DIR)

