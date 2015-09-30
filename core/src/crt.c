#include <i386/cpu.h>
#include "sys/log.h"
#include "interrupt.h"

// Somewhere out there, the user has implemented a main function.
extern void main(void);

// Main entrypoint invoked by the _start function when the assembly bootstrap
// is ready to hand things over to the C world.
void _crt(void)
{
	_log_init();
	_interrupt_init();
	_sti();
	main();
	while (1) {}
}

