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

static void scan_device(uint8_t bus, uint8_t slot, _pcibus_scan_callback proc)
{
	// If this device actually exists, its vendor ID will be something other
	// than 0xFFFF. 
	uint16_t vendorID = read16(bus, slot, 0, 0);
	if (vendorID != 0xFFFF) {
		uint16_t deviceID = read16(bus, slot, 0, 2);
		proc(bus, slot, vendorID, deviceID);
	}
}

static void scan_bus(uint8_t bus, _pcibus_scan_callback proc)
{
	for (uint8_t slot = 0; slot < 32; ++slot) {
		scan_device(bus, slot, proc);
	}
}

void _pcibus_scan(_pcibus_scan_callback proc)
{
	// Reset the device count, then start out by scanning bus 0, which
	// should be the chipset's main bridge.
	scan_bus(0, proc);
}
