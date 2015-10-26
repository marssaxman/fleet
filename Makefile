default: lib

lib: libfleet.a

libfleet.a: startc/libstartc.a kernel/libkernel.a libc/libc.a
	cat mergelibs | ar -M

startc/libstartc.a:
	@cd startc && $(MAKE) -s

kernel/libkernel.a:
	@cd kernel && $(MAKE) -s

libc/libc.a:
	@cd libc && $(MAKE) -s

clean:
	@cd startc && $(MAKE) -s clean
	@cd kernel && $(MAKE) -s clean
	@cd libc && $(MAKE) -s clean
	-@rm -f libfleet.a

.PHONY: clean


