default: lib

lib: libfleet.a

libfleet.a:
	@cd startc && $(MAKE) -s
	@cd kernel && $(MAKE) -s
	@cd libc && $(MAKE) -s
	cat mergelibs | ar -M

clean:
	-@rm -f libfleet.a
	-@rm -f startc/libstartc.a kernel/libkernel.a libc/libc.a

.PHONY: clean


