default: lib

lib: libfleet.a

libfleet.a:
	@cd startc && $(MAKE) -s
	@cd kernel && $(MAKE) -s
	@cd libc && $(MAKE) -s
	cat mergelibs | ar -M

clean:
	-@rm -f libfleet.a

.PHONY: clean


