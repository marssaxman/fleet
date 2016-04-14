default: libc.a

include ../build/rules.mk

CPPFLAGS+=-isystem include

libc.a: $(call listobjs, c s, src, obj)
	@mkdir -p $(@D)
	@echo "ar rcs $@ \$$^"
	@ar rcs $@ $^

