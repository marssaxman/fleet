#ifndef _SYS_DRIVER_H
#define _SYS_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/pci.h>

// Each device is identified by its bus, slot, and function numbers.
struct pci_address
{
	uint8_t bus;		// 0..255
	uint8_t slot;		// 0..31
	uint8_t function;	// 0..7
} __attribute__((packed));

// The configuration structure includes a variety of fields containing codes
// identifying a device type. This struct collects them all in one place.
struct pci_device_id
{
	uint16_t vendor;
	uint16_t device;
	uint8_t class_code;
	uint8_t subclass;
	uint8_t prog_if;
	uint8_t header_type;
} __attribute__((packed));

// The device driver interface is a structure containing type fields and
// function pointers which will allow the kernel to identify and operate the
// appropriate driver when it discovers a PCI device. Each driver must define
// such a structure, and it must be assigned to the read-only data segment
// named ".driver", so the linker will concatenate them into a single array.
struct driver
{
	// Provide a name to use when generating device handles.
	const char *name;
	// The driver only cares about devices which have IDs matching the values
	// in this structure. Use 0xFFFF or 0xFF to match any value in that field.
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

// Accessors for device configuration registers.
extern uint8_t pci_config_read8(struct pci_address, uint8_t field_offset);
extern uint16_t pci_config_read16(struct pci_address, uint8_t field_offset);
extern uint32_t pci_config_read32(struct pci_address, uint8_t field_offset);

#endif //_SYS_DRIVER_H
