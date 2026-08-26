#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>
#include "../kernel/multiboot.h"

#define PMM_BLOCK_SIZE 4096

void pmm_initialize(struct multiboot_info* mbi);
void* pmm_alloc_block(void);
void pmm_free_block(void* addr);
uint32_t pmm_get_total_memory(void);
uint32_t pmm_get_free_memory(void);
uint32_t pmm_get_used_memory(void);

#endif
