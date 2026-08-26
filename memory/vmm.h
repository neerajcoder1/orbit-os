#ifndef VMM_H
#define VMM_H

#include <stdint.h>

#define I86_PDE_PRESENT 1
#define I86_PDE_WRITABLE 2
#define I86_PDE_USER 4

void vmm_initialize(void);
void vmm_map_page(uint32_t phys, uint32_t virt, uint32_t flags);

#endif
