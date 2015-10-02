#ifndef _SYS_DRIVER_H
#define _SYS_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/pci.h>

// The device driver interface is a structure containing type fields and
// function pointers which will allow the kernel to identify and operate the
// appropriate driver when it discovers a PCI device. Each driver must define
// such a structure, and it must be assigned to the read-only data segment
// named ".driver", so the linker will concatenate them into a single array.

// PCI devices can be identified through a hierarchy of codes. The driver
// should supply values that suitable devices should match. If the field is
// not important, use 0xFF or 0xFFFF to indicate that any match is acceptable.
struct pci_device_id
{
	uint16_t vendor;
	uint16_t device;
	uint8_t class_code;
	uint8_t subclass;
	uint8_t prog_if;
};

struct driver
{
	// Provide a name to use when generating device handles.
	const char *name;
	// Identify the IDs of devices this driver can manage.
	struct pci_device_id id;
	// Inspect the device. If it has the correct type, initialize it and return
	// a reference address for future use. Otherwise, return zero to indicate
	// that the driver does not support this particular device.
	void *(*probe)(struct pci_address);
	// The interrupt line this device uses has been fired. Check its state.
	void (*check)(struct pci_address, void *reference);
	// The device has been removed. Clean up after its disappearance.
	void (*remove)(void *reference);
	// The device is no longer needed. Turn it off and clean up.
	void (*shutdown)(struct pci_address, void *reference);
};

#define DRIVER_ATTR __attribute__((section(".driver")))
#define DEFINE_DRIVER(name) extern const struct driver name DRIVER_INFO_ATTR

#endif //_SYS_DRIVER_H
