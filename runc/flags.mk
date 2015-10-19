RUNC_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(RUNC_DIR)/../startc/flags.mk
CCFLAGS += -isystem $(RUNC_DIR)/include
LDFLAGS += --library-path=$(RUNC_DIR)
LDFLAGS += --library=runc

