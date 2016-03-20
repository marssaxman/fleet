default: libfleet.a

include ../build/srctree.mk
include ../build/target.mk
-include $(call findtype, d, obj)

CFLAGS+=-MD -MP -Werror -g -fvisibility=hidden -Wswitch
CFLAGS+=-Isrc -isystem ../startc/include

libfleet.a: $(call listobjs, c s, src, obj)
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
	-@rm -rf libfleet.a obj
.PHONY: clean

