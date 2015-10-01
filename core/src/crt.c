#include <i386/cpu.h>
#include "sys/log.h"
#include "interrupt.h"
#include "pcibus.h"

// Somewhere out there, the user has implemented a main function.
extern void main(void);

static void log_pci_device(struct _pcibus_addr addr, struct _pcibus_id id)
{
	_log_printf("found PCI device type %hd:%hd, at %hhd:%hhd.%hhd\n",
			id.vendor_id, id.device_id, addr.bus, addr.slot, addr.function);
}

// Main entrypoint invoked by the _start function when the assembly bootstrap
// is ready to hand things over to the C world.
void _crt(void)
{
	_log_init();
	_interrupt_init();
	_pcibus_init(log_pci_device);
	_sti();
	main();
	while (1) {}
}

