# Identify our input components and our output file
BIN := hello.bin
SRCS := $(wildcard *.c)
OBJS := $(addsuffix .o, $(basename $(SRCS)))

include ../build/link.mk

all: $(BIN)

$(BIN): $(OBJS)
	ld -o $@ $(LDFLAGS) --start-group $^ $(LDLIBS) --end-group

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	-@rm -f $(BIN) $(OBJS)

run: $(BIN)
	qemu-system-i386 -kernel $(BIN) -monitor stdio -nographic -debugcon stdio

.PHONY: all clean run

