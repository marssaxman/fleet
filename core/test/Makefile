SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)
DEP = $(wildcard obj/*.d)
BIN = test.bin

include ../flags.mk
CCFLAGS += -MD -MP

all: $(BIN)
.PHONY: all clean run

-include $(OBJ:%.o=%.d)

$(BIN): $(OBJ)
	ld $(OBJ) $(LDFLAGS) -o $@

obj/%.o: src/%.c
	@mkdir -p obj
	@echo "cc \$$(CCFLAGS) -c $< -o $@"
	@cc $(CCFLAGS) -c $< -o $@

clean:
	-rm -f $(BIN) $(OBJ) $(DEP)

run: $(BIN)
	./run.sh $(BIN)

