# Standard build script for runc submodules, to be invoked by the stub
# makefile in each module directory.

# This file is currently the last one on the makefile list, so we can use it
# to get the base path for the runc library.
RUNC := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

# The previous item on the list is the makefile that included us, which lives
# in the module's directory, so we can use that to find out what the module's
# name is and where its files live.
MAKEFILE_COUNT = $(words $(MAKEFILE_LIST))
DUMMY_LIST = $(addprefix "foo ", $(MAKEFILE_LIST))
PREV_MAKEFILE = $(word $(MAKEFILE_COUNT), $(DUMMY_LIST))
MODULE := $(realpath $(dir $(PREV_MAKEFILE)))

# Load the standard target settings
include $(RUNC)/../target.mk

# Expose our own system headers and those provided by startc.
CFLAGS += -isystem $(RUNC)/include -isystem $(FLEETBASE)/startc/include

MODNAME := $(notdir $(MODULE:%/=%))
OUTLIB := $(MODULE)/$(addprefix lib, $(MODNAME:%=%.a))
SRCS := $(wildcard $(MODULE)/src/*.c)
OBJS := $(subst /src/,/obj/,$(SRCS:%.c=%.o))

# We will use auto dependency tracking.
DEPS := $(OBJS:%.o=%.d)
CFLAGS += -MD -MP
-include $(DEPS)

all: lib $(OBJS)

lib: $(OUTLIB)

$(OUTLIB): $(OBJS)
	ar rcs $@ $^

obj/%.o: src/%.c
	@mkdir -p obj
	echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -f $(OUTLIB) $(OBJS) $(DEPS)

.PHONY: clean compile

