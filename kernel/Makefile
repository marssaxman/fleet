default: libkernel.a

include ../build/rules.mk

CPPFLAGS+=-Wswitch
CPPFLAGS+=-isystem include -isystem headers

libkernel.a: $(call listobjs, c s, src, obj)
	@mkdir -p $(@D)
	@echo "ar rcs $@ \$$^"
	@ar rcs $@ $^

test.bin: libkernel.a
	ld -o $@ $(LDFLAGS) -T ../build/linker.ld --start-group $^ --end-group

test: test.bin
	qemu-system-i386 -kernel $^ -S -nographic -no-reboot -nodefaults \
		-monitor stdio -debugcon stdio -serial pty
.PHONY: test

