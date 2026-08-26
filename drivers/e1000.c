#include "e1000.h"
#include "pci.h"
#include "../memory/vmm.h"
#include "terminal.h"

static uint32_t e1000_mmio_base = 0;
static uint8_t mac_addr[6];
static int e1000_found = 0;

#define E1000_VENDOR 0x8086
#define E1000_DEVICE 0x100E

static uint32_t e1000_read_reg(uint16_t reg) {
    return *((volatile uint32_t*)(e1000_mmio_base + reg));
}

void e1000_initialize(void) {
    uint8_t bus, slot, func;
    if (pci_find_device(E1000_VENDOR, E1000_DEVICE, &bus, &slot, &func)) {
        e1000_found = 1;
        
        pci_enable_bus_mastering(bus, slot, func);
        
        uint32_t bar0 = pci_get_bar(bus, slot, func, 0);
        uint32_t phys_addr = bar0 & ~0xF;
        
        // Map 1MB for MMIO (256 pages)
        for(uint32_t i = 0; i < 256; i++) {
            vmm_map_page(phys_addr + (i * 4096), phys_addr + (i * 4096), I86_PDE_WRITABLE);
        }
        
        e1000_mmio_base = phys_addr;
        
        // Read MAC Address from RAL (0x5400) and RAH (0x5404)
        uint32_t ral = e1000_read_reg(0x5400);
        uint32_t rah = e1000_read_reg(0x5404);
        
        mac_addr[0] = ral & 0xFF;
        mac_addr[1] = (ral >> 8) & 0xFF;
        mac_addr[2] = (ral >> 16) & 0xFF;
        mac_addr[3] = (ral >> 24) & 0xFF;
        mac_addr[4] = rah & 0xFF;
        mac_addr[5] = (rah >> 8) & 0xFF;
    }
}

void e1000_get_mac(uint8_t* mac) {
    for (int i = 0; i < 6; i++) {
        mac[i] = mac_addr[i];
    }
}

int e1000_is_found(void) {
    return e1000_found;
}
