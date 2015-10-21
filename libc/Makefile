CFLAGS += -Isrc -isystem include
CFLAGS += -MD -MP

SRCS := $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJS := $(patsubst src%.c,obj/rel%.o,$(SRCS))
DEPS := $(OBJS%.o=%.d)
TARGET := libc.a

default: lib
all: lib suite

include ../startc/link.mk

lib: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $(OBJS)

-include $(DEPS)

obj/rel/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# The library source files contain embedded test suites which are only
# compiled in when the symbol TEST_SUITE_MODE is defined.
# We will link these sources into a locally hosted executable.
# We add test/ to the include path so the test suites can use the check and
# result functions.
# Unlike the library, the test host is a normal executable linked against the
# host system's libc. It must still compile with the same target flags as the
# library files and run as a 32-bit executable even on a 64-bit machine.

TESTOBJS := $(patsubst obj/rel%,obj/test%,$(OBJS))
TESTDEPS := $(TESTOBJS%.o=%.d)
HOSTSRCS := $(wildcard test/*.c)
HOSTOBJS := $(patsubst test%.c,obj/host%.o,$(HOSTSRCS))
HOSTDEPS := $(HOSTOBJS%.o=%.d)
TESTHOST := runlibctests
HOSTCFLAGS += -Itest $(ARCHFLAGS) -MD -MP -std=c99

-include $(TESTDEPS)
-include $(HOSTDEPS)

obj/test/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) -DTEST_SUITE_MODE \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -DTEST_SUITE_MODE -Itest -c $< -o $@

obj/host/%.o: test/*.c Makefile
	@mkdir -p $(@D)
	$(CC) $(HOSTCFLAGS) -c $< -o $@

$(TESTHOST): $(HOSTOBJS) $(TESTOBJS)
	@echo "HOSTOBJS: $(HOSTOBJS)"
	gcc $(ARCHFLAGS) -o $(TESTHOST) $(HOSTOBJS) $(TESTOBJS)

suite: $(TESTHOST)

test: $(TESTHOST)
	$(abspath $(TESTHOST))

clean:
	-@rm -f $(TARGET) $(TESTHOST)
	-@rm -rf obj/*

.PHONY: clean test


