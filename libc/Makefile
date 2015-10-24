
default: lib
all: lib test
lib: libc.a
test: runtests

include srctree.mk
include ../kernel/target.mk

-include $(call findtype, c, obj)

libc.a: $(call objs, c, src, obj/lib)
	ar rcs $@ $^

runtests: $(call objs, c, src, obj/test) $(call objs, c, test, obj/host)
	gcc -march=$(ARCH) -m$(BITS) -o $@ $^

LIBCFLAGS := $(CFLAGS) -MD -MP
LIBCFLAGS += -iquote src
LIBCFLAGS += -isystem include -isystem ../kernel/include
obj/lib/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(LIBCFLAGS) -c $< -o $@"
	@$(CC) $(LIBCFLAGS) -c $< -o $@

TESTCFLAGS := $(LIBCFLAGS) -DTESTSUITE -include "test/testsuite.h"
obj/test/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(TESTCFLAGS) -c $< -o $@"
	@$(CC) $(TESTCFLAGS) -c $< -o $@

HOSTCFLAGS := -iquote test -march=$(ARCH) -m$(BITS) -MD -MP -std=c99
obj/host/%.o: test/*.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(HOSTCFLAGS) -c $< -o $@"
	@$(CC) $(HOSTCFLAGS) -c $< -o $@

check: test
	./runtests

clean:
	-@rm -f libc.a runtests
	-@rm -rf $(call findtype, o d, obj)

.PHONY: check clean

