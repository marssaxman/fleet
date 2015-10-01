#include "pcibus.h"
#include <i386/cpu.h>
#include <sys/log.h>

// IO port addresses for interacting with PCI configuration space.
static const uint32_t config_address = 0xCF8;
static const uint32_t config_data = 0xCFC;

// Read a 16-bit value from PCI configuration space.
static uint16_t read16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t lbus  = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint16_t tmp = 0;
	uint32_t address = (uint32_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
	// write out the address so the device knows what we want to read
	_outl(config_address, address);
	return (uint16_t)((_inl(config_data) >> ((offset & 2) * 8)) & 0xffff);
}

void _pcibus_scan(_pcibus_scan_callback proc)
{
	// We have at least one and possibly as many as 256 buses to scan. We
	// will start with bus 0, then add more buses as we discover bridge
	// devices offering them.
	uint8_t bus_list[256] = {0};
	unsigned bus_count = 1;
	for (unsigned scan_idx = 0; scan_idx < bus_count; ++scan_idx) {
		uint8_t bus = bus_list[scan_idx];
		// Scan each of the 64 possible devices on this bus to see which of
		// them actually exist. We can identify devices that actually exist
		// by the presence of a vendor ID which is not 0xFFFF.
		for (uint8_t slot = 0; slot < 32; ++slot) {
			uint16_t vendorID = read16(bus, slot, 0, 0);
			if (0xFFFF == vendorID) continue;
			uint16_t deviceID = read16(bus, slot, 0, 2);
			proc(bus, slot, vendorID, deviceID);
		}
	}
}

