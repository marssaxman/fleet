SOURCES := $(wildcard src/*.c)
HEADERS := $(wildcard src/*.h)
OBJECTS := $(patsubst src%.c,obj%.o,$(SOURCES))
TARGET := librunc.a

default: $(TARGET)

CFLAGS += -isystem include
include ../startc/link.mk

$(TARGET): $(OBJECTS)
	ar rcs $@ $(OBJECTS)

obj/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-@rm -f $(TARGET) $(OBJECTS)

.PHONY: clean


