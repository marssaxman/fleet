default: lib
all: lib
lib: libkernel.a

include srctree.mk
include ../startc/link.mk

libkernel.a: $(call objs, c, src, obj)
	ar rcs $@ $^

obj/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -rf libkernel.a $(call findtype, o d, obj)

.PHONY: clean

