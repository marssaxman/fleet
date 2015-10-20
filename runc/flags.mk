RUNC_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(RUNC_DIR)/../startc/flags.mk
CRT_LIBS += $(RUNC_DIR)/librunc.a

CCFLAGS += -isystem $(RUNC_DIR)/include
LDFLAGS += --library-path=$(RUNC_DIR)
LDFLAGS += --library=runc

