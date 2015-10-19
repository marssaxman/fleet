
static void debug_write(const char *msg)
{
	// Use the port E9 hack to write data to the emulator's debug console.
	while (*msg) {
		__asm__("outb %%al,%%dx;": :"d"(0xE9), "a"(*msg++));
	}
}

void main()
{
	debug_write("Hello, world!\n");
	while (1);
}

