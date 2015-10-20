# Standard build script for runc submodules.
# The last word on the makefile list is this rules.mk, so we can find the
# runc library's root directory.
RUNC := $(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))

# We expect that the module which included this makefile has pulled the same
# trick and gotten its own $(MODULE) path.

# Load the standard target settings
include $(RUNC)/../target.mk

# Expose our own system headers and those provided by startc.
CFLAGS += -isystem $(RUNC)/include -isystem $(FLEETBASE)/startc/include

MODNAME := $(notdir $(MODULE:%/=%))
LIBNAME := $(addprefix lib, $(addsuffix .a, $(MODNAME)))
OUTLIB := $(MODULE)$(LIBNAME)
SRCS := $(wildcard $(MODULE)src/*.c)
OBJS := $(subst /src/,/obj/,$(basename $(SRCS))).o

# We will use auto dependency tracking.
DEPS := $(OBJS:%.o=%.d)
CFLAGS += -MD -MP
-include $(DEPS)

all: $(OUTLIB)

$(OUTLIB): $(OBJS)
	ar rcs $@ $^

obj/%.o: src/%.c
	@mkdir -p obj
	echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -f $(OUTLIB) $(OBJS) $(DEPS)

.PHONY: clean compile

