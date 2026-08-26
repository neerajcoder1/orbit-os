#include "vmm.h"
#include "pmm.h"
#include "../kernel/string.h"

static uint32_t* page_directory = 0;

static void vmm_enable_paging(uint32_t pd_phys) {
    __asm__ volatile (
        "mov %0, %%eax\n"
        "mov %%eax, %%cr3\n"
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0\n"
        :: "r"(pd_phys) : "eax"
    );
}

void vmm_map_page(uint32_t phys, uint32_t virt, uint32_t flags) {
    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x03FF;

    if ((page_directory[pd_index] & I86_PDE_PRESENT) == 0) {
        // Allocate a new page table
        uint32_t* pt = (uint32_t*)pmm_alloc_block();
        if (!pt) return; // Out of memory
        memset(pt, 0, 4096);
        page_directory[pd_index] = ((uint32_t)pt) | flags | I86_PDE_PRESENT;
    }

    uint32_t* pt = (uint32_t*)(page_directory[pd_index] & ~0xFFF);
    pt[pt_index] = (phys & ~0xFFF) | flags | I86_PDE_PRESENT;
}

void vmm_initialize(void) {
    // Allocate page directory
    page_directory = (uint32_t*)pmm_alloc_block();
    if (!page_directory) return;
    
    memset(page_directory, 0, 4096);

    // Identity map the first 16MB (Maps kernel, VGA buffer, and multiboot modules)
    // We include I86_PDE_USER so our simple user-mode test can run directly in kernel memory space
    for (uint32_t i = 0; i < 4096; i++) {
        vmm_map_page(i * 4096, i * 4096, I86_PDE_WRITABLE | I86_PDE_USER);
    }

    // Enable paging
    vmm_enable_paging((uint32_t)page_directory);
}
