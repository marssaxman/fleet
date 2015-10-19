SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
LIB = librunc.a
CCFLAGS = -ffreestanding -nostdlib -nostdinc -m32 -std=c99 -march=i686

all: $(LIB)
.PHONY: all clean test

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)

obj/%.o: src/%.c
	@mkdir -p obj
	cc $(CCFLAGS) -Iinclude -I../startc/include -c $< -o $@

clean:
	-rm -f $(LIB) $(OBJ)

test: $(LIB)
	cd test && $(MAKE) run

