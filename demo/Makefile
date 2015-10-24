# Identify our input components and our output file
BIN := hello.bin
SRCS := $(wildcard *.c)
OBJS := $(addsuffix .o, $(basename $(SRCS)))

include ../libc/target.mk
CFLAGS += -isystem ../libc/include
LDFLAGS += -L../ -T ../startc/linker.ld
LDLIBS += -lfleet

all: $(BIN)

$(BIN): $(OBJS) ../libfleet.a
	ld -o $@ $(LDFLAGS) --start-group $^ $(LDLIBS) --end-group

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

../libfleet.a:
	@cd .. && $(MAKE) -s

clean:
	-@rm -f $(BIN) $(OBJS)

run: $(BIN)
	qemu-system-i386 -kernel $(BIN) -monitor stdio -nographic -debugcon stdio

.PHONY: all clean run

