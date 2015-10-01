#include "pcibus.h"
#include <i386/cpu.h>
#include <sys/log.h>
#include <stdbool.h>
#include <stddef.h>

// IO port addresses for interacting with PCI configuration space.
static const uint32_t config_address = 0xCF8;
static const uint32_t config_data = 0xCFC;

// Compute the PCI configuration space address we must send in order to read
// or write some register.
static uint32_t pci_config_address(struct _pcibus_addr addr, uint8_t offset)
{
	uint32_t lbus  = (uint32_t)addr.bus;
	uint32_t lslot = (uint32_t)addr.slot;
	uint32_t lfunc = (uint32_t)addr.function;
	uint16_t tmp = 0;
	return (uint32_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
}

static uint8_t read8(struct _pcibus_addr addr, uint8_t offset)
{
	_outl(config_address, pci_config_address(addr, offset));
	return _inb(config_data);
}

static uint16_t read16(struct _pcibus_addr addr, uint8_t offset)
{
	_outl(config_address, pci_config_address(addr, offset));
	return (uint16_t)((_inl(config_data) >> ((offset & 2) * 8)) & 0xffff);
}

static uint8_t bus_list[256];
static unsigned bus_count;

static bool scan(struct _pcibus_addr addr, _pcibus_callback proc)
{
	// Read this slot's vendor ID to see if there is a device attached.
	struct _pcibus_id id;
	id.vendor_id = read16(addr, offsetof(struct _pci_header_common, vendor_id));
	if (0xFFFF == id.vendor_id) return false;
	id.device_id = read16(addr, offsetof(struct _pci_header_common, device_id));
	proc(addr, id);
	return true;
}

static void scan_slot(uint8_t bus, uint8_t slot, _pcibus_callback proc)
{
	struct _pcibus_addr addr = {bus, slot, 0};
	if (!scan(addr, proc)) return;
	// this device exists; check to see if it is a multifunction device.
	uint32_t offset = offsetof(struct _pci_header_common, header_type);
	uint8_t header_type = read8(addr, offset);
	if (0 == header_type & _PCI_HEADER_TYPE_MULTIFUNCTION_FLAG) return;
	// it is a multifunction device, so we must check its other functions.
	for (++addr.function; addr.function < 7; ++addr.function) {
		scan(addr, proc);
	}
}

void _pcibus_init(_pcibus_callback proc)
{
	// We have at least one and possibly as many as 256 buses to scan. We
	// will start with bus 0, then add more buses as we discover bridge
	// devices offering them.
	bus_list[0] = 0;
	bus_count = 1;
	for (unsigned scan_idx = 0; scan_idx < bus_count; ++scan_idx) {
		uint8_t bus = bus_list[scan_idx];
		// Scan each of the 64 possible devices on this bus to see which of
		// them actually exist. We can identify devices that actually exist
		// by the presence of a vendor ID which is not 0xFFFF.
		for (uint8_t slot = 0; slot < 32; ++slot) {
			scan_slot(bus, slot, proc);
		}
	}
}

