CORE_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(CORE_DIR)/../runc/flags.mk
CRT_LIBS += $(CORE_DIR)/libcore.a

CCFLAGS += -isystem $(CORE_DIR)/include
LDFLAGS += --library-path=$(CORE_DIR)
LDFLAGS += --library=core


