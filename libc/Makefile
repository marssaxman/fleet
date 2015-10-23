include target.mk
CFLAGS += -iquote . -isystem include
CFLAGS += -MD -MP

SRCS := $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJS := $(patsubst src%.c,obj/rel%.o,$(SRCS))
DEPS := $(OBJS%.o=%.d)
TARGET := libc.a

default: lib
all: lib suite

lib: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

-include $(DEPS)

obj/rel/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# The library source files contain embedded test suites which are only
# compiled in when the symbol TEST_SUITE_MODE is defined.
# We will link these sources into a locally hosted executable.
# It must still compile with the same target flags as the library files.

TESTOBJS := $(patsubst obj/rel%,obj/test%,$(OBJS))
TESTDEPS := $(TESTOBJS%.o=%.d)
HOSTSRCS := $(wildcard test/*.c)
HOSTOBJS := $(patsubst test%.c,obj/host%.o,$(HOSTSRCS))
HOSTDEPS := $(HOSTOBJS%.o=%.d)
TESTHOST := runtests
HOSTCFLAGS += -iquote . -march=$(ARCH) -m$(BITS) -MD -MP -std=c99

-include $(TESTDEPS)
-include $(HOSTDEPS)

obj/test/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) -DTEST_SUITE_MODE \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -DTEST_SUITE_MODE -c $< -o $@

obj/host/%.o: test/*.c Makefile
	@mkdir -p $(@D)
	$(CC) $(HOSTCFLAGS) -c $< -o $@

$(TESTHOST): $(HOSTOBJS) $(TESTOBJS)
	gcc -march=$(ARCH) -m$(BITS) -o $@ $^

suite: $(TESTHOST)

test: $(TESTHOST)
	$(abspath $(TESTHOST))

clean:
	-@rm -f $(TARGET) $(TESTHOST)
	-@rm -rf obj/*

.PHONY: clean test


