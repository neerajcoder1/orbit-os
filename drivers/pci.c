#include "pci.h"
#include "../kernel/io.h"
#include "terminal.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    
    // Create configuration address
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
              
    // Write out the address
    outl(PCI_CONFIG_ADDRESS, address);
    
    // Read in the data
    return inl(PCI_CONFIG_DATA);
}

// Need a simple hex printer for PCI devices
static void print_hex(uint32_t num, int digits) {
    char buf[16];
    char* digits_str = "0123456789ABCDEF";
    buf[digits] = '\0';
    for (int i = digits - 1; i >= 0; i--) {
        buf[i] = digits_str[num & 0x0F];
        num >>= 4;
    }
    terminal_writestring(buf);
}

void pci_enumerate(void) {
    terminal_writestring("Bus  Slot Func Vendor   Device   Class\n");
    terminal_writestring("------------------------------------------\n");
    
    for (uint32_t bus = 0; bus < 256; bus++) {
        for (uint32_t slot = 0; slot < 32; slot++) {
            for (uint32_t func = 0; func < 8; func++) {
                uint32_t vendor_device = pci_read_config(bus, slot, func, 0);
                uint16_t vendor_id = vendor_device & 0xFFFF;
                uint16_t device_id = vendor_device >> 16;
                
                if (vendor_id != 0xFFFF) {
                    uint32_t class_sub = pci_read_config(bus, slot, func, 0x08);
                    uint8_t class_id = class_sub >> 24;
                    uint8_t subclass_id = (class_sub >> 16) & 0xFF;
                    
                    print_hex(bus, 2); terminal_writestring("   ");
                    print_hex(slot, 2); terminal_writestring("   ");
                    print_hex(func, 1); terminal_writestring("    ");
                    
                    print_hex(vendor_id, 4); terminal_writestring("     ");
                    print_hex(device_id, 4); terminal_writestring("     ");
                    print_hex(class_id, 2); terminal_writestring(":");
                    print_hex(subclass_id, 2); terminal_writestring("\n");
                    
                    // If function 0 is not a multi-function device, skip the other functions
                    if (func == 0) {
                        uint32_t header_type = pci_read_config(bus, slot, func, 0x0C);
                        if (((header_type >> 16) & 0x80) == 0) {
                            break;
                        }
                    }
                }
            }
        }
    }
}

void pci_initialize(void) {
    // Basic PCI initialization
}

void pci_write_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    
    // Create configuration address
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
              
    // Write out the address
    outl(PCI_CONFIG_ADDRESS, address);
    
    // Write out the data
    outl(PCI_CONFIG_DATA, value);
}

int pci_find_device(uint16_t vendor_id, uint16_t device_id, uint8_t* bus_out, uint8_t* slot_out, uint8_t* func_out) {
    for (uint32_t bus = 0; bus < 256; bus++) {
        for (uint32_t slot = 0; slot < 32; slot++) {
            for (uint32_t func = 0; func < 8; func++) {
                uint32_t vendor_device = pci_read_config(bus, slot, func, 0);
                uint16_t v_id = vendor_device & 0xFFFF;
                uint16_t d_id = vendor_device >> 16;
                
                if (v_id == vendor_id && d_id == device_id) {
                    *bus_out = bus;
                    *slot_out = slot;
                    *func_out = func;
                    return 1;
                }
                
                // If function 0 is not a multi-function device, skip the other functions
                if (func == 0) {
                    uint32_t header_type = pci_read_config(bus, slot, func, 0x0C);
                    if (((header_type >> 16) & 0x80) == 0) {
                        break;
                    }
                }
            }
        }
    }
    return 0; // Not found
}

uint32_t pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, int bar_num) {
    uint8_t offset = 0x10 + (bar_num * 4);
    return pci_read_config(bus, slot, func, offset);
}

void pci_enable_bus_mastering(uint8_t bus, uint8_t slot, uint8_t func) {
    uint32_t command = pci_read_config(bus, slot, func, 0x04);
    command |= (1 << 2); // Set Bus Master bit
    pci_write_config(bus, slot, func, 0x04, command);
}
