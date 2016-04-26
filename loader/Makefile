default: loader

include ../build/srctree.mk
-include $(call findtype, d, obj)

CPPFLAGS:=-MD -MP -Wall -g -static -Werror -Isrc
CXXFLAGS:=$(CPPFLAGS) -std=c++11
CFLAGS:=$(CPPFLAGS) -std=c99

loader: bin/fleet
.PHONY: loader

bin/fleet: $(call cxx_objs, src, obj)
	@mkdir -p $(@D)
	$(CC) $^ $(LDLIBS) -o $@

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf bin obj
.PHONY: clean

install: loader
	cp bin/fleet /usr/bin/
.PHONY: install









