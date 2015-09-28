NAME := "test-idt"
LIBS := vga interrupt
CCFLAGS := -Wall -g
LDFLAGS :=

all: executable

executable:
	@NAME="$(NAME)" LIBS="$(LIBS)" \
	CCFLAGS="$(CCFLAGS)" LDFLAGS="$(LDFLAGS)" \
	../../build/exe.sh

clean:
	@rm -rf $(NAME).iso ./obj/*











