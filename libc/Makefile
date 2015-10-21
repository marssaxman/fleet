CFLAGS += -Isrc -isystem include
CFLAGS += -MD -MP

SRCS := $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJS := $(patsubst src%.c,obj%.o,$(SRCS))
DEPS := $(OBJS%.o=%.d)
TARGET := libc.a

default: $(TARGET)

include ../startc/link.mk

$(TARGET): $(OBJS)
	ar rcs $@ $(OBJS)

-include $(DEPS)

obj/%.o: src/%.c Makefile
	@mkdir -p $(@D)
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -f $(TARGET) $(OBJS) $(DEPS) $(wildcard obj/*.o) $(wildcard obj/*.d)
	@$(MAKE) -s -C test clean

suite:
	$(MAKE) -s -C test default

test:
	$(MAKE) -s -C test run

.PHONY: clean


