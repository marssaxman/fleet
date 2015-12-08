
default: lib
all: lib test
lib: libc.a
test: runtests libc.so

include srctree.mk
include ../kernel/target.mk

LIBCFLAGS := $(CFLAGS) -MD -MP -Werror
LIBCFLAGS += -iquote src
LIBCFLAGS += -isystem include -isystem ../kernel/include
TESTCFLAGS := $(LIBCFLAGS) -fPIC -DTESTSUITE
TESTCFLAGS += -include "test/testsuite.h" -iquote test
TESTLDFLAGS := -nodefaultlibs -march=$(ARCH) -m$(BITS) -static-libgcc
HOSTCFLAGS := -march=$(ARCH) -m$(BITS) -MD -MP -std=c99 -Werror
HOSTCFLAGS += -iquote src -iquote ../

-include $(call findtype, c, obj)

libc.a: $(call objs, c, src, obj/lib)
	ar rcs $@ $^

libc.so: $(call objs, c, src, obj/test)
	$(CC) $^ -shared $(TESTLDFLAGS) -o $@

runtests: $(call objs, c, test, obj/host)
	gcc $(HOSTCFLAGS) -rdynamic -o $@ $^ -ldl

obj/lib/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(LIBCFLAGS) -c $< -o $@"
	@$(CC) $(LIBCFLAGS) -c $< -o $@

obj/test/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(TESTCFLAGS) -c $< -o $@"
	@$(CC) $(TESTCFLAGS) -c $< -o $@

obj/host/%.o: test/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(HOSTCFLAGS) -c $< -o $@"
	@$(CC) $(HOSTCFLAGS) -c $< -o $@

check: test
	./runtests

clean:
	-@rm -f libc.a runtests libc.so
	-@rm -rf $(call findtype, o d, obj)

.PHONY: check clean

