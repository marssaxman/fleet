default: libkernel.a

include ../build/srctree.mk
include ../build/target.mk
-include $(call findtype, d, obj)

CFLAGS+=-MD -MP -Werror -g -fvisibility=hidden -Wswitch
CFLAGS+=-Isrc -isystem include
CFLAGS+=-isystem headers

libkernel.a: $(call listobjs, c s, src, obj)
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
	-@rm -f *.a *.bin
	-@rm -rf obj
.PHONY: clean

test.bin: libkernel.a
	ld -o $@ $(LDFLAGS) -T ../build/linker.ld --start-group $^ --end-group

test: test.bin
	qemu-system-i386 -kernel $^ -nographic -no-reboot \
		-monitor stdio -debugcon stdio \
		-chardev file,id=COM1,path=/home/mars/osdev/fleet/kernel/log.txt \
		-serial chardev:COM1
.PHONY: test

