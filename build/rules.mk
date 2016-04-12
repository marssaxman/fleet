include ../build/srctree.mk
include ../build/target.mk
-include $(call findtype, d, obj)

CPPFLAGS+=-MD -MP -Werror -g -fvisibility=hidden -Wswitch
CPPFLAGS+=-Isrc
CFLAGS+=-std=c99
CXXFLAGS+=-std=c++11

obj/%.o: src/%.c
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CPPFLAGS) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

obj/%.o: src/%.cpp
	@mkdir -p $(@D)
	@echo "$(CXX) \$$(CPPFLAGS) \$$(CXXFLAGS) -c $< -o $@"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

obj/%.o: src/%.s
	@mkdir -p $(@D)
	as $(ASFLAGS) -o $@ $<

clean:
	-@rm -f *.a *.bin
	-@rm -rf obj
.PHONY: clean

