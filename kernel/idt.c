#include "idt.h"
#include <string.h>

extern void idt_flush(uint32_t);

static struct idt_entry idt_entries[256];
static struct idt_ptr   idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_low  = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].sel       = sel;
    idt_entries[num].always0   = 0;
    idt_entries[num].flags     = flags /* | 0x60 */; // Uncomment for user-mode
}

/* Memset is required since we don't have libc yet */
static void* idt_memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while(n--)
        *p++ = (unsigned char)c;
    return s;
}

void idt_initialize(void) {
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    idt_memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);

    idt_flush((uint32_t)&idt_ptr);
}
