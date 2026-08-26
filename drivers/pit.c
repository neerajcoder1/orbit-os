#include "pit.h"
#include "../kernel/io.h"
#include "../kernel/isr.h"
#include "terminal.h"
#include "../process/process.h"

static uint32_t tick = 0;

static void pit_callback(registers_t* regs) {
    (void)regs; /* Unused */
    tick++;
    if (tick % 50 == 0) {
        process_schedule();
    }
}

uint32_t pit_get_ticks(void) {
    return tick;
}

void pit_initialize(uint32_t frequency) {
    /* Register our timer callback. IRQ0 is interrupt 32 */
    register_interrupt_handler(32, pit_callback);

    /* The value we send to the PIT is the value to divide it's input clock
       (1193180 Hz) by, to get our required frequency. Important to note is
       that the divisor must be small enough to fit into 16-bits. */
    uint32_t divisor = 1193180 / frequency;

    /* Send the command byte. */
    outb(0x43, 0x36);

    /* Divisor has to be sent byte-wise, so split here into upper/lower bytes. */
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    /* Send the frequency divisor. */
    outb(0x40, l);
    outb(0x40, h);
    
    terminal_writestring("PIT initialized.\n");
}
