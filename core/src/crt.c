#include <i386/cpu.h>
#include "sys/log.h"
#include "interrupt.h"
#include "pcibus.h"

// Somewhere out there, the user has implemented a main function.
extern void main(void);

static void log_pci_device(
		uint8_t bus, uint8_t slot, uint16_t vendorID, uint16_t deviceID)
{
	_log_printf("found PCI device type %hd:%hd, at %hd:%hd\n",
			vendorID, deviceID, bus, slot);
}

// Main entrypoint invoked by the _start function when the assembly bootstrap
// is ready to hand things over to the C world.
void _crt(void)
{
	_log_init();
	_interrupt_init();
	_pcibus_scan(log_pci_device);
	_sti();
	main();
	while (1) {}
}

