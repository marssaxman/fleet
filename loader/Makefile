default: fleet

include ../build/srctree.mk
-include $(call findtype, d, obj)

CPPFLAGS:=-MD -MP -Wall -g -static -Werror -Isrc
CXXFLAGS:=$(CPPFLAGS) -std=c++11
CFLAGS:=$(CPPFLAGS) -std=c99

fleet: $(call cxx_objs, src, obj)
	$(CC) $^ $(LDLIBS) -o $@

obj/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm fleet
	@rm -rf obj
.PHONY: clean

install: fleet
	cp fleet /usr/bin/
.PHONY: install









