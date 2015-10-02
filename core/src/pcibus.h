#ifndef _PCIBUS_H
#define _PCIBUS_H

#include <sys/pci.h>
#include <sys/driver.h>

// Search the PCI bus, looking for devices and secondary buses. Configure each
// bridge we find, then invoke the callback for each device we find.
typedef void (*_pcibus_callback)(struct pci_address, struct pci_device_id);
extern void _pcibus_init(_pcibus_callback);

#endif //_PCIBUS_H
