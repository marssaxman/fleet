SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
DEP = $(wildcard obj/*.d)
LIB = librunc.a

# Build self-contained code with no dependencies on the host system.
CCFLAGS = -ffreestanding -nostdlib -nostdinc
# Target is a 32-bit x86 machine, regardless of the host's architecture.
CCFLAGS += -march=i686 -m32
# Include the startc headers in our system search path.
CCFLAGS += -isystem ../startc/include
# Include runc's own public headers in the search path.
CCFLAGS += -Iinclude
# Use C99 and generate dependency files.
CCFLAGS += -std=c99 -MD -MP

all: $(LIB)
.PHONY: all clean test

-include $(OBJ:%.o=%.d)

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)

obj/%.o: src/%.c
	@mkdir -p obj
	@echo "cc \$$(CCFLAGS) -c $< -o $@"
	@cc $(CCFLAGS) -c $< -o $@

clean:
	-rm -f $(LIB) $(OBJ) $(DEP)

test: $(LIB)
	cd test && $(MAKE) run

