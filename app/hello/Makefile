NAME := "hello"
LIBS := vga
CCFLAGS := -Wall -g
LDFLAGS :=

all: executable

executable:
	@NAME="$(NAME)" LIBS="$(LIBS)" \
	CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" \
	../../build/exe.sh

clean:
	@rm -rf $(NAME).bin ./obj/*

run: executable
	@../run.sh $(NAME).bin

debug: executable
	@../debug.sh $(NAME).bin









