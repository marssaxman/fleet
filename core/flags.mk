CORE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(CORE_DIR)/../runc/flags.mk
CCFLAGS += -isystem $(CORE_DIR)/include
LDFLAGS += --library-path=$(CORE_DIR)
LDFLAGS += --library=core


