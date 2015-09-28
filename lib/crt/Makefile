NAME := "crt"

all: library

library:
	@NAME="$(NAME)" LIBS="$(LIBS)" \
	CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" \
	../../build/lib.sh

clean:
	@rm -rf lib$(NAME).a ./obj/*






