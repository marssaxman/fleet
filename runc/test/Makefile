SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c,obj/%.o,$(SRC))
BIN = test.bin
CCFLAGS = -ffreestanding -nostdlib -nostdinc -m32 -std=c99 -march=i686
LDFLAGS = -static -nostdlib -L../ -lrunc -L../../startc -lstartc

all: $(BIN)
.PHONY: all clean run

$(BIN): $(OBJ)
	ld $(LDFLAGS) -T linker.ld -o $@ $(OBJ)

obj/%.o: src/%.c
	@mkdir -p obj
	cc $(CCFLAGS) -I../include -I../../startc/include -Isrc -c $< -o $@

clean:
	-rm -f $(BIN) $(OBJ)

run: $(BIN)
	./run.sh $(BIN)

