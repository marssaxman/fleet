SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)
DEP = $(wildcard obj/*.d)
BIN = test.bin

CCFLAGS = -ffreestanding -nostdlib -nostdinc
CCFLAGS += -march=i686 -m32
CCFLAGS += -isystem ../../startc/include -isystem ../include
CCFLAGS += -std=c99 -MD -MP

LDFLAGS = -static -nostdlib -L../ -lrunc -L../../startc -lstartc

all: $(BIN)
.PHONY: all clean run

-include $(OBJ:%.o=%.d)

$(BIN): $(OBJ)
	ld $(OBJ) $(LDFLAGS) -T linker.ld -o $@

obj/%.o: src/%.c
	@mkdir -p obj
	@echo "cc \$$(CCFLAGS) -c $< -o $@"
	@cc $(CCFLAGS) -c $< -o $@

clean:
	-rm -f $(BIN) $(OBJ) $(DEP)

run: $(BIN)
	./run.sh $(BIN)

