default: lib
all: lib
lib: libkernel.a

include srctree.mk
include ../startc/target.mk

EXTS := c s
libkernel.a: $(call objs, $(EXTS), src, obj)
	ar rcs $@ $^

CFLAGS += -isystem ../startc/include -isystem include
obj/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.s
	@mkdir -p $(@D)
	as $(ASFLAGS) -o $@ $<

clean:
	-@rm -rf libkernel.a $(call findtype, o d, obj)

.PHONY: clean

