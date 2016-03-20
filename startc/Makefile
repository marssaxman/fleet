include ../build/target.mk
include ../build/srctree.mk

default: libstartc.a

libstartc.a: $(call listobjs, s, src, obj)
	@mkdir -p $(@D)
	ar rcs $@ $^

obj/%.o: src/%.s
	@mkdir -p $(@D)
		as $(ASFLAGS) -o $@ $<

clean:
	-@rm -rf libstartc.a obj
.PHONY: clean

