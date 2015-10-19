SRC = $(wildcard src/*.c src/*.s)
OBJ = $(addsuffix .o, $(basename $(SRC:src/%=obj/%)))
DEP = $(wildcard obj/*.d)
LIB = libcore.a

CCFLAGS = -ffreestanding -nostdlib -nostdinc
CCFLAGS += -march=i686 -m32
CCFLAGS += -isystem ../startc/include -isystem ../runc/include
CCFLAGS += -Iinclude
CCFLAGS += -std=c99 -MD -MP

ASFLAGS = -march=i686 --32 --strip-local-absolute

all: $(LIB)
.PHONY: all clean test

-include $(OBJ:%.o=%.d)

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)

obj/%.o: src/%.c
	@mkdir -p obj
	@echo "cc \$$(CCFLAGS) -c $< -o $@"
	@cc $(CCFLAGS) -c $< -o $@

obj/%.o: src/%.s
	@mkdir -p obj
	@echo "as \$$(ASFLAGS) -o $@ $<"
	@as $(ASFLAGS) -o $@ $<

clean:
	-rm -f $(LIB) $(OBJ) $(DEP)

test: $(LIB)
	cd test && $(MAKE) run

