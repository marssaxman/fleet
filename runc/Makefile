SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=obj/%.o)
DEP = $(wildcard obj/*.d)
LIB = librunc.a

include flags.mk
CCFLAGS += -MD -MP

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

