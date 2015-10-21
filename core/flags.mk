CORE_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
LIBC_DIR := $(abspath $(dir $(CORE_DIR)))
include $(LIBC_DIR)/flags.mk


