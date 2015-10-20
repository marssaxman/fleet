CFLAGS += -isystem include
CFLAGS += -MD -MP

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src%.c,obj%.o,$(SRCS))
DEPS := $(OBJS%.o=%.d)
TARGET := librunc.a

default: $(TARGET)

include ../startc/link.mk

$(TARGET): $(OBJS)
	ar rcs $@ $(OBJS)

-include $(DEPS)

obj/%.o: src/%.c Makefile
	@mkdir -p obj
	@echo "$(CC) \$$(CFLAGS) -c $< -o $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -f $(TARGET) $(OBJS)

.PHONY: clean


