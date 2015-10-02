#include <sys/pci.h>
#include <cpu.h>

// IO ports used to communicate with PCI configuration space.
static const uint32_t config_address = 0xCF8;
static const uint32_t config_data = 0xCFC;

// Compute the PCI configuration space address we must send in order to read
// or write some register.
static uint32_t register_address(struct pci_address addr, uint8_t offset)
{
	uint32_t lbus  = (uint32_t)addr.bus;
	uint32_t lslot = (uint32_t)addr.slot;
	uint32_t lfunc = (uint32_t)addr.function;
	uint16_t tmp = 0;
	return (uint32_t)((lbus << 16) | (lslot << 11) |
			(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
}

uint8_t pci_config_read8(struct pci_address addr, uint8_t offset)
{
	_outl(config_address, register_address(addr, offset));
	return _inb(config_data);
}

uint16_t pci_config_read16(struct pci_address addr, uint8_t offset)
{
	_outl(config_address, register_address(addr, offset));
	return (uint16_t)((_inl(config_data) >> ((offset & 2) * 8)) & 0xffff);
}

uint32_t pci_config_read32(struct pci_address addr, uint8_t offset)
{
	_outl(config_address, register_address(addr, offset));
	return _inl(config_data);
}
