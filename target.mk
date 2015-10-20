# standard build target for the fleet project, defined by the startc runtime
FLEETBASE := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))
include $(FLEETBASE)/startc/target.mk

