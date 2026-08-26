#include "gdt.h"
#include "string.h"

extern void gdt_flush(uint32_t);
extern void tss_flush(void);

static struct gdt_entry gdt_entries[6];
static struct gdt_ptr   gdt_ptr;
static struct tss_entry tss;

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

static void tss_write(int num, uint16_t ss0, uint32_t esp0) {
    uint32_t base = (uint32_t)&tss;
    uint32_t limit = sizeof(tss);

    gdt_set_gate(num, base, limit, 0xE9, 0x00);
    memset(&tss, 0, sizeof(tss));

    tss.ss0 = ss0;
    tss.esp0 = esp0;
    tss.cs = 0x0B;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x13;
    tss.iomap_base = sizeof(tss);
}

void tss_set_stack(uint32_t kernel_ss, uint32_t kernel_esp) {
    tss.ss0 = kernel_ss;
    tss.esp0 = kernel_esp;
}

void gdt_initialize(void) {
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gdt_ptr.base  = (uint32_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                /* 0x00: Null segment */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); /* 0x08: Kernel Code segment */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); /* 0x10: Kernel Data segment */
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); /* 0x18: User Code segment */
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); /* 0x20: User Data segment */
    tss_write(5, 0x10, 0);                      /* 0x28: TSS */

    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
}
