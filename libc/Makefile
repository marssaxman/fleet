include target.mk
CFLAGS += -iquote . -isystem include
CFLAGS += -MD -MP

# function (map function,list):
#  Apply the function to each item in the list.
map = $(foreach item,$(2),$(call $(1),$(item)))

# function (list dir):
#  List the directory and its contents, recursively.
list = $(strip $(1) $(call map,list,$(wildcard ${1:/=}/*)))

# function (find pattern,dir):
#  Find all of the items within this directory that match the pattern.
find = $(filter $(1),$(call list,$(2)))

# function (findtype suffixes,dir):
#  Find all of the files in this directory with one of these type extensions.
findtype = $(call find,$(addprefix %.,$(1)),$(2))

# function (setprefix old,new,names)
#  Filter items matching old prefix, then substitute new prefix.
setprefix = $(patsubst $(strip $(1))%,$(strip $(2))%, $(filter $(1)%, $(3)))

# function (setsuffix suffix,names):
#  Drop the old suffix and add a new one.
setsuffix = $(addsuffix $(1),$(basename $(2)))

# function (objfiles srcdir,objdir,srcfiles)
#  List the corresponding object files for each source file in the list,
#  mapping the source file hierarchy to an equivalent object file hierarchy.
objfiles = $(call setsuffix,.o, $(call setprefix,$(1),$(2),$(3)))

# Find our source files under src/. Generate the corresponding object file
# names under obj/, using a parallel directory hierarchy. Generate dep file
# names to live alongside the object files.
SRCS := $(call findtype, c, src)
OBJS := $(call objfiles, src, obj/rel, $(SRCS))
DEPS := $(OBJS.o=.d)
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
	-@rm -rf $(call findtype, o d, obj)

.PHONY: clean test


