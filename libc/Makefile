include ../build/target.mk
include ../build/srctree.mk
CFLAGS+=-MD -MP -Werror -g -fvisibility=hidden -Wswitch
CFLAGS+=-Isrc -isystem include

default: libc.a

libc.a: $(call listobjs, c s, src, obj)
	@mkdir -p $(@D)
	@echo "ar rcs $@ \$$^"
	@ar rcs $@ $^

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.s
	@mkdir -p $(@D)
	as $(ASFLAGS) -o $@ $<

clean:
	-@rm -rf libc.a obj
.PHONY: clean


