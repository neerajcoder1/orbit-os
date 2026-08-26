#ifndef PCI_H
#define PCI_H

#include <stdint.h>

void pci_initialize(void);
void pci_enumerate(void);

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value);
int pci_find_device(uint16_t vendor_id, uint16_t device_id, uint8_t* bus, uint8_t* slot, uint8_t* func);
uint32_t pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, int bar_num);
void pci_enable_bus_mastering(uint8_t bus, uint8_t slot, uint8_t func);

#endif
