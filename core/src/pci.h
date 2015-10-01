#ifndef _PCI_H
#define _PCI_H

#include <stdint.h>

// All PCI configuration spaces share these initial fields.
struct _pci_header_common
{
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t command;
	uint16_t status;
	uint8_t revision_id;
	uint8_t prog_if;
	uint8_t subclass;
	uint8_t class_code;
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;
} __attribute__((packed));

// The class code describes the device type.
enum _pci_class_codes {
	_PCI_CLASS_UNKNOWN = 0x00,
	_PCI_CLASS_MASS_STORAGE_CONTROLLER = 0x01,
	_PCI_CLASS_NETWORK_CONTROLLER = 0x02,
	_PCI_CLASS_DISPLAY_CONTROLLER = 0x03,
	_PCI_CLASS_MULTIMEDIA_CONTROLLER = 0x04,
	_PCI_CLASS_MEMORY_CONTROLLER = 0x05,
	_PCI_CLASS_BRIDGE_DEVICE = 0x06,
	_PCI_CLASS_SIMPLE_COMMUNICATION_CONTROLLER = 0x07,
	_PCI_CLASS_BASE_SYSTEM_PERIPHERAL = 0x08,
	_PCI_CLASS_INPUT_DEVICE = 0x09,
	_PCI_CLASS_DOCKING_STATION = 0x0A,
	_PCI_CLASS_PROCESSOR = 0x0B,
	_PCI_CLASS_SERIAL_BUS_CONTROLLER = 0x0C,
	_PCI_CLASS_WIRELESS_CONTROLLER = 0x0D,
	_PCI_CLASS_INTELLIGENT_IO_CONTROLLER = 0x0E,
	_PCI_CLASS_SATELLITE_COMMUNICATION_CONTROLLER = 0x0F,
	_PCI_CLASS_ENCRYPTION_CONTROLLER = 0x10,
	_PCI_CLASS_DAQ_DSP_CONTROLLER = 0x11,
	_PCI_CLASS_UNDEFINED = 0xFF
};

// The header type determines the layout of the remaining fields.
// It also contains a flag bit indicating "multifunction" status.
enum _pci_header_type {
	_PCI_HEADER_TYPE_MASK = 0x7F,
	_PCI_HEADER_TYPE_ENDPOINT = 0x00,
	_PCI_HEADER_TYPE_BRIDGE = 0x01,
	_PCI_HEADER_TYPE_CARDBUS = 0x02,
	_PCI_HEADER_TYPE_MULTIFUNCTION_FLAG = 0x80
};

// Most devices use an endpoint type structure.
struct _pci_config_endpoint {
	struct _pci_header_common header;
	uint32_t base_address[6];
	uint32_t cardbus_cis_pointer;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_id;
	uint32_t expansion_rom_base_address;
	uint32_t reserved[2];
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint8_t minimum_grant;
	uint8_t maximum_latency;
} __attribute__((packed));

// The root complex and other switches use the bridge structure.
struct _pci_config_bridge {
	struct _pci_header_common header;
	uint32_t base_address[2];
	uint8_t primary_bus_number;
	uint8_t secondary_bus_number;
	uint8_t subordinate_bus_number;
	uint8_t secondary_latency_timer;
	uint8_t io_base;
	uint8_t io_limit;
	uint16_t secondary_status;
	uint16_t memory_base;
	uint16_t memory_limit;
	uint16_t prefetchable_memory_base;
	uint16_t prefetchable_memory_limit;
	uint32_t prefetchable_base_upper32;
	uint32_t prefetchable_limit_upper32;
	uint16_t io_base_upper16;
	uint16_t io_limit_upper16;
	uint32_t reserved;
	uint32_t expansion_rom_base_address;
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint16_t bridge_control;
} __attribute((packed));

#endif //_SYS_PCI_H

