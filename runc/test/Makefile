# Build the library in test-suite mode so we can exercise the tests without
# having to boot up a VM every time. Source files guard their test sections
# behind RUN_TESTSUITE checks, so we'll define that symbol and throw in the
# header which defines our CHECK macros to boot. This a completely different
# sort of build target than our usual one, so all the intermediate files will
# live under this test directory instead of in the project root.


CTARGET := -march=i686 -m32 -std=c99

TESTDIR := $(realpath .)
ROOTDIR := $(realpath ..)
STARTCDIR := $(realpath ../../startc)
LIBSRCS := $(wildcard $(ROOTDIR)/src/*.c)
LIBOBJS := $(patsubst $(ROOTDIR)/src%.c,$(TESTDIR)/obj%.o,$(LIBSRCS))
LIBDEPS := $(LIBOBJS%.o=%.d)
APPSRCS := $(wildcard $(TESTDIR)/src/*.c)
APPOBJS := $(patsubst $(TESTDIR)/src/%.c,$(TESTDIR)/obj/%.o,$(APPSRCS))
APPDEPS := $(APPOBJS%.o=%.d)
TESTAPP := testsuite.bin

default: suite

-include $(LIBDEPS)
-include $(APPDEPS)

# Disable access to system headers when compiling the library files, which
# are designed to operate in a freestanding environment. Define a symbol whose
# absence normally prevents the test code from being compiled.
LIB_CFLAGS := $(CTARGET) -nostdinc -MD -MP -Wall
LIB_CFLAGS += -isystem $(STARTCDIR)/include -isystem $(ROOTDIR)/include
LIB_CFLAGS += -D TEST_SUITE_MODE

$(TESTDIR)/obj/%.o: $(ROOTDIR)/src/%.c Makefile
	@mkdir -p obj
	@echo "$(CC) \$$(LIB_CFLAGS) -c $< -o $@"
	@$(CC) $(LIB_CFLAGS) -c $< -o $@

# runc doesn't implement the four special gcc-required string functions -
# those belong to startc. We can't link against startc in this locally hosted
# app, so we will just assemble that one file and throw it in when we build
# the test lib.
STARTSTR := $(STARTCDIR)/obj/string.o

$(STARTSTR): $(STARTCDIR)/src/string.s
	$(AS) -march=i686 --32 --strip-local-absolute -o $@ $<

# Compile the application functions in a more normal fashion.
APP_CFLAGS := $(CTARGET) -MD -MP -Wall
APP_CFLAGS += -isystem $(STARTCDIR)/include -isystem $(ROOTDIR)/include
AP_CFLAGS += -I $(ROOTDIR)/src

$(TESTDIR)/obj/%.o: $(TESTDIR)/src/%.c Makefile
	@mkdir -p obj
	@echo "$(CC) \$$(APP_CFLAGS) -c $< -o $@"
	@$(CC) $(APP_CFLAGS) -c $< -o $@

suite: $(TESTAPP)

$(TESTAPP): $(APPOBJS) $(LIBOBJS) $(STARTSTR)
	gcc $(CTARGET) -o $(TESTAPP) $(APPOBJS) $(LIBOBJS) $(STARTSTR)

clean:
	-@rm -f $(TESTAPP) $(wildcard obj/*.o) $(wildcard obj/*.d)

test: $(TESTAPP)
	@$(realpath $(TESTAPP))

.PHONY: clean test

