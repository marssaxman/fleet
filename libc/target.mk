LIBC_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(LIBC_DIR)/../kernel/target.mk

