# Link your project against libc by including this file in your Makefile.
LIBC := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
STARTC := $(realpath "$(LIBC)/../")
include $(STARTC)/link.mk
CFLAGS += -isystem $(LIBC)/include
LDLIBS += -lc
LDFLAGS += -L$(LIBC)

