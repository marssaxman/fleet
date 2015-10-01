#ifndef _PCIBUS_H
#define _PCIBUS_H

#include <sys/pci.h>

// Unique location of a PCI device: bus:slot.function
struct _pcibus_addr
{
	uint8_t bus;
	uint8_t slot;
	uint8_t function;
};

// Unique identifier for a PCI device control protocol
struct _pcibus_id
{
	uint16_t vendor_id;
	uint16_t device_id;
};

// Search the PCI bus, looking for devices and secondary buses. Configure each
// bridge we find, then invoke the callback for each device we find.
typedef void (*_pcibus_callback)(struct _pcibus_addr, struct _pcibus_id);
extern void _pcibus_init(_pcibus_callback);

#endif //_PCIBUS_H
