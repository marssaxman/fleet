# Link your project against runc by including this file in your Makefile.
RUNC := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
STARTC := $(realpath "$(RUNC)/../")
include $(STARTC)/link.mk
CFLAGS += -isystem $(RUNC)/include
LDLIBS += -lrunc
LDFLAGS += -L$(RUNC)

