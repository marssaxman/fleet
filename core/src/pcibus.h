#ifndef _PCIBUS_H
#define _PCIBUS_H

#include <sys/pci.h>

// Enumerate the PCI bus looking for devices. When one is found, invoke the
// callback, providing it with the bus, slot, vendor, and device IDs.
typedef void (*_pcibus_scan_callback)
		(uint8_t bus, uint8_t slot, uint16_t vendorID, uint16_t slotID);
extern void _pcibus_scan(_pcibus_scan_callback);

#endif //_PCIBUS_H
