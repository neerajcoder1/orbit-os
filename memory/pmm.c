#include "pmm.h"
#include "../kernel/string.h"
#include "../drivers/terminal.h"

extern uint32_t kernel_end; // defined in linker script

static uint32_t* pmm_bitmap = 0;
static uint32_t pmm_max_blocks = 0;
static uint32_t pmm_used_blocks = 0;

static inline void bitmap_set(uint32_t bit) {
    pmm_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_clear(uint32_t bit) {
    pmm_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_test(uint32_t bit) {
    return pmm_bitmap[bit / 32] & (1 << (bit % 32));
}

void pmm_initialize(struct multiboot_info* mbi) {
    uint32_t mem_size_kb = mbi->mem_lower + mbi->mem_upper;
    uint32_t mem_size_bytes = mem_size_kb * 1024;
    pmm_max_blocks = mem_size_bytes / PMM_BLOCK_SIZE;
    pmm_used_blocks = pmm_max_blocks; // Mark all as used by default

    // Calculate where safe memory starts (after kernel)
    uint32_t safe_mem_start = (uint32_t)&kernel_end;
    
    // Also protect multiboot modules!
    if (mbi->flags & (1 << 3)) {
        if (mbi->mods_count > 0) {
            multiboot_module_t* mod = (multiboot_module_t*)mbi->mods_addr;
            uint32_t last_mod_end = mod[mbi->mods_count - 1].mod_end;
            if (last_mod_end > safe_mem_start) {
                safe_mem_start = last_mod_end;
            }
        }
    }

    // Place bitmap immediately after the safe memory start (kernel + modules)
    pmm_bitmap = (uint32_t*)safe_mem_start;
    
    // Calculate size of bitmap and initialize to 1 (all used)
    uint32_t bitmap_size = pmm_max_blocks / 8;
    memset(pmm_bitmap, 0xFF, bitmap_size);

    // Now update safe_mem_start to be after the bitmap
    safe_mem_start += bitmap_size;

    // Parse the multiboot mmap and free available regions
    if (mbi->flags & (1 << 6)) {
        struct multiboot_mmap_entry* mmap = (struct multiboot_mmap_entry*)mbi->mmap_addr;
        while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
            if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
                // Free this region in the bitmap
                uint32_t addr = mmap->addr_low;
                uint32_t len = mmap->len_low;
                
                for (uint32_t i = 0; i < len; i += PMM_BLOCK_SIZE) {
                    uint32_t block_addr = addr + i;
                    // Don't free memory that's part of the kernel, multiboot data, or lower memory (0-1MB)
                    if (block_addr >= safe_mem_start) {
                        bitmap_clear(block_addr / PMM_BLOCK_SIZE);
                        pmm_used_blocks--;
                    }
                }
            }
            mmap = (struct multiboot_mmap_entry*) ((uint32_t)mmap + mmap->size + sizeof(mmap->size));
        }
    }
}

void* pmm_alloc_block(void) {
    for (uint32_t i = 0; i < pmm_max_blocks; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            pmm_used_blocks++;
            return (void*)(i * PMM_BLOCK_SIZE);
        }
    }
    return 0; // Out of memory
}

void pmm_free_block(void* addr) {
    uint32_t block = (uint32_t)addr / PMM_BLOCK_SIZE;
    if (bitmap_test(block)) {
        bitmap_clear(block);
        pmm_used_blocks--;
    }
}

uint32_t pmm_get_total_memory(void) {
    return pmm_max_blocks * PMM_BLOCK_SIZE;
}

uint32_t pmm_get_free_memory(void) {
    return (pmm_max_blocks - pmm_used_blocks) * PMM_BLOCK_SIZE;
}

uint32_t pmm_get_used_memory(void) {
    return pmm_used_blocks * PMM_BLOCK_SIZE;
}
