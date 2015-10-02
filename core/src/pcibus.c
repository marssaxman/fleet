#include <stdbool.h>
#include <stddef.h>
#include <sys/driver.h>
#include <sys/pci.h>
#include "pcibus.h"
#include "cpu.h"
#include "log.h"


#define PCIOFFSET(reg) offsetof(struct pci_header_common, reg)
#define READ8(addr,reg) pci_config_read8(addr, PCIOFFSET(reg))
#define READ16(addr,reg) pci_config_read16(addr, PCIOFFSET(reg))
#define READ32(addr,reg) pci_config_read32(addr, PCIOFFSET(reg))

static uint8_t bus_list[256];
static unsigned bus_count;

static void register_device(struct pci_address addr, uint16_t vendor_id)
{
	// Read the type fields for this device so we can identify the drivers
	// capable of handling it.
	struct pci_device_id id = {vendor_id};
	id.device = READ16(addr, device_id);
	id.class_code = READ8(addr, class_code);
	id.subclass = READ8(addr, subclass);
	id.prog_if = READ8(addr, prog_if);
	id.header_type = READ8(addr, header_type) & PCI_HEADER_TYPE_MASK;
	_log(PCIPROBE, "found PCI device at %hhd:%hhd.%hhd:\n"
			"\tvendor_id = %hd\n"
			"\tdevice_id = %hd\n"
			"\tclass_code = %hhd\n"
			"\tsubclass = %hhd\n"
			"\tprog_if = %hhd\n"
			"\theader_type = %hhd\n",
			addr.bus, addr.slot, addr.function,
			id.vendor, id.device,
			id.class_code, id.subclass,
			id.prog_if, id.header_type);
}

/*
static bool scan(struct pci_address addr)
{
	// Read this slot's vendor ID to see if there is a device attached.
	struct pci_device_id id;
	id.vendor = read16(addr, PCIOFFSET(vendor_id));
	if (0xFFFF == id.vendor) return false;
	id.device = read16(addr, PCIOFFSET(device_id));
	id.class_code = read8(addr, PCIOFFSET(class_code));
	id.subclass = read8(addr, PCIOFFSET(subclass));
	proc(addr, id);
	// Is this device a PCI bridge? If so, add its bus number to our list.
	uint8_t header_type = read8(addr, PCIOFFSET(header_type));
	header_type &= PCI_HEADER_TYPE_MASK;
	if (header_type != PCI_HEADER_TYPE_BRIDGE) return true;
	uint32_t off = offsetof(struct pci_config_bridge, secondary_bus_number);
	bus_list[bus_count++] = read8(addr, off);
	return true;
}
*/

static void scan_slot(uint8_t bus, uint8_t slot)
{
	// We found an active device at function 0 on this slot. Check the other
	// functions to see if any of them are active.
	struct pci_address addr = {bus, slot, 0};
	for (addr.function = 1; addr.function < 8; ++addr.function) {
		uint16_t vendor_id = READ16(addr, vendor_id);
		if (PCI_NO_DEVICE_ID == vendor_id) continue;
		register_device(addr, vendor_id);
	}
}

static void scan_bus(uint8_t bus)
{
	// Look at each slot on this bus.
	struct pci_address addr = {bus, 0, 0};
	for (addr.slot = 0; addr.slot < 32; ++addr.slot) {
		// Check the device ID to see if something actually exists here.
		uint16_t vendor_id = READ16(addr, vendor_id);
		if (PCI_NO_DEVICE_ID == vendor_id) continue;
		register_device(addr, vendor_id);
		// If it is a multifunction device, it may have other functions on
		// the same slot.
		uint8_t header_type = READ8(addr, header_type);
		if (0 == header_type & PCI_HEADER_TYPE_MULTIFUNCTION_FLAG) continue;
		// It's a multifunction device, so continue looking for functions we
		// can use on this slot.
		scan_slot(bus, addr.slot);
	}
}

void _pcibus_init()
{
	// We have at least one and possibly as many as 256 buses to scan. We
	// will start with bus 0, then add more buses as we discover bridge
	// devices offering them.
	bus_list[0] = 0;
	bus_count = 1;
	for (unsigned scan_idx = 0; scan_idx < bus_count; ++scan_idx) {
		scan_bus(bus_list[scan_idx]);
	}
}

