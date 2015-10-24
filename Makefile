default: lib
all: lib test
lib: libfleet.a
test: runtests

# Use libc's handy library of source-tree search & mapping functions
include libc/srctree.mk

include libc/link.mk
include startc/link.mk

libfleet.a: $(call objs, c, src, obj)
	ar rcs $@ $^

CFLAGS += -isystem libc/system
obj/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -rf libfleet.a $(call findtype, o d, obj)

.PHONY: clean

