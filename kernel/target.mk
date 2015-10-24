KERNEL_DIR := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(KERNEL_DIR)/../startc/target.mk

